/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG_XML__
#define __PKG_TOMSG_XML__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_XML( char * opcOutMsg, int * opiOutMsgLen, 
                   int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                   MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
