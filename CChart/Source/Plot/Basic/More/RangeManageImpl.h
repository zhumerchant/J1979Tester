/*============================================================================*/
/*                                                                            */
/*                            C O P Y R I G H T                               */
/*                                                                            */
/*                          (C) Copyright 2019 by                             */
/*                              Yang Guojun                                   */
/*                           All Rights Reserved                              */
/*                                                                            */
/*      The author assumes no responsibility for the use or reliability of    */
/*      his software.                                                         */
/*                                                                            */
/*============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                  ��Ȩ����                                  //
//                         ��Ȩ����(C)2006-2019�������                       //
//                                ����ȫ��Ȩ��                                //
////////////////////////////////////////////////////////////////////////////////
/* ############################################################################################################################## */

#pragma once

#include "RangeManage.h"

Declare_Namespace_CChart

template<class PlotImplT>
class	CRangeManageImpl : public CRangeManage
{
public:
	CRangeManageImpl();
	virtual ~CRangeManageImpl();

};

template<class PlotImplT>
CRangeManageImpl<PlotImplT>::CRangeManageImpl()
{
	PlotImplT* pT = static_cast<PlotImplT*>(this);
}

template<class PlotImplT>
CRangeManageImpl<PlotImplT>::~CRangeManageImpl()
{
	PlotImplT* pT = static_cast<PlotImplT*>(this);
}

Declare_Namespace_End
