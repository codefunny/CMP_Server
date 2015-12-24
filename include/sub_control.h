/*
** file:sub_control.h
*/

#ifndef __SUB_CONTROL_H__
#define __SUB_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** 功能：运行子服务流程
** 输入参数：1 服务指针
** 输入参数：1 服务指针
** 返回值：    0正常，其他错误
*/
int SUB_StartFlow( SVC_Content *eopstService );

/*
** 功能：运行子服务错误处理流程
** 输入参数：1 服务指针
** 输入参数：1 服务指针
** 返回值：    0正常，1-错误(冲正报文重组发生错误,需检查重组配置)
*/
int SUB_ErrorFlow( SVC_Content *eopstService );

int SUB_bsearchMsgFldCfg( const void *pLeft, const void *pRight );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
