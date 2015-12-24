/*
**  file: pkg_base.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "pkg/pkg_base.h"
#include "pkg/pkg_base_cup20.h"
#include "shm_base.h"

#include "getAlignPos.h"
#include "tempadd.h"
#include "log.h"

/*
** 短整型转换, 处理aix与sco之间的转换
*/
short chg_short( short s )
{
   char sshort[3];
   char tmp;

   memset( sshort, 0x00, sizeof(sshort) );
   memcpy( sshort, (unsigned char *)&s, 2 );
   tmp = sshort[0];
   sshort[0] = sshort[1];
   sshort[1] = tmp;
 
   memcpy( (char *)&s, sshort, 2 );
   return s;
}


/*
** 整型转换, 处理aix与sco之间的转换（包括整型之间以及32位机长整型之间）
*/
int chg_int( int t )
{
   char sint[5];
   int i;
   char tmp;
 
   memset( sint, 0x00, sizeof(sint) );
   memcpy( sint, (unsigned char *)&t, 4 );
   for( i=0; i<2; i++ )
   {
      tmp = sint[i];
      sint[i] = sint[3-i];
      sint[3-i] = tmp;
   }
   memcpy( (char *)&t, sint, 4 );
   return t;
}

/*
** 浮点型转换, 处理aix与sco之间的转换
*/
float chg_float( float f )
{
   char sfloat[5];
   int i;
   char tmp;

   memset( sfloat, 0x00, sizeof(sfloat) );
   memcpy( sfloat, (unsigned char *)&f, 4 );
   for( i=0; i<2; i++ )
   {
      tmp = sfloat[i];
      sfloat[i] = sfloat[3-i];
      sfloat[3-i] = tmp;
   }
   memcpy( (char *)&f, sfloat, 4 );
   return f;
}

/*
** 双精度型转换, 处理aix与sco之间的转换
*/
double chg_double( double d )
{
   char sdouble[9];
   int i;
   char tmp;

   memset( sdouble, 0x00, sizeof(sdouble) );
   memcpy( sdouble, (unsigned char *)&d, 8 );
   for( i=0; i<4; i++ )
   {
      tmp = sdouble[i];
      sdouble[i] = sdouble[7-i];
      sdouble[7-i] = tmp;
   }
   memcpy((char *)&d,sdouble,8);
   return d;
}

int PKG_ChangeCharToInt( char cChar )
{
   char cUpper;

   if( cChar >= '0' && cChar <= '9' )
   {
      return cChar - '0';
   }

   cUpper = toupper( cChar );
   if( cUpper > 'F' || cUpper < 'A' )
   {
      return -1;
   }

   return cUpper - 'A' + 10;
}

/**
  * change bitmap to flag
  * para:
  * 1 bitmap
  * 2 compress or uncompress flag
  * 3 bitflag
  */
int PKG_ChangeBitmapToFlag( char *pcBitmap, int iCompressFlag, char *pczFlag )
{
   char aczBitFlags[16][5] = { "0000", "0001", "0010", "0011",
                                "0100", "0101", "0110", "0111",
                                "1000", "1001", "1010", "1011",
                                "1100", "1101", "1110", "1111" };
   char cValue=0;
   int i=0, iIdx=0, iTop=0;
   unsigned char uCh;
   int nBitLen=0;

   assert( pcBitmap != NULL );
   assert( pczFlag  != NULL );

   uCh = (unsigned char)pcBitmap[i];
   if( uCh & 0x80 )
   {
       nBitLen = 16;
   }
   else
   {
       nBitLen = 8 ;
   }

   for( i = 0; i < nBitLen; i++ )
   {
      /*
      if( (iIdx = PKG_ChangeCharToInt( pcBitmap[i] )) == -1 )
      {
         return -1;
      }
      memcpy( pczFlag + i*4, aczBitFlags[iIdx], 4 );
      */
      uCh = (unsigned char)pcBitmap[i];
      iIdx = uCh/16;
      memcpy( pczFlag+i*8, aczBitFlags[iIdx], 4 );
      iIdx = uCh%16;
      memcpy( pczFlag+i*8+4, aczBitFlags[iIdx], 4 );

   }

   return 0;
}

MsgFldCfg * PKG_GetTheFieldConfig( MsgFldCfgAry *epstMsgFldCfgAry, int iFldId )
{
   int i;

   for( i = 0; i < epstMsgFldCfgAry->iNum; i++ )
   {
      if( epstMsgFldCfgAry->pstMsgFldCfg[i].iFldSeq == iFldId )
      {
         return epstMsgFldCfgAry->pstMsgFldCfg+i;
      }
   }

   return NULL;
}

