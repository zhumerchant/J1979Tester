#ifndef _TRAYPOPMENU_H
#define _TRAYPOPMENU_H

#include "MenuWnd.h"

class TrayPopmenu : public CMenuWnd
{

	
public:
	TrayPopmenu():CMenuWnd(0,_T("tray_menu.xml"))
	{ 
		
	}

    ~TrayPopmenu()
	{

	}
public:
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
						if(msg.pSender->GetName() == _T("menu_quit"))
						{
							::PostQuitMessage(0);
						}
					}
					break;
			}
	}

	void Popup(WindowImplBase * pOwnerWnd, POINT pt)  
	{
		CMenuWnd::Init(pOwnerWnd, pt);  
	}
 
#if _MSC_VER <=1200
    typedef  CMenuWnd __super;
#endif 

private:


};
#endif