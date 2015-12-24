/*
**    tool_base.c
**
**    ���ļ�������EasyLink�Ͳ㹤�ߺ�����
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/time.h>
#include <sys/sem.h>
#include <signal.h>
#include <stdarg.h>

#include "tool_base.h"
#include "cmp.h"
#include "log.h"


/*#define __TOOLDUMPDEBUG */
int iTOOL_ErrorNO = 0;

/*ȫ�ֱ�����ִ���ļ�����*/
char *pczExecFileName;
TOOL_LogInfo stToolLogInfo;

/*  ȡʱ�䵽�� hh:mm:ss        */
void TOOL_GetTime ( char *pszTime )
{
   struct timeval stTimeVal;
   struct timezone stTimeZone;
   struct tm *pstTime;

   memset( (char *)&stTimeVal, 0, sizeof( struct timeval ) );
   memset( (char *)&stTimeZone, 0, sizeof( struct timezone ) );

   gettimeofday( &stTimeVal, &stTimeZone );
   pstTime = localtime( &stTimeVal.tv_sec );

   if( pszTime != NULL )
   {
      sprintf( pszTime,"%02d:%02d:%02d", pstTime->tm_hour,
              pstTime->tm_min,  pstTime->tm_sec );
   }
   return ;
}

/*  ȡʱ�䵽���� hh:mm:ss:ms        */
void TOOL_GetMicroTime ( char *pszMicroTime )
{
   struct timeval stTimeVal;
   struct timezone stTimeZone;
   struct tm *pstTime;

   memset( (char *)&stTimeVal, 0, sizeof( struct timeval ) );
   memset( (char *)&stTimeZone, 0, sizeof( struct timezone ) );

   gettimeofday( &stTimeVal, &stTimeZone );
   pstTime = localtime( &stTimeVal.tv_sec );

   sprintf( pszMicroTime,"%02d:%02d:%02d:%03d", pstTime->tm_hour,
           pstTime->tm_min,  pstTime->tm_sec, stTimeVal.tv_usec/1000 );

   return ;
}


void TOOL_GetCurrTime( char *epcDateAndTime )
{
   time_t lCurTime;
   int  iCount = 0;

   assert( epcDateAndTime != NULL );

   time( &lCurTime );

   sprintf( epcDateAndTime, "%s", ctime( &lCurTime ) );

   iCount = strlen( epcDateAndTime );

   epcDateAndTime[ iCount-1 ] = ':';
   epcDateAndTime[ iCount ] = '\n';
   
   return ;
}

void TOOL_Trace(int eiTraceLevel, char *epczFileName, char *epcFormat, ... )
{
   FILE *fp;
   int iSize;
   char *pcTrace, *pcTraceLevel, *pcTracePath, aczFileName[256];
   char aczBakFileName[256], aczDateTime[128], aczTemp[64];

   va_list args;

   if( pczExecFileName == NULL )
   {
      pczExecFileName = "log";
   }
printf("pczExecFileName=[%s]\n", pczExecFileName);
   va_start( args, epcFormat );
   pcTrace = (char*)getenv( "TRACE" );
   if ( pcTrace != NULL )
   {
      if ( pcTrace[0] == '0' )
      {
         return;
      }
   }
   else
   {
      return;
   }
printf("pTrace[0]=%c\n",  pcTrace[0] );
   pcTraceLevel = (char*)getenv( "TRACE_LEVEL" );
   if ( pcTraceLevel != NULL )
   {
      if ( eiTraceLevel > atoi( pcTraceLevel ) )
      {
         return;
      }
   }
   else
   {
      return;
   }
printf("pcTraceLevel=%s\n",  pcTraceLevel );

   memset( aczFileName, 0, sizeof(aczFileName) );
   pcTracePath = (char*)getenv( "TRACE_PATH" );
   if ( pcTracePath != NULL )
   {
      if( *(pcTracePath + strlen( pcTracePath ) - 1 ) == '/' )
      {
         sprintf( aczFileName, "%s%s/%s",
                  pcTracePath, pczExecFileName, epczFileName );
      } 
      else
      {
         sprintf( aczFileName, "%s/%s/%s",
                  pcTracePath, pczExecFileName, epczFileName );
      }
   }
   else
   {
      pcTracePath = (char*)getenv( "MYROOT" );
      sprintf( aczFileName, "%s/log/%s/%s",
               pcTracePath, pczExecFileName, epczFileName );
   }
printf("aczFileName=%s\n",  aczFileName );

   fp = fopen( aczFileName, "a+" );
   if( fp == NULL )
   {
      return;
   }
   fseek( fp, 0, SEEK_END );
   iSize = ftell( fp );
   if( iSize > 100000000 )  /**the file size is no larger than 100M **/
   {
      fclose( fp );
      memset( aczBakFileName, 0, 256 );
      memset( aczTemp, 0, 64 );
      TOOL_GetTimeString( aczTemp );
      if( *(pcTracePath + strlen( pcTracePath ) - 1 ) == '/' )
      {
         sprintf( aczBakFileName, "%s%s/%s%s",
                  pcTracePath, pczExecFileName, epczFileName, aczTemp );
      }
      else
      {
         if( getenv( "TRACE_PATH" ) == NULL )
         {
            sprintf( aczBakFileName, "%s/log/%s/%s%s",
                     pcTracePath, pczExecFileName, epczFileName, aczTemp );
         }
         else
         {
            sprintf( aczBakFileName, "%s/%s/%s%s",
                     pcTracePath, pczExecFileName, epczFileName, aczTemp );
         }
      }

      if( rename( aczFileName, aczBakFileName ) < 0 )
      {
         fclose( fp );
         return;
      }
      fp = fopen( aczFileName, "a+" );
      if( fp == NULL )
      {
         return;
      }
   }

   memset( aczDateTime, 0, 128 );
   TOOL_GetCurrTime( aczDateTime );
   fprintf( fp, "%s", aczDateTime );
   (void)vfprintf( fp, epcFormat, args );
   fprintf( fp, "\n\n" );

   va_end( args );
   fclose( fp );
}

