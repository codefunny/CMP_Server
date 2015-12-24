#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#ifdef __hpux
#include <sys/wait.h>
#else
#include <wait.h>
#endif

//#include "sqlite.h"
#include "easysock.h"
#include "log.h"

int iRecvListenFd = -1;
int iSendListenFd = -1;
int iRecvMaxNum   = -1;

int gIsTimeout = 0;
int client = 0;

int giQueueId;

void readWatchNodes( int client );

pthread_mutex_t monitor_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

#define DEBUG_REPORT

#ifdef DEBUG_REPORT
int iEnqueueNum = 0;
int iDequeueNum = 0;
int iDesertNum  = 0;
#endif

int giMsgNum = 0;

#define MAX_MONITOR     300
/*#define PTHREAD_THREADS_MAX 128*/
#define MYTHREAD_THREADS_MAX 128

FILE *addrFile;
static const char *ADDRFILE="addr.log";
static char ADDrFILe[300];
/*static char WritLogFile[300];*/
char WritLogFile[300];
FILE *fTraceLog = NULL;

int giConfigKey;
char *pcShmAddr;
void DealTimeout();

typedef struct _MonitorAddr MonitorAddr;
struct _MonitorAddr
{
   int  m_Active;       /* 0-dead, 1-active */
   char m_AddrIp[20];
   int  m_Port;
   int  m_ErrorTimes;
   int  m_Server;
   FILE *m_Fsocket;
   struct sockaddr_in sk;
   pthread_mutex_t sm ;
   char m_checktime[7];
};
struct _MonitorAddr astMonitorAddr[MAX_MONITOR];   /* 最多300个监控点 */
/*pthread_mutex_t astMonitorMutex[MAX_MONITOR]; */
int iMonitorNum = 0;

pthread_mutex_t mutexAddr;
pthread_mutex_t mutexAccept;

typedef struct _MessageQueue
{
   long m_Type;
   char m_Buffer[2];
} MessageQueue;

MessageQueue stMessageQueue = { 1, '0' };

#define ERRORHANDLE( pos )\
{\
   iMonitorNum--;\
   memset( &astMonitorAddr[pos], 0, sizeof(MonitorAddr) );\
}
#define CLEARHANDLE( pos )\
{\
   iMonitorNum--;\
   memset( &astMonitorAddr[pos], 0, sizeof(MonitorAddr) );\
}


void TOOL_GetSystemhhmmsstmp( char *pchhmmss )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   if( pchhmmss == NULL ) return ;

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );


 sprintf( pchhmmss, "%02d%02d%02d", pstTM->tm_hour,
                                    pstTM->tm_min,
                                    pstTM->tm_sec );
}
void initCache( void )
{
   int i, j, iAlive=30;
   FILE *connectLog;
#if 0
   addrFile = fopen( ADDrFILe, "r+" );
   fseek( addrFile, 0, SEEK_SET );
   fread( &iMonitorNum, sizeof(iMonitorNum), 1, addrFile );
   fread( astMonitorAddr, sizeof(astMonitorAddr), 1, addrFile );
   fclose( addrFile );
#endif
   fTraceLog = fopen( WritLogFile, "a" );
   fprintf( fTraceLog, "%d in %s enter initCache\n",
            __LINE__, __FILE__ );
   fclose(fTraceLog);

   pthread_mutex_init( &mutexAddr, NULL );
   pthread_mutex_init( &mutexAccept, NULL );

   for( i = 0; i < MAX_MONITOR; i++ )
   {
      pthread_mutex_init( &astMonitorAddr[i].sm, NULL );
   }

   /** 建立和监控端的联接 **/
   pthread_mutex_lock( &mutexAddr );
   connectLog = fopen( WritLogFile, "a" );
   for( i = 0, j = 0; i < iMonitorNum && j < MAX_MONITOR; i++ )
   {
      if( connectLog != NULL )
      {
         fprintf( connectLog, "connect to %s : %d\n",
                  astMonitorAddr[i].m_AddrIp, astMonitorAddr[i].m_Port );
      }
      if( astMonitorAddr[i].m_Active == 1 )
      {
      }
   }
   if( connectLog != NULL )
   {
      fclose( connectLog );
   }
   pthread_mutex_unlock( &mutexAddr );
}

