#include "CVehicleComm.h"
#include  <time.h>
#include  <algorithm>

std::unique_ptr<CVehicleComm> theVehicleComm = std::unique_ptr<CVehicleComm>(new CVehicleComm());

const std::vector<unsigned long>& CVehicleComm::GetEcuID() const
{
	return m_vectEcuID;
}

bool CVehicleComm::IsChannelConnected(void)
{
	return m_ptrJ2534Chan.get() != NULL;
}

bool CVehicleComm::GetCurrentDataValue(int nEcuIndex, std::map<unsigned long, std::vector<unsigned char>>& values) const
{
	if ((nEcuIndex < 0) || (nEcuIndex >= (int)m_vectCurrentDataValue.size()))
	{
		return false;
	}
	values = m_vectCurrentDataValue[nEcuIndex];
	return true;
}

bool CVehicleComm::GetConfirmedDtc(int nEcuIndex, std::vector<unsigned long>& dtcs) const
{
	if ((nEcuIndex < 0) || (nEcuIndex >= (int)m_vectEcuConfirmedDtc.size()))
	{
		return false;
	}
	dtcs = m_vectEcuConfirmedDtc[nEcuIndex];
	return true;
}

bool CVehicleComm::GetPendingDtc(int nEcuIndex, std::vector<unsigned long>& dtcs) const
{
	if ((nEcuIndex < 0) || (nEcuIndex >= (int)m_vectEcuPendingDtc.size()))
	{
		return false;
	}
	dtcs = m_vectEcuPendingDtc[nEcuIndex];
	return true;
}

bool CVehicleComm::GetPermanentDtc(int nEcuIndex, std::vector<unsigned long>& dtcs) const
{
	if ((nEcuIndex < 0) || (nEcuIndex >= (int)m_vectEcuPermanentDtc.size()))
	{
		return false;
	}
	dtcs = m_vectEcuPermanentDtc[nEcuIndex];
	return true;
}

unsigned long CVehicleComm::GetECUIDLength(void) const
{
	switch (m_dwProtocolId)
	{
	case J1850PWM:
		return 1;
	case J1850VPW:
		return 1;
	case ISO9141:
		return 1;
	case ISO14230:
		return 1;
	case ISO15765:
		if (m_bIsExtCANID)
		{
			return 4;
		}
		else
		{
			return 2;
		}
	default:
		return 1;
	}
}

bool CVehicleComm::GetBatteryVoltage(unsigned long& dwVoltage_mV)
{
	if (m_ptrJ2534Dev.get())
	{
		long lRetCode = STATUS_NOERROR;
		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Dev->ioctl(READ_VBATT, NULL, &dwVoltage_mV);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}

		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}
		return true;
	}
	return false;
}

const CDuiString CVehicleComm::GetProtocolName(void) const
{
	switch (m_dwProtocolId)
	{
	case J1850PWM:
		return _T("J1850PWM");
	case J1850VPW:
		return _T("J1850VPW");
	case ISO9141:
		return _T("ISO9141");
	case ISO14230:
		return _T("ISO14230");
	case ISO15765:
		if (m_bIsExtCANID)
		{
			return _T("ISO15765");
		}
		else
		{
			return _T("ISO15765");
		}
	default:
		return _T("Unknown");
	}
}

long CVehicleComm::ISO15765_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}


	while (1)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 150);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 4) && (0 == (msg.RxStatus & 0x0F)))
		{
			rxMsgAry.push_back(msg);
		}
	}
	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

inline long time_elapse(clock_t start)
{
	return (((long)clock() - (long)start) * 1000 / CLOCKS_PER_SEC);
}

long CVehicleComm::ISO15765_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;
	unsigned long dwMsgRxed = 0;
	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	clock_t ts = clock();
	while (time_elapse(ts) < 150)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 150);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 4) && (0 == (msg.RxStatus & 0x0F)))
		{
			ts = clock(); //restart timer
			rxMsgAry.push_back(msg);
			dwMsgRxed++;
			if (dwNumberOfMsgToRx == dwMsgRxed)
			{
				break;
			}
		}
	}
	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	if (dwNumberOfMsgToRx > dwMsgRxed)
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::CAN_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	while (1)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 300);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 4) && (0 == (msg.RxStatus & 0x0F)))
		{
			rxMsgAry.push_back(msg);
		}
	}
	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::J1850_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 200);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	while (1)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 150);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			rxMsgAry.push_back(msg);
		}
	}
	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::J1850_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	unsigned long dwMsgRxed = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 200);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	clock_t ts = clock();
	while (time_elapse(ts) < 150)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 150);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			ts = clock(); //restart timer
			rxMsgAry.push_back(msg);
			dwMsgRxed++;
			if (dwMsgRxed == dwNumberOfMsgToRx)
			{
				break;
			}
		}
	}
	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	if (dwNumberOfMsgToRx > dwMsgRxed)
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::ISO9141_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	unsigned long dwMsgRxed = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	clock_t ts = clock();
	while (time_elapse(ts) < 300)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 300);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			ts = clock(); //restart timer
			rxMsgAry.push_back(msg);
			dwMsgRxed++;
			if (dwMsgRxed == dwNumberOfMsgToRx)
			{
				break;
			}
		}
	}

	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	if (dwNumberOfMsgToRx > dwMsgRxed)
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::ISO9141_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	msgAry.push_back(txMsg);
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return lRetCode;
	}

	while (1)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 300);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			rxMsgAry.push_back(msg);
		}
	}

	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

bool ISO14230ParseResponse(const unsigned char* res_data, int res_len, unsigned char* prs_data, int* prs_len)
{
	if ((res_len < 3) || (res_len >= 128))
	{
		return false;
	}
	if (0x80 != (res_data[0] & 0xC0))
	{
		return false;
	}

	if (res_data[0] & 0x3F) //length info included in FMT
	{
		//83 F1 12 C1 EF 8F
		if (((int)(res_data[0] & 0x3F) + 3) != res_len)
		{
			return false;
		}
		::memcpy(prs_data, &res_data[2], (int)(res_data[0] & 0x3F) + 1);
		*prs_len = (int)(res_data[0] & 0x3F) + 1;
	}
	else
	{
		if (res_len < 4)
		{
			return false;
		}
		//80 F1 12 03 C1 EF 8F
		if (((int)res_data[3] + 4) != res_len)
		{
			return false;
		}
		prs_data[0] = res_data[2];
		::memcpy(prs_data + 1, &res_data[4], (int)res_data[3]);
		*prs_len = (int)res_data[3] + 1;
	}

	return true;
}

long CVehicleComm::ISO14230_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;
	unsigned char byRespData[128];
	int nRespLen = 0;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	if (txMsg.DataSize > 0)
	{
		msgAry.push_back(txMsg);
		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}

		if (STATUS_NOERROR != lRetCode)
		{
			return lRetCode;
		}
	}

	while (1)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 300);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			if (ISO14230ParseResponse(msg.Data, msg.DataSize, byRespData, &nRespLen))
			{
				rxMsgAry.push_back(msg);
			}		
		}
	}

	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

long CVehicleComm::ISO14230_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx)
{
	long lRetCode = STATUS_NOERROR;
	size_t dwNumMsgs = 0;
	size_t i = 0;
	std::vector <PASSTHRU_MSG> msgAry;
	unsigned char byRespData[128];
	int nRespLen = 0;
	unsigned long dwMsgRxed = 0;

	m_ptrJ2534Chan->ioctl(CLEAR_TX_BUFFER, NULL, NULL);
	m_ptrJ2534Chan->ioctl(CLEAR_RX_BUFFER, NULL, NULL);

	if (txMsg.DataSize > 0)
	{
		msgAry.push_back(txMsg);
		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Chan->writeMsgs(msgAry, 1000);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}

		if (STATUS_NOERROR != lRetCode)
		{
			return lRetCode;
		}
	}

	clock_t ts = clock();
	while (time_elapse(ts) < 300)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			dwNumMsgs = 0;
			msgAry.clear();
			msgAry.resize(1);
			dwNumMsgs = m_ptrJ2534Chan->readMsgs(msgAry, 300);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if ((STATUS_NOERROR != lRetCode) && (0 == dwNumMsgs))
		{
			break;
		}

		PASSTHRU_MSG& msg = msgAry[0];
		if ((msg.DataSize > 3) && (0 == (msg.RxStatus & 0x0F)))
		{
			if (ISO14230ParseResponse(msg.Data, msg.DataSize, byRespData, &nRespLen))
			{
				ts = clock(); //restart timer
				rxMsgAry.push_back(msg);
				dwMsgRxed++;
				if (dwMsgRxed == dwNumberOfMsgToRx)
				{
					break;
				}
			}
		}
	}

	if ((STATUS_NOERROR != lRetCode) && (ERR_BUFFER_EMPTY != lRetCode))
	{
		return lRetCode;
	}

	if (rxMsgAry.empty())
	{
		return ERR_BUFFER_EMPTY;
	}
	if (dwNumberOfMsgToRx > dwMsgRxed)
	{
		return ERR_BUFFER_EMPTY;
	}
	return STATUS_NOERROR;
}

CVehicleComm::CVehicleComm()
{
	m_dwProtocolId = ISO15765;
	m_dwBaudrate = 500000;
	m_bIsExtCANID = false;
	m_vectEcuID.clear();
	m_dwPrdcMsgID = 0;
}

CVehicleComm::~CVehicleComm()
{
	if (m_ptrJ2534Lib.get())
	{
		m_ptrJ2534Lib.reset();
	}
}

bool CVehicleComm::LoadPassThruLibrary()
{
	HKEY hKey;
	DWORD nPathLen;
	DWORD dwType;
	DWORD dwError = 0;
	char strDllPath[MAX_PATH];

	if (m_ptrJ2534Lib.get())
	{
		return true;
	}
	strcpy_s(strDllPath, MAX_PATH, "");
	if (ERROR_SUCCESS == RegOpenKeyExA(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\PassThruSupport.04.04\\RKW - VNCI Nano",
		0,
		KEY_READ,
		&hKey))
	{
		nPathLen = MAX_PATH;
		if (ERROR_SUCCESS == RegQueryValueExA(
			hKey,
			"FunctionLibrary",
			NULL,
			&dwType,
			(LPBYTE)strDllPath,
			&nPathLen))
		{
			try
			{
				m_ptrJ2534Lib = std::make_shared<J2534Library>((char*)strDllPath);
			}
			catch (J2534LoadException& exception)
			{
				exception.what();
				dwError = 1;
			}
		}
		else
		{
			dwError = 2;
		}
		RegCloseKey(hKey);
	}
	else
	{
		dwError = 2;
	}

    if (dwError)
    {
        return false;
    }
    return true;
}

bool CVehicleComm::ConnectDevice(void)
{
	long lRetCode = STATUS_NOERROR;
	int i = 0;

	if (NULL == m_ptrJ2534Lib.get())
	{
		return false;
	}
	if (m_ptrJ2534Dev.get())
	{
		m_ptrJ2534Dev.reset();
	}

	for (i = 0; i < 3; i++)
	{
		try {
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Dev = m_ptrJ2534Lib->open(NULL);
		}
		catch (J2534FunctionException& exception) {
			lRetCode = exception.code();
		}
		if (STATUS_NOERROR == lRetCode)
		{
			break;
		}
	}

	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}
    return true;
}

