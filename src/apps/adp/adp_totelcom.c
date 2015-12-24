/********************************************************************
**   �������ŷ���������������
**
**   ��    ��: adp_totelcom.c
**   ��    ��: ����뱱������������ͨѶ
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
  * ��ǰ�÷��ͽ�������,����ǰ��ȡ������
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

   /*�����ͷ����
   *
   sLen[0] = (eiSendLen-2)/256;
   sLen[1] = (eiSendLen-2)%256;
   
   memcpy( aczSendBuf, sLen, HEAD_LEN );
   memcpy( aczSendBuf+HEAD_LEN, epczSendBuf+2, eiSendLen-2 );
   iLen = eiSendLen-2+HEAD_LEN;
   */
   memcpy( aczSendBuf, epczSendBuf, eiSendLen );
   iLen = eiSendLen;
   /*�����β0A*/
   aczSendBuf[iLen] = 0x0A;
   iLen ++;
   sprintf(sLen, "%04d", iLen);
   memcpy(aczSendBuf+4, sLen, 4);
   
   LOG4C(( LOG_DEBUG, "To ǰ��: [%s:%d]", epczAddr, eiPort ));
   TOOL_Dump( LOG_DEBUG, "toTelcomsend.dump", (unsigned char*)aczSendBuf, iLen );
   LOG4C(( LOG_DEBUG, "���ͱ���: [%d:%s]", iLen, aczSendBuf ));
   
