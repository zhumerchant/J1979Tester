#include "CVehicleDatabase.h"

std::unique_ptr<CVehicleDatabase> theVehicleDb = std::unique_ptr<CVehicleDatabase>(new CVehicleDatabase());
int g_nLanguageIndex = 0;
int g_nTemperatureIndex = 0;
int g_nPressureIndex = 0;
int g_nDistanceIndex = 0;
int g_nVacuumIndex = 0;
int g_nMassIndex = 0;

BOOL GetUserPreference(void)
{
	CString IFName;
	CString strAppName;

	GetCurrentDirectory(MAX_PATH, IFName.GetBuffer(MAX_PATH));
	IFName.ReleaseBuffer();
	IFName += _T("\\setting.ini");

	strAppName = _T("PREFER");
	g_nLanguageIndex = GetPrivateProfileInt(strAppName, _T("LanguageIndex"), 0, IFName);
	g_nTemperatureIndex = GetPrivateProfileInt(strAppName, _T("TemperatureIndex"), 0, IFName);
	g_nPressureIndex = GetPrivateProfileInt(strAppName, _T("PressureIndex"), 0, IFName);
	g_nDistanceIndex = GetPrivateProfileInt(strAppName, _T("DistanceIndex"), 0, IFName);
	g_nVacuumIndex = GetPrivateProfileInt(strAppName, _T("VacuumIndex"), 0, IFName);
	g_nMassIndex = GetPrivateProfileInt(strAppName, _T("MassIndex"), 0, IFName);

	return TRUE;
}

