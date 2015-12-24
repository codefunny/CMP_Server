/*
** file:sub_control.c
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tool_base.h"
#include "chk_api.h"
#include "svc_base.h"
#include "sub_base.h"
#include "sub_control.h"
#include "udo.h"
#include "log_udo.h"
#include "asm/asm_api.h"
#include "adp/adp_control.h"
#include "log.h"
#include "adp/adp_base.h"

int SUB_ChkNeedAsm( int, int );

#ifdef __cplusplus
extern "C" {
#endif

extern int ADP_RunTemplet( int, int, int, UDP, UDP, SVC_Content * );
extern int LOG_WriteTranLog( SVC_Content *epstService, int eiSuccFlag );
extern int SUB_Monitor( int eiNodeId, char *epczTranCode, int eiFlag,
                        int eiSuccFlag, UDP pfList );

#ifdef __cplusplus
}
#endif

/**
  * 不要删除该文件, 监控要使用该字段
  */
int iRunTimes = 0;


TranFldCfg * SUB_SearchTranFldCfg( TranFldCfg * pstTranFldCfgBase, int iRowNum,
                                   int iNodeId, char *pczTranCode, int iType )
{
      int i;   
      TranFldCfg *pstSearch;
   
      pstSearch = pstTranFldCfgBase;
      for( i = 0; i < iRowNum; i++ )
      {
            if( pstSearch->iNodeId == iNodeId &&
                pstSearch->iMsgDscrb == iType &&
                strcmp( pstSearch->aczTranCode, pczTranCode ) == 0 )
            {
                  return pstSearch;
            }
            pstSearch++;
      }
      
      return NULL;
}

int SUB_bsearchMsgFldCfg( const void *pLeft, const void *pRight )
{
      MsgFldCfg *pstLeft = (MsgFldCfg *)pLeft;
      MsgFldCfg *pstRight = (MsgFldCfg *)pRight;
   
      if( pstLeft->iMsgAppId != pstRight->iMsgAppId )
      {
            return pstLeft->iMsgAppId - pstRight->iMsgAppId;
      }
      
      return pstLeft->iFldSeq - pstRight->iFldSeq;
}

int SUB_SearchFieldType( int iDictId, int iFldId )
{
      int iRowNum;
      MsgFldCfg  *pstMsgFldCfg, stMsgFldKey, *pstSearch;
   
      pstMsgFldCfg = (MsgFldCfg*)SHM_ReadData( (void*)pcShmAddr, MSGFLDCFG,
                                               &iRowNum );
      if( pstMsgFldCfg == NULL )
      {
            return -1;
      }
   
      stMsgFldKey.iMsgAppId = iDictId;
      stMsgFldKey.iFldSeq   = iFldId;
      pstSearch = (MsgFldCfg*)bsearch( &stMsgFldKey, pstMsgFldCfg, iRowNum,
                                       sizeof(stMsgFldKey),
                                       SUB_bsearchMsgFldCfg );
      if( pstSearch == NULL )
      {
            return -1;
      }
      
      return pstSearch->iFldType;
}

void SUB_AddNullValue( UDP pfList, int iFldId, int iFldType )
{
   char acUniValue[33];
   
   memset( acUniValue, 0, sizeof(acUniValue) );
   UDO_AddField( pfList, iFldId, iFldType, acUniValue, 0 );
}

/**
 * 根据节点、交易代码、数据字典和报文类型创建字段值为空的 UDO
 * @param iNodeId     节点编号
 * @param pczTranCode 交易代码
 * @param iDictId     数据字典编号
 * @param iType       交易类型
 * @param opList      产生的 UDO，每个字段值都为空
 */