void CVehicleComm::DisconnectDevice(void)
{
	m_vectEcuID.clear();
	m_vectEcuKeyword.clear();
	m_vectSupportCurrentData.clear();
	m_vectEcuConfirmedDtc.clear();
	m_vectEcuPendingDtc.clear();
	m_vectEcuPermanentDtc.clear();
	m_vectSupportFreezeFrameData.clear();
	m_vectSupportInfoType.clear();

	if (m_ptrJ2534Chan.get())
	{
		m_ptrJ2534Chan.reset();
	}
	if (m_ptrJ2534Dev.get())
	{
		m_ptrJ2534Dev.reset();
	}
}

int CVehicleComm::GetEcuIndexByID(unsigned long dwID) const
{
	std::vector<unsigned long>::const_iterator it = std::find(m_vectEcuID.begin(), m_vectEcuID.end(), dwID);
	if (it != m_vectEcuID.end())
		return (it - m_vectEcuID.begin());
	else
		return -1;
}

static PASSTHRU_MSG MaskPTMsg;
static PASSTHRU_MSG PatternPTMsg;
static PASSTHRU_MSG FCPTMsg;

bool CVehicleComm::ScanForDtc(void)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	int i = 0;
	int m = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;
	std::vector<unsigned long> vectDtcCount;
	std::vector<unsigned long> vectDtcRecord;
	std::vector<bool> vectDtcSupport;
	unsigned long dwNumberOfMsgToRx = 0;

	m_vectEcuConfirmedDtc.clear();
	m_vectEcuPendingDtc.clear();
	m_vectEcuPermanentDtc.clear();

	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		vectDtcSupport.push_back(false);
	}
	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		m_vectEcuConfirmedDtc.push_back(vectDtcRecord);
	}
	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		m_vectEcuPendingDtc.push_back(vectDtcRecord);
	}
	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		m_vectEcuPermanentDtc.push_back(vectDtcRecord);
	}

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	if (ISO15765 == m_dwProtocolId)
	{
		//current DTC
		vectDtcCount.clear();
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			vectDtcCount.push_back(0);
		}

		for (i = 0; i < (int)m_vectSupportCurrentData.size(); i++)
		{
			if (m_vectSupportCurrentData[i][0] & 0x80) //PID#01 supported
			{
				vectDtcSupport[i] = true;
			}
		}

		dwNumberOfMsgToRx = 0;
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			if (vectDtcSupport[i])
			{
				dwNumberOfMsgToRx++;
			}
		}

		if (dwNumberOfMsgToRx > 0)
		{
			// Get DTC number
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
				TxPTMsg.Data[4] = 0x01;
				TxPTMsg.Data[5] = 0x01;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
				TxPTMsg.Data[4] = 0x01;
				TxPTMsg.Data[5] = 0x01;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			//00 00 07 E8 41 01 XX XX XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 7) || (0x41 != rxMsgAry[i].Data[4]) || (0x01 != rxMsgAry[i].Data[5]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectDtcCount[nEcuIndex] = rxMsgAry[i].Data[6] & 0x7F;
			}

			for (i = 0; i < (int)vectDtcCount.size(); i++)
			{
				if (vectDtcCount[i] > 0)
				{
					break;
				}
			}

			if (i < (int)vectDtcCount.size()) //confirmed DTC exist
			{
				// confirmed DTC
				TxPTMsg.ProtocolID = ISO15765;
				TxPTMsg.DataSize = 5;
				TxPTMsg.ExtraDataIndex = 5;

				if (m_bIsExtCANID)
				{
					TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
					TxPTMsg.Data[0] = 0x18;
					TxPTMsg.Data[1] = 0xDB;
					TxPTMsg.Data[2] = 0x33;
					TxPTMsg.Data[3] = 0xF1;
					TxPTMsg.Data[4] = 0x03;
				}
				else
				{
					TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
					TxPTMsg.Data[0] = 0x00;
					TxPTMsg.Data[1] = 0x00;
					TxPTMsg.Data[2] = 0x07;
					TxPTMsg.Data[3] = 0xDF;
					TxPTMsg.Data[4] = 0x03;
				}

				rxMsgAry.clear();
				lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
				if (STATUS_NOERROR == lRetCode)
				{
					//00 00 07 E8 43 XX XX
					for (i = 0; i < (int)rxMsgAry.size(); i++)
					{
						if ((rxMsgAry[i].DataSize < 7) || (0x43 != rxMsgAry[i].Data[4]))
						{
							return false;
						}
						nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
						if (nEcuIndex < 0)
						{
							return false;
						}
						for (m = 0; m < (int)(rxMsgAry[i].DataSize - 5); m += 2)
						{
							if ((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m])
							{
								m_vectEcuConfirmedDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m]));
							}
						}
					}
				}
			}

			//pending DTC
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
				TxPTMsg.Data[4] = 0x07;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
				TxPTMsg.Data[4] = 0x07;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
			if (STATUS_NOERROR == lRetCode)
			{
				//00 00 07 E8 47 XX XX
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize < 7) || (0x47 != rxMsgAry[i].Data[4]))
					{
						return false;
					}
					nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < (int)(rxMsgAry[i].DataSize - 5); m += 2)
					{
						if ((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m])
						{
							m_vectEcuPendingDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m]));
						}
					}
				}
			}

			//permanent DTC
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
				TxPTMsg.Data[4] = 0x0A;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
				TxPTMsg.Data[4] = 0x0A;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
			if (STATUS_NOERROR == lRetCode)
			{
				//00 00 07 E8 4A XX XX
				//00 00 07 E8 7F 0A 11
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize == 7) && (0x7F == rxMsgAry[i].Data[4]) && (0x0A == rxMsgAry[i].Data[5]))
					{
						continue;
					}
					if ((rxMsgAry[i].DataSize < 7) || (0x4A != rxMsgAry[i].Data[4]))
					{
						return false;
					}
					nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < (int)(rxMsgAry[i].DataSize - 5); m += 2)
					{
						if ((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m])
						{
							m_vectEcuPermanentDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[5 + m] << 8) | rxMsgAry[i].Data[6 + m]));
						}
					}
				}
			}
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		//current DTC
		vectDtcCount.clear();
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			vectDtcCount.push_back(0);
		}

		for (i = 0; i < (int)m_vectSupportCurrentData.size(); i++)
		{
			if (m_vectSupportCurrentData[i][0] & 0x80) //PID#01 supported
			{
				vectDtcSupport[i] = true;
			}
		}

		dwNumberOfMsgToRx = 0;
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			if (vectDtcSupport[i])
			{
				dwNumberOfMsgToRx++;
			}
		}

		if (dwNumberOfMsgToRx > 0)
		{
			// Get DTC number
			TxPTMsg.ProtocolID = J1850PWM;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x61;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x01;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			//41 6B 10 41 01 XX XX XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x01 != rxMsgAry[i].Data[4]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectDtcCount[nEcuIndex] = rxMsgAry[i].Data[5] & 0x7F;
			}

			dwNumberOfMsgToRx = 0;
			for (i = 0; i < (int)vectDtcCount.size(); i++)
			{
				if (vectDtcCount[i] > 0)
				{
					dwNumberOfMsgToRx += (vectDtcCount[i] + 2) / 3;
				}
			}

			if (dwNumberOfMsgToRx > 0)
			{
				TxPTMsg.ProtocolID = J1850PWM;
				TxPTMsg.DataSize = 4;
				TxPTMsg.ExtraDataIndex = 4;
				TxPTMsg.Data[0] = 0x61;
				TxPTMsg.Data[1] = 0x6A;
				TxPTMsg.Data[2] = 0xF1;
				TxPTMsg.Data[3] = 0x03;

				rxMsgAry.clear();
				lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
				if (STATUS_NOERROR != lRetCode)
				{
					return false;
				}

				//41 6B 10 43 XX XX
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize < 4) || (0x43 != rxMsgAry[i].Data[3]))
					{
						return false;
					}
					nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}

					for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
					{
						if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
						{
							m_vectEcuConfirmedDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
						}
					}
				}
			}
		}

		//request pending DTC
		TxPTMsg.ProtocolID = J1850PWM;
		TxPTMsg.DataSize = 4;
		TxPTMsg.ExtraDataIndex = 4;
		TxPTMsg.Data[0] = 0x61;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x07;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			//41 6B 10 47 XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 4) || (0x47 != rxMsgAry[i].Data[3]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
				{
					if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
					{
						m_vectEcuPendingDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
					}
				}
			}
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		//current DTC
		vectDtcCount.clear();
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			vectDtcCount.push_back(0);
		}

		for (i = 0; i < (int)m_vectSupportCurrentData.size(); i++)
		{
			if (m_vectSupportCurrentData[i][0] & 0x80) //PID#01 supported
			{
				vectDtcSupport[i] = true;
			}
		}

		dwNumberOfMsgToRx = 0;
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			if (vectDtcSupport[i])
			{
				dwNumberOfMsgToRx++;
			}
		}

		if (dwNumberOfMsgToRx > 0)
		{
			// Get DTC number
			TxPTMsg.ProtocolID = J1850VPW;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x01;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			//48 6B 10 41 01 XX XX XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x01 != rxMsgAry[i].Data[4]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectDtcCount[nEcuIndex] = rxMsgAry[i].Data[5] & 0x7F;
			}

			dwNumberOfMsgToRx = 0;
			for (i = 0; i < (int)vectDtcCount.size(); i++)
			{
				if (vectDtcCount[i] > 0)
				{
					dwNumberOfMsgToRx += (vectDtcCount[i] + 2) / 3;
				}
			}

			if (dwNumberOfMsgToRx > 0)
			{
				TxPTMsg.ProtocolID = J1850VPW;
				TxPTMsg.DataSize = 4;
				TxPTMsg.ExtraDataIndex = 4;
				TxPTMsg.Data[0] = 0x68;
				TxPTMsg.Data[1] = 0x6A;
				TxPTMsg.Data[2] = 0xF1;
				TxPTMsg.Data[3] = 0x03;

				rxMsgAry.clear();
				lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
				if (STATUS_NOERROR != lRetCode)
				{
					return false;
				}

				//48 6B 10 43 XX XX
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize < 4) || (0x43 != rxMsgAry[i].Data[3]))
					{
						return false;
					}
					nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}

					for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
					{
						if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
						{
							m_vectEcuConfirmedDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
						}
					}
				}
			}
		}

		//request pending DTC
		TxPTMsg.ProtocolID = J1850VPW;
		TxPTMsg.DataSize = 4;
		TxPTMsg.ExtraDataIndex = 4;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x07;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			//48 6B 10 47 XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 4) || (0x47 != rxMsgAry[i].Data[3]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
				{
					if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
					{
						m_vectEcuPendingDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
					}
				}
			}
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		//current DTC
		vectDtcCount.clear();
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			vectDtcCount.push_back(0);
		}

		for (i = 0; i < (int)m_vectSupportCurrentData.size(); i++)
		{
			if (m_vectSupportCurrentData[i][0] & 0x80) //PID#01 supported
			{
				vectDtcSupport[i] = true;
			}
		}

		dwNumberOfMsgToRx = 0;
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			if (vectDtcSupport[i])
			{
				dwNumberOfMsgToRx++;
			}
		}

		if (dwNumberOfMsgToRx > 0)
		{
			// Get DTC number
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x01;

			rxMsgAry.clear();
			lRetCode = ISO9141_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			//48 6B 10 41 01 XX XX XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x01 != rxMsgAry[i].Data[4]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectDtcCount[nEcuIndex] = rxMsgAry[i].Data[5] & 0x7F;
			}

			dwNumberOfMsgToRx = 0;
			for (i = 0; i < (int)vectDtcCount.size(); i++)
			{
				if (vectDtcCount[i] > 0)
				{
					dwNumberOfMsgToRx += (vectDtcCount[i] + 2) / 3;
				}
			}

			if (dwNumberOfMsgToRx > 0)
			{
				TxPTMsg.ProtocolID = ISO9141;
				TxPTMsg.DataSize = 4;
				TxPTMsg.ExtraDataIndex = 4;
				TxPTMsg.Data[0] = 0x68;
				TxPTMsg.Data[1] = 0x6A;
				TxPTMsg.Data[2] = 0xF1;
				TxPTMsg.Data[3] = 0x03;

				rxMsgAry.clear();
				lRetCode = ISO9141_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
				if (STATUS_NOERROR != lRetCode)
				{
					return false;
				}

				//48 6B 10 43 XX XX
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize < 4) || (0x43 != rxMsgAry[i].Data[3]))
					{
						return false;
					}
					nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}

					for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
					{
						if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
						{
							m_vectEcuConfirmedDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
						}
					}
				}
			}
		}

		//request pending DTC
		TxPTMsg.ProtocolID = ISO9141;
		TxPTMsg.DataSize = 4;
		TxPTMsg.ExtraDataIndex = 4;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x07;

		rxMsgAry.clear();
		lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			//48 6B 10 47 XX XX
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				if ((rxMsgAry[i].DataSize < 4) || (0x47 != rxMsgAry[i].Data[3]))
				{
					return false;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < (int)(rxMsgAry[i].DataSize - 4); m += 2)
				{
					if ((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m])
					{
						m_vectEcuPendingDtc[nEcuIndex].push_back(((rxMsgAry[i].Data[4 + m] << 8) | rxMsgAry[i].Data[5 + m]));
					}
				}
			}
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		//current DTC
		vectDtcCount.clear();
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			vectDtcCount.push_back(0);
		}

		for (i = 0; i < (int)m_vectSupportCurrentData.size(); i++)
		{
			if (m_vectSupportCurrentData[i][0] & 0x80) //PID#01 supported
			{
				vectDtcSupport[i] = true;
			}
		}

		dwNumberOfMsgToRx = 0;
		for (i = 0; i < (int)m_vectEcuID.size(); i++)
		{
			if (vectDtcSupport[i])
			{
				dwNumberOfMsgToRx++;
			}
		}

		if (dwNumberOfMsgToRx > 0)
		{
			// Get DTC number
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x01;

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				//10 41 01 XX XX XX XX
				if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
					(7 <= nRespLen) &&
					(0x41 == byRespData[1]) &&
					(0x01 == byRespData[2]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					vectDtcCount[nEcuIndex] = rxMsgAry[i].Data[5] & 0x7F;
				}
			}

			dwNumberOfMsgToRx = 0;
			for (i = 0; i < (int)vectDtcCount.size(); i++)
			{
				if (vectDtcCount[i] > 0)
				{
					dwNumberOfMsgToRx += (vectDtcCount[i] + 2) / 3;
				}
				else
				{
					dwNumberOfMsgToRx++;
				}
			}

			if (dwNumberOfMsgToRx > 0)
			{
				TxPTMsg.ProtocolID = ISO14230;
				TxPTMsg.DataSize = 4;
				TxPTMsg.ExtraDataIndex = 4;
				TxPTMsg.Data[0] = 0xC1;
				TxPTMsg.Data[1] = 0x33;
				TxPTMsg.Data[2] = 0xF1;
				TxPTMsg.Data[3] = 0x03;

				rxMsgAry.clear();
				lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
				if (STATUS_NOERROR != lRetCode)
				{
					return false;
				}

				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					//10 43 XX XX
					if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
						(4 <= nRespLen) &&
						(0x43 == byRespData[1]))
					{
						nEcuIndex = GetEcuIndexByID(byRespData[0]);
						if (nEcuIndex < 0)
						{
							return false;
						}
						for (m = 0; m < (int)(nRespLen - 2); m += 2)
						{
							if ((byRespData[2 + m] << 8) | byRespData[3 + m])
							{
								m_vectEcuConfirmedDtc[nEcuIndex].push_back((byRespData[2 + m] << 8) | byRespData[3 + m]);
							}
						}
					}
				}
			}
		}

		//request pending DTC
		TxPTMsg.ProtocolID = ISO14230;
		TxPTMsg.DataSize = 4;
		TxPTMsg.ExtraDataIndex = 4;
		TxPTMsg.Data[0] = 0xC1;
		TxPTMsg.Data[1] = 0x33;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x07;

		rxMsgAry.clear();
		lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				//10 47 XX XX
				if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
					(4 <= nRespLen) &&
					(0x47 == byRespData[1]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < (int)(nRespLen - 2); m += 2)
					{
						if ((byRespData[2 + m] << 8) | byRespData[3 + m])
						{
							m_vectEcuPendingDtc[nEcuIndex].push_back((byRespData[2 + m] << 8) | byRespData[3 + m]);
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

inline unsigned long GetBit1Count(unsigned long val)
{
	unsigned long i = 0;
	unsigned long m = 0;
	while (m < 32)
	{
		if ((val >> m) & 1)
		{
			i++;
		}
		m++;
	}

	return i;
}

bool CVehicleComm::ReadDataValues(std::vector<std::vector<DD_PARAM_REC>>& vectDataList, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values)
{
	std::vector<std::vector<unsigned char>>::iterator it;
	std::map<unsigned long, std::vector<unsigned char>> mapValue;
	unsigned long dwPID = 0;
	unsigned long dwNumberOfMsgToRecv[0x100];
	unsigned long i = 0;
	unsigned long m = 0;
	unsigned long dwMaxRxCount = 0;
	unsigned long dwPIDCount = 0;
	unsigned long dwPIDs[0x100];
	unsigned long dwFlags[0x100];
	unsigned long dwLength[0x100];
	std::map<unsigned long, unsigned long> mapLength;
	std::map<int, std::vector<unsigned long>> mapEcuPid;
	std::list<unsigned long> listPids;

	//clock_t ts = clock();
	//DUITRACE(_T("CVehicleComm::ReadDataValues. Enter ... clock=%lu"), ts);

	values.clear();
	for (i = 0; i < (unsigned long)vectDataList.size(); i++)
	{
		mapValue.clear();
		values.push_back(mapValue);
	}

	//ts = clock();
	//DUITRACE(_T("CVehicleComm::ReadDataValues. 1 ... clock=%lu"), ts);

	for (dwPID = 0; dwPID < 0x100; dwPID++)
	{
		dwNumberOfMsgToRecv[dwPID] = 0;
		for (auto & pid : m_vectCurrentDataValue)
		{
			std::map<unsigned long, std::vector<unsigned char>>::iterator it = pid.find(dwPID);
			if (it != pid.end())
			{
				dwNumberOfMsgToRecv[dwPID]++;
				if (mapLength.find(dwPID) == mapLength.end())
				{
					mapLength.insert(std::make_pair(dwPID, it->second.size()));
				}
			}
		}
	}

//	DUITRACE(_T("CVehicleComm::ReadDataValues. 2 ... clock=%lu, time elapsed=%lu"), clock(), clock() - ts);
	//ts = clock();

	if (ISO15765 == m_dwProtocolId)
	{
		//integrate PID
		memset(dwFlags, 0, sizeof(dwFlags));
		for (i = 0; i < (unsigned long)m_vectCurrentDataValue.size(); i++)
		{
			std::map<unsigned long, std::vector<unsigned char>>& pid = m_vectCurrentDataValue[i];
			std::vector<DD_PARAM_REC > & data_list = vectDataList[i];
			for (auto& param : data_list)
			{
				if (pid.find(param.paramRd.address.address) != pid.end())
				{
					dwFlags[param.paramRd.address.address] |= (1 << i);
				}
			}
		}

//		DUITRACE(_T("CVehicleComm::ReadDataValues. 3 ... clock=%lu, time elapsed=%lu"), clock(), clock() - ts);
		//ts = clock();

		for (dwPID = 0; dwPID < 0x100; dwPID++)
		{
			if (dwFlags[dwPID])
			{
				auto it = std::find(listPids.begin(), listPids.end(), dwFlags[dwPID]);
				if (it == listPids.end())
				{
					listPids.push_back(dwFlags[dwPID]);
				}
			}
		}
//		DUITRACE(_T("CVehicleComm::ReadDataValues. 4 ... clock=%lu, time elapsed=%lu"), clock(), clock() - ts);

		for (auto & flag : listPids)
		{
			dwPIDCount = 0;
			for (dwPID = 0; dwPID < 0x100; dwPID++)
			{
				if (flag == dwFlags[dwPID])
				{
					dwLength[dwPIDCount] = mapLength[dwPID];
					dwPIDs[dwPIDCount] = dwPID;
					dwPIDCount++;
				}
			}

			if (dwPIDCount > 0)
			{
				for (m = 0; m < dwPIDCount / 6; m++)
				{
					if (!ReadMultipleDataValue(6, &dwPIDs[m * 6], &dwLength[m * 6], &dwNumberOfMsgToRecv[0], flag, values))
					{
						return false;
					}
				}
				if (dwPIDCount % 6)
				{
					if (!ReadMultipleDataValue(dwPIDCount % 6, &dwPIDs[m * 6], &dwLength[m * 6], &dwNumberOfMsgToRecv[0], flag, values))
					{
						return false;
					}
				}
			}
		}
	}
	else
	{
		for (dwPID = 0; dwPID < 0x100; dwPID++)
		{
			if (dwNumberOfMsgToRecv[dwPID] > 0)
			{
				if (!ReadDataValue(dwPID, dwNumberOfMsgToRecv[dwPID], values))
				{
					return false;
				}
			}
		}
	}

	//DUITRACE(_T("CVehicleComm::ReadDataValues. Exit ... clock=%lu, time elapsed=%lu"), clock(), clock() - ts);
	return true;
}


bool CVehicleComm::ReadMultipleDataValue(unsigned long dwNumberOfPID, unsigned long* pdwPID, unsigned long* pdwLength, unsigned long* pdwNumberOfMsgToRx, unsigned long flag, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	int i = 0;
	int m = 0;
	int n = 0;
	int p = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;
	std::vector<unsigned char> vectValue;

	//clock_t ts = clock();
	//DUITRACE(_T("CVehicleComm::ReadMultipleDataValue. pdwNumberOfMsgToRx[pdwPID[0]] = %u. clock = %lu"), pdwNumberOfMsgToRx[pdwPID[0]], ts);

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	if ((ISO15765 != m_dwProtocolId) || (dwNumberOfPID > 6))
	{
		return false;
	}
	TxPTMsg.ProtocolID = ISO15765;
	TxPTMsg.DataSize = 5 + dwNumberOfPID;
	TxPTMsg.ExtraDataIndex = 5 + dwNumberOfPID;

	if (m_bIsExtCANID)
	{
		TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
		TxPTMsg.Data[0] = 0x18;
		TxPTMsg.Data[1] = 0xDB;
		TxPTMsg.Data[2] = 0x33;
		TxPTMsg.Data[3] = 0xF1;
		TxPTMsg.Data[4] = 0x01;
		for (i = 0; i < (int)dwNumberOfPID; i++)
		{
			TxPTMsg.Data[5 + i] = (unsigned char)pdwPID[i];
		}	
	}
	else
	{
		TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
		TxPTMsg.Data[0] = 0x00;
		TxPTMsg.Data[1] = 0x00;
		TxPTMsg.Data[2] = 0x07;
		TxPTMsg.Data[3] = 0xDF;
		TxPTMsg.Data[4] = 0x01;
		for (i = 0; i < (int)dwNumberOfPID; i++)
		{
			TxPTMsg.Data[5 + i] = (unsigned char)pdwPID[i];
		}
	}

	rxMsgAry.clear();
	lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, pdwNumberOfMsgToRx[pdwPID[0]]);
	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}

	//00 00 07 E8 41 01 XX XX XX XX
	for (i = 0; i < (int)rxMsgAry.size(); i++)
	{
		if ((rxMsgAry[i].DataSize < 7) || (0x41 != rxMsgAry[i].Data[4]))
		{
			return false;
		}
		nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
		if (nEcuIndex < 0)
		{
			return false;
		}

		if (0 == ((1 << nEcuIndex) & flag))
		{
			continue;
		}

		for (n = 0, p = 0; (n < (int)dwNumberOfPID) && (p < (int)(rxMsgAry[i].DataSize - 5)); n++)
		{
			if ((unsigned char)pdwPID[n] == rxMsgAry[i].Data[5 + p])
			{
				vectValue.clear();
				for (m = 0; m < (int)pdwLength[n]; m++)
				{
					vectValue.push_back(rxMsgAry[i].Data[6 + p + m]);
				}
				values[nEcuIndex].insert(std::make_pair(pdwPID[n], vectValue));
				p += m + 1;
			}
			else
			{
				return false;
			}
		}
	}

	//DUITRACE(_T("CVehicleComm::ReadMultipleDataValue. Exit. clock = %lu, time elapsed = %lu"), clock(), clock() - ts);
	return true;
}

