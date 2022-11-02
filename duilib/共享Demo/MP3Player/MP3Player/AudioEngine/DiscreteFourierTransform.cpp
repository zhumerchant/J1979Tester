#include "stdafx.h"
#include "DiscreteFourierTransform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace AudioEngine {

CDiscreteFourierTransform::CDiscreteFourierTransform(int bufsize, int samplerate, int bitspersample, int channels) : 
  m_BufSize(bufsize), m_SampleRate(samplerate), m_BitsPerSample(bitspersample), m_NumChannels(channels)
{
  // make space for buffers
  m_Input = new double[bufsize];
  m_FreqOutput = new double[(bufsize/2)+1];
  m_AmpOutput = new double[(bufsize/2)+1];

  // we know already what the frequencies of each bin will be
  for (int bin=0; bin < m_BufSize/2; bin++) {
    m_FreqOutput[bin] = (double)bin * m_SampleRate / m_BufSize;
  }
}

CDiscreteFourierTransform::~CDiscreteFourierTransform()
{
  delete[] m_Input;
  delete[] m_FreqOutput;
  delete[] m_AmpOutput;
}

void CDiscreteFourierTransform::LoadInputData(unsigned char *data)
{
  // convert each data point from 0-255 to floating point -1..1
  if (m_BitsPerSample == 8) {
    for (int q=0; q < m_BufSize; q++) {
      float d = data[q*m_NumChannels];
      m_Input[q] = (d-127.0f)/127.0f;
      if (m_Input[q] < -1.0f) m_Input[q] = -1.0f;
      if (m_Input[q] >  1.0f) m_Input[q] =  1.0f;
    }
  }
  else if (m_BitsPerSample == 16) {
    short int *sidata = reinterpret_cast<short int *>(data);
    for (int q=0; q < m_BufSize; q++) {
      float d = sidata[q*m_NumChannels];
      m_Input[q] = (d-32767.0f)/32767.0f;
      if (m_Input[q] < -1.0f) m_Input[q] = -1.0f;
      if (m_Input[q] >  1.0f) m_Input[q] =  1.0f;
    }
  }
  else {
    // not supported
    for (int q=0; q < m_BufSize; q++) { m_Input[q] = 0.0f; }
  }
    
}

void CDiscreteFourierTransform::PerformSlowFourierTransform()
{
  for (int bin = 0; bin <= m_BufSize/2; bin++) { 
    double cosAmp = 0.0f;
    double sinAmp = 0.0f;
    for (int k = 0; k < m_BufSize; k++) { 
      double x = 2.0f * M_PI * (double)bin * (double)k / (double)m_BufSize; 
      sinAmp += m_Input[k] * sin(x); 
      cosAmp += m_Input[k] * cos(x); 
    }
    m_AmpOutput[bin] = sqrt(sinAmp*sinAmp + cosAmp*cosAmp);
  }
}

} // namespace