void Usage( char * pczName )
{
   printf( "Usage: %s -r recv_port -n accept_num -s send_port -m key -h|-H\n",
           pczName );
   printf( "-r recv_port         监控接收侦听的端口\n" );
   printf( "-n accept_num        监控侦听最大连接数\n" );
   printf( "-s send_port         监控发送侦听的端口\n" );
   printf( "-m key               配置信息共享内存KEY\n" );
   printf( "-h|-H                帮助信息\n" );
}

int iContinue = 1;

void doExit( int sig )
{
   /** 设置为退出 **/
   iContinue = 0;
   printf( "****************** exit ********************\n" );fflush(stdout);
}

void sendMessage( char *epczMessage )
{
   int i, iSock, j;
   char aczConfirm[2];
   FILE *fp;

   pthread_mutex_lock (&monitor_queue_mutex);
   SaveToDataBase( epczMessage );
   pthread_mutex_unlock (&monitor_queue_mutex);
   return;

   for( i = 0, j = 0; i < MAX_MONITOR && j < iMonitorNum; i++ )
   {
      if( astMonitorAddr[i].m_Server > 0 )
      {
         pthread_mutex_lock( &astMonitorAddr[i].sm );
         if( astMonitorAddr[i].m_Active == 0 )
         {
            pthread_mutex_unlock( &astMonitorAddr[i].sm );
            continue;
         }
         if( tcp_wait_send( astMonitorAddr[i].m_Server, 1000 ) == false )
         {
            printf( "wait send timeout [%s]\n", strerror(errno) );
            fflush(stdout);
            tcp_close( astMonitorAddr[i].m_Server );
            pthread_mutex_lock( &mutexAddr );
            ERRORHANDLE( i );
            pthread_mutex_unlock( &mutexAddr );
            pthread_mutex_unlock( &astMonitorAddr[i].sm );
            continue;
         }
         /** TODO: 检测联接已经断开, 避免send语句不正常进程退出 **/
         if( sendto( astMonitorAddr[i].m_Server, epczMessage,
                     strlen(epczMessage), 0 ,
                     (struct sockaddr *)&astMonitorAddr[i].sk,
                     sizeof(astMonitorAddr[i].sk) )
             == -1 )
         {
            printf( "tcp_raw_send error [%d][%s]\n", 
                    errno, strerror(errno) );
            fflush(stdout);
            fclose( astMonitorAddr[i].m_Fsocket );
            tcp_close( astMonitorAddr[i].m_Server );
            pthread_mutex_lock( &mutexAddr );
            ERRORHANDLE( i );
            pthread_mutex_unlock( &mutexAddr );
            pthread_mutex_unlock( &astMonitorAddr[i].sm );
            continue;
         }
         fflush(astMonitorAddr[i].m_Fsocket);
         fflush(stdout);
         j++;
         pthread_mutex_unlock( &astMonitorAddr[i].sm );
      }
   }
}

void * handleRecv( void *eviSeq )
{
#if 0
   FILE *tlog;
   char filename[100];
   sprintf( filename, "%d.log", pthread_self() );
   tlog = fopen( filename, "w" );

   fprintf( fTraceLog, "%d in %s enter handleRecv\n",
            __LINE__, __FILE__ );
#endif
   while( iContinue == 1 )
   {
      int iNewSock;
      char aczRecv[2048];

      pthread_mutex_lock( &mutexAccept );
      iNewSock = tcp_accept( iRecvListenFd, -1 );
      pthread_mutex_unlock( &mutexAccept );
      if( iNewSock == -1 )
      {
         continue;
      }

      memset( aczRecv, 0, sizeof(aczRecv) );
      if( tcp_wait_recv( iNewSock, 2000 ) == false )
      {
         tcp_close( iNewSock );
         continue;
      }

      /** 接收的 buf 格式: nodeid,directflag,udolen,udostr **/
      if( tcp_raw_recv( iNewSock, aczRecv, 1000 ) == -1 )
      {
         tcp_close( iNewSock );
         continue;
      }

      if( tcp_wait_send( iNewSock, 1000 ) == true )
      {
         tcp_raw_send( iNewSock, "o", 1 );
      }

      if( tcp_wait_recv( iNewSock, 2000 ) == false )
      {
         tcp_close( iNewSock );
      }
      if( tcp_raw_recv( iNewSock, aczRecv, 1000 ) == -1 )
      {
         tcp_close( iNewSock );
      }

      tcp_close( iNewSock );

      sendMessage( aczRecv );
   }
   return NULL;
}