bool CVehicleComm::ReadDataValue(unsigned long dwPID, unsigned long dwNumberOfMsgToRx, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	int i = 0;
	int m = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;
	std::vector<unsigned char> vectValue;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	if (ISO15765 == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = ISO15765;
		TxPTMsg.DataSize = 6;
		TxPTMsg.ExtraDataIndex = 6;

		if (m_bIsExtCANID)
		{
			TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
			TxPTMsg.Data[0] = 0x18;
			TxPTMsg.Data[1] = 0xDB;
			TxPTMsg.Data[2] = 0x33;
			TxPTMsg.Data[3] = 0xF1;
			TxPTMsg.Data[4] = 0x01;
			TxPTMsg.Data[5] = (unsigned char)dwPID;
		}
		else
		{
			TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
			TxPTMsg.Data[0] = 0x00;
			TxPTMsg.Data[1] = 0x00;
			TxPTMsg.Data[2] = 0x07;
			TxPTMsg.Data[3] = 0xDF;
			TxPTMsg.Data[4] = 0x01;
			TxPTMsg.Data[5] = (unsigned char)dwPID;
		}

		rxMsgAry.clear();
		lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//00 00 07 E8 41 01 XX XX XX XX
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 7) || (0x41 != rxMsgAry[i].Data[4]) || ((unsigned char)dwPID != rxMsgAry[i].Data[5]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 6; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[6 + m]);
			}
			values[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = J1850PWM;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x61;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//41 6B 10 41 01 XX XX XX XX
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			values[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = J1850VPW;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//48 6B 10 41 01 XX XX XX XX
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			values[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = ISO9141;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = ISO9141_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//48 6B 10 41 01 XX XX XX XX
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			values[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		TxPTMsg.ProtocolID = ISO14230;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0xC2;
		TxPTMsg.Data[1] = 0x33;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			//10 41 01 XX XX XX XX
			if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
				(4 <= nRespLen) &&
				(0x41 == byRespData[1]) &&
				((unsigned char)dwPID == byRespData[2]))
			{
				nEcuIndex = GetEcuIndexByID(byRespData[0]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectValue.clear();
				for (m = 0; m < (int)rxMsgAry[i].DataSize - 3; m++)
				{
					vectValue.push_back(rxMsgAry[i].Data[3 + m]);
				}
				values[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
			}
		}
		return true;
	}
	return false;
}

bool CVehicleComm::ReadDataValue(unsigned long dwPID, unsigned long dwNumberOfMsgToRx)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	int i = 0;
	int m = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;
	std::vector<unsigned char> vectValue;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	if (ISO15765 == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = ISO15765;
		TxPTMsg.DataSize = 6;
		TxPTMsg.ExtraDataIndex = 6;

		if (m_bIsExtCANID)
		{
			TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
			TxPTMsg.Data[0] = 0x18;
			TxPTMsg.Data[1] = 0xDB;
			TxPTMsg.Data[2] = 0x33;
			TxPTMsg.Data[3] = 0xF1;
			TxPTMsg.Data[4] = 0x01;
			TxPTMsg.Data[5] = (unsigned char)dwPID;
		}
		else
		{
			TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
			TxPTMsg.Data[0] = 0x00;
			TxPTMsg.Data[1] = 0x00;
			TxPTMsg.Data[2] = 0x07;
			TxPTMsg.Data[3] = 0xDF;
			TxPTMsg.Data[4] = 0x01;
			TxPTMsg.Data[5] = (unsigned char)dwPID;
		}

		rxMsgAry.clear();
		lRetCode = ISO15765_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//00 00 07 E8 41 01 XX XX XX XX
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 7) || (0x41 != rxMsgAry[i].Data[4]) || ((unsigned char)dwPID != rxMsgAry[i].Data[5]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 6; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[6 + m]);
			}
			m_vectCurrentDataValue[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = J1850PWM;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x61;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//41 6B 10 41 01 XX ...
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			m_vectCurrentDataValue[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = J1850VPW;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = J1850_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//48 6B 10 41 01 XX ...
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			m_vectCurrentDataValue[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		TxPTMsg.ProtocolID = ISO9141;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0x68;
		TxPTMsg.Data[1] = 0x6A;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = ISO9141_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		//48 6B 10 41 01 XX ...
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			if ((rxMsgAry[i].DataSize < 6) || (0x41 != rxMsgAry[i].Data[3]) || ((unsigned char)dwPID != rxMsgAry[i].Data[4]))
			{
				return false;
			}
			nEcuIndex = GetEcuIndexByID(rxMsgAry[i].Data[2]);
			if (nEcuIndex < 0)
			{
				return false;
			}
			vectValue.clear();
			for (m = 0; m < (int)rxMsgAry[i].DataSize - 5; m++)
			{
				vectValue.push_back(rxMsgAry[i].Data[5 + m]);
			}
			m_vectCurrentDataValue[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		TxPTMsg.ProtocolID = ISO14230;
		TxPTMsg.DataSize = 5;
		TxPTMsg.ExtraDataIndex = 5;
		TxPTMsg.Data[0] = 0xC2;
		TxPTMsg.Data[1] = 0x33;
		TxPTMsg.Data[2] = 0xF1;
		TxPTMsg.Data[3] = 0x01;
		TxPTMsg.Data[4] = (unsigned char)dwPID;

		rxMsgAry.clear();
		lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, dwNumberOfMsgToRx);
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			//10 41 01 XX XX XX XX
			if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
				(4 <= nRespLen) &&
				(0x41 == byRespData[1]) &&
				((unsigned char)dwPID == byRespData[2]))
			{
				nEcuIndex = GetEcuIndexByID(byRespData[0]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				vectValue.clear();
				for (m = 0; m < (int)rxMsgAry[i].DataSize - 3; m++)
				{
					vectValue.push_back(rxMsgAry[i].Data[3 + m]);
				}
				m_vectCurrentDataValue[nEcuIndex].insert(std::make_pair(dwPID, vectValue));
			}
		}
		return true;
	}
	return false;
}

bool CVehicleComm::ScanForCurrentDataValue(void)
{
	std::vector<std::vector<unsigned char>>::iterator it;
	std::map<unsigned long, std::vector<unsigned char>> mapValue;
	unsigned long dwNumberOfMsgToRecv = 0;
	unsigned char ucPID = 0;
	int i = 0;

	m_vectCurrentDataValue.clear();

	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		mapValue.clear();
		m_vectCurrentDataValue.push_back(mapValue);
	}

	for (ucPID = 0; ucPID < 0xB0; ucPID++)
	{
		if (0 == (ucPID & 0x1F))
		{
			continue;
		}
		dwNumberOfMsgToRecv = 0;
		for (it = m_vectSupportCurrentData.begin(); it != m_vectSupportCurrentData.end(); it++)
		{
			if (it->at((ucPID / 0x20) * 4 + ((ucPID - 1) % 0x20) / 8) & (0x80 >> ((ucPID - 1) % 8)))
			{
				dwNumberOfMsgToRecv++;
			}
		}
		if (dwNumberOfMsgToRecv > 0)
		{
			if (!ReadDataValue(ucPID, dwNumberOfMsgToRecv))
			{
				return false;
			}
		}
	}

	return true;
}

