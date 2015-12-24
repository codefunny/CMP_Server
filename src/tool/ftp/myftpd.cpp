#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "easysock.h"
#include "myftp.h"

char *pczUpLoadDir = "/tmp/myftp/upload";
char *pczDownLoadDir = "/tmp/myftp/download";
char *pczUserListFile = "/tmp/myftp/userlist";

void StrRightTrim( char * epczStr, char ecCh )
{
   char * pcEndPos;

   if( strlen(epczStr) == 0 )
      return;

   for( pcEndPos = epczStr; *pcEndPos != '\0'; pcEndPos++);

   for( pcEndPos--; (pcEndPos >= epczStr) && *pcEndPos == ecCh;
         *(pcEndPos ) = '\0', pcEndPos-- );
}
void StrLeftTrim( char * epczStr, char ecCh )
{
   char *pczTemp, *pczBegin;

   pczBegin = epczStr;
   for( pczTemp = epczStr; *pczTemp == ecCh; pczTemp++ );
   while( *pczTemp )
   {
      *(pczBegin++) = *(pczTemp++);
   }
   if( pczBegin != epczStr )
   {
      *pczBegin = 0;
   }
}

void StrTrim( char * epczStr, char ecCh )
{
   StrLeftTrim( epczStr, ecCh );
   StrRightTrim( epczStr, ecCh );
}

