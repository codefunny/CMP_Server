#include <stdio.h>

#include "pkg/pkg_api.h"
#include "udo.h"
#include "svc_base.h"

#ifndef __ADP_CONTROL_H__
#define __ADP_CONTROL_H__

#define INVALID_TEMPLET_ID   -1001
#define ADP_ERR_LEVEL        2
#define ADP_DEBUG_LEVEL      4
#define ADP_ERR_LOG          "adp_tohost_err.log"
#define ADP_DEBUG_LOG        "adp_tohost_dbg.log"

#define ADDADP( funname ) { "ADP_"#funname, ADP_##funname }

typedef struct _ADP_TempletEntity
{
   char * pczFuncName;
   int  ( *pfnEntity )( int eiSubSvcId, int eiSubSeq,
                        UDP epfReqListMsg, UDP opfRespListMsg, 
                        SVC_Content * epstService);
} ADP_TempletEntity;

#ifdef __cplusplus
extern "C" {
#endif

int  ADP_RunTemplet( int eiSubSvcId, int eiTempletID,
                     int eiSubSeq,
                     UDP epfReqListMsg,
                     UDP opfRespListMsg,
                     SVC_Content * epstService );

#ifdef __cplusplus
}
#endif


#endif

/*
** end of file
*/
