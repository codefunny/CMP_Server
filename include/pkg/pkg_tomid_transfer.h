/*
**  file: pkg_tomid_transfer.h
*/

#ifndef __PKG_TOMID_TRANSFER_H_
#define __PKG_TOMID_TRANSFER_H_

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif
int PKG_ToMid_Transfer(  char * epcOrgMsg, int eiOrgMsgLen,
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