int PKG_CreateBitmap( MsgFldCfgAry *pstMsgFldCfgAry, char *pczBitFlags,
                      char *pczBitmap )
{
   int iIdx, iFlag, iRet;
   char aczBit2[3];
   char aczBitFlags[16][6] = { "00000", "00011", "00102", "00113",
                               "01004", "01015", "01106", "01117",
                               "10008", "10019", "1010A", "1011B",
                               "1100C", "1101D", "1110E", "1111F" };

   memset( pczBitFlags, '0', 128 );
   pczBitFlags[128] = 0;
   

   for( iIdx = 0; iIdx < pstMsgFldCfgAry->iNum; iIdx++ )
   {
      if( pstMsgFldCfgAry->pstMsgFldCfg[iIdx].iFldSeq > 1 &&
          pstMsgFldCfgAry->pstMsgFldCfg[iIdx].iFldSeq <= 128 )
      {
         pczBitFlags[pstMsgFldCfgAry->pstMsgFldCfg[iIdx].iFldSeq-1] = '1';
      }
   }

   for( iIdx = 0; iIdx < 16; iIdx++ )
   {
      memset( aczBit2, 0, sizeof(aczBit2) );
      for( iFlag = 0; iFlag < 16; iFlag++ )
      {
         if( memcmp( aczBitFlags[iFlag], pczBitFlags+iIdx*8, 4 ) == 0 )
         {
            break;
         }
      }
      aczBit2[0] = aczBitFlags[iFlag][4];
      for( iFlag = 0; iFlag < 16; iFlag++ )
      {
         if( memcmp( aczBitFlags[iFlag], pczBitFlags+iIdx*8+4, 4 ) == 0 )
         {
            break;
         }
      }
      aczBit2[1] = aczBitFlags[iFlag][4];
      iRet = PKG_XToA( aczBit2, &(pczBitmap[iIdx]) );
      if( iRet != 0 )
      {
         LOG4C((LOG_FATAL, "pkg_base.log", "PKG_XToA error[%d], bit[%s]", 
                     iRet, aczBit2)); 
         return -1;
      }
      if( iIdx == 0 )
      {
         pczBitmap[iIdx] += 128;/*第0位置1*/
      }
   }

   return 0;
}

/*根据8583报文和域号取域的值*/
int PKG_8583GetField( int eiMsgAppId, char *epczMsgBuf, int eiMsgLen, 
                      int eiFldId, char *opczValue, int *oiFldLen )
{
   char aczBitFlag[129];
   char aczField[1000];
   int  iFldId, iFldLen, iMsgPos = 0, iRet, iIdx;
   MsgFldCfg *pstMsgFldCfg;
   MsgFldCfgAry stMsgFldCfgAry;

   if( eiFldId < 2 || eiFldId > 128 )
   {
      LOG4C ((LOG_FATAL, "eiFldId error[%d]", eiFldId)); 
      return -1;
   }

   memset( &stMsgFldCfgAry, 0, sizeof(stMsgFldCfgAry) );
   iRet = PKG_GetInfoFromMsgFldCfg( eiMsgAppId, &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "PKG_GetInfoFromMsgFldCfg error[%d]", iRet));
      return -1;
   }

   if( memcmp( epczMsgBuf+41, "00000", 5 ) == 0 )
   {
      iMsgPos = 50;
   }
   else
   {
      iMsgPos = 96;
   }

   memset( aczBitFlag, 0, sizeof(aczBitFlag) );
   if( PKG_ChangeBitmapToFlag( epczMsgBuf+iMsgPos, 0, aczBitFlag ) == -1 )
   {
      LOG4C ((LOG_FATAL, "Invalid BITMAP"));
      free( stMsgFldCfgAry.pstMsgFldCfg );
      return -1;
   }
   LOG4C ((LOG_DEBUG, "BITMAP[%s]", aczBitFlag));
   iMsgPos += 16;

   for( iFldId = 1; iFldId < 128; iFldId++ )
   {
      if( aczBitFlag[iFldId] == '0' )  /** no field **/
      {
         if( iFldId+1 == eiFldId )
         {
            LOG4C ((LOG_FATAL, "该域号[%d]值为空", iFldId+1));
            free( stMsgFldCfgAry.pstMsgFldCfg );
            return -2;
         }
         continue;
      }
      /** has the field value **/
      pstMsgFldCfg = PKG_GetTheFieldConfig( &stMsgFldCfgAry, iFldId+1 );
      if( pstMsgFldCfg == NULL )
      {
         LOG4C ((LOG_FATAL, "PKG_GetTheFieldConfig error, fldid=%d",
                     iFldId+1));
         free( stMsgFldCfgAry.pstMsgFldCfg );
         return -1;
      }
      memset( aczField, 0, sizeof(aczField) );
      iRet = PKG_Get8583FieldContent( epczMsgBuf, eiMsgLen, &iMsgPos,
                                      pstMsgFldCfg, aczField, &iFldLen );
      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, "get the field %d value error at message position %d",
                     iFldId+1, iMsgPos));
         free( stMsgFldCfgAry.pstMsgFldCfg );
         return -2;
      }
      if( iFldId+1 == eiFldId )
      {
         memcpy( opczValue, aczField, iFldLen );
         *oiFldLen = iFldLen;
         free( stMsgFldCfgAry.pstMsgFldCfg );
         return 0;
      }
   }
}

