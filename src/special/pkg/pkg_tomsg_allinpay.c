/*
**  file: pkg_tomsg.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "udo.h"
#include "pkg/pkg_tomsg_cup20.h"
#include "pkg/pkg_tomsg_allinpay.h"
#include "tempadd.h"
#include "log.h"

static char nestedName[50];

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_8583_3( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );

#ifdef __cplusplus
}
#endif


int PKG_CreateAllinpayFieldContent( MsgFldCfgAry *pstMsgFldCfgAry, int iFldId,
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
	  sprintf( aczFormat, "%c0%dd", '%', pstMsgFldCfg->iLenFldLen );
	  sprintf( aczLen, aczFormat, iLen );
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



int PKG_ToMsg_8583_3( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   char aczBitFlags[129], aczBitmap[17], aczFldContent[1000];
   int iFldId, iMsgPos = 0, iFldLen=0, iTop=0, i=0;
   int iRet;
   char aczMsgId[10];
   char aczTmp[3000];

   memset( aczBitFlags, '0', sizeof(aczBitFlags) );
   memset( aczBitmap, 0, sizeof(aczBitmap) );
   memset( aczMsgId, 0, sizeof(aczMsgId) );
   aczBitFlags[128] = 0;

   UDO_GetField( epfsAddr, 0, 0, aczMsgId, &iFldLen );
   memcpy( opcOutMsg+iMsgPos, aczMsgId, iFldLen );
   iMsgPos += iFldLen;

   /** generate bitmap according to fieldid **/
   if( PKG_CreateBitmap( epstMsgFldCfgAry, aczBitFlags, aczBitmap ) == -1 )
   {
      LOG4C ((LOG_FATAL, " create bitmap error\n"));
      return -1;
   }

  // aczBitmap[0] &= 0x7f;

   UDO_ACField( epfsAddr, 1, UDT_BIN, aczBitmap,16 );

   memcpy( opcOutMsg+iMsgPos, aczBitmap, 16 );
   iMsgPos += 16;

   /** create the buffer according the bitmap **/
   for( iFldId = 1; iFldId < 128; iFldId++ )
   {
      if( aczBitFlags[iFldId] == '0' )
      {
         continue;
      }
      memset( aczFldContent, 0, sizeof(aczFldContent) );
      iFldLen = PKG_CreateAllinpayFieldContent( epstMsgFldCfgAry, iFldId+1,
                                        epfsAddr, aczFldContent );

      memset( aczTmp, 0, sizeof(aczTmp) );
      for( i=0; i<iFldLen; i++ )
      {
          sprintf( aczTmp, "%s%02X ", aczTmp, aczFldContent[i]&0xff );
      }
      LOG4C(( LOG_DEBUG, "FIELD[%03d][LEN=%03d][%s]", iFldId+1, iFldLen, aczTmp ));
      memcpy( opcOutMsg + iMsgPos, aczFldContent, iFldLen );
      iMsgPos += iFldLen;
   }
   *opiOutMsgLen  = iMsgPos;

   return 0;
}