int handleCommand( char *epczCommand, char *epczIp, char *epczPort )
{
   int i,j, iPort, iAlive=30;
   FILE *connectLog;
   struct in_addr * addp1;
   struct hostent * hostinfo1;
   char aczSystime[7];

   fTraceLog = fopen( WritLogFile, "a" );
   fprintf( fTraceLog, "%d in %s enter handleCommand %s\n",
            __LINE__, __FILE__ ,epczCommand);
   fclose(fTraceLog);
   iPort = atoi(epczPort);
   memset(aczSystime,0,sizeof(aczSystime));
   TOOL_GetSystemhhmmsstmp(aczSystime);
   if( strcmp( epczCommand, "login" ) == 0 )
   {
      if( iMonitorNum == MAX_MONITOR )
      {
         return -1;
      }
  //    addrFile = fopen( ADDrFILe, "r+" );\
      /* 检查是否已经有指定的地址和端口 */
      for( i = 0; i < MAX_MONITOR; i++ )
      {
         if( astMonitorAddr[i].m_Active == 0 ) /* 该位置可用 */
         {
            j = i;
            continue;
            /* break;*/
         }
         if( strcmp( epczIp, astMonitorAddr[i].m_AddrIp ) == 0 &&
             iPort == astMonitorAddr[i].m_Port )
         {
            CLEARHANDLE( i );
            continue;
#if 0
            /* 如果有相同地址和端口的记录断开以前的联接 建立新联接 */
            if( astMonitorAddr[i].m_Active == 1 )
            {
               tcp_close( astMonitorAddr[i].m_Server );
            }
            break;
#endif
         }
      }
      //fclose( addrFile );
      i = j ;
      connectLog = fopen( WritLogFile, "a" );
      if( connectLog != NULL )
      {
         fprintf( connectLog, "%s in %d connect to %s : %d\n", __FILE__,
                  __LINE__,epczIp, iPort );
      }
      astMonitorAddr[i].m_Server = socket(AF_INET,SOCK_DGRAM,0);
      if ( (hostinfo1=(struct hostent*)gethostbyname(epczIp)) == NULL )
      {
         fclose(connectLog);
         return -1;
      }

      addp1=(struct in_addr *)*(hostinfo1->h_addr_list);
      astMonitorAddr[i].sk.sin_addr=*addp1;
      astMonitorAddr[i].sk.sin_family=AF_INET;
      astMonitorAddr[i].sk.sin_port=htons(iPort);

      fprintf( connectLog, "astMonitorAddr[i].m_Server is [%d]\n",
               astMonitorAddr[i].m_Server );
      if( astMonitorAddr[i].m_Server == -1 )
      {
         if( connectLog != NULL )
         {
            fprintf( connectLog, "%s in %d connect to %s : %d error %s\n",
                     __FILE__,__LINE__, epczIp, iPort, strerror(errno) );
         }
         fclose( connectLog );
         return -1;
      }

      strcpy( astMonitorAddr[i].m_AddrIp, epczIp );
      strcpy( astMonitorAddr[i].m_checktime, aczSystime );
      astMonitorAddr[i].m_Port = iPort;
      astMonitorAddr[i].m_Active = 1;
      astMonitorAddr[i].m_ErrorTimes = 0;
      
      fprintf( connectLog, "connect to %s : %d file descriptor %d\n",
               astMonitorAddr[i].m_AddrIp, astMonitorAddr[i].m_Port,
               astMonitorAddr[i].m_Server );
      iMonitorNum++;
#if 0
      addrFile = fopen( ADDrFILe, "r+" );
      fseek( addrFile, 0, SEEK_SET );
      fwrite( &iMonitorNum, sizeof(iMonitorNum), 1, addrFile );
      fwrite( astMonitorAddr, sizeof(astMonitorAddr), 1, addrFile );
      fclose( addrFile );
#endif
      fclose( connectLog );
      return 0;
   }
   if( strcmp( epczCommand, "logout" ) == 0 )
   {
      //addrFile = fopen( ADDrFILe, "r+" );\
      /* 检查是否已经有指定的地址和端口 */
      for( i = 0; i < MAX_MONITOR; i++ )
      {
         if( astMonitorAddr[i].m_Active == 0 ) /* 该位置可用 */
         {
            continue;
            /* break;*/
         }
         if( strcmp( epczIp, astMonitorAddr[i].m_AddrIp ) == 0 &&
             iPort == astMonitorAddr[i].m_Port )
         {
            astMonitorAddr[i].m_Active = 0 ;
            CLEARHANDLE( i );
            continue;
         }
      }
      //fclose( addrFile );
      i = j ;
      connectLog = fopen( WritLogFile, "a" );
      if( connectLog != NULL )
      {
         fprintf( connectLog, "%s in %d logout to %s : %d\n", __FILE__,
                  __LINE__,epczIp, iPort );
      }
      
      fclose(connectLog);
      return 0;
     
   }
   else if( strcmp( epczCommand, "check" ) == 0 )
   {
      int iFlag = 0;


      /** 检查如果有指定地址和端口, 返回 0 **/
      for( i = 0; i < MAX_MONITOR; i++ )
      {

         if( strcmp( epczIp, astMonitorAddr[i].m_AddrIp ) == 0 &&
             iPort == astMonitorAddr[i].m_Port &&
             astMonitorAddr[i].m_Active == 1 )
         {
            strcpy( astMonitorAddr[i].m_checktime, aczSystime );
            iFlag = 1;
         }
         else if(astMonitorAddr[i].m_Active == 1)
         {
             if( atoi(aczSystime)  - atoi( astMonitorAddr[i].m_checktime)  > 120 )
             {
                 astMonitorAddr[i].m_Active = 0;
             }
         }
      }
      /** 如果没有找到指定地址和端口, 返回失败 **/
      if( iFlag == 0)
         return -1;
      else 
         return 0;
   }
   else
   {
      /* 查找是否有指定的地址和端口 */
      for( i = 0; i < MAX_MONITOR; i++ )
      {
         if( strcmp( epczIp, astMonitorAddr[i].m_AddrIp ) == 0 &&
             iPort == astMonitorAddr[i].m_Port &&
             astMonitorAddr[i].m_Active == 1 )
         {
            tcp_close( astMonitorAddr[i].m_Server );
            /* 如果有相同地址和端口的记录 删除 */
            memset( &astMonitorAddr[i], 0, sizeof(MonitorAddr) );
            iMonitorNum--;
#if 0
            addrFile = fopen( ADDrFILe, "r+" );
            fseek( addrFile, 0, SEEK_SET );
            fwrite( &iMonitorNum, sizeof(iMonitorNum), 1, addrFile );
            fwrite( astMonitorAddr, sizeof(astMonitorAddr), 1, addrFile );
            fclose( addrFile );
#endif
            return 0;
         }
      }
      return -1;
   }
}

