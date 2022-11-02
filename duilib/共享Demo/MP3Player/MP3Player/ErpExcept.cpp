/********************************************************************
  Class Name: ERPException
  Created Date:20080905
  Author: wuding
  Definition file: ErpExcept.cpp
  Last Version Date: 
  Version Date:
********************************************************************/
#include "stdafx.h"
#include "ErpExcept.h"

ERPException::ERPException(EGrade nGrade,const char *pFormat,...)
              :mGrade(nGrade),
              nErrCode(0) 
{
      va_list pArg;
      va_start(pArg, pFormat);
      _vsnprintf(m_str,MAX_EXCEPTION_LEN, pFormat, pArg);
      va_end(pArg);
}

ERPException::ERPException(EGrade nGrade, int nCode, const char *pFormat,...)
                :mGrade(nGrade),
                nErrCode(nCode)
{
      va_list pArg;
      va_start(pArg, pFormat);
      _vsnprintf(m_str,MAX_EXCEPTION_LEN, pFormat, pArg);
      va_end(pArg);
}
ERPException::~ERPException()
{
}
 
const char *ERPException::GetErrorMsg()
{
     return m_str;
}

int ERPException::GetErrorCode()
{
     return nErrCode;
}

EGrade ERPException::GetErrorGrade()
{
     return mGrade;
}
