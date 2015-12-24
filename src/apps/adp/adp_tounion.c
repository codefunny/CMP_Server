/********************************************************************
**   发往银行服务主机－适配器
**
**   文    件: adp_tounion.c
**   功    能: 完成与银行主机的通讯
*********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "udo.h"
#include "log.h"
#include "easysock.h"
#include "adp/adp_base.h"
#include "pkg/pkg_api.h"

#define RET_CODE  39
#define RET_CODE_LEN  2
#define RET_MSG   1002
#define RET_SERIAL   37
#define BUFFER_LEN 1024*5
#define HEAD_LEN  2
#define RET_MSG_LEN  64

#define uchar unsigned char
#define uint  unsigned int

static int Union_HostNodeId=204;

/**
  * 往前置发送交易数据,并从前置取回数据
  */
int ADP_TransUnion( char *epczAddr, int eiPort, char *epczSendBuf,
                  int eiSendLen, char *opczRecvBuf, long *opiRecvLen,
                  int iTimeOut )
{
   int iSock, iRet, i;
   int iLen, iRecv;
   unsigned char sLen[20];
   char aczSendBuf[2048];
   char aczRecvBuf[2048];
   char aczMac[3];
   char aczRespMsg[120];
   char aczTpduNum[32];
   char aczTpdu[16];

   memset( aczSendBuf, 0, sizeof(aczSendBuf) );
   memset( aczRecvBuf, 0, sizeof(aczRecvBuf) );
   memset( sLen, 0x00, sizeof(sLen) );
   sLen[0] = (eiSendLen)/256;
   sLen[1] = (eiSendLen)%256;
   
   memcpy( aczSendBuf, sLen, 2 );
   memcpy( aczSendBuf+2, epczSendBuf, eiSendLen );
   iLen = eiSendLen+2;

   LOG4C(( LOG_DEBUG, "To 前置: [%s:%d]", epczAddr, eiPort ));
   TOOL_Dump( LOG_DEBUG, "tounionsend.dump", (unsigned char*)aczSendBuf, iLen );
   LOG4C(( LOG_DEBUG, "发送报文: [%d:%s]", iLen, aczSendBuf ));
#if  1
   iSock = tcp_connect( epczAddr, eiPort );
   if( iSock == -1 )
   {
      LOG4C(( LOG_FATAL, "与前置[%s:%d]连接失败,error=%s", 
              epczAddr, eiPort, strerror(errno) ));
      return ADP_SMTS_ERR;
   }

   /**
   * 发送报文体,暂时定为iTimeOut秒超时
   */
   iRet = tcp_ctl_send( iSock, aczSendBuf, iLen, iTimeOut*1000, NO_HEAD );
   if( iRet == -1 )
   {
      LOG4C(( LOG_FATAL, "向前置[%s:%d]发送失败,error=%s", 
              epczAddr, eiPort, strerror(errno) ));
      tcp_close( iSock );
      return ADP_SMTS_ERR;
   }

   memset( sLen, 0x00, sizeof(sLen) );
   /* 取报文头(长度) */
   iRet = tcp_ctl_recv( iSock, sLen, 2, iTimeOut*1000, NO_HEAD );
   if( iRet == -1  )
   {
      LOG4C(( LOG_FATAL, "从前置接收报文长度失败, 返回%d, 原因%s",
              iRet, strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }

   LOG4C(( LOG_DEBUG, "接收包头内容: [%s]", sLen ));
   iRecv = 0;
   iLen = sLen[0]*256 + sLen[1];
   if( iLen <= 0 )
   {
      LOG4C(( LOG_FATAL, "从前置接收长度失败 [%s]", strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }

   /* 循环接收20次 */
   for( i=0; i<20 && iRecv<iLen; i++ )
   {
      iRet =0;
      iRet = tcp_ctl_recv( iSock, aczRecvBuf+iRecv, iLen-iRecv, iTimeOut*1000, NO_HEAD );
      if( iRet == -1 )
      {
         LOG4C(( LOG_FATAL, "从前置接收失败 [%s]", strerror(errno) ));
         tcp_close( iSock );
         return ADP_RMFS_ERR;
      }
      iRecv += iRet;
   }

   tcp_close( iSock );

#else
/*
iRecv=105;//签到成功
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x14\x00\x00\x03\x14\x33\x40\x07\x22\x08\x00\x09\x39\x00\x31\x34\x33\x33\x34\x30\x32\x31\x34\x33\x37\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x00\x11\x00\x00\x00\x31\x00\x10\x00\x24\x99\xEC\x3F\x67\x80\x85\xE3\x62\xE4\xB2\x26\xD2\x03\x4C\x4F\x6B\xCE\x9E\x2F\xED\x94\x50\x77\x48", 105 );
*/

/*
iRecv=79; //签到失败
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x10\x00\x00\x12\x14\x43\x03\x07\x22\x08\x00\x09\x39\x00\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x37\x31\x32\x33\x34\x35\x36\x37\x38\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x00\x11\x00\x00\x00\x32\x00\x10", 79 ); 
*/
	
/*消费成功
iRecv=170;
memcpy( aczRecvBuf,"\x60\x00\x07\x00\x00\x60\x22\x00\x00\x00\x00\x02\x00\x30\x20\x04\xC0\x30\xC0\x98\x11\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x07\x02\x10\x00\x12\x37\x90\x03\x02\x24\x00\x85\x92\x39\xD0\x00\x05\x01\x00\x00\x00\x06\x44\x85\x50\x01\x04\x99\x90\x03\x02\x24\x00\x85\x92\x39\xD1\x56\x15\x60\x50\x00\x00\x00\x00\x00\x01\x30\x00\x00\x02\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xD0\x00\x00\x00\x00\x00\x0D\x06\x44\x85\x50\x00\x00\x00\x00\x00\x33\x30\x38\x30\x38\x30\x31\x35\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x31\x35\x36\x5E\x0A\x58\x36\xE1\xA7\xB7\x4B\x20\x00\x00\x00\x00\x00\x00\x00\x00\x13\x22\x00\x00\x31\x00\x05\x00\x31\x42\x43\x32\x37\x35\x33\x35", iRecv );
*/

//消费失败
iRecv = 172;
memcpy(aczRecvBuf, "\x37\x00\x00\x00\x03\x60\x21\x10\x00\x00\x00\x02\x10\x70\x3e\x00\x81\x0e\xd0\x88\x13\x16\x95\x55\x50\x59\x11\x13\x96\x97\x00\x00\x00\x00\x00\x00\x00\x50\x00\x00\x00\x95\x15\x31\x39\x08\x21\x09\x09\x12\x31\x00\x11\x00\x11\x00\x01\x10\xff\x31\x31\x30\x38\x32\x31\x31\x35\x33\x31\x33\x39\x39\x37\x35\x33\x31\x31\x32\x35\x31\x32\x33\x34\x35\x36\x37\x36\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x31\x31\x31\x31\x31\x32\x32\x32\x32\x32\x33\x33\x33\x33\x33\x34\x34\x34\x34\x34\x35\x35\x31\x35\x36\x20\x00\x00\x00\x00\x00\x00\x00\x00\x08\x22\x00\x00\x01\x00\x20\x42\x42\x32\x32\x33\x33\x34\x34\x35\x35\x36\x36\x37\x37\x38\x38\x39\x39\x30\x30\x45\x43\x32\x43\x43\x46\x34\x43", iRecv);


#endif


   TOOL_Dump( LOG_DEBUG, "tounionrecv.dump", (unsigned char*)aczRecvBuf, iRecv );
   LOG4C(( LOG_DEBUG, "接收报文:[Len=%d]", iRecv ));

   /* 处理响应信息 */
   *opiRecvLen = iRecv;
   
   memcpy( opczRecvBuf, aczRecvBuf, *opiRecvLen );

   return 0;
}

/*
** function:do the standard templet(translate->send->receive->translate)
**          to 前置
** input:1 subservice identify
**       2 subservice request
**       3 service structure
** output:1 subservice answer list
** return:0-success,1-failed
*/
int ADP_ToUnion( int eiSubID, int eiSubSeq, UDP epfReqList, UDP opfRspList, SVC_Content * epstService )
{
   char aczSendBuf[BUFFER_LEN], aczRecvBuf[BUFFER_LEN];
   char aczRetCode[16] ;
   char aczTranCode[10];
   char aczRetMsg[128];
   char aczChannelCode[20];
   char aczMac[3],aczDmzIp[30],aczDmzPort[8];
   int i, iRet, iSendLen, iHostNodeId ;
   int iTotalLen;
   long lRecvLen;
   MsgFldCfgAry stMsgFldCfgAry;
   SUB_SvcList *pstSubSvcCfg;
   NodeCommCfg stNodeCommCfg;
   ADPService stAdpService;
   PkgNeedInfo stPkgNeedInfo;

   assert( epfReqList != NULL );
   assert( opfRspList != NULL );
   assert( epstService != NULL );

   memset( aczSendBuf, 0x00, sizeof(aczSendBuf) );
   memset( aczRecvBuf, 0x00, sizeof(aczRecvBuf) );
   memset( aczRetCode,0x00,sizeof(aczRetCode));
   
   memset( aczDmzIp,0x00,sizeof(aczDmzIp));
   memset( aczDmzPort,0x00,sizeof(aczDmzPort));
   
   sprintf(aczDmzIp,"%s", getenv("UNIONPAY_IP"));
   sprintf(aczDmzPort,"%s", getenv("UNIONPAY_PORT"));

   /*从服务的当前子服务节点取主机节点号*/
   iHostNodeId = epstService->pstSubSvcCurr->iHostId;
   pstSubSvcCfg = epstService->pstSubSvcCurr;

   /*取交易码*/
   memset( aczTranCode, 0x00, sizeof(aczTranCode) );
   sprintf( aczTranCode, "%s", epstService->pstSubSvcCurr->aczTranCode );

   LOG4C(( LOG_DEBUG, "交易码:[%s], 节点号:[%d]", aczTranCode, iHostNodeId ));

   iRet = ADP_GetPkgNeedInfo( eiSubID, &stPkgNeedInfo );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "01" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      strcpy( aczRetMsg, "取打包配置信息出错" );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GPNI_ERR;
   }

   iRet = ADP_FindNodeCommCfg( iHostNodeId, &stNodeCommCfg );
   if (iRet != 0)
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "02" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "没有定义主机节点%d，交易代码%s", stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode);
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_NO_HOST_DEF;
   }

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoByTranCode( stPkgNeedInfo.iNodeId,
                                 stPkgNeedInfo.aczTranCode, 1,
                                 &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "03" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "取域配置出错,数据字典编号%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* 把CMP内部的UDO数据类型转换为前置要的数据类型 */
   iRet = PKG_ToMsg( stPkgNeedInfo.iToMsgId, aczSendBuf, &iSendLen,
                     stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode,
                     1, &stMsgFldCfgAry, epfReqList );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "04" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "子服务%d打包出错，交易代码%s", eiSubID, stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* 取交易超时时间 */
   iRet = ADP_GetService( eiSubID, &stAdpService );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "取主机通讯参数错误" );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "05" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   
   iRet = ADP_TransUnion(  aczDmzIp, atoi(aczDmzPort), aczSendBuf,
                        iSendLen, aczRecvBuf, &lRecvLen,
                        stAdpService.iTimeOut ); 
   if( iRet != 0 )
   {
      if ( iRet != ADP_RMFS_ERR )
      {
         epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      }

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "06" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "与前置[%s]通讯出错", ADP_GetNodeName( iHostNodeId ) );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s", Union_HostNodeId, CMP_COM_ERR, aczRetMsg);

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));
      return ADP_COMM_ERR;
   }
   
  
   /* 报文处理 */
   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoByTranCode( stPkgNeedInfo.iNodeId,
                                 stPkgNeedInfo.aczTranCode, 2,
                                 &stMsgFldCfgAry );
   if( iRet != 0 )
   {

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "07" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "取域配置出错,数据字典编号%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   iRet = PKG_ToMid( stPkgNeedInfo.iToMidId, aczRecvBuf, lRecvLen,
                     stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode,
                     2, &stMsgFldCfgAry, opfRspList );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 && iRet != PKG_LENGTH_OVER )
   {
      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "08" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "拆包出错，交易代码[%s]", stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Union_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_DEBUG, "%s [%s] %d", epstService->aczErrorMessage,
              aczRecvBuf , lRecvLen ));

      return ADP_PKGTOMID_ERR;
   }

   memset( aczRetCode, 0, sizeof(aczRetCode) );
   UDO_GetField( opfRspList, RET_CODE, 0, aczRetCode, &iRet );
   if( memcmp(aczRetCode, "00", 2) != 0 )  /*接受到具体应答*/
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      memset( aczChannelCode, 0, sizeof(aczChannelCode) );
      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      if( ADP_GetChanRet( iHostNodeId, epstService->iChannelId, aczRetCode, NULL,  aczChannelCode , aczRetMsg )!=0)
      {
          strcpy( aczChannelCode, "99" );
          strcpy( aczRetMsg, "查找错误码出错" );
      }
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczChannelCode, RET_CODE_LEN );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      LOG4C(( LOG_DEBUG, "response code[%s]", aczRetCode ));
      return ADP_HOST_DEAL_FAIL;
   }
     
   UDO_ACField( opfRspList, RET_MSG, UDT_STRING, "交易成功", 8 );
   return 0;
}