void TOOL_LogInit( char *epczSerial, char *epczBusiInfo )
{
   char *pczPath,
        *pczLevel,
        aczIndexName[128],
        aczBackFileName[128];
   FILE *fp;
   time_t tNow;
   struct stat stStat;
   struct tm   *pstTime;

/*   memset( &stToolLogInfo, 0x00, sizeof( stToolLogInfo ) );*/
   strcpy( stToolLogInfo.aczStanId, epczSerial );
   stToolLogInfo.iPid = getpid();

   pczPath = getenv( "TRACE_PATH" );
   if( pczPath == NULL )
   {
      LOG4C ((LOG_WARN, "TRACE_PATH not set\n"));
      stToolLogInfo.iCanWrite = 0;
      return;
   }

   pczLevel = getenv( "TRACE_LEVEL" );
   if( pczLevel == NULL )
   {
      stToolLogInfo.iLogLevel = 2;
   }
   else
   {
      stToolLogInfo.iLogLevel = atoi( pczLevel );
   }

   if( *(pczPath + strlen( pczPath ) - 1) == '/' )
   {
      sprintf( aczIndexName, "%s%s/index.log", pczPath, pczExecFileName );
      if (strlen (stToolLogInfo.aczFileName) == 0)
      {
         sprintf( stToolLogInfo.aczFileName, "%s%s/cmp_run.log",
                  pczPath, pczExecFileName );
      }
   }
   else
   {
      sprintf( aczIndexName, "%s/%s/index.log", pczPath, pczExecFileName );
      if (strlen (stToolLogInfo.aczFileName) == 0)
      {
          sprintf( stToolLogInfo.aczFileName, "%s/%s/cmp_run.log",
                   pczPath, pczExecFileName );
      }
   }

   /********
   stat( aczIndexName, &stStat );
   if( stStat.st_size > 262144 )
   {
      tNow = time( NULL );
      pstTime = localtime( &tNow );
      sprintf( aczBackFileName, "%s.%04d%02d%02d%02d%02d%02d",
               aczIndexName, pstTime->tm_year + 1900,
               pstTime->tm_mon + 1, pstTime->tm_mday,
               pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec );
      rename( aczIndexName, aczBackFileName );
   }
   fp = fopen( aczIndexName, "a+" );
   if( fp == NULL )
   {
LOG4C ((LOG_WARN, "Open Log Index File[%s] error:%s\n", aczIndexName, strerror(errno)));
      stToolLogInfo.iCanWrite = 0;
      return;
   }

   fprintf( fp, "[%d:%s] ҵ������: [%s]\n",
            stToolLogInfo.iPid, epczSerial, epczBusiInfo );
   fclose( fp );
   *********/

   fp = fopen( stToolLogInfo.aczFileName, "a+" );
   if( fp == NULL )
   {
LOG4C ((LOG_WARN, "Open Log File[%s] error:%s\n", stToolLogInfo.aczFileName, strerror(errno)));
      stToolLogInfo.iCanWrite = 0;
      return;
   }
   
   stToolLogInfo.iCanWrite = 1;
   fclose( fp );

   return;
}

/*
** ����:����ָ���ļ������־,��TOOL_Trace�Ĳ�����ڲ���ÿ�ζ�ȡ��������
**      Ҳ����ָ���ļ���,���е���־����һ���ļ�.ÿһ�ж��Ե�ǰ������̵�
**      PID��ʼ,Ϊ��־ͳ������׼��
*/
/***
void TOOL_Log(int eiTraceLevel, char *epcFormat, ... )
{
   FILE *fp;
   char aczBakFileName[256], aczDateTime[128];
   time_t tNow;
   struct stat stStat;
   struct tm   *pstTime;
   extern TOOL_LogInfo stToolLogInfo;
   va_list args;

   if( stToolLogInfo.iCanWrite == 0 || eiTraceLevel > stToolLogInfo.iLogLevel )
   {
      return;
   }

   va_start( args, epcFormat );

   stat( stToolLogInfo.aczFileName, &stStat );
   if( stStat.st_size > 262144 )
   {
      time( &tNow );
      pstTime = localtime( &tNow );
      sprintf( aczBakFileName, "%s.%04d%02d%02d%02d%02d%02d.bak",
               stToolLogInfo.aczFileName, pstTime->tm_year + 1900,
               pstTime->tm_mon + 1, pstTime->tm_mday,
               pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec );
      rename( stToolLogInfo.aczFileName, aczBakFileName );
   }

   fp = fopen( stToolLogInfo.aczFileName, "a+" );
   if( fp == NULL )
   {
      return;
   }

   memset( aczDateTime, 0, 128 );
   TOOL_GetCurrTime( aczDateTime );
   fprintf( fp, "[%d:%s] %s", stToolLogInfo.iPid, stToolLogInfo.aczStanId,
            aczDateTime );
   fprintf( fp, "[%d:%s] ", stToolLogInfo.iPid, stToolLogInfo.aczStanId );
   (void)vfprintf( fp, epcFormat, args );
   fprintf( fp, "\n\n" );

   va_end( args );
   fclose( fp );
}
***/

