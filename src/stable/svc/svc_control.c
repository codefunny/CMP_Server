/*
** file:svc_control.c
*/
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "tool_base.h"
#include "svc_base.h"
#include "sub_control.h"
#include "sub_base.h"
#include "svc_control.h"
#include "log.h"

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
int SVC_Prepare( UDP epfReqList, int eiChanId, int eiPkgAppId,
                 char *epczTranCode, SVC_Content *opstService )
{
   Service stServiceCfg;
   int iRet;
   time_t tSysSeconds;
   pid_t shPid;

   assert( epfReqList   != NULL );
   assert( epczTranCode != NULL );
   assert( opstService  != NULL );
/**
   LOG4C ((LOG_DEBUG, "enter the service control model"));
****/
   /*根据渠道节点，报文应用号和交易代码读内部服务配置*/
   iRet = SVC_GetConfig( eiChanId, eiPkgAppId, epczTranCode, &stServiceCfg );
   if( iRet != SVC_SUCC )
   {
     sprintf( opstService->aczErrorMessage,
              "%d||%d|系统错误",
              CMP_NODE_ID, CMP_SYS_ERR);

     LOG4C ((LOG_FATAL,
               "没有此服务.渠道结点号=[%d],交易码=[%s],报文应用号=[%d]",
                eiChanId, epczTranCode, eiPkgAppId));

      opstService->iErrorType = -1;
   }
   else
   {
      opstService->iErrorType = 0;
   
      /*初始化结构同时初始化第0个子服务结构*/
      iRet = SVC_Init( opstService, epfReqList, &stServiceCfg, eiPkgAppId );
      if( iRet != SVC_SUCC )
      {
         LOG4C ((LOG_FATAL,
                 "初始化服务结构出错."
                 "渠道结点号=[%d],交易码=[%s],报文应用号=[%d]",
                 eiChanId, epczTranCode, eiPkgAppId));

         opstService->iErrorType = -1;
      }
      strcpy( opstService->stSubSvcHead.aczTranCode, epczTranCode );
      opstService->iChannelId = eiChanId;
      time( &tSysSeconds );
      shPid = getpid();
      /* 流水号=当前秒数+pid+运行次数 */
      sprintf( opstService->aczElSerial, "%010d%05d%05d",
               (int)tSysSeconds, (short)shPid, 
                1);
               /*gpstProcess->shRunTimes );*/
      /*创建交易要素日志链表*/
      /*保存服务的交易要素信息*/

/**********************************************************************
      LOG_ChgLogFss( opstService, opstService->pfLogList, 2 );
      
      TOOL_MonInit  ();
      TOOL_MonSendMsg (eiChanId, epfReqList, 1, opstService->aczElSerial);
***************************/
   }
  
   /*下面开始子服务控制流程,子服务运行过程中设置子服务运行链号*/
   iRet = SUB_StartFlow( opstService );
   if( iRet != SVC_SUCC )
   {
      LOG4C ((LOG_FATAL,
              "服务流程运行结果出错."
              "渠道结点号=[%d],交易码=[%s],报文应用号=[%d]",
               eiChanId, epczTranCode, eiPkgAppId));

      iRet =  SVC_SUBFLOW_ERR;
   }

   /*释放服务使用的相关内存*/
   SVC_Free( opstService );

   return iRet;
}

/*
**功能：释放服务占用的空间
**输入参数：1 服务指针
**输出参数：  无
**返回值：    无
*/
void SVC_Free( SVC_Content *epstService )
{
   SUB_SvcList *pstTemp;

   assert( epstService != NULL );

   /*总是释放第一个子服务，然后将第二个子服务变为第一个子服务*/
   for( pstTemp = epstService->stSubSvcHead.pstNextSubSvc;
        pstTemp != NULL;
        pstTemp = epstService->stSubSvcHead.pstNextSubSvc )
   {
      epstService->stSubSvcHead.pstNextSubSvc = pstTemp->pstNextSubSvc;
      SUB_Free( pstTemp );
      free( pstTemp );
   }

   /*释放第0个子服务的链表空间*/
   SUB_Free( &epstService->stSubSvcHead );

   /*释放服务的请求和响应信息链表,交易要素日志链表*/
   UDO_Free( epstService->pfReqList );
   UDO_Free( epstService->pfAnswList );
   UDO_Free( epstService->pfLogList );
}

/*
** end of file
*/

