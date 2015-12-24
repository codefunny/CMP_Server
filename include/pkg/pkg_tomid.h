/*
**  file: pkg_tomid.h
*/

#ifndef __PKG_TOMID__
#define __PKG_TOMID__

/*
**  该文件包含的头文件
*/

#include "pkg_tomidbase.h"

#define PKG_TRANCODE_LEN 128

#ifdef __cplusplus
extern "C" {
#endif

/*
** 函数功能: 取拆包函数的交易代码的0号函数
**
** 函数名:   PKG_GetToMidTranCode0
**
** 输入参数: char * epcOrgMsg                 // 源报文
**           int    eiOrgMsgLen               // 源报文长度 
**           int    eiNodeId                  // 节点号
**           int    eiMsgDscrb                // 报文描述符
**           MsgFldCfgAry * epstMsgFldCfgAry  // 域空间起始地址
**                                             及域内记录数
**      
** 输出参数: char ** opczTranCode             // 交易代码
**         
** 返回值: int      // 0成功,其他失败
**
*/
int PKG_GetToMidTranCode0( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry * epstMsgFldCfgAry ,  
                           char *opczTranCode );

/*
** 函数功能: 取拆包函数的交易代码的0号函数
**
** 函数名:   PKG_GetToMidTranCode1
**
** 输入参数: char * epcOrgMsg                 // 源报文
**           int    eiOrgMsgLen               // 源报文长度 
**           int    eiNodeId                  // 节点号
**           int    eiMsgDscrb                // 报文描述符
**           MsgFldCfgAry * epstMsgFldCfgAry  // 域空间起始地址
**                                             及域内记录数
**      
** 输出参数: char ** opczTranCode             // 交易代码
**         
** 返回值: int      // 0成功,其他失败
** note:just only for shanghai ccb,added by yinzhuhua,2001/11/14
*/
int PKG_GetToMidTranCode1( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry * epstMsgFldCfgAry ,  
                           char *opczTranCode );

/*
**  函数功能: 将源报文转化为中间信息链表的0号函数
**
**  函数名称: PKG_ToMid0
**
**  输入参数: char * epcOrgMsg                // 源报文
**            int    eiOrgMsgLen              // 源报文的长度
**            int    iConnectId               // 连接号
**            int    eiNodeId                 // 节点号
**            int    eiMsgDscrb               // 报文描述符
**            char * epczTranCode             // 交易代码
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr                    // 中间信息链表
**
**  返回值:   int  0成功, 其他失败
*/
int  PKG_ToMid0( char * epcOrgMsg, int eiOrgMsgLen, 
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
		 MsgFldCfgAry *epstmsgFldCfgAry , 
                 UDP opfsAddr );

/*
**  函数功能: 将源报文转化为中间信息链表的0号函数
**
**  函数名称: PKG_ToMid0
**
**  输入参数: char * epcOrgMsg                // 源报文
**            int    eiOrgMsgLen              // 源报文的长度
**            int    iConnectId               // 连接号
**            int    eiNodeId                 // 节点号
**            int    eiMsgDscrb               // 报文描述符
**            char * epczTranCode             // 交易代码
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr                    // 中间信息链表
**
**  返回值:   int  0成功, 其他失败
**  Note:just only for shanghai new genaretion message package,added by yinzhuhua
**  Date:2001/11/14
*/
int  PKG_ToMid2( char * epcOrgMsg, int eiOrgMsgLen, 
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
		 MsgFldCfgAry *epstmsgFldCfgAry , 
                 UDP opfsAddr );

/*
**  函数功能: 将源报文转化为中间信息链表的1号函数(针对标准8583)
**
**  函数名称: PKG_ToMid1
**
**  输入参数: char * epcOrgMsg// 源报文
**            int    eiOrgMsgLen// 源报文的长度
**            int    iConnectId// 连接号
**            int    eiNodeId// 节点号
**            int    eiMsgDscrb// 报文描述符
**            char * epczTranCode             // 交易代码(可以填*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                              及域记录数
**  输出参数: UDP opfsAddr          // 中间信息报文
**
**  返回值:   int  0成功, 其他失败
*/
int  PKG_ToMid1( char * epcOrgMsg, int eiOrgMsgLen,
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb,
		 MsgFldCfgAry *epstmsgFldCfgAry ,
                 UDP opfsAddr );

/*
**  函数功能: 将源报文转化为中间信息链表的1号函数(针对网银)
**
**  函数名称: PKG_ToMidWY
**
**  输入参数: char * epcOrgMsg// 源报文
**            int    eiOrgMsgLen// 源报文的长度
**            int    iConnectId// 连接号
**            int    eiNodeId// 节点号
**            int    eiMsgDscrb// 报文描述符
**            char * epczTranCode             // 交易代码(可以填*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr          // 中间信息报文
**
**  返回值:   int  0成功, 其他失败
*/
int  PKG_ToMidWY( char * epcOrgMsg, int eiOrgMsgLen,
                  int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                  MsgFldCfgAry *epstMsgFldCfgAry ,
                  UDP opfsAddr );
                  
                  

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomid.h
*/