void TOOL_Dump( int eiTraceLevel,
                char *epcFileName, 
                unsigned char *epbyDumpedData,
                int eiDumpedDataSize )
{
   int iFileDes; 
   int iRename = 0, iLen, iNameLen; 
   int usDataLen = 0;
   off_t lFileSize=0;
   char acDateAndTime[ TOOL_DATE_AND_TIME_LEN ];
   char acTitleBuffer[ TOOL_LINE_LENGTH ];
   char acBakFileName[ TOOL_FILE_NAME_LEN ];
   char acFileName[ TOOL_FILE_NAME_LEN ];
   char acTmp[ TOOL_FILE_NAME_LEN ];
   char *pcTraceFilePath;
   char *pcTraceOn, *pcTraceLevel;
   char aczLogFile[200];
   unsigned char abyBuffer[ TOOL_MAX_MSG_LEN ];
   FILE *fp;

   assert( epcFileName    != NULL );
   assert( epbyDumpedData != NULL );

   /*
   **  �ж��Ƿ����û�������"TRACE"
   */
   pcTraceOn = getenv( "TRACE" );
   
   if ( pcTraceOn != NULL )
   {
      if ( pcTraceOn[0] == '0' )
      {
         goto EXIT;
      }
   }
   else
   {
      goto EXIT;
   }

   /*
   **  �жϻ�������"TRACE_LEVEL"
   */
   pcTraceLevel = getenv( "TRACE_LEVEL" );

   if ( pcTraceLevel != NULL )
   {
      if ( eiTraceLevel > atoi(pcTraceLevel) )
      {
         goto EXIT;
      }
   }
   else
   {
      goto EXIT;
   }

   memset( abyBuffer,     0, TOOL_MAX_MSG_LEN       ); 
   memset( aczLogFile,     0, sizeof(aczLogFile)       ); 
   memset( acDateAndTime, 0, TOOL_DATE_AND_TIME_LEN ); 
   memset( acTitleBuffer, 0, TOOL_LINE_LENGTH       ); 
   memset( acBakFileName, 0, TOOL_FILE_NAME_LEN     );
   memset( acFileName,    0, TOOL_FILE_NAME_LEN     );
   memset( acTmp,         0, TOOL_FILE_NAME_LEN     );
   sprintf(aczLogFile,"%s/log/dump.log",getenv("MYROOT"));

#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)

   fprintf(fp ,"%d in %s , ��ʼ��ӡdump\n",__LINE__,__FILE__,"");
   fflush(fp);
   fclose(fp);
#endif
   /*
   **  �ж��Ƿ����û�������"TRACE_PATH"
   */
   pcTraceFilePath = NULL;
   pcTraceFilePath = getenv( "TRACE_PATH" );

#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)

   fprintf(fp ,"%d in %s , pcTraceFilePath [%s]\n",__LINE__,__FILE__,pcTraceFilePath);
   fflush(fp);
   fclose(fp);
#endif
   if ( pcTraceFilePath != NULL )
   {
      iLen = strlen( pcTraceFilePath );
      iNameLen = strlen( epcFileName );
#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)

   fprintf(fp ,"%d in %s ,epcFileName[%s]\n",__LINE__,__FILE__,epcFileName);
   fprintf(fp ,"%d in %s ,pczExecFileName[%s]\n",__LINE__,__FILE__,pczExecFileName);
   fflush(fp);
   fclose(fp);
#endif

      if( pcTraceFilePath[iLen-1] == '/' )
      {
      /*   sprintf( acFileName, "%s%s/%s", pcTraceFilePath, pczExecFileName,
                  epcFileName );*/

         sprintf( acFileName, "%s%s", pcTraceFilePath, 
                  epcFileName );
#if 0
	  
        iLen = strlen( pczExecFileName );
        if (iLen > 0)
        {
             if( pczExecFileName[iLen-1] == '/' )
             {
                /* modify in 20070508 */
                /*sprintf( acFileName, "%s%s%s", pcTraceFilePath, pczExecFileName,
                         epcFileName ); */
                sprintf( acFileName, "%s%s%s", pcTraceFilePath, pczExecFileName,
                         epcFileName ); 
             }
             else
             {
                sprintf( acFileName, "%s%s/%s", pcTraceFilePath, pczExecFileName,
                         epcFileName );
             }
        }
        else
        {

                sprintf( acFileName, "%s%s", pcTraceFilePath, epcFileName );
        }
#endif 

      }
      else
      {
         /*sprintf( acFileName, "%s/%s/%s", pcTraceFilePath, pczExecFileName,
                  epcFileName );*/
         sprintf( acFileName, "%s/%s", pcTraceFilePath, 
                  epcFileName );
#if 0
      iLen = strlen( pczExecFileName );
	  if (iLen > 0)
	  {
          if( pczExecFileName[iLen-1] == '/' && iLen > 1 )
          {
             sprintf( acFileName, "%s/%s%s", pcTraceFilePath, pczExecFileName,
                      epcFileName );
          }
          else
          {
             sprintf( acFileName, "%s/%s/%s", pcTraceFilePath, pczExecFileName,
                      epcFileName );
          }
	  }
	  else
	  {

             sprintf( acFileName, "%s/%s", pcTraceFilePath, epcFileName );
	  }
#endif


      }
   }
   else
   {
             sprintf( acFileName, "%s/%s", getenv("MYROOT"), epcFileName );
   }

#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)

   fprintf(fp ,"%d in %s ,acFileName[%s]\n",__LINE__,__FILE__,acFileName);
   fflush(fp);
   fclose(fp);
#endif

   if ( (iFileDes = open( acFileName, O_WRONLY|O_CREAT,0777 ) )==-1 ) 
   {
      iTOOL_ErrorNO = TOOL_FILE_OPEN_ERROR;

      goto EXIT;
   }

   /*
   **  ������־�ļ��ĳ��ȣ��糬��TOOL_FILE_SIZE��������һ�������ļ�.
   */
   if ( (lFileSize=lseek(iFileDes,0,SEEK_END)) == -1 )
   {
      iTOOL_ErrorNO = TOOL_FILE_SEEK_ERROR;

      close( iFileDes );

      goto EXIT;
   }

   if ( lFileSize >= TOOL_FILE_SIZE )
   {
      if ( pcTraceFilePath != NULL )
      {
         memcpy( acBakFileName, pcTraceFilePath, strlen(pcTraceFilePath) );
         if( pcTraceFilePath[iLen-1] != '/' )
         {
         acBakFileName[strlen(pcTraceFilePath)] = '/';
         }
      }
      strcat( acBakFileName, epcFileName );
      TOOL_GetTimeString( acTmp );
      strcat( acBakFileName, acTmp );
 
      write( iFileDes, acBakFileName, strlen(acBakFileName) );
      if ( (iRename = rename( acFileName, acBakFileName ) )<0 )
      {
         iTOOL_ErrorNO = TOOL_RENAME_ERROR;

         close( iFileDes );

         goto EXIT;
      }
   }

   TOOL_GetCurrTime( acDateAndTime );
   write( iFileDes, acDateAndTime, strlen(acDateAndTime) );
#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)
   
   fprintf(fp ,"%d in %s ,acDateAndTime[%s]\n",__LINE__,__FILE__,acDateAndTime);
   fflush(fp);
   fclose(fp);
#endif

   /*
   **  Making a title to show data.
   */
   memcpy( acTitleBuffer,"-1--2--3--4--5--6--7--8-Hex-2--3--4--5--6--7--8",48 );

   write( iFileDes, acTitleBuffer, strlen(acTitleBuffer) );

   memset( acTitleBuffer, '\0', TOOL_LINE_LENGTH );
   memcpy( acTitleBuffer, "  ---------ASCII CODE---------", 30 );

   write( iFileDes, acTitleBuffer, strlen(acTitleBuffer) );
   write( iFileDes, "\n", 1 );


   if ( eiDumpedDataSize > TOOL_MAX_MSG_LEN )
   {
      usDataLen = TOOL_MAX_MSG_LEN;
   }
   else 
   {
      usDataLen = eiDumpedDataSize;
   } 
   memcpy( abyBuffer, epbyDumpedData, usDataLen ); 

   TOOL_BinaryDump( iFileDes, ( unsigned char * )abyBuffer, usDataLen );

   write( iFileDes, " \n\n", 3 );

   close( iFileDes );

EXIT :

#ifdef __TOOLDUMPDEBUG
   fp = fopen(aczLogFile,"a");
   if (fp != NULL)
   
   fprintf(fp ,"%d in %s , game over \n",__LINE__,__FILE__);
   fflush(fp);
   fclose(fp);
#endif


   return;
}





