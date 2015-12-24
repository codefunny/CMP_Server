/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG_BC__
#define __PKG_TOMSG_BC__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"
#include "pkg_base_cup20.h"

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_8583_2( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );

 
#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
