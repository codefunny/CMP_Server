/*
** file: pub_base.c
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include "pub_base.h"


/*      函数名称:     PUB_StrLeftTrim
**
**　　　功能说明：　　去字符串左边指定字符，pczSrc是以'\0'结束的字符串
**
**　　　引用参数：
**　　　　　输入参数：char * epczStr      // 输入的字符串
**                    char   ecCh         // 去掉的字符
**　　　　　输出参数：无
**
**　　　返回值说明：　无
*/

void PUB_StrLeftTrim( char * epczStr, char ecCh )
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

/*      函数名称:     PUB_StrRightTrim
**
**　　　功能说明：　　去字符串右边指定字符，pczSrc是以'\0'结束的字符串
**
**　　　引用参数：
**　　　　　输入参数：char * epczStr      // 输入的字符串
**                    char ecCh           // 去掉的字符
**　　　　　输出参数：无
**
**　　　返回值说明：　无
*/

void PUB_StrRightTrim( char * epczStr, char ecCh )
{
   char * pcEndPos;

   assert( epczStr != NULL );

   if( strlen(epczStr) == 0 )
      return;

   for( pcEndPos = epczStr; *pcEndPos != '\0'; pcEndPos++);

   for( pcEndPos--; (pcEndPos >= epczStr) && *pcEndPos == ecCh;
         *(pcEndPos ) = '\0', pcEndPos-- );
}

/*      函数说明:     PUB_StrTrim
**
**　　　功能说明：　　去字符串左右两边指定字符，pczSrc是以'\0'结束的字符串
**
**　　　引用参数：
**　　　　　输入参数：char * epczStr      // 输入的字符串
**                    char ecCh           // 去掉的字符
**　　　　　输出参数：无
**
**　　　返回值说明：　无
*/

void PUB_StrTrim( char * epczStr, char ecCh )
{
   PUB_StrLeftTrim( epczStr, ecCh );
   PUB_StrRightTrim( epczStr, ecCh );
}

/*
** 功能：字符串全部变大写
** 输入参数：1 输入待操作的字符串
** 输出参数：1 输出大写字符串
** 返回值：    无
*/
void PUB_StrUpper( char *eopczStr )
{
   int iLen, iTemp;

   iLen = strlen( eopczStr );
   for( iTemp = 0; iTemp < iLen; iTemp++ )
   {
      eopczStr[iTemp] = toupper( eopczStr[iTemp] );
   }
}

/*
** 功能：字符串全部变小写
** 输入参数：1 输入待操作的字符串
** 输出参数：1 输出小写字符串
** 返回值：    无
*/
void PUB_StrLower( char *eopczStr )
{
   int iLen, iTemp;

   iLen = strlen( eopczStr );
   for( iTemp = 0; iTemp < iLen; iTemp++ )
   {
      eopczStr[iTemp] = tolower( eopczStr[iTemp] );
   }
}

long PUB_CalcTimeVal( struct timeval stTimeBegin, struct timeval stTimeEnd )
{
   long lTmVal;

   if( stTimeEnd.tv_usec < stTimeBegin.tv_usec )
   {
      stTimeEnd.tv_sec -= 1;
      stTimeEnd.tv_usec += 1000000;
   }

   lTmVal = ( stTimeEnd.tv_sec - stTimeBegin.tv_sec ) * 1000000
            + ( stTimeEnd.tv_usec - stTimeBegin.tv_usec );

   return lTmVal;
}

/**
 * 功能: 字符串按分隔符分解成字符串矢量
 * 参数: char * epczStr  - 待分解字符串
 *       char   chSep    - 分隔符  
 *       char **oppczRet - 字符串指针数组,指向各矢量字符串
 *       u_int  max_flds - 最大分解矢量数,为0完全分解整个字符串
 * 返回: 矢量个数
 * 备注: 注意,此函数会将原字符串所有或max_flds个分隔符替换成字符'\0'
**/
int PUB_ParseStr(char *epczStr, char chSep, char *oppczRet[], 
                 unsigned int max_flds)
{
    char    *pczTmp1, *pczTmp2;
    int     i = 0;

    if  (epczStr == NULL || oppczRet == NULL)
    {
         return (0);
    }

    pczTmp1 = epczStr;

    do {
        oppczRet[i++] = pczTmp1;
	if (max_flds != 0 && i >= max_flds)
	{
            break;
	}

        pczTmp2 = strchr (pczTmp1, chSep);
        if (pczTmp2 != NULL) {
            *pczTmp2 = '\0';
            pczTmp1 = pczTmp2 + 1; 
        }
    } while (pczTmp2 != NULL && *pczTmp1 != '\0');

    return (i);
}

/*
** end of file: pub_base.c
*/
