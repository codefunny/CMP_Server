#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <cmqc.h>

#include "ap_base.h"
#include "ap_handle.h"
#include "ap_global.h"

#include "transctrl.h"

#include "svc_control.h"
#include "udo.h"
#include "pkg/pkg_api.h"
#include "log.h"
#include "shm_base.h"

apr_pool_t *gpstLocalPool;
apr_pool_t *gpstRootPool;

apr_socket_t *pstClient;

#ifdef __cplusplus
extern "C"
{
#endif

MQHCONN MQHcon; 
MQHOBJ  MQInQueObj, MQOutQueObj;

#ifdef __cplusplus
}
#endif

/**
  * add for dynamic change config shm
  */
static int iKeyShmKey = 0;

int handleTxn( int iNodeId, int iDictId, char *pczTxCode,
               char *pczRecvBuff, int iRecvLen )
{
   char aczErrorMsg[512];
   int iToMidTranCodeId, iToMsgTranCodeId, iToMidId, iToMsgId, iLen;
   int iRet;
   char acSvcRecvHead[30*1024];
   char         aczagserial[20];
   SVC_Content stService;
   MsgFldCfgAry stMsgFldCfgAry;

   UDO_Init( acSvcRecvHead, sizeof(acSvcRecvHead) );
   memset(aczagserial,0x00,sizeof(aczagserial));
   getString( "agserial",aczagserial);

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );

   iRet = PKG_GetInfoByTranCode( iNodeId, pczTxCode, 1, &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, 
              "取服务请求报文出错! 渠道结点号=[%d] 交易码=[%s]",
              iNodeId, pczTxCode));
      sprintf( aczErrorMsg, "取域配置错" );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1001;
   }

   iRet = PKG_GetFunId( iNodeId, &iToMidTranCodeId, &iToMsgTranCodeId,
                        &iToMidId, &iToMsgId );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, 
              "取节点报文拆包函数出错! 渠道结点号=[%d]",
              iNodeId));
      sprintf( aczErrorMsg, "报文拆包取查找转换函数出错,返回码%d", iRet );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      free( stMsgFldCfgAry.pstMsgFldCfg );
      return 1002;
   }


   iRet = PKG_ToMid( iToMidId, pczRecvBuff, iRecvLen, iNodeId,
                     pczTxCode, 1, &stMsgFldCfgAry, acSvcRecvHead );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 && iRet != PKG_LENGTH_OVER )
   {
      LOG4C ((LOG_FATAL, 
              "服务请求报文拆包出错! 渠道结点号=[%d] 交易码=[%s]",
              iNodeId, pczTxCode));
      sprintf( aczErrorMsg, "报文转换到UDO出错,返回码%d", iRet );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1004;
   }

   if( postCheck( acSvcRecvHead, pczRecvBuff, iRecvLen ) == -1 )
   {
      UDO_Free( acSvcRecvHead );
      return -1;
   }

   UDO_Prt2File( LOG_INFO, "commudo.log", acSvcRecvHead, 
                 "From Channel Node [%d]Serial[%s]\n", iNodeId ,aczagserial );

   /*send service request message to monitor*/
   SVC_Prepare( (UDP)acSvcRecvHead, iNodeId, iDictId, pczTxCode, &stService );
   UDO_Free( acSvcRecvHead );

   return 0;
}

char *pcShmAddr = NULL;

int doTrans( int iNodeId, int iDictId, char *pczRecvBuffer, int iRecvLen )
{
   int  iLen;
   char aczTxCode[100];
   char aczErrorMsg[512];

   memset( aczTxCode, 0, sizeof(aczTxCode) );

   if( preCheck( pczRecvBuffer, iRecvLen ) == -1 )
   {
      return -1;
   }

   if( getTxCode( pczRecvBuffer, aczTxCode ) == -1 )
   {
      return -1;
   }

   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX (aczTxCode);
   LOG4C_OPEN ();

   if (transCtrl (iNodeId, aczTxCode)) {
      LOG4C(( LOG_FATAL, "交易已被设置为停止运行\n"));
      sprintf (aczErrorMsg, "交易已被设置为停止运行");
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return -1;
   }

   handleTxn( iNodeId, iDictId, aczTxCode, pczRecvBuffer, iRecvLen );

   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX ("");
   LOG4C_OPEN ();

   return 0;
}