int PKG_Get8583FieldContent( char *pczOrgMsg, int eiMsgLen, int *piCurrPos,
                             MsgFldCfg *pstMsgFldCfg, char *pczValue,
                             int *piValLen )
{
   char cDefault=0 ;
   int  iLen=0;
   int  iFldLen=0;
   unsigned char aczLen[20];
   char  aczTmp[999];

   /* only has fixed length and variable length field **/
   switch( pstMsgFldCfg->iSepFldFlag )
   {
   case PKG_FIXLEN_FLD:      /* fixed length field */
      if( (*piCurrPos + pstMsgFldCfg->iFldLen) > eiMsgLen )
      {
         LOG4C ((LOG_FATAL, "PKG_LENGTH_SHORT"));
         return PKG_LENGTH_SHORT;
      }
      memcpy( pczValue, pczOrgMsg + *piCurrPos, pstMsgFldCfg->iFldLen );
      pczValue[pstMsgFldCfg->iFldLen] = 0;
      if( pstMsgFldCfg->iFldType == UDT_STRING )
      {
         cDefault = PKG_ChangeCharToInt( pstMsgFldCfg->aczFldDef[0] )*16 +
                    PKG_ChangeCharToInt( pstMsgFldCfg->aczFldDef[1] );
         if( toupper(pstMsgFldCfg->aczFldAlign[0]) == 'L' )
         {
            PUB_StrRightTrim( pczValue, cDefault );
         }
         else
         {
            PUB_StrLeftTrim( pczValue, cDefault );
         }
         *piValLen = strlen(pczValue);
      }
      else if( pstMsgFldCfg->iFldType == UDT_BIN )
      {
         *piValLen = pstMsgFldCfg->iFldLen;
      }
      *piCurrPos += pstMsgFldCfg->iFldLen;
      return 0;
   case PKG_UNFIXLEN_FLD:    /* variable length field */
      if( (*piCurrPos + pstMsgFldCfg->iLenFldLen) > eiMsgLen )
      {
         LOG4C ((LOG_FATAL, "PKG_LENGTH_SHORT"));
         return PKG_LENGTH_SHORT;
      }
      memset( aczLen, 0, sizeof(aczLen) );
      memcpy( aczLen, pczOrgMsg + *piCurrPos, pstMsgFldCfg->iLenFldLen );



      if( pstMsgFldCfg->iLenFldType == UDT_BIN )
      {
          if( pstMsgFldCfg->iLenFldLen == 1 )
          {
              iLen = (aczLen[0] >> 4) * 10 + (aczLen[0] & 0x0f);
          }
          else
          {
              iLen = (aczLen[0] >> 4) * 10 + (aczLen[0] & 0x0f);
              iLen = iLen*100 + (aczLen[1] >> 4) * 10 + (aczLen[1] & 0x0f);
          }
      }
      else
      {
          iLen = atoi(aczLen);
      }

	  memset(aczTmp, 0, sizeof(aczTmp));
	  sprintf(aczTmp, "%d", iLen);

      iFldLen = iLen;
      if( pstMsgFldCfg->iFldType == UDT_BIN )
      {
          iLen = (iLen+1) / 2;
      }

      if( (*piCurrPos + iLen) > eiMsgLen )
      {
         LOG4C ((LOG_FATAL, "PKG_LENGTH_SHORT, msg[%s],msglen[%d],fldlen[%d],offset[%d] ", pczOrgMsg + *piCurrPos, eiMsgLen, iLen, *piCurrPos));
         return PKG_LENGTH_SHORT;
      }
      /*
      memset( aczTmp, 0, sizeof(aczTmp) );
      memcpy( aczTmp, pczOrgMsg + *piCurrPos + pstMsgFldCfg->iLenFldLen, iLen );
      TOOL_ConvertBCD2Ascii( aczTmp, iFldLen, pczValue );
      */

      /*
      memcpy( pczValue, pczOrgMsg + *piCurrPos + pstMsgFldCfg->iLenFldLen, iLen );
      */
      memcpy( pczValue, pczOrgMsg + *piCurrPos, iLen + pstMsgFldCfg->iLenFldLen );
      *piCurrPos += pstMsgFldCfg->iLenFldLen + iLen;
      *piValLen = iLen + pstMsgFldCfg->iLenFldLen;
      return 0;
   default:
      LOG4C ((LOG_FATAL, "Invalid field type %d", 
                pstMsgFldCfg->iSepFldFlag));
      return -1;
   }
}

