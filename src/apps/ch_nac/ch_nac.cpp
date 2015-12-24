#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "log.h"
#include <netdb.h>
#include "pub_base.h"
#include "ap_base.h"
#include "apr_signal.h"
#include "tool_base.h"


void setupSignals( void )
{
   signal( SIGTERM, SIG_IGN );
   signal( SIGINT,  SIG_IGN );
   signal( SIGPIPE, SIG_IGN );
   signal( SIGHUP,  SIG_IGN );
   signal( SIGCHLD, SIG_IGN );
   signal( SIGTTOU, SIG_IGN );
   signal( SIGTTIN, SIG_IGN );
   signal( SIGTSTP, SIG_IGN );
}

static int _tcp_connect(char *host, int port)
{
   int sock, flags;
   struct sockaddr_in rsock;
   struct hostent * hostinfo;
   struct in_addr * addp;
   struct linger stLinger = { 1, 0 };

   memset ((char *)&rsock,0,sizeof(rsock));

   if ( (hostinfo=gethostbyname(host)) == NULL )
   {
      return -1;
   }

   sock=socket(AF_INET,SOCK_STREAM,0);
   if ( sock == -1 )
   {
      return -1;
   }

   addp=(struct in_addr *)*(hostinfo->h_addr_list);
   rsock.sin_addr=*addp;
   rsock.sin_family=AF_INET;
   rsock.sin_port=htons(port);

   if( connect(sock,(struct sockaddr *)(&rsock),sizeof(rsock)) == -1 )
   {
      close (sock);
      return -1;
   }

   flags = setsockopt( sock, SOL_SOCKET, SO_LINGER, &stLinger,
                       sizeof( struct linger ) );
   if( flags == -1 )
   {
      close (sock);
      return -1;
   }

   return sock;
}

static int _tcp_close( int sock )
{
   return close( sock );
}

static int _tcp_send( int sock, char *buf, int len )
{
   char sendbuf[1001];
   int pos = 0, sendlen;

   do
   {
      sendlen = len - pos;
      if( sendlen > 1000 )
      {
         sendlen = 1000;
      }

      memcpy( sendbuf, buf+pos, sendlen );
      if( write( sock, sendbuf, sendlen  ) == -1 )
      {
         return -1;
      }
      pos += sendlen;
   } while( pos < len );

   return pos;
}

static int _server(char * hostname, int port)
{
    int fdSock, optval;
    struct sockaddr_in srvaddr;

    bzero(&srvaddr, sizeof(struct sockaddr_in));

    if (hostname != NULL) {
        srvaddr.sin_addr.s_addr = inet_addr(hostname);
    } else {
        srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(port);

    fdSock = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSock <= 0) 
    {
        return ( -1);
    }

    optval = 1;
    if (setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(int)) < 0) {
        close(fdSock);
        return ( -1);
    }

    if (bind(fdSock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) < 0) {
        close(fdSock);
        return ( -1);
    }

    if (listen(fdSock, 64) < 0) {
        close(fdSock);
        return ( -1);
    }

    return (fdSock);
}

static int _tcp_read(int cd, char *buf, int len, int ntimeout )
{
    ssize_t ileft, iread;
    char *ptr;
    fd_set readset;
    struct timeval tvp ;
    int  maxfd=0;
    int  ret=0;

    ptr = buf;
    ileft = len;
    while (ileft > 0) 
    {
        tvp.tv_sec = ntimeout ;
        tvp.tv_usec = 0 ;
        maxfd = cd + 1 ;
        FD_ZERO( &readset );
        FD_SET( cd, &readset );

        if( ntimeout == 0 )/*������*/
        {
            ret = select(maxfd, &readset, NULL, NULL, NULL) ;
            if( ret <= 0 )
            {
                LOG4C(( LOG_DEBUG, "000000[%d]", ret));
                continue;
            }
        }
        else
        {
            ret = select(maxfd, &readset, NULL, NULL, &tvp) ;
            if( ret <= 0 )
            {
                LOG4C(( LOG_DEBUG, "Ҫ��ȡ�ĳ���[%d] ����ֵ[%d]  ��ʱ�˳�", ileft, ret));
                break;
            }
        }

        if(FD_ISSET(cd, &readset)) 
        {
            iread = read(cd, ptr, ileft);
            if (iread <= 0) 
            {   /* --> partner close socket */
                return (-__LINE__);
            } 
            else 
            {	    /* --> read OK */
                ileft -= iread;
                ptr += iread;
                continue;
            }
        }
    } /* end while */

    ileft = ileft<0? 0:ileft;

    return (len-ileft);
}


