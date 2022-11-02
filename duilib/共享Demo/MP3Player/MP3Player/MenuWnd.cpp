#include "StdAfx.h"
#include "resource.h"
#include "MenuWnd.h"
#include "SysConfig.h"

CMenuWnd::CMenuWnd(int nData,CDuiString xmlFileName) : m_pOwner(NULL) 
{
   m_XmlFileName = xmlFileName;
   m_Data = nData;
}

int  CMenuWnd::getData()
{
   return m_Data;
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
	return _T("MenuWnd");
} 

LONG CMenuWnd::GetStyle() const
{ 
	LONG styleValue = __super::GetStyle();

	styleValue &= ~WS_MAXIMIZEBOX;
	styleValue &= ~WS_SIZEBOX; 

	return styleValue|WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
} 


UINT CMenuWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; 
} 


UILIB_RESOURCETYPE CMenuWnd::GetResourceType() const
{
	return SysConfig::GetResourceType();
}

CDuiString CMenuWnd::GetSkinFile()
{
	return m_XmlFileName.GetData();
}

CDuiString CMenuWnd::GetSkinFolder()
{
	return SysConfig::GetSkinFolder();
} 

CDuiString CMenuWnd::GetZIPFileName() const
{
	return SysConfig::GetZIPFileName();
}

LPCTSTR CMenuWnd::GetResourceID() const
{
	return SysConfig::GetResourceID();
}

void CMenuWnd::Init(WindowImplBase * pOwnerWnd, POINT pt) 
{
        if( pOwnerWnd == NULL ) return;
        m_pOwnerWnd = pOwnerWnd;
        m_ptPos = pt;
 
		 __super::Create(m_pOwnerWnd->GetHWND(), NULL, WS_POPUP, WS_EX_TOOLWINDOW);
        HWND hWndParent = m_hWnd;
        while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
        ::ShowWindow(m_hWnd, SW_SHOW);
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void CMenuWnd::Init(CControlUI* pOwner, POINT pt) 
{
        if( pOwner == NULL ) return;
        m_pOwner = pOwner;
        m_ptPos = pt;
		    __super::Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW);
        HWND hWndParent = m_hWnd;
        while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
        ::ShowWindow(m_hWnd, SW_SHOW);
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void CMenuWnd::AdjustPostion() 
{
        CDuiRect rcWnd;
        GetWindowRect(m_hWnd, &rcWnd);
        int nWidth = rcWnd.GetWidth();
        int nHeight = rcWnd.GetHeight();
        rcWnd.left = m_ptPos.x;
        rcWnd.top = m_ptPos.y;
        rcWnd.right = rcWnd.left + nWidth;
        rcWnd.bottom = rcWnd.top + nHeight;
        MONITORINFO oMonitor ;
        oMonitor.cbSize = sizeof(oMonitor);

		 
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;

        if( rcWnd.bottom > rcWork.bottom ) {
            if( nHeight >= rcWork.GetHeight() ) {
                rcWnd.top = 0;
                rcWnd.bottom = nHeight;
            }
            else {
                rcWnd.bottom = rcWork.bottom;
                rcWnd.top = rcWnd.bottom - nHeight;
            }
        }
        if( rcWnd.right > rcWork.right ) {
            if( nWidth >= rcWork.GetWidth() ) {
                rcWnd.left = 0;
                rcWnd.right = nWidth;
            }
            else {
                rcWnd.right = rcWork.right;
                rcWnd.left = rcWnd.right - nWidth;
            }
        }
        ::SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

     

void CMenuWnd::Notify(TNotifyUI& msg)
{
		
}
 
void CMenuWnd::Init()
{
	  m_PaintManager.SetRoundCorner(3, 3); 
    
	  AdjustPostion();
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
		if( m_hWnd != (HWND) wParam )
		{ 
			//m_Destroying =true;
			PostMessage(WM_CLOSE); 
		}

        return 0;
}

LRESULT CMenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
        if( wParam == VK_ESCAPE ) Close();
        return 0;
}
 