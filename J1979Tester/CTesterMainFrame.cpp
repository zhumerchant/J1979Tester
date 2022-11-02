#include "CTesterMainFrame.h"

CTesterMainFrame::CTesterMainFrame()
{
    m_pCloseBtn = NULL;
    m_pMinBtn = NULL;

    m_nCurWndIndex = 0;
    memset(m_pSubWnds, 0, sizeof(m_pSubWnds));
}
