#ifndef _AUDIO_H
#define _AUDIO_H


#include <mmsystem.h>
#include <dsound.h>
#include <dxerr8.h>
#include <sstream>
#include <assert.h>

 
#define SAFE_DELETE(p)       { if(p) { delete p;    p=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[]p;   p=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { p->Release(); p=NULL; } }

#include "AudioEngine/WAVFile.h"
#include "AudioEngine/DiscreteFourierTransform.h" 

#include "mpg123.h"

using namespace std;
using namespace AudioEngine;


#define WM_STATE_STOP			WM_USER+308		//音乐播放完毕通知主窗口

#define PAUSE_DELAY				3

class CAudio
{
private:
	HWND m_hWnd;
	static const int BUFFERSIZE = 88;

	LPDIRECTSOUND8 g_pDS;
	LPDIRECTSOUNDBUFFER g_pPrimaryBuffer;
	LPDIRECTSOUNDBUFFER8 g_pWAVBuffer;
	
	static GUID m_Driver;//音频设备

	//波形数据
	WAVEFORMATEX m_fmtWave;
	CWAVFile *m_wavFile;
	
	static const DWORD NUM_PLAY_NOTIFICATIONS = 16;//缓冲块数
	DWORD m_dwNotifySize;//每个缓冲块的大小
	DWORD m_dwDSBufferSize;//全局缓冲大小
	int m_LockPos;

	CDiscreteFourierTransform *g_FT;
	double m_pos[BUFFERSIZE], m_topPos[BUFFERSIZE];
	int m_stopCnt[BUFFERSIZE];
	double FastDown;
	double SlowDown;
	static const int StopTime = 10;
	unsigned char m_Mp3buffer[50000];
	
	//MP3总的帧数
	off_t m_Mp3TotalFrame;

	//到达MP3文件尾
	bool m_bEnd;
	
	//剩余缓冲区块数
	int m_RemainBlock;

	//音量
	int m_db;

	//是否静音
	bool muted;

	//暂停播放缓冲计数
	int m_PauseCnt;

	//单曲循环
	bool m_bLoop;

	//暂停位置
	DWORD m_lst_pos;

	//播放状态
	enum STATE
	{
		STATE_STOP,
		STATE_PLAYING,
		STATE_PAUSE
	}m_play_state;

	//等待播放结束事件事件
	static HANDLE m_hNotifyEvent;
	
	//线程handle
	DWORD m_dwNotifyThreadID;
	HANDLE m_hNotifyThread;

	//线程同步
	CRITICAL_SECTION m_cs;

	LPDIRECTSOUNDNOTIFY8 m_pDSNotify;
	DSBPOSITIONNOTIFY m_notifyPos[NUM_PLAY_NOTIFICATIONS];
	mpg123_handle_struct* m_hMp3;

public:
	CAudio()
	{
		m_PauseCnt = 0;
		memset(&m_Driver, 0, sizeof(m_Driver));
		g_pDS = NULL;
		g_pPrimaryBuffer = NULL;
		g_pWAVBuffer = NULL;

		g_FT = NULL;
		m_wavFile = NULL;
		
		FastDown = 1.5;
		SlowDown = 0.7;
		m_lst_pos = 0;
		m_play_state = STATE_STOP;
		ZeroPos();

		m_hNotifyEvent = NULL;
		m_hNotifyThread = NULL;
		m_dwNotifyThreadID = 0;
		m_pDSNotify = NULL;
		
		m_dwNotifySize = 0;//每个缓冲块的大小
		m_dwDSBufferSize = 0;//全局缓冲大小
		m_LockPos = 0;

		m_db = 0;
		muted =false;
		m_RemainBlock = 0;
		m_bEnd = false;

		memset(&m_fmtWave, 0, sizeof(m_fmtWave));

		m_hMp3 = NULL;
		m_bLoop = false;
	}

	~CAudio()
	{
		SAFE_RELEASE(g_pWAVBuffer);
		SAFE_RELEASE(g_pPrimaryBuffer);
		SAFE_RELEASE(g_pDS);
		SAFE_DELETE(g_FT);
	}
  

	//清空播放结束标志
	void RestFlag()
	{
		m_bEnd = false;
		m_RemainBlock = 0;
	}

