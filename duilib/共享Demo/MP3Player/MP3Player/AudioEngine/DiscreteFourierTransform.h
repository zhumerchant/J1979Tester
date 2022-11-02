#if !defined(AFX_DISCRETEFOURIERTRANSFORM_H__1F33E87C_F389_4D68_B492_41DFBBB0DD8F__INCLUDED_)
#define AFX_DISCRETEFOURIERTRANSFORM_H__1F33E87C_F389_4D68_B492_41DFBBB0DD8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <dsound.h>


namespace AudioEngine {

#include <math.h>
#define M_PI 3.1415926536

class CDiscreteFourierTransform
{
public:

  CDiscreteFourierTransform(int bufsize, int samplerate, int bitspersample, int channels);
  virtual ~CDiscreteFourierTransform();

  void LoadInputData(unsigned char *data);
  void PerformSlowFourierTransform();

  double GetAmp(int ndx)  { return(ndx > m_BufSize ? 0.0 : m_AmpOutput[ndx]); }
  double GetFreq(int ndx) { return(ndx > m_BufSize ? 0.0 : m_FreqOutput[ndx]); }

  int m_SampleRate;
  int m_NumChannels;
  int m_BitsPerSample;
  
protected:

  int m_BufSize;
  double *m_Input;
  double *m_FreqOutput;
  double *m_AmpOutput;
};

} // namespace

#endif 