#if 0
      else if( strcmp( aczCommand, "report" ) == 0 )
      {
         printf( "enqueue message num %d\n", iEnqueueNum );
         printf( "dequeue message num %d\n", iDequeueNum );
         printf( "desert  message num %d\n", iDesertNum );
         {
            int i;
            CacheBlock *pstBlock;
            pstBlock = stFreeHead.m_Next;
            i = 0;
            while( pstBlock != NULL )
            {
               i++;
               pstBlock = pstBlock->m_Next;
            }
            printf( "free block cache %d\n", i );
            i = 0;
            while( pstBlock != NULL )
            {
               i++;
               pstBlock = pstBlock->m_Next;
            }
            printf( "message block cache %d\n", i );
         }
      }
#endif

void monitorMain( )
{
   int i, iRet;
   pthread_t tThread;
   pthread_attr_t stAttr;
   char aczSendBuf[1000];

   pthread_attr_init( &stAttr );
   fTraceLog = fopen( WritLogFile, "a" );
   fprintf( fTraceLog, "%d in %s enter monitorMain\n",
            __LINE__, __FILE__ );
   fclose(fTraceLog);
   /** 初始化接收线程 **/
   for( i = 0; i < iRecvMaxNum; i++ )
   {
      if( pthread_create( &tThread, &stAttr, handleRecv, &i ) != 0 )
      {
         printf( "create %d'th receive thread error %s\n", i, strerror(errno) );
         exit(0);
      }
      pthread_detach( tThread );
   }

   while( 1 )
   {
      char aczRecvBuff[200], aczCommand[100], aczIp[20], aczPort[10];
      char aczsystime[8];

      memset(aczsystime,0x00,sizeof(aczsystime));
      TOOL_GetSystemhhmmsstmp(aczsystime);
      fTraceLog = fopen( WritLogFile, "a" );
      fprintf( fTraceLog, "%d in %s enter tcp_accept begin time %s\n",
                  __LINE__, __FILE__ ,aczsystime);
      fflush(fTraceLog);
      fclose(fTraceLog);
      do
      {
         client = tcp_accept( iSendListenFd, -1 );
         if( client == -1 )
         {
            memset(aczsystime,0x00,sizeof(aczsystime));
            TOOL_GetSystemhhmmsstmp(aczsystime);

            fTraceLog = fopen( WritLogFile, "a" );
            fprintf( fTraceLog, "%d in %s enter tcp_accept return %d time %s\n",
                        __LINE__, __FILE__ ,client ,aczsystime);
            fclose(fTraceLog);

            sleep( 1 );
         }
      } while( client == -1 );

      memset( aczCommand, 0, sizeof(aczCommand) );
      memset( aczIp,      0, sizeof(aczIp) );
      memset( aczPort,    0, sizeof(aczPort) );

      memset( aczRecvBuff, 0, sizeof(aczRecvBuff) );
      if( tcp_wait_recv( client, 1000 ) == false )
      {
         fTraceLog = fopen( WritLogFile, "a" );
         fprintf( fTraceLog, "%d in %s enter11 tcp_wait_recv\n",
                     __LINE__, __FILE__ );
         fclose(fTraceLog);
         tcp_close( client );
         continue;
      }
      for(;;)
      {      		
         //if( tcp_raw_recv( client, aczRecvBuff, 200 ) == -1 )
         if( tcp_raw_recv( client, aczRecvBuff, 200 ) <=  0 )
         {
            tcp_close( client );
            gIsTimeout = 0;
            alarm(0);
            fTraceLog = fopen( WritLogFile, "a" );
            fprintf( fTraceLog, "%d in %s enter tcp_raw_recv \n",
                        __LINE__, __FILE__ );
            fclose(fTraceLog);
            
            break;
         }
         
         	
	alarm(1);
	signal(SIGALRM,DealTimeout);

	if (gIsTimeout)
	{
		gIsTimeout = 0;
		tcp_close( client );
		alarm(0);
		fTraceLog = fopen( WritLogFile, "a" );
           	fprintf( fTraceLog, "%d in %s tcp_raw_recv TimeOut\n",
                        __LINE__, __FILE__ );
            	fclose(fTraceLog);
	
		break;
	}
         

         sscanf( aczRecvBuff, "%s %s %s", aczCommand, aczIp, aczPort );
         if( strcmp( aczCommand, "login" ) == 0  ||
             strcmp( aczCommand, "check" ) == 0  ||
             strcmp( aczCommand, "logout" ) == 0 ||
             strcmp( aczCommand, "exit" ) == 0 )
         {
            pthread_mutex_lock( &mutexAddr );
            iRet = handleCommand( aczCommand, aczIp, aczPort );
            pthread_mutex_unlock( &mutexAddr );
            if( iRet == 0 )
            {
               strcpy( aczSendBuf, "OK" );
            }
            else
            {
               strcpy( aczSendBuf, "FAIL" );
            }

            if( tcp_wait_send( client, 10000 ) == true )
            {
               tcp_raw_send( client, aczSendBuf, strlen(aczSendBuf) );
            }
         }
         else if( strcmp( aczCommand, "nodes" ) == 0 )
         {
            readWatchNodes( client );
         }
         else if( strcmp( aczCommand, "trancode" ) == 0 )
         {
            int iNodeId = atoi(aczIp);
            readNodesTrans( client, iNodeId );
         }
         else if( strcmp( aczCommand, "field" ) == 0 )
         {
            int iNodeId = atoi(aczIp);
            readNodeFields( client, iNodeId );
         }
         else
         {
            sprintf( aczSendBuf, "Invalid command %s", aczCommand );
         }
      }
//      tcp_close( client );
   }
}

