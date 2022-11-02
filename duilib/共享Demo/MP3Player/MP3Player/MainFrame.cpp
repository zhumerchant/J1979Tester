// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "WinImplBase.hpp"
#include "MainFrame.h"  
#include "MenuWnd.h"
#include "SysConfig.h"
#include "Tools.h" 
#include "TrayPopmenu.h"
#include <shlwapi.h>

#define FRESH_MP3_TIME 1
#define ID_TIMER_SHOW_TIME 2
#define ID_TIMER_OPENFILE_TIME 3

HANDLE CAudio::m_hNotifyEvent;
GUID CAudio::m_Driver;//音频设备

#pragma comment (lib,"libmpg123.lib")
#pragma comment (lib,"winmm.lib")


const TCHAR* const kPlayListControlName = _T("playlist");
const TCHAR* const kPlayTimeControlName = _T("play_time");
const TCHAR* const kPlayAllTimeControlName = _T("play_alltime");
const TCHAR* const kPlayVolumeControlName = _T("play_volume");
const TCHAR* const kPlayTitleControlName = _T("play_title");
const TCHAR* const kPlayProgressControlName = _T("progress_controlor");
 

MainFrame::MainFrame()
:m_pPlayList(NULL),m_nListItem(-1)
{ 

}


MainFrame::~MainFrame()
{ 
	PostQuitMessage(0);
}


LPCTSTR MainFrame::GetWindowClassName() const 
{ 
	return _T("MP3PlayerMain"); 
}


CDuiString MainFrame::GetSkinFile()
{
	return _T("MP3Player.xml");
}
 

UINT MainFrame::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; 
} 


DuiLib::CDuiString MainFrame::GetSkinFolder()
{
	return SysConfig::GetSkinFolder();

}


UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
	return SysConfig::GetResourceType();
}


LPCTSTR MainFrame::GetResourceID() const
{
	return SysConfig::GetResourceID();
}

void MainFrame::OnFinalMessage(HWND hWnd) 
{  
	RemoveIcon();
	__super::OnFinalMessage(hWnd);
	delete this; 
}

void MainFrame::Init() 
{ 
	   HICON hIconSmall = (HICON)::LoadImage(CPaintManagerUI::GetInstance(),
										MAKEINTRESOURCE(IDI_ICON1), 
										IMAGE_ICON,
										::GetSystemMetrics(SM_CXSMICON),
										::GetSystemMetrics(SM_CYSMICON),
										LR_DEFAULTCOLOR);
		
		InstallIcon(_T("小兔子"), hIconSmall, IDI_ICON1);
}

bool MainFrame::OnHChanged(void* param) 
{
        TNotifyUI* pMsg = (TNotifyUI*)param;
		switch(pMsg->iType)
		{
			case NOTIFY_VALUECHANGED_VALUE:
				{
					short H, S, L;
					CPaintManagerUI::GetHSL(&H, &S, &L);
					CPaintManagerUI::SetHSL(true, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), S, L);
				}
				break;
		}

        return true;
}

    
bool MainFrame::OnSChanged(void* param) 
{
        TNotifyUI* pMsg = (TNotifyUI*)param;
		switch(pMsg->iType)
		{
			case NOTIFY_VALUECHANGED_VALUE:
				{
					    short H, S, L;
						CPaintManagerUI::GetHSL(&H, &S, &L);
						CPaintManagerUI::SetHSL(true, H, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), L);
				}
				break;
		}

        return true;
}

bool MainFrame::OnProgress(void *param)
{
        TNotifyUI* pMsg = (TNotifyUI*)param;
		switch(pMsg->iType)
		{
              case NOTIFY_VALUECHANGED_VALUE:
				  { 
					 int maxV = (static_cast<CSliderUI*>(pMsg->pSender))->GetMaxValue();
					 int posV = (static_cast<CSliderUI*>(pMsg->pSender))->GetValue();
					 
					 m_Audio.SetPlayingPos((int)(posV*m_Audio.Duration()/maxV)); 

				  }
				  break;
		}
		return true;
}