void TOOL_BinaryDump( int eiFileDes,
                      unsigned char *epbyMessage,
                      int eiLength )
{
   int i,j,k,iCount = 0;
   unsigned char abyTmp[ TOOL_LINE_LENGTH ];
   unsigned char abyCharBuf[TOOL_LINE_LENGTH];
   unsigned char *pbyPtr;

   assert( epbyMessage != NULL );

   memset( abyTmp,     '\0', TOOL_LINE_LENGTH );
   memset( abyCharBuf, '\0', TOOL_LINE_LENGTH );
   pbyPtr = epbyMessage;

   while( pbyPtr<(epbyMessage+eiLength) )
   {
      for ( j=0; j<=(int)(eiLength-1)/16; j++ )
      {
         abyTmp[0] = '\0';
         abyCharBuf[0] = '\0';

         for ( i=0; (i<16)&&(pbyPtr<(epbyMessage+eiLength)); i++ )
         {
	    sprintf( ( char * )abyTmp,"%s%.2x ", abyTmp, *pbyPtr );

            if ( ((*pbyPtr)<=126)&&((*pbyPtr)>=0) )
            {
               if ( ( *pbyPtr<=31 ) && ( *pbyPtr>=0 ) )
               {
                  /*
                  **  Replace special char with '.'
                  */
                  sprintf( ( char * )abyCharBuf, "%s.", abyCharBuf );
               }
               else
               {
                  sprintf( ( char * )abyCharBuf, "%s%c", abyCharBuf, *pbyPtr );
               }
            }
            else 
            {
               sprintf( ( char * )abyCharBuf, "%s*", abyCharBuf );
            }
	    pbyPtr += 1;
	 }
         write( eiFileDes, abyTmp, strlen( ( char * )abyTmp ) );
         iCount = strlen( ( char * )abyTmp );

         if ( iCount < 48 )
         {
             for( k=0;k<(48-iCount);k++ )
             { 
                write( eiFileDes, " ", 1  );
             }
         }                 
         write( eiFileDes, "      ", 6 );
         write( eiFileDes, abyCharBuf, strlen( ( char * )abyCharBuf ) );
         write( eiFileDes, "\n", 1 );
      } 
   }
}




