#include <assert.h>

#include "adp/adp_control.h"
#include "adp_fundef.h"
#include "ap_global.h"
#include "udo.h"
#include "log.h"
#include "easysock.h"

#include "apr_network_io.h"
#include "adp/adp_base.h"
#include "pkg/pkg_api.h"

#include "adp/adp_comm.h"
#include "tool_base.h"
#include "ap_handle.h"

/*��ʱȫ�ֱ���*/
int iOutTime;

void Calc_Time(struct timeval stTimeStart, struct timeval stTimeEnd, 
               long *lDiff);
long Calc_TimeVal( struct timeval stTimeBegin, struct timeval stTimeEnd );

int ADP_ChanStdFixup( int eiSubID,    int eiSubSeq, UDP epfReqList, UDP opfRspList, SVC_Content   * epstService );
/*
**���ܣ����������ӷ���ģ��
**���������1 �ӷ���ID��
**          2 �ӷ�������ģ��ID��
**          3 �ӷ���������Ϣ����
**          4 ����ָ�루������Ϣ���棩
**���������1 �ӷ�����Ӧ��Ϣ����
**����ֵ��0-�ɹ�������-ʧ��
*/

int  ADP_RunTemplet ( int eiSubID, int eiSubSeq, int eiTempletID,  
                      UDP epfReqListMsg, UDP opfRspListMsg,
                      SVC_Content * epstService )
{
   int          iRet;
   int          iMaxTempletID;
   int          iHostNodeId;
   int          iOutTime;

   assert( epfReqListMsg != NULL );
   assert( opfRspListMsg != NULL );
   assert( epstService   != NULL );

   iOutTime = epstService->pstSubSvcTail->iOutTime;

   iMaxTempletID = sizeof( stTempletEntity )/sizeof( ADP_TempletEntity ) - 1;

   /*�ж��ӷ������������Ƿ�Ϸ�*/
   if( ( eiTempletID < 0 ) || ( eiTempletID > iMaxTempletID ) )
   {
      LOG4C(( LOG_FATAL, "TempletID = [%d] ERROR, not exist!", eiTempletID ));
      sprintf( epstService->aczErrorMessage,
               "%d|%d|�ӷ���%d���õķ���������%d����ȷ",
                CMP_NODE_ID, CMP_SYS_ERR, eiSubID, eiTempletID );
      epstService->iErrorType = -1;

      return( INVALID_TEMPLET_ID );
   }

   /*ִ���ӷ��񣬲��ж��ӷ���ִ�н��,���Ǵ�����־*/
   LOG4C(( LOG_DEBUG, "�ӷ���[%d]����%s,��������[%d]", 
           eiSubID, stTempletEntity[ eiTempletID ].pczFuncName,
           eiTempletID ));

   iHostNodeId = epstService->pstSubSvcCurr->iHostId;

   iRet = ( *stTempletEntity[eiTempletID].pfnEntity ) (
            eiSubID, eiSubSeq, epfReqListMsg, opfRspListMsg, epstService );
   if( iHostNodeId != 0 )
   {
      if( ( eiTempletID == epstService->pstSubSvcCurr->stCorrSubCfg.iTempletId )
          || ( eiTempletID == epstService->pstSubSvcCurr->stErrSubCfg.iTempletId
          && epstService->pstSubSvcCurr->iErrorHandleType == SUB_ERR_BALANCE ) )
      {
         UDO_Prt2File( LOG_INFO, "commudo.log", epfReqListMsg, 
                      "To Host Node [%d], SubId [%d]\n", 
                      iHostNodeId, eiSubID );
         UDO_Prt2File( LOG_INFO, "commudo.log", opfRspListMsg, 
                      "From Host Node [%d], SubId [%d]\n", 
                      iHostNodeId, eiSubID );
      }
   }
   else
   {
      UDO_Prt2File( LOG_INFO, "commudo.log", epfReqListMsg, 
                   "To Channel Node [%d], SvcId [%d]\n", 
                   epstService->iChannelId, epstService->iSvcId );
   }

   if( iRet != 0 )
   {
      LOG4C(( LOG_FATAL, "ִ��������[%d]����,iRet=[%d]", 
              eiTempletID, iRet ));
      return iRet;
   }

   return( 0 );
}

void Calc_Time(struct timeval stTimeStart, struct timeval stTimeEnd, 
               long *lDiff)
{
    struct timeval stTimeDiff;

    if( stTimeEnd.tv_usec < stTimeStart.tv_usec )
    {
        stTimeEnd.tv_usec += 1000000;
        stTimeEnd.tv_sec -= 1 ;
    }

    stTimeDiff.tv_sec = stTimeEnd.tv_sec - stTimeStart.tv_sec;
    stTimeDiff.tv_usec = stTimeEnd.tv_usec - stTimeStart.tv_usec;

    *lDiff = stTimeDiff.tv_sec * 1000000 + stTimeDiff.tv_usec ;
}