bool MainFrame::OnLChanged(void* param) 
{
        TNotifyUI* pMsg = (TNotifyUI*)param;
		switch(pMsg->iType)
		{
			case NOTIFY_VALUECHANGED_VALUE:
				{
					short H, S, L;
					CPaintManagerUI::GetHSL(&H, &S, &L);
					CPaintManagerUI::SetHSL(true, H, S, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
				}
				break;
		}

        return true;
}


bool MainFrame::OnAlphaChanged(void* param) 
{
        TNotifyUI* pMsg = (TNotifyUI*)param;
        switch(pMsg->iType)
		{
			case NOTIFY_VALUECHANGED_VALUE:
				{
					m_PaintManager.SetTransparent((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
				}
				break;
		}

        return true;
}

bool MainFrame::OnVolumeChanged(void* param)
{
		TNotifyUI* pMsg = (TNotifyUI*)param;
        switch(pMsg->iType)
		{
			case NOTIFY_VALUECHANGED_VALUE:
				{ 
					CSliderUI *pSliderUI    = static_cast<CSliderUI*>(pMsg->pSender);
					SetVolume(pSliderUI->GetValue()); 
				}
				break;
		}

        return true;
}
    
void MainFrame::OnPrepare() 
{
        CSliderUI* pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("alpha_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnAlphaChanged);
        pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("h_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnHChanged);
        pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("s_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnSChanged);
        pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("l_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnLChanged);
		pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("volume_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnVolumeChanged);

		//进度条
		pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(kPlayProgressControlName));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &MainFrame::OnProgress);

		 
	    const TCHAR *sDir = _T("D:\\music\\");
	
		m_pPlayList = static_cast<CPlayList *>(m_PaintManager.FindControl(kPlayListControlName));

		if (!playlists_.empty())
			playlists_.clear();
		if (m_pPlayList->GetCount() > 0)
			m_pPlayList->RemoveAll();


	    LoadMP3InDir(sDir,true,_T(""));

		if(m_pPlayList->GetCount()>0)
		  m_pPlayList->SelectItem(0,true);
 

		
	
        m_Audio.InitAudio(this->GetHWND());

		pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("volume_controlor")));
		if( pSilder ) pSilder->SetValue(m_Audio.GetVolume());

		pPlayListPopMenu =NULL;
		::DragAcceptFiles(this->GetHWND(),TRUE);
 
}  
 
void MainFrame::Notify(TNotifyUI& msg)
{
		switch(msg.iType)
		{
			case NOTIFY_WINDOWINIT_VALUE:
				{
					OnPrepare();
				}
				break;
			case NOTIFY_CLICK_VALUE:
				{ 
					if( _tcsicmp(msg.pSender->GetName(),_T("closebtn")) == 0) 
					{
						RemoveIcon();
						PostQuitMessage(0);
						return; 
					}
					else if( _tcsicmp( msg.pSender->GetName() , _T("minbtn")) == 0 ) 
					{ 
						this->ShowWindow(false);
						//SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
						return; 
					}
					else if( _tcsicmp(msg.pSender->GetName() ,_T("maxbtn")) ==0 ) 
					{ 
						//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; 
					}
					else if( _tcsicmp(msg.pSender->GetName() , _T("restorebtn")) ==0 ) 
					{ 
						//SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
					} 
					else if( _tcsicmp(msg.pSender->GetName() , _T("play_btn")) ==0 ) //播放
					{ 
						this->Play();
					} 
					else if( _tcsicmp(msg.pSender->GetName() , _T("stop_btn")) ==0 ) //停止
					{ 
						this->StopPlay();
					} 
					else if( _tcsicmp(msg.pSender->GetName() , _T("prev_btn")) ==0 ) //上一首
					{ 
						Prev();
					} 
					else if( _tcsicmp(msg.pSender->GetName() , _T("next_btn")) ==0 ) //下一首
					{ 
						Next();
					} 
					else if( _tcsicmp(msg.pSender->GetName() , _T("mute_btn")) ==0 ) //静音
					{ 
						MuteClick();
					} 
					
				}
				break;
				case NOTIFY_ITEMACTIVATE_VALUE:
			{
				 
						CPlayList *pPlaysList = m_pPlayList;
						if ((pPlaysList != NULL) &&  pPlaysList->GetItemIndex(msg.pSender) != -1)
						{
							if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
							{
								Node* node = (Node*)msg.pSender->GetTag();
 
								if (!pPlaysList->CanExpand(node))
								{  

									PlayListItemInfo mp3_info;

									for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
									{
										if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
										{
											mp3_info = *citer;
											break;
										}
									} 

									
									pPlaysList->InvalidateNodes(node);
									PlayMP3(mp3_info.full_filename.GetData());
									
								}

							}
						} 
			}
			break;
		case NOTIFY_ITEMCLICK_VALUE:
			{
 
						CPlayList* pPlaysList = m_pPlayList;
						if ((pPlaysList != NULL) &&  pPlaysList->GetItemIndex(msg.pSender) != -1)
						{
							if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
							{
								Node* node = (Node*)msg.pSender->GetTag();

								if (pPlaysList->CanExpand(node))
								{
									pPlaysList->SetChildVisible(node, !node->data().child_visible_);
								}
							}
						} 

			}
			break;
			case NOTIFY_MENU_VALUE:
				{ 
					if( msg.pSender->GetName() != _T("playlist") ) return;

					CListUI* pList = static_cast<CListUI*>(msg.pSender);
					int nSel = pList->GetCurSel();


					pPlayListPopMenu = new PlayListPopMenu(nSel);
					if( pPlayListPopMenu == NULL ) { return; }
					POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
					::ClientToScreen(*this, &pt);
					pPlayListPopMenu->Popup(this, pt); 
				}
				break;
			case POPUPMENU_DELETEITEM_CLICK:
				{  
					CPlayList *pPlaysList = m_pPlayList;
		  
					if (pPlaysList != NULL)
					{
						int nSel = msg.wParam;
                        if( nSel < 0 ) return;  


						CControlUI *pListItem =pPlaysList->GetItemAt(nSel);

						if(NULL!=pListItem)
						{
								Node* node = (Node*)pListItem->GetTag();
								for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
								{
									if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
									{ 
										playlists_.erase(citer);
										break;
									}
								} 
						} 
						pPlaysList->RemoveAt(nSel); 
					}  
				}
				break;
			case POPUPMENU_OPENDIR_CLICK:
				{ 
						if(pPlayListPopMenu)
						   pPlayListPopMenu->Close();

						m_nListItem = msg.wParam;
 

						::SetTimer(this->GetHWND(), ID_TIMER_OPENFILE_TIME, 300, NULL);  
				}
				break; 
			case POPUPMENU_DELETEFILE_CLICK:
				{
						CPlayList *pPlaysList = m_pPlayList;
						if (pPlaysList != NULL)
						{
							int nSel = msg.wParam;
							if( nSel < 0 ) return;  

							bool delete_file=false;
							PlayListItemInfo mp3_info;
							CControlUI *pListItem =pPlaysList->GetItemAt(nSel);
							if(NULL!=pListItem)
							{
								    PlayListItemInfo mp3_info;

									Node* node = (Node*)pListItem->GetTag();
									for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
									{
										if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
										{ 

											mp3_info =*citer;

											CDuiString rm_msg;
											rm_msg  =_T("确定要从电脑中删除歌曲 ");
											rm_msg +=mp3_info.file_name;
											rm_msg +=_T("吗？ \n 删除后将不可恢复! ");

											if(IDYES ==::MessageBox(NULL,rm_msg,_T("删除歌曲"),MB_YESNO))
											  delete_file = true;
											break;
										}
									} 
							} 

							if(delete_file)
							{
									if(NULL!=pListItem)
									{
											Node* node = (Node*)pListItem->GetTag();
											for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
											{
												if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
												{ 
													mp3_info =*citer;
													::DeleteFile(mp3_info.full_filename.GetData());
													playlists_.erase(citer);
													break;
												}
											} 
									} 
									pPlaysList->RemoveAt(nSel);  
							} 
							
						}  
				}
				break;
			case POPUPMENU_CLEARFILE_CLICK:
				{
					CPlayList *pPlaysList = m_pPlayList;
					if (pPlaysList != NULL)
					{
						playlists_.clear();
						pPlaysList->RemoveAll();  
					}  
				}
				break;
		}
}

void MainFrame::DelayDo(int nListItem)
{ 
	    CPlayList *pPlaysList = m_pPlayList;
		if ((pPlaysList != NULL) &&  nListItem!= -1)
		{ 
				CControlUI *pListItem =pPlaysList->GetItemAt(nListItem);

				if(NULL!=pListItem)
				{
						Node* node = (Node*)pListItem->GetTag();

						if (!pPlaysList->CanExpand(node))
						{
							PlayListItemInfo mp3_info;

							for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
							{
								if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
								{
									mp3_info = *citer;
									break;
								}
							} 

							
							Tools::FindTarget(mp3_info.full_filename.GetData());
						} 
				} 
		}
}

void MainFrame::MuteClick()
{
	CSliderUI *pSliderUI    = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("volume_controlor")));
	CButtonUI *pMuteButtonUI	= static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("mute_btn"))); 

	if(m_Audio.IsMuted())
	{
		UnMute();
	}
	else
	{
		Mute();
	}
}


