#ifndef __ERPEXCEPTION_H
#define __ERPEXCEPTION_H

/********************************************************************
  Class Name: ERPException
  Created Date:20090805
  Version 2.0
  Author: wuding
  Definition file: ErpExcept.cpp
  Last Version Date: 20091105
  Version Date:
  vserion 2.0  �����쳣����������ǰ��̨���쳣���з���
********************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>    
#include <stdarg.h>
#include "ErpErrorDef.h"



#define MAX_EXCEPTION_LEN  4*1024

enum EGrade { EERROR=0,EWARNING,EHINT};


class ERPException
{
    public:            
        explicit ERPException(EGrade nGrade,const char *pFormat,...);
        explicit ERPException(EGrade nGrade, int nCode, const char *pFormat,...);
        ~ERPException();
    public:
        EGrade  GetErrorGrade() ;
        int   GetErrorCode();
        const char *GetErrorMsg(); 
    private:
        EGrade mGrade;             //����ȼ�
        int nErrCode;               //������
        char m_str[MAX_EXCEPTION_LEN];            //����˵�� 
};

#endif
