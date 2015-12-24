
/*
**  file: pkg_api.h
*/

#ifndef __PKG_API__
#define __PKG_API__

/*
**  该文件包含的头文件
*/

#include "pkg_tomid.h"
#include "pkg_tomsg.h"

/* 定义拆包取交易代码函数指针结构 */
typedef struct 
{
   char * pczFunName;
   int    (*pfnFun)( char * epcOrgMsg, int eiOrgMsgLen, 
                     int eiNodeId, int eiMsgDscrb, 
                     MsgFldCfgAry *epstMsgFldCfgAry,
                     char *opczTranCode );
}PKG_GET_TOMID_TRANCODE;

/* 定义拆包函数指针 */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( char * epcOrgMsg, int eiOrgMsgLen,
                     int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
                     MsgFldCfgAry *epstMsgFldCfgAry, UDP opsAddr );
}PKG_TOMID;

/* 定义打包取交易代码函数指针结构 */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( UDP epfsAddr,
                     int eiNodeId, int eiMsgDscrb,
                     char ** opczTranCode ); 
}PKG_GET_TOMSG_TRANCODE;

/* 定义打包函数指针结构 */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( char * opcOutMsg, int * opiOutMsgLen,
                     int eiNodeid, char * epczTranCode, int eiMsgDscrb,  
                     MsgFldCfgAry *pstMsgFldCfgAry, UDP epsAddr );
}PKG_TOMSG;

#ifdef __cplusplus
extern "C" {
#endif

/* 函数功能: 得到所有函数的Id号 */
int  PKG_GetFunId( int eiNodeId, 
                   int * opiToMidTranCodeId, int * opiToMsgTranCodeId, 
                   int * opiToMidId, int * opiToMsgId );

/* 函数功能: 取得拆包函数交易代码 */
int PKG_GetToMidTranCode( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen,
                          int eiNodeId, int eiMsgDscrb, 
                          MsgFldCfgAry * opstMsgFldCfgAry,
                          char *opczTranCode );

/* 函数功能: 将源报文转换为中间信息链表 */
int  PKG_ToMid( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen, int eiNodeId,
                char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry, UDP opfsAddr );

/* 函数功能: 取得打包函数交易代码 */
int  PKG_GetToMsgTranCode( int eiFunId, UDP epfsAddr,  
                           int eiNodeId, int eiMsgDscrb, char ** opczTranCode );

/* 函数功能: 将中间信息链表转换为输出报文 */
int  PKG_ToMsg( int eiFunId, char * opcOutMsg, int * opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry, UDP epfsAddr );

int PKG_Disp8583Pkg (char * pczTitle, char * pczBuffer, int iPktLen);

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_api.h
*/
