/*
**    tool_ascii.c
**
**    本文件包含所有关于ASCII、EBCDIC等码制转换的函数.
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
#include <limits.h>

#include "tool_ascii.h"



void TOOL_ConvertAscii2Long( unsigned char *pucString, 
                            unsigned long *pulValue,
	                    int iLength)
{
   long  i;

   assert( pucString != NULL );
   assert( pulValue  != NULL );

   /*
   ** 参考/usr/include/limits.h后，我们发现unsigned long的最大数值为
   ** 4294967295, 因此 iLength应<=10, 并且pucString应<="4294967295".
   ** 否则回出现不可预测的数值。
   */
   if ( iLength > 10 )
   {
      iLength = 10;
   }

   for ( pucString += iLength-1, *pulValue = 0, i = 1; iLength --; i *= 10 )
       *pulValue += ( ( *pucString-- & 017 ) * i );

   return;
}







void TOOL_ConvertLong2Ascii( unsigned char *pucString, 
                            unsigned long lValue, 
                            int iLength )
{
    assert ( pucString != NULL );

    for ( pucString += iLength -1; iLength --; 
	 *pucString-- = (unsigned char)(lValue % 10 | '0'), lValue /= 10 )
	 ;

    return;
}






void TOOL_ConvertAscii2Int( unsigned char *pucString, 
                           unsigned int  *puiValue,
	                   int iLength )
{
   int  i;

   assert( pucString != NULL );
   assert( puiValue  != NULL );

   /*
   ** 参考/usr/include/limits.h后，我们发现unsigned int的最大数值为
   ** 4294967295, 因此 iLength应<=10, 并且pucString应<="4294967295".
   ** 否则回出现不可预测的数值。
   */
   if ( iLength > 10 )
   {
      iLength = 10;
   }

   for ( pucString += iLength-1, *puiValue = 0, i = 1; iLength --; i *= 10 )
       *puiValue += ( ( *pucString-- & 017 ) * i );

   return;
}





void TOOL_ConvertInt2Ascii( unsigned char *pucString, 
                           unsigned int  iValue, 
                           int iLength )
{
    assert ( pucString != NULL );

    for ( pucString += iLength -1; iLength --; 
	 *pucString-- = (unsigned char)(iValue % 10 | '0'), iValue /= 10 )
	 ;

    return;
}




void TOOL_AsciiIncrement( unsigned char *pucAscii, int iLength )
{
   unsigned long ulValue;
	
   assert( pucAscii != NULL );

   /*
   ** 参考/usr/include/limits.h后，我们发现unsigned long的最大数值为
   ** 4294967295, 因此 iLength应<=10, 并且pucString应<="4294967295".
   ** 否则回出现不可预测的数值。
   */
   TOOL_ConvertAscii2Long( pucAscii, &ulValue, iLength );
   ulValue += 1;
	
   TOOL_ConvertLong2Ascii( pucAscii, ulValue, iLength );

   return;
}








unsigned short TOOL_Host2NetworkShort( unsigned short usConvert )
{
   unsigned short usResult;

   ( ( char * )&usResult )[0] = ( ( char * )&usConvert )[1];
   ( ( char * )&usResult )[1] = ( ( char * )&usConvert )[0];

   return( usResult );
}






unsigned long TOOL_Host2NetworkLong( unsigned long ulConvert )
{
   unsigned long ulResult;

   ( ( char * )&ulResult )[0] = ( ( char * )&ulConvert )[3];
   ( ( char * )&ulResult )[1] = ( ( char * )&ulConvert )[2];
   ( ( char * )&ulResult )[2] = ( ( char * )&ulConvert )[1];
   ( ( char * )&ulResult )[3] = ( ( char * )&ulConvert )[0];

   return( ulResult );
}








