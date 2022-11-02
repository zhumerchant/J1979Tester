// WAVFile.h: interface for the CWAVFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVFILE_H__FD21FFAC_A281_4906_BCF3_CB317D7A82F6__INCLUDED_)
#define AFX_WAVFILE_H__FD21FFAC_A281_4906_BCF3_CB317D7A82F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmreg.h>

#include <mmsystem.h>
#include <dmusicc.h>
#include <dmusici.h>
#include <dsound.h>
 

#include <string>

namespace AudioEngine {

class CWAVFile  
{
public:
  friend class CAudioManager;

  void Init();
  CWAVFile() { Init(); }
  CWAVFile(const CWAVFile &r) { Init(); Copy(r); }
	virtual ~CWAVFile();

  // format chunk
  // note: the size of these members do not correspond to the size of
  // the fields in the WAV format.
  int m_AudioFormat;
  int m_NumberOfChannels;
  int m_SampleRate;
  int m_ByteRate;
  int m_BlockAlign;
  int m_BitsPerSample;
  unsigned int m_DataLen;
  
  CWAVFile &operator=(const CWAVFile &r) { Copy(r); return(*this); }

  unsigned char *GetData() { return(m_Data); }
  void SetData(const unsigned char *data, unsigned int len);

  void Load(const unsigned char *data);

  unsigned char *Save() const; 

  int GetTotalSize() const { return(44 + m_DataLen); }

  std::string GetInfo();

protected:

  void Copy(const CWAVFile &r) {
  
  m_AudioFormat      = r.m_AudioFormat;     
  m_NumberOfChannels = r.m_NumberOfChannels;
  m_SampleRate       = r.m_SampleRate;      
  m_ByteRate         = r.m_ByteRate;        
  m_BlockAlign       = r.m_BlockAlign;      
  m_BitsPerSample    = r.m_BitsPerSample;   
  
  SetData(r.m_Data, r.m_DataLen);
  }

  // data chunk
  unsigned char *m_Data;

};

} // namespace
#endif // !defined(AFX_WAVFILE_H__FD21FFAC_A281_4906_BCF3_CB317D7A82F6__INCLUDED_)
