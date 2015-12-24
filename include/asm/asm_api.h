/*
**  file    : asm_api.h
*/

#include "asm/asm_base.h"

#ifndef __ASM_API_H__
#define __ASM_API_H__

#define ADDASM( funname ) { "ASM_"#funname, ASM_##funname }

typedef struct
{
   char *OperaName;
   int  (*OperaFun)( SVC_Content *, UDP, int , int, UDP);
} OperaFunEntity;

#ifdef __cplusplus
extern "C" {
#endif

/*
**  函数功能: 生成子服务请求链表
**  函数名:   CreateSubReqList
**  输入参数: 1 服务号
**            2 子服务序号
**            3 是否冲正标识
**            4 服务信息链表
**  输出参数: 创建的子服务请求链表
**            错误信息描述，只需描述错误信息即可
**  返回值:   成功返回0，其他值失败
**  说明:     
*/
int  ASM_CreateSubReqList( int eiSvcID, int eiSubSeq, int eiBalFlag,
                           SVC_Content * epstService, UDP opstSubReqPkgList,
                           char *opcErrorMsg );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: asm_api.h
*/