unsigned short TOOL_Network2HostShort( unsigned short usConvert )
{
   unsigned short usResult;

   ( ( char * )&usResult )[0] = ( ( char * )&usConvert )[1];
   ( ( char * )&usResult )[1] = ( ( char * )&usConvert )[0];
	
   return( usResult );
}





unsigned long TOOL_Network2HostLong( unsigned long ulConvert )
{
   unsigned long ulResult;
	
   ( ( char * )&ulResult )[0] = ( ( char * )&ulConvert )[3];
   ( ( char * )&ulResult )[1] = ( ( char * )&ulConvert )[2];
   ( ( char * )&ulResult )[2] = ( ( char * )&ulConvert )[1];
   ( ( char * )&ulResult )[3] = ( ( char * )&ulConvert )[0];
	
   return( ulResult );
}

/*ASCII TO BCD 右对齐*/
int TOOL_ConvertAscii2BCDR( const char *epctAsc, char *opctBcd,  int iAscLen )
{
   int  i, j;
   char cTmp=0;

   for (i=0,j=0,cTmp=0; i<iAscLen; i++)
   {
      if( epctAsc[i] >= 'a' && epctAsc[i] <= 'f' )
      {
          cTmp = epctAsc[i] - 'a' + 10;
      }
      else if( epctAsc[i] >= 'A' && epctAsc[i] <='F' )
      {
          cTmp = epctAsc[i] - 'A' + 10;
      }
      else if( epctAsc[i] >= '0' && epctAsc[i] <='9' )
      {
          cTmp = epctAsc[i] - '0';
      }
      else
      {
          cTmp = epctAsc[i] & 0x0F;
      }

      if (i & 1)
      {
          opctBcd[j++] |= cTmp & 0x0f;
      }
      else
      {
          opctBcd[j] = cTmp << 4;
      }
   }
   iAscLen = (iAscLen+1) / 2;
   return iAscLen;
}

/*
**   说明：
**      合法的输入字符为"0 1 2 3 4 5 6 7 8 9 a b c d e f", 对于其它的输入字符，
**      缺省转换为"f".
**      如果iInBufLen为奇数，则转换成的BCD码右对齐, 最左侧补0x0,例如：
**      输入为"1234567", 则输出为"0x01,0x23,0x45,0x67".
*/
void TOOL_ConvertAscii2BCD( unsigned char *pucInBuf, 
                           unsigned char *pucOutBuf, 
                           int iInBufLen)
{
   unsigned char  ucValue;
   int iLastOutByte;               
   int iIndex, iOutBufPtr;

   assert( pucInBuf  != NULL );
   assert( pucOutBuf != NULL );

   if (iInBufLen % 2 == 0)
   {
      iLastOutByte = iInBufLen/2;
   }
   else
      iLastOutByte = ( iInBufLen + 1 )/2;
		
   for ( iIndex = iInBufLen - 1, iOutBufPtr = iLastOutByte - 1; 
         iIndex >= 0; iIndex--, iOutBufPtr--)
   {
      if ( ( pucInBuf[iIndex] >= '0' ) && ( pucInBuf[iIndex] <= '9' ) )
         ucValue = pucInBuf[iIndex] - '0';
      else if ( ( pucInBuf[iIndex] >= 'A' ) && ( pucInBuf[iIndex] <= 'F' ) )
      {
         ucValue = pucInBuf[iIndex] - 55;                 /* 'A' = 65  */
      }
      else if( ( pucInBuf[iIndex] >= 'a' ) && ( pucInBuf[iIndex] <= 'f' ) )
      {
         ucValue = pucInBuf[iIndex] - 87;                 /* 'a' = 97  */
      }
      else
      {
         ucValue = 0x0f;
      }

      iIndex--;
		
      if ( iIndex < 0 )
      {
         /* impair number */
         pucOutBuf[iOutBufPtr] = ucValue;
         break;
      }
		
      if ( ( pucInBuf[iIndex] >= '0' ) && ( pucInBuf[iIndex] <= '9' ) )
         ucValue |= (pucInBuf[iIndex] - '0') << 4;
      else if ( ( pucInBuf[iIndex] >= 'A' ) && ( pucInBuf[iIndex] <= 'F' ) )
         ucValue |= (pucInBuf[iIndex] - 55) << 4;
      else if ( ( pucInBuf[iIndex] >= 'a' ) && ( pucInBuf[iIndex] <= 'f' ) )
         ucValue |= ( pucInBuf[iIndex] - 87 ) << 4;
      else	
         ucValue |= 0xf0;

      pucOutBuf[iOutBufPtr] = ucValue;
   }
	
   return;
}

