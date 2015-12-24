#ifndef __ADP_TO_CHANNEL_SCRIPT_H__
#define __ADP_TO_CHANNEL_SCRIPT_H__

#include "udo.h"
#include "svc_base.h"
#include "pkg/pkg_api.h"
#include "adp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

int  ADP_ToBankcomm( int eiSubID,    int eiSubSeq,
                       UDP epfReqList, UDP opfRspList,
                       SVC_Content   * epstService );

int  ADP_ToYacol( int eiSubID,    int eiSubSeq,
                       UDP epfReqList, UDP opfRspList,
                       SVC_Content   * epstService );


int ADP_ToOther( int eiSubID,    int eiSubSeq, 
                       UDP epfReqList, UDP opfRspList, 
                       SVC_Content   * epstService );

int ADP_ToPos( int eiSubID,    int eiSubSeq, 
                       UDP epfReqList, UDP opfRspList, 
                       SVC_Content   * epstService );



#ifdef __cplusplus
}
#endif

#endif
