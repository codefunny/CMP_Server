/*
**    chk_api.h
**
**    本文件定义了条件路由模块的数据结构和宏定义
**
*/

#ifndef __CHK_API__
#define __CHK_API__

#include "global.h"
#include "chk.h"
#include "svc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************
功能: 对渠道服务号、当前子服务内部号，
      对服务信息链表进行条件检查 
输入: int      eiSvcId          渠道服务号
      int      eiCurSubSvcseq   当前子服务内部号
      SVC_Content *epstsvcList  服务信息链表
      int      eiNum            批处理序号( 从0开始 )
输出: int      *oiNextsubSvcseq 下一级子服务内部号
返回: 0   成功
      !=0 失败
***************************************************/
int CHK_GetSubSvcSeq( int eiSvcId,
                      int eiCurSubSvcSeq,
                      SVC_Content *epstSvcList,
                      int eiNum,
                      int *oiNextSubSvcSeq );

#ifdef __cplusplus
}
#endif

#endif
