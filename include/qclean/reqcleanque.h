#ifndef  _REQCLEANQUE_H_
#define  _REQCLEANQUE_H_

#include "qclean.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/***********************************************************
����:
       �ڰ�400������Ӧ��Ϣд����Ӧ��Ϣ����ǰ��
       ����Ƚ���д����������ڵĴ���(�������������ʹ��) 

input:
       iQidֵΪ�������idֵ
       mTypeֵΪ��ϢType
ret:
   	   0 �ɹ���1 ʧ��
***********************************************************/
int nReqCleanQueMsg ( int iQid, long mType );

#ifdef __cplusplus
}
#endif 

#endif
