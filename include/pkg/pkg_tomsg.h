/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG__
#define __PKG_TOMSG__

/*
**  该文件包含的头文件
*/

#include "pkg_tomsgbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 函数功能: 取打包函数交易代码的0号函数 */
int PKG_GetToMsgTranCode0( UDP epfsAddr, 
                           int eiNodeId, int eiMsgDscrb,
                           char ** opczTranCode );

/* 函数功能: 从中间信息链表转换成输出报文的0号函数 */
int PKG_ToMsg0( char * opcOutMsg, int * opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

/* 函数功能: 从中间信息链表转换成输出报文的1号函数 */
int PKG_ToMsg1( char * opcOutMsg, int * opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

/* 函数功能: 从中间信息链表转换成输出报文的1号函数 */
int PKG_ToMsg2( char * opcOutMsg, int * opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
