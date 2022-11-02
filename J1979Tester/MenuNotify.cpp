#include "StdAfx.h"
#include "MenuNotify.h"
#include "UIMenu.h"
#include "CTesterMainFrame.h"

extern int g_nLanguageIndex;
extern int g_nTemperatureIndex;
extern int g_nPressureIndex;
extern int g_nDistanceIndex;
extern int g_nVacuumIndex;
extern int g_nMassIndex;

extern BOOL SetUserPreference(int nLanguageIndex,
	int nTemperatureIndex,
	int nPressureIndex,
	int nDistanceIndex,
	int nVacuumIndex,
	int nMassIndex);

extern void ReloadLanguage(void);

CMenuNotify::CMenuNotify(void)
{
}


CMenuNotify::~CMenuNotify(void)
{
}

void CMenuNotify::Notify(TNotifyUI& msg)
{
	if( msg.sType == DUI_MSGTYPE_ITEMCLICK) 
	{
		CMenuElementUI *pMenuElement  = (CMenuElementUI*)msg.pSender;
		CPaintManagerUI *m_pm = pMenuElement->GetManager();
		if (m_pm != NULL)
		{
			CControlUI *pRootControl = m_pm->GetRoot();
			if (pRootControl != NULL)
			{
				CDuiString szMenuName = pMenuElement->GetName();
				if (szMenuName == _T("temp_cel"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						0,
						g_nPressureIndex,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("temp_fah"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						1,
						g_nPressureIndex,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("dist_km"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						0,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("dist_mile"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						1,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("pres_kpa"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						0,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("pres_bar"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						1,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("pres_psi"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						2,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("pres_inh2o"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						3,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
				}
				else if (szMenuName == _T("vacu_kpa"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						0,
						g_nMassIndex);
				}
				else if (szMenuName == _T("vacu_mmhg"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						1,
						g_nMassIndex);
				}
				else if (szMenuName == _T("vacu_bar"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						2,
						g_nMassIndex);
				}
				else if (szMenuName == _T("vacu_inh2o"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						3,
						g_nMassIndex);
				}
				else if (szMenuName == _T("mass_kg"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						g_nVacuumIndex,
						0);
				}
				else if (szMenuName == _T("mass_lb"))
				{
					SetUserPreference(
						g_nLanguageIndex,
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						g_nVacuumIndex,
						1);
				}
				else
				{
					SetUserPreference(
						pMenuElement->GetIndex(),
						g_nTemperatureIndex,
						g_nPressureIndex,
						g_nDistanceIndex,
						g_nVacuumIndex,
						g_nMassIndex);
					ReloadLanguage();
				}
			}
		}
	}
}