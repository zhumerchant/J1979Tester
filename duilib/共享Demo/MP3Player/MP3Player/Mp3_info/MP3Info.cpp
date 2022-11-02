#include "stdafx.h"
#include "MP3Info.h"
#include <shlwapi.h>
#include "../CommonUtil.h"

CMP3Info::CMP3Info(CDuiString strFile)
{
	Clear();
	if (strFile != _T(""))
		Open(strFile);
}

CMP3Info::~CMP3Info()
{
}

// helper functions
BOOL CMP3Info::GetNextFrameHeader(HANDLE hFile, MP3FRAMEHEADER* pHeader, int nPassBytes)
{
	memset(pHeader,0,sizeof(*pHeader));
	if (nPassBytes > 0)
		SetFilePointer(hFile,nPassBytes,NULL,FILE_CURRENT);

	int n = 0;
	BOOL bReadOK;
	DWORD dwNumBytesRead;
	do
	{
		bReadOK = ReadFile(hFile,pHeader,4,&dwNumBytesRead,NULL);
		ChangeEndian(pHeader,4); // convert from big-endian to little-endian

		// only search in 10kb
		if (!bReadOK || dwNumBytesRead != 4 ||
			pHeader->framesync == 0x7FF || ++n > 10000)
			break;

		SetFilePointer(hFile,-3,NULL,FILE_CURRENT);
	}
	while (1);

	return (pHeader->framesync == 0x7FF);
}