void MainFrame::Mute()
{
	CSliderUI *pSliderUI    = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("volume_controlor")));
	CButtonUI *pMuteButtonUI	= static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("mute_btn"))); 

 
	m_Audio.Mute();
	pMuteButtonUI->SetToolTip(_T("关闭静音"));

	CButtonUI *pButton = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("mute_btn"))); 
	pButton->ApplyAttributeList(_T("normalimage=\"file='mute.png'  source='66,0,88,22' \" "));
	 
}

void MainFrame::UnMute()
{
	CSliderUI *pSliderUI    = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("volume_controlor")));
	CButtonUI *pMuteButtonUI	= static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("mute_btn")));  
	 
	m_Audio.UnMute();
	pMuteButtonUI->SetToolTip(_T("静音"));
	CButtonUI *pButton = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("mute_btn")));  
	pButton->ApplyAttributeList(_T("normalimage=\"file='mute.png'  source='22,0,44,22' \" ")); 
}

void MainFrame::Prev()
{
    CPlayList *pPlaysList = m_pPlayList;
	if (pPlaysList != NULL)
	{ 
		    if(pPlaysList->GetCount()>0)
			{
				int nSel = pPlaysList->GetCurSel();

				
				nSel = nSel>0 ? nSel-1 :0;

				pPlaysList->SelectItem(nSel,true);
				Play();
			}
	}
}