int ReqProc ( int enSock, char *epczBuf, int enLen )
{
    int iRet=0;

    iRet = _tcp_send( enSock, epczBuf, enLen );
    if( iRet == -1 )
    {
        LOG4C(( LOG_FATAL, "�������ݵ������ʧ��" ));
        return -1;
    }

    return 0;
}

int RspProc (int enSerSock, int sock )
{
    int iRet = 0;
    int iLen = 0;
    int iRecvLen=0;
    int iHaveLen=0;
    int i=0;
    char aczBuf[8000];
    char aczPack[1024];
    int iPackLen=0;
    unsigned char aczLen[10];
    unsigned char uCh=0;

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );
    iRet = _tcp_read (enSerSock, (char * )aczLen, 2, 70);
    if (iRet <=0 ) 
    {
        LOG4C((LOG_FATAL, "TCPIP���շ���Ӧ���ĳ���ʧ��.[%d]\n", iRet));
        return -1;
    }
    memcpy( aczBuf, aczLen, 2 );
    iLen = aczLen[0]*256+aczLen[1];
    LOG4C(( LOG_DEBUG, "���շ���Ӧ�𳤶� [%d] ���� [%02x %02x]\n", iLen, aczLen[0]&0xff, aczLen[1]&0xff ));
    if( iLen > sizeof(aczBuf)-3 )
    {
        LOG4C(( LOG_FATAL, "��Ч����" ));
        return -1;
    }

    iRet = _tcp_read (enSerSock, aczBuf+2, iLen, 5);
    if (iRet <=0 ) 
    {
        LOG4C((LOG_FATAL, "TCPIP���շ���Ӧ���İ���ʧ��.[%d]\n", iRet));
        return -1;
    }

    iRet = _tcp_send( sock, aczBuf, iLen+2 );
    if( iRet == -1 )
    {
        LOG4C(( LOG_FATAL, "�������ݵ�POSʧ��" ));
        return -1;
    }

    TOOL_Dump( LOG_DEBUG, "nac_send.dump", (unsigned char*)aczBuf, iLen+2 );

    return 0;
}

static jmp_buf env_alarm;
void AlarmFork( int enPara )
{
    siglongjmp( env_alarm, 1 );
}

int Process( int sockId, char *epczSerIp, int enSerPort )
{
    int iRet = 0;
    int iLen = 0;
    int iRecvLen=0;
    int iHaveLen=0;
    int i=0;
    char aczBuf[3000];
    unsigned char aczLen[10];
    char  aczKill[30];
    pid_t  pid;
    pid_t  pidchild;
    int  iSerSock;
    int  iTimeOut=0;
   
    if( getenv("TIMEOUT") != NULL )
    {
        iTimeOut = atoi( getenv("TIMEOUT") );
    }
    else
    {
        iTimeOut = 0;
    }

    while( 1 )
    {
        LOG4C_OPEN();
        memset( aczBuf, 0, sizeof(aczBuf) );
        memset( aczLen, 0, sizeof(aczLen) );

        /*
         *   nTimeOut=0������   
         *   �����趨������ʱ�䣺��
         */

        iRet = _tcp_read (sockId, (char *)aczLen, 2, iTimeOut );
        if (iRet <=0 ) 
        {
            LOG4C((LOG_FATAL, "TCPIP����POS�����ĳ���ʧ��.[%d] �˳�������\n", iRet));
            LOG4C_CLOSE();
            return -1;
        }
        memcpy( aczBuf, aczLen, 2 );
        iLen = aczLen[0]*256+aczLen[1];
        LOG4C(( LOG_DEBUG, "����POS���󳤶� [%d] ���� [%02x %02x]\n", iLen, aczLen[0]&0xff, aczLen[1]&0xff ));
        if( iLen > sizeof(aczBuf)-3 )
        {
            LOG4C(( LOG_FATAL, "��Ч����" ));
            LOG4C_CLOSE();
            return -1;
        }

        iRet = _tcp_read (sockId, aczBuf+2, iLen, 5);
        if (iRet <=0 ) 
        {
            LOG4C((LOG_FATAL, "TCPIP����POS�����İ���ʧ��.[%d]\n", iRet));
            LOG4C_CLOSE();
            return -1;
        }
        iLen += 2;

        TOOL_Dump( LOG_DEBUG, "nac_recv.dump", (unsigned char*)aczBuf, iLen );
        pid = fork();
        if( pid < 0 )
        {
            LOG4C_CLOSE();
            continue;
        }
        else if (pid == 0)
        {
            iSerSock = _tcp_connect( epczSerIp, enSerPort );
            if( iSerSock == -1 )
            {
                LOG4C(( LOG_FATAL, "��ǰ��[%s:%d]����ʧ��", epczSerIp, enSerPort  ));
                exit(0);
            }

            pidchild = fork();
            if ( pidchild  < 0) 
            {
                LOG4C ((LOG_FATAL, "Fork error." ));
                _tcp_close( iSerSock );
                exit (0);
            } 
            else if (pidchild == 0) 
            {
                RspProc( iSerSock, sockId );
                _tcp_close( iSerSock );
                exit(0);
            } 
            else 
            {
                signal( SIGALRM, AlarmFork );
                alarm( 60*10 ); //10���� �Ͽ��ͷ��������
                if ( sigsetjmp(env_alarm, 1) != 0 )
                {
                    LOG4C(( LOG_FATAL, "����[%d]��ʱδ�˳�,ִ��ǿ���˳�", pidchild ));
                    alarm( 0 );
                    signal( SIGALRM, SIG_IGN );
                    memset( aczKill, 0, sizeof(aczKill) );
                    sprintf(aczKill, "kill -9 %d", pidchild );
                    system( aczKill );
                    exit(0);
                }

                ReqProc( iSerSock, aczBuf, iLen );
                waitpid( pidchild, NULL, 0 );

                alarm( 0 );
                signal( SIGALRM, SIG_IGN );

                _tcp_close( iSerSock );
                exit(0);
            }
        }
        else
        {
            waitpid( pid, NULL, 0 );
        }
        LOG4C_CLOSE();
    }
    return 0;
}

