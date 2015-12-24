#ifndef __ADP_TO_UNIONPAY_H__
#define __ADP_TO_UNIONPAY_H__

#include "udo.h"
#include "svc_base.h"
#include "pkg/pkg_api.h"
#include "adp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** ˵��:��ģ�����������ϵͳ���ͱ���
*/
int ADP_ToUnion( int eiSubID, int eiSubSeq,
               UDP epfReqList, UDP opfRspList,
               SVC_Content * epstService );

#ifdef __cplusplus
}
#endif

#endif
