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
  * ��Ҫɾ�����ļ�, ���Ҫʹ�ø��ֶ�
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
 * ���ݽڵ㡢���״��롢�����ֵ�ͱ������ʹ����ֶ�ֵΪ�յ� UDO
 * @param iNodeId     �ڵ���
 * @param pczTranCode ���״���
 * @param iDictId     �����ֵ���
 * @param iType       ��������
 * @param opList      ������ UDO��ÿ���ֶ�ֵ��Ϊ��
 */
void SUB_PrepareResponse( int iNodeId, char *pczTranCode, int iDictId,
                           int iType, UDP opList )
{
      char aczTranFld[1024], *p1, *p2;
      int iRecNum, iTemp;
      TranFldCfg *pstTranFldCfg, *pstResultTranFldCfg;
      
      /**
       * ��һ�����ݽڵ㡢���״���ͱ������Ͳ����ֶ�����
       */
      pstTranFldCfg = (TranFldCfg *)SHM_ReadData( (void*)pcShmAddr,
                                               TRANFLDCFG, &iRecNum );
   pstResultTranFldCfg = SUB_SearchTranFldCfg( pstTranFldCfg, iRecNum,
                                               iNodeId, pczTranCode, iType );
   if( pstResultTranFldCfg == NULL )
   {
         LOG4C ((LOG_FATAL, "δ�ҵ���Ӧ�ı�����ɶ���"
                 "�ڵ�=[%d] ������=[%s] ��������=[%d]",
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
** ���ܣ������ӷ�������
** ���������1 ����ָ��
** ���������1 ����ָ��
** ����ֵ��    0����,��������
** ˵�������з���ϵͳ�����ʱ��ֻ��ϵͳ����ԭ����ڷ���Ĵ���ԭ���ֶ���
** �����������أ�������ϵͳ��������
*/
int SUB_StartFlow( SVC_Content *eopstService )
{
   char aczErrorMessage[128];
   int iCurInternalId, iNextInternalId, iTempletId, iError, iDoAsm;
   int iRet, iReadKeyId, iWriteKeyId, iHandleSysError, iHasRunError;
   int iTplRet, iErrorHandleType;
   int iReformError, iHasReformError;   /** ��������־: 0-����, 1-���� **/
   UDP pfReqList, pfAnswList;
   SUB_Config stSubSvcConfig;

   LOG4C((LOG_DEBUG, "�����ӷ������̿���ģ��"));

   iRunTimes++;
   iCurInternalId = 0;
   iHandleSysError = 0;
   iHasRunError = 0;
   iHasReformError = 0;
   /*����Ĭ��ϵͳִ����һ���ӷ�������*/
   iTplRet = 0;

   SUB_Monitor( eopstService->iChannelId, 
                eopstService->stSubSvcHead.aczTranCode, 1, 0,
                eopstService->stSubSvcHead.pfAnswList );

   memset( eopstService->aczErrorMessage, 0,
           sizeof(eopstService->aczErrorMessage) );
   /*������������ӷ���,ֱ��Ϊ���һ���ӷ���Ϊֹ*/
   do
   {
         iReformError = 0;
      if( eopstService->iErrorType != 0 )
      {
         if( iHandleSysError == 1 )
         {
            LOG4C ((LOG_FATAL,
                    "�Ѿ�������ϵͳ�������ӷ���0,"
                    "�������ú�ϵͳ�������"));

            break;
         }
         LOG4C ((LOG_FATAL,
                 "��������ϵͳ����."
                 "�����=[%d],�ӷ����ڲ����=[%d],��������",
                 eopstService->iSvcId, iCurInternalId));

         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|ϵͳ����", CMP_NODE_ID, CMP_SYS_ERR );
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }

      LOG4C ((LOG_DEBUG,
              "��ʼ����������һ�ӷ���."
              "��ǰ�����=[%d],�ӷ����ڲ����=[%d]",
              eopstService->iSvcId, iCurInternalId ));

      /*��������ӷ���ִ�еĽ��ȡ����һ���ڲ��ӷ����*/
      /*ʵ�������ж���һ��ȱʡֵ,���ᷢ������         */
      iRet = CHK_GetSubSvcSeq( eopstService->iSvcId, iCurInternalId,
                               eopstService, 0, &iNextInternalId );
      if( iRet != 0 )
      {
         eopstService->iErrorType = -2;
         LOG4C ((LOG_FATAL,
                 "���̾���ʧ��,����ϵͳ������."
                 "��ǰ�����=[%d],�ӷ����ڲ����=[%d]",
                 eopstService->iSvcId, iCurInternalId));

         /*���û�з���������̴���,��ʼ������,���ı���*/
         if( iHasRunError == 0 )
         {
            /*����ϵͳ�������,��ʼ�������̴���*/
            SUB_ErrorFlow( eopstService );
         }
         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|����������", CMP_NODE_ID, CMP_CHK_ERR );
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }
      /*��һ�ӷ���ż����,��һ���ڲ��ӷ���ű�ɵ�ǰ�ӷ����*/

      LOG4C ((LOG_DEBUG,
              "������һ�ӷ����ڲ����=[%d]."
              "��ǰ�����=[%d],�ӷ����ڲ����=[%d]",
               iNextInternalId,
               eopstService->iSvcId, iCurInternalId));

      iCurInternalId = iNextInternalId;
      memset( &stSubSvcConfig, 0, sizeof( stSubSvcConfig ) );
      iRet = SUB_GetSubConfig( eopstService->iSvcId, iCurInternalId,
                               &stSubSvcConfig );
      if( iRet != SVC_SUCC )
      {
         sprintf( eopstService->aczErrorMessage,
                  "%d|%d|ȡ�ӷ������ô���", CMP_NODE_ID,CMP_GSUBCFG_ERR );
         eopstService->iErrorType = -3;

         LOG4C ((LOG_FATAL,
                 "ȡ�ӷ������ó���.�����=[%d],�ӷ����ڲ����=[%d].",
                 eopstService->iSvcId, iCurInternalId));

         /*���û�з���������̴���,��ʼ������,���ı���*/
         if( iHasRunError == 0 )
         {
            /*����ϵͳ�������,��ʼ�������̴���*/
            SUB_ErrorFlow( eopstService );
         }
         /*����CMP����Ϊһ��ϵͳ�����ӷ���*/
         iTempletId = 0;
         iHandleSysError = 1;
         goto RunTemplet;
      }

       LOG4C ((LOG_DEBUG,
              "���ȡ�ӷ���������Ϣ,�����µ��ӷ���."
              "�����=[%d],�ӷ����ڲ����=[%d].",
               eopstService->iSvcId, iCurInternalId));

      /*�����µ��ӷ���ڵ�,����д������Ϣ,ʵ��������ᷢ������*/
      iRet = SUB_AddNew( eopstService, &stSubSvcConfig,
                         iReadKeyId, iWriteKeyId );


      pfReqList = eopstService->pstSubSvcTail->pfReqList;
      pfAnswList = eopstService->pstSubSvcTail->pfAnswList;


      iErrorHandleType = eopstService->pstSubSvcTail->iErrorHandleType;
/***
      LOG4C ((LOG_DEBUG,
              "��������µ��ӷ���.�����=[%d],�ӷ����ڲ����=[%d]",
              eopstService->iSvcId, iCurInternalId));
****/
      /*�������ӷ�������������ӷ�����Ҫ��������*/
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
                "�����ӷ�������."
                "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId));
         memset( aczErrorMessage, 0, sizeof(aczErrorMessage) );


         iRet = ASM_CreateSubReqList( eopstService->iSvcId, iCurInternalId,
                                      0, eopstService, pfReqList,
                                      aczErrorMessage );
		
         if( iRet != SVC_SUCC )
         {
            LOG4C ((LOG_FATAL,
                    "�ӷ���������ʧ��,"
                    "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                    eopstService->iSvcId, iCurInternalId,
                    stSubSvcConfig.stCorrSubCfg.iSubId));

            eopstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
            iError = -9999999;
            /*���û�з���������̴���,��ʼ������,���ı���*/
            if( iHasRunError == 0 )
            {
               /*����ϵͳ�������,��ʼ�������̴���*/
               SUB_ErrorFlow( eopstService );
               iHasRunError = 1;
            }
#if 0
                strchr(strchr(eopstService->aczErrorMessage,'|')+1,'|')== NULL)           
#endif
            if( strlen( aczErrorMessage) == 0 )
            {
               sprintf( eopstService->aczErrorMessage,
                        "%d|%d|�ӷ������������", CMP_NODE_ID, CMP_ASM_ERR );
            }
            else
            {
               sprintf( eopstService->aczErrorMessage,
                        "%d|%d|%s", CMP_NODE_ID, CMP_ASM_ERR, aczErrorMessage );
            }

            /**
             * ֻ�ж������з�Ҷ�ӽڵ����ӿյ���ӦUDO
             */
            if( iCurInternalId > 0 )
            {
               UDO_ACField( pfAnswList, -100, UDT_INT, (char*)&iError, 0 );
               /*UDO_ACField( pfAnswList, -101, UDT_INT, "���������", 10 );*/
               UDO_ACField( pfAnswList, -101, UDT_STRING, "���������", 10 );
            }

            iHandleSysError = 1;
            iReformError = 1;
            iHasReformError = 1;
         }
         LOG4C ((LOG_DEBUG,
                 "����ӷ�����������."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId));
      }
      else
      {
         LOG4C ((LOG_DEBUG,
                 "�ӷ�����Ҫ��������."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
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
              "��ʼִ���ӷ���������."
              "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d],��������=[%d]",
              eopstService->iSvcId, iCurInternalId,
              stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

      if( iTempletId == 0 )
      {
         LOG4C((LOG_FATAL,
                "CMP����ϵͳ����,ִ��ϵͳ�����ӷ���������"));
         pfReqList = UDO_Alloc( 4096 );
         pfAnswList = UDO_Alloc( 4096 );
      }
      /** ��� **/
      if( iNextInternalId > 0 )
      {
         /** �ӷ������� **/
         SUB_Monitor( stSubSvcConfig.iHostNodeId,
                      stSubSvcConfig.stCorrSubCfg.aczSubTranCode,
                      1, 0, pfReqList );
      }
      
      /**
       * ����������ʱִ��ָ����������
       */
      if( iReformError == 0 || iCurInternalId < 0 )
      {
         /*�ж��ӷ���ִ�н�������Ǵ�����Ϣ����Ӧ����*/
         if( iHasReformError == 1 && iCurInternalId < 0 )
         {
               eopstService->pstSubSvcCurr->iErrorHandleType = SVC_ERR_RETURN;
         }
         iTplRet = ADP_RunTemplet( stSubSvcConfig.stCorrSubCfg.iSubId,
                                   stSubSvcConfig.iSubSeq, iTempletId,
                                   pfReqList, pfAnswList, eopstService );
         LOG4C ((LOG_DEBUG,
                 "ִ���ӷ�������������."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d],��������=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

      }
      else
      {
            /** ����ʧ���Ҳ������һ���ӷ���ʱ�����ӷ���յ���ӦUDO **/
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
                   * ���ݽڵ㡢���״��롢�����ֵ��źͱ�������(Ĭ��Ϊ������Ӧ)
                   * �����յ� UDO
                   */
                  LOG4C((LOG_DEBUG, "ϵͳ���鷢����������Ĭ����ӦUDO"));
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
         /** �ӷ�����Ӧ **/
         SUB_Monitor( stSubSvcConfig.iHostNodeId,
                      stSubSvcConfig.stCorrSubCfg.aczSubTranCode, 2,
                      (iTplRet==0)? 0: 1, pfAnswList );
      }
      else
      {
         /** ������Ӧ **/
         SUB_Monitor( eopstService->iChannelId,
                      eopstService->stSubSvcHead.aczTranCode,
                      2, (iHasRunError==0)? 0: 1, pfReqList );
      }

      if( iTplRet != 0 )
      {
         LOG4C ((LOG_FATAL,
                 "�ӷ���������ִ�д���,���=[%d]."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d],��������=[%d]",
                 iTplRet, eopstService->iSvcId, iCurInternalId,
                 stSubSvcConfig.stCorrSubCfg.iSubId, iTempletId));

         UDO_ACField( pfAnswList, -101, UDT_STRING,
                       eopstService->aczErrorMessage,
                       strlen( eopstService->aczErrorMessage ) );
         if( ( eopstService->pstSubSvcTail->iStartErrorFlow == 1 ) &&
             ( iHasRunError == 0 ) )
         {
            /*����ϵͳ�������,��ʼ�������̴���*/
            LOG4C ((LOG_FATAL, "����ִ��ʧ��.��ʼ��������.�����=[%d]",
                    eopstService->iSvcId));

            SUB_ErrorFlow( eopstService );
            iHasRunError = 1;
         }
      }
      else
      {
         UDO_ACField( pfAnswList, -101, UDT_STRING, "", 1 );
      }
      /*��-100�����ӷ���ִ�н��,Ϊ�����һ���ӷ������׼��,����Ҫ���ӷ��񷵻�*/
      /*���к���ȷ�Ķ���,Ҫ����ϵͳ�����Ӧ�ô���*/
      LOG4C((LOG_DEBUG, "���ؽ��:[%d]", iTplRet ));
      UDO_ACField( pfAnswList, -100, UDT_INT, (char *)&iTplRet, 0 );
      if( iTempletId == 0 )
      {
         UDO_Free( pfReqList );
         UDO_Free( pfAnswList );
      }
   } while( ( iNextInternalId > 0 ) && ( iTempletId > 0 ) );

   /*LOG_WriteTranLog( eopstService, 1 - iHasRunError );*/

   LOG4C ((LOG_DEBUG, "====================����ִ����.�����=[%d]==================", eopstService->iSvcId));
   LOG4C ((LOG_DEBUG, "************************************************************" ));
   LOG4C ((LOG_DEBUG, "************************************************************" ));
   LOG4C ((LOG_DEBUG, "************************************************************" ));


   return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/* Ϊ���ٶ�ͷ�ļ�������, �����ⲿ���� */
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
   /*�������õ�ǰ�ӷ���Ϊ��һ���ڲ��ӷ���*/
   pstTempSubSvc = eopstService->stSubSvcHead.pstNextSubSvc;
   while( pstTempSubSvc != NULL && pstTempSubSvc->iSubSvcSeq >0 )
   {
      eopstService->pstSubSvcCurr = pstTempSubSvc;
      iCurInternalId = pstTempSubSvc->iSubSvcSeq;
      /*�������ӷ�����Ϣ�Ѿ��������ӷ������̳�ʼ�����*/


      if( pstTempSubSvc->iErrorHandleType != SUB_ERR_NOOP )
      {
         do
         {
            pstTempSubSvc->pfBalanceList = UDO_Alloc( 20 * 1024 );
         } while( pstTempSubSvc->pfBalanceList == NULL );

         LOG4C ((LOG_DEBUG,
                 "�����ӷ������������."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 pstTempSubSvc->stErrSubCfg.iSubId));

         iRet = ASM_CreateSubReqList( eopstService->iSvcId, iCurInternalId,
                                      1, eopstService,
                                      pstTempSubSvc->pfBalanceList,
                                      aczErrorMessage );
         if( iRet != SVC_SUCC )
         {
            LOG4C ((LOG_FATAL,
                    "�ӷ���������ʧ��,"
                    "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                    eopstService->iSvcId, iCurInternalId,
                    pstTempSubSvc->stErrSubCfg.iSubId));

            pstTempSubSvc->iErrorHandleType = SUB_ERR_TRANLOG;
         }

         LOG4C ((LOG_DEBUG,
                 "��ʼ������."
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                 eopstService->iSvcId, iCurInternalId,
                 pstTempSubSvc->stErrSubCfg.iSubId));

         eopstService->pstSubSvcCurr->iSubId = pstTempSubSvc->stErrSubCfg.iSubId;
         eopstService->pstSubSvcCurr->iTempletId = pstTempSubSvc->stErrSubCfg.iTempletId;
         eopstService->pstSubSvcCurr->iRecordsPerTime = pstTempSubSvc->stErrSubCfg.iRecordsPerTime;
         strcpy( eopstService->pstSubSvcCurr->aczTranCode,
                 pstTempSubSvc->stErrSubCfg.aczSubTranCode );
         strcpy( eopstService->pstSubSvcCurr->aczSubName,
                 pstTempSubSvc->stErrSubCfg.aczSubName );
         /* �����ж��Ƿ���������, �����, ���ݳ����Ľ�������Ƿ�ǳ�����־ */
         if( pstTempSubSvc->iErrorHandleType == SUB_ERR_BALANCE )
         {
            iRet = ADP_RunTemplet( pstTempSubSvc->stErrSubCfg.iSubId,
                                   pstTempSubSvc->iSubSvcSeq,
                                   pstTempSubSvc->stErrSubCfg.iTempletId,
                                   pstTempSubSvc->pfBalanceList,
                                   pstTempSubSvc->pfReqList,
                                   eopstService );
            LOG4C ((LOG_DEBUG,
                   "����%d�ڲ��ӷ���%d�ӷ���%d�������Ӧ������"
                   "�� %d ִ�н�� %d ",
                    eopstService->iSvcId, iCurInternalId,
                    pstTempSubSvc->stErrSubCfg.iSubId,
                    pstTempSubSvc->stErrSubCfg.iTempletId, iRet));
         }
         if( pstTempSubSvc->iErrorHandleType == SUB_ERR_TRANLOG ||
             (pstTempSubSvc->iErrorHandleType == SUB_ERR_BALANCE && iRet!=0 ) )
         {
            /* ���汾�ķ������15 */
            iRet = ADP_WriteBalLog( pstTempSubSvc->stErrSubCfg.iSubId,
                                    pstTempSubSvc->iSubSvcSeq,
                                    pstTempSubSvc->pfBalanceList,
                                    pstTempSubSvc->pfReqList,
                                    eopstService );
            LOG4C((LOG_DEBUG,
                   "����%d�ڲ��ӷ���%d�ӷ���%d�ǳ�����־��������%d"
                   "ִ�н��%d",
                   eopstService->iSvcId, iCurInternalId,
                   pstTempSubSvc->stErrSubCfg.iSubId, iRet));
         }
      }
      else
      {
         LOG4C ((LOG_DEBUG,
                 "����%d�ڲ��ӷ���%d����������������",
                 eopstService->iSvcId, iCurInternalId));
      }
      pstTempSubSvc = pstTempSubSvc->pstNextSubSvc;
   }

   if (pstTempSubSvc != NULL) {
      eopstService->pstSubSvcCurr = pstTempSubSvc;
   }

   /* �ָ������ʼ������Ϣ */
   strcpy( eopstService->aczErrorMessage, aczErrorMessage );
   return iStatus;
}

/*
**����:���ݷ���ź͵�ǰ�ڲ��ӷ����ȷ�������ӷ����Ƿ���Ҫ����
**����:1 �����
**     2 ��ǰ�ڲ��ӷ����
**���:  ��
**����:  0-����Ҫ����,1-��Ҫ����
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


