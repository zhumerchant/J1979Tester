#include "stdafx.h"
#include "WAVFile.h" 
#include "RefCountPtr.h"

#include <mmsystem.h> // FOURCC
#include <io.h>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>

using namespace std;

namespace AudioEngine {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CWAVFile::Init()
{
  m_Data = NULL;
  m_AudioFormat = 0;
  m_BitsPerSample = 0;
  m_BlockAlign = 0;
  m_ByteRate = 0;
  m_NumberOfChannels = 0;
  m_SampleRate = 0;
  m_DataLen = 0;
}

CWAVFile::~CWAVFile()
{
  if (NULL != m_Data) { delete[] m_Data; m_Data = NULL; m_DataLen = 0;}

}

void CWAVFile::SetData(const unsigned char *data, unsigned int len)
{
  if (NULL != m_Data) { delete[] m_Data; m_Data = NULL; m_DataLen = 0;}
  m_Data = new unsigned char[len];
  memcpy(m_Data, data, len);
  m_DataLen = len;
}

                                                         
void CWAVFile::Load(const unsigned char *data)
{
  FOURCC riff, wave, fmt, datacc;
  
  // check for RIFF ChunkID
  memcpy(&riff, &data[0], sizeof(FOURCC)); 
  if (riff != mmioFOURCC('R', 'I', 'F', 'F')) 
  { 
	  //Throw("CWAVFile::Load: invalid WAV file data (missing RIFF header)."); 
  }

  // check for WAVE format
  memcpy(&wave, &data[8], sizeof(FOURCC)); 
  if (wave != mmioFOURCC('W', 'A', 'V', 'E'))  
  { 
	  //Throw("CWAVFile::Load: invalid WAV file data (not WAVE format).");
  }

  // check for FMT ChunkID
  memcpy(&fmt, &data[12], sizeof(FOURCC)); 
  if (fmt  != mmioFOURCC('f', 'm', 't', ' ')) 
  { 
	  //Throw("CWAVFile::Load: invalid WAV file data (FMT subchunk not found)."); 
  }

  // check for DATA ChunkID
  memcpy(&datacc, &data[36], sizeof(FOURCC)); 
  if (datacc != mmioFOURCC('d', 'a', 't', 'a'))  
  { 
	  //Throw("CWAVFile::Load: invalid WAV file data (DATA subchunk not found)."); 
  }

  // if we get here, we're pretty sure it's a WAV file.

  // load in data members
  memcpy(&m_AudioFormat,      &data[20], sizeof(short int));
  memcpy(&m_BitsPerSample,    &data[34], sizeof(short int));
  memcpy(&m_BlockAlign,       &data[32], sizeof(short int));
  memcpy(&m_ByteRate,         &data[28], sizeof(int));
  memcpy(&m_NumberOfChannels, &data[22], sizeof(short int));
  memcpy(&m_SampleRate,       &data[24], sizeof(short int));

  unsigned int datasize=0;
  memcpy(&datasize, &data[40], sizeof(int));
  SetData(&data[44], datasize);
}

unsigned char *CWAVFile::Save() const
{
  unsigned char *dest = NULL;
  int savesize = GetTotalSize();

  // allocate memory
  dest = new unsigned char[savesize];

  int       i32=0; // 32 bits
  short int i16=0; // 16 bits

  FOURCC fourcc = 0;
  
  // populate main chunk

  // chunk ID
  fourcc = mmioFOURCC('R', 'I', 'F', 'F'); 
  memcpy(&dest[ 0], &fourcc, sizeof(FOURCC));

  // ChunkSize
  i32 = 36 + m_DataLen;
  memcpy(&dest[4], &i32, sizeof(int));

  // format
  fourcc = mmioFOURCC('W', 'A', 'V', 'E'); 
  memcpy(&dest[ 8], &fourcc, sizeof(FOURCC));

  // populate fmt subchunk

  // chunk ID
  fourcc = mmioFOURCC('f', 'm', 't', ' '); 
  memcpy(&dest[12], &fourcc, sizeof(FOURCC));

  // ChunkSize
  i32 = 16;
  memcpy(&dest[16], &i32, sizeof(int));

  // audio format
  i16 = m_AudioFormat;
  memcpy(&dest[20], &i16, sizeof(short int));

  // number of channels
  i16 = m_NumberOfChannels;
  memcpy(&dest[22], &i16, sizeof(short int));

  // sample rate
  i32 = m_SampleRate;
  memcpy(&dest[24], &i32, sizeof(int));

  // byte rate
  i32 = m_ByteRate;
  memcpy(&dest[28], &i32, sizeof(int));

  // block align
  i16 = m_BlockAlign;
  memcpy(&dest[32], &i16, sizeof(short int));

  // bits per sample
  i16 = m_BitsPerSample;
  memcpy(&dest[34], &i16, sizeof(short int));

  // populate data subchunk

  // chunk ID
  fourcc = mmioFOURCC('d', 'a', 't', 'a'); 
  memcpy(&dest[36], &fourcc, sizeof(FOURCC));

  // chunk size
  i32 = m_DataLen;
  memcpy(&dest[40], &i32, sizeof(int));

  // data
  memcpy(&dest[44], m_Data, m_DataLen);

  return(dest);
} 

string CWAVFile::GetInfo()
{
  stringstream s;

  // print out its stats
  s << "Audio Format: " << m_AudioFormat << endl;
  s << "Bits per Sample: " << m_BitsPerSample << endl;
  s << "Block Align: " << m_BlockAlign << endl;
  s << "Byte Rate: " << m_ByteRate << endl;
  s << "Number of Channels: " << m_NumberOfChannels;

  switch(m_NumberOfChannels) {
    case 1: s << " (mono)"; break;
    case 2: s << " (stereo)"; break;
    default: s << " (multichannel)"; break;
  }
  s << endl;

  s << "Sample Rate: " << m_SampleRate << endl;
  s << "Data Length: " << m_DataLen << endl;
  s << "Duration (seconds): " << (float)m_DataLen / (float)m_ByteRate;

  return(s.str());
}

}; // namespace