void loadEnv( char *moduleName )
{
   char aczCompleteFileName[128];
   char aczLine[1024], *p;
   FILE *fp;

   /**
   sprintf( aczCompleteFileName, "%s/etc/%s.env",
            getenv( "MYROOT" ), moduleName );
   ***/
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
   while( fgets( aczLine, sizeof(aczLine), fp ) != NULL )
   {
      StrTrim( aczLine, '\n' );
      StrTrim( aczLine, ' ' );
      if( aczLine[0] == '#' )
      {
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
      *p = 0;
      if( setenv( aczLine, p+1, 1 ) != 0 )
      {
         fprintf( stderr, "setenv [%s] fail\n", aczLine );
      }
#else
     p = (char *)malloc (strlen (aczLine) + 1);
     strcpy (p, aczLine);

      if( putenv( p ) != 0 )
      {
         fprintf( stderr, "setenv [%s] fail\n", p );
      }

     free(p);/*zhd add 2010-09-15*/
#endif
   }

   fclose( fp );
}

void checkPath( char *pczUpLoadPath, char *pczDownLoadPath )
{
   struct stat stStat;
   if( lstat( pczUpLoadPath, &stStat ) == -1 )
   {
      printf( "上传文件目录状态不正常: %s\n", strerror(errno) );
      exit( 0 );
   }

   if( !S_ISDIR( stStat.st_mode ) )
   {
      printf( "%s 不是一个目录\n", pczUpLoadPath );
   }

   if( lstat( pczDownLoadPath, &stStat ) == -1 )
   {
      printf( "下传文件目录状态不正常: %s\n", strerror(errno) );
      exit( 0 );
   }

   if( !S_ISDIR( stStat.st_mode ) )
   {
      printf( "%s 不是一个目录\n", pczDownLoadPath );
   }
}

void checkUser( char *pczUserListFile )
{
   struct stat stStat;
   if( lstat( pczUserListFile, &stStat ) == -1 )
   {
      printf( "警告:操作员配置文件 %s 不存在\n", pczUserListFile );
      /*
      exit( 0 );
      */
   }
   if( stStat.st_size == 0 )
   {
      printf( "警告:操作员配置文件 %s 为空\n", pczUserListFile );
      /*
      exit( 0 );
      */
   }
}

/**
  * 从socket 读一个命令
  * 最长不超过5个字节,如果超过则命令出错,将所有的东西全部读出
  */
int readWord( int client, char *opczCmd, char *opczNext )
{
   char recycle[1024];
   int ret;

   memset( recycle, 0, sizeof(recycle) );
   ret = tcp_raw_recv( client, recycle, sizeof(recycle) );
   if( ret == -1 )  /*socket 出错*/
   {
#ifdef DEBUG
      printf( "%d in %s 接收命令出错 %s\n",
              __LINE__, __FILE__, strerror(errno) );
#endif
      exit( 0 );
   }

   for( int i = 0; i < ret; i++ )
   {
      if( recycle[i] == ' ' )
      {
         strncpy( opczCmd, recycle, i );
         opczCmd[i] = 0;
         strcpy( opczNext, &recycle[i+1] );
         return 0;
      }
   }
   return -1;
}

void recvToWrite( int client, FILE *fp, int fileSize )
{
   int iTotal = 0, iRead, iCanRead;
   char buf[2049], confirm[1];
   const int nMaxByte=256;

   confirm[0] = '1';

   do
   {
      if( tcp_wait_recv( client, 5000 ) )
      {
         iCanRead = (fileSize-iTotal)>nMaxByte? nMaxByte: fileSize-iTotal;
         iRead = tcp_raw_recv( client, buf, iCanRead );
         if( iRead == -1 )
         {
#ifdef DEBUG
            printf( "%d in %s read client buffer error: %s\n",
                    __LINE__, __FILE__, strerror(errno) );
#endif
            sprintf( buf, "%drecv error", RECV_ERR );
            tcp_raw_send( client, buf, strlen(buf) );
            return;
         }
         if( tcp_wait_send( client, 5000 ) == false )
         {
            printf( "send error timeout\n" );
            break;
         }
         if( tcp_raw_send( client, confirm, 1 ) == -1 )
         {
#ifdef DEBUG
            printf( "%d in %s write client confirm error: %s\n",
                    __LINE__, __FILE__, strerror(errno) );
#endif
            tcp_close( client );
            return;
         }
         iTotal += iRead;
#ifdef DEBUG
         printf( "%d in %s write %d charater to file\n",
                 __LINE__, __FILE__, iRead );
#endif
         fwrite( buf, iRead, 1, fp );
      }
      else
      {
#ifdef DEBUG
         printf( "%d in %s read client buffer timeout\n", __LINE__, __FILE__ );
#endif
         sprintf( buf, "%drecv timeout", TIMEOUT_ERR );
         tcp_raw_send( client, buf, strlen(buf) );
         return;
      }
   } while( iTotal < fileSize );

   tcp_raw_send( client, "200Finished", 11 );
}

void MkDir( const char *epczPath, const char *epczName )
{
    char aczBuf[300];
    char *p=NULL;
    char  aczTmp[300];
    char aczCmd[300];
    char  aczDir[300];
    DIR *dp=NULL;

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczDir, 0, sizeof(aczDir) );
    strcpy( aczBuf, epczName );
    strcpy( aczDir, epczPath );
    if( aczDir[strlen(aczDir) - 1] != '/' )
    {
        strcat( aczDir, "/" );
    }

    while( 1 )
    {
        p=strchr( aczBuf,  '/' );
        if( p != NULL )
        {
            memset( aczTmp, 0, sizeof(aczTmp) );
            memset( aczCmd, 0, sizeof(aczCmd) );
            memcpy( aczTmp, aczBuf, p-aczBuf );
            if( strlen(aczTmp) != 0 )
            {
                sprintf( aczDir, "%s%s/", aczDir, aczTmp );
                sprintf( aczCmd, "mkdir %s", aczDir );
                dp = opendir( aczDir );
                if( dp == NULL )
                {
                    system( aczCmd );
                }
            }

            strcpy( aczTmp, p+1 );
            strcpy( aczBuf, aczTmp );
        }
        else
            break;
    }
}

