/*
**    tool_datetime.c
**
**    本文件所有关于日期和时间的处理函数。
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>


#include "tool_datetime.h"


int TOOL_GetWeekofDay(char * epaczDate )
{

    struct tm mytm ,*p;
    time_t itime = 0;
    char aczYea[5],aczMon[3],aczDat[3];
    int iweek;

    memset(aczYea,0x00,sizeof(aczYea));
    memset(aczMon,0x00,sizeof(aczMon));
    memset(aczDat,0x00,sizeof(aczDat));

    memcpy(aczYea,epaczDate ,4);
    memcpy(aczMon,epaczDate + 4 ,2);
    memcpy(aczDat,epaczDate  + 6 ,2);
    memset(&mytm,0x00,sizeof(mytm));

    mytm.tm_sec =1;
    mytm.tm_min =0;
    mytm.tm_hour =0;
    mytm.tm_year = atoi(aczYea) - 1900 ;
    mytm.tm_mon = atoi(aczMon) - 1 ;
    mytm.tm_mday = atoi(aczDat) ;

   itime = mktime(&mytm);
   memset(&mytm,0x00,sizeof(mytm));
   p = localtime(&itime);
   iweek = p->tm_wday +1;
   free(p);

   return iweek;
}

void TOOL_GetSystemYYYYMMDD( char *pcYYYYMMDD )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcYYYYMMDD != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   pstTM->tm_year += 1900;
   
/*
   sprintf( pcYYYYMMDD,   "%4d", pstTM->tm_year );

   sprintf( pcYYYYMMDD+4, "%2d", (pstTM->tm_mon+1) );
   if ( pcYYYYMMDD[4] == ' ' )
   {
      pcYYYYMMDD[4] = '0';
   }

   sprintf( pcYYYYMMDD+6, "%2d", pstTM->tm_mday );
   if ( pcYYYYMMDD[6] == ' ' )
   {
      pcYYYYMMDD[6] = '0';
   }
*/
   sprintf( pcYYYYMMDD,   "%04d%02d%02d", pstTM->tm_year,
                                        pstTM->tm_mon+1,
                                        pstTM->tm_mday );
}




void TOOL_GetSystemYYMMDD( char *pcYYMMDD )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcYYMMDD != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pcYYMMDD,   "%2d", pstTM->tm_year );
   if ( pcYYMMDD[0] == ' ' )
   {
      pcYYMMDD[0] = '0';
   }

   sprintf( pcYYMMDD+2, "%2d", (pstTM->tm_mon+1) );
   if ( pcYYMMDD[2] == ' ' )
   {
      pcYYMMDD[2] = '0';
   }

   sprintf( pcYYMMDD+4, "%2d", pstTM->tm_mday );
   if ( pcYYMMDD[4] == ' ' )
   {
      pcYYMMDD[4] = '0';
   }
}



void TOOL_GetSystemYYMM( char *pcYYMM )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcYYMM != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pcYYMM,   "%2d", pstTM->tm_year );
   if ( pcYYMM[0] == ' ' )
   {
      pcYYMM[0] = '0';
   }

   sprintf( pcYYMM+2, "%2d", (pstTM->tm_mon+1) );
   if ( pcYYMM[2] == ' ' )
   {
      pcYYMM[2] = '0';
   }
}




void TOOL_GetSystemMMDD( char *pcMMDD )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcMMDD != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pcMMDD, "%2d", (pstTM->tm_mon+1) );
   if ( pcMMDD[0] == ' ' )
   {
      pcMMDD[0] = '0';
   }

   sprintf( pcMMDD+2, "%2d", pstTM->tm_mday );
   if ( pcMMDD[2] == ' ' )
   {
      pcMMDD[2] = '0';
   }
}




void TOOL_GetSystemMMDDhhmmss( char *pcMMDDhhmmss )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcMMDDhhmmss != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pcMMDDhhmmss, "%2d", (pstTM->tm_mon+1) );
   if ( pcMMDDhhmmss[0] == ' ' )
   {
      pcMMDDhhmmss[0] = '0';
   }

   sprintf( pcMMDDhhmmss+2, "%2d", pstTM->tm_mday );
   if ( pcMMDDhhmmss[2] == ' ' )
   {
      pcMMDDhhmmss[2] = '0';
   }

   sprintf( pcMMDDhhmmss+4, "%2d", pstTM->tm_hour );
   if ( pcMMDDhhmmss[4] == ' ' )
   {
      pcMMDDhhmmss[4] = '0';
   }

   sprintf( pcMMDDhhmmss+6, "%2d", pstTM->tm_min );
   if ( pcMMDDhhmmss[6] == ' ' )
   {
      pcMMDDhhmmss[6] = '0';
   }

   sprintf( pcMMDDhhmmss+8, "%2d", pstTM->tm_sec );
   if ( pcMMDDhhmmss[8] == ' ' )
   {
      pcMMDDhhmmss[8] = '0';
   }
}