bool CVehicleComm::ScanForSupportCurrentData(void)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	int i = 0;
	int m = 0;
	int n = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	if (ISO15765 == m_dwProtocolId)
	{
		for (i = 0; i < 7; i++)
		{
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[4] = 0x01;
			TxPTMsg.Data[5] = 0x20 + (i * 0x20);

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//00 00 07 E8 41 RR XX XX XX XX
			//18 DA F1 10 41 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize == 10) && 
					(0x41 == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID((rxMsgAry[n].Data[0] << 24) |
						(rxMsgAry[n].Data[1] << 16) |
						(rxMsgAry[n].Data[2] << 8) |
						(rxMsgAry[n].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportCurrentData[nEcuIndex][4 + (i * 4) + m] = rxMsgAry[n].Data[6 + m];
					}
				}
			}
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		for (i = 0; i < 7; i++)
		{
			TxPTMsg.ProtocolID = J1850PWM;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x61;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x20 + (i * 0x20);

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//41 6B 10 41 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize < 9) ||
					(0x41 != rxMsgAry[n].Data[3]) ||
					(TxPTMsg.Data[4] != rxMsgAry[n].Data[4]))
				{
					continue;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < 4; m++)
				{
					m_vectSupportCurrentData[nEcuIndex][4 + (i * 4) + m] = rxMsgAry[n].Data[5 + m];
				}
			}
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		for (i = 0; i < 7; i++)
		{
			TxPTMsg.ProtocolID = J1850VPW;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x20 + (i * 0x20);

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 41 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize < 9) ||
					(0x41 != rxMsgAry[n].Data[3]) ||
					(TxPTMsg.Data[4] != rxMsgAry[n].Data[4]))
				{
					continue;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < 4; m++)
				{
					m_vectSupportCurrentData[nEcuIndex][4 + (i * 4) + m] = rxMsgAry[n].Data[5 + m];
				}
			}
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		for (i = 0; i < 7; i++)
		{
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x20 + (i * 0x20);

			rxMsgAry.clear();
			lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 41 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize != 9) ||
					(0x41 != rxMsgAry[n].Data[3]) ||
					(TxPTMsg.Data[4] != rxMsgAry[n].Data[4]))
				{
					continue;
				}
				nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
				if (nEcuIndex < 0)
				{
					return false;
				}
				for (m = 0; m < 4; m++)
				{
					m_vectSupportCurrentData[nEcuIndex][4 + (i * 4) + m] = rxMsgAry[n].Data[5 + m];
				}
			}
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		for (i = 0; i < 7; i++)
		{
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x20 + (i * 0x20);

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				//10 41 RR XX XX XX XX
				if ((ISO14230ParseResponse(rxMsgAry[n].Data, rxMsgAry[n].DataSize, byRespData, &nRespLen)) &&
					(7 == nRespLen) &&
					(0x41 == byRespData[1]) &&
					(TxPTMsg.Data[4] == byRespData[2]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportCurrentData[nEcuIndex][4 + (i * 4) + m] = rxMsgAry[n].Data[3 + m];
					}
				}
			}
		}
		return true;
	}

	return false;
}

