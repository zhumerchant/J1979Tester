#include "CTesterMainFrame.h"

CTesterMainFrame::CTesterMainFrame()
{
    m_pCloseBtn = NULL;
    m_pMinBtn = NULL;
    m_pTextBattVolt = NULL;
    m_pTextSoftVer = NULL;
    m_pTextFwVer = NULL;
    m_bGotFwVer = FALSE;

    m_tmrDev = 0;
    m_nCurWndIndex = 0;
    memset(m_pSubWnds, 0, sizeof(m_pSubWnds));
}
