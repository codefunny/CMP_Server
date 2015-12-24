#ifndef __ADP_TO_CHANNEL_H__
#define __ADP_TO_CHANNEL_H__

#include "udo.h"
#include "svc_base.h"
#include "pkg/pkg_api.h"
#include "adp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** ˵������ģ���������̨������������Ӧ
*/
int ADP_ToChanErr_GT( int eiSubID, int eiSubSeq,
                     UDP epfReqList, UDP opfRspList,
                     SVC_Content * epstService );

/*
**˵��:��������������̨��������ȷ��Ӧ
*/
int  ADP_ToChannel_GT( int eiSubID,    int eiSubSeq,
                       UDP epfReqList, UDP opfRspList,
                       SVC_Content   * epstService );

int ADP_ChanStdFixup( int eiSubID,    int eiSubSeq, 
                       UDP epfReqList, UDP opfRspList, 
                       SVC_Content   * epstService );

#ifdef __cplusplus
}
#endif

#endif