bool CVehicleComm::ScanForSupportInfoType(void)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	std::vector <unsigned char> vectSupportRec;
	int i = 0;
	int m = 0;
	int n = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}
	m_vectSupportInfoType.clear();

	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		vectSupportRec.clear();
		for (m = 0; m < 4; m++)
		{
			vectSupportRec.push_back(0);
		}
		m_vectSupportInfoType.push_back(vectSupportRec);
	}

	if (ISO15765 == m_dwProtocolId)
	{
		for (i = 0; i < 1; i++)
		{
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[4] = 0x09;
			TxPTMsg.Data[5] = (unsigned char)(i * 0x20);

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//00 00 07 E8 49 RR XX XX XX XX
			//18 DA F1 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize == 10) &&
					(0x49 == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID((rxMsgAry[n].Data[0] << 24) |
						(rxMsgAry[n].Data[1] << 16) |
						(rxMsgAry[n].Data[2] << 8) |
						(rxMsgAry[n].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportInfoType[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[6 + m];
					}
				}
			}
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		for (i = 0; i < 1; i++)
		{
			TxPTMsg.ProtocolID = J1850PWM;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x61;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//41 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize >= 9) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportInfoType[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[5 + m];
					}
				}
			}
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		for (i = 0; i < 1; i++)
		{
			TxPTMsg.ProtocolID = J1850VPW;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize >= 9) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportInfoType[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[5 + m];
					}
				}
			}
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);

			rxMsgAry.clear();
			lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize == 9) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportInfoType[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[5 + m];
					}
				}
			}
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		for (i = 0; i < 1; i++)
		{
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				//10 49 RR XX XX XX XX
				if ((ISO14230ParseResponse(rxMsgAry[n].Data, rxMsgAry[n].DataSize, byRespData, &nRespLen)) &&
					(7 == nRespLen) &&
					(0x49 == byRespData[1]) &&
					(TxPTMsg.Data[4] == byRespData[2]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportInfoType[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[3 + m];
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool CVehicleComm::ReadInfoTypes(std::vector <T_INFO_TYPE>& tInfoTypes)
{
	PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	std::vector <unsigned char> vectSupportRec;
	int i = 0;
	int m = 0;
	int n = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;
	T_INFO_TYPE oneInfoType;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}

	tInfoTypes.clear();
	for (i = 0; i < (int)m_vectSupportInfoType.size(); i++)
	{
		for (m = 0; m < (int)m_vectSupportInfoType[i].size(); m++)
		{
			for (n = 0; n < 8; n++)
			{
				if (m_vectSupportInfoType[i][m] & (0x80 >> n))
				{
					oneInfoType.ecuIndex = i;
					oneInfoType.infoType = (unsigned char)((m / 4) * 0x20 + ((m % 4) * 8) + 1 + n);
					oneInfoType.infoData.clear();
					tInfoTypes.push_back(oneInfoType);
				}
			}
		}
	}

	for (i = 0; i < (int)tInfoTypes.size(); i++)
	{
		if (ISO15765 == m_dwProtocolId)
		{
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[4] = 0x09;
			TxPTMsg.Data[5] = tInfoTypes[i].infoType;

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//00 00 07 E8 49 RR XX XX XX XX
			//18 DA F1 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize > 6) &&
					(0x49 == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID((rxMsgAry[n].Data[0] << 24) |
						(rxMsgAry[n].Data[1] << 16) |
						(rxMsgAry[n].Data[2] << 8) |
						(rxMsgAry[n].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					if (tInfoTypes[i].ecuIndex == nEcuIndex)
					{
						for (m = 0; m < (int)(rxMsgAry[n].DataSize - 6); m++)
						{
							tInfoTypes[i].infoData.push_back(rxMsgAry[n].Data[6 + m]);
						}
					}
				}
			}
		}
		else if (J1850PWM == m_dwProtocolId)
		{
			TxPTMsg.ProtocolID = J1850PWM;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x61;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = tInfoTypes[i].infoType;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//41 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize > 5) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					if (tInfoTypes[i].ecuIndex == nEcuIndex)
					{
						for (m = 0; m < (int)(rxMsgAry[n].DataSize - 5); m++)
						{
							tInfoTypes[i].infoData.push_back(rxMsgAry[n].Data[5 + m]);
						}
					}
				}
			}
		}
		else if (J1850VPW == m_dwProtocolId)
		{
			TxPTMsg.ProtocolID = J1850VPW;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = tInfoTypes[i].infoType;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize > 5) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					if (tInfoTypes[i].ecuIndex == nEcuIndex)
					{
						for (m = 0; m < (int)(rxMsgAry[n].DataSize - 5); m++)
						{
							tInfoTypes[i].infoData.push_back(rxMsgAry[n].Data[5 + m]);
						}
					}
				}
			}
		}
		else if (ISO9141 == m_dwProtocolId)
		{
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = tInfoTypes[i].infoType;

			rxMsgAry.clear();
			lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 49 RR XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize > 5) &&
					(0x49 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					if (tInfoTypes[i].ecuIndex == nEcuIndex)
					{
						for (m = 0; m < (int)(rxMsgAry[n].DataSize - 5); m++)
						{
							tInfoTypes[i].infoData.push_back(rxMsgAry[n].Data[5 + m]);
						}
					}
				}
			}
		}
		else if (ISO14230 == m_dwProtocolId)
		{
			unsigned char byRespData[128];
			int nRespLen = 0;

			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x09;
			TxPTMsg.Data[4] = tInfoTypes[i].infoType;

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				//10 49 RR XX XX XX XX
				if ((ISO14230ParseResponse(rxMsgAry[n].Data, rxMsgAry[n].DataSize, byRespData, &nRespLen)) &&
					(3 < nRespLen) &&
					(0x49 == byRespData[1]) &&
					(TxPTMsg.Data[4] == byRespData[2]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					if (tInfoTypes[i].ecuIndex == nEcuIndex)
					{
						for (m = 0; m < (int)(rxMsgAry[n].DataSize - 3); m++)
						{
							tInfoTypes[i].infoData.push_back(rxMsgAry[n].Data[3 + m]);
						}
					}
				}
			}
		}
	}

	return true;
}

bool CVehicleComm::ScanForSupportFreezeFrameData(void)
{
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	std::vector <unsigned char> vectSupportRec;
	int i = 0;
	int m = 0;
	int n = 0;
	int nEcuIndex = 0;
	long lRetCode = STATUS_NOERROR;

	if (NULL == m_ptrJ2534Chan.get())
	{
		return false;
	}
	m_vectSupportFreezeFrameData.clear();

	for (i = 0; i < (int)m_vectEcuID.size(); i++)
	{
		vectSupportRec.clear();
		for (m = 0; m < 32; m++)
		{
			vectSupportRec.push_back(0);
		}
		m_vectSupportFreezeFrameData.push_back(vectSupportRec);
	}

	if (ISO15765 == m_dwProtocolId)
	{
		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = ISO15765;
			TxPTMsg.DataSize = 7;
			TxPTMsg.ExtraDataIndex = 7;
			TxPTMsg.Data[4] = 0x02;
			TxPTMsg.Data[5] = (unsigned char)(i * 0x20);
			TxPTMsg.Data[6] = 0x00;

			if (m_bIsExtCANID)
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD | CAN_29BIT_ID;
				TxPTMsg.Data[0] = 0x18;
				TxPTMsg.Data[1] = 0xDB;
				TxPTMsg.Data[2] = 0x33;
				TxPTMsg.Data[3] = 0xF1;
			}
			else
			{
				TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
				TxPTMsg.Data[0] = 0x00;
				TxPTMsg.Data[1] = 0x00;
				TxPTMsg.Data[2] = 0x07;
				TxPTMsg.Data[3] = 0xDF;
			}

			rxMsgAry.clear();
			lRetCode = ISO15765_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//00 00 07 E8 42 RR 00 XX XX XX XX
			//18 DA F1 10 42 RR 00 XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize == 11) &&
					(0x42 == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]) &&
					(TxPTMsg.Data[6] == rxMsgAry[n].Data[6]))
				{
					nEcuIndex = GetEcuIndexByID((rxMsgAry[n].Data[0] << 24) |
						(rxMsgAry[n].Data[1] << 16) |
						(rxMsgAry[n].Data[2] << 8) |
						(rxMsgAry[n].Data[3] << 0));
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportFreezeFrameData[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[7 + m];
					}
				}
			}
		}
		return true;
	}
	else if (J1850PWM == m_dwProtocolId)
	{
		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = J1850PWM;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[0] = 0x61;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x02;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);
			TxPTMsg.Data[5] = 0x00;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//41 6B 10 42 RR 00 XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize >= 10) &&
					(0x42 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportFreezeFrameData[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[6 + m];
					}
				}
			}
		}
		return true;
	}
	else if (J1850VPW == m_dwProtocolId)
	{
		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = J1850VPW;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x02;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);
			TxPTMsg.Data[5] = 0x00;

			rxMsgAry.clear();
			lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 42 RR 00 XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize >= 10) &&
					(0x42 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportFreezeFrameData[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[6 + m];
					}
				}
			}
		}
		return true;
	}
	else if (ISO9141 == m_dwProtocolId)
	{
		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x02;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);
			TxPTMsg.Data[5] = 0x00;

			rxMsgAry.clear();
			lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			//48 6B 10 42 RR 00 XX XX XX XX
			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				if ((rxMsgAry[n].DataSize == 10) &&
					(0x42 == rxMsgAry[n].Data[3]) &&
					(TxPTMsg.Data[4] == rxMsgAry[n].Data[4]) &&
					(TxPTMsg.Data[5] == rxMsgAry[n].Data[5]))
				{
					nEcuIndex = GetEcuIndexByID(rxMsgAry[n].Data[2]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportFreezeFrameData[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[6 + m];
					}
				}
			}
		}
		return true;
	}
	else if (ISO14230 == m_dwProtocolId)
	{
		unsigned char byRespData[128];
		int nRespLen = 0;

		for (i = 0; i < 8; i++)
		{
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 6;
			TxPTMsg.ExtraDataIndex = 6;
			TxPTMsg.Data[0] = 0xC3;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x02;
			TxPTMsg.Data[4] = (unsigned char)(i * 0x20);
			TxPTMsg.Data[5] = 0x00;

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR != lRetCode)
			{
				break;
			}

			for (n = 0; n < (int)rxMsgAry.size(); n++)
			{
				//10 41 RR 00 XX XX XX XX
				if ((ISO14230ParseResponse(rxMsgAry[n].Data, rxMsgAry[n].DataSize, byRespData, &nRespLen)) &&
					(8 == nRespLen) &&
					(0x41 == byRespData[1]) &&
					(TxPTMsg.Data[4] == byRespData[2]) &&
					(TxPTMsg.Data[5] == byRespData[3]))
				{
					nEcuIndex = GetEcuIndexByID(byRespData[0]);
					if (nEcuIndex < 0)
					{
						return false;
					}
					for (m = 0; m < 4; m++)
					{
						m_vectSupportFreezeFrameData[nEcuIndex][(i * 4) + m] = rxMsgAry[n].Data[4 + m];
					}
				}
			}
		}
		return true;
	}

	return false;
}