void recvFile( int client, char *pczNext, char *epczPath )
{
   char aczFileName[128], aczCmd[128], aczName[128];
   int  iLen, iFileSize;
   FILE *fp;

   strcpy( aczCmd, pczNext );

#ifdef DEBUG
   printf( "%d in %s next path %s command %s\n",
           __LINE__, __FILE__, epczPath, aczCmd );
#endif
   iLen = strlen( aczCmd );
   for( int i = 0; i < iLen; i++ )
   {
      if( aczCmd[i] == ' ' )
      {
         memset( aczName, 0, sizeof(aczName) );
         memcpy( aczName, aczCmd, i );
         MkDir( epczPath, aczName );
         sprintf( aczFileName, "%s/%s", epczPath, aczName );
         iFileSize = atoi( &aczCmd[i+1] );
#ifdef DEBUG
         printf( "%d in %s recv file %s size %d\n",
                 __LINE__, __FILE__, aczFileName, iFileSize );
#endif
         fp = fopen( aczFileName, "w" );
         if( fp == NULL )
         {
#ifdef DEBUG
            printf( "%d in %s open file %s error %s\n",
                    __LINE__, __FILE__, aczFileName, strerror(errno) );
#endif
            sprintf( aczCmd, "%dOpen file error: %s",
                     OPEN_WRITE_ERR, strerror(errno) );
            tcp_raw_send( client, aczCmd, strlen(aczCmd) );
            tcp_close( client ); 
            exit( 0 );
         }

         sprintf( aczCmd, "%dPrepare to receive", READY_RECV );
#ifdef DEBUG
         printf( "%d in %s write response %s\n", __LINE__, __FILE__, aczCmd );
#endif
         if( tcp_wait_send( client, 5000 ) == false )
         {
            printf( "send error timeout\n" );
            break;
         }
         if( tcp_raw_send( client, aczCmd, strlen(aczCmd) ) == -1 )
         {
#ifdef DEBUG
            printf( "写文件准备好响应失败: %s\n", strerror(errno) );
#endif
            exit( 0 );
         }
         recvToWrite( client, fp, iFileSize );
         tcp_close( client );
         fclose( fp );
         exit( 0 );
      }
   }

   sprintf( aczCmd, "%dInvalid command", INVALID_CMD );
   tcp_raw_send( client, aczCmd, strlen(aczCmd) );
   tcp_close( client );

   exit(0);
}

void delFile( int client, char *pczNext, char *epczPath )
{
   char aczFileName[256], aczCmd[1024], aczName[256];
   char aczSystem[1024];
   int  i=0, j=0;
   int  iLen=0;

   memset( aczCmd, 0, sizeof(aczCmd) );
   strcpy( aczCmd, pczNext );
   memset( aczFileName, 0, sizeof(aczFileName) );

#ifdef DEBUG
   printf( "%d in %s next path %s command %s\n",
           __LINE__, __FILE__, epczPath, aczCmd );
#endif
   iLen = strlen( aczCmd );
   for( i = 0; i < iLen; i++ )
   {
      if( aczCmd[i] == ' ' )
      {
          if( j==0 )
          {
              memset( aczFileName, 0, sizeof(aczFileName) );
              continue;
          }
          j=0;
          memset( aczName, 0, sizeof(aczName) );
          sprintf( aczName, "%s/%s", epczPath, aczFileName );

          memset( aczSystem, 0, sizeof(aczSystem) );
          sprintf( aczSystem, "rm -rf %s 1>/dev/null 2>&1", aczName );

#ifdef DEBUG
         printf( "%d in %s del file %s \n", __LINE__, __FILE__, aczName );
#endif
         system( aczSystem );

         memset( aczFileName, 0, sizeof(aczFileName) );
      }
      else
      {
          aczFileName[j++] = aczCmd[i];
      }
   }
   if( strlen( aczFileName ) != 0 )
   {
      memset( aczName, 0, sizeof(aczName) );
      sprintf( aczName, "%s/%s", epczPath, aczFileName );

      memset( aczSystem, 0, sizeof(aczSystem) );
      sprintf( aczSystem, "rm -rf %s 1>/dev/null 2>&1", aczName );

#ifdef DEBUG
      printf( "%d in %s del file %s \n", __LINE__, __FILE__, aczName );
#endif
      system( aczSystem );
   }

   sprintf( aczCmd, "ok" );
   tcp_raw_send( client, aczCmd, strlen(aczCmd) );
   tcp_close( client );

   exit(0);
}