void loadEnv( char *moduleName )
{
   char aczCompleteFileName[128];
   char aczLine[1024], *p;
   FILE *fp;
   char aczHead[60];
   char aczEnv[100];

   strcpy (aczCompleteFileName, moduleName);
   fp = fopen( aczCompleteFileName, "r" );
   if( fp == NULL )
   {
      return;
   }

   /**
     * 下面处理环境变量
     */
   memset( aczLine, 0, sizeof(aczLine) );
   memset( aczHead, 0, sizeof(aczHead) );
   while( fgets( aczLine, sizeof(aczLine), fp ) != NULL )
   {
      PUB_StrTrim( aczLine, '\n' );
      PUB_StrTrim( aczLine, ' ' );
      if( aczLine[0] == '#' )
      {
         continue;
      }

      if( aczLine[0] == '[' )
      {
         if( (p = strchr( aczLine, ']' )) == NULL )
            continue;
         *p = 0;
         memset( aczHead, 0, sizeof(aczHead) );
         sprintf( aczHead, "%.*s", sizeof(aczHead)-1, aczLine+1 );
         PUB_StrTrim( aczHead, ' ' );
         continue;
      }

      if( (p = strchr( aczLine, '=' )) == NULL )
      {
         continue;
      }

      if( strlen( aczLine ) < 3 )
      {
         continue;
      }
#ifndef __hpux
      memset( aczEnv, 0, sizeof(aczEnv) );
      *p = 0;
      sprintf( aczEnv, "%s_%s", aczHead, aczLine );
      setenv( aczEnv, p+1, 1 );
#else
      p = (char *)malloc (strlen (aczLine)+ strlen(aczHead) + 2);
      strcpy( p, aczHead);
      strcat( p, "_");
      strcat (p, aczLine);

      if( putenv( p ) != 0 )
      {
         LOG4C(( LOG_WARN, "putenv: %s" , strerror(errno) ));
      }
      free( p );
#endif
   }

   fclose( fp );
}


