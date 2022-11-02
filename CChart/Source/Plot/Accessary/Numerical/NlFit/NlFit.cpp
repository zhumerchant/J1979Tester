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

//#include "stdafx.h"
#include "nlfit.h"
#include <math.h>
#include <memory>

//�в�������
double remain(double *pA,int cnum,double *pX,double *pY,int no);
//ƫ�����������
void derivative(double *pA,double *pDiff,int cnum,double *pX,int no);

static FcnRemain		*Fcn_NlFitRemain = remain;
static FcnDerivative	*Fcn_NlFitDerivative = derivative;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double sign(double val)
{
	if(val > 0.0)
		return 1.0;
	else if(val < 0.0)
		return -1.0;
	else
		return 0.0;
}

//��Gauss-Jordan��ȥ�������Է�����
//flag:0����;1����
void gaussj(double *a,int n,double *b,int &flag)
{
	int i,j,k,l,ll,irow,icol;
	double big,pivinv,dum;
    //int ipiv[50], indxr[50], indxc[50];
	int *ipiv=new int[n];
	int *indxr=new int[n];
	int *indxc=new int[n];
    for (j=0;j<=n-1;j++)
	{
        ipiv[j]=0;
    }
    for (i=0;i<=n-1;i++)
	{
        big=0.0;
        for (j=0;j<=n-1;j++)
		{
            if(ipiv[j]!=1)
			{
                for(k=0;k<=n-1;k++)
				{
					if(ipiv[k]==0)
					{
						if(fabs(a[j*n+k])>=big)
						{
							big=fabs(a[j*n+k]);
							irow=j;
							icol=k;
						}
						else if(ipiv[k]>1)
						{
							//cout<<"singular matrix";
							flag=1;
							return;
						}
					}
                }
            }
        }
        ipiv[icol]=ipiv[icol]+1;
        if(irow!=icol)
		{
            for(l=0;l<=n-1;l++)
			{
                dum=(a[irow*n+l]);
                a[irow*n+l]=a[icol*n+l];
                a[icol*n+l]=dum;
            }
            dum=b[irow];
            b[irow]=b[icol];
            b[icol]=dum;
		}
        indxr[i]=irow;
        indxc[i]=icol;
        if(a[icol*n+icol]==0.0)
		{
			//cout<< "singular matrix.";
			flag=1;
			return;
		}
        pivinv=1.0/(a[icol*n+icol]);
        a[icol*n+icol]=1.0;
        for(l=0;l<=n-1;l++)
		{
            a[icol*n+l]=a[icol*n+l]*pivinv;
        }
        b[icol]=b[icol]*pivinv;
        for(ll=0;ll<=n-1;ll++)
        {
            if(ll!=icol)
			{
                dum=a[ll*n+icol];
                a[ll*n+icol]=0.0;
                for(l=0;l<=n-1;l++)
				{
                    a[ll*n+l]=a[ll*n+l]-a[icol*n+l]*dum;
                }
                b[ll]=b[ll]-b[icol]*dum;
            }
        }
	}	
    for(l=n-1;l<=0;l--)
	{
        if(indxr[l]!=indxc[l])
		{
            for(k=0;k<=n-1;k++)
			{
                dum=a[k*n+indxr[l]];
                a[k*n+indxr[l]]=a[k*n+indxc[l]];
                a[k*n+indxr[l]]=dum;
            }
        }
    }

	delete []indxc;
	delete []indxr;
	delete []ipiv;

	flag=0;
	return;
}

//�в�ƽ���ͼ������
//pA:�������
//cnum:�����������
//pX:�Ա���
//pY:�����
//len:���ݸ���
//pErr:�в�
double sum(double *pA,int cnum,double *pX,double *pY,int len,double *pErr)
{
	int i;
	double q,err;
	err=0.0;
	for(i=0;i<len;i++)
	{
		q=(*Fcn_NlFitRemain)(pA,cnum,pX,pY,i);
		pErr[i]=q;
		err+=q*q;
	}
	return err;
}

