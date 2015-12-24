/*
** file: sub_base.c
*/
#include <stdio.h>
#include <stdlib.h>

#include "tool_base.h"
#include "sub_base.h"
#include "svc_base.h"
#include "global.h"
#include "udo.h"
#include "shm_base.h"
#include "log.h"

int SUB_SearchSvcCfg( const void *p1, const void *p2 )
{
   ServiceCfg *pstL = (ServiceCfg *)p1;
   ServiceCfg *pstR = (ServiceCfg *)p2;

   if( pstL->iSvcId != pstR->iSvcId )
   {
      return pstL->iSvcId - pstR->iSvcId;
   }
   else
   {
      return pstL->iSubSeq - pstR->iSubSeq;
   }
}

int SUB_SearchSubSvc( const void *p1, const void *p2 )
{
   SubService *pstL = (SubService *)p1;
   SubService *pstR = (SubService *)p2;

   return pstL->iSubId - pstR->iSubId;
}

/*
** 功能: 根据服务号和内部子服务号读子服务配置信息
** 输入参数: 1 服务号
**           2 内部子服务号
** 输出参数: 1 子服务配置指针
** 返回值:     0成功,其他失败
*/
int SUB_GetSubConfig( int eiSvcId, int eiInternalId,
                      SUB_Config *opstSubSvcConfig )
{
   int  iTemp, iRowNum, iAssignTime;
   ServiceCfg *pstServiceCfg, *pstSCfg, stSCfgKey;
   SubService *pstSubService, *pstSSvc, stSSvcKey;

   assert( opstSubSvcConfig != NULL );

   pstServiceCfg = (ServiceCfg *)SHM_ReadData( pcShmAddr, SERVICECFG,
                                               &iRowNum );
   stSCfgKey.iSvcId = eiSvcId;
   stSCfgKey.iSubSeq = eiInternalId;
   pstSCfg = bsearch( &stSCfgKey, pstServiceCfg, iRowNum, sizeof(stSCfgKey),
                      SUB_SearchSvcCfg );
   if( pstSCfg == NULL )
   {
      LOG4C ((LOG_FATAL,
              "末找到相应服务组成配置,服务号=[%d],子服务内部序号=[%d]",
              eiSvcId, eiInternalId));

      return SUB_GETCFG_ERR;
   }
   pstServiceCfg = pstSCfg;
   opstSubSvcConfig->iSvcId           = pstServiceCfg->iSvcId;
   opstSubSvcConfig->iHostNodeId      = pstServiceCfg->iHostNodeId;
   opstSubSvcConfig->iSubSeq          = pstServiceCfg->iSubSeq;
   opstSubSvcConfig->iOutTime         = pstServiceCfg->iOutTime;
   opstSubSvcConfig->iRetryTime       = pstServiceCfg->iRetryTime;
   opstSubSvcConfig->iBatFlag         = pstServiceCfg->iBatFlag;
   opstSubSvcConfig->iBatMode         = pstServiceCfg->iBatMode;
   opstSubSvcConfig->iLstId           = pstServiceCfg->iLstId;
   opstSubSvcConfig->iErrorHandleType = pstServiceCfg->iErrorHandleType;

   /**
     * 查找正常服务子服务信息
     */
   stSSvcKey.iSubId = pstServiceCfg->iCorrSubId;
   pstSubService= (SubService*)SHM_ReadData( pcShmAddr, SUBSERVICE, &iRowNum );
   pstSSvc = bsearch( &stSSvcKey, pstSubService, iRowNum, sizeof(stSSvcKey),
                      SUB_SearchSubSvc );
   if( pstSSvc == NULL )
   {
      LOG4C ((LOG_FATAL,
              "对应的子服务配置未找到,"
              "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
               eiSvcId, eiInternalId,  pstServiceCfg->iCorrSubId));

      return SUB_GETCFG_ERR;
   }
   strcpy( opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
           pstSSvc->aczTranCode);
   opstSubSvcConfig->stCorrSubCfg.iSubId = pstSSvc->iSubId;
   opstSubSvcConfig->iMsgAppId = pstSSvc->iMsgAppId;
   opstSubSvcConfig->stCorrSubCfg.iTempletId = pstSSvc->iTempletId;
   opstSubSvcConfig->stCorrSubCfg.iRecordsPerTime = pstSSvc->iRecordPerTime;
   strcpy( opstSubSvcConfig->stCorrSubCfg.aczSubName, pstSSvc->aczSubName );

   /**
     * 如果错误类型是需要同步冲正(主机子服务)
     * 找冲正子服务配置
     */
   if( pstServiceCfg->iErrorHandleType != SUB_ERR_NOOP && eiInternalId > 0 )
   {
      stSSvcKey.iSubId = pstServiceCfg->iErrSubId;
      pstSSvc = bsearch( &stSSvcKey, pstSubService, iRowNum, sizeof(stSSvcKey),
                         SUB_SearchSubSvc );
      if( pstSSvc == NULL )
      {
         LOG4C ((LOG_FATAL,
                 "对应的错误处理子服务配置未找到,"
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eiSvcId, eiInternalId,  pstServiceCfg->iErrSubId));

         return SUB_GETCFG_ERR;
      }
      strcpy( opstSubSvcConfig->stErrSubCfg.aczSubTranCode,
              pstSSvc->aczTranCode);
      opstSubSvcConfig->stErrSubCfg.iSubId = pstSSvc->iSubId;
      opstSubSvcConfig->stErrSubCfg.iTempletId = pstSSvc->iTempletId;
      opstSubSvcConfig->stErrSubCfg.iRecordsPerTime = pstSSvc->iRecordPerTime;
      strcpy( opstSubSvcConfig->stErrSubCfg.aczSubName, pstSSvc->aczSubName );

      /*调试信息,将服务的当前子服务配置打印出来*/
      LOG4C ((LOG_DEBUG, "服务%d的内部子服务号%d配置信息: \n主机节点%d,批量标"
                "志%d批量模式%d,数据字典号%d,错误处理类型%d,交易代码%s,子服务"
                "名%s,子服务号%d,适配器%d,冲正交易代码%s,冲正子服务名%s,冲正"
                "子服务号%d,冲正适配器%d,超时时间%d",
                eiSvcId, eiInternalId,
                opstSubSvcConfig->iHostNodeId, opstSubSvcConfig->iBatFlag,
                opstSubSvcConfig->iBatMode, opstSubSvcConfig->iMsgAppId,
                opstSubSvcConfig->iErrorHandleType,
                opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
                opstSubSvcConfig->stCorrSubCfg.aczSubName,
                opstSubSvcConfig->stCorrSubCfg.iSubId,
                opstSubSvcConfig->stCorrSubCfg.iTempletId,
                opstSubSvcConfig->stErrSubCfg.aczSubTranCode,
                opstSubSvcConfig->stErrSubCfg.aczSubName,
                opstSubSvcConfig->stErrSubCfg.iSubId,
                opstSubSvcConfig->stErrSubCfg.iTempletId,
                opstSubSvcConfig->iOutTime));
   }
   else
   {
      /*调试信息,将服务的当前子服务配置打印出来*/
      LOG4C ((LOG_DEBUG, 
              "服务%d的内部子服务号%d配置信息:"
              "主机节点%d,批量标志%d 批量模式%d, 数据字典号%d,"
              "无错误处理类型,交易代码%s,子服务名%s,子服务号%d,适配器%d",
              eiSvcId, eiInternalId,
              opstSubSvcConfig->iHostNodeId, opstSubSvcConfig->iBatFlag,
              opstSubSvcConfig->iBatMode, opstSubSvcConfig->iMsgAppId,
              opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
              opstSubSvcConfig->stCorrSubCfg.aczSubName,
              opstSubSvcConfig->stCorrSubCfg.iSubId,
              opstSubSvcConfig->stCorrSubCfg.iTempletId));
   }

   return SVC_SUCC;
}

