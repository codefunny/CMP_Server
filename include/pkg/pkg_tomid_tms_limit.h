/*
**  file: pkg_tomid_yhtransfer.h
*/

#ifndef __PKG_TOMID_YHTRANSFER_H_
#define __PKG_TOMID_YHTRANSFER_H_

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif
int PKG_ToMid_LimitTms(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr );
#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomid_yhtransfer.h
*/