void SUB_PrepareResponse( int iNodeId, char *pczTranCode, int iDictId,
                           int iType, UDP opList )
{
      char aczTranFld[1024], *p1, *p2;
      int iRecNum, iTemp;
      TranFldCfg *pstTranFldCfg, *pstResultTranFldCfg;
      
      /**
       * 第一步根据节点、交易代码和报文类型查找字段配置
       */
      pstTranFldCfg = (TranFldCfg *)SHM_ReadData( (void*)pcShmAddr,
                                               TRANFLDCFG, &iRecNum );
   pstResultTranFldCfg = SUB_SearchTranFldCfg( pstTranFldCfg, iRecNum,
                                               iNodeId, pczTranCode, iType );
   if( pstResultTranFldCfg == NULL )
   {
         LOG4C ((LOG_FATAL, "未找到相应的报文组成定义"
                 "节点=[%d] 交易码=[%s] 报文类型=[%d]",
                 iNodeId, pczTranCode, iType ));
         return;
   }

   strcpy( aczTranFld, pstResultTranFldCfg->aczTranFld );
   p1 = aczTranFld;
   do
   {
         char aczField[12];
         int  iFldType, iFldId;
         p2 = strchr( p1, ',' );
         if( p2 != NULL )
         {
               memcpy( aczField, p1, (int)(p2 - p1) );
               aczField[p2-p1] = 0;
         }
         else
         {
               strcpy( aczField, p1 );
         }
         iFldId = atoi(aczField);
         iFldType = SUB_SearchFieldType( iDictId, iFldId );
         SUB_AddNullValue( opList, iFldId, iFldType );
         if( p2 != NULL )
         {
            p1 = p2 + 1;
         }
   }while( p2 != NULL );

   return;
}