long Calc_TimeVal( struct timeval stTimeBegin, struct timeval stTimeEnd )
{

    long lTmVal;

    if( stTimeEnd.tv_usec < stTimeBegin.tv_usec ) 
    {
       stTimeEnd.tv_sec -= 1;
       stTimeEnd.tv_usec += 1000000;
    }

    lTmVal = ( stTimeEnd.tv_sec - stTimeBegin.tv_sec ) * 1000000
             + ( stTimeEnd.tv_usec - stTimeBegin.tv_usec );

    return( lTmVal );
}

#define RET_CODE 97
#define RET_MSG  63
/*
** function:send answer to channel
**          we can assume that the host can receive repeat field
**          HeadFlag HEXBYTE_TWO_NONE_HEAD
** input:1 subservice identify
**       2 subservice request
**       3 service structure
** output:1 subservice answer list
** return:0-success,1-failed
*/
int ADP_ChanStdFixup( int eiSubID,    int eiSubSeq,
                      UDP epfReqList, UDP opfRspList,
                      SVC_Content   * epstService )
{
   PkgNeedInfo stPkgNeedInfo;
   char aczSendBuf[1024*20];
   MsgFldCfgAry stMsgFldCfgAry;
   SUB_SvcList *pstSubSvcCfg;
   int  iRet, iSendLen;
   int  iHostNodeId, iLstId;
   char aczRetCode[30], aczRetMsg[200];

   assert( epfReqList != NULL );
   assert( epstService != NULL );

   iHostNodeId = epstService->pstSubSvcCurr->iHostId;
   iLstId = epstService->pstSubSvcCurr->iLstId;
   pstSubSvcCfg = epstService->pstSubSvcCurr;

   iRet = ADP_GetChanPkgNeedInfo( epstService->iSvcId, eiSubID, &stPkgNeedInfo );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|ȡ�������ó���,�����ֵ���%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999001", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "ȡ����������Ϣ����", 60 );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GPNI_ERR;
   }

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoByTranCode( epstService->iChannelId, 
                                 stPkgNeedInfo.aczTranCode, iLstId,
                                 &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|ȡ�����ó���,�����ֵ���%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999002", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "ȡ�����ó���", 60 );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* ����ϵͳ������Ӧ��Ϣ(���������) */
   PUB_StrTrim( epstService->aczErrorMessage, ' ' );
   LOG4C(( LOG_DEBUG, " aczErrorMessage: %s\n", epstService->aczErrorMessage ));
   if( strlen(epstService->aczErrorMessage) != 0 )
   {
      int iLen=0;
      char *pTmp1, *pTmp2;

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      memset( aczRetMsg, 0x00, sizeof(aczRetMsg) );

      pTmp1 = epstService->aczErrorMessage;  /* ���ڵ�|��Ӧ��|��Ӧ��Ϣ */
      pTmp2 = strchr( pTmp1, '|' );
      if( pTmp2 == NULL )
      {
         strcpy( aczRetCode, "999999004" );
         strcpy( aczRetMsg, pTmp1 );
         goto ERROUT;
      }

      pTmp1 = pTmp2 + 1;
      pTmp2 = strchr( pTmp1, '|' );
      if( pTmp2 == NULL )
      {
         strcpy( aczRetCode, "999999005" );
         strcpy( aczRetMsg, pTmp1 );
         goto ERROUT;
      }

      iLen = (int)(pTmp2 - pTmp1)>(sizeof(aczRetCode)-1) ? (sizeof(aczRetCode)-1) : (int)(pTmp2 - pTmp1);
      memcpy( aczRetCode, pTmp1, iLen );
      pTmp1 = pTmp2 + 1;
      iLen = strlen(pTmp1)>(sizeof(aczRetMsg)-1) ? (sizeof(aczRetMsg)-1) : strlen(pTmp1);
      memcpy( aczRetMsg, pTmp1, iLen );

ERROUT:
      if( iHostNodeId != 3 ) 
      {
         UDO_ACField( epfReqList, RET_CODE, UDT_STRING, aczRetCode, 9 );
         UDO_ACField( epfReqList, RET_MSG, UDT_STRING, aczRetMsg, 60 );
      }

      LOG4C(( LOG_FATAL, "[%s:%s]", aczRetCode, aczRetMsg ));
   }

   memset( aczSendBuf, 0x00, sizeof(aczSendBuf) );
   iRet = PKG_ToMsg( stPkgNeedInfo.iToMidId, aczSendBuf, &iSendLen,
                     stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode,
                     iLstId, &stMsgFldCfgAry, epfReqList );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|�ӷ���%d����������״���%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999003", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "�ӷ���������", 60 );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_PKGTOMSG_ERR;
   }

   iRet = sendBuffer( aczSendBuf, (apr_size_t)iSendLen );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|�ӷ���%d������Ӧ����ʧ�ܣ����״���%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );
      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));
      return ADP_SMTS_ERR;
   }

   LOG4C(( LOG_DEBUG, "������Ӧ���ĳɹ�!" ));

   return 0;
} 
/*
** end of file
*/
