/********************************************************************
**   ��    ��: ת�����ĵ���װ����
*********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "udo.h"
#include "log.h"
#include "easysock.h"
#include "adp/adp_base.h"
#include "pkg/pkg_api.h"

/*
** function:do the standard templet(translate->send->receive->translate)
**          to ǰ��
** input:1 subservice identify
**       2 subservice request
**       3 service structure
** output:1 subservice answer list
** return:0-success,1-failed
*/

#define RET_CODE  39
#define RET_MSG   2358

#define SEND_HEAD_LEN  2
#define RECV_HEAD_LEN  2

#define DEBUG_MODE 0

int ADP_ToTerm( int eiSubID, int eiSubSeq,
                UDP epfReqList, UDP opfRspList,
                SVC_Content * epstService )
{
   PkgNeedInfo stPkgNeedInfo;
   char aczSendBuf[1024*20], aczRecvBuf[1024*20];
   char aczTmpBuf[1024*20];
   char aczRetCode[10], aczRetMsg[61];
   int iRet, iSendLen, iRecvLen, iHostNodeId;
   MsgFldCfgAry stMsgFldCfgAry;
   SUB_SvcList *pstSubSvcCfg;
   NodeCommCfg stNodeCommCfg;
   ADPService stAdpService;
   char  aczDmzIp[20];
   char  aczDmzPort[10];
   char  aczTerminalId[20];
   char  aczMerchantId[20];

   assert( epfReqList != NULL );
   assert( opfRspList != NULL );
   assert( epstService != NULL );

   memset( aczRecvBuf, 0x00, sizeof(aczRecvBuf) );
   memset( aczSendBuf, 0x00, sizeof(aczSendBuf) );
   memset( aczTmpBuf, 0x00, sizeof(aczTmpBuf) );
   iRecvLen = 0;

   memset( aczDmzIp,0x00,sizeof(aczDmzIp));
   memset( aczDmzPort,0x00,sizeof(aczDmzPort));


   /*�ӷ���ĵ�ǰ�ӷ���ڵ�ȡ�����ڵ��*/
   iHostNodeId = epstService->pstSubSvcCurr->iHostId;
   pstSubSvcCfg = epstService->pstSubSvcCurr;

   iRet = ADP_GetPkgNeedInfo( eiSubID, &stPkgNeedInfo );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      sprintf( epstService->aczErrorMessage, "%d|%d|ȡ���������Ϣ����",
               CMP_NODE_ID, CMP_PKG_ERR );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,                     
                                    pstSubSvcCfg->aczTranCode, -1,             
                                    opfRspList );                              

      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             

      /*                                                                         
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );                                                                             
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );  
      */

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GPNI_ERR;
   }

   switch( iHostNodeId )
   {
       case 202:
           if( memcmp( stPkgNeedInfo.aczTranCode, "202001", 6)==0 )
           {
                sprintf(aczDmzIp,"%s", getenv("TMS_IP"));
                sprintf(aczDmzPort,"%s", getenv("TMS_PORT"));
           }
           else if( memcmp( stPkgNeedInfo.aczTranCode, "202002", 6)==0 )
           {
                sprintf(aczDmzIp,"%s", getenv("BANK_IP"));
                sprintf(aczDmzPort,"%s", getenv("BANK_PORT"));
           }
           break;
       case 206:
           sprintf(aczDmzIp,"%s", getenv("POSP_IP"));
           sprintf(aczDmzPort,"%s", getenv("POSP_PORT"));
           break;
       default:
           break;
   }

   iRet = ADP_FindNodeCommCfg( iHostNodeId, &stNodeCommCfg);
   if (iRet != 0)
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      sprintf( epstService->aczErrorMessage,
               "%d|%d|û�ж��������ڵ�%d�����״���%s",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iNodeId,\
               stPkgNeedInfo.aczTranCode );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,                     
                                    pstSubSvcCfg->aczTranCode, -1,             
                                    opfRspList );                              

      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             

      /*                                                                         
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );                                                                             
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );  
      */

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_NO_HOST_DEF;
   }

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoByTranCode( stPkgNeedInfo.iNodeId,
                                 stPkgNeedInfo.aczTranCode, 1,
                                 &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      sprintf( epstService->aczErrorMessage,
               "%d|%d|ȡ�����ó���,�����ֵ���%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,                     
                                    pstSubSvcCfg->aczTranCode, -1,             
                                    opfRspList );                              
      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*                                                                         
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );                                                                             
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );  
      */

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GIFMFC_ERR;
   }

   iRet = PKG_ToMsg( stPkgNeedInfo.iToMsgId, aczTmpBuf, &iSendLen,
                     stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode,
                     1, &stMsgFldCfgAry, epfReqList );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      sprintf( epstService->aczErrorMessage,
               "%d|%d|�ӷ���%d����������״���%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,                     
                                    pstSubSvcCfg->aczTranCode, -1,             
                                    opfRspList );                              
      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*                                                                         
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );                                                                             
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );  
      */

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GIFMFC_ERR;
   }

   switch( iHostNodeId )
   {
       case 206:
           memcpy( aczSendBuf, "\x60\x00\x01\x00\x00", 5 );
           memcpy( aczSendBuf+5, aczTmpBuf, iSendLen );
           iSendLen += 5;
           memset( aczTerminalId, 0, sizeof(aczTerminalId) );
           memset( aczMerchantId, 0, sizeof(aczMerchantId) );
           UDO_GetField( epfReqList, 41, 0, aczTerminalId, 0 );
           UDO_GetField( epfReqList, 42, 0, aczMerchantId, 0 );
           if( aczSendBuf[5+6+2+8-1] & 0x01 )
           {
               LOG4C(( LOG_DEBUG, "��Ҫ����MAC��TERMID[%s][%s]", aczTerminalId, aczMerchantId ));
               GenMac( aczSendBuf+5+6, iSendLen-5-6-8, aczTerminalId, aczMerchantId );
           }
           break;
       default :
           memcpy( aczSendBuf, aczTmpBuf, iSendLen );
           break;
   }

   /* ȡ���׳�ʱʱ�� */
   iRet = ADP_GetService( eiSubID, &stAdpService );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      sprintf( epstService->aczErrorMessage,
               "%d|%d|ȡ����ͨѶ��������,[%d:%s]",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,                     
                                    pstSubSvcCfg->aczTranCode, -1,             
                                    opfRspList );                              
      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*                                                                         
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );                                                                             
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );                                                              
      */
                                                              
      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));
                                                              
      return ADP_GIFMFC_ERR;                                  
   }

   /* ��ʼ���ͱ��� */

   memset( aczTmpBuf, 0, sizeof(aczTmpBuf) );
   iRet = TransToHost( aczDmzIp, atoi(aczDmzPort), aczSendBuf, iSendLen, 
           aczTmpBuf, &iRecvLen, stAdpService.iTimeOut, HEXBYTE_TWO_NONE_HEAD );
   LOG4C(( LOG_FATAL, "TransToHost iRet:[%d] IRecvlen:[%d]", iRet, iRecvLen));
   if( iRet != 0 )
   {
#if 0
      memcpy( aczRecvBuf, "12345678901234567890", 20 );
      iRecvLen=20;
#else      
      if ( iRet != ADP_RMFS_ERR )
      {
         epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      }

      sprintf( epstService->aczErrorMessage,
               "%d|%d|��ǰ��F[%s]ͨѶ����", CMP_NODE_ID, CMP_COM_ERR,
               ADP_GetNodeName( iHostNodeId ) );

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );

      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );
      */

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_COMM_ERR;
#endif
   }

   switch( iHostNodeId )
   {
       case 206:
           memcpy( aczRecvBuf, aczTmpBuf+5, iRecvLen-5 );
           iRecvLen -= 5;
           break;
       default :
           memcpy( aczRecvBuf, aczTmpBuf, iRecvLen );
           break;
   }

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoByTranCode( stPkgNeedInfo.iNodeId,
                                 stPkgNeedInfo.aczTranCode, 2,
                                 &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|ȡ�����ó���,�����ֵ���%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );
   
      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      
      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );
      */
   
      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));
   
      return ADP_GIFMFC_ERR;
   }
   
   iRet = PKG_ToMid( stPkgNeedInfo.iToMidId, aczRecvBuf, iRecvLen,
                     stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode,
                     2, &stMsgFldCfgAry, opfRspList );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 && iRet != PKG_LENGTH_OVER )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|�ӷ���%d����������״���%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,\
               stPkgNeedInfo.aczTranCode );
   
      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      
      UDO_ChgField( opfRspList, RET_CODE, 0, "96", 2 );                                                                             
      /*
      UDO_ChgField( opfRspList, RET_CODE, 0, "9999999999", 10 );
      UDO_ChgField( opfRspList, RET_MSG, 0, epstService->aczErrorMessage, 50 );
      */
   
      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));
   
      return ADP_PKGTOMID_ERR;
   }

   switch( iHostNodeId )
   {
       case 206:
           memset( aczRetCode, 0, sizeof(aczRetCode) );
           UDO_GetField( opfRspList, RET_CODE, 0, aczRetCode, &iRet );
           if( memcmp(aczRetCode, "00", 2) != 0 )  /*���ܵ�����Ӧ��*/
           {
              epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
              LOG4C(( LOG_DEBUG, "response code[%s]", aczRetCode ));
              return ADP_HOST_DEAL_FAIL;
           }
           break;
       default :
           break;
   }

   UDO_ACField( opfRspList, RET_MSG, UDT_STRING, "���׳ɹ�", 8 );
   return 0;
}