/*
** 功能: 添加新的子服务
** 输入参数: 1 子服务配置指针
**           2 读队列号
**           3 写队列号
** 输出参数: 1 服务结构
** 返回值      0成功,其他失败
*/
int SUB_AddNew( SVC_Content *eopstService, SUB_Config *epstSubSvcConfig,
                int eiReadKeyId, int eiWriteKeyId )
{
   SUB_SvcList *pstSubSvc;

   assert( eopstService     != NULL );
   assert( epstSubSvcConfig != NULL );

   do
   {
      pstSubSvc = (SUB_SvcList *)malloc( sizeof( SUB_SvcList ) );
   }while( pstSubSvc == NULL );
   pstSubSvc->pstNextSubSvc = NULL;
   pstSubSvc->pstService    = eopstService;
   pstSubSvc->iSubSvcSeq    = epstSubSvcConfig->iSubSeq;
   pstSubSvc->iHostId       = epstSubSvcConfig->iHostNodeId;
   pstSubSvc->iMsgAppId     = epstSubSvcConfig->iMsgAppId;
   pstSubSvc->iIsBatch      = epstSubSvcConfig->iBatFlag;
   pstSubSvc->iBatchMode    = epstSubSvcConfig->iBatMode;
   pstSubSvc->iOutTime      = epstSubSvcConfig->iOutTime;   
   /**********************************************************/
   pstSubSvc->iLstId        = epstSubSvcConfig->iLstId;
   /***********************************************************/
   pstSubSvc->iReadKeyId    = eiReadKeyId;
   pstSubSvc->iWriteKeyId   = eiWriteKeyId;
   pstSubSvc->iRunResultCode= 0;
   pstSubSvc->iErrorHandleType = epstSubSvcConfig->iErrorHandleType;

   pstSubSvc->iStartErrorFlow  = 1;
   memcpy( &pstSubSvc->stCorrSubCfg, &epstSubSvcConfig->stCorrSubCfg,
            sizeof( epstSubSvcConfig->stCorrSubCfg ) );
   memcpy( &pstSubSvc->stErrSubCfg, &epstSubSvcConfig->stErrSubCfg,
            sizeof( epstSubSvcConfig->stErrSubCfg ) );
   do
   {
      pstSubSvc->pfReqList = UDO_Alloc( 50*1024 );
   } while( pstSubSvc->pfReqList == NULL );
   do
   {
      pstSubSvc->pfAnswList = UDO_Alloc( 50*1024 );
   } while( pstSubSvc->pfAnswList == NULL );
   pstSubSvc->pfBalanceList = NULL;
   /*修改子服务链表指针指向,先把新节点增加进去,然后修改尾节点指针*/
   eopstService->pstSubSvcTail->pstNextSubSvc = pstSubSvc;
   eopstService->pstSubSvcTail = pstSubSvc;
   eopstService->pstSubSvcCurr = pstSubSvc;

   return SVC_SUCC;
}

/*
**功能：释放子服务的请求信息和响应信息链表
**输入参数：1 子服务节点指针
**输出参数：  无
**返回值：    无
*/
void SUB_Free( SUB_SvcList *epstSubService )
{
   assert( epstSubService != NULL );

   UDO_Free( epstSubService->pfReqList );
   UDO_Free( epstSubService->pfAnswList );
   if( epstSubService->pfBalanceList != NULL )
   {
      UDO_Free( epstSubService->pfBalanceList );
   }
}

/*
** end of file
*/

