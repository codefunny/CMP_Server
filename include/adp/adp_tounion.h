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
** 说明:本模板用于向核心系统发送报文
*/
int ADP_ToUnion( int eiSubID, int eiSubSeq,
               UDP epfReqList, UDP opfRspList,
               SVC_Content * epstService );

#ifdef __cplusplus
}
#endif

#endif