	inline bool IsLoop()
	{
		return m_bLoop;
	}

	bool IsStop(int i)
	{
		return m_stopCnt[i] != 0;
	}

	void SetStop(int i)
	{
		m_stopCnt[i] = StopTime;
	}

	void Stop()
	{
		if (!IsPlaying())
			return;

		m_play_state = STATE_STOP;
		
		if (g_pWAVBuffer)
		{
			EnterCriticalSection(&m_cs);

			g_pWAVBuffer->GetCurrentPosition(&m_lst_pos, NULL);

			unsigned char *ptr1 = NULL, *ptr2 = NULL;
			unsigned long len1 = 0, len2 = 0;

			m_lst_pos += m_dwNotifySize+m_lst_pos%m_dwNotifySize;
			m_lst_pos %= m_dwDSBufferSize;

			int numbytes = m_dwNotifySize*PAUSE_DELAY;
			m_PauseCnt = PAUSE_DELAY+1;

			if (SUCCEEDED(g_pWAVBuffer->Lock(m_lst_pos, numbytes, (void **)&ptr1, &len1, (void **)&ptr2, &len2, 0))) 
			{
				short *pt1 = (short*)ptr1;
				short *pt2 = (short*)ptr2;
				double step = 1.0/((numbytes)/2);
				unsigned long i = 0;

				for (; i < len1/2; ++i)
				{
					pt1[i] = (short)(pt1[i]*(1-step*i));
				}
				for (int index = 0; i < len2/2; ++i, ++index)
				{
					pt2[index] = (short)(pt2[index]*(1-step*i));
				}

				g_pWAVBuffer->Unlock(ptr1, len1, ptr2, len2);
			}
			else
			{
				printf("DS Lock fail\n");
			}

			LeaveCriticalSection(&m_cs);
		}
	}

	void ZeroPos()
	{
		memset(m_pos, 0, sizeof(m_pos));
		memset(m_stopCnt, 0, sizeof(m_stopCnt));
		fill(m_topPos, m_topPos+50, 1.0f);
	}

