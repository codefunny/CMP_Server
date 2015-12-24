#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ap_base.h"
#include "ap_handle.h"
#include "ap_global.h"
#include "transctrl.h"

#include "svc_control.h"
#include "udo.h"
#include "pkg/pkg_api.h"
#include "log.h"
#include "udo.h"

apr_pool_t *gpstLocalPool;
apr_pool_t *gpstRootPool;

apr_socket_t *pstClient;

AxpTxnPara stAxpTxnPara;

/**
  * for dynamic change config shm
  */
static int iKeyShmKey = 0;

int handleTxn( int iNodeId, int iDictId, char *pczTxCode, UDP pstReqList )
{
   SVC_Content stService;
   char         aczagserial[20];
   if( postCheck( pstReqList, "", 0 ) == -1 )
   {
      return -1;
   }
   memset(aczagserial,0x00,sizeof(aczagserial));
   getString( "agserial",aczagserial);

   UDO_Prt2File( LOG_INFO, "commudo.log", pstReqList, 
                 "From Channel Node [%d]Serial[%s]\n", iNodeId ,aczagserial );

   /*send service request message to monitor*/
   SVC_Prepare( (UDP)pstReqList, iNodeId, iDictId, pczTxCode, &stService );

   return 0;
}

/*ϵͳΨһһ�� C ����ȫ�ֱ���*/
char *pcShmAddr = NULL;

int doTrans( int iNodeId, int iDictId, char *pczRecvBuffer, int iRecvLen )
{
   int  iLen;
   char aczTxCode[100];
   char aczErrorMsg[512];
   char acReqUdo[4096];

   memset( aczTxCode, 0, sizeof(aczTxCode) );

#if 0
   if( preCheck( pczRecvBuffer, iRecvLen ) == -1 )
   {
      //LOG4C(( LOG_FATAL, "����Ԥ���ʧ�� %s\n", getString("error") ));
      return -1;
   }
#endif

   UDO_Init( acReqUdo, sizeof(acReqUdo) );
   UDO_sscanf( pczRecvBuffer, iRecvLen, acReqUdo );

   if( getTxCode( acReqUdo, aczTxCode ) == -1 )
   {
      UDO_Free( acReqUdo );
      return -1;
   }

   /**
     *        ��ʼ��־�ļ�: log4c�����ļ�ָ�����ļ���
     *        ���彻�״�����־Ϊ�����ļ���+������������,
     *        ���ļ��������̴����д��ݵ�������������,���󽫽�����־��¼��ͬһ�ļ��� 
     **/
   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX (aczTxCode);
   LOG4C_OPEN ();

   strcpy (stAxpTxnPara.m_aczTranCode, aczTxCode);
   stAxpTxnPara.m_iNodeId = iNodeId;
   stAxpTxnPara.m_iDictId = iDictId;

   if (transCtrl (iNodeId, aczTxCode)) {
      LOG4C(( LOG_FATAL, "�����ѱ�����Ϊֹͣ����\n"));
      return -1;
   }

   LOG4C(( LOG_DEBUG, "��ʼִ�н��� [%s]\n", aczTxCode ));
   handleTxn( iNodeId, iDictId, aczTxCode, acReqUdo );

   UDO_Free( acReqUdo );

   /**
     *        �ָ�ԭ������־�ļ��� 
     **/
   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX ("");
   LOG4C_OPEN ();

   return 0;
}

extern "C" {
int ADP_GetDictId( int iNodeId );
}

/**
  * ���Ϻ�����ϵͳ����ʱ˳������
  * ϵͳ����ʱ���� sysStart, ϵͳ�˳�ʱ���� sysEnd
  * 1 recvBuffer( pczBuffer, &lLen ), ���ʧ���򷵻� -1, ϵͳ���ټ�������
      �ɹ��򷵻�
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
   /**
     * add for dynamic change config shm
     **/
   int iCfgShmKey;
   /***/

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
      LOG4C(( LOG_FATAL, "���ش���, �ڵ�%d������\n", iNodeId ));
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

      pczBuffer = aczBuffer;
      apr_pool_clear( gpstLocalPool );
      pstProcess->m_bDoTxn = false;
      if( apr_socket_accept( &pstClient, pstListener, gpstLocalPool ) !=
          APR_SUCCESS )
      {
         iErrorCount++;
         if( iErrorCount == 10 )
         {
            LOG4C(( LOG_FATAL, "����10�� accept ���� %s\n", strerror(errno) ));
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
      /************************************************/

      iErrorCount = 0;

      pstProcess->m_tBegin = apr_time_sec (apr_time_now());
      /************************************/

      pstProcess->m_bDoTxn = true;

      if( onTxStart( pczLogicName ) == -1 )
      {
         LOG4C(( LOG_FATAL, "���׿�ʼ������ʧ��%s\n", strerror(errno) ));
         apr_socket_close( pstClient );
         continue;
      }

      tLen = sizeof(aczBuffer);
      if( (iRecvLen = recvBuffer( &pczBuffer, &tLen )) == -1 )
      {
         LOG4C(( LOG_FATAL, "�ӿͻ��˶���Ϣʧ��%s\n", strerror(errno) ));
         continue;
      }

      doTrans( iNodeId, iDictId, aczBuffer, (int)iRecvLen );

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
  * ����:
  * name socket procshmkey cfgshmkey maxchild index offset loginit maxruntimes nodeid
  * name        -   ��ϵͳ����
  * socket      -   �Ѿ��򿪵��ļ�������
  * procshmkey  -   ���̹�����Ϣ�����ڴ� KEY
  * cfgshmkey   -   ������Ϣ�����ڴ� KEY
  * maxchild    -   ÿ��ϵͳ����ӽ�����
  * index       -   ��ǰ��ϵͳ��ϵͳ�е�����
  * offset      -   ��ǰ�ӽ�������ϵͳ�е�ƫ����
  * loginit     -   ��־��ʼ���ļ�
  * maxruntimes -   ��ǰ�ӽ���������д���
  * nodeid      -   �ڵ���
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
      printf( "�ָ�socket %dʧ��%s", iSocket, strerror(errno) );
      return -1;
   }

   run( pstProcess, pstListener, iMaxRunTimes, iNodeId, aczName );

   return 0;
}

