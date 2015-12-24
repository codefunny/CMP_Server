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
#include "pkg/pkg_tomsg_bc.h"
#include "tempadd.h"
#include "log.h"

static char nestedName[50];

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_8583_2( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );
#ifdef __cplusplus
}
#endif

int PKG_ToMsg_8583_2( char * opcOutMsg, int *opiOutMsgLen,
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

   UDO_GetField( epfsAddr, 1003, 0, aczFldContent, &iFldLen );
   memcpy( opcOutMsg, aczFldContent, iFldLen );
   iMsgPos += iFldLen;

   UDO_GetField( epfsAddr, 0, 0, aczMsgId, &iFldLen );
   memcpy( opcOutMsg+iMsgPos, aczMsgId, iFldLen );
   iMsgPos += iFldLen;

   /** generate bitmap according to fieldid **/
   if( PKG_CreateBitmap( epstMsgFldCfgAry, aczBitFlags, aczBitmap ) == -1 )
   {
      LOG4C ((LOG_FATAL, " create bitmap error\n"));
      return -1;
   }

   aczBitmap[0] &= 0x7f;

   UDO_ACField( epfsAddr, 1, UDT_BIN, aczBitmap,8 );

   memcpy( opcOutMsg+iMsgPos, aczBitmap, 8 );
   iMsgPos += 8;

   /** create the buffer according the bitmap **/
   for( iFldId = 1; iFldId < 64; iFldId++ )
   {
      if( aczBitFlags[iFldId] == '0' )
      {
         continue;
      }
      memset( aczFldContent, 0, sizeof(aczFldContent) );
      iFldLen = PKG_CreateFieldContent( epstMsgFldCfgAry, iFldId+1,
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
