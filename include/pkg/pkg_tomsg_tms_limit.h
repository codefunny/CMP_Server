/*
**  file: pkg_tomsg_transfer.h
*/

#ifndef __PKG_TOMSG_YLTRANSFER_H_
#define __PKG_TOMSG_YLTRANSFER_H_

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif
int PKG_ToMsg_LimitTms( char * opcOutMsg, int *opiOutMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );
#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomid_yhtransfer.h
*/