void MainFrame::Next()
{
    CPlayList *pPlaysList = m_pPlayList;
	if (pPlaysList != NULL)
	{ 
		    if(pPlaysList->GetCount()>0)
			{
				int nSel = pPlaysList->GetCurSel();

				
				nSel = nSel<pPlaysList->GetCount()-1 ? nSel+1 :0;

				pPlaysList->SelectItem(nSel,true);
				Play();
			}
	}
}

void MainFrame::Play()
{
    CPlayList *pPlaysList = m_pPlayList;
	if (pPlaysList != NULL && pPlaysList->GetCount()>0)
	{ 
		    int nSel = pPlaysList->GetCurSel();

			if(nSel<0)
				pPlaysList->SelectItem(0,true);

			nSel =(nSel<0)? 0:nSel;

		    CControlUI *pListItem =pPlaysList->GetItemAt(nSel);

			if(NULL!=pListItem)
			{
					Node* node = (Node*)pListItem->GetTag();

					if (!pPlaysList->CanExpand(node))
					{ 
						PlayListItemInfo mp3_info;

						for (std::vector<PlayListItemInfo>::const_iterator citer = playlists_.begin(); citer != playlists_.end(); ++citer)
						{
							if (_tcsicmp(citer->id.GetData(), node->data().value.GetData()) == 0)
							{
								mp3_info = *citer;

								 
								pPlaysList->InvalidateNodes(node);
								PlayMP3(mp3_info.full_filename);
								break;
							}
						}  
					} 
			} 
	} 
}

