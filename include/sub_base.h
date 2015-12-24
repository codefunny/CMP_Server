/*
** file:sub_base.h
*/
#include "svc_base.h"
#ifndef __SUB_BASE_H__
#define __SUB_BASE_H__

typedef struct _SUB_Config
{
   int  iSvcId;                        /*服务号              */
   int  iMsgAppId;                     /*子服务使用报文应用号*/
   int  iHostNodeId;                   /*子服务主机节点号    */
   int  iSubSeq;                       /*内部子服务号        */
   int  iErrorHandleType;               /*错误处理类型       */
   int  iOutTime;                      /*子服务超时时间      */
   int  iRetryTime;                    /*超时重试次数        */
   int  iBatFlag;                      /*批量标志            */
   int  iBatMode;                      /*批量模式            */
   int  iLstId;                        /*链号                */
   SUB_SvcCfg stCorrSubCfg;            /*正确子服务配置      */
   SUB_SvcCfg stErrSubCfg;             /*错误子服务配置      */
} SUB_Config;

typedef struct
{
   int iReadKeyId;               /*读队列KEY值         */
   int iWriteKeyId;              /*写队列KEY值         */
} SUB_RWKey;

typedef struct _SUB_TimeCount
{
   int iTime;
   int iHostId;
} SUB_TimeCount;

#ifdef __cplusplus
extern "C" {
#endif

/*
** 功能：根据服务号和内部子服务号读子服务配置信息
** 输入参数：1 服务号
**           2 内部子服务号
** 输出参数：1 子服务配置指针
** 返回值：    0成功，其他失败
*/
int SUB_GetSubConfig( int eiSvcId, int eiInternalId,
                      SUB_Config *opstSubSvcConfig );

/*
** 功能：添加新的子服务
** 输入参数：1 子服务配置指针
**           3 读队列号
**           4 写队列号
** 输出参数：1 服务结构
** 返回值      0成功，其他失败
*/
int SUB_AddNew( SVC_Content *eopstService, SUB_Config *epstSubSvcConfig,
                int eiReadQueId, int eiWriteQueId );

/*
** 功能：根据节点号得到读写队列KEY值
** 输入参数：1 节点号
** 输出参数：1 读队列KEY值
**           2 写队列KEY值
** 返回值：    0成功，其他失败
*/
int SUB_GetKeyVal( int eiNodeId, int *opiReadKeyId, int *opiWriteKeyId );

/*
**功能：释放子服务的请求信息和响应信息链表
**输入参数：1 子服务节点指针
**输出参数：  无
**返回值：    无
*/
void SUB_Free( SUB_SvcList *epstSubServie );

#ifdef __cplusplus
}
#endif

#endif