void TOOL_GetTimeString( char *epcTime )
{
   time_t lTime;
   struct tm *tp;
   char pcGetTime[20];

   assert( epcTime != NULL );

   memset( pcGetTime, '\0', 20 );
   time( &lTime );
   tp=localtime( &lTime );
    
   sprintf( epcTime, "%d", tp->tm_year+1900);
   sprintf( pcGetTime, "%02d", tp->tm_mon+1 );
   strcat( epcTime, pcGetTime );

   sprintf( pcGetTime, "%d",tp->tm_mday );
   strcat( epcTime, pcGetTime );

   sprintf( pcGetTime, "%d",tp->tm_hour );
   strcat( epcTime, pcGetTime );

   sprintf (pcGetTime, "%d", tp->tm_min );
   strcat( epcTime, pcGetTime );

   sprintf( pcGetTime, "%d", tp->tm_sec );
   strcat( epcTime, pcGetTime );
   strcat( epcTime, ".bak" );
}





void TOOL_Message( int eiTraceLevel, char *epcSender, char *epcMessage )
{
   char *pcTraceOn, *pcTraceLevel;

   assert( epcSender  != NULL );
   assert( epcMessage != NULL );

   /*
   **  �ж��Ƿ����û�������"TRACE"
   */
   pcTraceOn = getenv( "TRACE" );

   if ( pcTraceOn != NULL )
   {
      if ( pcTraceOn[0] == '0' )
      {
         goto EXIT;
      }
   }
   else
   {
      goto EXIT;
   }


   /*
   **  �жϻ�������"TRACE_LEVEL"
   */
   pcTraceLevel = getenv( "TRACE_LEVEL" );
   if ( pcTraceLevel != NULL )
   {
      if ( eiTraceLevel > atoi( pcTraceLevel ) )
      {
         goto EXIT;
      }
   }
   else
   {
      goto EXIT;
   }

/*
   openlog( epcSender, LOG_CONS|LOG_PID|LOG_NDELAY|LOG_NOWAIT, LOG_USER );

   syslog( LOG_DEBUG, epcMessage );

   closelog();
*/

EXIT:
   return;
}






void TOOL_Debug( char *epcFileName, char *epcMessage, ... )
{
#ifdef DEBUG

   int   iFileDes, iLen, iNameLen, iCount=0, iRename;
   off_t lFileSize = 0;

   char acBuffer[ TOOL_COMMON_MSG_LEN ];
   char acDateAndTime[ TOOL_DATE_AND_TIME_LEN ];
   char acBakFileName[ TOOL_FILE_NAME_LEN ] ;
   char acFileName[ TOOL_FILE_NAME_LEN ];
   char acTmp[ TOOL_FILE_NAME_LEN ];
   char *pcTraceFilePath;
   char *pcTraceOn;

   va_list ArgList;

   assert( epcFileName  != NULL );
   assert( epcMessage   != NULL );

   memset( acBuffer,      0, TOOL_COMMON_MSG_LEN    );
   memset( acDateAndTime, 0, TOOL_DATE_AND_TIME_LEN );
   memset( acBakFileName, 0, TOOL_FILE_NAME_LEN     );
   memset( acFileName,    0, TOOL_FILE_NAME_LEN     );
   memset( acTmp,         0, TOOL_FILE_NAME_LEN     );


   /*
   **  �ж��Ƿ����û�������"TRACE_PATH"
   */
   pcTraceFilePath = NULL;
   pcTraceFilePath = getenv( "TRACE_PATH" );
   if ( pcTraceFilePath != NULL )
   {
      iLen = strlen( pcTraceFilePath );
      iNameLen = strlen( epcFileName );
      memcpy( acFileName, pcTraceFilePath, iLen );
      memcpy( acFileName+iLen, epcFileName, iNameLen );
   }
   else
   {
      strcpy( acFileName, epcFileName );
   }

   if ( ( iFileDes = open( acFileName, O_WRONLY|O_CREAT,0777 ) ) == -1 ) 
   {
      iTOOL_ErrorNO = TOOL_FILE_OPEN_ERROR;

      goto EXIT;
   }

   /*
   **  ������־�ļ��ĳ��ȣ��糬��TOOL_FILE_SIZE��������һ�������ļ�.
   */
   if ( ( lFileSize = lseek( iFileDes,0,SEEK_END ) ) == -1 )
   {
      iTOOL_ErrorNO = TOOL_FILE_SEEK_ERROR;

      close( iFileDes );

      goto EXIT;
   }

   if ( lFileSize >= TOOL_FILE_SIZE )
   {
      if ( pcTraceFilePath != NULL )
      {
         memcpy( acBakFileName, pcTraceFilePath, iLen );
      }
      strcat( acBakFileName, epcFileName );
      TOOL_GetTimeString( acTmp );
      strcat( acBakFileName, acTmp );
	
      if ( (iRename = rename( acFileName, acBakFileName ) )<0 )
      {
         iTOOL_ErrorNO = TOOL_RENAME_ERROR;

         close( iFileDes );

         goto EXIT;
      }
   } 

   va_start( ArgList, epcMessage );

   vsprintf( (char *)acBuffer, epcMessage, ArgList );
   va_end( ArgList );

   iCount = strlen( (char *)acBuffer );

   if ( iCount >= TOOL_COMMON_MSG_LEN )
   {
       acBuffer[TOOL_COMMON_MSG_LEN-1] = '\0';
   }

   TOOL_GetCurrTime( acDateAndTime );

   write( iFileDes, acDateAndTime, strlen(acDateAndTime) );
   write( iFileDes, acBuffer, strlen(acBuffer) );
   write( iFileDes, "\n\n", 2 );
        
   close( iFileDes );

EXIT :

   return;

#endif
}