//���������
//pA:�������,����ʼʱ����²�ֵ,����ʱ������ֵ
//cnum:�����������
//pX:�Ա���
//pY:�����
//len:���ݸ���
//err:�������
//iter:�����������
//method:��ͷ���
//0:ţ�ٷ�;1:����ѷ�
//fail:�����ɹ���־
//0:����ɹ�;��1:��������������;-2:�޷���������;-3:Gauss��ȥʧ��
void nlfit(double *pA, double *pAbsErr, int cnum, double *pX, double *pY, int len,
		   double &err, int iter, int method, int &fail)
{
	double *pA0=new double[cnum];//����ϵ����ʱ�洢
	double *matrix=new double[cnum*cnum];//������ϵ������
	double *matrix0=new double[cnum*cnum];//������ϵ�������ʼֵ
	double *b=new double[cnum];//�������Ҷ�ϵ��
	double *b0=new double[cnum];//�������Ҷ�ϵ����ʼֵ
	double *delta=new double[cnum];//������Ľ�
	double *pErr=new double[len];//�������������
	double *pDiff=new double[cnum];//����ƫ����

	int i,j,k;//ѭ������

	double d,d0;//����ѷ�����,d=0��ţ�ٷ�
	double alpha,c=10.0;//d�仯����

	double q,q0,dq;//�в�ƽ����
	int flag;//��˹��ȥ���ɹ���־

	fail=0;//���ʧ�ܱ�־

	//ȷ����ʼ��ϵ������
	for(i=0;i<cnum;i++)
	{
		pA0[i]=pA[i];
	}

	//ȷ����ʼ���������
	if(method==0)d0=0;
	else d0=0.01;

	int it=0;//��ѭ������
	int it0;//��ѭ������

	//����в�ƽ���ͣ�pErr�з��ظ������ݵ�Ĳв�
	q=sum(pA,cnum,pX,pY,len,pErr);
	q0=q;

	if(method==0)//����ţ�ٷ�
	do
	{
		it++;
		if(it>iter)//�������ѭ���������˳�
		{
			fail=-1;
			break;
		}
		//���㷽�����ϵ������
		for(j=0;j<cnum;j++)
		{
			for(i=0;i<cnum;i++)
			{
				matrix0[j*cnum+i]=0;
			}
			b0[j]=0;
		}
		for(k=0;k<len;k++)//�Ը������ݵ�ѭ��
		{
			//���Ȳ���ƫ��������
			(*Fcn_NlFitDerivative)(pA,pDiff,cnum,pX,k);
			for(j=0;j<cnum;j++)//����ѭ��
			{
				for(i=0;i<cnum;i++)//ÿ����ѭ��
				{
					matrix0[j*cnum+i]+=pDiff[j]*pDiff[i];
				}
				b0[j]+=pErr[k]*pDiff[j];
			}
		}
		for(j=0;j<cnum;j++)
		{
			pA[j]=pA0[j];
			for(i=0;i<cnum;i++)
			{
				matrix[j*cnum+i]=matrix0[j*cnum+i];
			}
			b[j]=b0[j];
		}
		//��cnumԪ1�η�����
		gaussj(matrix,cnum,b,flag);
		if(flag==1)//��˹��Ԫ�ⷽ����ʧ��
		{
			fail=-3;
			break;
		}
		for(i=0;i<cnum;i++)
		{
			delta[i]=b[i];
			pA[i]+=delta[i];
		}
		q=sum(pA,cnum,pX,pY,len,pErr);//����в�ƽ���ͣ�����pErr�з��زв�
		dq=q0-q;
		q0=q;
	}while( fabs(dq)>err && (fail==0) );

	else //��������ѷ�,��ѭ����ʼ
	do
	{
		it++;
		if(it>iter)//�������ѭ���������˳�
		{
			fail=-1;
			break;
		}

		//���㷽�����ϵ������
		for(j=0;j<cnum;j++)
		{
			for(i=0;i<cnum;i++)
			{
				matrix0[j*cnum+i]=0;
			}
			b0[j]=0;
		}
		for(k=0;k<len;k++)//�Ը������ݵ�ѭ��
		{
			//���Ȳ���ƫ��������
			(*Fcn_NlFitDerivative)(pA,pDiff,cnum,pX,k);
			for(j=0;j<cnum;j++)//����ѭ��
			{
				for(i=0;i<cnum;i++)//ÿ����ѭ��
				{
					matrix0[j*cnum+i]+=pDiff[j]*pDiff[i];
				}
				b0[j]+=pErr[k]*pDiff[j];
			}
		}

		//��ѭ����ʼ
		d=d0;
		alpha=-1;
		it0=0;
		do
		{
			it0++;
			if(it0>iter)//�������ѭ���������˳�
			{
				fail=-2;
				break;
			}
			for(j=0;j<cnum;j++)
			{
				pA[j]=pA0[j];
				for(i=0;i<cnum;i++)
				{
					matrix[j*cnum+i]=matrix0[j*cnum+i];
				}
				b[j]=b0[j];
			}
			//Ϊ������ϵ������������������
			d=d0*pow(c,alpha);
			for(i=0;i<cnum;i++)
			{
				matrix[i*cnum+i]*=(1+d);
			}
			//��cnumԪ1�η�����
			gaussj(matrix,cnum,b,flag);
			if(flag==1)
			{
				fail=-3;
				break;
			}
			for(i=0;i<cnum;i++)
			{
				delta[i]=b[i];
				pA[i]+=delta[i];
			}
			q=sum(pA,cnum,pX,pY,len,pErr);//����в�ƽ���ͣ�����pErr�з��زв�
			alpha++;//���q>q0,ֻ������d
		}while(q>q0);
		dq=q0-q;
		q0=q;
		d0=d;
		for(j=0;j<cnum;j++)
		{
			pA0[j]=pA[j];
		}
	}while( (dq>err) && (fail==0) );

	err = dq;
	memcpy(pAbsErr, delta, cnum*sizeof(double));
	
	delete []pDiff;
	delete []pErr;
	delete []b0;
	delete []b;
	delete []delta;
	delete []matrix0;
	delete []matrix;
	delete []pA0;
}

