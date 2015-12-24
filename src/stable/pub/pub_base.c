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


/*      ��������:     PUB_StrLeftTrim
**
**����������˵��������ȥ�ַ������ָ���ַ���pczSrc����'\0'�������ַ���
**
**���������ò�����
**�������������������char * epczStr      // ������ַ���
**                    char   ecCh         // ȥ�����ַ�
**���������������������
**
**����������ֵ˵��������
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

/*      ��������:     PUB_StrRightTrim
**
**����������˵��������ȥ�ַ����ұ�ָ���ַ���pczSrc����'\0'�������ַ���
**
**���������ò�����
**�������������������char * epczStr      // ������ַ���
**                    char ecCh           // ȥ�����ַ�
**���������������������
**
**����������ֵ˵��������
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

/*      ����˵��:     PUB_StrTrim
**
**����������˵��������ȥ�ַ�����������ָ���ַ���pczSrc����'\0'�������ַ���
**
**���������ò�����
**�������������������char * epczStr      // ������ַ���
**                    char ecCh           // ȥ�����ַ�
**���������������������
**
**����������ֵ˵��������
*/

void PUB_StrTrim( char * epczStr, char ecCh )
{
   PUB_StrLeftTrim( epczStr, ecCh );
   PUB_StrRightTrim( epczStr, ecCh );
}

/*
** ���ܣ��ַ���ȫ�����д
** ���������1 ������������ַ���
** ���������1 �����д�ַ���
** ����ֵ��    ��
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
** ���ܣ��ַ���ȫ����Сд
** ���������1 ������������ַ���
** ���������1 ���Сд�ַ���
** ����ֵ��    ��
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
 * ����: �ַ������ָ����ֽ���ַ���ʸ��
 * ����: char * epczStr  - ���ֽ��ַ���
 *       char   chSep    - �ָ���  
 *       char **oppczRet - �ַ���ָ������,ָ���ʸ���ַ���
 *       u_int  max_flds - ���ֽ�ʸ����,Ϊ0��ȫ�ֽ������ַ���
 * ����: ʸ������
 * ��ע: ע��,�˺����Ὣԭ�ַ������л�max_flds���ָ����滻���ַ�'\0'
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