extern "C" {
int ADP_GetDictId( int iNodeId );
}

#if 0
int AP_GetDictId( int iNodeId )
{
   int iRow, iTemp;

   pstNodeTranCfg = (NodeTranCfg*)SHM_ReadData( pcShmAddr, NODETRANCFG, &iRow);
   for( iTemp = 0; iTemp < iRow; iTemp++ )
   {
      if( pstNodeTranCfg->iNodeId == iNodeId )
      {
         return pstNodeTranCfg->iMsgAppId;
      }
      pstNodeTranCfg++;
   }

   return -1;
}
#endif

/**
  * 以上函数在系统运行时顺序如下
  * 系统启动时调用 sysStart, 系统退出时调用 sysEnd
  * 1 recvBuffer( pczBuffer, &lLen ), 如果失败则返回 -1, 系统不再继续处理
      成功则返回
  * 2 doTrans( int iNodeId, char *pczRecvBuffer, int iRecvLen );
  * 3 sendBuffer( pczBuffer, int iSendLen );
  */
void run( ProcessRegister *pstProcess, apr_socket_t *pstListener,
          int iMaxRunTimes, int iNodeId, char *pczExecFile )
{
   int iErrorCount = 0, iRecvLen, iDictId;
   unsigned int iAllowRunTimes, iRunTimes = 0;
   char aczBuffer[4096], *pczBuffer;
   apr_size_t tLen;
   apr_pool_t *pstSocketPool;
   /** 
     * add for dynamic change config shm
     **/
   int iCfgShmKey;
   /***/

   if( iMaxRunTimes <= 0 )
   {
      iAllowRunTimes = (unsigned int)-1;
   }
   else
   {
      iAllowRunTimes = iMaxRunTimes;
   }

   if( (iDictId = ADP_GetDictId(iNodeId) ) == -1 )
   {
      LOG4C(( LOG_FATAL, "严重错误, 节点%d不存在\n", iNodeId ));
      return;
   }

   /** 
     * add for dynamic change config shm 
     **/
   if( pcShmAddr != NULL )
   {
       shmdt( pcShmAddr );
       pcShmAddr = NULL;
    }
   /************************************************/

   iErrorCount = 0;
   pstProcess->m_bDoTxn = false;

   while( pstProcess->m_bActive && iRunTimes < iAllowRunTimes )
   {
      /** 
        * add for dynamic change config shm 
        **/
      if( pcShmAddr != NULL )
      {
          shmdt( pcShmAddr );
          pcShmAddr = NULL;
      }
      /************************************************/

      pczBuffer = aczBuffer;
      pstProcess->m_bDoTxn = false;
      apr_pool_clear( gpstLocalPool );
      iErrorCount = 0;

      tLen = sizeof(aczBuffer);
      if( (iRecvLen = recvBuffer( &pczBuffer, &tLen )) == -1 )
      {
         LOG4C(( LOG_FATAL, "从客户端读信息失败%s\n", strerror(errno) ));
         return;
      }

      LOG4C_CLOSE();
      LOG4C_OPEN();

      /** 
        * add for dynamic change config shm 
        **/
      pcShmAddr = (char *)Shm_GetCfgShm (iKeyShmKey, &iCfgShmKey);
      if (pcShmAddr == NULL)
      {
          LOG4C ((LOG_FATAL,
                 "attach get channal node %d config memory error\n"
                 "KEYSHM IPCKEY = %d\n", iNodeId, iKeyShmKey));
          continue;
      }   

      pstProcess->m_tBegin = apr_time_sec (apr_time_now());

      pstProcess->m_bDoTxn = true;

      if( onTxStart( pczExecFile ) == -1 )
      {
         LOG4C(( LOG_FATAL, "交易开始处理函数失败%s\n", strerror(errno) ));
         return;
      }

      doTrans( iNodeId, iDictId, aczBuffer, (int)tLen );

      onTxEnd();

      iRunTimes++;
      pstProcess->m_iRunTimes = iRunTimes;
   }
}

