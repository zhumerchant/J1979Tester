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

#ifndef __NLFIT_32167_H__
#define __NLFIT_32167_H__

//�������
double sign(double val);
//��Gauss-Jordan��ȥ�������Է�����
void gaussj(double *a,int n,double *b,int &flag);
//�в�ƽ���ͼ���
double sum(double *pA,int cnum,double *pX,double *pY,int len,double *pErr);
//���������������
void nlfit(double *pA, double *pAbsErr, int cnum, double *pX, double *pY, int len, double &err, int iter, int method, int &fail);

typedef double	(FcnRemain)( double *pA,int cnum,double *pX,double *pY,int no );
typedef void	(FcnDerivative)( double *pA,double *pDiff,int cnum,double *pX,int no );
typedef	double	(FcnFitFunction)( double *, int, double );

void initfunc(FcnDerivative *der, FcnRemain *rem);

#endif