/*
**  file: pkg_tomid_tms.h
*/

#ifndef __PKG_TOMID_TMS_H_
#define __PKG_TOMID_TMS_H_

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif
int PKG_ToMid_Tms(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr );
#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomid_tms.h
*/