void atExitFun( void )
{
   onSysEnd();

   apr_pool_destroy( gpstRootPool );

   apr_terminate();

   LOG4C_CLOSE();
}

void sig_usr1( int iSignal )
{
   exit(0);
}

void setupSignals( void )
{
   apr_signal( SIGTERM, SIG_IGN );
   apr_signal( SIGINT,  SIG_IGN );
   apr_signal( SIGPIPE, SIG_IGN );
   apr_signal( SIGHUP,  SIG_IGN );
   apr_signal( SIGCHLD, SIG_IGN );
   apr_signal( SIGTTOU, SIG_IGN );
   apr_signal( SIGTTIN, SIG_IGN );
   apr_signal( SIGTSTP, SIG_IGN );
}

/**
  * 参数:
  * name socket procshmkey cfgshmkey maxchild index offset loginit maxruntimes nodeid
  * name        -   子系统名字
  * socket      -   已经打开的文件描述符
  * procshmkey  -   进程管理信息共享内存 KEY
  * cfgshmkey   -   配置信息共享内存 KEY
  * maxchild    -   每个系统最大子进程数
  * index       -   当前子系统在系统中的索引
  * offset      -   当前子进程在子系统中的偏移量
  * loginit     -   日志初始化文件
  * maxruntimes -   当前子进程最大运行次数
  * nodeid      -   节点编号
  */