void TOOL_DebugDump( char *epcFileName, 
                    unsigned char *epbyDumpedData,
                    int eiDumpedDataSize )
{
#ifdef DEBUG

   int iFileDes; 
   int iRename = 0, iLen, iNameLen; 
   int usDataLen = 0;
   off_t lFileSize=0;
   char acDateAndTime[ TOOL_DATE_AND_TIME_LEN ];
   char acTitleBuffer[ TOOL_LINE_LENGTH ];
   char acBakFileName[ TOOL_FILE_NAME_LEN ];
   char acFileName[ TOOL_FILE_NAME_LEN ];
   char acTmp[ TOOL_FILE_NAME_LEN ];
   char *pcTraceFilePath;
   char *pcTraceOn;
   unsigned char abyBuffer[ TOOL_MAX_MSG_LEN ];

   assert( epcFileName    != NULL );
   assert( epbyDumpedData != NULL );

   memset( abyBuffer,     0, TOOL_MAX_MSG_LEN       ); 
   memset( acDateAndTime, 0, TOOL_DATE_AND_TIME_LEN ); 
   memset( acTitleBuffer, 0, TOOL_LINE_LENGTH       ); 
   memset( acBakFileName, 0, TOOL_FILE_NAME_LEN     );
   memset( acFileName,    0, TOOL_FILE_NAME_LEN     );
   memset( acTmp,         0, TOOL_FILE_NAME_LEN     );

   /*
   **  �ж��Ƿ����û�������"TRACE_PATH"
   */
   pcTraceFilePath = NULL;
   pcTraceFilePath = getenv( "TRACE_PATH" );
   if ( pcTraceFilePath != NULL )
   {
      iLen = strlen( pcTraceFilePath );
      iNameLen = strlen( epcFileName );
      memcpy( acFileName, pcTraceFilePath, iLen );
      memcpy( acFileName+iLen, epcFileName, iNameLen );
   }
   else
   {
      strcpy( acFileName, epcFileName );
   }

   if ( (iFileDes = open( acFileName, O_WRONLY|O_CREAT,0777 ) )==-1 ) 
   {
      iTOOL_ErrorNO = TOOL_FILE_OPEN_ERROR;

      goto EXIT;
   }

   /*
   **  ������־�ļ��ĳ��ȣ��糬��TOOL_FILE_SIZE��������һ�������ļ�.
   */
   if ( (lFileSize=lseek(iFileDes,0,SEEK_END)) == -1 )
   {
      iTOOL_ErrorNO = TOOL_FILE_SEEK_ERROR;

      close( iFileDes );

      goto EXIT;
   }

   if ( lFileSize >= TOOL_FILE_SIZE )
   {
      if ( pcTraceFilePath != NULL )
      {
         memcpy( acBakFileName, pcTraceFilePath, strlen(pcTraceFilePath) );
      }
      strcat( acBakFileName, epcFileName );
      TOOL_GetTimeString( acTmp );
      strcat( acBakFileName, acTmp );
 
      if ( (iRename = rename( acFileName, acBakFileName ) )<0 )
      {
         iTOOL_ErrorNO = TOOL_RENAME_ERROR;

         close( iFileDes );

         goto EXIT;
      }
   }

   TOOL_GetCurrTime( acDateAndTime );
   write( iFileDes, acDateAndTime, strlen(acDateAndTime) );

   /*
   **  Making a title to show data.
   */
   memcpy( acTitleBuffer,"-1--2--3--4--5--6--7--8-Hex-2--3--4--5--6--7--8",48 );

   write( iFileDes, acTitleBuffer, strlen(acTitleBuffer) );

   memset( acTitleBuffer, '\0', TOOL_LINE_LENGTH );
   memcpy( acTitleBuffer, "  ---------ASCII CODE---------", 30 );

   write( iFileDes, acTitleBuffer, strlen(acTitleBuffer) );
   write( iFileDes, "\n", 1 );


   if ( eiDumpedDataSize > TOOL_MAX_MSG_LEN )
   {
      usDataLen = TOOL_MAX_MSG_LEN;
   }
   else 
   {
      usDataLen = eiDumpedDataSize;
   } 
   memcpy( abyBuffer, epbyDumpedData, usDataLen ); 

   TOOL_BinaryDump( iFileDes, ( unsigned char * )abyBuffer, usDataLen );

   write( iFileDes, " \n\n", 3 );

   close( iFileDes );

EXIT :
   
   return;

#endif

}