void MainFrame::PlayMP3(CDuiString sFileName)
{ 
    StopPlay(); 

	UnMute();
	
	if (m_Audio.IsPlaying())
		m_Audio.Stop();

	CSliderUI  *pSilder= static_cast<CSliderUI*>(m_PaintManager.FindControl(kPlayProgressControlName));
	if(pSilder)
		pSilder->SetValue(0);



	m_Audio.LoadMp3(sFileName);
					
	CLabelUI  *pLabelTitle = static_cast<CLabelUI *>(m_PaintManager.FindControl(kPlayTitleControlName));




	//读取歌曲音频信息
	CDuiString title;
	mpg123_frameinfo info;
	info = m_Audio.GetMp3Information();  
	 

	CMP3Info mp3;
	mp3.Open(sFileName);
	 
	title.Append( mp3.GetArtist().GetData()); 

	if(title.GetLength()>0)
		title.Append(_T("专辑:"));
	title.Append(mp3.GetAlbum().GetData());  

	pLabelTitle->SetText(title.GetData());


	CDuiString len_title  = _T("长度: ");
	CDuiString ss, mm;
	int m, s;
	s = (int)m_Audio.Duration();
	m = s/60;
	s %= 60;
	ss.Format(_T("%02d"), s);
	mm.Format(_T("%d"), m);
	len_title.Append(mm);
	len_title.Append(_T(":"));
	len_title.Append(ss); 
  

	CLabelUI  *pLabelAllTitle = static_cast<CLabelUI *>(m_PaintManager.FindControl(kPlayAllTimeControlName));
	
	pLabelAllTitle->SetText(len_title.GetData());

	//开始时钟定时
	::KillTimer(this->GetHWND(),ID_TIMER_SHOW_TIME);

	::SetTimer(this->GetHWND(), ID_TIMER_SHOW_TIME, 500, NULL);  

	m_Audio.Play(); 


	CDuiString  trayTitle = _T("小兔子 ");
	trayTitle.Append(sFileName);
	SetTooltipText(trayTitle.GetData());
}

void MainFrame::StopPlay()
{
    if (m_Audio.IsPlaying())
		m_Audio.Stop();
} 

LRESULT MainFrame::OnMouseWheel(UINT  uMsg , WPARAM  wParam, LPARAM lParam, BOOL& bHandled)
{ 
	POINT pt;
	GetCursorPos(&pt);    


	::ScreenToClient(this->GetHWND(), &pt);


	CPlayList *pPlaysList = m_pPlayList;

	CSliderUI  *pSilder= static_cast<CSliderUI*>(m_PaintManager.FindControl(kPlayProgressControlName));
	
	if(!::PtInRect(&pPlaysList->GetPos(),pt) 
	&& !::PtInRect(&pSilder->GetPos(),pt) ) //如果不是在播放列表中，且不在进度条上滚动鼠标，则调整音量
	{  
			int old_Vol = m_Audio.GetVolume();
			int test = wParam;
			if (test > 0)
				old_Vol += 3;
			else
				old_Vol -= 3;

			SetVolume(old_Vol);

	} 

	bHandled = FALSE;
	return 0;
}

void MainFrame::SetVolume(int volume)
{
	m_Audio.SetVolume(volume);   
 
    CLabelUI  *pLabelVolume = static_cast<CLabelUI *>(m_PaintManager.FindControl(kPlayVolumeControlName));
	CSliderUI *pSliderUI    = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("volume_controlor")));
	 

	CDuiString text;
	text.Format(_T("音量: %d%%"), m_Audio.GetVolume());
	pLabelVolume->SetText(text);

	pSliderUI->SetValue(m_Audio.GetVolume());
}



//显示托盘菜单
void MainFrame::ShowTrayMenu(POINT pt)
{  
		TrayPopmenu* pMenu = new TrayPopmenu();
		if( pMenu != NULL )
          pMenu->Popup(this, pt); 		 
}

LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ 
	if(uMsg == WM_TRAYICON)
	   OnTrayIcon(uMsg,wParam,lParam,bHandled);
  
	return 0;
}

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	    switch(uMsg)
		{ 
			case WM_STATE_STOP:
			{
				this->Next();
				return TRUE;
			}
			break; 
			case WM_TIMER:
			{  
				switch (wParam)
				{
					case ID_TIMER_OPENFILE_TIME:
						{
							::KillTimer(this->GetHWND(),ID_TIMER_OPENFILE_TIME);
							if(m_nListItem!=-1)
							  DelayDo(m_nListItem); 

						}
						break;
				   case ID_TIMER_SHOW_TIME:
				 	{   
						CLabelUI  *pLabelVolume = static_cast<CLabelUI *>(m_PaintManager.FindControl(kPlayTimeControlName));
						//进度条
		                CSliderUI  *pSilder = static_cast<CSliderUI*>(m_PaintManager.FindControl(kPlayProgressControlName));

						int second = (int)m_Audio.CurPlayingPos();
						int m_time_minute = second/60;
		                int m_time_second = second%60;

						CDuiString str;
						str.Format(_T("%02d:%02d"), m_time_minute, m_time_second);

						//当前时间：
						pLabelVolume->SetText(str); 
						pSilder->SetValue((int)(pSilder->GetMaxValue()*m_Audio.CurPlayingPos()/m_Audio.Duration()));

						break; 
					}
					break;
				}
			}
			break;
			
		case WM_DROPFILES:
			{ 
				CDuiString sFile;
				TCHAR szFileName[_MAX_PATH];

				HDROP query = (HDROP) wParam;
	            UINT nFileCount = ::DragQueryFile((HDROP)query, 0xFFFFFFFF, NULL, 0);

				for (UINT i=0; i < nFileCount; i++)
				{
					::DragQueryFile((HDROP)query, i, szFileName, _MAX_PATH);

					sFile.Format(_T("%s"),  szFileName);   

					POINT pt;
					::GetCursorPos(&pt); 

					::ScreenToClient(this->GetHWND(), &pt);
 
					 
					if(::PtInRect(&this->m_pPlayList->GetPos(),pt))
					{
						//::MessageBox(NULL,sFile,_T("拖放文件到播放列表中"),0);
						if(::PathIsDirectory(sFile.GetData()))
						{
							LoadMP3InDir(sFile.GetData(),true,_T(""));
						}
						else
						{ 
							ReadMP3(sFile.GetData());
						}
					} 
				}
				::DragFinish((HDROP)query); 
			}
			break;
		} 

        return __super::HandleMessage(uMsg, wParam, lParam);
}


void MainFrame::OnTimer(TNotifyUI& msg)
{
   //m_PaintManager.KillTimer(this,FRESH_MP3_TIME);
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
        // 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
        if( wParam == SC_CLOSE ) 
		{
			RemoveIcon();
            ::PostQuitMessage(0L);
            bHandled = TRUE;
            return 0;
        }

        BOOL bZoomed = ::IsZoomed(*this);
        LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        if( ::IsZoomed(*this) != bZoomed ) {
            if( !bZoomed ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
                if( pControl ) pControl->SetVisible(false);
                pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
                if( pControl ) pControl->SetVisible(true);
            }
            else {
                CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
                if( pControl ) pControl->SetVisible(true);
                pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
                if( pControl ) pControl->SetVisible(false);
            }
        }
        return lRes;
}
 
LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}


CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("PlayList")) == 0)
	{
		return new CPlayList(m_PaintManager);
	}
	 
	return NULL;
}
 
LONG MainFrame::GetStyle() const
{ 
	LONG styleValue = __super::GetStyle();
	  
	styleValue &= ~WS_MINIMIZEBOX;
	styleValue &= ~WS_MAXIMIZEBOX; 
	styleValue &= ~WS_SIZEBOX; 

	return styleValue|WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
} 


void MainFrame::UpdatePlayList()
{
	
}

void MainFrame::ReadMP3(const TCHAR *FullFileName)
{ 
		  LPCTSTR filename =PathFindFileName(FullFileName);

		  int len = _tcsclen(filename);
		  if(len>3)
		  { 
				  if(_T('3') ==filename[len-1])  
				  {
					 if( _T('p') ==filename[len-2] || _T('P') ==filename[len-2])
					 {
						 if(  _T('m') ==filename[len-3] || _T('M') ==filename[len-3])
						 {
								   if (m_pPlayList != NULL)
								  { 
										PlayListItemInfo item;
						 
										unsigned int size =playlists_.size();

										CDuiString id;
										id.Format(_T("%d"),size);

										item.id = id;
										item.folder = false; 
										item.logo = _T("logo.png");
										item.playing =false;
										item.file_name     =filename;
										item.full_filename =FullFileName;
										m_pPlayList->AddNode(item, NULL);
										playlists_.push_back(item); 
								 
									}
						 }
					 }
				  } 				 
		  }
}