// operations
BOOL CMP3Info::Open(CDuiString strFile)
{
	Clear(); // make sure all data is blank

	HANDLE hFile = NULL;
	if ((hFile = ::CreateFile(strFile.GetData(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL))
		!= INVALID_HANDLE_VALUE)
	{
		int nNextSearch = 0;

		MP3FRAMEHEADER sFrameHeader;
		memset(&sFrameHeader,0,sizeof(sFrameHeader));

		//int nFrameBR;
		double dLength = 0; // total length of file
		ULONG nTotalBR = 0; // total frames bit rate (used to calc. average)
		DWORD dwNumBytesRead;

		// read frame by frame
		/*while (GetNextFrameHeader(hFile,&sFrameHeader,nNextSearch))
		{
			if (m_nFrames < 1)
			{
				// first read the MPEG version
				switch (sFrameHeader.mpegver)
				{
					case 0: m_enMPEGVersion = MPEGVER_25; break;
					case 1: m_enMPEGVersion = MPEGVER_NA; break;
					case 2: m_enMPEGVersion = MPEGVER_2; break;
					case 3: m_enMPEGVersion = MPEGVER_1; break;
				}

				// next, read the MPEG layer description
				switch (sFrameHeader.mpeglayer)
				{
					case 0: m_nMPEGLayer = 0; break;
					case 1: m_nMPEGLayer = 3; break;
					case 2: m_nMPEGLayer = 2; break;
					case 3: m_nMPEGLayer = 1; break;
				}

				// read the bit for CRC or no CRC
				m_bHasCRC = sFrameHeader.hascrc;
			}

			// read the bitrate, based on the mpeg layer and version
			if (m_nMPEGLayer > 0)
			{
				if (m_enMPEGVersion == MPEGVER_1)
				{
					switch (m_nMPEGLayer)
					{
						case 1: nFrameBR = g_nMP3BitRate[0][sFrameHeader.bitrate]; break;
						case 2: nFrameBR = g_nMP3BitRate[1][sFrameHeader.bitrate]; break;
						case 3: nFrameBR = g_nMP3BitRate[2][sFrameHeader.bitrate]; break;
					}
				}
				else
				{
					switch (m_nMPEGLayer)
					{
						case 1: nFrameBR = g_nMP3BitRate[3][sFrameHeader.bitrate]; break;
						case 2: nFrameBR = g_nMP3BitRate[4][sFrameHeader.bitrate]; break;
						case 3: nFrameBR = g_nMP3BitRate[5][sFrameHeader.bitrate]; break;
					}
				}
			}

			// if nFrameBR is 0 or -1 then the bitrate is either free or bad
			if (nFrameBR > 0)
				nTotalBR += nFrameBR;

			// read sample rate
			if (m_enMPEGVersion == MPEGVER_1)
				switch (sFrameHeader.samplerate)
				{
					case 0: m_nSampleRate = 44100; break;
					case 1: m_nSampleRate = 48000; break;
					case 2: m_nSampleRate = 32000; break;
				}
			else if (m_enMPEGVersion == MPEGVER_2)
				switch (sFrameHeader.samplerate)
				{
					case 0: m_nSampleRate = 22050; break;
					case 1: m_nSampleRate = 24000; break;
					case 2: m_nSampleRate = 16000; break;
				}
			else if (m_enMPEGVersion == MPEGVER_25)
				switch (sFrameHeader.samplerate)
				{
					case 0: m_nSampleRate = 11025; break;
					case 1: m_nSampleRate = 12000; break;
					case 2: m_nSampleRate = 8000; break;
				}

			if (!m_nSampleRate)
				break;

			// read channel mode
			switch (sFrameHeader.chanmode)
			{
				case 0: m_enChannelMode = MP3CM_STEREO; break;
				case 1: m_enChannelMode = MP3CM_JOINT_STEREO; break;
				case 2: m_enChannelMode = MP3CM_DUAL_CHANNEL; break;
				case 3: m_enChannelMode = MP3CM_SINGLE_CHANNEL; break;
			}

			// read the copyright and original bits
			m_bCopyrighted = sFrameHeader.copyright;
			m_bOriginal = sFrameHeader.original;

			// read the emphasis
			switch (sFrameHeader.emphasis)
			{
				case 0: m_enEmphasis = MP3EM_NONE; break;
				case 1: m_enEmphasis = MP3EM_50_15_MS; break;
				case 2: m_enEmphasis = MP3EM_RESERVED; break;
				case 3: m_enEmphasis = MP3EM_CCIT_J17; break;
			}

			// don't read the CRC -- maybe in a future version

			// nNextSearch = frame length, in bytes
			if (m_nMPEGLayer == 1)
				nNextSearch = (12000 * nFrameBR / m_nSampleRate + sFrameHeader.padding) * 4;
			else
				nNextSearch = 144000 * nFrameBR / m_nSampleRate + sFrameHeader.padding;

			nNextSearch -= 4; // the frame header was already read

			m_nFrames++;

			// calculate the length in seconds of this frame and add it to total
			if (nFrameBR)
				dLength += (double)(nNextSearch + 4) * 8 / (nFrameBR * 1000);
		}*/

		// if at least one frame was read, the MP3 is considered valid
		if (m_nFrames > 0)
		{
			m_dwValidity |= MP3VLD_DATA_VALID;
			m_nBitRate = nTotalBR / m_nFrames; // average the bitrate
			m_nLength = (int)dLength;
		}

		// read the ID3 tag

		// set up a structure for reading the ID3 version 1 tag
		MP3ID3V1INFO sID3V1;
		ZeroMemory(&sID3V1, sizeof(sID3V1)); 

		if (SetFilePointer(hFile,-128,NULL,FILE_END) != INVALID_SET_FILE_POINTER)
		{
			if( ReadFile(hFile,&sID3V1,128,&dwNumBytesRead,NULL))
			{
				if( dwNumBytesRead == 128  && strncmp(sID3V1.Identify , ("TAG"),3) == 0)
				{ 
					m_strTitle  = CSECommonUtil::StringToDuiString(sID3V1.Title);
					m_strArtist = CSECommonUtil::StringToDuiString(sID3V1.Artist);
					m_strAlbum  = CSECommonUtil::StringToDuiString(sID3V1.Album);

					m_strYear = CSECommonUtil::StringToDuiString(sID3V1.Year);

					// now, depending on the reserved byte, the comment is 28 bytes or 30 bytes
					if (sID3V1.CommentFlag)
					{
						// NOTE: even if sID3V1.album is of size 28, the reserved and tracknum
						// are right after, so they'll be included in the comment, which is what
						// we want
						char strTemp[30];
						memcpy(strTemp,sID3V1.Comment,30); 
						m_strComment = CSECommonUtil::StringToDuiString(strTemp); 
					}
					else
					{
						char strTemp[30];
						memcpy(strTemp,sID3V1.Comment,28);
						m_strComment = CSECommonUtil::StringToDuiString(strTemp);
						m_nTrack = sID3V1.Track;
						m_nGenre = sID3V1.Genre;
					}

					m_dwValidity |= MP3VLD_ID3V1_VALID;
				}
				else
				{
					m_strTitle = PathFindFileName(strFile.GetData());
					m_strArtist= PathFindFileName(strFile.GetData());
				}
			}
		}

		m_strFile = strFile;
		CloseHandle(hFile);
	}
	else
		return FALSE;

	return TRUE;
}

BOOL CMP3Info::WriteID3V1(CDuiString strDestFile, BOOL bClear)
{
	if (strDestFile == _T(""))
		strDestFile = m_strFile;

	HANDLE hFile = NULL;
	if ((hFile = ::CreateFile(strDestFile.GetData(),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL)) != INVALID_HANDLE_VALUE)
	{
		// write the ID3
		MP3ID3V1INFO sID3V1;
		DWORD dwNumBytesReadWritten;

		// first check if the ID3V1 exists in the file
		if (SetFilePointer(hFile,-128,NULL,FILE_END) != INVALID_SET_FILE_POINTER 
			&& ReadFile(hFile,&sID3V1,128,&dwNumBytesReadWritten,NULL) 
			&& dwNumBytesReadWritten == 128 
			&& strncmp(sID3V1.Identify,("TAG"),3) == 0
		)
			SetFilePointer(hFile,-128,NULL,FILE_END); // tag exists, overwrite it
		else
			SetFilePointer(hFile,0,NULL,FILE_END); // tag doesnt exists

		// write the ID3 tag (or clear it)
		if (bClear)
			SetEndOfFile(hFile);
		else
		{
			memset(&sID3V1,0,sizeof(sID3V1));
 
			memcpy(sID3V1.Identify,_T("TAG"),3);

			
			memcpy(sID3V1.Title,m_strTitle.GetData(),min(m_strTitle.GetLength(),30));
			memcpy(sID3V1.Artist,m_strArtist.GetData(),min(m_strArtist.GetLength(),30));
			memcpy(sID3V1.Album,m_strAlbum.GetData(),min(m_strAlbum.GetLength(),30));
			memcpy(sID3V1.Year,m_strYear.GetData(),min(m_strYear.GetLength(),4));

			// NOTE: copying 30 bytes into sID3V1.comment will set reserved and tracknum
			memcpy(sID3V1.Comment,m_strComment.GetData(),min(m_strComment.GetLength(), m_nTrack ? 28 : 30)); // if a track is specified, max length is 28, otherwise 30

			if (m_nTrack)
				sID3V1.Track = max(0,min(255,m_nTrack));

			sID3V1.Genre = m_nGenre; 

			if (!WriteFile(hFile,&sID3V1,128,&dwNumBytesReadWritten,NULL) ||
				dwNumBytesReadWritten != 128)
			{
				CloseHandle(hFile);
				return FALSE;
			}
		}

		CloseHandle(hFile);
	}
	else
		return FALSE;

	return TRUE;
}

void CMP3Info::Clear()
{
	m_strFile = _T("");
	m_dwValidity = MP3VLD_INVALID;

	m_strArtist =
	m_strTitle =
	m_strAlbum =
	m_strComment =
	m_strYear = _T("");

	m_nTrack = 0;
	m_nGenre = 255;

	m_nFrames = 0;
	m_nLength = 0;
	m_enMPEGVersion = MPEGVER_NA;
	m_nMPEGLayer = 0;
	m_bHasCRC = FALSE;
	m_nBitRate = 0;
	m_nSampleRate = 0;
	m_enChannelMode = MP3CM_STEREO;
	m_enEmphasis = MP3EM_NONE;
	m_bCopyrighted = FALSE;
	m_bOriginal = FALSE;
}

// functions
CDuiString CMP3Info::GetGenreString(int nIndex)
{
	if (nIndex > g_nMP3GenreCount)
		return _T("");
	
	return g_arrMP3Genre[nIndex];
}

CDuiString CMP3Info::GetLengthString(int nSeconds)
{
    int nMin = nSeconds / 60;
    int nSec = nSeconds % 60;

	CDuiString str;
    str.Format(_T("%d:%02d"),nMin,nSec);

	return str;
}

// helper functions
void CMP3Info::ChangeEndian(void* pBuffer, int nBufSize)
{
	if (!pBuffer || !nBufSize)
		return;

	char temp;
	for (int i = 0; i < nBufSize / 2; i++)
	{
		temp = ((char*)pBuffer)[i];
		((char*)pBuffer)[i] = ((char*)pBuffer)[nBufSize - i - 1];
		((char*)pBuffer)[nBufSize - i - 1] = temp;
	}
}