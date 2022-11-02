#ifndef _PlayListPopMenu_H
#define _PlayListPopMenu_H

#include "MenuWnd.h"
#include "WinImplBase.hpp"

class PlayListPopMenu : public CMenuWnd
{
public:
	PlayListPopMenu(int nData):CMenuWnd(nData,_T("menu.xml"))
	{ 
		
	}

    ~PlayListPopMenu()
	{

	}
public:
    void SetData(const char *pData)
	{
        data = pData;
	}

	void Popup(WindowImplBase * pOwner, POINT pt)
	{
		mPOwner = pOwner;
		CMenuWnd::Init(pOwner, pt);  
	}

	void Notify(TNotifyUI& msg)
	{
		switch(msg.iType)
		{
			case NOTIFY_ITEMSELECT_VALUE:
				{
					Close();
				}
				break;
			case NOTIFY_ITEMCLICK_VALUE:
				{ 
					if( msg.pSender->GetName() == _T("menu_Open") ) 
					{
						TNotifyUI notify;
						notify.pSender = msg.pSender;
						notify.iType   = POPUPMENU_OPENDIR_CLICK;
						notify.sType   = _T("");
						notify.lParam  = m_Data;
						notify.wParam  = 0;

						static_cast<INotifyUI*>(m_pOwnerWnd)->Notify(notify);  
					}
					else if( msg.pSender->GetName() == _T("menu_Delete") ) 
					{
						TNotifyUI notify;
						notify.pSender = msg.pSender;
						notify.iType   = POPUPMENU_DELETEITEM_CLICK;
						notify.sType   = _T("");
						notify.lParam  = m_Data;
						notify.wParam  = 0;

						static_cast<INotifyUI*>(m_pOwnerWnd)->Notify(notify);   
					} 
					else if( msg.pSender->GetName() == _T("menu_Delete_File") ) 
					{
						TNotifyUI notify;
						notify.pSender = msg.pSender;
						notify.iType   = POPUPMENU_DELETEFILE_CLICK;
						notify.sType   = _T("");
						notify.lParam  = m_Data;
						notify.wParam  = 0;

						static_cast<INotifyUI*>(m_pOwnerWnd)->Notify(notify);    
					}
					else if( msg.pSender->GetName() == _T("menu_clear") ) 
					{
                        TNotifyUI notify;
						notify.pSender = msg.pSender;
						notify.iType   = POPUPMENU_CLEARFILE_CLICK;
						notify.sType   = _T("");
						notify.lParam  = m_Data;
						notify.wParam  = 0;

						static_cast<INotifyUI*>(m_pOwnerWnd)->Notify(notify);     
					}
				}
				break;
		}
	}
 
#if _MSC_VER <=1200
    typedef  CMenuWnd __super;
#endif 

private:
	std::string  data;
	WindowImplBase *mPOwner;
   
};
#endif