int main( int argc, char *argv[] )
{
   int iPort;
   int iRet;
   int lscd, sockId;
   char aczServiceIp[30];
   int iServicePort;
   pid_t  pid;
   char  aczEnvConfig[256];
   char  aczLogConfig[256];

   if( getenv("MYROOT") == NULL)
   {
       fprintf( stderr, "Env MYROOT unset\n" );
       return -1;
   }
   memset( aczLogConfig, 0, sizeof(aczLogConfig) );
   memset( aczEnvConfig, 0, sizeof(aczEnvConfig) );
   sprintf( aczEnvConfig, "%s/etc/%s.env", getenv("MYROOT"), argv[0] );
   loadEnviorment( aczEnvConfig );
   sprintf( aczLogConfig, "%s/etc/%s.ini", getenv("MYROOT"), argv[0] );

   if( getenv( "SERVICE_PORT" ) == NULL )
   {
       fprintf( stderr, "Env SERVICE_PORT unset\n" );
       return -1;
   }
   else
   {
       iPort = atoi (getenv( "SERVICE_PORT" ));
   }

   if( getenv( "POSP_IP" ) == NULL )
   {
       fprintf( stderr, "Env POSP_IP unset\n" );
       return -1;
   }
   else
   {
       strcpy (aczServiceIp, getenv( "POSP_IP" ));
   }

   if( getenv( "POSP_PORT" ) == NULL )
   {
       fprintf( stderr, "Env POSP_PORT unset\n" );
       return -1;
   }
   else
   {
       iServicePort = atoi (getenv( "POSP_PORT" ));
   }

   if ((pid = fork()) < 0) 
   {
        fprintf (stderr, "fork error\n");
        return -1;
   }
   else if (pid > 0) 
   {
        exit (0);
   }

   if (setsid () < 0) 
   {
        return -1;
   }

   if ((pid = fork()) < 0) 
   {
        fprintf (stderr, "fork error\n");
        return -1;
   }
   else if (pid > 0) 
   {
        exit (0);
   }

   LOG4C_INIT( aczLogConfig );

   setupSignals();

   lscd = _server (NULL, iPort);

   while (1) 
   {
      sockId = accept (lscd, NULL, NULL) ;
      if (sockId < 0) 
      {
          continue;
      }

      LOG4C_OPEN();
      if ((pid = fork()) < 0) 
      {
          LOG4C ((LOG_FATAL, "Fork error: \n" ));
          close (sockId);
          LOG4C_CLOSE();
          continue;
      } 
      else if (pid == 0) 
      {
          close (lscd);
          Process( sockId , aczServiceIp, iServicePort );
          close( sockId );
          exit( 0 );
      }
      close (sockId);
      LOG4C_CLOSE();
   }

   return 0;
}

