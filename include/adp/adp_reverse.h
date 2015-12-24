#include "svc_base.h"

#ifndef __ADAPTOR_REVERSE_H__
#define __ADAPTOR_REVERSE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************
����: ����iErrorHandleType���д�����
      SUB_ERR_TRANLOG ��֯������¼, д�������
����: int            eiSubSvcId        �ӷ����
      int            eiSubSeq          �ӷ����ڲ����
      UDP            epfSubSvcReq      �ӷ���������Ϣ
      UDP            opfSubSvcRsp      �ӷ�����Ӧ��Ϣ
      SVC_Content    *epstService
���: ��
����: int 0-�ɹ�  ����-ʧ��
****************************************************/
int ADP_WriteBalLog( int eiSubSvcId,
                     int eiSubSeq,
                     UDP epfSubSvcReq,
                     UDP opfSubSvcRsp,
                     SVC_Content *epstService );

int InsertBallog (int eiSubSvcId, int eiSubSvcSeq,
                  char * epczTranCode, SVC_Content *epstService,
                  UDP epfFss);

#ifdef __cplusplus
}
#endif


#endif
