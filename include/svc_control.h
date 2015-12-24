/*
** file:   svc_basic.h
*/
#include "svc_base.h"

#ifndef __SERVICE_CONTROL_H__
#define __SERVICE_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** 功能：准备服务相关的参数，同时设置第0个子服务
** 输入参数：1 服务请求链表
**           2 渠道节点
**           3 报文应用号
**           4 交易代码
**           5 读队列KEY值
**           6 写队列KEY值
** 输出参数：1 服务响应链表
**           2 服务指针
** 返回值：    0成功，其他失败
*/
int SVC_Prepare( UDP epfReqList, int eiChanId, int eiPkgId,
                 char *epczTranCode, SVC_Content *opstService );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