/*
** 功能：运行子服务流程
** 输入参数：1 服务指针
** 输入参数：1 服务指针
** 返回值：    0正常,其他错误
** 说明：所有发生系统错误的时候都只把系统错误原因放在服务的错误原因字段里
** 并且立即返回，并设置系统错误类型
*/
int SUB_StartFlow( SVC_Content *eopstService )
{
   char aczErrorMessage[128];
   int iCurInternalId, iNextInternalId, iTempletId, iError, iDoAsm;
   int iRet, iReadKeyId, iWriteKeyId, iHandleSysError, iHasRunError;
   int iTplRet, iErrorHandleType;
   int iReformError, iHasReformError;   /** 重组错误标志: 0-正常, 1-错误 **/
   UDP pfReqList, pfAnswList;
   SUB_Config stSubSvcConfig;

   LOG4C((LOG_DEBUG, "进入子服务流程控制模块"));

   iRunTimes++;
   iCurInternalId = 0;
   iHandleSysError = 0;
   iHasRunError = 0;
   iHasReformError = 0;
   /*首先默认系统执行上一个子服务正常*/
   iTplRet = 0;

   SUB_Monitor( eopstService->iChannelId, 
                eopstService->stSubSvcHead.aczTranCode, 1, 0,
                eopstService->stSubSvcHead.pfAnswList );

   memset( eopstService->aczErrorMessage, 0,
           sizeof(eopstService->aczErrorMessage) );
   /*下面运行这个子服务,直到为最后一个子服务为止*/
   do
   {
         iReformError = 0;
      if( eopstService->iErrorType != 0 )
      {
         if( iHandleSysError == 1 )
         {
            LOG4C ((LOG_FATAL,
                    "已经调用了系统错误处理子服务0,"
                    "请检查配置和系统错误过程"));

            break;
         }
         LOG4C ((LOG_FATAL,
                 "服务处理发生系统错误."
                 "服务号=[%d],子服务内部序号=[%d],请检查配置",
                 eopstService->iSvcId, iCurInternalId));

         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|系统错误", CMP_NODE_ID, CMP_SYS_ERR );
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }

      LOG4C ((LOG_DEBUG,
              "开始决策流程下一子服务."
              "当前服务号=[%d],子服务内部序号=[%d]",
              eopstService->iSvcId, iCurInternalId ));

      /*根据这个子服务执行的结果取得下一个内部子服务号*/
      /*实际配置中都有一个缺省值,不会发生错误         */
      iRet = CHK_GetSubSvcSeq( eopstService->iSvcId, iCurInternalId,
                               eopstService, 0, &iNextInternalId );
      if( iRet != 0 )
      {
         eopstService->iErrorType = -2;
         LOG4C ((LOG_FATAL,
                 "流程决策失败,调用系统错误处理."
                 "当前服务号=[%d],子服务内部序号=[%d]",
                 eopstService->iSvcId, iCurInternalId));

         /*如果没有发起错误流程处理,开始错误处理,并改变标记*/
         if( iHasRunError == 0 )
         {
            /*发生系统错误错误,开始错误流程处理*/
            SUB_ErrorFlow( eopstService );
         }
         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|条件检查错误", CMP_NODE_ID, CMP_CHK_ERR );
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }
      /*下一子服务号检查完,下一个内部子服务号变成当前子服务号*/

      LOG4C ((LOG_DEBUG,
              "流程下一子服务内部序号=[%d]."
              "当前服务号=[%d],子服务内部序号=[%d]",
               iNextInternalId,
               eopstService->iSvcId, iCurInternalId));

      iCurInternalId = iNextInternalId;
      memset( &stSubSvcConfig, 0, sizeof( stSubSvcConfig ) );
      iRet = SUB_GetSubConfig( eopstService->iSvcId, iCurInternalId,
                               &stSubSvcConfig );
      if( iRet != SVC_SUCC )
      {
         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|取子服务配置错误", CMP_NODE_ID,CMP_GSUBCFG_ERR );
         eopstService->iErrorType = -3;

         LOG4C ((LOG_FATAL,
                 "取子服务配置出错.服务号=[%d],子服务内部序号=[%d].",
                 eopstService->iSvcId, iCurInternalId));

         /*如果没有发起错误流程处理,开始错误处理,并改变标记*/
         if( iHasRunError == 0 )
         {
            /*发生系统错误错误,开始错误流程处理*/
            SUB_ErrorFlow( eopstService );
         }
         /*设置CMP服务为一个系统错误子服务*/
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }

       LOG4C ((LOG_DEBUG,
              "完成取子服务配置信息,增加新的子服务."
              "服务号=[%d],子服务内部序号=[%d].",
               eopstService->iSvcId, iCurInternalId));

      /*增加新的子服务节点,并填写配置信息,实际情况不会发生错误*/
      iRet = SUB_AddNew( eopstService, &stSubSvcConfig,
                         iReadKeyId, iWriteKeyId );


      pfReqList = eopstService->pstSubSvcTail->pfReqList;
      pfAnswList = eopstService->pstSubSvcTail->pfAnswList;


      iErrorHandleType = eopstService->pstSubSvcTail->iErrorHandleType;
/***
      LOG4C ((LOG_DEBUG,
              "完成增加新的子服务.服务号=[%d],子服务内部序号=[%d]",
              eopstService->iSvcId, iCurInternalId));
****/
      /*到主机子服务和正常返回子服务需要报文重组*/
      if( iCurInternalId > 0 || 
          ( iCurInternalId < 0 && iErrorHandleType != SVC_ERR_RETURN ) )
      {
         iDoAsm = 1;
      }
      else
      {
         iDoAsm = 0;
      }

      if( iDoAsm == 1 )
      {
         LOG4C ((LOG_DEBUG,
                "重组子服务请求."
                "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId));
         memset( aczErrorMessage, 0, sizeof(aczErrorMessage) );


         iRet = ASM_CreateSubReqList( eopstService->iSvcId, iCurInternalId,
                                      0, eopstService, pfReqList,
                                      aczErrorMessage );
		
         if( iRet != SVC_SUCC )
         {
            LOG4C ((LOG_FATAL,
                    "子服务报文重组失败,"
                    "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                    eopstService->iSvcId, iCurInternalId,
                    stSubSvcConfig.stCorrSubCfg.iSubId));

            eopstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
            iError = -9999999;
            /*如果没有发起错误流程处理,开始错误处理,并改变标记*/
            if( iHasRunError == 0 )
            {
               /*发生系统错误错误,开始错误流程处理*/
               SUB_ErrorFlow( eopstService );
               iHasRunError = 1;
            }
#if 0
                strchr(strchr(eopstService->aczErrorMessage,'|')+1,'|')== NULL)           
#endif
            if( strlen( aczErrorMessage) == 0 )
            {
               sprintf( eopstService->aczErrorMessage,
                        "%d|%d|子服务报文重组错误", CMP_NODE_ID, CMP_ASM_ERR );
            }
            else
            {
               sprintf( eopstService->aczErrorMessage,
                        "%d|%d|%s", CMP_NODE_ID, CMP_ASM_ERR, aczErrorMessage );
            }

            /**
             * 只有对流程中非叶子节点增加空的响应UDO
             */
            if( iCurInternalId > 0 )
            {
               UDO_ACField( pfAnswList, -100, UDT_INT, (char*)&iError, 0 );
               /*UDO_ACField( pfAnswList, -101, UDT_INT, "报文重组错", 10 );*/
               UDO_ACField( pfAnswList, -101, UDT_STRING, "报文重组错", 10 );
            }

            iHandleSysError = 1;
            iReformError = 1;
            iHasReformError = 1;
         }
         LOG4C ((LOG_DEBUG,
                 "完成子服务请求重组."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId));
      }
      else
      {
         LOG4C ((LOG_DEBUG,
                 "子服务不需要重组请求."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId));
      }

      iTempletId = stSubSvcConfig.stCorrSubCfg.iTempletId;
      eopstService->pstSubSvcTail->iSubId = stSubSvcConfig.stCorrSubCfg.iSubId;
      eopstService->pstSubSvcTail->iTempletId =
                                        stSubSvcConfig.stCorrSubCfg.iTempletId;
      eopstService->pstSubSvcTail->iRecordsPerTime =
                                   stSubSvcConfig.stCorrSubCfg.iRecordsPerTime;
      strcpy( eopstService->pstSubSvcTail->aczTranCode,
              stSubSvcConfig.stCorrSubCfg.aczSubTranCode );
      strcpy( eopstService->pstSubSvcTail->aczSubName,
              stSubSvcConfig.stCorrSubCfg.aczSubName );

RunTemplet:
      LOG4C ((LOG_DEBUG,
              "开始执行子服务适配器."
              "服务号=[%d],子服务内部序号=[%d],子服务号=[%d],适配器号=[%d]",
              eopstService->iSvcId, iCurInternalId,
              stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

      if( iTempletId == 0 )
      {
         LOG4C((LOG_FATAL,
                "CMP发生系统错误,执行系统错误子服务适配器"));
         pfReqList = UDO_Alloc( 4096 );
         pfAnswList = UDO_Alloc( 4096 );
      }
      /** 监控 **/
      if( iNextInternalId > 0 )
      {
         /** 子服务请求 **/
         SUB_Monitor( stSubSvcConfig.iHostNodeId,
                      stSubSvcConfig.stCorrSubCfg.aczSubTranCode,
                      1, 0, pfReqList );
      }
      
      /**
       * 当重组正常时执行指定的适配器
       */
      if( iReformError == 0 || iCurInternalId < 0 )
      {
         /*判断子服务执行结果，并记错误信息到响应链表*/
         if( iHasReformError == 1 && iCurInternalId < 0 )
         {
               eopstService->pstSubSvcCurr->iErrorHandleType = SVC_ERR_RETURN;
         }
         iTplRet = ADP_RunTemplet( stSubSvcConfig.stCorrSubCfg.iSubId,
                                   stSubSvcConfig.iSubSeq, iTempletId,
                                   pfReqList, pfAnswList, eopstService );
         LOG4C ((LOG_DEBUG,
                 "执行子服务适配器结束."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d],适配器号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

      }
      else
      {
            /** 重组失败且不是最后一个子服务时创建子服务空的响应UDO **/
            if( iNextInternalId > 0 )
            {
               UDO_Prt2File(LOG_INFO, "commudo.log", pfReqList,
                         "To Host Node [%d], SvcId [%d]\n%s\n",
                          stSubSvcConfig.iHostNodeId, eopstService->iSvcId,
                          "" );
            }
            else
            {
                  UDO_Prt2File( LOG_INFO, "commudo.log", pfReqList,
                          "To Channel Node [%d], SvcId [%d]\n%s\n",
                          eopstService->iChannelId, eopstService->iSvcId,
                          "");
            }
            
            if( iNextInternalId > 0 )
            {
                  /**
                   * 根据节点、交易代码、数据字典编号和报文类型(默认为正常响应)
                   * 创建空的 UDO
                   */
                  LOG4C((LOG_DEBUG, "系统重组发生错误设置默认响应UDO"));
                  SUB_PrepareResponse( stSubSvcConfig.iHostNodeId,
                                       stSubSvcConfig.stCorrSubCfg.aczSubTranCode,
                                       stSubSvcConfig.iMsgAppId, 2, pfAnswList );
                  UDO_Prt2File(LOG_INFO, "commudo.log", pfAnswList,
                         "From Host Node [%d], SvcId [%d]\n%s\n",
                          stSubSvcConfig.iHostNodeId, eopstService->iSvcId,
                          "" );
            }
            iTplRet = -9999;
      }

      if( iNextInternalId > 0 )
      {
         /** 子服务响应 **/
         SUB_Monitor( stSubSvcConfig.iHostNodeId,
                      stSubSvcConfig.stCorrSubCfg.aczSubTranCode, 2,
                      (iTplRet==0)? 0: 1, pfAnswList );
      }
      else
      {
         /** 服务响应 **/
         SUB_Monitor( eopstService->iChannelId,
                      eopstService->stSubSvcHead.aczTranCode,
                      2, (iHasRunError==0)? 0: 1, pfReqList );
      }

      if( iTplRet != 0 )
      {
         LOG4C ((LOG_FATAL,
                 "子服务适配器执行错误,结果=[%d]."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d],适配器号=[%d]",
                 iTplRet, eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

         UDO_ACField( pfAnswList, -101, UDT_STRING,
                       eopstService->aczErrorMessage,
                       strlen( eopstService->aczErrorMessage ) );
         if( ( eopstService->pstSubSvcTail->iStartErrorFlow == 1 ) &&
             ( iHasRunError == 0 ) )
         {
            /*发生系统错误错误,开始错误流程处理*/
            LOG4C ((LOG_FATAL, "服务执行失败.开始错误流程.服务号=[%d]",
                    eopstService->iSvcId));

            SUB_ErrorFlow( eopstService );
            iHasRunError = 1;
         }
      }
      else
      {
         UDO_ACField( pfAnswList, -101, UDT_STRING, "", 1 );
      }
      /*用-100保存子服务执行结果,为检查下一个子服务号做准备,这里要求子服务返回*/
      /*码有很明确的定义,要区分系统错误和应用错误*/
      LOG4C((LOG_DEBUG, "返回结果:[%d]", iTplRet ));
      UDO_ACField( pfAnswList, -100, UDT_INT, (char *)&iTplRet, 0 );
      if( iTempletId == 0 )
      {
         UDO_Free( pfReqList );
         UDO_Free( pfAnswList );
      }
   } while( ( iNextInternalId > 0 ) && ( iTempletId > 0 ) );

   /*LOG_WriteTranLog( eopstService, 1 - iHasRunError );*/

   LOG4C ((LOG_DEBUG, "====================流程执行完.服务号=[%d]==================", eopstService->iSvcId));
   LOG4C ((LOG_DEBUG, "************************************************************" ));
   LOG4C ((LOG_DEBUG, "************************************************************" ));
   LOG4C ((LOG_DEBUG, "************************************************************" ));


   return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/* 为减少对头文件的依赖, 引用外部定义 */
extern int  ADP_WriteBalLog( int eiSubSvcId,  int eiSubSeq,
                             UDP epfSubSvcReq,
                             UDP opfSubSvcRsp,
                             SVC_Content *epstService );
#ifdef __cplusplus
}
#endif


int SUB_ErrorFlow( SVC_Content *eopstService )
{
   int iStatus, iCurInternalId, iRet;
   SUB_SvcList  *pstTempSubSvc;
   char aczErrorMessage[SVC_ERRMSG_LEN];
   
   assert( eopstService != NULL );

   strcpy( aczErrorMessage, eopstService->aczErrorMessage );
   iStatus = 0;
   /*首先设置当前子服务为第一个内部子服务*/
   pstTempSubSvc = eopstService->stSubSvcHead.pstNextSubSvc;
   while( pstTempSubSvc != NULL && pstTempSubSvc->iSubSvcSeq >0 )
   {
      eopstService->pstSubSvcCurr = pstTempSubSvc;
      iCurInternalId = pstTempSubSvc->iSubSvcSeq;
      /*错误处理子服务信息已经由正常子服务流程初始化完成*/


      if( pstTempSubSvc->iErrorHandleType != SUB_ERR_NOOP )
      {
         do
         {
            pstTempSubSvc->pfBalanceList = UDO_Alloc( 20 * 1024 );
         } while( pstTempSubSvc->pfBalanceList == NULL );

         LOG4C ((LOG_DEBUG,
                 "重组子服务错误处理请求."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 pstTempSubSvc->stErrSubCfg.iSubId));

         iRet = ASM_CreateSubReqList( eopstService->iSvcId, iCurInternalId,
                                      1, eopstService,
                                      pstTempSubSvc->pfBalanceList,
                                      aczErrorMessage );
         if( iRet != SVC_SUCC )
         {
            LOG4C ((LOG_FATAL,
                    "子服务报文重组失败,"
                    "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                    eopstService->iSvcId, iCurInternalId,
                    pstTempSubSvc->stErrSubCfg.iSubId));

            pstTempSubSvc->iErrorHandleType = SUB_ERR_TRANLOG;
         }

         LOG4C ((LOG_DEBUG,
                 "开始错误处理."
                 "服务号=[%d],子服务内部序号=[%d],子服务号=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 pstTempSubSvc->stErrSubCfg.iSubId));

         eopstService->pstSubSvcCurr->iSubId = pstTempSubSvc->stErrSubCfg.iSubId;
         eopstService->pstSubSvcCurr->iTempletId = pstTempSubSvc->stErrSubCfg.iTempletId;
         eopstService->pstSubSvcCurr->iRecordsPerTime = pstTempSubSvc->stErrSubCfg.iRecordsPerTime;
         strcpy( eopstService->pstSubSvcCurr->aczTranCode,
                 pstTempSubSvc->stErrSubCfg.aczSubTranCode );
         strcpy( eopstService->pstSubSvcCurr->aczSubName,
                 pstTempSubSvc->stErrSubCfg.aczSubName );
         /* 首先判断是否立即冲正, 如果是, 根据冲正的结果决定是否记冲正日志 */
         if( pstTempSubSvc->iErrorHandleType == SUB_ERR_BALANCE )
         {
            iRet = ADP_RunTemplet( pstTempSubSvc->stErrSubCfg.iSubId,
                                   pstTempSubSvc->iSubSvcSeq,
                                   pstTempSubSvc->stErrSubCfg.iTempletId,
                                   pstTempSubSvc->pfBalanceList,
                                   pstTempSubSvc->pfReqList,
                                   eopstService );
            LOG4C ((LOG_DEBUG,
                   "服务%d内部子服务%d子服务%d错误处理对应的适配"
                   "器 %d 执行结果 %d ",
                    eopstService->iSvcId, iCurInternalId,
                    pstTempSubSvc->stErrSubCfg.iSubId,
                    pstTempSubSvc->stErrSubCfg.iTempletId, iRet));
         }
         if( pstTempSubSvc->iErrorHandleType == SUB_ERR_TRANLOG ||
             (pstTempSubSvc->iErrorHandleType == SUB_ERR_BALANCE && iRet!=0 ) )
         {
            /* 本版本的服务号是15 */
            iRet = ADP_WriteBalLog( pstTempSubSvc->stErrSubCfg.iSubId,
                                    pstTempSubSvc->iSubSvcSeq,
                                    pstTempSubSvc->pfBalanceList,
                                    pstTempSubSvc->pfReqList,
                                    eopstService );
            LOG4C((LOG_DEBUG,
                   "服务%d内部子服务%d子服务%d记冲正日志的适配器%d"
                   "执行结果%d",
                   eopstService->iSvcId, iCurInternalId,
                   pstTempSubSvc->stErrSubCfg.iSubId, iRet));
         }
      }
      else
      {
         LOG4C ((LOG_DEBUG,
                 "服务%d内部子服务%d发生错误不做错误处理",
                 eopstService->iSvcId, iCurInternalId));
      }
      pstTempSubSvc = pstTempSubSvc->pstNextSubSvc;
   }

   if (pstTempSubSvc != NULL) {
      eopstService->pstSubSvcCurr = pstTempSubSvc;
   }

   /* 恢复服务初始错误信息 */
   strcpy( eopstService->aczErrorMessage, aczErrorMessage );
   return iStatus;
}

/*
**功能:根据服务号和当前内部子服务号确定结束子服务是否需要重组
**输入:1 服务号
**     2 当前内部子服务号
**输出:  无
**返回:  0-不需要重组,1-需要重组
*/
int SUB_ChkNeedAsm( int eiSvcId, int eiCurInternalId )
{
   int iRowNum, iTemp;
   ServiceCfg *pstServiceCfg;

   pstServiceCfg = SHM_ReadData( pcShmAddr, SERVICECFG, &iRowNum );
   for( iTemp = 0; iTemp < iRowNum; iTemp++ )
   {
      if( (pstServiceCfg->iSvcId == eiSvcId) &&
          (pstServiceCfg->iParentSubSeq == eiCurInternalId) &&
          (pstServiceCfg->iSubSeq > 0) )
      {
         return 0;
      }
      pstServiceCfg++;
   }

   return 1;
}

/*
** end of file
*/