//�в�������
//������a1+a2*exp(-( (x-a3)/a4 )^2/2)
//pA:�������
//cnum:�����������
//pX:�Ա���
//pY:�����
//no:���
double remain(double *pA,int cnum,double *pX,double *pY,int no)
{
	double a1,a2,a3,a4;
	double val,x,y;

	a1=pA[0];
	a2=pA[1];
	a3=pA[2];
	a4=pA[3];

	x=pX[no];
	y=pY[no];

	val=a1+a2*exp(-pow((x-a3)/a4,2)/2);

	return y-val;//���������������val-y�Ļ��Ͳ���
/*	double val,x,y;
	int i;

	x=pX[no];
	y=pY[no];

	val=pA[0];
	for(i=1;i<cnum;i+=3)
	{
		val+=pA[i]*exp(-pow((x-pA[i+1])/pA[i+2],2)/2);
	}
	return y-val;*/
}

//��ϵ����ƫ�����������
//������a1+a2*exp(-( (x-a3)/a4 )^2/2)
//pA:�������
//pDiff:ƫ����
//cnum:�����������
//pX:�Ա���
//no:�������
void derivative(double *pA,double *pDiff,int cnum,double *pX,int no)
{
	double a1,a2,a3,a4;
	double x,tmp;

	a1=pA[0];
	a2=pA[1];
	a3=pA[2];
	a4=pA[3];

	x=pX[no];

	tmp=exp(-pow((x-a3)/a4,2)/2);

	pDiff[0]=1;
	pDiff[1]=tmp;
	pDiff[2]=a2*tmp*(x-a3)/(a4*a4);
	pDiff[3]=a2*tmp*(x-a3)*(x-a3)/(a4*a4*a4);

	return;
/*	double x,tmp;
	int i;

	x=pX[no];

	pDiff[0]=1;
	for(i=1;i<cnum;i++)
	{
		pDiff[i]=0;
	}
	for(i=1;i<cnum;i+=3)
	{
		tmp=exp(-pow((x-pA[i+1])/pA[i+2],2)/2);

		pDiff[i]+=tmp;
		pDiff[i+1]+=pA[i]*tmp*(x-pA[i+1])/(pA[i+2]*pA[i+2]);
		pDiff[i+2]+=pA[i]*tmp*(x-pA[i+1])*(x-pA[i+1])/(pA[i+2]*pA[i+2]*pA[i+2]);
	}

	return;*/
}

void	initfunc(FcnDerivative *der, FcnRemain *rem)
{
	Fcn_NlFitDerivative = der;
	Fcn_NlFitRemain = rem;
}