bool CVehicleComm::ScanForChannel(void)
{
	int i = 0;
	int m = 0;
	int n = 0;
	long lRetCode = STATUS_NOERROR;
	static PASSTHRU_MSG TxPTMsg;
	std::vector <PASSTHRU_MSG> rxMsgAry;
	unsigned long dwyBaudrateRecord[2] = { 500000, 250000, };
	unsigned long dwBaseEcuId = 0;
	std::vector<unsigned char> vectSupportRec;
	int nEcuIndex = 0;

	if (m_ptrJ2534Chan.get())
	{
		m_ptrJ2534Chan.reset();
	}
	m_vectEcuID.clear();
	m_vectEcuKeyword.clear();
	m_vectSupportCurrentData.clear();

	//11bit
	for (m = 0; m < 2; m++)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Chan = m_ptrJ2534Dev->connect(ISO15765, 0, dwyBaudrateRecord[m]);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
		::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
		::memset(&FCPTMsg, 0, sizeof(PASSTHRU_MSG));
		::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

		dwBaseEcuId = 0x7E0;

		for (i = 0; i < 8; i++, dwBaseEcuId++)
		{
			MaskPTMsg.ProtocolID = ISO15765;
			MaskPTMsg.TxFlags = ISO15765_FRAME_PAD;
			MaskPTMsg.DataSize = 4;
			MaskPTMsg.ExtraDataIndex = 4;
			MaskPTMsg.Data[0] = 0xFF;
			MaskPTMsg.Data[1] = 0xFF;
			MaskPTMsg.Data[2] = 0xFF;
			MaskPTMsg.Data[3] = 0xFF;

			PatternPTMsg.ProtocolID = ISO15765;
			PatternPTMsg.TxFlags = ISO15765_FRAME_PAD;
			PatternPTMsg.DataSize = 4;
			PatternPTMsg.ExtraDataIndex = 4;
			PatternPTMsg.Data[0] = (unsigned char)((dwBaseEcuId + 8) >> 24);
			PatternPTMsg.Data[1] = (unsigned char)((dwBaseEcuId + 8) >> 16);
			PatternPTMsg.Data[2] = (unsigned char)((dwBaseEcuId + 8) >> 8);
			PatternPTMsg.Data[3] = (unsigned char)((dwBaseEcuId + 8) >> 0);

			FCPTMsg.ProtocolID = ISO15765;
			FCPTMsg.TxFlags = ISO15765_FRAME_PAD;
			FCPTMsg.DataSize = 4;
			FCPTMsg.ExtraDataIndex = 4;
			FCPTMsg.Data[0] = (unsigned char)(dwBaseEcuId >> 24);
			FCPTMsg.Data[1] = (unsigned char)(dwBaseEcuId >> 16);
			FCPTMsg.Data[2] = (unsigned char)(dwBaseEcuId >> 8);
			FCPTMsg.Data[3] = (unsigned char)(dwBaseEcuId >> 0);

			try
			{
				lRetCode = STATUS_NOERROR;
				m_ptrJ2534Chan->startMsgFilter(FLOW_CONTROL_FILTER, &MaskPTMsg, &PatternPTMsg, &FCPTMsg);
			}
			catch (J2534FunctionException& exception)
			{
				lRetCode = exception.code();
			}
			if (STATUS_NOERROR != lRetCode)
			{
				m_ptrJ2534Chan.reset();
				return false;
			}
		}

		rxMsgAry.clear();
		TxPTMsg.ProtocolID = ISO15765;
		TxPTMsg.TxFlags = ISO15765_FRAME_PAD;
		TxPTMsg.DataSize = 6;
		TxPTMsg.ExtraDataIndex = 6;
		TxPTMsg.Data[0] = 0x00;
		TxPTMsg.Data[1] = 0x00;
		TxPTMsg.Data[2] = 0x07;
		TxPTMsg.Data[3] = 0xDF;
		TxPTMsg.Data[4] = 0x01;
		TxPTMsg.Data[5] = 0x00;

		lRetCode = ISO15765_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				//00 00 07 E8 41 00 XX XX XX XX
				if ((rxMsgAry[i].DataSize != 10) || (0x41 != rxMsgAry[i].Data[4]) || (0x00 != rxMsgAry[i].Data[5]))
				{
					break;
				}
				m_vectEcuID.push_back((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));

				vectSupportRec.clear();
				for (m = 0; m < 32; m++)
				{
					vectSupportRec.push_back(0);
				}
				for (m = 0; m < (int)(rxMsgAry[i].DataSize - 6); m++)
				{
					vectSupportRec[m] = rxMsgAry[i].Data[6 + m];
				}
				m_vectSupportCurrentData.push_back(vectSupportRec);
			}

			if (i == (int)rxMsgAry.size())
			{
				m_dwProtocolId = ISO15765;
				m_dwBaudrate = dwyBaudrateRecord[m];
				m_bIsExtCANID = false;
				return true;
			}
			m_vectEcuID.clear();
			m_vectSupportCurrentData.clear();
		}
		m_ptrJ2534Chan.reset();
	}
	
	//29bit
	for (m = 0; m < 2; m++)
	{
		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Chan = m_ptrJ2534Dev->connect(CAN, CAN_29BIT_ID, dwyBaudrateRecord[m]);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if (STATUS_NOERROR != lRetCode)
		{
			return false;
		}

		::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
		::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
		::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

		dwBaseEcuId = 0x18DAF100;

		MaskPTMsg.ProtocolID = CAN;
		MaskPTMsg.TxFlags = CAN_29BIT_ID;
		MaskPTMsg.DataSize = 4;
		MaskPTMsg.ExtraDataIndex = 4;
		MaskPTMsg.Data[0] = 0xFF;
		MaskPTMsg.Data[1] = 0xFF;
		MaskPTMsg.Data[2] = 0xFF;
		MaskPTMsg.Data[3] = 0x00;

		PatternPTMsg.ProtocolID = CAN;
		PatternPTMsg.TxFlags = CAN_29BIT_ID;
		PatternPTMsg.DataSize = 4;
		PatternPTMsg.ExtraDataIndex = 4;
		PatternPTMsg.Data[0] = (unsigned char)(dwBaseEcuId >> 24);
		PatternPTMsg.Data[1] = (unsigned char)(dwBaseEcuId >> 16);
		PatternPTMsg.Data[2] = (unsigned char)(dwBaseEcuId >> 8);
		PatternPTMsg.Data[3] = (unsigned char)(dwBaseEcuId >> 0);

		try
		{
			lRetCode = STATUS_NOERROR;
			m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
		}
		catch (J2534FunctionException& exception)
		{
			lRetCode = exception.code();
		}
		if (STATUS_NOERROR != lRetCode)
		{
			m_ptrJ2534Chan.reset();
			return false;
		}

		rxMsgAry.clear();
		TxPTMsg.ProtocolID = CAN;
		TxPTMsg.TxFlags = CAN_29BIT_ID;
		TxPTMsg.DataSize = 12;
		TxPTMsg.ExtraDataIndex = 12;
		TxPTMsg.Data[0] = 0x18;
		TxPTMsg.Data[1] = 0xDB;
		TxPTMsg.Data[2] = 0x33;
		TxPTMsg.Data[3] = 0xF1;
		TxPTMsg.Data[4] = 0x02;
		TxPTMsg.Data[5] = 0x01;
		TxPTMsg.Data[6] = 0x00;
		TxPTMsg.Data[7] = 0x00;
		TxPTMsg.Data[8] = 0x00;

		lRetCode = CAN_SendRecv_U(TxPTMsg, rxMsgAry);
		if (STATUS_NOERROR == lRetCode)
		{
			for (i = 0; i < (int)rxMsgAry.size(); i++)
			{
				//18 DA F1 10 06 41 00 XX XX XX XX
				if ((rxMsgAry[i].DataSize < 11) || 
					(0x06 != rxMsgAry[i].Data[4]) || 
					(0x41 != rxMsgAry[i].Data[5]) || 
					(0x00 != rxMsgAry[i].Data[6]))
				{
					break;
				}
				m_vectEcuID.push_back((rxMsgAry[i].Data[0] << 24) | (rxMsgAry[i].Data[1] << 16) | (rxMsgAry[i].Data[2] << 8) | (rxMsgAry[i].Data[3] << 0));
				vectSupportRec.clear();
				for (n = 0; n < 32; n++)
				{
					vectSupportRec.push_back(0);
				}
				for (n = 0; n < (int)(rxMsgAry[i].DataSize - 7); n++)
				{
					vectSupportRec[n] = rxMsgAry[i].Data[7 + n];
				}
				m_vectSupportCurrentData.push_back(vectSupportRec);
			}

			if (i == (int)rxMsgAry.size())
			{
				m_ptrJ2534Chan.reset();

				try
				{
					lRetCode = STATUS_NOERROR;
					m_ptrJ2534Chan = m_ptrJ2534Dev->connect(ISO15765, CAN_29BIT_ID, dwyBaudrateRecord[m]);
				}
				catch (J2534FunctionException& exception)
				{
					lRetCode = exception.code();
				}
				if (STATUS_NOERROR != lRetCode)
				{
					return false;
				}

				for (i = 0; i < (int)m_vectEcuID.size(); i++)
				{
					dwBaseEcuId = m_vectEcuID[i];

					::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
					::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
					::memset(&FCPTMsg, 0, sizeof(PASSTHRU_MSG));

					MaskPTMsg.ProtocolID = ISO15765;
					MaskPTMsg.TxFlags = CAN_29BIT_ID | ISO15765_FRAME_PAD;
					MaskPTMsg.DataSize = 4;
					MaskPTMsg.ExtraDataIndex = 4;
					MaskPTMsg.Data[0] = 0xFF;
					MaskPTMsg.Data[1] = 0xFF;
					MaskPTMsg.Data[2] = 0xFF;
					MaskPTMsg.Data[3] = 0xFF;

					PatternPTMsg.ProtocolID = ISO15765;
					PatternPTMsg.TxFlags = CAN_29BIT_ID | ISO15765_FRAME_PAD;
					PatternPTMsg.DataSize = 4;
					PatternPTMsg.ExtraDataIndex = 4;
					PatternPTMsg.Data[0] = (unsigned char)(dwBaseEcuId >> 24);
					PatternPTMsg.Data[1] = (unsigned char)(dwBaseEcuId >> 16);
					PatternPTMsg.Data[2] = (unsigned char)(dwBaseEcuId >> 8);
					PatternPTMsg.Data[3] = (unsigned char)(dwBaseEcuId >> 0);

					FCPTMsg.ProtocolID = ISO15765;
					FCPTMsg.TxFlags = CAN_29BIT_ID | ISO15765_FRAME_PAD;
					FCPTMsg.DataSize = 4;
					FCPTMsg.ExtraDataIndex = 4;
					FCPTMsg.Data[0] = (unsigned char)(dwBaseEcuId >> 24);
					FCPTMsg.Data[1] = (unsigned char)(dwBaseEcuId >> 16);
					FCPTMsg.Data[2] = (unsigned char)(dwBaseEcuId >> 0);
					FCPTMsg.Data[3] = (unsigned char)(dwBaseEcuId >> 8);

					try
					{
						lRetCode = STATUS_NOERROR;
						m_ptrJ2534Chan->startMsgFilter(FLOW_CONTROL_FILTER, &MaskPTMsg, &PatternPTMsg, &FCPTMsg);
					}
					catch (J2534FunctionException& exception)
					{
						lRetCode = exception.code();
					}
					if (STATUS_NOERROR != lRetCode)
					{
						m_ptrJ2534Chan.reset();
						return false;
					}
				}

				m_dwProtocolId = ISO15765;
				m_dwBaudrate = dwyBaudrateRecord[m];
				m_bIsExtCANID = true;
				return true;
			}
			m_vectEcuID.clear();
			m_vectSupportCurrentData.clear();
		}
		m_ptrJ2534Chan.reset();
	}

	unsigned long dwBatteryVolt = 0;

	if ((!GetBatteryVoltage(dwBatteryVolt)) || (dwBatteryVolt < 8000))
	{
		return false;
	}

	//PWM
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan = m_ptrJ2534Dev->connect(J1850PWM, 0, 41600);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}

	SCONFIG CfgItems[5];
	SCONFIG_LIST Input;

	CfgItems[0].Parameter = NODE_ADDRESS;
	CfgItems[0].Value = 0xF1;

	Input.NumOfParams = 1;
	Input.ConfigPtr = &CfgItems[0];

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->ioctl(SET_CONFIG, &Input, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	SBYTE_ARRAY InputMsg;
	unsigned char FuncAddr[4]; // Functional address array ¨C address values defined in J2178-4

	FuncAddr[0] = 0x6B;

	InputMsg.NumOfBytes = 1; // Functional address array contains one address.
	InputMsg.BytePtr = &FuncAddr[0]; // Assign pointer to functional address array.

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->ioctl(ADD_TO_FUNCT_MSG_LOOKUP_TABLE, &InputMsg, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

	MaskPTMsg.ProtocolID = J1850PWM;
	MaskPTMsg.DataSize = 2;
	MaskPTMsg.ExtraDataIndex = 2;
	MaskPTMsg.Data[0] = 0xFF;
	MaskPTMsg.Data[1] = 0xFF;

	PatternPTMsg.ProtocolID = J1850PWM;
	PatternPTMsg.DataSize = 2;
	PatternPTMsg.ExtraDataIndex = 2;
	PatternPTMsg.Data[0] = 0x41;
	PatternPTMsg.Data[1] = 0x6B;

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	rxMsgAry.clear();
	TxPTMsg.ProtocolID = J1850PWM;
	TxPTMsg.DataSize = 5;
	TxPTMsg.ExtraDataIndex = 5;
	TxPTMsg.Data[0] = 0x61;
	TxPTMsg.Data[1] = 0x6A;
	TxPTMsg.Data[2] = 0xF1;
	TxPTMsg.Data[3] = 0x01;
	TxPTMsg.Data[4] = 0x00;

	lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
	if (STATUS_NOERROR == lRetCode)
	{
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			//41 6B 10 41 00 XX XX XX XX
			//41 6B 10 41 00 BE 1F B8 10
			if ((rxMsgAry[i].ExtraDataIndex < 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x00 != rxMsgAry[i].Data[4]))
			{
				break;
			}
			m_vectEcuID.push_back(rxMsgAry[i].Data[2]);

			vectSupportRec.clear();
			for (m = 0; m < 32; m++)
			{
				vectSupportRec.push_back(0);
			}
			for (m = 0; m < (int)(rxMsgAry[i].ExtraDataIndex - 5); m++)
			{
				vectSupportRec[m] = rxMsgAry[i].Data[5 + m];
			}
			m_vectSupportCurrentData.push_back(vectSupportRec);
		}

		if (i == (int)rxMsgAry.size())
		{
			m_dwProtocolId = J1850PWM;
			m_dwBaudrate = 41600;
			return true;
		}
		m_vectEcuID.clear();
		m_vectSupportCurrentData.clear();
	}
	m_ptrJ2534Chan.reset();

	//VPW
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan = m_ptrJ2534Dev->connect(J1850VPW, 0, 10400);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}

	::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

	MaskPTMsg.ProtocolID = J1850VPW;
	MaskPTMsg.DataSize = 2;
	MaskPTMsg.ExtraDataIndex = 2;
	MaskPTMsg.Data[0] = 0xFF;
	MaskPTMsg.Data[1] = 0xFF;

	PatternPTMsg.ProtocolID = J1850VPW;
	PatternPTMsg.DataSize = 2;
	PatternPTMsg.ExtraDataIndex = 2;
	PatternPTMsg.Data[0] = 0x48;
	PatternPTMsg.Data[1] = 0x6B;

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	rxMsgAry.clear();
	TxPTMsg.ProtocolID = J1850VPW;
	TxPTMsg.DataSize = 5;
	TxPTMsg.ExtraDataIndex = 5;
	TxPTMsg.Data[0] = 0x68;
	TxPTMsg.Data[1] = 0x6A;
	TxPTMsg.Data[2] = 0xF1;
	TxPTMsg.Data[3] = 0x01;
	TxPTMsg.Data[4] = 0x00;

	lRetCode = J1850_SendRecv_U(TxPTMsg, rxMsgAry);
	if (STATUS_NOERROR == lRetCode)
	{
		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			//48 6B 10 41 00 XX XX XX XX
			if ((rxMsgAry[i].DataSize < 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x00 != rxMsgAry[i].Data[4]))
			{
				break;
			}
			m_vectEcuID.push_back(rxMsgAry[i].Data[2]);
			vectSupportRec.clear();
			for (m = 0; m < 32; m++)
			{
				vectSupportRec.push_back(0);
			}
			for (m = 0; m < (int)(rxMsgAry[i].DataSize - 5); m++)
			{
				vectSupportRec[m] = rxMsgAry[i].Data[5 + m];
			}
			m_vectSupportCurrentData.push_back(vectSupportRec);
		}

		if (i == (int)rxMsgAry.size())
		{
			m_dwProtocolId = J1850VPW;
			m_dwBaudrate = 10400;
			return true;
		}
		m_vectEcuID.clear();
		m_vectSupportCurrentData.clear();
	}
	m_ptrJ2534Chan.reset();

	//ISO9141
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan = m_ptrJ2534Dev->connect(ISO9141, 0, 10400);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}

	::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

	MaskPTMsg.ProtocolID = ISO9141;
	MaskPTMsg.DataSize = 2;
	MaskPTMsg.ExtraDataIndex = 2;
	MaskPTMsg.Data[0] = 0xFF;
	MaskPTMsg.Data[1] = 0xFF;

	PatternPTMsg.ProtocolID = ISO9141;
	PatternPTMsg.DataSize = 2;
	PatternPTMsg.ExtraDataIndex = 2;
	PatternPTMsg.Data[0] = 0x48;
	PatternPTMsg.Data[1] = 0x6B;

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	SBYTE_ARRAY OutputMsg;
	unsigned char EcuAddr[1];
	unsigned char KeyWord[2];
	unsigned char byRespData[128];
	int nRespLen = 0;

	EcuAddr[0] = 0x33;
	InputMsg.NumOfBytes = 1;
	InputMsg.BytePtr = &EcuAddr[0];
	OutputMsg.NumOfBytes = 0;
	OutputMsg.BytePtr = &KeyWord[0];
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->ioctl(FIVE_BAUD_INIT, &InputMsg, &OutputMsg);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if ((STATUS_NOERROR == lRetCode) && (2 == OutputMsg.NumOfBytes))
	{
		rxMsgAry.clear();
		if (((0x08 == KeyWord[0]) && (0x08 == KeyWord[1])) || ((0x94 == KeyWord[0]) && (0x94 == KeyWord[1]))) //ISO9141
		{
			TxPTMsg.ProtocolID = ISO9141;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0x68;
			TxPTMsg.Data[1] = 0x6A;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x00;

			lRetCode = ISO9141_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR == lRetCode)
			{
				//48 6B 10 41 00 XX XX XX XX
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					if ((rxMsgAry[i].DataSize != 9) || (0x41 != rxMsgAry[i].Data[3]) || (0x00 != rxMsgAry[i].Data[4]))
					{
						break;
					}
					::memcpy(&MaskPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
					::memcpy(&PatternPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
					::memset(&(MaskPTMsg.Data[0]), 0xFF, rxMsgAry[i].DataSize);

					try
					{
						lRetCode = STATUS_NOERROR;
						m_ptrJ2534Chan->startMsgFilter(BLOCK_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
					}
					catch (J2534FunctionException& exception)
					{
						lRetCode = exception.code();
					}
					if (STATUS_NOERROR != lRetCode)
					{
						m_ptrJ2534Chan.reset();
						return false;
					}
				}

				if (i == (int)rxMsgAry.size())
				{
					try
					{
						lRetCode = STATUS_NOERROR;
						m_dwPrdcMsgID = m_ptrJ2534Chan->startPeriodicMsg(&TxPTMsg, 3000);
					}
					catch (J2534FunctionException& exception)
					{
						lRetCode = exception.code();
					}

					if (STATUS_NOERROR == lRetCode)
					{
						for (i = 0; i < (int)rxMsgAry.size(); i++)
						{
							m_vectEcuID.push_back(rxMsgAry[i].Data[2]);

							vectSupportRec.clear();
							for (m = 0; m < 32; m++)
							{
								vectSupportRec.push_back(0);
							}
							for (m = 0; m < (int)(rxMsgAry[i].DataSize - 5); m++)
							{
								vectSupportRec[m] = rxMsgAry[i].Data[5 + m];
							}
							m_vectSupportCurrentData.push_back(vectSupportRec);
						}

						m_vectEcuKeyword.push_back((KeyWord[0] << 8) | KeyWord[1]);
						m_dwProtocolId = ISO9141;
						m_dwBaudrate = 10400;
						return true;
					}
				}
				m_vectEcuID.clear();
				m_vectSupportCurrentData.clear();
			}
		}
		else
		{
			//ISO14230 SlowInit
			m_ptrJ2534Chan.reset(); //disconnect ISO9141

			try
			{
				lRetCode = STATUS_NOERROR;
				m_ptrJ2534Chan = m_ptrJ2534Dev->connect(ISO14230, 0, 10400);
			}
			catch (J2534FunctionException& exception)
			{
				lRetCode = exception.code();
			}
			if (STATUS_NOERROR != lRetCode)
			{
				return false;
			}

			::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
			::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
			::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

			MaskPTMsg.ProtocolID = ISO14230;
			MaskPTMsg.DataSize = 2;
			MaskPTMsg.ExtraDataIndex = 2;
			MaskPTMsg.Data[0] = 0xC0;
			MaskPTMsg.Data[1] = 0xFF;

			PatternPTMsg.ProtocolID = ISO14230;
			PatternPTMsg.DataSize = 2;
			PatternPTMsg.ExtraDataIndex = 2;
			PatternPTMsg.Data[0] = 0x80;
			PatternPTMsg.Data[1] = 0xF1;

			try
			{
				lRetCode = STATUS_NOERROR;
				m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
			}
			catch (J2534FunctionException& exception)
			{
				lRetCode = exception.code();
			}
			if (STATUS_NOERROR != lRetCode)
			{
				m_ptrJ2534Chan.reset();
				return false;
			}

			//01 00
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x00;

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
			if (STATUS_NOERROR == lRetCode)
			{
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					//10 41 00 XX XX XX XX
					if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
						(7 == nRespLen) &&
						(0x41 == byRespData[1]) &&
						(0x00 == byRespData[2]))
					{
						m_vectEcuID.push_back(byRespData[0]);
						vectSupportRec.clear();
						for (m = 0; m < 32; m++)
						{
							vectSupportRec.push_back(0);
						}
						for (m = 0; m < nRespLen - 3; m++)
						{
							vectSupportRec[m] = byRespData[3 + m];
						}
						m_vectSupportCurrentData.push_back(vectSupportRec);
					}
					else
					{
						break;
					}
				}


				if (i == (int)rxMsgAry.size())
				{
					TxPTMsg.ProtocolID = ISO14230;
					TxPTMsg.DataSize = 4;
					TxPTMsg.ExtraDataIndex = 4;
					TxPTMsg.Data[0] = 0xC1;
					TxPTMsg.Data[1] = 0x33;
					TxPTMsg.Data[2] = 0xF1;
					TxPTMsg.Data[3] = 0x3E;

					rxMsgAry.clear();
					lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
					if (STATUS_NOERROR == lRetCode)
					{
						for (i = 0; i < (int)rxMsgAry.size(); i++)
						{
							::memcpy(&MaskPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
							::memcpy(&PatternPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
							::memset(&MaskPTMsg.Data[0], 0xFF, rxMsgAry[i].DataSize);

							try
							{
								lRetCode = STATUS_NOERROR;
								m_ptrJ2534Chan->startMsgFilter(BLOCK_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
							}
							catch (J2534FunctionException& exception)
							{
								lRetCode = exception.code();
							}
							if (STATUS_NOERROR != lRetCode)
							{
								m_ptrJ2534Chan.reset();
								return false;
							}
						}

						try
						{
							lRetCode = STATUS_NOERROR;
							m_dwPrdcMsgID = m_ptrJ2534Chan->startPeriodicMsg(&TxPTMsg, 3000);
						}
						catch (J2534FunctionException& exception)
						{
							lRetCode = exception.code();
						}

						if (STATUS_NOERROR == lRetCode)
						{
							m_vectEcuKeyword.push_back((KeyWord[0] << 8) | KeyWord[1]);
							m_dwProtocolId = ISO14230;
							m_dwBaudrate = 10400;
							return true;
						}
					}
				}
				m_vectEcuID.clear();
				m_vectSupportCurrentData.clear();
			}
		}
	}
	m_ptrJ2534Chan.reset();

	//ISO14230 FastInit
	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan = m_ptrJ2534Dev->connect(ISO14230, 0, 10400);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		return false;
	}

	::memset(&MaskPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
	::memset(&TxPTMsg, 0, sizeof(PASSTHRU_MSG));

	MaskPTMsg.ProtocolID = ISO14230;
	MaskPTMsg.DataSize = 2;
	MaskPTMsg.ExtraDataIndex = 2;
	MaskPTMsg.Data[0] = 0xC0;
	MaskPTMsg.Data[1] = 0xFF;

	PatternPTMsg.ProtocolID = ISO14230;
	PatternPTMsg.DataSize = 2;
	PatternPTMsg.ExtraDataIndex = 2;
	PatternPTMsg.Data[0] = 0x80;
	PatternPTMsg.Data[1] = 0xF1;

	try
	{
		lRetCode = STATUS_NOERROR;
		m_ptrJ2534Chan->startMsgFilter(PASS_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}
	if (STATUS_NOERROR != lRetCode)
	{
		m_ptrJ2534Chan.reset();
		return false;
	}

	TxPTMsg.ProtocolID = ISO14230;
	TxPTMsg.DataSize = 4;
	TxPTMsg.ExtraDataIndex = 4;
	TxPTMsg.Data[0] = 0xC1;
	TxPTMsg.Data[1] = 0x33;
	TxPTMsg.Data[2] = 0xF1;
	TxPTMsg.Data[3] = 0x81;

	try
	{
		lRetCode = STATUS_NOERROR;
		::memset(&PatternPTMsg, 0, sizeof(PASSTHRU_MSG));
		m_ptrJ2534Chan->ioctl(FAST_INIT, &TxPTMsg, &PatternPTMsg);
	}
	catch (J2534FunctionException& exception)
	{
		lRetCode = exception.code();
	}

	if ((STATUS_NOERROR == lRetCode) && (ISO14230ParseResponse(PatternPTMsg.Data, PatternPTMsg.DataSize, byRespData, &nRespLen)) && (4 == nRespLen))
	{
		rxMsgAry.clear();
		//receive only
		TxPTMsg.DataSize = 0;
		ISO14230_SendRecv_U(TxPTMsg, rxMsgAry);
		if (rxMsgAry.empty())
		{
			rxMsgAry.push_back(PatternPTMsg);
		}
		else
		{
			rxMsgAry.insert(rxMsgAry.begin(), PatternPTMsg);
		}

		for (i = 0; i < (int)rxMsgAry.size(); i++)
		{
			//10 C1 XX XX
			if ((ISO14230ParseResponse(PatternPTMsg.Data, PatternPTMsg.DataSize, byRespData, &nRespLen)) && (4 == nRespLen) && (0xC1 == byRespData[1]))
			{
				m_vectEcuKeyword.push_back((byRespData[2] << 8) | byRespData[3]);
				m_vectEcuID.push_back(byRespData[0]);
				vectSupportRec.clear();
				for (m = 0; m < 32; m++)
				{
					vectSupportRec.push_back(0);
				}
				m_vectSupportCurrentData.push_back(vectSupportRec);
			}
			else
			{
				break;
			}
		}

		if (i == (int)rxMsgAry.size())
		{
			//01 00
			TxPTMsg.ProtocolID = ISO14230;
			TxPTMsg.DataSize = 5;
			TxPTMsg.ExtraDataIndex = 5;
			TxPTMsg.Data[0] = 0xC2;
			TxPTMsg.Data[1] = 0x33;
			TxPTMsg.Data[2] = 0xF1;
			TxPTMsg.Data[3] = 0x01;
			TxPTMsg.Data[4] = 0x00;

			rxMsgAry.clear();
			lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
			if (STATUS_NOERROR == lRetCode)
			{
				for (i = 0; i < (int)rxMsgAry.size(); i++)
				{
					//10 41 00 XX XX XX XX
					if ((ISO14230ParseResponse(rxMsgAry[i].Data, rxMsgAry[i].DataSize, byRespData, &nRespLen)) &&
						(7 == nRespLen) &&
						(0x41 == byRespData[1]) &&
						(0x00 == byRespData[2]))
					{
						nEcuIndex = GetEcuIndexByID(byRespData[0]);
						if (nEcuIndex >= 0)
						{
							for (m = 0; m < nRespLen - 3; m++)
							{
								m_vectSupportCurrentData[nEcuIndex][m] = byRespData[3 + m];
							}
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				if (i == (int)rxMsgAry.size())
				{
					TxPTMsg.ProtocolID = ISO14230;
					TxPTMsg.DataSize = 4;
					TxPTMsg.ExtraDataIndex = 4;
					TxPTMsg.Data[0] = 0xC1;
					TxPTMsg.Data[1] = 0x33;
					TxPTMsg.Data[2] = 0xF1;
					TxPTMsg.Data[3] = 0x3E;

					rxMsgAry.clear();
					lRetCode = ISO14230_SendRecv_N(TxPTMsg, rxMsgAry, (unsigned long)m_vectEcuID.size());
					if (STATUS_NOERROR == lRetCode)
					{
						for (i = 0; i < (int)rxMsgAry.size(); i++)
						{
							::memcpy(&MaskPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
							::memcpy(&PatternPTMsg, &rxMsgAry[i], sizeof(PASSTHRU_MSG));
							::memset(&MaskPTMsg.Data[0], 0xFF, rxMsgAry[i].DataSize);

							try
							{
								lRetCode = STATUS_NOERROR;
								m_ptrJ2534Chan->startMsgFilter(BLOCK_FILTER, &MaskPTMsg, &PatternPTMsg, NULL);
							}
							catch (J2534FunctionException& exception)
							{
								lRetCode = exception.code();
							}
							if (STATUS_NOERROR != lRetCode)
							{
								m_ptrJ2534Chan.reset();
								return false;
							}
						}

						try
						{
							lRetCode = STATUS_NOERROR;
							m_dwPrdcMsgID = m_ptrJ2534Chan->startPeriodicMsg(&TxPTMsg, 3000);
						}
						catch (J2534FunctionException& exception)
						{
							lRetCode = exception.code();
						}

						if (STATUS_NOERROR == lRetCode)
						{
							m_dwProtocolId = ISO14230;
							m_dwBaudrate = 10400;
							return true;
						}
					}
				}
				else
				{
					m_vectSupportCurrentData.clear();
				}
			}
		}
		m_vectEcuID.clear();
		m_vectEcuKeyword.clear();
	}
	m_ptrJ2534Chan.reset();
	return false;
}