int PKG_CreateFieldContent( MsgFldCfgAry *pstMsgFldCfgAry, int iFldId,
                            UDP epfAddr, char *pczFldContent )
{
   MsgFldCfg *pstMsgFldCfg;
   char aczFieldContent[1000], aczFormat[20], aczFldDef[10];
   int iLen;
   unsigned char aczLen[20];

   pstMsgFldCfg = PKG_GetTheFieldConfig( pstMsgFldCfgAry, iFldId );

   memset( aczFldDef, 0, sizeof(aczFldDef) );
   if( strlen( pstMsgFldCfg->aczFldDef ) != 0 )
   {
      PKG_XToA( pstMsgFldCfg->aczFldDef, aczFldDef );
   }

   switch( pstMsgFldCfg->iSepFldFlag )
   {
   case PKG_FIXLEN_FLD:

      iLen = UDO_FieldLen( epfAddr, iFldId, 0 );
      if( pstMsgFldCfg->iFldType == UDT_STRING )
      {
         memset( pczFldContent, aczFldDef[0], 
                 (size_t)pstMsgFldCfg->iFldLen );

         if( pstMsgFldCfg->iFldLen < iLen )
            iLen = pstMsgFldCfg->iFldLen;

         if( pstMsgFldCfg->aczFldAlign[0] == 'R' ||
             pstMsgFldCfg->aczFldAlign[0] == 'r' )
         {
              pczFldContent += pstMsgFldCfg->iFldLen-iLen; 
         }
      }

      UDO_GetField( epfAddr, iFldId, 0, pczFldContent, 0 );
      pczFldContent[pstMsgFldCfg->iFldLen] = 0;
      return pstMsgFldCfg->iFldLen;;

   case PKG_UNFIXLEN_FLD:
      memset( aczFieldContent, 0, sizeof(aczFieldContent) );
      UDO_GetField( epfAddr, iFldId, 0, aczFieldContent, &iLen );
      /*
      sprintf( aczFormat, "%c0%dd", '%', pstMsgFldCfg->iLenFldLen );
      sprintf( aczLen, aczFormat, iLen );
      */

      if( pstMsgFldCfg->iLenFldLen == 1 )
      {
          aczLen[0] = iLen/10;
          aczLen[0] = (aczLen[0] << 4) & 0xf0;
          aczLen[0] |= iLen % 10;
      }
      if( pstMsgFldCfg->iLenFldLen == 2 )
      {
          aczLen[0] = iLen/100;
          aczLen[1] = (iLen%100) / 10;
          aczLen[1] = (aczLen[1] << 4) & 0xf0;
          aczLen[1] |= (iLen % 100) % 10;
      }

      memcpy( pczFldContent, aczLen, pstMsgFldCfg->iLenFldLen );
      if( pstMsgFldCfg->iFldType == UDT_STRING )
      {
          memcpy( pczFldContent + pstMsgFldCfg->iLenFldLen, aczFieldContent, iLen );
          return iLen + pstMsgFldCfg->iLenFldLen;
      }
      else
      {
          TOOL_ConvertAscii2BCDR( aczFieldContent, pczFldContent + pstMsgFldCfg->iLenFldLen,  iLen );
          return (iLen+1)/2 + pstMsgFldCfg->iLenFldLen;
      }
      break;
   default:
      LOG4C ((LOG_FATAL, "Invalid field type %d\n",
                pstMsgFldCfg->iSepFldSeq));
      return -1;
   }
}
