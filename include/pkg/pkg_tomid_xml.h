/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMID_XML__
#define __PKG_TOMID_XML__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

int  PKG_ToMid_XML (char * epcOrgMsg, int eiOrgMsgLen, 
                    int eiNodeId, char *epczTranCode, int eiMsgDscrb, 
   		    MsgFldCfgAry *epstMsgFldCfgAry ,
                    UDP opfsAddr);
static int PKG_GetXmlItem (char *epcOrgMsg, char *epczItem, int iFldType,
                           char *opczValue, int *oiOffset, int *oiLen);

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
