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

/*超时全局变量*/
int iOutTime;

void Calc_Time(struct timeval stTimeStart, struct timeval stTimeEnd, 
               long *lDiff);
long Calc_TimeVal( struct timeval stTimeBegin, struct timeval stTimeEnd );

int ADP_ChanStdFixup( int eiSubID,    int eiSubSeq, UDP epfReqList, UDP opfRspList, SVC_Content   * epstService );
/*
**功能：控制运行子服务模板
**输入参数：1 子服务ID号
**          2 子服务运行模板ID号
**          3 子服务请求信息链表
**          4 服务指针（错误信息保存）
**输出参数：1 子服务相应信息链表
**返回值：0-成功，其他-失败
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

   /*判断子服务适配器号是否合法*/
   if( ( eiTempletID < 0 ) || ( eiTempletID > iMaxTempletID ) )
   {
      LOG4C(( LOG_FATAL, "TempletID = [%d] ERROR, not exist!", eiTempletID ));
      sprintf( epstService->aczErrorMessage,
               "%d|%d|子服务%d配置的服务适配器%d不正确",
                CMP_NODE_ID, CMP_SYS_ERR, eiSubID, eiTempletID );
      epstService->iErrorType = -1;

      return( INVALID_TEMPLET_ID );
   }

   /*执行子服务，并判断子服务执行结果,并记错误日志*/
   LOG4C(( LOG_DEBUG, "子服务[%d]调用%s,适配器号[%d]", 
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
      LOG4C(( LOG_FATAL, "执行适配器[%d]出错,iRet=[%d]", 
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
               "%d|%d|取报文配置出错,数据字典编号%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999001", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "取报文配置信息出错", 60 );

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
               "%d|%d|取域配置出错,数据字典编号%d",
               CMP_NODE_ID, CMP_PKG_ERR, stPkgNeedInfo.iPkgAppId );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999002", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "取域配置出错", 60 );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* 处理系统错误响应信息(如重组出错) */
   PUB_StrTrim( epstService->aczErrorMessage, ' ' );
   LOG4C(( LOG_DEBUG, " aczErrorMessage: %s\n", epstService->aczErrorMessage ));
   if( strlen(epstService->aczErrorMessage) != 0 )
   {
      int iLen=0;
      char *pTmp1, *pTmp2;

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      memset( aczRetMsg, 0x00, sizeof(aczRetMsg) );

      pTmp1 = epstService->aczErrorMessage;  /* 主节点|响应码|响应信息 */
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
               "%d|%d|子服务%d打包出错，交易代码%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );

      UDO_ACField( epfReqList, RET_CODE, UDT_STRING, "999999003", 9 );
      UDO_ACField( epfReqList, RET_MSG, UDT_STRING, "子服务打包出错", 60 );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_PKGTOMSG_ERR;
   }

   iRet = sendBuffer( aczSendBuf, (apr_size_t)iSendLen );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%d|子服务%d发送响应报文失败，交易代码%s",
               CMP_NODE_ID, CMP_PKG_ERR, eiSubID,
               stPkgNeedInfo.aczTranCode );
      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));
      return ADP_SMTS_ERR;
   }

   LOG4C(( LOG_DEBUG, "发送响应报文成功!" ));

   return 0;
} 
/*
** end of file
*/
