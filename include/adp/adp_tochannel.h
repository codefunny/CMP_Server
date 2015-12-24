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
** 说明：本模板用于向柜台渠道发错误响应
*/
int ADP_ToChanErr_GT( int eiSubID, int eiSubSeq,
                     UDP epfReqList, UDP opfRspList,
                     SVC_Content * epstService );

/*
**说明:本函数用于往柜台渠道发正确响应
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