int main( int argc, char * argv[] )
{
   char opt;
   int iRecvListenPort = -1, iSendListenPort = -1, iShmKey = -1;
   int iPid, iStatus, iSize;
   char *pczError = NULL;
   char FileName[300],aczLine[2000],*p1;
   char aczEnvFile[300];
   FILE * fp;
   pthread_t tThread;
   pthread_attr_t stAttr;

   pthread_attr_init( &stAttr );

   giConfigKey = 0;
   while( (opt=getopt( argc, argv, "r:n:s:m:hH" )) != -1 )
   {
      switch( opt )
      {
      case 'r':
         iRecvListenPort = atoi( optarg );
         break;
      case 'n':
         iRecvMaxNum = atoi( optarg );
         break;
      case 's':
         iSendListenPort = atoi( optarg );
         break;
      case 'm':
         giConfigKey = atoi( optarg );
         break;
      case 'h':
      case 'H':
      default:
         Usage( argv[0] );
         return 0;
      }
   }

   if( (iRecvMaxNum + 2) > MYTHREAD_THREADS_MAX )
   {
      printf( "指定的接收线程数 %d 超过最大线程数 %d\n"
              "请重新指定或找系统管理员调整进程允许的最大线程数\n",
              iRecvMaxNum, MYTHREAD_THREADS_MAX );
      return 0;
   }

   if( iRecvMaxNum == -1 || iRecvListenPort == -1 ||
       iSendListenPort == -1 || giConfigKey == 0 )
   {
      Usage( argv[0] );
      return 0;
   }

   /************ 取日志目录*************/
   memset( FileName,0x00,sizeof(FileName));
   sprintf(FileName,"%s/etc/%s.ini",getenv("MYROOT"),argv[0]);

   LOG4C_INIT( FileName );
   LOG4C_OPEN();
   LOG4C(( LOG_DEBUG, "monitor begin! " ));

   fp = fopen( FileName, "r" );
   if(fp == NULL)
   {
       printf("open file error [%s]\n",strerror(errno));
       exit(0);
   }

   memset( aczLine,0x00,sizeof(aczLine));
   while(fgets(aczLine,2000,fp) != NULL)
   {
     if(strstr(aczLine,"file=") != NULL)
     { break;
     }
     memset( aczLine,0x00,sizeof(aczLine));
   }
   fclose(fp);

   if(strlen(aczLine) < 10)
   {
       printf("ini file error\n",strerror(errno));
       exit(0);
   }

   memset( aczEnvFile, 0, sizeof(aczEnvFile) );
   sprintf( aczEnvFile, "%s/etc/monitor.env", getenv("MYROOT") );
   loadEnv( aczEnvFile );

   memset( WritLogFile,0x00,sizeof(WritLogFile));
   memset( ADDrFILe,0x00,sizeof(ADDrFILe));
   p1 = strstr(aczLine,"file=");

   if( aczLine[strlen(aczLine) -1 ] == '\n')
       aczLine[strlen(aczLine) -1 ] = 0x00;
   strcpy( WritLogFile,p1 + 5 );
   p1 = strchr( WritLogFile ,' ');
   if(p1 != NULL)
      p1[0] = 0x00;

   p1 = strstr(aczLine,"main.");
   p1[0] = 0x00;
   p1 = strstr(aczLine,"file=");
   strcpy(ADDrFILe ,p1 + 5 );
   strcat(ADDrFILe,"addr.log");
   printf("ini file [%s],addFile[%s]\n",WritLogFile,ADDrFILe);
#if 0
   addrFile = fopen( ADDrFILe, "w" );
   if( addrFile == NULL )
   {
      printf( "fopen( %s, w ) error %s %s %d\n",ADDrFILe, strerror(errno),__FILE__,__LINE__ );
      exit( 0 );
   }


   iMonitorNum = 0;
   fwrite( &iMonitorNum, sizeof(iMonitorNum), 1, addrFile );
   memset( astMonitorAddr, 0, sizeof(astMonitorAddr) );
   fwrite( astMonitorAddr, sizeof(astMonitorAddr), 1, addrFile );
   fclose( addrFile );
#endif
   iMonitorNum = 0;
   memset( astMonitorAddr, 0, sizeof(astMonitorAddr) );
   printf( "recv listen port %d\n", iRecvListenPort );
   printf( "send listen port %d\n", iSendListenPort );
   printf( "recv max thread  %d\n", iRecvMaxNum );

   signal( SIGINT,    SIG_IGN );
   signal( SIGHUP,    SIG_IGN );
   signal( SIGTERM,   SIG_IGN );
   signal( SIGQUIT,   SIG_IGN );
   signal( SIGTRAP,   SIG_IGN );
   signal( SIGABRT,   SIG_IGN );
   signal( SIGBUS,    SIG_IGN );
   signal( SIGSEGV,   SIG_IGN );
   signal( SIGSYS,    SIG_IGN );
   signal( SIGSTOP,   SIG_IGN );
   signal( SIGTSTP,   SIG_IGN );
   signal( SIGTTIN,   SIG_IGN );
   signal( SIGTTOU,   SIG_IGN );
   signal( SIGVTALRM, SIG_IGN );
   signal( SIGPROF,   SIG_IGN );
   signal( SIGALRM,   SIG_IGN );
   signal( SIGCLD,    SIG_IGN );

   /* signal( SIGUSR1, doExit ); */

   switch( fork() )
   {
   case -1:
      printf( "%s\n", strerror(errno) );
      break;
   case 0:
      break;
   default:
      return 0;
   }

   iRecvListenFd = tcp_bind( iRecvListenPort );
   if( iRecvListenFd == -1 )
   {
      printf( "bind port %d error %s\n", iRecvListenPort, strerror(errno) );
      exit( 0 );
   }

   iSendListenFd = tcp_bind( iSendListenPort );
   if( iSendListenFd == -1 )
   {
      printf( "bind port %d error %s\n", iSendListenPort, strerror(errno) );
      tcp_close( iRecvListenFd );
      return -1;
   }

   fTraceLog = fopen( WritLogFile, "a" );
   if( fTraceLog == NULL )
   {
      printf( "open tracelog error[%s]\n", strerror(errno) );
      tcp_close( iRecvListenFd );
      return -1;
   }
   fclose(fTraceLog);
   /* 产生一子进程和父进程 */
   do
   {
      switch( fork() )
      {
      case -1:
         printf( "%s\n", strerror(errno) );
         exit(0);
      case 0:
         goto CHILDACTION;
         break;
      default:  /* 父进程管理子进程, 防止子进程以外退出 */
         wait( &iStatus );
         printf( "exit status %d\n", WEXITSTATUS(iStatus) );fflush(stdout);
         if( WIFSIGNALED(iStatus) )
         {
            printf( "child signal (%d) exited\n", WSTOPSIG(iStatus) );
            fflush( stdout );
         }
      }
   } while( 1 );

CHILDACTION:
   if( DataBaseConnect() )
   {
      printf( "联接数据库失败\n" );
      exit(1);
   }
   initCache();

   monitorMain( );
   DisConnectDB();
   return 0;
}

void DealTimeout()
{
	tcp_close( client );
	fTraceLog = fopen( WritLogFile, "a" );
   	fprintf( fTraceLog, "%d in %s enter DealTimeout\n",  __LINE__, __FILE__ );
   	fclose(fTraceLog);
	signal(SIGALRM,SIG_IGN);
	gIsTimeout = 1;
	return;
}
