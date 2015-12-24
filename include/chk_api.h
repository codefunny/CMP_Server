/*
**    chk_api.h
**
**    ���ļ�����������·��ģ������ݽṹ�ͺ궨��
**
*/

#ifndef __CHK_API__
#define __CHK_API__

#include "global.h"
#include "chk.h"
#include "svc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************
����: ����������š���ǰ�ӷ����ڲ��ţ�
      �Է�����Ϣ�������������� 
����: int      eiSvcId          ���������
      int      eiCurSubSvcseq   ��ǰ�ӷ����ڲ���
      SVC_Content *epstsvcList  ������Ϣ����
      int      eiNum            ���������( ��0��ʼ )
���: int      *oiNextsubSvcseq ��һ���ӷ����ڲ���
����: 0   �ɹ�
      !=0 ʧ��
***************************************************/
int CHK_GetSubSvcSeq( int eiSvcId,
                      int eiCurSubSvcSeq,
                      SVC_Content *epstSvcList,
                      int eiNum,
                      int *oiNextSubSvcSeq );

#ifdef __cplusplus
}
#endif

#endif
