#pragma once
#include "j2534.h"
#include "framework.h"
#include <map>
#include "CVehicleDatabase.h"

typedef struct tagINFO_TYPE
{
	int ecuIndex;
	unsigned char infoType;
	std::vector<unsigned char> infoData;

} T_INFO_TYPE;

class CVehicleComm
{
private:
	J2534LibraryPtr m_ptrJ2534Lib;
	J2534DevicePtr m_ptrJ2534Dev;
	J2534ChannelPtr m_ptrJ2534Chan;
	unsigned long m_dwProtocolId;
	unsigned long m_dwBaudrate;
	bool m_bIsExtCANID;
	std::vector<unsigned long> m_vectEcuID;
	std::vector<unsigned long> m_vectEcuKeyword;
	J2534Channel::PeriodicMessage m_dwPrdcMsgID;
	std::vector<std::vector<unsigned char>> m_vectSupportCurrentData;
	std::vector<std::vector<unsigned char>> m_vectSupportFreezeFrameData;
	std::vector<std::vector<unsigned char>> m_vectSupportInfoType;
	std::vector<std::vector<unsigned long>> m_vectEcuConfirmedDtc;
	std::vector<std::vector<unsigned long>> m_vectEcuPendingDtc;
	std::vector<std::vector<unsigned long>> m_vectEcuPermanentDtc;
	std::vector<std::map<unsigned long, std::vector<unsigned char>>> m_vectCurrentDataValue;

	int GetEcuIndexByID(unsigned long dwID) const;
	bool ReadDataValue(unsigned long dwPID, unsigned long dwNumberOfMsgToRx, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values);

public:
	CVehicleComm();
	~CVehicleComm();
public:
	bool IsChannelConnected(void);
	const std::vector<unsigned long>& GetEcuID(void) const;
	bool GetCurrentDataValue(int nEcuIndex, std::map<unsigned long, std::vector<unsigned char>> & values) const;
	bool GetConfirmedDtc(int nEcuIndex, std::vector<unsigned long> & dtcs) const;
	bool GetPendingDtc(int nEcuIndex, std::vector<unsigned long>& dtcs) const;
	bool GetPermanentDtc(int nEcuIndex, std::vector<unsigned long>& dtcs) const;
	const CDuiString GetProtocolName(void) const;
	bool GetBatteryVoltage(unsigned long & dwVoltage_mV);
	unsigned long GetECUIDLength(void) const;
	bool LoadPassThruLibrary(void);
	bool ConnectDevice(void);
	void DisconnectDevice(void);
	bool ScanForChannel(void);
	bool ScanForDtc(void);
	bool ScanForSupportCurrentData(void);
	bool ScanForCurrentDataValue(void);
	bool ScanForSupportFreezeFrameData(void);
	bool ScanForSupportInfoType(void);
	bool ReadInfoTypes(std::vector <T_INFO_TYPE> & tInfoTypes);
	bool ReadDataValue(unsigned long dwPID, unsigned long dwNumberOfMsgToRx);
	bool ReadDataValues(std::vector<std::vector<DD_PARAM_REC>> & vectDataList, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values);
	bool ReadMultipleDataValue(unsigned long dwNumberOfPID, unsigned long * pdwPID, unsigned long* pdwLength, unsigned long * pdwNumberOfMsgToRx, unsigned long flag, std::vector<std::map<unsigned long, std::vector<unsigned char>>>& values);

	long CAN_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry);
	long ISO15765_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry);
	long J1850_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry);
	long ISO9141_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry);
	long ISO14230_SendRecv_U(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry);

	long J1850_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx);
	long ISO9141_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx);
	long ISO14230_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx);
	long ISO15765_SendRecv_N(PASSTHRU_MSG& txMsg, std::vector <PASSTHRU_MSG>& rxMsgAry, unsigned long dwNumberOfMsgToRx);
};

extern std::unique_ptr<CVehicleComm> theVehicleComm;