BOOL SetUserPreference(int nLanguageIndex, 
	int nTemperatureIndex, 
	int nPressureIndex, 
	int nDistanceIndex, 
	int nVacuumIndex,
	int nMassIndex)
{
	CString IFName;
	CString strAppName;
	CString strValue;

	GetCurrentDirectory(MAX_PATH, IFName.GetBuffer(MAX_PATH));
	IFName.ReleaseBuffer();
	IFName += _T("\\setting.ini");

	strAppName = _T("PREFER");
	if (nLanguageIndex != g_nLanguageIndex)
	{
		strValue.Format(_T("%d"), nLanguageIndex);
		if (!WritePrivateProfileString(strAppName, _T("LanguageIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nLanguageIndex = nLanguageIndex;
	}
	if (nTemperatureIndex != g_nTemperatureIndex)
	{
		strValue.Format(_T("%d"), nTemperatureIndex);
		if (!WritePrivateProfileString(strAppName, _T("TemperatureIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nTemperatureIndex = nTemperatureIndex;
	}
	if (nPressureIndex != g_nPressureIndex)
	{
		strValue.Format(_T("%d"), nPressureIndex);
		if (!WritePrivateProfileString(strAppName, _T("PressureIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nPressureIndex = nPressureIndex;
	}
	if (nDistanceIndex != g_nDistanceIndex)
	{
		strValue.Format(_T("%d"), nDistanceIndex);
		if (!WritePrivateProfileString(strAppName, _T("DistanceIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nDistanceIndex = nDistanceIndex;
	}
	if (nVacuumIndex != g_nVacuumIndex)
	{
		strValue.Format(_T("%d"), nVacuumIndex);
		if (!WritePrivateProfileString(strAppName, _T("VacuumIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nVacuumIndex = nVacuumIndex;
	}
	if (nMassIndex != g_nMassIndex)
	{
		strValue.Format(_T("%d"), nMassIndex);
		if (!WritePrivateProfileString(strAppName, _T("MassIndex"), strValue, IFName))
		{
			return FALSE;
		}
		g_nMassIndex = nMassIndex;
	}

	return TRUE;
}

bool CVehicleDatabase::Open(const char* lang)
{
	if (3 != strlen(lang))
	{
		return false;
	}
	for (int i = 0; i < (int)strlen(lang); i++)
	{
		m_tszLang[i] = lang[i];
	}
	m_tszLang[3] = 0;

	if (!m_xmlDocDTC.LoadFromFile(_T("obd2_dtc.xml")))
	{
		return false;
	}
	if (!m_xmlDocDTC.IsValid())
	{
		m_xmlDocDTC.Release();
		return false;
	}

	//load parameters
	if (!m_xmlDocParam.LoadFromFile(_T("obd2_param.xml")))
	{
		m_xmlDocDTC.Release();
		return false;
	}
	if (!m_xmlDocParam.IsValid())
	{
		m_xmlDocParam.Release();
		m_xmlDocDTC.Release();
		return false;
	}

	if (!ParseParameters())
	{
		m_xmlDocParam.Release();
		m_xmlDocDTC.Release();
		return false;
	}

	//load messages
	if (!m_xmlDocMessage.LoadFromFile(_T("messages.xml")))
	{
		m_xmlDocParam.Release();
		m_xmlDocDTC.Release();
		return false;
	}
	if (!m_xmlDocMessage.IsValid())
	{
		m_xmlDocMessage.Release();
		m_xmlDocParam.Release();
		m_xmlDocDTC.Release();
		return false;
	}

	if (!LoadMessages())
	{
		m_xmlDocMessage.Release();
		m_xmlDocParam.Release();
		m_xmlDocDTC.Release();
		return false;
	}

	return true;
}

bool CVehicleDatabase::ParseParamConverterState(CONVERTER_STATE_REC& cvs)
{
	int i = 0;
	TCHAR* stopstring;
	CDuiString name = cvs.name;

	if (0 == name.GetLength())
	{
		return false;
	}
	for (i = 0; i < name.GetLength(); i++)
	{
		if ((!_istalnum(name.GetAt(i))) &&
			(_T('_') != name.GetAt(i)) &&
			(_T('-') != name.GetAt(i)) &&
			(_T('.') != name.GetAt(i))
			)
		{
			name.SetAt(i, _T('_'));
		}
	}
	CMarkupNode elemLv2 = m_xmlDocParam.GetRoot().GetChild(_T("CONVERTER_STATE_REC_CON")).GetChild(name);
	if (!elemLv2.IsValid())
	{
		return false;
	}
	cvs.stateText = GetParamMnemonic(elemLv2.GetAttributeValue(_T("stateText")));
	cvs.stateLow = _tcstod(elemLv2.GetAttributeValue(_T("stateLow")), &stopstring);
	cvs.stateHigh = _tcstod(elemLv2.GetAttributeValue(_T("stateHigh")), &stopstring);
	return true;
}

bool CVehicleDatabase::ParseParamConverter(DD_PARAM_REC& dd_param_rec, CDuiString name)
{
	int i = 0;
	TCHAR* stopstring;
	if (0 == name.GetLength())
	{
		return false;
	}
	if (isdigit(name.GetAt(0)))
	{
		name = _T('C') + name;
		name = _T('V') + name;
		name = _T('_') + name;
	}
	for (i = 0; i < name.GetLength(); i++)
	{
		if ((!_istalnum(name.GetAt(i))) &&
			(_T('_') != name.GetAt(i)) &&
			(_T('-') != name.GetAt(i)) &&
			(_T('.') != name.GetAt(i))
			)
		{
			name.SetAt(i, _T('_'));
		}
	}
	CMarkupNode elemLv2 = m_xmlDocParam.GetRoot().GetChild(_T("CONVERTER_REC_CON")).GetChild(name);

	if (!elemLv2.IsValid())
	{
		return false;
	}

	dd_param_rec.paramRd.converter.offset = _tcstod(elemLv2.GetAttributeValue(_T("offset")), &stopstring);
	dd_param_rec.paramRd.converter.multiplier = _tcstod(elemLv2.GetAttributeValue(_T("multiplier")), &stopstring);
	dd_param_rec.paramRd.converter.divisor = _tcstod(elemLv2.GetAttributeValue(_T("divisor")), &stopstring);
	dd_param_rec.paramRd.converter.oldType = _tcstol(elemLv2.GetAttributeValue(_T("oldType")), &stopstring, 10);
	dd_param_rec.paramRd.converter.quantityType = _tcstol(elemLv2.GetAttributeValue(_T("quantityType")), &stopstring, 10);

	dd_param_rec.paramRd.converter.states.clear();
	dd_param_rec.paramRd.converter.maps.clear();

	for (CMarkupNode elemLv3 = elemLv2.GetChild(); elemLv3.IsValid(); elemLv3 = elemLv3.GetSibling())
	{
		CONVERTER_STATE_REC cvs;
		CONVERTER_MAP_REC cvm;

		if (0 == _tcscmp(elemLv3.GetName(), _T("CONVERTER_STATE_REC")))
		{
			cvs.name = elemLv3.GetAttributeValue(_T("name"));
			if (!ParseParamConverterState(cvs))
			{
				return false;
			}
			dd_param_rec.paramRd.converter.states.push_back(cvs);
		}
		else if (0 == _tcscmp(elemLv3.GetName(), _T("CONVERTER_MAP_REC")))
		{
			cvm.fromQuantity = _tcstod(elemLv3.GetAttributeValue(_T("fromQuantity")), &stopstring);
			cvm.toQuantity = _tcstod(elemLv3.GetAttributeValue(_T("toQuantity")), &stopstring);
			dd_param_rec.paramRd.converter.maps.push_back(cvm);
		}
	}

	return true;
}

CDuiString CVehicleDatabase::GetParamMnemonic(CDuiString name)
{
	int i = 0;
	if (0 == name.GetLength())
	{
		return _T("");
	}
	if (isdigit(name.GetAt(0)))
	{
		name = _T('_') + name;
	}
	for (i = 0; i < name.GetLength(); i++)
	{
		if ( (!isalpha(name.GetAt(i))) &&
			(!isdigit(name.GetAt(i))) &&
			(_T('_') != name.GetAt(i)) &&
			(_T('-') != name.GetAt(i))&&
			(_T('.') != name.GetAt(i))
			)
		{
			name.SetAt(i, _T('_'));
		}
	}

	CMarkupNode elemLv3 = m_xmlDocParam.GetRoot().GetChild(_T("MNEMONICS")).GetChild(m_tszLang).GetChild(name);
	return elemLv3.GetValue();
}

bool CVehicleDatabase::LoadMessages(void)
{
	CDuiString strName;
	int nStrId = 0;

	CMarkupNode elemRoot = m_xmlDocMessage.GetRoot();
	if (!elemRoot.IsValid())
	{
		return false;
	}
	if (_tcscmp(_T("Messages"), elemRoot.GetName()))
	{
		return false;
	}

	CMarkupNode elemLv1 = m_xmlDocMessage.GetRoot().GetChild(m_tszLang);
	if (!elemLv1.IsValid())
	{
		return false;
	}

	for (CMarkupNode elemLv2 = elemLv1.GetChild(); elemLv2.IsValid(); elemLv2 = elemLv2.GetSibling())
	{
		strName = elemLv2.GetName();
		if ((strName.GetLength() > 4) && (0 == strName.Mid(0, 4).Compare(_T("IDS_"))))
		{
			_stscanf_s(strName.Mid(4), _T("%d"), &nStrId);
			CPaintManagerUI::AddMultiLanguageString(nStrId, elemLv2.GetValue());
		}
	}
	return true;
}

bool CVehicleDatabase::ParseParameters(void)
{
	CDuiString strName;
	DD_PARAM_REC dd_param_rec;
	TCHAR* stopstring;

	CMarkupNode elemRoot = m_xmlDocParam.GetRoot();
	if (!elemRoot.IsValid())
	{
		return false;
	}
	if (_tcscmp(_T("OBD2"), elemRoot.GetName()))
	{
		return false;
	}

	CMarkupNode elemLv1 = elemRoot.GetChild(_T("DD_PARAM_REC_CON"));
	if (!elemLv1.IsValid())
	{
		return false;
	}

	strName = _T("0");
	for (CMarkupNode elemLv2 = elemLv1.GetChild(); elemLv2.IsValid(); elemLv2 = elemLv2.GetSibling())
	{
		if (0 != strName.Compare(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetChild(_T("PARAM_ADDR_REC")).GetAttributeValue(_T("address"))))
		{
			dd_param_rec.databasename = elemLv2.GetName();
			dd_param_rec.quantityType = _tcstol(elemLv2.GetAttributeValue(_T("quantityType")), &stopstring, 10);
			dd_param_rec.displayMode = _tcstol(elemLv2.GetAttributeValue(_T("displayMode")), &stopstring, 10);
			dd_param_rec.name = GetParamMnemonic(elemLv2.GetAttributeValue(_T("name")));
			dd_param_rec.mnemonic = GetParamMnemonic(elemLv2.GetAttributeValue(_T("mnemonic")));
			
			dd_param_rec.paramRd.shift = _tcstol(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetAttributeValue(_T("shift")), &stopstring, 10);
			dd_param_rec.paramRd.mask = _tcstol(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetAttributeValue(_T("mask")), &stopstring, 10);
			dd_param_rec.paramRd.LeftShift = _tcscmp(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetAttributeValue(_T("LeftShift")), _T("false")) ? true : false;
			dd_param_rec.paramRd.address.address = _tcstol(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetChild(_T("PARAM_ADDR_REC")).GetAttributeValue(_T("address")), &stopstring, 10);
			dd_param_rec.paramRd.address.size = _tcstol(elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetChild(_T("PARAM_ADDR_REC")).GetAttributeValue(_T("size")), &stopstring, 10);
			if (!ParseParamConverter(dd_param_rec, elemLv2.GetChild(_T("PARAM_TYPE_REC")).GetChild(_T("CONVERTER_REC")).GetAttributeValue(_T("name"))))
			{
				return false;
			}
			m_mapNameToParam.insert(std::make_pair(elemLv2.GetName(), dd_param_rec));
		}
	}

	return true;
}

CVehicleDatabase::~CVehicleDatabase()
{
	Close();
}

bool CVehicleDatabase::Close(void)
{
	m_mapNameToParam.clear();

	if (!m_xmlDocDTC.IsValid())
	{
		return false;
	}
	m_xmlDocDTC.Release();

	if (!m_xmlDocParam.IsValid())
	{
		return false;
	}
	m_xmlDocParam.Release();

	CPaintManagerUI::RemoveAllMultiLanguageString();

	if (!m_xmlDocMessage.IsValid())
	{
		return false;
	}
	m_xmlDocMessage.Release();

	return true;
}

inline void GetQualityText(CDuiString& strText, double dblValue, int quantityType)
{
	switch (quantityType)
	{
	case 0:
		strText.Format(_T("%u"), dblValue);
		break;
	case 1: //voltage
		strText.Format(_T("%.2f"), dblValue);
		break;
	case 2: //resistance
		strText.Format(_T("%u"), dblValue);
		break;
	case 3: //current
		strText.Format(_T("%.3f"), dblValue);
		break;
	case 5: //temperature
		if (0 == g_nTemperatureIndex)
		{
			strText.Format(_T("%.0f"), dblValue);
		}
		else
		{
			dblValue = dblValue * 9.0 / 5.0 + 32.0;
			strText.Format(_T("%.0f"), dblValue);
		}
		break;
	case 6: //frequency
		strText.Format(_T("%u"), dblValue);
		break;
	case 7: //milli-second
		strText.Format(_T("%u"), dblValue);
		break;
	case 8: //percent
		strText.Format(_T("%.1f"), dblValue);
		break;
	case 10: //RPM
		strText.Format(_T("%u"), dblValue);
		break;
	case 11: //speed
		if (0 == g_nDistanceIndex)
		{
			strText.Format(_T("%u"), dblValue);
		}
		else
		{
			dblValue *= 0.62137;
			strText.Format(_T("%u"), dblValue);
		}
		break;
	case 12: //distance
		strText.Format(_T("%u"), dblValue);
		break;
	case 13: //mass
		strText.Format(_T("%u"), dblValue);
		break;
	case 14: //angle
		strText.Format(_T("%.1f"), dblValue);
		break;
	case 15: //acceleration
		strText.Format(_T("%.2f"), dblValue);
		break;
	case 16: //mass flow
		strText.Format(_T("%.3f"), dblValue);
		break;
	case 17: //ratio
		strText.Format(_T("%.3f"), dblValue);
		break;
	case 18: //second
		strText.Format(_T("%u"), dblValue);
		break;
	case 19: //power density
		strText.Format(_T("%.2f"), dblValue);
		break;
	case 20: //density
		strText.Format(_T("%.3f"), dblValue);
		break;
	case 21: //volume
		strText.Format(_T("%u"), dblValue);
		break;
	case 22: //area
		strText.Format(_T("%.2f"), dblValue);
		break;
	case 23: //distance
		strText.Format(_T("%u"), dblValue);
		break;
	case 24: //distance
		strText.Format(_T("%u"), dblValue);
		break;
	case 25: //economy
		strText.Format(_T("%.3f"), dblValue);
		break;
	case 29: //pressure
		if (0 == g_nPressureIndex)
		{
			strText.Format(_T("%.0f"), dblValue);
		}
		else if (1 == g_nPressureIndex)
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		else if (2 == g_nPressureIndex)
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		else
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		break;
	case 30: //yaw rate
		strText.Format(_T("%u"), dblValue);
		break;
	case 32: //fuel rate
		strText.Format(_T("%.1f"), dblValue);
		break;
	case 33: //vacuum
		if (0 == g_nVacuumIndex)
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		else if (1 == g_nVacuumIndex)
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		else if (2 == g_nVacuumIndex)
		{
			strText.Format(_T("%.1f"), dblValue);
		}
		else
		{
			strText.Format(_T("%.3f"), dblValue);
		}
		break;
	case 34: //MAF
		strText.Format(_T("%.2f"), dblValue);
		break;
	case 42: //torque
		strText.Format(_T("%.1f"), dblValue);
		break;
	case 43: //power
		strText.Format(_T("%.1f"), dblValue);
		break;
	case 46: //concentration
		strText.Format(_T("%d"), dblValue);
		break;
	case 48: //current
		strText.Format(_T("%d"), dblValue);
		break;
	default:
		strText.Format(_T("%d"), dblValue);
		break;
	}
}

void CVehicleDatabase::CalcDataValues(std::vector<std::vector<DD_PARAM_REC>>& vectDataList,
	std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values,
	std::vector<CDuiString>& texts)
{
	int nEcuIndex = 0;
	CDuiString strText;
	unsigned long dwPidValue = 0;
	unsigned long dwMaxValue = 0;
	unsigned long dwMask = 0;
	int bits = 0;
	int i = 0;
	double dblValue = 0.0;
	double lastFromValue = 0.0;
	double lastToValue = 0.0;

	std::map<unsigned long, std::vector<unsigned char>>::iterator itM;

	texts.clear();

	for (auto & data_list : vectDataList)
	{
		for (auto & param : data_list)
		{
			strText = _T("");
			itM = values[nEcuIndex].find(param.paramRd.address.address);
			if (itM != values[nEcuIndex].end())
			{
				bits = 0;
				dwPidValue = 0;
				dwMaxValue = 0;
				dwMask = param.paramRd.mask;
				for (i = (int)param.paramRd.shift; i < (int)param.paramRd.address.size * 8; i++, bits++)
				{
					dwPidValue <<= 1;
					if ((itM->second.at(param.paramRd.address.size - 1 - (i >> 3)) >> (i & 7)) & 1 & dwMask)
					{
						dwPidValue |= 1;
					}
					if (1 & dwMask)
					{
						dwMaxValue |= 1;
					}		
					dwMask >>= 1;
					if (0 == dwMask)
					{
						bits++;
						break;
					}
				}
				dwMask = 0;
				for (i = 0; i < bits; i++)
				{
					dwMask <<= 1;
					if ((dwPidValue >> i) & 1)
					{
						dwMask |= 1;
					}
				}
				dwPidValue = dwMask;

				dwMask = 0;
				for (i = 0; i < bits; i++)
				{
					dwMask <<= 1;
					if ((dwMaxValue >> i) & 1)
					{
						dwMask |= 1;
					}
				}
				dwMaxValue = dwMask;

				param.minValue = (param.paramRd.converter.offset) * param.paramRd.converter.multiplier / param.paramRd.converter.divisor;
				param.maxValue = ((double)dwMaxValue + param.paramRd.converter.offset) * param.paramRd.converter.multiplier / param.paramRd.converter.divisor;

				dblValue = ((double)dwPidValue + param.paramRd.converter.offset) * param.paramRd.converter.multiplier / param.paramRd.converter.divisor;
				if ((0 == param.paramRd.converter.states.size()) && (0 == param.paramRd.converter.maps.size()))
				{
					GetQualityText(strText, dblValue, param.quantityType);
					param.curValue = dblValue;
				}
				else if (0 < param.paramRd.converter.states.size())
				{
					for (auto & state : param.paramRd.converter.states)
					{
						if ((dblValue >= state.stateLow) && (dblValue <= state.stateHigh))
						{
							strText = state.stateText;
							break;
						}
					}
					param.curValue = dblValue;
				}
				else if (0 < param.paramRd.converter.maps.size())
				{
					for (auto & mapV : param.paramRd.converter.maps)
					{
						if (dblValue == mapV.fromQuantity)
						{
							dblValue = mapV.toQuantity;
							break;
						}
						if (dblValue < mapV.fromQuantity)
						{
							dblValue = (dblValue - lastFromValue) * (mapV.toQuantity - lastToValue) / (mapV.fromQuantity - lastFromValue) + lastToValue;
							break;
						}
						lastFromValue = mapV.fromQuantity;
						lastToValue = mapV.toQuantity;
					}
					GetQualityText(strText, dblValue, param.quantityType);
					param.curValue = dblValue;

					for (auto& mapV : param.paramRd.converter.maps)
					{
						if (param.minValue == mapV.fromQuantity)
						{
							param.minValue = mapV.toQuantity;
							break;
						}
						if (param.minValue < mapV.fromQuantity)
						{
							param.minValue = (param.minValue - lastFromValue) * (mapV.toQuantity - lastToValue) / (mapV.fromQuantity - lastFromValue) + lastToValue;
							break;
						}
						lastFromValue = mapV.fromQuantity;
						lastToValue = mapV.toQuantity;
					}

					for (auto& mapV : param.paramRd.converter.maps)
					{
						if (param.maxValue == mapV.fromQuantity)
						{
							param.maxValue = mapV.toQuantity;
							break;
						}
						if (param.maxValue < mapV.fromQuantity)
						{
							param.maxValue = (param.maxValue - lastFromValue) * (mapV.toQuantity - lastToValue) / (mapV.fromQuantity - lastFromValue) + lastToValue;
							break;
						}
						lastFromValue = mapV.fromQuantity;
						lastToValue = mapV.toQuantity;
					}
				}
			}
			texts.push_back(strText);
		}
		nEcuIndex++;
	}
}

void CVehicleDatabase::GetSupportParameters(const std::map<unsigned long, std::vector<unsigned char>> & mapDataValue, std::vector<DD_PARAM_REC>& data_list)
{
	data_list.clear();

	for (auto & PidValue : mapDataValue)
	{
		for (auto & Param : m_mapNameToParam)
		{
			if (Param.second.paramRd.address.address == PidValue.first)
			{
				switch (Param.second.quantityType)
				{
				case 0:
					Param.second.unit = _T("");
					break;
				case 1: //voltage
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(55);
					break;
				case 2: //resistance
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(56);
					break;
				case 3: //current
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(57);
					break;
				case 5: //temperature
					if (0 == g_nTemperatureIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(58);
					}
					else
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(87);
					}
					break;
				case 6: //frequency
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(59);
					break;
				case 7: //milli-second
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(60);
					break;
				case 8: //percent
					Param.second.unit = _T("%");
					break;
				case 10: //RPM
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(61);
					break;
				case 11: //speed
					if (0 == g_nDistanceIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(62);
					}
					else
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(63);
					}
					break;
				case 12: //distance
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(64);
					break;
				case 13: //mass
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(65);
					break;
				case 14: //angle
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(66);
					break;
				case 15: //acceleration
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(67);
					break;
				case 16: //mass flow
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(68);
					break;
				case 17: //ratio
					Param.second.unit = _T("lambda");
					break;
				case 18: //second
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(69);
					break;
				case 19: //power density
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(70);
					break;
				case 20: //density
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(71);
					break;
				case 21: //volume
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(72);
					break;
				case 22: //area
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(73);
					break;
				case 23: //distance
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(74);
					break;
				case 24: //distance
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(75);
					break;
				case 25: //economy
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(76);
					break;
				case 29: //pressure
					if (0 == g_nPressureIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(77);
					}
					else if (1 == g_nPressureIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(78);
					}
					else if (2 == g_nPressureIndex)
					{
						Param.second.unit = _T("psi");
					} 
					else 
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(79);
					}
					break;
				case 30: //yaw rate
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(80);
					break;
				case 32: //fuel rate
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(81);
					break;
				case 33: //vacuum
					if (0 == g_nVacuumIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(77);
					}
					else if (1 == g_nVacuumIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(82);
					}
					else if (2 == g_nVacuumIndex)
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(78);
					}
					else
					{
						Param.second.unit = CPaintManagerUI::GetMultiLanguageString(79);
					}
					break;
				case 34: //MAF
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(83);
					break;
				case 42: //torque
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(84);
					break;
				case 43: //power
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(85);
					break;
				case 46: //concentration
					Param.second.unit = _T("ppm");
					break;
				case 48: //current
					Param.second.unit = CPaintManagerUI::GetMultiLanguageString(86);
					break;
				default:
					Param.second.unit = _T("");
					break;
				}
				data_list.push_back(Param.second);
			}
		}
	}

	//trim

	//get ignition type
	bool bCompressiionIgnition = false;
	auto DV = mapDataValue.find(1); //PID:$01
	if (DV != mapDataValue.end())
	{
		if ((DV->second.size() >= 4) && (DV->second.at(1) & 8))
		{
			bCompressiionIgnition = true;
		}
	}
	if (bCompressiionIgnition)
	{
		auto param = data_list.begin();
		while (param != data_list.end())
		{
			if ((0 == param->databasename.Compare(_T("MODE1_CAT_SUP"))) || //C.0
				(0 == param->databasename.Compare(_T("MODE1_EVAP_SUP"))) || //C.2
				(0 == param->databasename.Compare(_T("MODE1_AIR_SUP"))) || //C.3
				(0 == param->databasename.Compare(_T("MODE1_O2S_SUP"))) || //C.5
				(0 == param->databasename.Compare(_T("MODE1_HTR_SUP"))) || //C.6
				(0 == param->databasename.Compare(_T("MODE1_HCAT_SUP"))) || //C.1
				(0 == param->databasename.Compare(_T("MODE1_CAT_EVAL"))) || //D.0
				(0 == param->databasename.Compare(_T("MODE1_HCAT_EVAL"))) || //D.1
				(0 == param->databasename.Compare(_T("MODE1_EVAP_EVAL"))) || //D.2
				(0 == param->databasename.Compare(_T("MODE1_AIR_EVAL"))) || //D.3
				(0 == param->databasename.Compare(_T("MODE1_O2S_EVAL"))) || //D.5
				(0 == param->databasename.Compare(_T("MODE1_HTR_EVAL")))) //D.6
			{
				param = data_list.erase(param);
			}
			else
			{
				param++;
			}
		}
	}
	else //spark ignition
	{
		auto param = data_list.begin();
		while (param != data_list.end())
		{
			if ((0 == param->databasename.Compare(_T("EXHGAS_MON_MODE1"))) || //D.5
				(0 == param->databasename.Compare(_T("PM_FILT_MODE1"))) || //D.6
				(0 == param->databasename.Compare(_T("BOOSTP_MON_MODE1"))) || //D.3
				(0 == param->databasename.Compare(_T("NOx_AFT_TREAT_MODE1"))) || //D.1
				(0 == param->databasename.Compare(_T("NMHC_CATMON_MODE1"))) || //D.0
				(0 == param->databasename.Compare(_T("PM_FILTSUP_MODE1"))) || //C.6
				(0 == param->databasename.Compare(_T("EXHGASSUP_MODE1"))) ||  //C.5
				(0 == param->databasename.Compare(_T("BOOSTPSUP_MODE1"))) || //C.3
				(0 == param->databasename.Compare(_T("NOxSUP_MODE1"))) || //C.1
				(0 == param->databasename.Compare(_T("NMHCSUP_MODE1")))) //C.0
			{
				param = data_list.erase(param);
			}
			else
			{
				param++;
			}
		}
	}
}

bool CVehicleDatabase::GetDTC(unsigned long code, DTC_REC& dtc)
{
	bool bFound = false;
	CDuiString strName;

	switch ((code >> 14) & 3)
	{
	case 0:
		dtc.number.Format(_T("P%04X"), code & 0x3FFF);
		break;
	case 1:
		dtc.number.Format(_T("C%04X"), code & 0x3FFF);
		break;
	case 2:
		dtc.number.Format(_T("B%04X"), code & 0x3FFF);
		break;
	case 3:
		dtc.number.Format(_T("U%04X"), code & 0x3FFF);
		break;
	default:
		return false;
	}

	CMarkupNode elemRoot = m_xmlDocDTC.GetRoot();
	if (!elemRoot.IsValid())
	{
		return false;
	}
	if (_tcscmp(_T("OBD2"), elemRoot.GetName()))
	{
		return false;
	}
	CMarkupNode elemLv1 = elemRoot.GetChild(_T("DTC_REC_CON"));
	if (!elemLv1.IsValid())
	{
		return false;
	}
	strName.Format(_T("%u"), code);
	bFound = false;
	for (CMarkupNode elemLv2 = elemLv1.GetChild(_T("DTC_REC")); elemLv2.IsValid(); elemLv2 = elemLv2.GetSibling())
	{
		if (0 == strName.Compare(elemLv2.GetAttributeValue(_T("name"))))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		dtc.naming = _T("");
		return true;
	}
	elemLv1 = elemRoot.GetChild(_T("MNEMONICS"));
	if (!elemLv1.IsValid())
	{
		return false;
	}

	bFound = false; 
	for (CMarkupNode elemLv3 = elemLv1.GetChild(m_tszLang).GetChild(_T("MNEMONIC")); elemLv3.IsValid(); elemLv3 = elemLv3.GetSibling())
	{
		if (0 == strName.Compare(elemLv3.GetAttributeValue(_T("name"))))
		{
			dtc.naming = elemLv3.GetAttributeValue(_T("message"));
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		dtc.naming = _T("");
		return true;
	}

	if (dtc.naming.IsEmpty())
	{
		dtc.naming = _T("ISO/SAE Reserved");
	}
	return true;
}
