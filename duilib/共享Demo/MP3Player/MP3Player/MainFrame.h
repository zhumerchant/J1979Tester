#ifndef MAINFRAME_H
#define MAINFRAME_H
 

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <mmsystem.h>
#include "mpg123.h"
#include "Audio.h"
#include "PlayList.h"
#include "Mp3_info/MP3Info.h"
#include "trayiconimpl.h"
#include "PlayListPopMenu.h"

 
class MainFrame : public WindowImplBase, public CTrayIconImpl<MainFrame>
{
protected:
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetResourceID() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetWindowClassName() const ;
	virtual UINT GetClassStyle() const ;
	virtual LONG GetStyle() const;
	virtual void Notify(TNotifyUI& msg); 
	virtual void OnTimer(TNotifyUI& msg);
public:
    MainFrame();
	~MainFrame();
public:
	
#if _MSC_VER <=1200
    typedef  WindowImplBase __super;
#endif 
    
    void ShowTrayMenu(POINT pt);  //ÏÔÊ¾ÍÐÅÌ²Ëµ¥

    void OnFinalMessage(HWND /*hWnd*/);

    void Init();

    bool OnHChanged(void* param);

    bool OnSChanged(void* param) ;

    bool OnLChanged(void* param);

    bool OnAlphaChanged(void* param);

	bool OnVolumeChanged(void* param);

	bool OnProgress(void *param);

    void OnPrepare();
 
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam); 

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	
    void UpdatePlayList(); 

	std::vector<PlayListItemInfo> playlists_;

	
	void PlayMP3(CDuiString sFileName);	
	void Play();
	void Prev();
	void Next();
	void MuteClick();
    void StopPlay();

	void Mute();
	void UnMute();

	CAudio m_Audio;


	CPlayList *m_pPlayList;

	void SetVolume(int volume);

	void ReadMP3NameFunc(bool isDir,const TCHAR *DirName,const TCHAR *filename,const TCHAR *dstdir);
	void ReadMP3(const TCHAR *FullFileName);
    void LoadMP3InDir(const TCHAR *rootDir,bool recurseDir, const TCHAR *sDstDir);

private: 
	int m_nListItem;
	void DelayDo(int nListItem);
	PlayListPopMenu *pPlayListPopMenu;
}; 

#endif //MAINFRAME_H