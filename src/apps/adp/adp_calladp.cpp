#include "adp/adp.h"
#include "adp/adp_calladp.h"
#include "adp/host_syserr.h"
#include "ap_global.h"
#include "log.h"


#ifdef __cplusplus
extern "C"
{
#endif
void PUB_StrTrim( char *, char );
int ADP_GetChanRet( int enHostId, int enChanId, char *epczHostRetCode,
                            char *epczHostRetMsg, char *opczChanRetCode, char *opczChanRetMsg );
#ifdef __cplusplus
}
#endif

/*与ADC主机通讯*/
int ADP_AdpComm(char *epczHost, int eiPort,
                char *req,  char *res,  
		char *epcFun, char *epcTranCode, int eiTime)
{
   int ret;
   struct timeval stStart;
   struct timeval stEnd;
   double lStart, lEnd;

   AdpClient client( epczHost, eiPort );
   ret = client.call( epcFun, epcTranCode, req, res, eiTime );
   if( ret != 0 )
   {
      switch( client.getError( ) )
      {
      case 7:
      case 9:
         return 9999;
      default:
         return 9998;
      }
   }

   return 0;
}

/*
** function:do the standard templet(translate->send->receive->translate)
**          to ADC主机
** input:1 subservice identify
**       2 subservice request
**       3 service structure
** output:1 subservice answer list
** return:0-success,1-failed
*/
int ADP_StdAdp_Comm( int eiSubID, int eiSubSeq,
                     UDP epfReqList, UDP opfRspList,
                     SVC_Content * epstService )
{
   int iRet, iHostNodeId, iAdpRet=0;
   char aczRetCode[10], aczRetMesg[1024], aczHostRetCode[100];
   char aczRetMsg[101];
   char aczSuccRetCode[20];
   SUB_SvcList *pstSubSvcCfg;
   ADPService stAdpService;
   char aczFun[40], aczRetCodeTmp[128], aczChannelCode[20];
   int  iRetCodeNo, iRetMsgNo;
   int  changeReverseMode = REVERSE_NO_CHANGE;
   int  iInteractFlag = 0;
   int iUseRetChk=0;
   char aczAdcIp[40];
   char aczAdcPort[20];
   int iPort=0;

   assert( epfReqList != NULL );
   assert( opfRspList != NULL );
   assert( epstService != NULL );

   pstSubSvcCfg = epstService->pstSubSvcCurr;

   iHostNodeId = epstService->pstSubSvcCurr->iHostId;

   memset( aczFun, 0x00, sizeof( aczFun ) );
   memset( &stAdpService, 0, sizeof(stAdpService) );
   memset( aczRetCode, 0, sizeof (aczRetCode));
   memset( aczRetMsg, 0, sizeof(aczRetMsg) );

   /* 不对主机节点返回报文中返回码、返回信息域段号不同
      需在此增加分支语句，赋相应值 */
   memset( aczAdcIp, 0, sizeof(aczAdcIp) );
   memset( aczAdcPort, 0, sizeof(aczAdcPort) );

   iPort = atoi( aczAdcPort );
   switch (iHostNodeId)
   {
       case 200:
           sprintf(aczAdcIp,"%s", getenv("ADC_TMS_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_TMS_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
       case 201:
           sprintf(aczAdcIp,"%s", getenv("ADC_ACCT_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_ACCT_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
       case 203:
           sprintf(aczAdcIp,"%s", getenv("ADC_CONSOLE_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_CONSOLE_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
       case 205:
           sprintf(aczAdcIp,"%s", getenv("ADC_ACCOUNT_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_ACCOUNT_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
       case 208:
	   sprintf(aczAdcIp,"%s", getenv("ADC_OTHER_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_OTHER_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
	   case 209:
           sprintf(aczAdcIp,"%s", getenv("ADC_BANKCOMM_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_BANKCOMM_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
      case 210:
           sprintf(aczAdcIp,"%s", getenv("ADC_ALLINPAY_IP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_ALLINPAY_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
	 case 211:
		   sprintf(aczAdcIp,"%s", getenv("ADC_MITENO_IP"));
		   sprintf(aczAdcPort,"%s", getenv("ADC_MITENO_PORT"));
		   iPort = atoi( aczAdcPort );
		   iRetCodeNo = 2;
		   iRetMsgNo  = 3;
		   break;
       default:
           sprintf(aczAdcIp,"%s", getenv("ADCIP"));
           sprintf(aczAdcPort,"%s", getenv("ADC_POSP_PORT"));
           iPort = atoi( aczAdcPort );
           iRetCodeNo = 2;
           iRetMsgNo  = 3;
           break;
   }
  
  LOG4C ((LOG_DEBUG, "aczAdcIp[%s] aczAdcPort[%s]",aczAdcIp, aczAdcPort));
   iRet = ADP_GetService( eiSubID, &stAdpService );
   if( iRet == -10 )
   {
      LOG4C ((LOG_FATAL, "取ADC主机通讯参数错误,请检查表"));
      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, 2,
                                    opfRspList );
      UDO_ACField( opfRspList, iRetCodeNo, 0, "9999", 9 );
      UDO_ACField( opfRspList, iRetMsgNo, 0, "取ADC主机通讯参数错误,请检查表", 60 );

      epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;

      return HOST_SEND_FAIL;
   }

   LOG4C ((LOG_DEBUG, "[%d][%s][%d][%s][%s][%d]",
           iRet, aczAdcIp, iPort, stAdpService.aczService, pstSubSvcCfg->aczTranCode,
           stAdpService.iTimeOut ));

   iRet = ADP_AdpComm(  aczAdcIp, iPort,
                        epfReqList, opfRspList,
                        stAdpService.aczService, pstSubSvcCfg->aczTranCode, 
                        stAdpService.iTimeOut);
   if (iRet != 0)
   {
      LOG4C(( LOG_DEBUG, "ADP_AdpComm ret[%d]", iRet ));
      if( iRet != 9999 )
      {
         /*如果不是接收失败,该子服务不用冲正也不用记冲正日志*/
         epstService->pstSubSvcTail->iErrorHandleType = SUB_ERR_NOOP;
      }

      ADP_CreateNullListByTranCode( pstSubSvcCfg->iHostId,
                                    pstSubSvcCfg->aczTranCode, 2,
                                    opfRspList );

      sprintf( aczRetMesg, "与主机[%s]通讯出错", ADP_GetNodeName( iHostNodeId ) );

      UDO_ACField( opfRspList, iRetCodeNo, 0, "9999999", 7 );
      UDO_ACField( opfRspList, iRetMsgNo, 0, aczRetMesg,
                    strlen(aczRetMesg) );

      sprintf( epstService->aczErrorMessage,
               "%d|%d|%s", CMP_NODE_ID, CMP_COM_ERR, aczRetMesg);

      LOG4C (( LOG_FATAL, "%s", epstService->aczErrorMessage));

      if (iRet != 9999)
      {
          return HOST_SEND_FAIL;
      }
      else
      {
          return HOST_RECV_FAIL;
      }
   }

   memset( aczRetCode, 0x00, sizeof( aczRetCode ) );
   iRet = UDO_GetField( opfRspList, iRetCodeNo, 0, aczRetCode, 0 );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
            "%d|-9002|ADC主机没有返回响应码，交易代码%s",
            CMP_NODE_ID, stAdpService.aczTxnCode );
      LOG4C(( LOG_FATAL, "ADC返回缺少响应码字段(%d号域)!", iRetCodeNo ));

      UDO_ACField( opfRspList, iRetCodeNo, 0, "9999998", 9 );
      UDO_ACField( opfRspList, iRetMsgNo, 0, "取ADC主机应答码失败", 60 );
      return ADP_GIFMFC_ERR;
   }
   aczRetCode[9] = '\0';  

   iRet = UDO_GetField( opfRspList, iRetMsgNo, 0, aczRetMsg, 0 );
   if( iRet != 0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|-9002|ADC主机没有返回响应信息，交易代码%s",
               CMP_NODE_ID, stAdpService.aczTxnCode );
      LOG4C(( LOG_FATAL, "ADC返回缺少响应信息字段(%d号域)!", iRetMsgNo ));
      UDO_ACField( opfRspList, iRetCodeNo, 0, "9999997", 9 );
      UDO_ACField( opfRspList, iRetMsgNo, 0, "取ADC主机应答信息失败", 60 );
      return ADP_GIFMFC_ERR;
   }
   aczRetMsg[70] = '\0';


   LOG4C(( LOG_DEBUG, "完成调用ADC主机[%s:%d]节点[%d]下的函数[%s],"
           "交易码[%s],超时时间[%d],返回码[%s],返回信息[%s]",
           stAdpService.aczServerIp, stAdpService.iServerPort,
           iHostNodeId, stAdpService.aczService,
           pstSubSvcCfg->aczTranCode, stAdpService.iTimeOut,
           aczRetCode, aczRetMsg ));

   PUB_StrTrim( aczRetCode,' ');

   memset( aczRetCodeTmp,0x00,sizeof( aczRetCodeTmp ) );
   strcpy( aczRetCodeTmp,aczRetCode );

   PUB_StrTrim( aczRetCodeTmp,'0' );

   if( strlen( aczRetCodeTmp )!=0 )
   {
      sprintf( epstService->aczErrorMessage,
               "%d|%s|%s", iHostNodeId, aczRetCode, aczRetMsg );
      LOG4C(( LOG_FATAL, "调用ADC模块处理失败:[%s][%s]",
              aczRetCode, aczRetMsg ));
      /*if( iHostNodeId == 200 )
      {
          memset( aczChannelCode, 0, sizeof(aczChannelCode) );
          if( ADP_GetChanRet( iHostNodeId, epstService->iChannelId, aczRetCodeTmp, NULL, aczChannelCode , aczRetMsg )!=0)
          {
               strcpy( aczChannelCode, "96" );
               strcpy( aczRetMsg, "查找错误码出错" );
          }
          UDO_ACField( opfRspList, iRetCodeNo, 0, aczChannelCode, strlen(aczChannelCode) );
          UDO_ACField( opfRspList, iRetMsgNo,  0, aczRetMsg, strlen(aczRetMsg) );
      }
      */
      return CMP_HOST_ERR;
   }

   return 0;
}