	static INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPCTSTR strDesc, LPCTSTR strDrvName,
		VOID* pContext )
	{
		// Set aside static storage space for 20 audio drivers
		static GUID  AudioDriverGUIDs[20];
		static DWORD dwAudioDriverIndex = 0;

		GUID* pTemp  = NULL;

		if( pGUID )
		{
			if( dwAudioDriverIndex >= 20 )
				return TRUE;

			pTemp = &AudioDriverGUIDs[dwAudioDriverIndex++];
			memcpy( pTemp, pGUID, sizeof(GUID) );

			CDuiString str=  strDesc;
			if(str.Find(_T("主声音"))!=-1)
				memcpy(&m_Driver, pGUID, sizeof(GUID) );
		}

		return TRUE;
	}

	static DWORD NotifyHandleProc(LPVOID param)
	{
		CAudio* obj = (CAudio*)param;
		DWORD   dwResult;
		MSG     msg;
		while (1)
		{
			dwResult = MsgWaitForMultipleObjects( 1, &m_hNotifyEvent, FALSE, INFINITE, QS_ALLEVENTS );

			switch( dwResult )
			{
			case WAIT_OBJECT_0 + 0:
				{
					//obj->LoadDSData();
					obj->LoadMp3Data();
					break;
				}

			case WAIT_OBJECT_0 + 1:
				// Messages are available
				while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
				{ 
					if( msg.message == WM_QUIT )
						return 0;
				}
				break;
			}
		}
		return 0;
	}

	bool CreateNotifyThread()
	{
		m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hNotifyEvent)
		{
			printf("CreateNotifyThread->CreateEvent fail\n");
			return false;
		}

		InitializeCriticalSection(&m_cs);

		m_hNotifyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NotifyHandleProc, 
			(LPVOID)this, 0, &m_dwNotifyThreadID);

		if (!m_hNotifyThread)
			return false;

		return true;
	}

	// InitAudio: initializes DirectX Audio
	bool InitAudio(HWND hwnd)
	{
		m_hWnd = hwnd;
		HRESULT hr;
		stringstream err;

		if(mpg123_init() != MPG123_OK)
			return false;

		int ret = -1;
		m_hMp3 = mpg123_new(NULL, &ret);
		if(m_hMp3 == NULL || ret != MPG123_OK)
			return false;

		// step 1: create primary DS interface
		if (FAILED(hr = DirectSoundCreate8(NULL, &g_pDS, NULL))) 
		{
			err << "DirectSoundCreate8 failed: " << DXGetErrorString8(hr) << endl;
			throw(err.str());
			return false;
		}

		// step 2: Set cooperative level        
		if(FAILED(hr = g_pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))) 
		{
				err << "SetCooperativeLevel failed: " << DXGetErrorString8(hr) << endl;
				throw(err.str());
				return false; 
		}

		// Get the primary buffer 
		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;// | DSBCAPS_CTRLVOLUME;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;

		// "create it" (or get it if it already exists, which it does)
		if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &g_pPrimaryBuffer, NULL ) ) ) {
			err << "CreateSoundBuffer failed: " << DXGetErrorString8(hr) << endl;
			throw(err.str());
			return false;
		}

		// set its format
		WAVEFORMATEX wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = WAVE_FORMAT_PCM; 
		wfx.nChannels       = (WORD)2; 
		wfx.nSamplesPerSec  = 44100; // 22 Khz (CD quality is ~ 44 Khz, so this is "half-CD" quality)
		wfx.wBitsPerSample  = (WORD) 16; // 2 bytes per sample * 22050 samples/sec = 44100 bytes/sec)
		wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		if( FAILED( hr = g_pPrimaryBuffer->SetFormat(&wfx) ) ) {
			err << "pSDBPrimary->SetFormat() failed: " << DXGetErrorString8(hr) << endl;
			throw(err.str());
			return false;
		}

		if (!CreateNotifyThread())
			return false;

		return true;
	}

	int CreateSecondaryBuffer(LPDIRECTSOUNDBUFFER8 *ppBuffer, CWAVFile &wav)
	{
		HRESULT hr;
		stringstream err;

		DSBUFFERDESC dsbd;
		WAVEFORMATEX wfx;

		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = wav.m_AudioFormat;
		wfx.nChannels       = wav.m_NumberOfChannels; 
		wfx.nSamplesPerSec  = wav.m_SampleRate; 
		wfx.wBitsPerSample  = wav.m_BitsPerSample; 
		wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize          = sizeof(DSBUFFERDESC);
		dsbd.dwFlags         = DSBCAPS_CTRLPOSITIONNOTIFY| DSBCAPS_CTRLVOLUME|DSBCAPS_STICKYFOCUS|DSBCAPS_LOCHARDWARE;
		dsbd.dwBufferBytes   = m_dwDSBufferSize;
		dsbd.guid3DAlgorithm = GUID_NULL;
		dsbd.lpwfxFormat     = &wfx;

		if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, (LPDIRECTSOUNDBUFFER *)ppBuffer, NULL ) ) )
		{
			err << "CreateSecondaryBuffer: CreateSoundBuffer failed: " << DXGetErrorString8(hr) << endl;
			throw(err.str());;
		}

		return(dsbd.dwBufferBytes); // return size of buffer
	}

	int CreateSecondaryBuffer(LPDIRECTSOUNDBUFFER8 *ppBuffer, WAVEFORMATEX * wfx)
	{
		HRESULT hr;
		DSBUFFERDESC dsbd;

		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize          = sizeof(DSBUFFERDESC);
		dsbd.dwFlags         = DSBCAPS_CTRLPOSITIONNOTIFY| DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS;
		dsbd.dwBufferBytes   = m_dwDSBufferSize;
		dsbd.guid3DAlgorithm = GUID_NULL;
		dsbd.lpwfxFormat     = wfx;

		if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, (LPDIRECTSOUNDBUFFER *)ppBuffer, NULL ) ) )
			return 0;

		return(dsbd.dwBufferBytes); // return size of buffer
	}
	
	bool IsMuted()
	{
		return muted;
	}

	void Mute()
	{
		muted=true;

		if(g_pWAVBuffer==NULL)
			return;

		HRESULT hr;
		if( FAILED( hr = g_pWAVBuffer->SetVolume(-10000)))
			printf("Direct sound SetVolume %d fail\n", m_db);
	}

	void UnMute()
	{

		muted=false; 

		
		if(g_pWAVBuffer==NULL)
			return;
		HRESULT hr;
		if( FAILED( hr = g_pWAVBuffer->SetVolume(m_db)))
			printf("Direct sound SetVolume %d fail\n", m_db);
	}

	void SetVolume(int db, bool bSet = true)
	{	
		
		//设置音量范围:-10000 -- 0，-10000是静音,0是最大

		if (db < 0 || db > 100)
			return; 


		if (bSet)
			m_db = db = (int)(2000*(db/100.0f-1));
		else
			db = (int)(2000*(db/100.0f-1));

		if (!g_pWAVBuffer)
			return;

		HRESULT hr;
		if( FAILED( hr = g_pWAVBuffer->SetVolume(db)))
			printf("Direct sound SetVolume %d fail\n", m_db);
	}

	int GetVolume()
	{
		return (int)(100*m_db/2000.0+100);
	}

	bool UpdatePos()
	{
		bool ret = false;
		double scale = 8;
		const double limitHight = 30;

		for (int i = 0; i < BUFFERSIZE/2; ++i)
		{	
			m_pos[i] -= FastDown;
			m_pos[i] = max(m_pos[i], 0.0f);
			
			if (!IsStop(i))
				m_topPos[i] -= SlowDown;
			m_stopCnt[i]--;
			m_stopCnt[i] = max(m_stopCnt[i], 0);

			m_topPos[i] = max(m_topPos[i], 1.0f);
			
			//刷新最高值
			if (IsPlaying())
			{
				double height = 0;
				height = g_FT->GetAmp(i)*scale;
				height = min(height, limitHight);
				height = max(height, 0);

				m_pos[i] = max(m_pos[i], height);

				if (height > m_topPos[i])
				{
					m_topPos[i] = height;
					SetStop(i);
				}
			}

			if (m_topPos[i] != 1.0)
				ret = true;

			scale += 0.66;
		}

		return ret;
	}

	bool LoadMp3(CDuiString filename)
	{
		SAFE_RELEASE(g_pWAVBuffer);
		
		RestFlag();

		if( filename.Find(_T("mp3")) <0 )
			return false;

		int ret =MPG123_OK;

		ret = mpg123_topen(m_hMp3, filename.GetData());
		if( ret!= MPG123_OK)
			return false;		

		m_Mp3TotalFrame = mpg123_tellframe(m_hMp3);

		long rate = 0;
		int channel = 0;
		int encoding = 0;

		if(mpg123_getformat(m_hMp3, &rate, &channel, &encoding) != MPG123_OK)
			return false;	
		
		if((encoding & MPG123_ENC_16) == MPG123_ENC_16)
			encoding = 16;
		else if((encoding & MPG123_ENC_32) == MPG123_ENC_32)
			encoding = 32;
		else
			encoding = 8;

		memset(&m_fmtWave, 0, sizeof(WAVEFORMATEX));
		m_fmtWave.wFormatTag = WAVE_FORMAT_PCM;
		m_fmtWave.nChannels = channel;
		m_fmtWave.nSamplesPerSec = rate;
		m_fmtWave.wBitsPerSample = encoding;
		m_fmtWave.nBlockAlign = encoding / 8 * channel;
		m_fmtWave.nAvgBytesPerSec = rate * (encoding / 8) * channel;

		//缓冲块大小
		m_dwNotifySize = rate * 3 * m_fmtWave.nBlockAlign / NUM_PLAY_NOTIFICATIONS;
		m_dwNotifySize -= m_dwNotifySize % m_fmtWave.nBlockAlign;   
		//总缓冲大小
		m_dwDSBufferSize = m_dwNotifySize * NUM_PLAY_NOTIFICATIONS;

		//缓冲区起始偏移
		m_LockPos = 0;

		//创建DFT
		SAFE_DELETE(g_FT);
		g_FT = new CDiscreteFourierTransform(BUFFERSIZE, rate, encoding, 
			channel);

		if (CreateSecondaryBuffer(&g_pWAVBuffer, &m_fmtWave) == 0)
			return false;

		SetVolume(GetVolume());

		//读MP3音频数据
		for (int i = 0; i < NUM_PLAY_NOTIFICATIONS; ++i)
			LoadMp3Data();

		HRESULT hr = g_pWAVBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pDSNotify);

		for (int i = 0; i < NUM_PLAY_NOTIFICATIONS; ++i)
		{
			m_notifyPos[i].dwOffset     = (m_dwNotifySize * i) + m_dwNotifySize - 1;
			m_notifyPos[i].hEventNotify = m_hNotifyEvent; 
		}

		hr = m_pDSNotify->SetNotificationPositions(NUM_PLAY_NOTIFICATIONS, m_notifyPos);

		SAFE_RELEASE( m_pDSNotify );

		return true;
	}

	double Duration()
	{
		if (m_fmtWave.nSamplesPerSec == 0)
			return 0;
		
		off_t len = mpg123_length(m_hMp3);
		return (len / m_fmtWave.nSamplesPerSec);
	}

	int CurPlayingPos()
	{
		if (IsStop())
			return 0;

		off_t len = mpg123_tell(m_hMp3);
		if (len == 0)
			return 0;

		return ((len-2)/m_fmtWave.nSamplesPerSec);
	}

	bool SetPlayingPos(int pos)
	{
		if (IsStop())
			return false;

		if (pos < 0 || pos > Duration())
			return false;

		RestFlag();
		
		Stop();
 

		off_t p = mpg123_timeframe(m_hMp3, pos);
		mpg123_seek_frame(m_hMp3, p, SEEK_SET);

		Mp3FillBuffer();
		Play();
		
		return true;
	}

	mpg123_frameinfo GetMp3Information()
	{
		mpg123_frameinfo info;
		mpg123_info(m_hMp3, &info);

		return info;
	}

	BOOL LoadMp3Data()
	{
		//暂停的淡入淡出
		if (IsPause())
		{
			m_PauseCnt--;
			if (m_PauseCnt == 0)
			{
				EnterCriticalSection(&m_cs);
				g_pWAVBuffer->GetCurrentPosition(&m_lst_pos, NULL);
				//m_lst_pos = (m_lst_pos+m_dwNotifySize-m_lst_pos%m_dwNotifySize)%m_dwDSBufferSize;
				//m_LockPos = (m_lst_pos-m_dwNotifySize+m_dwDSBufferSize)%m_dwDSBufferSize;
				g_pWAVBuffer->Stop();
				LeaveCriticalSection(&m_cs);
				return TRUE;
			}
		}

		//停止的淡入淡出
		if (IsStop())
		{
			m_PauseCnt--;
			if (m_PauseCnt == 0)
			{
				EnterCriticalSection(&m_cs);
				g_pWAVBuffer->Stop();
				//m_lst_pos = (m_lst_pos+m_dwNotifySize-m_lst_pos%m_dwNotifySize)%m_dwDSBufferSize;
				//m_LockPos = (m_lst_pos-m_dwNotifySize+m_dwDSBufferSize)%m_dwDSBufferSize;
				LeaveCriticalSection(&m_cs);
				return TRUE;
			}
		}

		//检测是否读完所有缓冲区
		if (m_bEnd)
		{
			m_RemainBlock--;
			if (m_RemainBlock == 0)
			{
				Stop();
				::PostMessage(m_hWnd, WM_STATE_STOP, 0, 0);
			}
			return FALSE;
		}

		BOOL ret = TRUE;
		HRESULT hr;
		unsigned char *aptr1 = NULL, *aptr2 = NULL;
		unsigned long abyte1 = 0, abyte2 = 0;

		size_t outsize = 0;

		EnterCriticalSection(&m_cs);

		if( FAILED( hr = g_pWAVBuffer->Lock(m_LockPos, m_dwNotifySize, (void **)&aptr1, &abyte1, 
			(void **)&aptr2, &abyte2, 0L) ) )
		{
			printf("lock fail %d %d\n", m_LockPos, m_dwNotifySize);
		}

		if (mpg123_read(m_hMp3, m_Mp3buffer, m_dwNotifySize, &outsize) == MPG123_ERR)
		{
			printf("mpg123_read error\n");
			ret = FALSE;
			goto END;
		}

		if (outsize < m_dwNotifySize)
		{		
			int step = outsize;
			memcpy(aptr1, m_Mp3buffer, outsize);
			if (mpg123_seek(m_hMp3, 0, SEEK_SET) < 0)
			{
				printf("mpg123_seek error\n");
				ret = FALSE;
				goto END;
			}

			if (IsLoop())//单曲循环播放,则需要把文件指针移到开头出继续读数据
			{
				int remain = m_dwNotifySize-outsize;
				if(mpg123_read(m_hMp3, m_Mp3buffer, remain, &outsize) != MPG123_OK)
				{
					printf("mpg123_read error\n");
					ret = FALSE;
					goto END;
				}
				memcpy(aptr1+step, m_Mp3buffer, outsize);
			}
			else//不需要循环，则填充空白数据，然后标记结束，等待剩下的缓冲区播放完毕
			{
				//memset(aptr1+step, 0, outsize);
				m_bEnd = true;//到达文件尾了
				m_RemainBlock = NUM_PLAY_NOTIFICATIONS-1;
			}

		}
		else
		{
			memcpy(aptr1, m_Mp3buffer, outsize);
		}

END:
		hr = g_pWAVBuffer->Unlock(aptr1, abyte1, aptr2, abyte2);
		if (hr != S_OK)
			printf("Unlock fail\n");

		//缓冲区指针前进
		m_LockPos += m_dwNotifySize;
		m_LockPos %= m_dwDSBufferSize;

		LeaveCriticalSection(&m_cs);

		return ret;
	}

	void Mp3FillBuffer()
	{
		assert(g_pWAVBuffer != NULL);

		m_LockPos = 0;
		g_pWAVBuffer->SetCurrentPosition(0);
		
		//读MP3音频数据
		for (int i = 0; i < NUM_PLAY_NOTIFICATIONS; ++i)
			LoadMp3Data();
	}

	void Play()
	{
		if (!IsPlaying())
		{
			if (g_pWAVBuffer)
			{
				g_pWAVBuffer->SetCurrentPosition(0);
				g_pWAVBuffer->Play(0, 0, DSBPLAY_LOOPING);
			}
			m_play_state = STATE_PLAYING;
		}
	}

	bool Resume()
	{
		bool ret = false;
		if (IsPause())
		{
			if (g_pWAVBuffer)
			{
				EnterCriticalSection(&m_cs);
				m_play_state = STATE_PLAYING;

				DWORD state = 0;
				g_pWAVBuffer->GetStatus(&state);
				if (state & DSBSTATUS_PLAYING)
				{
					g_pWAVBuffer->GetCurrentPosition(&m_lst_pos, NULL);

					unsigned char *ptr1 = NULL, *ptr2 = NULL;
					unsigned long len1 = 0, len2 = 0;

					m_lst_pos += m_dwNotifySize+m_dwNotifySize-m_lst_pos%m_dwNotifySize;
					m_lst_pos %= m_dwDSBufferSize;

					int numbytes = m_dwNotifySize*3;

					if (SUCCEEDED(g_pWAVBuffer->Lock(m_lst_pos, numbytes, (void **)&ptr1, &len1, (void **)&ptr2, &len2, 0))) 
					{
						short *pt1 = (short*)ptr1;
						short *pt2 = (short*)ptr2;
						double step = 1.0/((numbytes)/2);
						unsigned long i = 0;

						for (; i < len1/2; ++i)
						{
							pt1[i] = (short)(pt1[i]*(step*i));
						}
						for (int index = 0; i < len2/2; ++i, ++index)
						{
							pt2[index] = (short)(pt2[index]*(step*i));
						}

						g_pWAVBuffer->Unlock(ptr1, len1, ptr2, len2);
					}
					else
					{
						printf("DS Lock fail\n");
					}
				}
				else
				{	
					m_lst_pos = (m_lst_pos+m_dwNotifySize-(m_lst_pos%m_dwNotifySize))%m_dwDSBufferSize;
					g_pWAVBuffer->SetCurrentPosition(m_lst_pos);

					int numbytes = m_dwNotifySize*4;

					unsigned char *ptr1 = NULL, *ptr2 = NULL;
					unsigned long len1 = 0, len2 = 0;

					if (SUCCEEDED(g_pWAVBuffer->Lock(m_lst_pos, numbytes, (void **)&ptr1, &len1, (void **)&ptr2, &len2, 0))) 
					{
						short *pt1 = (short*)ptr1;
						short *pt2 = (short*)ptr2;
						double step = 1.0/(numbytes/2);
						unsigned long i = 0;

						for (; i < len1/2; ++i)
						{
							pt1[i] = (short)(pt1[i]*(step*i));
						}
						for (int index = 0; i < len2/2; ++i, ++index)
						{
							pt2[index] = (short)(pt2[index]*(step*i));
						}

						g_pWAVBuffer->Unlock(ptr1, len1, ptr2, len2);
					}
					else
					{
						printf("DS Lock fail\n");
					}

					ret = S_OK == g_pWAVBuffer->Play(0, 0, DSBPLAY_LOOPING);
				}

				LeaveCriticalSection(&m_cs);
			}
			
		}
		return ret;
	}

	bool Pause()
	{
		//没有在播放
		if (!IsPlaying())
			return false;

		bool ret = false;
		if (IsPlaying())
		{
			if (g_pWAVBuffer)
			{
				EnterCriticalSection(&m_cs);

				m_play_state = STATE_PAUSE;
				
				g_pWAVBuffer->GetCurrentPosition(&m_lst_pos, NULL);

				unsigned char *ptr1 = NULL, *ptr2 = NULL;
				unsigned long len1 = 0, len2 = 0;
				
				m_lst_pos += m_dwNotifySize+(m_dwNotifySize-m_lst_pos%m_dwNotifySize);
				m_lst_pos %= m_dwDSBufferSize;
				
				int numbytes = m_dwNotifySize*PAUSE_DELAY;
				m_PauseCnt = PAUSE_DELAY+1;

				if (SUCCEEDED(g_pWAVBuffer->Lock(m_lst_pos, numbytes, (void **)&ptr1, &len1, (void **)&ptr2, &len2, 0))) 
				{
					short *pt1 = (short*)ptr1;
					short *pt2 = (short*)ptr2;
					double step = 1.0/((numbytes)/2);
					unsigned long i = 0;

					for (; i < len1/2; ++i)
					{
						pt1[i] = (short)(pt1[i]*(1-step*i));
					}
					for (int index = 0; i < len2/2; ++i, ++index)
					{
						pt2[index] = (short)(pt2[index]*(1-step*i));
					}

					g_pWAVBuffer->Unlock(ptr1, len1, ptr2, len2);
				}
				else
				{
					printf("DS Lock fail\n");
				}

				LeaveCriticalSection(&m_cs);
			}
		}

		return ret;
	}

	inline bool IsPlaying()
	{
		return m_play_state == STATE_PLAYING;
	}

	inline bool IsPause()
	{
		return m_play_state == STATE_PAUSE;
	}

	//播放是否结束
	inline bool IsStop()
	{
		return m_play_state == STATE_STOP;
	}

	void UpdateSpectrum(LPDIRECTSOUNDBUFFER8 pDSBuf)
	{
		//没有播放音乐
		if (!IsPlaying() || ::IsIconic(m_hWnd))
			return;

		BOOL ret = FALSE;
		unsigned char curbuf[BUFFERSIZE*4];
		unsigned char *ptr1 = NULL, *ptr2 = NULL;
		unsigned long len1 = 0, len2 = 0;
		DWORD playpos = 0;

		int numbytes = BUFFERSIZE * g_FT->m_NumChannels * (g_FT->m_BitsPerSample/8);

		EnterCriticalSection(&m_cs);

		pDSBuf->GetCurrentPosition(&playpos, NULL);
		if (SUCCEEDED(pDSBuf->Lock(playpos, numbytes, (void **)&ptr1, &len1, (void **)&ptr2, &len2, 0))) 
		{
			memcpy(curbuf, ptr1, len1);
			if (ptr2 != NULL) 
				memcpy(curbuf+len1, ptr2, len2);
			pDSBuf->Unlock(ptr1, len1, ptr2, len2);
		}
		else 
		{
			printf("UpdateSpectrum lock fail\n");
			memset(curbuf, 0, BUFFERSIZE*4);
		}

		LeaveCriticalSection(&m_cs);

		// perform DFT on buf
		g_FT->LoadInputData(curbuf);
		g_FT->PerformSlowFourierTransform();
	} 
};



#endif