/*  ��ȥ�ַ���ǰ��ո�        */
char *TOOL_StripSpace( pszStr )
 char *pszStr;
{
   char *pszPos;
   char *pszEnd;
   char *pszHdStr;
   char *pszWord;
           
   int iLen;
 
   pszHdStr = pszStr;
   pszWord = pszStr;
   pszPos = (char *)NULL;
   while( *pszStr )
   {
      if( *pszStr != ' ' )
      {
         pszPos = pszStr;
         break;
      }
      pszStr++;
   }

   if( pszPos == (char *)NULL )
   {
      *pszHdStr = 0;
      return pszHdStr;
    }

    iLen = strlen(pszPos); 
    pszEnd = pszPos+(long)iLen-1;
    while( *pszEnd )
    {
    if( *pszEnd == ' ' )
       {
          *pszEnd = '\0';
          pszEnd--;
        }
        else 
        {
           break;
         }
     }
  
    while( *pszHdStr = *pszPos++ )
   {
      pszHdStr++;
   }

   return pszWord;
}


/* ��������Ϣ���С��źŵơ������ڴ����õ� KEY ֵ */
long TOOL_Ftok ( char *pszKeyFile )
{
	char	*pszWorkDir;
	char	szProfile[30];
        key_t    lKey; 

        lKey = 0;
	memset ( szProfile, 0, sizeof szProfile );
	if ( ( pszWorkDir = getenv("KEYDIR") ) == NULL )
		return ( long ) -1;

        /*  ��ȥ KEY �ļ�ǰ��ո�    */
        TOOL_StripSpace( pszKeyFile );

	sprintf ( szProfile, "%s/%s", pszWorkDir, pszKeyFile );

        lKey = 0;
	lKey =  ftok ( szProfile, TOOL_KEYID );

        /*    EEXIST -- 17  ��Ӧ�� IPC �Ѵ��� */
        /*
        if( errno != 0 && errno != EEXIST )
        */

        if( lKey < 0 )
        {
           TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
                "���� TOOL_Ftok() �ļ�Ϊ [%s] ����Ϊ[%d] ������Ϣ:%s\n",
                      __FILE__,
                      __LINE__,
                      strerror( errno ) ) ;
           //TOOL_Printf( (va_list)"szProfile is [%s] id is %d lkey is %ld errmsg [%s]\n",
           TOOL_Printf( "szProfile is [%s] id is %d lkey is %ld errmsg [%s]\n",
                  szProfile, TOOL_KEYID, lKey, strerror( errno )  );
           return ( long ) -1;
        } 
        return lKey;
}


/*   ȡÿһ������������Ϣ�����õ���Ϣ��ʶ      */
int TOOL_GetMsgType( int iNodeId, long *plMsgType )
{
   char *pszSqnoPath; 
   char szSqno[ TOOL_SQNO_LEN ];
   int iReadFd, iWriteFd;
   long lMsgType;
   long lSqno;
   int iWriteLen;

   char szMsgType[12];
   

   /* ������ʼ�� */
   memset( szSqno, 0 , sizeof( szSqno ) );
   memset( szMsgType, 0 , sizeof( szMsgType ) );

   /* ȡ����ļ�·�� */
   if ( ( pszSqnoPath = getenv("SQNOFILE") ) == NULL )
   {
      TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
                  "���� TOOL_GetMsgType() �ļ� %s ���� %d ������Ϣ:%s\n",
                  __FILE__, __LINE__,
                  strerror( errno ) ) ;
      return  -1;
   }

   /*  Ϊ����Ŷ���      */ 
   if( ( iReadFd = open( pszSqnoPath, O_RDWR ) ) == -1 )
   {
      TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
           "���� TOOL_GetMsgType() �ļ�Ϊ [%s] ������[%d] ������Ϣ:?%%s\n",
                 __FILE__,
                 __LINE__,
                 strerror( errno ) ) ;
      return -1;
   }

   /*  Ϊ������Ŷ���      */ 
   if( ( iWriteFd = open( pszSqnoPath, O_RDWR ) ) == -1 ){
      TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
           "���� TOOL_GetMsgType() �ļ�Ϊ [%s] ������[%d] ������Ϣ:?%%s\n",
                 __FILE__,
                 __LINE__,
                 strerror( errno ) ) ;
      return -1;
   }

   if( read( iReadFd, szSqno, sizeof( szSqno ) ) < (unsigned int )0 ){
      TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
           "���� TOOL_GetMsgType() �ļ�Ϊ [%s] ������[%d] ������Ϣ:?%%s\n",
                 __FILE__,
                 __LINE__,
                 strerror( errno ) ) ;
      close( iReadFd );
      return -1;
   }

   /* ǿ�Ʒ�β */
   szSqno[7] = 0; 
   lSqno = atol( szSqno );
   
   close( iReadFd );

   /*  TOOL_NODE_GAP -- 10 */
   sprintf( szMsgType,"%02d%07s", iNodeId + TOOL_NODE_GAP, szSqno ); 
   lMsgType = atol( szMsgType ) ;
   *plMsgType = lMsgType;

   /*   ����ż� 1        */
   sprintf( szSqno,"%07ld\n", lSqno + 1 );

   /*   ��������ļ�               */
   iWriteLen = strlen( szSqno ) +2 ;
   if( write( iWriteFd, szSqno, iWriteLen  ) !=  iWriteLen ){
      TOOL_Trace( TOOL_SYS_TRACE_LEVEL , "Trace",
           "���� TOOL_GetMsgType() �ļ�Ϊ [%s] ������[%d] ������Ϣ:?%%s\n",
                 __FILE__,
                 __LINE__,
                 strerror( errno ) ) ;
      return -1;
   }
   close( iWriteFd );

   
   return 0;
} 