void sendFromFile( int client, int localFile, int fileSize )
{
   char buf[2049], cFlag;
   int iTotalSend = 0, iRead, iCanRead, iSend;
   const int nMaxByte=256;

   if( fileSize == 0 )
   {
      return;
   }

   do
   {
      iCanRead = (fileSize-iTotalSend)>nMaxByte? nMaxByte: fileSize-iTotalSend;
      iRead = read( localFile, buf, iCanRead );
      if( iRead == -1 )
      {
#ifdef DEBUG
         printf( "%d in %s 读本地文件出错: %s\n",
                 __LINE__, __FILE__, strerror(errno) );
#endif
         tcp_close( client );
         exit( 0 );
      }
      if( tcp_wait_send( client, 5000 ) == false )
      {
         printf( "send error timeout\n" );
         break;
      }
      iSend = tcp_raw_send( client, buf, iRead );
#ifdef DEBUG
      printf( "%d in %s 已发送文件字节 %d\n", __LINE__, __FILE__, iSend );
#endif
      if( iSend == -1 )
      {
#ifdef DEBUG
         printf( "%d in %s 发送到客户端出错: %s\n",
                 __LINE__, __FILE__, strerror(errno) );
#endif
         tcp_close( client );
         exit( 0 );
      }
      if( !tcp_wait_recv( client, 5000 ) )
      {
#ifdef DEBUG
         printf( "%d in %s 等待客户端响应超时, 退出会话\n",
                 __LINE__, __FILE__ );
#endif
         tcp_close( client );
         exit( 0 );
      }
      if( tcp_raw_recv( client, &cFlag, 1 ) == -1 )
      {
#ifdef DEBUG
         printf( "%d in %s 读客户端响应出错: %s\n",
                 __LINE__, __FILE__, strerror(errno) );
#endif
         tcp_close( client );
         exit( 0 );
      }
      iTotalSend += iSend;
   } while( iTotalSend < fileSize );

   if( tcp_wait_recv( client, 5000 ) )
   {
      tcp_raw_recv( client, buf, sizeof(buf) );
   }
}

void sendFile( int client, char *pczNext, char *epczPath )
{
   char aczFileName[128], aczCmd[200], aczName[128];
   struct stat stStat;
   int ret, iLocalFile;

   strcpy( aczName, pczNext );

   sprintf( aczFileName, "%s/%s", epczPath, aczName );
   if( lstat( aczFileName, &stStat ) == -1 )
   {
      sprintf( aczCmd, "%dGet file %s size error: %s",
               RGET_FSIZE_ERR, aczFileName, strerror(errno) );
      tcp_raw_send( client, aczCmd, strlen(aczCmd) );
      tcp_close( client ); 
      exit( 0 );
   }

#ifdef DEBUG
   printf( "%d in %s get local file %s\n", __LINE__, __FILE__, aczFileName );
#endif
   iLocalFile = open( aczFileName, O_NONBLOCK );
   if( iLocalFile == -1 )
   {
#ifdef DEBUG
      printf( "%d in %s 打开文件 %s 失败: %s",
              __LINE__, __FILE__, aczFileName, strerror(errno) );
#endif
      sprintf( aczCmd, "%dOpen file %s error: %s",
               OPEN_READ_ERR, aczFileName, strerror(errno) );
      tcp_raw_send( client, aczCmd, strlen(aczCmd) );
      tcp_close( client ); 
      exit( 0 );
   }

   /*发送文件大小*/
   sprintf( aczCmd, "%d%d", FILE_SIZE, stStat.st_size );
#ifdef DEBUG
   printf( "%d in %s 发送命令 %s\n", __LINE__, __FILE__, aczCmd );
#endif
   if( tcp_wait_send( client, 5000 ) == false )
   {
      printf( "send error timeout\n" );
      exit( 0 );
   }
   if( tcp_raw_send( client, aczCmd, strlen(aczCmd) ) == -1 )
   {
#ifdef DEBUG
      printf( "%d in %s 发送文件大小失败\n", __LINE__, __FILE__ );
#endif
      exit( 0 );
   }

   /*接收准备好的指示*/
   if( !tcp_wait_recv( client, 5000 ) )
   {
#ifdef DEBUG
      printf( "%d in %s 读客户端准备好标志超时\n", __LINE__, __FILE__ );
#endif
      exit( 0 );
   }
   memset( aczCmd, 0, sizeof(aczCmd) );
   tcp_raw_recv( client, aczCmd, sizeof(aczCmd) );
   if( strncasecmp( aczCmd, "corr", 4 ) )
   {
#ifdef DEBUG
      printf( "%d in %s 客户端放弃", __LINE__, __FILE__ );
#endif
      exit( 0 );
   }

   sendFromFile( client, iLocalFile, stStat.st_size );

   close( iLocalFile );

   exit( 0 );
}

