#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "log4c.h"

#define DFT_LOG_FLAG "0000"

struct log4c_globle_var {
    char log_iniFile[64];
    char log_flag[16];
    char logFileName[64];
    char logFileSuffix[64];
    char logCurFileName[128];
    char logDir[64];
    int  logFileSize;
    FILE *logFile;
};

struct log4c_tag {
    char  fname[128];
    char  func[128];
    int   lineno;
    char  serial[16];
    char  usertag[64];
};

static struct log4c_globle_var log4c_var ;
static struct log4c_tag log4c_tag;

char *log4c_basename(char *str)
{
        int i;
        for(i=strlen(str)-1; i>=0; i--)
                if (str[i]=='/') break;
        return &str[i+1];
}

char *log4c_dirname(char *str)
{
    int i;
    static char dirStr[128];

    for(i=strlen(str)-1; i>=0; i--) {
        if (str[i]=='/') {
            break;
        }
    }

    strncpy (dirStr, str, i + 1);
    dirStr[i] = '\0';

    return dirStr;
}

void log4c_init( char *config )
{
   char line[1205];
   char *p1;
   FILE *fp;

   memset (&log4c_var, 0, sizeof (log4c_var));
   strcpy (log4c_var.log_iniFile, config);
   strcpy (log4c_var.log_flag, DFT_LOG_FLAG);
   log4c_var.logFile = stdout;

   memset (&log4c_tag, 0, sizeof (log4c_tag));
   
   fp = fopen( config, "r" );
   if( fp == NULL )
   {
      return;
   }

   memset( line, 0, sizeof(line) );
   while( fgets( line, sizeof(line), fp ) != NULL )
   {
      if( line[0] != '#' )
      {
         line[strlen(line)-1] = 0;
         p1 = strchr( line, '=' );
         if( p1 != NULL )
         {
            *p1 = 0;
            p1 ++;
            if( strcmp( line, "file" ) == 0 )
            {
               strcpy( log4c_var.logFileName, p1 );
               strcpy( log4c_var.logDir, log4c_dirname (p1));
            }
            else if( strcmp( line, "flag" ) == 0 )
            {
               strcpy( log4c_var.log_flag, p1 );
            }
            else if( strcmp( line, "size" ) == 0 )
            {
               log4c_var.logFileSize = atoi( p1 );
            }
         }
      }
      memset( line, 0, sizeof(line) );
   }

   fclose( fp );
}

void log4c_open( void )
{
   char dateStr[16];
   struct stat st;
   char filebak[200];
   char timeStr[16];

   if( strlen (log4c_var.logFileName) == 0 )
   {
      log4c_var.logFile = stdout;
   }
   else
   {
      memset (dateStr, 0, sizeof (dateStr));

      TOOL_GetSystemYYYYMMDD (dateStr);
      if (strlen (log4c_var.logFileSuffix) != 0) {
          sprintf (log4c_var.logCurFileName, "%s.%s.%s",
                   log4c_var.logFileName,
                   dateStr,
                   log4c_var.logFileSuffix
                  );
      }
      else {
          sprintf (log4c_var.logCurFileName, "%s.%s",
                   log4c_var.logFileName,
                   dateStr
                  );
      }

      /********** add 2011-10-21***********/
      memset( &st, 0, sizeof( struct stat ) );
      stat( log4c_var.logCurFileName, &st );
      if( st.st_size > 1024*1024*30 )/*30M*/
      {
          memset( timeStr, 0, sizeof(timeStr) );
          TOOL_GetSystemhhmmss (timeStr);
          memset( filebak, 0, sizeof(filebak) );
          sprintf( filebak, "%s.%s", log4c_var.logCurFileName, timeStr );
          rename( log4c_var.logCurFileName, filebak );
      }
      /********** add 2011-10-21END********/

      log4c_var.logFile = fopen(log4c_var.logCurFileName, "a+" );
      if( log4c_var.logFile == NULL )
      {
         log4c_var.logFile = stdout;
      }
   }
}

void log4c_close( void )
{
   if( log4c_var.logFile != stdout && log4c_var.logFile != NULL)
   {
      fclose( log4c_var.logFile );
      log4c_var.logFile = NULL;
   }
}

FILE *log4c_fp ()
{
    return log4c_var.logFile;
}

const char *log4c_logCurFileName ()
{
    return log4c_var.logCurFileName;
}

void log4c_chgIntertag (const char * fname, const char * func, int lineno)
{
    strncpy (log4c_tag.fname, fname, sizeof (log4c_tag.fname) - 1);     
    strncpy (log4c_tag.func, func, sizeof (log4c_tag.func) - 1);     
    log4c_tag.lineno = lineno;
}

void log4c_chgUsertag (const char * serial, const char * usertag)
{
    strncpy (log4c_tag.serial, serial, sizeof (log4c_tag.serial) - 1);     
    strncpy (log4c_tag.usertag, usertag, sizeof (log4c_tag.usertag) - 1);     
}

const char * log4c_logFileSuffix ()
{
     return log4c_var.logFileSuffix;
}

void log4c_chgLogFileSuffix (const char *suffix)
{
    strcpy (log4c_var.logFileSuffix, suffix);
}

const char * log4c_logDir ()
{
     return log4c_var.logDir;
}

void log4c_logInfo (struct log4c_logInfo *log_info)
{
    strcpy (log_info->log_iniFile, log4c_var.log_iniFile);
    strcpy (log_info->logFileSuffix, log4c_var.logFileSuffix);
    strcpy (log_info->serial, log4c_tag.serial);
    strcpy (log_info->usertag, log4c_tag.usertag);
}

int log4c_IsLog (int level)
{
   if (level < LOG_FATAL || level > LOG_DEBUG) {
       return 0;
   }

   if (log4c_var.log_flag[level] != '1')
   {
       return 0;
   }

   return (1);
}

void log4c( int level, char *fformat, ... )
{
   va_list args;
   static char *preLine[] = { "FATAL", "WARN",
                              "INFO", "DEBUG" };

   time_t timeNow;
   struct tm *logTimeNow;
   char   timestr[32];

   if (level < LOG_FATAL || level > LOG_DEBUG) {
       return;
   }

   if (log4c_var.log_flag[level] != '1')
   {
       return;
   }

   time (&timeNow);
   logTimeNow = localtime( &timeNow );
   sprintf( timestr, "%04d%02d%02d|%02d:%02d:%02d",
            logTimeNow->tm_year+1900,
            logTimeNow->tm_mon+1,
            logTimeNow->tm_mday,
            logTimeNow->tm_hour,
            logTimeNow->tm_min,
            logTimeNow->tm_sec
          );

   fprintf (log4c_var.logFile, "%s|%d|%s|%s|%s|%s|%d|", 
            preLine[level],
            getpid(),
            log4c_tag.usertag,
            log4c_tag.serial,
            timestr,
            log4c_tag.fname,
            log4c_tag.lineno
    );

   va_start( args, fformat );
   vfprintf( log4c_var.logFile, fformat, args );
   va_end( args );
   fprintf( log4c_var.logFile, "\n\n" );

   fflush( log4c_var.logFile );
}
