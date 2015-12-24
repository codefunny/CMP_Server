#include "svc_base.h"

#ifndef __ADAPTOR_REVERSE_H__
#define __ADAPTOR_REVERSE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************
功能: 根据iErrorHandleType进行错误处理
      SUB_ERR_TRANLOG 组织冲正记录, 写入冲正表
输入: int            eiSubSvcId        子服务号
      int            eiSubSeq          子服务内部序号
      UDP            epfSubSvcReq      子服务请求信息
      UDP            opfSubSvcRsp      子服务响应信息
      SVC_Content    *epstService
输出: 无
返回: int 0-成功  其他-失败
****************************************************/
int ADP_WriteBalLog( int eiSubSvcId,
                     int eiSubSeq,
                     UDP epfSubSvcReq,
                     UDP opfSubSvcRsp,
                     SVC_Content *epstService );

int InsertBallog (int eiSubSvcId, int eiSubSvcSeq,
                  char * epczTranCode, SVC_Content *epstService,
                  UDP epfFss);

#ifdef __cplusplus
}
#endif


#endif