bool userLogin( char *pczOrgan, char *pczOperator, char *pczPass )
{
   return true;
}

void doRequest( int client, char *pczUpLoadPath, char *pczDownLoadPath,
                char *pczUserListFile )
{
   char cmd[100], aczOrgan[14], aczOperator[15], aczPass[20];
   char aczResponse[100], aczLine[200], buf[1024];
   int iRet;
   bool bLogin = false;

   bLogin=true; /*关闭登录认证*/

   memset( aczOrgan,    0, sizeof(aczOrgan) );
   memset( aczOperator, 0, sizeof(aczOperator) );
   memset( aczPass,     0, sizeof(aczPass) );

   do
   {
      if( tcp_wait_recv( client, 10000 ) )
      {
         memset( cmd, 0, sizeof(cmd) );
         iRet = readWord( client, cmd, buf );
         if( iRet == -1 )
         {
#ifdef DEBUG
            printf( "%d in %s 接收客户端命令错误\n", __LINE__, __FILE__ );
#endif
            strcpy( aczResponse, "200Recv timeout" );
         }
#ifdef DEBUG
         printf( "%d in %s 接收命令 %s\n", __LINE__, __FILE__, cmd );
#endif
         if( strcasecmp( cmd, "org" ) == 0 )
         {
            strcpy( aczOrgan, buf );
#ifdef DEBUG
            printf( "%d in %s 机构 %s\n", __LINE__, __FILE__, aczOrgan );
#endif
            sprintf( aczResponse, "%dPlease input user", READ_ORG_SUCC );
         }
         else if( strcasecmp( cmd, "user" ) == 0 )
         {
            strcpy( aczOperator, buf );
#ifdef DEBUG
            printf( "%d in %s 操作员 %s\n", __LINE__, __FILE__, aczOperator );
#endif
            sprintf( aczResponse, "%dPlease input pass", READ_OPER_SUCC );
         }
         else if( strcasecmp( cmd, "pass" ) == 0 )
         {
            strcpy( aczPass, buf );
#ifdef DEBUG
            printf( "%d in %s 操作员 %s\n", __LINE__, __FILE__, aczPass );
#endif
            if( userLogin( aczOrgan, aczOperator, aczPass ) == true )
            {
#ifdef DEBUG
               printf( "%d in %s 验证操作员 %s 成功\n",
                       __LINE__, __FILE__, aczOperator );
#endif
               bLogin = true;
               sprintf( aczResponse, "%dLogin correct", LOGIN_CORR );
            }
            else
            {
#ifdef DEBUG
               printf( "%d in %s 验证操作员 %s 失败\n",
                       __LINE__, __FILE__, aczOperator );
#endif
               sprintf( aczResponse, "%dLogin failed", LOGIN_ERR );
            }
         }
         else if( strcasecmp( cmd, "put" ) == 0 )
         {
            if( !bLogin )
            {
               sprintf( aczResponse, "%sNot login", NOT_LOGIN );
            }
            else
            {
               /*暂时只支持只收或者只发*/
               recvFile( client, buf, pczUpLoadPath );
               break;
            }
         }
         else if( strcasecmp( cmd, "get" ) == 0 )
         {
            if( !bLogin )
            {
               sprintf( aczResponse, "%sNot login", NOT_LOGIN );
            }
            else /*get 后面是 filename*/
            {
               /*暂时只支持只收或者只发*/
               sendFile( client, buf, pczDownLoadPath );
               break;
            }
         }
         else if( strcasecmp( cmd, "del" ) == 0 )
         {
            if( !bLogin )
            {
               sprintf( aczResponse, "%sNot login", NOT_LOGIN );
            }
            else
            {
               delFile( client, buf, pczUpLoadPath );
               break;
            }
         }
         else if( strcasecmp( cmd, "quit" ) == 0 )
         {
#ifdef DEBUG
            printf( "%d in %s 退出会话\n", __LINE__, __FILE__ );
#endif
            break;
         }
         else
         {
            sprintf( aczResponse, "%dInvalid command", INVALID_CMD );
         }
         if( tcp_wait_send( client, 5000 ) == false )
         {
            break;
         }
         if( tcp_raw_send( client, aczResponse, strlen(aczResponse) ) == -1 )
         {
            break;
         }
      }
      else
      {
#ifdef DEBUG
         printf( "%d in %s 接收客户端命令超时, 退出会话\n",
                 __LINE__, __FILE__ );
#endif
         break;
      }
   } while( true );

   tcp_close( client );

   exit( 0 );
}

