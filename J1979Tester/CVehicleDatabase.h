#pragma once
#include "framework.h"
#include <memory>
#include <vector>
#include <map>
#include <list>

typedef struct tagDTC_REC
{
	CDuiString number;
	CDuiString naming;

} DTC_REC;


typedef struct tagPARAM_ADDR_REC
{
	unsigned long address;
	unsigned long size;

} PARAM_ADDR_REC;

typedef struct tagCONVERTER_STATE_REC
{
	CDuiString name;
	CDuiString stateText;
	double stateLow;
	double stateHigh;

} CONVERTER_STATE_REC;

typedef struct tagCONVERTER_MAP_REC
{
	double fromQuantity;
	double toQuantity;

} CONVERTER_MAP_REC;

typedef struct tagCONVERTER_REC
{
	CDuiString name;
	double offset;
	double multiplier;
	double divisor;
	int oldType;
	int quantityType;
	std::vector<CONVERTER_STATE_REC> states;
	std::vector<CONVERTER_MAP_REC>	maps;

} CONVERTER_REC;

typedef struct tagPARAM_TYPE_REC
{
	unsigned long shift;
	unsigned long mask;
	bool LeftShift;
	PARAM_ADDR_REC address;
	CONVERTER_REC converter;

} PARAM_TYPE_REC;

typedef struct tagDD_PARAM_REC
{
	int ecuIndex;
	CDuiString databasename;
	CDuiString name;
	CDuiString mnemonic;
	int quantityType;
	int displayMode;
	PARAM_TYPE_REC paramRd;
	CDuiString unit;
	double minValue;
	double maxValue;
	double curValue;

} DD_PARAM_REC;


class CVehicleDatabase
{
private:
	TCHAR m_tszLang[5];
	CMarkup m_xmlDocDTC;
	CMarkup m_xmlDocParam;
	CMarkup m_xmlDocMessage;
	std::map<CDuiString, DD_PARAM_REC> m_mapNameToParam;

private:
	bool ParseParameters(void);
	bool LoadMessages(void);
	CDuiString GetParamMnemonic(CDuiString name);
	bool ParseParamConverter(DD_PARAM_REC & dd_param_rec, CDuiString name);
	bool ParseParamConverterState(CONVERTER_STATE_REC & cvs);

public:
	~CVehicleDatabase();
	bool Open(const char * lang);
	bool Close(void);
	bool GetDTC(unsigned long code, DTC_REC & dtc);
	void GetSupportParameters(const std::map<unsigned long, std::vector<unsigned char>> & mapDataValue, std::vector<DD_PARAM_REC>& data_list);
	void CalcDataValues(std::vector<std::vector<DD_PARAM_REC>>& vectDataList,
		std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values,
		std::vector<CDuiString> & texts);
};

extern std::unique_ptr<CVehicleDatabase> theVehicleDb;
extern int g_nLanguageIndex;
extern int g_nTemperatureIndex;
extern int g_nPressureIndex;
extern int g_nDistanceIndex;
extern int g_nVacuumIndex;
extern int g_nMassIndex;
extern BOOL GetUserPreference(void);
extern BOOL SetUserPreference(int nLanguageIndex,
	int nTemperatureIndex,
	int nPressureIndex,
	int nDistanceIndex,
	int nVacuumIndex,
	int nMassIndex);
