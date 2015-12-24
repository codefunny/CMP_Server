#ifndef  _REQCLEANQUE_H_
#define  _REQCLEANQUE_H_

#include "qclean.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/***********************************************************
功能:
       在把400返回响应信息写入响应消息队列前，
       完成先将其写入清理队列内的处理(供队列清理进程使用) 

input:
       iQid值为清理队列id值
       mType值为信息Type
ret:
   	   0 成功，1 失败
***********************************************************/
int nReqCleanQueMsg ( int iQid, long mType );

#ifdef __cplusplus
}
#endif 

#endif