void TOOL_ConvertBCD2Ascii( unsigned char *pucSource, 
                           int iSourceLen,
                           unsigned char *pucDest )
{
   int iFirst=0;

   assert( pucSource != NULL );
   assert( pucDest   != NULL );

   while (iSourceLen--)
   {
      if (iFirst == 0)
      {
         if (((unsigned char)(*pucSource) >> 4) >= 0 && ((unsigned char)(*pucSource) >> 4) <= 9)
            *pucDest++ = (unsigned char)((*pucSource >> 4)|0x30);
         else
            *pucDest++ = (unsigned char)((*pucSource >> 4) + 0x37);

         iFirst ++;
      }
      else
      {
         if (((unsigned char)(*pucSource) & 0x0F)>=0 && ((unsigned char)(*pucSource) & 0x0F)<=9)
            *pucDest++ = (unsigned char)((*pucSource & 0x0F)|0x30);
         else
            *pucDest++ = (unsigned char)((*pucSource & 0x0F) + 0x37);

         pucSource ++;
         iFirst = 0;
     }

   }

   return;
}


void TOOL_ConvertBCD2AsciiR( unsigned char *pucSource, 
                           int iSourceLen,
                           unsigned char *pucDest )
{
   int iFirst=0;

   assert( pucSource != NULL );
   assert( pucDest   != NULL );

   if((iSourceLen %2) != 0)
   {
	   iFirst ++;
   }

   while (iSourceLen--)
   {
      if (iFirst == 0)
      {
         if (((unsigned char)(*pucSource) >> 4) >= 0 && ((unsigned char)(*pucSource) >> 4) <= 9)
            *pucDest++ = (unsigned char)((*pucSource >> 4)|0x30);
         else
            *pucDest++ = (unsigned char)((*pucSource >> 4) + 0x37);

         iFirst ++;
      }
      else
      {
         if (((unsigned char)(*pucSource) & 0x0F)>=0 && ((unsigned char)(*pucSource) & 0x0F)<=9)
            *pucDest++ = (unsigned char)((*pucSource & 0x0F)|0x30);
         else
            *pucDest++ = (unsigned char)((*pucSource & 0x0F) + 0x37);

         pucSource ++;
         iFirst = 0;
     }

   }

   return;
}




void TOOL_ConvertAscii2EBCDIC( char *pcBufAscii, 
                              int iLength,
                              char *pcBufEBCDIC)
{
   int iIndex = 0;

unsigned char as_to_ebc[128] =
{
0x00,0x7c,0x02,0x7c,0x7c,0x7c,0x2e,0x2f,0x16,0x05,0x25,0x0b,0x0c,0x0d,0x0e,0x0f,
0x7c,0x11,0x12,0x13,0x3c,0x7c,0x7c,0x7c,0x18,0x19,0x3f,0x27,0x1c,0x1d,0x1e,0x1f,
0x40,0x4f,0x7f,0x7b,0x5b,0x6c,0x50,0x7d,0x4d,0x5d,0x5c,0x4e,0x6b,0x60,0x4b,0x61,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0x7a,0x5e,0x4c,0x7e,0x6e,0x6f,
0x7c,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xC8,0xc9,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,
0xd7,0xd8,0xd9,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0x4a,0xe0,0x5a,0x5f,0x6d,
0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,
0x97,0x98,0x99,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xc6,0xad,0x00,0xa1,0x07
};

   assert( pcBufAscii  != NULL );
   assert( pcBufEBCDIC != NULL );
  
   while( iIndex < iLength)
   {
      if (*pcBufAscii >= 127)
      {
         *pcBufEBCDIC++ = 0x7C;
      }
      else
      {
         *pcBufEBCDIC++ = as_to_ebc[*pcBufAscii];
      }

      pcBufAscii++;
      iIndex++;
   }


   return;
}