int main( int argc, char *argv[] )
{
   char arg;
   bool bOptPort = false, bOptUser = false;
   int  iPort;
   char aczUpLoadPath[128];
   char aczDownLoadPath[128];
   char aczUserListFile[128];
   char  aczEnvConfig[256];
   char  aczLogConfig[256];

   if( getenv("MYROOT") == NULL)
   {
       fprintf( stderr, "Env MYROOT unset\n" );
       return -1;
   }
   memset( aczLogConfig, 0, sizeof(aczLogConfig) );
   memset( aczEnvConfig, 0, sizeof(aczEnvConfig) );
   sprintf( aczEnvConfig, "%s/etc/myftpd.env", getenv("MYROOT") );
   loadEnv( aczEnvConfig );
   sprintf( aczLogConfig, "%s/etc/myftpd.ini", getenv("MYROOT") );

   if( getenv( "SERVICE_PORT" ) == NULL )
   {
       fprintf( stderr, "Env SERVICE_PORT unset\n" );
       return -1;
   }
   else
   {
       iPort = atoi (getenv( "SERVICE_PORT" ));
   }

   if( getenv( "UPLOAD_PATH" ) == NULL )
   {
       strcpy( aczUpLoadPath, pczUpLoadDir );
   }
   else
   {
       strcpy( aczUpLoadPath, getenv("UPLOAD_PATH" ));
   }

   if( getenv( "DOWNLOAD_PATH" ) == NULL )
   {
       strcpy( aczDownLoadPath, pczDownLoadDir );
   }
   else
   {
       strcpy( aczDownLoadPath, getenv("DOWNLOAD_PATH" ));
   }

   if( getenv( "USERLIST" ) == NULL )
   {
       strcpy( aczUserListFile, pczUserListFile );
   }
   else
   {
       strcpy( aczUserListFile, getenv("USERLIST") );
   }

   int server = tcp_bind( iPort );
   if( server == -1 )
   {
      printf( "绑定端口失败: %s\n", strerror(errno) );
      exit( 0 );
   }

   checkPath( aczUpLoadPath, aczDownLoadPath );
   checkUser( aczUserListFile );

   signal( SIGCHLD, SIG_IGN );
   signal( SIGHUP, SIG_IGN );
   signal( SIGTERM, SIG_IGN );
   signal( SIGQUIT, SIG_IGN );

   if( fork() > 0 )
   {
      return 0;
   }

   do
   {
      int pid;
      int client = tcp_accept( server, -1 );
      if( client > 0 )
      {
         do
         {
            pid = fork();
         } while( pid == -1 );
         if( pid > 0 )
         {
            close( client );
         }
         else
         {
            close( server );
            doRequest( client, aczUpLoadPath, aczDownLoadPath,
                       aczUserListFile );
            exit( 0 );
         }
      }
   } while( true );

   return 0;
}
