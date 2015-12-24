#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>

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

int iInQueId, iOutQueId;

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

   LOG4C ((LOG_FATAL, "before PKG_GetInfoByTranCode"));
   iRet = PKG_GetInfoByTranCode( iNodeId, pczTxCode, 1, &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, 
              "ȡ���������ĳ���! ��������=[%d] ������=[%s]",
              iNodeId, pczTxCode));
      sprintf( aczErrorMsg, "ȡ�����ô�" );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1001;
   }

   LOG4C ((LOG_FATAL, "before PKG_GetFunId"));
   iRet = PKG_GetFunId( iNodeId, &iToMidTranCodeId, &iToMsgTranCodeId,
                        &iToMidId, &iToMsgId );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, 
              "ȡ�ڵ㱨�Ĳ����������! ��������=[%d]",
              iNodeId));
      sprintf( aczErrorMsg, "���Ĳ��ȡ����ת����������,������%d", iRet );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      free( stMsgFldCfgAry.pstMsgFldCfg );
      return 1002;
   }
   LOG4C ((LOG_FATAL, "before PKG_ToMid"));


   iRet = PKG_ToMid( iToMidId, pczRecvBuff, iRecvLen, iNodeId,
                     pczTxCode, 1, &stMsgFldCfgAry, acSvcRecvHead );
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 && iRet != PKG_LENGTH_OVER )
   {
      LOG4C ((LOG_FATAL, 
              "���������Ĳ������! ��������=[%d] ������=[%s]",
              iNodeId, pczTxCode));
      sprintf( aczErrorMsg, "����ת����UDO����,������%d", iRet );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1004;
   }

   LOG4C ((LOG_FATAL, "before postCheck"));
   if( postCheck( acSvcRecvHead, pczRecvBuff, iRecvLen ) == -1 )
   {
      UDO_Free( acSvcRecvHead );
      return -1;
   }

   LOG4C ((LOG_FATAL, "before UDO_Prt2File"));
   UDO_Prt2File( LOG_INFO, "commudo.log", acSvcRecvHead, 
                 "From Channel Node [%d]Serial[%s]\n", iNodeId ,aczagserial );
   /*send service request message to monitor*/
   SVC_Prepare( (UDP)acSvcRecvHead, iNodeId, iDictId, pczTxCode, &stService );
   UDO_Free( acSvcRecvHead );

   return 0;
}

/*ϵͳΨһһ�� C ����ȫ�ֱ���*/
char *pcShmAddr = NULL;

int doTrans( int iNodeId, int iDictId, char *pczRecvBuffer, int iRecvLen )
{
   int  iLen;
   char aczTxCode[100];
   char aczErrorMsg[512];

   memset( aczTxCode, 0, sizeof(aczTxCode) );

   if( preCheck( pczRecvBuffer, iRecvLen ) == -1 )
   {
      //LOG4C(( LOG_FATAL, "����Ԥ���ʧ�� %s\n", getString("error") ));
      return -1;
   }

   if( getTxCode_8583( pczRecvBuffer, aczTxCode , iRecvLen, iDictId) == -1 )
   {
      //LOG4C(( LOG_FATAL, "ȡ������ʧ�� %s\n", getString("error") ));
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

   if (transCtrl (iNodeId, aczTxCode)) {
      LOG4C(( LOG_FATAL, "�����ѱ�����Ϊֹͣ����\n"));
      sprintf (aczErrorMsg, "�����ѱ�����Ϊֹͣ����");
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return -1;
   }

   handleTxn( iNodeId, iDictId, aczTxCode, pczRecvBuffer, iRecvLen );

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
  * ���Ϻ�����ϵͳ����ʱ˳������
  * ϵͳ����ʱ���� sysStart, ϵͳ�˳�ʱ���� sysEnd
  * 1 recvBuffer( pczBuffer, &lLen ), ���ʧ���򷵻� -1, ϵͳ���ټ�������
      �ɹ��򷵻�
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
      LOG4C(( LOG_FATAL, "���ش���, �ڵ�%d������\n", iNodeId ));
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
         LOG4C(( LOG_FATAL, "�ӿͻ��˶���Ϣʧ��%s\n", strerror(errno) ));
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
         LOG4C(( LOG_FATAL, "���׿�ʼ������ʧ��%s\n", strerror(errno) ));
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
   char *pczTemp;
   int iInQueKey, iOutQueKey;


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

   LOG4C_INIT( aczLogIni );
   LOG4C_OPEN();

   pczTemp = getenv ("IN_QUE_KEY");
   if (pczTemp == NULL)
   {
       fprintf (stderr, "��������IN_QUE_KEYδ����!\n");
       LOG4C ((LOG_FATAL,"��������IN_QUE_KEYδ����!\n")); 
       return -1;
   }
   iInQueKey = atoi (pczTemp);
 
   pczTemp = getenv ("OUT_QUE_KEY");
   if (pczTemp == NULL)
   {
       fprintf (stderr, "��������OUT_QUE_KEYδ����!\n");
       LOG4C ((LOG_FATAL,"��������OUT_QUE_KEYδ����!\n")); 
       return -1;
   }
   iOutQueKey = atoi (pczTemp);

   iInQueId = msgget (iInQueKey, IPC_CREAT | 0660);
   if (iInQueId < 0) 
   {
       fprintf (stderr, "��ȡ�����������ʧ��! [%s] Key=[%s]\n",
                strerror (errno), iInQueKey);
       return -1;
   }
   
   iOutQueId = msgget (iOutQueKey, IPC_CREAT | 0660);
   if (iOutQueId < 0) 
   {
       fprintf (stderr, "��ȡ������Ӧ����ʧ��! [%s] Key=[%s]\n",
                strerror (errno), iOutQueKey);
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

   run( pstProcess, NULL, iMaxRunTimes, iNodeId, aczName );

   return 0;
}