void TOOL_GetSystemhhmmss( char *pchhmmss )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pchhmmss != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

/*
   sprintf( pchhmmss, "%2d", pstTM->tm_hour );
   if ( pchhmmss[0] == ' ' )
   {
      pchhmmss[0] = '0';
   }

   sprintf( pchhmmss+2, "%2d", pstTM->tm_min );
   if ( pchhmmss[2] == ' ' )
   {
      pchhmmss[2] = '0';
   }

   sprintf( pchhmmss+4, "%2d", pstTM->tm_sec );
   if ( pchhmmss[4] == ' ' )
   {
      pchhmmss[4] = '0';
   }
*/
   sprintf( pchhmmss, "%02d%02d%02d", pstTM->tm_hour,
                                    pstTM->tm_min,
                                    pstTM->tm_sec );
}




void TOOL_GetSystemhhmm( char *pchhmm )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pchhmm != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pchhmm, "%2d", pstTM->tm_hour );
   if ( pchhmm[0] == ' ' )
   {
      pchhmm[0] = '0';
   }

   sprintf( pchhmm+2, "%2d", pstTM->tm_min );
   if ( pchhmm[2] == ' ' )
   {
      pchhmm[2] = '0';
   }
}






void TOOL_GetSystemYYYYMMDDhhmmss( char *pcYYYYMMDDhhmmss )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcYYYYMMDDhhmmss != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   pstTM->tm_year += 1900;
   
   sprintf( pcYYYYMMDDhhmmss,   "%4d", pstTM->tm_year );

   sprintf( pcYYYYMMDDhhmmss+4, "%2d", (pstTM->tm_mon+1) );
   if ( pcYYYYMMDDhhmmss[4] == ' ' )
   {
      pcYYYYMMDDhhmmss[4] = '0';
   }

   sprintf( pcYYYYMMDDhhmmss+6, "%2d", pstTM->tm_mday );
   if ( pcYYYYMMDDhhmmss[6] == ' ' )
   {
      pcYYYYMMDDhhmmss[6] = '0';
   }

   sprintf( pcYYYYMMDDhhmmss+8, "%2d", pstTM->tm_hour );
   if ( pcYYYYMMDDhhmmss[8] == ' ' )
   {
      pcYYYYMMDDhhmmss[8] = '0';
   }

   sprintf( pcYYYYMMDDhhmmss+10, "%2d", pstTM->tm_min );
   if ( pcYYYYMMDDhhmmss[10] == ' ' )
   {
      pcYYYYMMDDhhmmss[10] = '0';
   }

   sprintf( pcYYYYMMDDhhmmss+12, "%2d", pstTM->tm_sec );
   if ( pcYYYYMMDDhhmmss[12] == ' ' )
   {
      pcYYYYMMDDhhmmss[12] = '0';
   }
}





void TOOL_GetSystemYYMMDDhhmmss( char *pcYYMMDDhhmmss )
{
   time_t        lCurrTime;
   struct tm   *pstTM;

   assert( pcYYMMDDhhmmss != NULL );

   time( &lCurrTime );

   pstTM = localtime( &lCurrTime );

   sprintf( pcYYMMDDhhmmss,   "%2d", pstTM->tm_year );
   if ( pcYYMMDDhhmmss[0] == ' ' )
   {
      pcYYMMDDhhmmss[0] = '0';
   }

   sprintf( pcYYMMDDhhmmss+2, "%2d", (pstTM->tm_mon+1) );
   if ( pcYYMMDDhhmmss[2] == ' ' )
   {
      pcYYMMDDhhmmss[2] = '0';
   }

   sprintf( pcYYMMDDhhmmss+4, "%2d", pstTM->tm_mday );
   if ( pcYYMMDDhhmmss[4] == ' ' )
   {
      pcYYMMDDhhmmss[4] = '0';
   }

   sprintf( pcYYMMDDhhmmss+6, "%2d", pstTM->tm_hour );
   if ( pcYYMMDDhhmmss[6] == ' ' )
   {
      pcYYMMDDhhmmss[6] = '0';
   }

   sprintf( pcYYMMDDhhmmss+8, "%2d", pstTM->tm_min );
   if ( pcYYMMDDhhmmss[8] == ' ' )
   {
      pcYYMMDDhhmmss[8] = '0';
   }

   sprintf( pcYYMMDDhhmmss+10, "%2d", pstTM->tm_sec );
   if ( pcYYMMDDhhmmss[10] == ' ' )
   {
      pcYYMMDDhhmmss[10] = '0';
   }
}