/* �����źŹ���   */
SigFunc TOOL_Signal( int iSigno, SigFunc vSigFunc  )
{
	struct  sigaction stAct, stOutAct ;

	memset( ( void * )&stAct,  0, sizeof stAct  );
	memset( ( void * )&stOutAct, 0, sizeof stOutAct );
	
	stAct.sa_handler = vSigFunc;
	stAct.sa_flags   = 0;

        /*   ��ʼ���źż�        */
	sigemptyset( & stAct.sa_mask );

        /*   �����źŴ���������ԭ�źŴ�����ں���   */
	if ( sigaction( iSigno, &stAct, &stOutAct ) < 0 )
		return ( SIG_ERR );

        /*   stOutAct.sa_handler  ԭ�źŴ�����ں���   */
	return stOutAct.sa_handler;
}


/*   ��־�ǼǺ���         */
void TOOL_Printf(va_list arglist,...)
{
  va_list arg_ptr;
  char *pszFormat;
  FILE *fp;
  struct stat stFileStat;
  struct tm *pstTime;

  time_t dwTime;
  char szName[200];
  char szLogFile[200];
  char szRename[200];
  char szLogPath[200];
  
  char szType[20];
  char *pszDisplay;
  int iDisplay;

  char *pszHome;

  va_start(arg_ptr,arglist);

  pszDisplay = getenv( "DISPLAY" );
  if( pszDisplay == NULL )
     iDisplay = 0;
  else {
     iDisplay = 1;
  }

  pszHome = getenv( "MYROOT" );
  if( pszHome == NULL )
  {
     return ;
  }
  
  sprintf( szLogPath, "%s/log/", pszHome );
  strcpy( szLogFile, "elink" );
  strcpy( szType, ".log" );

  sprintf( szName, "%s%s%s", szLogPath, szLogFile, szType );

  pszFormat = (char *)arglist;

  if( iDisplay ){
    vprintf( pszFormat,arg_ptr );
  }
  if( TRACE ){
    if( (fp = fopen( szName, "a+") ) == NULL ){
      //TOOL_Printf((va_list)"fopen(): %s\n",strerror(errno) );
      TOOL_Printf("fopen(): %s\n",strerror(errno) );
    }

    setvbuf(fp, NULL,_IONBF, 0);
    vfprintf( fp, pszFormat, arg_ptr);

    if( stat( szName, &stFileStat )  ){
      fclose( fp );
      //TOOL_Printf((va_list)"stat(): %s\n",strerror(errno) );
      TOOL_Printf("stat(): %s\n",strerror(errno) );
    }

    chmod( szName, 06777 );


    if( stFileStat.st_size > 1024*256 ){
      dwTime = time(0);
      pstTime = localtime( &dwTime );
      sprintf( szRename,"%s%s%04d%02d%02d%s%02d%02d%02d", 
                       szLogPath, szLogFile,
                       1900+pstTime->tm_year, pstTime->tm_mon+1,
                       pstTime->tm_mday, szType,
                       pstTime->tm_hour, pstTime->tm_min,
                       pstTime->tm_sec );
      rename( szName,szRename);
    }
  }
  fclose( fp );
  va_end(arg_ptr);
  return;
}


/*    ���ú����źż�      */
int TOOL_IgnSignal ( void )
{
   TOOL_Signal( SIGHUP, SIG_IGN ); 
   TOOL_Signal( SIGINT, SIG_IGN ); 
   TOOL_Signal( SIGQUIT, SIG_IGN ); 
   TOOL_Signal( SIGTTIN, SIG_IGN ); 

   TOOL_Signal( SIGTTOU, SIG_IGN ); 
   TOOL_Signal( SIGCHLD, SIG_IGN ); 
   TOOL_Signal( SIGTRAP, SIG_IGN ); 
   TOOL_Signal( SIGTERM, SIG_IGN ); 

   TOOL_Signal( SIGPROF, SIG_IGN ); 
   TOOL_Signal( SIGPIPE, SIG_IGN ); 
   return 0;
}


/* ȡ��ȷʱ��  */
char *TOOL_TimeTell( char *pszBuf )
{
   struct timeval stTimeVal;
   struct timezone stTimeZone;
   struct tm  *pstTime;

   memset( (char *)&stTimeVal, 0, sizeof( struct timeval ) );
   memset( (char *)&stTimeZone, 0, sizeof( struct timezone ) );

   gettimeofday( &stTimeVal, &stTimeZone );
   pstTime = localtime( &stTimeVal.tv_sec );

   sprintf(pszBuf," %d �� %d �� %d �� %02d:%02d:%02d %03d ���� %03d ΢��",
         1900 + pstTime->tm_year,
         pstTime->tm_mon+1, pstTime->tm_mday, pstTime->tm_hour,
         pstTime->tm_min, pstTime->tm_sec,
         stTimeVal.tv_usec/1000, stTimeVal.tv_usec%1000  );
   TOOL_Printf("%s\n", pszBuf );
   return pszBuf;
}


/*  
**  end of file tool_base.c
*/
