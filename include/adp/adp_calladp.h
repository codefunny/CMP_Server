#ifndef __ADP_TOHOST_H__
#define __ADP_TOHOST_H__

#include "udo.h"
#include "svc_base.h"
#include "adp_base.h"

const static int REVERSE_NO_CHANGE = 0;
const static int REVERSE_FORCE_NO  = 1;

#ifdef __cplusplus
extern "C"
{
#endif

int ADP_StdAdp_Comm( int eiSubID, int eiSubSeq,
                     UDP epfReqList, UDP opfRspList,
                     SVC_Content * epstService );


#ifdef __cplusplus
}
#endif

#endif
