/********************************************************************
**   北方电信服务主机－适配器
**
**   文    件: adp_totelcom.c
**   功    能: 完成与北方电信主机的通讯
*********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "udo.h"
#include "log.h"
#include "easysock.h"
#include "adp/adp_base.h"
#include "pkg/pkg_api.h"

#define RET_CODE  8
#define RET_CODE_LEN  10
#define RET_MSG   9999
#define RET_SERIAL   37
#define BUFFER_LEN 1024*5
#define HEAD_LEN  2
#define RET_MSG_LEN  64

#define uchar unsigned char
#define uint  unsigned int

static int Telcom_HostNodeId=206;

/**
  * 往前置发送交易数据,并从前置取回数据
  */
int ADP_TransTelcom( char *epczAddr, int eiPort, char *epczSendBuf,
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


   memset( aczSendBuf, 0, sizeof(aczSendBuf) );
   memset( aczRecvBuf, 0, sizeof(aczRecvBuf) );
   memset( sLen, 0x00, sizeof(sLen) );

   /*处理包头长度
   *
   sLen[0] = (eiSendLen-2)/256;
   sLen[1] = (eiSendLen-2)%256;
   
   memcpy( aczSendBuf, sLen, HEAD_LEN );
   memcpy( aczSendBuf+HEAD_LEN, epczSendBuf+2, eiSendLen-2 );
   iLen = eiSendLen-2+HEAD_LEN;
   */
   memcpy( aczSendBuf, epczSendBuf, eiSendLen );
   iLen = eiSendLen;
   /*补齐结尾0A*/
   aczSendBuf[iLen] = 0x0A;
   iLen ++;
   sprintf(sLen, "%04d", iLen);
   memcpy(aczSendBuf+4, sLen, 4);
   
   LOG4C(( LOG_DEBUG, "To 前置: [%s:%d]", epczAddr, eiPort ));
   TOOL_Dump( LOG_DEBUG, "toTelcomsend.dump", (unsigned char*)aczSendBuf, iLen );
   LOG4C(( LOG_DEBUG, "发送报文: [%d:%s]", iLen, aczSendBuf ));
   
#if 1
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
   /* 取报文头(长度) 北方电信数据包长度域为5-8字节*/
   //iRet = tcp_ctl_recv( iSock, sLen, HEAD_LEN, iTimeOut*1000, NO_HEAD );
   iRet = tcp_ctl_recv( iSock, sLen, 8, iTimeOut*1000, NO_HEAD );
   if( iRet == -1  )
   {
      LOG4C(( LOG_FATAL, "从前置接收报文长度失败, 返回%d, 原因%s",
              iRet, strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }

   LOG4C(( LOG_DEBUG, "接收包头内容: [%s]", sLen ));
   iRecv = 0;
   //iLen = sLen[0]*256 + sLen[1];
   sLen[8] = '\0';
   //去除4位交易代码，去除4位长度，去除2位回车换行
   iLen = atoi(sLen+4) - 8;
   if( iLen <= 0 )
   {
      LOG4C(( LOG_FATAL, "从前置接收长度失败 [%s]", strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }
   LOG4C(( LOG_DEBUG, "继续接收长度: [%d]", iLen ));
   memcpy(aczRecvBuf, sLen, 8);
   iRecv = 8;

   /* 循环接收20次 */
   for( i=0; i<20 && iRecv<iLen; i++ )
   {
      iRet =0;
      //iRet = tcp_ctl_recv( iSock, aczRecvBuf+iRecv, iLen-iRecv, iTimeOut*1000, NO_HEAD );
	  iRet = tcp_ctl_recv( iSock, aczRecvBuf+iRecv, iLen, iTimeOut*1000, NO_HEAD );
      if( iRet == -1 )
      {
         LOG4C(( LOG_FATAL, "从前置接收失败 [%s]", strerror(errno) ));
         tcp_close( iSock );
         return ADP_RMFS_ERR;
      }
      iRecv += iRet;
   }
   LOG4C(( LOG_DEBUG, "北方电信服务接收包完整内容: [%s]", aczRecvBuf));
   
   tcp_close( iSock );

#else

iRecv=105;/*签到成功*/
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x14\x00\x00\x03\x14\x33\x40\x07\x22\x08\x00\x09\x39\x00\x31\x34\x33\x33\x34\x30\x32\x31\x34\x33\x37\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x00\x11\x00\x00\x00\x31\x00\x10\x00\x24\x99\xEC\x3F\x67\x80\x85\xE3\x62\xE4\xB2\x26\xD2\x03\x4C\x4F\x6B\xCE\x9E\x2F\xED\x94\x50\x77\x48", 105 );

iRecv=79;/*签到失败*/
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x10\x00\x00\x12\x14\x43\x03\x07\x22\x08\x00\x09\x39\x00\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x37\x31\x32\x33\x34\x35\x36\x37\x38\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x00\x11\x00\x00\x00\x32\x00\x10", 79 ); 

/*消费成功
iRecv=170;
memcpy( aczRecvBuf,"\x60\x00\x07\x00\x00\x60\x22\x00\x00\x00\x00\x02\x00\x30\x20\x04\xC0\x30\xC0\x98\x11\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x07\x02\x10\x00\x12\x37\x90\x03\x02\x24\x00\x85\x92\x39\xD0\x00\x05\x01\x00\x00\x00\x06\x44\x85\x50\x01\x04\x99\x90\x03\x02\x24\x00\x85\x92\x39\xD1\x56\x15\x60\x50\x00\x00\x00\x00\x00\x01\x30\x00\x00\x02\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xD0\x00\x00\x00\x00\x00\x0D\x06\x44\x85\x50\x00\x00\x00\x00\x00\x33\x30\x38\x30\x38\x30\x31\x35\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x31\x35\x36\x5E\x0A\x58\x36\xE1\xA7\xB7\x4B\x20\x00\x00\x00\x00\x00\x00\x00\x00\x13\x22\x00\x00\x31\x00\x05\x00\x31\x42\x43\x32\x37\x35\x33\x35", iRecv );
*/
#endif

   TOOL_Dump( LOG_DEBUG, "toTelcomrecv.dump", (unsigned char*)aczRecvBuf, iRecv );
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
**
** eiSubID,子服务编号
** eiSubSeq,子服务内部序号
** UDP epfReqList,要发送给外部系统的请求UDO
** UDP opfRspList,从外部系统接收的UDO
** SVC_Content *epstService,服务信息
*/
int ADP_ToTelcom( int eiSubID, int eiSubSeq, UDP epfReqList, UDP opfRspList, SVC_Content * epstService )
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
   int iRetCode;

   assert( epfReqList != NULL );
   assert( opfRspList != NULL );
   assert( epstService != NULL );

   memset( aczSendBuf, 0x00, sizeof(aczSendBuf) );
   memset( aczRecvBuf, 0x00, sizeof(aczRecvBuf) );
   memset( aczRetCode,0x00,sizeof(aczRetCode));
   
   memset( aczDmzIp,0x00,sizeof(aczDmzIp));
   memset( aczDmzPort,0x00,sizeof(aczDmzPort));
   
   sprintf(aczDmzIp,"%s", getenv("TELCOM_IP"));
   sprintf(aczDmzPort,"%s", getenv("TELCOM_PORT"));

   /*从服务的当前子服务节点取主机节点号*/
   iHostNodeId = epstService->pstSubSvcCurr->iHostId;
   pstSubSvcCfg = epstService->pstSubSvcCurr;

   /*取交易码*/
   memset( aczTranCode, 0x00, sizeof(aczTranCode) );
   sprintf( aczTranCode, "%s", epstService->pstSubSvcCurr->aczTranCode );

   LOG4C(( LOG_DEBUG, "交易码:[%s], 节点号:[%d]", aczTranCode, iHostNodeId ));

   /* 获取打包所需各个节点配置信息 */
   iRet = ADP_GetPkgNeedInfo( eiSubID, &stPkgNeedInfo );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "9000000001" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      strcpy( aczRetMsg, "取打包配置信息出错" );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GPNI_ERR;
   }

   /* 根据主机节点号查询该节点的详细信息，如主机IP、端口、连接方式等*/
   iRet = ADP_FindNodeCommCfg( iHostNodeId, &stNodeCommCfg );
   if (iRet != 0)
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );
      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "9000000002" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "没有定义主机节点%d，交易代码%s", stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode);
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_NO_HOST_DEF;
   }

   /* 根据交易代码获取数据字典中的配置信息 */
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
      strcpy( aczRetCode, "9000000003" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "取域配置出错,数据字典编号%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* 把CMP内部的UDO数据类型转换为前置要的数据类型 */
   /* 将中间信息链表转换为输出报文 */
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
      strcpy( aczRetCode, "9000000004" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "子服务%d打包出错，交易代码%s", eiSubID, stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

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
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      memset( aczRetCode, 0x00, sizeof(aczRetCode) );
      strcpy( aczRetCode, "9000000005" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }


   /* 打包后进行数据报文交换 */
   /*为了处理电信错误返回报文比正常报文短*/
   memset(aczRecvBuf, '|', sizeof(aczRecvBuf));
   iRet = ADP_TransTelcom(  aczDmzIp, atoi(aczDmzPort), aczSendBuf,
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
      strcpy( aczRetCode, "9000000006" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "与前置[%s]通讯出错", ADP_GetNodeName( iHostNodeId ) );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s", Telcom_HostNodeId, CMP_COM_ERR, aczRetMsg);

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));
      return ADP_COMM_ERR;
   }

   /*为了处理64位的返回消息信息，客户报文中没有该消息，处理过程中需要*/
   /*为了处理电信错误返回报文比正常报文短*/
   lRecvLen += 1024;
  
   /* 报文处理,获取数据字典配置信息 */
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
      strcpy( aczRetCode, "9000000007" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "取域配置出错,数据字典编号%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* 将源报文转换为中间信息链表 */
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
      strcpy( aczRetCode, "9000000008" );
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczRetCode, RET_CODE_LEN );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "拆包出错，交易代码[%s]", stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_DEBUG, "%s [%s] %d", epstService->aczErrorMessage,
              aczRecvBuf , lRecvLen ));

      return ADP_PKGTOMID_ERR;
   }


   UDO_GetField( opfRspList, 13, 0, aczRetCode, &iRet );

   /* 获取UDO链表中的返回代码域，如不成功，从数据库中补充具体错误提示*/
   memset( aczRetCode, 0, sizeof(aczRetCode) );
   UDO_GetField( opfRspList, RET_CODE, 0, aczRetCode, &iRet );
   LOG4C(( LOG_DEBUG, "UDO_GetField response code[%s]", aczRetCode ));

   if( memcmp(aczRetCode, "0", 1) != 0 )  /*接受到具体应答*/
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      memset( aczChannelCode, 0, sizeof(aczChannelCode) );
      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      if( ADP_GetChanRet( iHostNodeId, epstService->iChannelId, aczRetCode, NULL,  aczChannelCode , aczRetMsg )!=0)
      {
          strcpy( aczChannelCode, "9000000009" );
          strcpy( aczRetMsg, "查找错误码出错" );
      }
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczChannelCode, RET_CODE_LEN );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      LOG4C(( LOG_DEBUG, "To_telcom错误转换:response code[%s] aczRetMsg[%s]", aczRetCode, aczRetMsg ));
      return ADP_HOST_DEAL_FAIL;
   }

   /* 增加或修改UDO链表中的返回消息 */ 
   LOG4C(( LOG_DEBUG, "ADP_ToTelcom 交易成功:[%s]", aczRetCode ));
   UDO_ACField( opfRspList, RET_MSG, UDT_STRING, "交易成功", 8 );
   return 0;
}



