#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>

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

AxpTxnPara stAxpTxnPara;

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

   memset (&stService, 0, sizeof (stService));

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
   LOG4C(( LOG_DEBUG, "ToMidId[%d]" , iToMidId));


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
   SVC_Prepare( (UDP)acSvcRecvHead, iNodeId, iDictId, pczTxCode, &stService );
   UDO_Free( acSvcRecvHead );

   return 0;
}

/*系统唯一一个 C 语言全局变量*/
char *pcShmAddr = NULL;

int doTrans( int iNodeId, int iDictId, char *pczRecvBuffer, int iRecvLen )
{
   int  iLen;
   char aczTxCode[100];
   char aczErrorMsg[512];

   memset( aczTxCode, 0, sizeof(aczTxCode) );
   if( preCheck( pczRecvBuffer, iRecvLen ) == -1 )
   {
      LOG4C(( LOG_FATAL, "报文预检查失败\n"));
      return -1;
   }

   if( getTxCodeByLen( pczRecvBuffer, iRecvLen, aczTxCode ) == -1 )
   {
      LOG4C(( LOG_FATAL, "取交易码失败\n"));
      return -1;
   }

   /**
     *        起始日志文件: log4c配置文件指定的文件名
     *        具体交易处理日志为上述文件名+渠道服务交易码,
     *        此文件名在流程处理中传递到各主机适配器,以求将交易日志记录在同一文件中 
     **/
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

   strcpy (stAxpTxnPara.m_aczTranCode, aczTxCode);
   stAxpTxnPara.m_iNodeId = iNodeId;
   stAxpTxnPara.m_iDictId = iDictId;

   LOG4C(( LOG_DEBUG, "开始执行交易 [%s]\n", aczTxCode ));
   handleTxn( iNodeId, iDictId, aczTxCode, pczRecvBuffer, iRecvLen );
   
   /**
     *        恢复原来的日志文件名 
     **/
   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX ("");
   LOG4C_OPEN ();

   return 0;
}

#ifdef __cplusplus
extern "C"
{
#endif

int ADP_GetDictId( int iNodeId );

#ifdef __cplusplus
}
#endif

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
          int iMaxRunTimes, int iNodeId, char *pczLogicName )
{
   int iErrorCount = 0, iRecvLen, iDictId;
   unsigned int iAllowRunTimes, iRunTimes = 0;
   char aczBuffer[4096], *pczBuffer;
   apr_size_t tLen;
   int iInteractFlag = 0;
   /**
     * add for dynamic change config shm
     **/
   int iCfgShmKey;
   /***/

   /*设置为阻塞式等待*/
   apr_socket_timeout_set( pstListener, -1 );
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

      apr_pool_clear( gpstLocalPool );
      pstProcess->m_bDoTxn = false;
      if( apr_socket_accept( &pstClient, pstListener, gpstLocalPool ) !=
          APR_SUCCESS )
      {
         iErrorCount++;
         if( iErrorCount == 10 )
         {
            LOG4C(( LOG_FATAL, "连续10次 accept 出错 %s\n", strerror(errno) ));
            return;
         }
         continue;
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

      iErrorCount = 0;

      pstProcess->m_tBegin = apr_time_sec(apr_time_now());
      pstProcess->m_bDoTxn = true;

      if( onTxStart( pczLogicName ) == -1 )
      {
         LOG4C(( LOG_FATAL, "交易开始处理函数失败%s\n", strerror(errno) ));
         apr_socket_close( pstClient );
         continue;
      }

      memset (aczBuffer, 0, sizeof (aczBuffer));
      pczBuffer = aczBuffer;

      tLen = sizeof(aczBuffer);
      if( (iRecvLen = recvBuffer( &pczBuffer, &tLen )) == -1 )
      {
         LOG4C(( LOG_FATAL, "从客户端读信息失败%s\n", strerror(errno) ));
         goto TX_END;
      }

      doTrans( iNodeId, iDictId, aczBuffer, iRecvLen);

TX_END:

      apr_socket_close( pstClient );

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
   apr_socket_t *pstListener;
   apr_os_sock_info_t stSockInfo;

#ifdef AF_CCITT
   int tSockLen;
#else
   size_t tSockLen;
#endif


   if( argc != 11 )
   {
      printf( "Usage: %s name socket prcshmkey cfgshmkey maxchild index "
              "offset loginit maxruntims nodeid\n", argv[0] );
      return -1;
   }

   strcpy( aczName, argv[1] );
   iSocket = atoi( argv[2] );
   iProcShmKey = atoi( argv[3] );

   /**
     * modify for dynamic change config shm
     **
   iCfgShmKey  = atoi( argv[4] );
   ******/
   iKeyShmKey  = atoi( argv[4] );
   /*****/

   iMaxChild   = atoi( argv[5] );
   iIndex      = atoi( argv[6] );
   iOffset     = atoi( argv[7] );
   strcpy( aczLogIni, argv[8] );
   iMaxRunTimes = atoi( argv[9] );
   iNodeId      = atoi( argv[10] );

   
   memset (&stAxpTxnPara, 0, sizeof (stAxpTxnPara));
   stAxpTxnPara.m_iNodeId = iNodeId;

   LOG4C_INIT( aczLogIni );
   LOG4C_OPEN();

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
     * modify for dynamic change config shm
     **
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

   stSockInfo.os_sock = &iSocket;
   tSockLen = sizeof(struct sockaddr);
   stSockInfo.local = (sockaddr *)apr_palloc( gpstRootPool,
                                              tSockLen );
   getsockname( iSocket, stSockInfo.local, (socklen_t *)&tSockLen );
   stSockInfo.remote  = (sockaddr *)apr_palloc( gpstRootPool,
                                                tSockLen );
   getsockname( iSocket, stSockInfo.remote, (socklen_t *)&tSockLen );
   stSockInfo.family  = APR_INET;
   stSockInfo.type    = SOCK_STREAM;

   if( apr_os_sock_make( &pstListener, &stSockInfo, gpstRootPool ) !=
       APR_SUCCESS )
   {
      printf( "恢复socket %d失败%s", iSocket, strerror(errno) );
      return -1;
   }

   run( pstProcess, pstListener, iMaxRunTimes, iNodeId, aczName );

   return 0;
}