void TOOL_ConvertEBCDIC2Ascii( char *pcEBCDICBuf, 
                              int iEBCDICLen,
                              char *pcAsciiBuf)
{
   unsigned char ucIndex;

unsigned char uc_tebcasc[256]=
{
0x00,0x01,0x02,0x03,0x84,0x09,0x86,0x7f,0x88,0x89,0x8a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x12,0x94,0x0a,0x08,0x97,0x18,0x19,0x9a,0x9b,0x1c,0x1d,0x1e,0x1f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xca,0x17,0x1b,0xa8,0xa9,0xaa,0xab,0xac,0x05,0x06,0x07,
0xb0,0xb1,0x16,0xb3,0xb4,0xb5,0xb6,0x04,0xb8,0xb9,0xba,0xbb,0x14,0x15,0xbe,0x1a,
' ',0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0x5b,0x2e,0x3c,0x28,0x2b,0x21,
0x26,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0x5d,0x24,0x2a,0x29,0x3b,0x5e,
0x2d,0x2f,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0x7c,0x2c,0x25,0x5f,0x3e,0x3f,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0x60,0x3a,0x23,0x40,0x27,0x3d,0x22,
0x80, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f, 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x7e, 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x7b, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x7d, 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x5c,0x3f, 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
};

   assert( pcAsciiBuf  != NULL );
   assert( pcEBCDICBuf != NULL );

   for (ucIndex = 0; ucIndex < (unsigned char)iEBCDICLen; ucIndex++ )
   {
      unsigned char ucEBCDIC =  pcEBCDICBuf[ucIndex];
      pcAsciiBuf[ucIndex] = uc_tebcasc[ucEBCDIC];
   }

   return;
}





/*
**   说明：
**       本函数将两个字节的数字字符转换为对应的数字，存放在一个字符里，例如：
**       输入为"12",则返回字符的ASCII值为12.
**
**       合法的输入字符为"0123456789",如果输入其它字符，则返回值依赖于对应字
**       符的ASCII值。
*/
unsigned char TOOL_ConvertAscii2Binary( char *pcString )
{
   assert( pcString != NULL );

   return( (*pcString - 0x30) * 10 + (*(pcString + 1) - 0x30));
}





/*
**   说明：
**       本函数将一个字节的ASCII值转换为对应的两个字符，例如：
**       输入字符的ASCII值为12,则返回字符"12".
**
**       合法的输入字符为ASCII值为00-99",如果输入其它字符，则返回依赖于对应字
**       符的ASCII值。
*/
void TOOL_ConvertByte2Ascii( unsigned char ucByte, char *pcOut )
{
   assert( pcOut != NULL );

   *pcOut++ = (ucByte / 10) + 0x30;
   *pcOut  = (ucByte % 10) + 0x30;

   return;
}




int  TOOL_BitExist( int iBitNo, unsigned char *pbyBitMap )
{
   int           p, q;
   unsigned char uch;

   assert( pbyBitMap != NULL );
   assert( iBitNo    >  0    );

   p = ( iBitNo-1 ) / 8;
   q = ( iBitNo-1 ) % 8;

   uch = pbyBitMap[p];
   if ( ( uch &= ( 0x80>>q ) ) != 0x00 )
   {
      return( 1 );
   }
   else
   {
      return( 0 );
   }
}





/*
**   end of file tool_ascii.c
*/