/*
 *
 * Function: GetDateBefore
 * Description:  根据日期计算前面若干天的日期
 * Input:
 *         eiDateNum - 相隔天数(>0表示前面若干天, <0表示后面若干天)
 *         epaczDate - 起始日期(格式YYYYMMDD或者YYYY-MM-DD)
 * Output:
 *         opaczDate - 返回日期(格式YYYYMMDD或者YYYY-MM-DD)
 *
 * Return:
 *         -1 - ERROR DATE FORMAT
 *          0 - SUCC
 * Other:
 *
 */
int TOOL_GetDateBefore(int eiDateNum ,char * epaczDate, char * opaczDate)
{  
   long lTime; 
   long lSub;
   time_t stTime;
   char aczYear[5];
   char aczMonth[3];
   char aczDay[3];   
   char aczTmpDate[50];
   int  iDateFormat = 0;

   struct tm stTmpTime ,stTmpTime2;
   struct tm * stTmpTime1;
   time_t iTmpTime2;

   memset(aczYear, 0, sizeof(aczYear));
   memset(aczMonth, 0, sizeof(aczMonth));
   memset(aczDay, 0, sizeof(aczDay));
   memset(aczTmpDate, 0, sizeof(aczTmpDate));

   memcpy(aczTmpDate,epaczDate,49);

   /* 长度大于 10位数据错误格式*/
   if( strlen(aczTmpDate) > 10)
   {
     return -1;
   }

   /*对于日期的进行拆分*/
   PUB_StrTrim(aczTmpDate,' ');
   if( strlen(aczTmpDate) > 8)
   {
     memcpy(aczYear,aczTmpDate,4);
     memcpy(aczMonth,aczTmpDate + 5,2);
     memcpy(aczDay,aczTmpDate + 8,2);
     iDateFormat = 1;
   }
   else
   {
     memcpy(aczYear,aczTmpDate,4);
     memcpy(aczMonth,aczTmpDate + 4,2);
     memcpy(aczDay,aczTmpDate + 6,2);
     iDateFormat = 0;
   }

   memset( &stTmpTime, 0, sizeof(stTmpTime) );
   stTmpTime.tm_year = atoi(aczYear) - 1900;
   stTmpTime.tm_mon = atoi(aczMonth) - 1;
   stTmpTime.tm_mday = atoi(aczDay);

   stTmpTime.tm_hour = 0;
   stTmpTime.tm_min = 0;
   stTmpTime.tm_sec = 1;

   memset( &stTmpTime2, 0, sizeof(stTmpTime2) );
   stTmpTime2.tm_year = atoi(aczYear) - 1900;
   stTmpTime2.tm_mon = atoi(aczMonth) - 1;
   stTmpTime2.tm_mday = atoi(aczDay) -1;
   stTmpTime2.tm_hour = 0;
   stTmpTime2.tm_min = 0;
   stTmpTime2.tm_sec = 1;

   iTmpTime2 = mktime(&stTmpTime);

   lSub = (mktime(&stTmpTime) - mktime(&stTmpTime2)) * eiDateNum;

   iTmpTime2 -= lSub ;

   stTmpTime1 = localtime( &iTmpTime2);

   if( iDateFormat == 1 )
      sprintf(opaczDate,"%04d-%02d-%02d",stTmpTime1->tm_year+ 1900 ,stTmpTime1->tm_mon+1,stTmpTime1->tm_mday );
   else
      sprintf(opaczDate,"%04d%02d%02d",stTmpTime1->tm_year+ 1900 ,stTmpTime1->tm_mon+1,stTmpTime1->tm_mday );

   return 0;
}



/*
**   end of file tool_datetime.c
*/