#if 1
   iSock = tcp_connect( epczAddr, eiPort );
   if( iSock == -1 )
   {
      LOG4C(( LOG_FATAL, "��ǰ��[%s:%d]����ʧ��,error=%s", 
              epczAddr, eiPort, strerror(errno) ));
      return ADP_SMTS_ERR;
   }

   /**
   * ���ͱ�����,��ʱ��ΪiTimeOut�볬ʱ
   */
   iRet = tcp_ctl_send( iSock, aczSendBuf, iLen, iTimeOut*1000, NO_HEAD );
   if( iRet == -1 )
   {
      LOG4C(( LOG_FATAL, "��ǰ��[%s:%d]����ʧ��,error=%s", 
              epczAddr, eiPort, strerror(errno) ));
      tcp_close( iSock );
      return ADP_SMTS_ERR;
   }

   memset( sLen, 0x00, sizeof(sLen) );
   /* ȡ����ͷ(����) �����������ݰ�������Ϊ5-8�ֽ�*/
   //iRet = tcp_ctl_recv( iSock, sLen, HEAD_LEN, iTimeOut*1000, NO_HEAD );
   iRet = tcp_ctl_recv( iSock, sLen, 8, iTimeOut*1000, NO_HEAD );
   if( iRet == -1  )
   {
      LOG4C(( LOG_FATAL, "��ǰ�ý��ձ��ĳ���ʧ��, ����%d, ԭ��%s",
              iRet, strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }

   LOG4C(( LOG_DEBUG, "���հ�ͷ����: [%s]", sLen ));
   iRecv = 0;
   //iLen = sLen[0]*256 + sLen[1];
   sLen[8] = '\0';
   //ȥ��4λ���״��룬ȥ��4λ���ȣ�ȥ��2λ�س�����
   iLen = atoi(sLen+4) - 8;
   if( iLen <= 0 )
   {
      LOG4C(( LOG_FATAL, "��ǰ�ý��ճ���ʧ�� [%s]", strerror(errno) ));
      tcp_close( iSock );
      return ADP_RMFS_ERR;
   }
   LOG4C(( LOG_DEBUG, "�������ճ���: [%d]", iLen ));
   memcpy(aczRecvBuf, sLen, 8);
   iRecv = 8;

   /* ѭ������20�� */
   for( i=0; i<20 && iRecv<iLen; i++ )
   {
      iRet =0;
      //iRet = tcp_ctl_recv( iSock, aczRecvBuf+iRecv, iLen-iRecv, iTimeOut*1000, NO_HEAD );
	  iRet = tcp_ctl_recv( iSock, aczRecvBuf+iRecv, iLen, iTimeOut*1000, NO_HEAD );
      if( iRet == -1 )
      {
         LOG4C(( LOG_FATAL, "��ǰ�ý���ʧ�� [%s]", strerror(errno) ));
         tcp_close( iSock );
         return ADP_RMFS_ERR;
      }
      iRecv += iRet;
   }
   LOG4C(( LOG_DEBUG, "�������ŷ�����հ���������: [%s]", aczRecvBuf));
   
   tcp_close( iSock );

#else

iRecv=105;/*ǩ���ɹ�*/
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x14\x00\x00\x03\x14\x33\x40\x07\x22\x08\x00\x09\x39\x00\x31\x34\x33\x33\x34\x30\x32\x31\x34\x33\x37\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x00\x11\x00\x00\x00\x31\x00\x10\x00\x24\x99\xEC\x3F\x67\x80\x85\xE3\x62\xE4\xB2\x26\xD2\x03\x4C\x4F\x6B\xCE\x9E\x2F\xED\x94\x50\x77\x48", 105 );

iRecv=79;/*ǩ��ʧ��*/
memcpy(aczRecvBuf, "\x60\x00\x00\x00\x07\x60\x22\x00\x00\x00\x00\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x10\x00\x00\x12\x14\x43\x03\x07\x22\x08\x00\x09\x39\x00\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x37\x31\x32\x33\x34\x35\x36\x37\x38\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x00\x11\x00\x00\x00\x32\x00\x10", 79 ); 

/*���ѳɹ�
iRecv=170;
memcpy( aczRecvBuf,"\x60\x00\x07\x00\x00\x60\x22\x00\x00\x00\x00\x02\x00\x30\x20\x04\xC0\x30\xC0\x98\x11\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x07\x02\x10\x00\x12\x37\x90\x03\x02\x24\x00\x85\x92\x39\xD0\x00\x05\x01\x00\x00\x00\x06\x44\x85\x50\x01\x04\x99\x90\x03\x02\x24\x00\x85\x92\x39\xD1\x56\x15\x60\x50\x00\x00\x00\x00\x00\x01\x30\x00\x00\x02\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xD0\x00\x00\x00\x00\x00\x0D\x06\x44\x85\x50\x00\x00\x00\x00\x00\x33\x30\x38\x30\x38\x30\x31\x35\x33\x30\x38\x33\x35\x30\x31\x34\x39\x30\x30\x30\x30\x38\x30\x31\x35\x36\x5E\x0A\x58\x36\xE1\xA7\xB7\x4B\x20\x00\x00\x00\x00\x00\x00\x00\x00\x13\x22\x00\x00\x31\x00\x05\x00\x31\x42\x43\x32\x37\x35\x33\x35", iRecv );
*/
#endif

   TOOL_Dump( LOG_DEBUG, "toTelcomrecv.dump", (unsigned char*)aczRecvBuf, iRecv );
   LOG4C(( LOG_DEBUG, "���ձ���:[Len=%d]", iRecv ));

   /* ������Ӧ��Ϣ */
   *opiRecvLen = iRecv;

   memcpy( opczRecvBuf, aczRecvBuf, *opiRecvLen );

   return 0;
}

/*
** function:do the standard templet(translate->send->receive->translate)
**          to ǰ��
** input:1 subservice identify
**       2 subservice request
**       3 service structure
** output:1 subservice answer list
** return:0-success,1-failed
**
** eiSubID,�ӷ�����
** eiSubSeq,�ӷ����ڲ����
** UDP epfReqList,Ҫ���͸��ⲿϵͳ������UDO
** UDP opfRspList,���ⲿϵͳ���յ�UDO
** SVC_Content *epstService,������Ϣ
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

   /*�ӷ���ĵ�ǰ�ӷ���ڵ�ȡ�����ڵ��*/
   iHostNodeId = epstService->pstSubSvcCurr->iHostId;
   pstSubSvcCfg = epstService->pstSubSvcCurr;

   /*ȡ������*/
   memset( aczTranCode, 0x00, sizeof(aczTranCode) );
   sprintf( aczTranCode, "%s", epstService->pstSubSvcCurr->aczTranCode );

   LOG4C(( LOG_DEBUG, "������:[%s], �ڵ��:[%d]", aczTranCode, iHostNodeId ));

   /* ��ȡ�����������ڵ�������Ϣ */
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
      strcpy( aczRetMsg, "ȡ���������Ϣ����" );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s %d", epstService->aczErrorMessage, iRet ));

      return ADP_GPNI_ERR;
   }

   /* ���������ڵ�Ų�ѯ�ýڵ����ϸ��Ϣ��������IP���˿ڡ����ӷ�ʽ��*/
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
      sprintf( aczRetMsg, "û�ж��������ڵ�%d�����״���%s", stPkgNeedInfo.iNodeId, stPkgNeedInfo.aczTranCode);
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_NO_HOST_DEF;
   }

   /* ���ݽ��״����ȡ�����ֵ��е�������Ϣ */
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
      sprintf( aczRetMsg, "ȡ�����ó���,�����ֵ���%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* ��CMP�ڲ���UDO��������ת��Ϊǰ��Ҫ���������� */
   /* ���м���Ϣ����ת��Ϊ������� */
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
      sprintf( aczRetMsg, "�ӷ���%d����������״���%s", eiSubID, stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* ȡ���׳�ʱʱ�� */
   iRet = ADP_GetService( eiSubID, &stAdpService );
   if( iRet != 0 )
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, -1,
                                    opfRspList );

      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      sprintf( aczRetMsg, "ȡ����ͨѶ��������" );
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


   /* �����������ݱ��Ľ��� */
   /*Ϊ�˴�����Ŵ��󷵻ر��ı��������Ķ�*/
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
      sprintf( aczRetMsg, "��ǰ��[%s]ͨѶ����", ADP_GetNodeName( iHostNodeId ) );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s", Telcom_HostNodeId, CMP_COM_ERR, aczRetMsg);

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));
      return ADP_COMM_ERR;
   }

   /*Ϊ�˴���64λ�ķ�����Ϣ��Ϣ���ͻ�������û�и���Ϣ�������������Ҫ*/
   /*Ϊ�˴�����Ŵ��󷵻ر��ı��������Ķ�*/
   lRecvLen += 1024;
  
   /* ���Ĵ���,��ȡ�����ֵ�������Ϣ */
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
      sprintf( aczRetMsg, "ȡ�����ó���,�����ֵ���%d", stPkgNeedInfo.iPkgAppId );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_FATAL, "%s", epstService->aczErrorMessage ));

      return ADP_GIFMFC_ERR;
   }

   /* ��Դ����ת��Ϊ�м���Ϣ���� */
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
      sprintf( aczRetMsg, "����������״���[%s]", stPkgNeedInfo.aczTranCode );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s",
               Telcom_HostNodeId, CMP_PKG_ERR, aczRetMsg );

      LOG4C(( LOG_DEBUG, "%s [%s] %d", epstService->aczErrorMessage,
              aczRecvBuf , lRecvLen ));

      return ADP_PKGTOMID_ERR;
   }


   UDO_GetField( opfRspList, 13, 0, aczRetCode, &iRet );

   /* ��ȡUDO�����еķ��ش������粻�ɹ��������ݿ��в�����������ʾ*/
   memset( aczRetCode, 0, sizeof(aczRetCode) );
   UDO_GetField( opfRspList, RET_CODE, 0, aczRetCode, &iRet );
   LOG4C(( LOG_DEBUG, "UDO_GetField response code[%s]", aczRetCode ));

   if( memcmp(aczRetCode, "0", 1) != 0 )  /*���ܵ�����Ӧ��*/
   {
      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      memset( aczChannelCode, 0, sizeof(aczChannelCode) );
      memset( aczRetMsg, 0, sizeof(aczRetMsg) );
      if( ADP_GetChanRet( iHostNodeId, epstService->iChannelId, aczRetCode, NULL,  aczChannelCode , aczRetMsg )!=0)
      {
          strcpy( aczChannelCode, "9000000009" );
          strcpy( aczRetMsg, "���Ҵ��������" );
      }
      UDO_ACField( opfRspList, RET_CODE,  UDT_STRING, aczChannelCode, RET_CODE_LEN );
      UDO_ACField( opfRspList, RET_MSG,  UDT_STRING, aczRetMsg, strlen(aczRetMsg) );

      LOG4C(( LOG_DEBUG, "To_telcom����ת��:response code[%s] aczRetMsg[%s]", aczRetCode, aczRetMsg ));
      return ADP_HOST_DEAL_FAIL;
   }

   /* ���ӻ��޸�UDO�����еķ�����Ϣ */ 
   LOG4C(( LOG_DEBUG, "ADP_ToTelcom ���׳ɹ�:[%s]", aczRetCode ));
   UDO_ACField( opfRspList, RET_MSG, UDT_STRING, "���׳ɹ�", 8 );
   return 0;
}