int main( int argc, char *argv[] )
{
   char aczName[64], aczLogIni[128];
   int iSocket, iProcShmKey, iCfgShmKey, iMaxChild;
   int iIndex, iOffset, iMaxRunTimes, iNodeId;
   ProcessRegister *pstProcess;
   char *pczTemp;
   char aczMQManager[64];
   char aczMQInQue[64];
   char aczMQOutQue[64];

   if( argc != 11 )
   {
      printf( "Usage: %s name socket prcshmkey cfgshmkey maxchild index "
              "offset loginit maxruntims nodeid\n", argv[0] );
      return -1;
   }

   strcpy( aczName, argv[1] );
   iSocket = atoi( argv[2] );
   iProcShmKey = atoi( argv[3] );

   iKeyShmKey  = atoi( argv[4] );

   iMaxChild   = atoi( argv[5] );
   iIndex      = atoi( argv[6] );
   iOffset     = atoi( argv[7] );
   strcpy( aczLogIni, argv[8] );
   iMaxRunTimes = atoi( argv[9] );
   iNodeId      = atoi( argv[10] );

   LOG4C_INIT( aczLogIni );
   LOG4C_OPEN();

   pczTemp = getenv ("QUE_MANAGER");
   if (pczTemp == NULL)
   {
       fprintf (stderr, "环境变量QUE_MANAGER未设置!\n");
       LOG4C ((LOG_FATAL,"环境变量QUE_MANAGER未设置!\n")); 
       return -1;
   }
   strcpy (aczMQManager, pczTemp);

   pczTemp = getenv ("IN_QUE_KEY");
   if (pczTemp == NULL)
   {
       fprintf (stderr, "环境变量IN_QUE_KEY未设置!\n");
       LOG4C ((LOG_FATAL,"环境变量IN_QUE_KEY未设置!\n")); 
       return -1;
   }
   strcpy (aczMQInQue, pczTemp);
 
   pczTemp = getenv ("OUT_QUE_KEY");
   if (pczTemp == NULL)
   {
       fprintf (stderr, "环境变量OUT_QUE_KEY未设置!\n");
       LOG4C ((LOG_FATAL,"环境变量OUT_QUE_KEY未设置!\n")); 
       return -1;
   }
   strcpy (aczMQOutQue, pczTemp);

   MQLONG MQCompCode;
   MQLONG MQReason;

   /* 连接MQ管理器 */
   MQCONN(aczMQManager,              /* queue manager                  */
          &MQHcon,                   /* connection handle              */
          &MQCompCode,               /* completion code                */
          &MQReason);                /* reason code                    */
   if (MQCompCode == MQCC_FAILED)     
   {
       fprintf(stderr, "MQCONN ended with reason code[%d][%s]\n", MQReason, aczMQManager);
       LOG4C ((LOG_FATAL, "MQCONN ended with reason code[%d][%s]\n", MQReason, aczMQManager));
       return -1;
   }

   /* 打开接收请求队列 */
   MQOD     MQod = {MQOD_DEFAULT};    /* Object Descriptor             */
   MQLONG   MQOpt;  

   strcpy (MQod.ObjectName, aczMQInQue);
   MQOpt =  MQOO_INPUT_SHARED          /* open queue for input     */
          | MQOO_FAIL_IF_QUIESCING     /* but not if MQM stopping   */
            ;

   MQOPEN(MQHcon,                      /* connection handle            */
          &MQod,                       /* object descriptor for queue  */
          MQOpt,                       /* open options                 */
          &MQInQueObj,                 /* object handle                */
          &MQCompCode,                 /* MQOPEN completion code       */
          &MQReason);                  /* reason code                  */
   if (MQCompCode == MQCC_FAILED)
   {
       fprintf(stderr, "MQOPEN ended with reason code %d\n", MQReason);
       LOG4C ((LOG_FATAL, "MQOPEN ended with reason code %d\n", MQReason));
       MQDISC (&MQHcon, &MQCompCode, &MQReason);
       return -1;
   }
   
   strcpy (MQod.ObjectName, aczMQOutQue);
   MQOpt =  MQOO_OUTPUT                /* open queue for output     */
          | MQOO_FAIL_IF_QUIESCING     /* but not if MQM stopping   */
            ;

   /* 打开发送响应队列 */
   MQOPEN(MQHcon,                      /* connection handle            */
          &MQod,                       /* object descriptor for queue  */
          MQOpt,                       /* open options                 */
          &MQOutQueObj,                /* object handle                */
          &MQCompCode,                 /* MQOPEN completion code       */
          &MQReason);                  /* reason code                  */
   if (MQCompCode == MQCC_FAILED)
   {
       fprintf(stderr, "MQOPEN ended with reason code %d\n", MQReason);
       LOG4C ((LOG_FATAL, "MQOPEN ended with reason code %d\n", MQReason));
       MQCLOSE (MQHcon, &MQInQueObj, MQCO_NONE, &MQCompCode, &MQReason);
       MQDISC (&MQHcon, &MQCompCode, &MQReason);
       return -1;
   }
   
   apr_initialize();
   atexit( atExitFun );

   setupSignals();

   apr_pool_create( &gpstRootPool, NULL );
   apr_pool_create( &gpstGlobalVariablePool, gpstRootPool );
   apr_pool_create( &gpstLocalPool, gpstRootPool );

   pstProcess = (ProcessRegister*)shm_get( iProcShmKey, 0 );
   if( pstProcess == NULL )
   {
      return -1;
   }
   pstProcess += iMaxChild*iIndex + iOffset;

   /**
   if( iCfgShmKey > 0 )
   {
      pcShmAddr = (char*)shm_get( iCfgShmKey, 0 );
      if( pcShmAddr == NULL )
      {
         return -1;
      }
   }
   *********/
   pcShmAddr = (char *)Shm_GetCfgShm (iKeyShmKey, &iCfgShmKey);
   if( pcShmAddr == NULL )
   {
       return -1;
   }
   /********/
   if( onSysStart( aczName ) == -1 )
   {
      return -1;
   }

   run( pstProcess, NULL, iMaxRunTimes, iNodeId, aczName );

   return 0;
}

