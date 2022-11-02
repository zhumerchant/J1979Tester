#include "CVehicleMonFrame.h"

CVehicleMonFrame::CVehicleMonFrame()
{
    m_nCurWndIndex = 0;
    memset(m_pSubWnds, 0, sizeof(m_pSubWnds));
}