void MainFrame::ReadMP3NameFunc(bool isDir,const TCHAR *DirName,const TCHAR *filename,const TCHAR *dstdir)
{
    if(!isDir)
    {
          TCHAR FullFileName[1024];
		   _tcscpy(FullFileName,DirName);
		  _tcscat(FullFileName,filename);
		  ReadMP3(FullFileName);
    }
}    

void MainFrame::LoadMP3InDir(const TCHAR *rootDir,bool recurseDir, const TCHAR *sDstDir)
{
      HANDLE   hSearch;
      WIN32_FIND_DATA   fd;       

      TCHAR   filePathName[256];   
      TCHAR   tmpPath[256];
      ZeroMemory(filePathName,   256);   
      ZeroMemory(tmpPath,   256);   
      _tcscpy(tmpPath,   rootDir);
      _tcscpy(filePathName,rootDir); 

      BOOL   bSearchFinished   =   FALSE;  

      if(   filePathName[_tcsclen(filePathName)   -1]   !=   _T('\\')   )
      {   
            _tcscat(filePathName,   _T("\\"));   
      }

      _tcscat(filePathName, _T("*"));


      ZeroMemory(&fd,   sizeof(WIN32_FIND_DATA));
      hSearch   =   FindFirstFile(filePathName,   &fd);
      if(INVALID_HANDLE_VALUE==hSearch)
      {
         return ;   
      }
      else
      {
            //如果是一个文件夹
            if(   (fd.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY)
                  && _tcscmp(fd.cFileName,   _T("."))!=0
                  && _tcscmp(fd.cFileName,   _T(".."))!=0
                  && recurseDir
              )
            {   
                  _tcscpy(tmpPath,   rootDir);
                  if( tmpPath[_tcsclen(tmpPath)   -1]!= _T('\\'))
                  {
                     _tcscat(tmpPath,   _T("\\"));   
                  }
                  _tcscat(tmpPath,   fd.cFileName);
                  ReadMP3NameFunc(true,tmpPath,fd.cFileName,sDstDir);
                  LoadMP3InDir(tmpPath,recurseDir, sDstDir);
            }   
            else if( !(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
            {
                    //否则如果是一个文件 
                    TCHAR DstName[512];
                    _tcscpy(DstName ,fd.cFileName);
                    _tcscpy(tmpPath,   rootDir);
                    if( tmpPath[_tcsclen(tmpPath)   -1]!= _T('\\'))
                    {
                       _tcscat(tmpPath,  _T("\\"));   
                    }
                    ReadMP3NameFunc(false,tmpPath,DstName,sDstDir); 
            }
      }
  
      while(!bSearchFinished)   
      {    
	    //ZeroMemory(&fd,   sizeof(WIN32_FIND_DATA));
            if( FindNextFile(hSearch,   &fd))   
            {   
                    if(   (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   
                          &&   _tcscmp(fd.cFileName,   _T("."))!=0
                          &&   _tcscmp(fd.cFileName,   _T(".."))!=0
                          &&   recurseDir
                      )
                    {//如果是一个文件夹
                          _tcscpy(tmpPath,   rootDir);
                          if( tmpPath[_tcsclen(tmpPath)   -1]!= _T('\\'))
                          {
                             _tcscat(tmpPath,   _T("\\"));   
                          }
                          _tcscat(tmpPath,   fd.cFileName);
                          ReadMP3NameFunc(true,tmpPath,fd.cFileName,sDstDir);
                          LoadMP3InDir(tmpPath,recurseDir,sDstDir);
                    }   
                    else if( !(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )   
                    {
                          //如果是一个文件
						  TCHAR DstName[512];
						  _tcscpy(DstName ,fd.cFileName);
                          _tcscpy(tmpPath,   rootDir);
                          if( tmpPath[_tcsclen(tmpPath)   -1]!= _T('\\'))
                          {
                             _tcscat(tmpPath,   _T("\\"));   
                          }
                          ReadMP3NameFunc(false,tmpPath,DstName,sDstDir);
                    }
            }   
            else   
            {   
                    if(ERROR_NO_MORE_FILES==GetLastError())     //Normal   Finished
                         bSearchFinished   =   true;
                    else   
                         bSearchFinished   =   false;           //Terminate   Search
            }   
      }     

     FindClose(hSearch); 
}