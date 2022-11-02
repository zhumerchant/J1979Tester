#ifndef MENU_WND_H
#define MENU_WND_H


#include "WinImplBase.hpp"
 
class CMenuWnd : public WindowImplBase
{
public:
    CMenuWnd(int nData,CDuiString xmlFileName);
    void Init(WindowImplBase * pOwnerWnd, POINT pt);
    void Init(CControlUI* pOwner, POINT pt);
    void AdjustPostion();   
	int  getData();
protected:
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual LPCTSTR GetWindowClassName() const ;
	virtual UINT GetClassStyle() const ;
	virtual LONG GetStyle() const;
	virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; };  
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
    virtual void Init();
	virtual void Notify(TNotifyUI& msg);   

public: 
    CControlUI *m_pOwner;
	WindowImplBase *m_pOwnerWnd;
    POINT m_ptPos;
	CDuiString m_XmlFileName;
	int m_Data;
};

#endif
