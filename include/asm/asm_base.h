/*
**  file    : asm_base.h
*/

#ifndef __ASM_BASE_H__
#define __ASM_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "global.h"
#include "fldanaly.h"
#include "tool_base.h"
#include "svc_base.h"
#include "pub_base.h"
#include "tool_datetime.h"
#include "udo.h"
#include "shm_base.h"

#define ASM_UDPSIZE 24576 /*24K*/
#define ASM_DATESIZE 14
#define MAX_STR_NODE_LEN 4096

#define ASM_SUCC 0
#define ASM_FAIL 501
#define ASM_NO_MEM 502
#define ASM_CRT_REQLIST_ERR 503
#define ASM_CRT_CFGLIST_ERR 504
#define ASM_TYPE_ERR 505
#define ASM_SRCTYPE_ERR 506
#define ASM_GetSrc_FAIL 507
#define ASM_SQL_FAIL    508

/* 子服务请求参数配置信息的结构 */
typedef struct _ReqCfgNode
{
   int        iFldSeq;          /*子服务报文域编号        */
   int        iFldType;         /*子服务报文域类型        */
   int        iRepFldFlag;      /*重复域标识              */
   int        iRepTmSubSeq;     /*重复次数域所在子服务序号*/
   int        iRepTmFld;        /*重复域的重复次数域域号  */
   int        iRepTmType;       /*重复次数域的数据类型    */
   int        iRepTmMsgDscrb;   /*重复次数域报文类型      */
   int        iFldFragCalFlag;  /*域碎片计算类型          */ 
   int        iFldAsmId;        /*域重组ID                */
}ReqCfgNode;

/* 子服务域来源配置信息的结构 */
typedef struct _FldSrcCfgNode
{
   int        iFldFragSeq;      /*子服务报文域内编号*/
   int        iSrcInfType;      /*来源信息类型*/
   int        iSrcSubSeq;       /*来源子服务内部序号*/
   int        iSrcFldSeq;       /*来源子服务报文域编号*/
   int        iSrcRepFldFlag;   /*来源重复域标识*/
   int        iSrcBeginPos;     /*起始位置*/
   int        iSrcLength;       /*起始长度*/
   char       aczSrcNote[41];   /*其他信息来源*/ 
}FldSrcCfgNode;

/*****************************************
typedef struct _ReqCfgList
{
   ReqCfgNode stReqCfgNode;
   struct _ReqCfgList * pNext;
}ReqCfgList;

typedef struct _FldSrcCfgList
{
   FldSrcCfgNode stFldSrcCfgNode;
   struct _FldSrcCfgList * pNext;
}FldSrcCfgList;
*****************************************/

/* 子服务请求参数配置信息链表 */
typedef struct _ReqCfgAry
{
   int iNum;
   ReqCfgNode * pstReqCfgNode;
}ReqCfgAry;

/* 子服务域来源配置信息链表 */
typedef struct _FldSrcCfgAry
{
   int iNum;
   FldSrcCfgNode * pstFldSrcCfgNode;
}FldSrcCfgAry;

#ifdef __cplusplus
extern "C" {
#endif

/*
**  函数功能: 创建子服务请求参数配置信息链表
**  函数名:   ASM_CreateReqCfgAry
**  输入参数: 1 服务号
**            2 子服务序号
**            3 是否冲正标识
**  输出参数: 子服务请求参数配置信息链表头
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_CreateReqCfgAry( int eiSvcId, int eiSubSeq, int eiBalFlag,
                           ReqCfgAry * opstReqCfgAry );




/*
**  函数功能: 将一个域来源配置信息插入链表
**  函数名:   ASM_AddToFldSrcCfgAry
**  输入参数: 子服务域来源配置信息链表头
**            子服务域来源配置信息节点
**  输出参数: 子服务域来源配置信息链表头
**  返回值: int 0 成功，其他失败
*/
int  ASM_AddToFldSrcCfgAry( FldSrcCfgAry * eopstFldSrcCfgAry,
                            FldSrcCfgNode * epstFldSrcCfgNode );

/*
**  函数功能: 根据子服务号定位到一个链表
**  函数名:   ASM_GetSrcList
**  输入参数: 来源子服务序号
**            服务信息链表
**            子服务响应或者请求标志,1-请求,2-响应
**  输出参数: 无
**  返回值:   子服务响应链表
*/
UDP ASM_GetSrcList( int eiSrcSubSeq, SVC_Content * epstService,
                    int eiDirectFlag );


/*
**  函数功能: 打印子服务请求参数配置信息链表各个域的内容到标准输出
**  函数名:   ASM_ReqCfgAryDisplay
**  输入参数: 子服务请求参数配置信息链表首地址
**  输出参数: 无
**  返回值:   无
*/
void ASM_ReqCfgAryDisplay( ReqCfgAry * epstReqCfgAry );


/*
**  函数功能: 创建子服务域来源配置信息链表
**  函数名:   ASM_CreateFldSrcCfgAry
**  输入参数: 域重组Id号
**  输出参数: 子服务域来源配置信息链表头
**  返回值:   int  0 成功, 其他失败
*/
int ASM_CreateFldSrcCfgAry( int eiFldAsmId, 
                             FldSrcCfgAry * opstFldSrcCfgAry );

/*
**  函数功能: 取得重复次数
**  函数名:   ASM_GetRepTime
**  输入参数: 1 服务信息链表
**            2 请求配置信息链表节点
**  输出参数: 重复次数
**  返回值:   int  0 成功, 其他失败
*/
int ASM_GetRepTime( SVC_Content * epstService, 
                    ReqCfgNode * epstReqCfgNode,
                    int * opiRepTime );


/*
**  函数功能: 创建来源域值链表
**  函数名:   ASM_CreateFldSrcValList
**  输入参数: 1 服务信息链表
**            2 域来源配置信息链表
**            3 重复序号
**  输出参数: 1 来源域值链表节点数
**            2 来源域值链表
**  返回值:   int  0 成功, 其他失败
*/
int ASM_CreateFldSrcValList( SVC_Content * epstService, 
                             FldSrcCfgAry * pstFldSrcCfgAry,
                             int iRepSerial, 
                             int * opiNum,
                             UDP opcSrcList ); 

/*
**功能:根据EasyLink返回码得到服务返回码
**输入:1 服务的渠道节点编号
**     2 EasyLink返回码值(以字符串方式提供,由调用者自己进行转换)
**输出:1 服务返回码
**返回 0-成功,1-没有定义主机节点或渠道节点返回码对应
*/
int ASM_GetCodeByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetCode );

/*
**功能:根据EasyLink返回码得到服务返回信息
**输入:1 服务的渠道节点编号
**     2 EasyLink返回码值(以字符串方式提供,由调用者自己进行转换)
**输出:1 服务返回码
**返回 0-成功,1-没有定义主机节点或渠道节点返回码对应
*/
int ASM_GetMesgByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetMesg );

/*
**功能:根据输入链表,产生正确的SQL语句
**输入:1 条件链表
**输出:1 SQL语句
**返回:0-正确,其他错误
*/
int ASM_CreateSql( UDP epcSrcList, char *opczSqlStr );

/*
**功能: 取域描述符对应的类型,现在支持FT_INT,FT_SHORT,FT_LONG,FT_STRING,FT_FLOAT
        和FT_DOUBLE
**输入: 1 域描述符
**输出:   无
**返回    域类型
*/
int ASM_GetFieldType( char *epczFldDesc );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: asm_base.h
*/
