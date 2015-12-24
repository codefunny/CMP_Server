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
#include "pkg/pkg_tomid_bc.h"
#include "tempadd.h"
#include "log.h"

static char nestedName[50];

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

extern char *pczExecFileName;
int PKG_ToMid_8583_2(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr )
{
   char aczBitFlag[129];
   char aczField[1000];
   char aczTmp[3000];
   char aczAscii[2000];
   int  iFldId, iFldLen, iMsgPos = 0, iRet, iIdx, i;
   MsgFldCfg *pstMsgFldCfg;

   /*解析报文头*/

   UDO_AddField( opfsAddr, 1000, UDT_BIN, epcOrgMsg, eiOrgMsgLen );

   UDO_AddField( opfsAddr, 0, UDT_BIN, epcOrgMsg+iMsgPos, 2 );
   iMsgPos += 2;

   UDO_AddField( opfsAddr, 1, UDT_BIN, epcOrgMsg+iMsgPos, 8 );

   memset( aczBitFlag, 0, sizeof(aczBitFlag) );
   if( PKG_ChangeBitmapToFlag( epcOrgMsg+iMsgPos, 0, aczBitFlag ) == -1 )
   {
      LOG4C ((LOG_FATAL, " Invalid BITMAP"));
      return -1;
   }

   iMsgPos += 8;

   /**
    * split the message to udo according to the bitmap
    */
   for( iFldId = 1; iFldId < 64; iFldId++ )
   {
      if( aczBitFlag[iFldId] == '0' )  /** no field **/
      {
         continue;
      }
      /** has the field value **/
      pstMsgFldCfg = PKG_GetTheFieldConfig( epstMsgFldCfgAry, iFldId+1 );
      if( pstMsgFldCfg == NULL )
      {
         LOG4C ((LOG_FATAL, " cann't find the field %d at node %d %s,but continue",
                  iFldId+1, eiNodeId, pczExecFileName));
         return -1;
      }
      memset( aczField, 0, sizeof(aczField) );
      iRet = PKG_Get8583FieldContent( epcOrgMsg, eiOrgMsgLen, &iMsgPos,
                                      pstMsgFldCfg, aczField, &iFldLen );
      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " get the field %d value error at message position %d\n",
                  iFldId+1, iMsgPos));
         return iRet;
      }
      /* all the field is charactor */
      memset( aczTmp, 0, sizeof(aczTmp) );
      for( i=0; i<iFldLen; i++ )
      {
          sprintf( aczTmp, "%s%02X ", aczTmp, aczField[i]&0xff );
      }
      LOG4C(( LOG_DEBUG, "FIELD[%03d][LEN=%03d][%s]", iFldId+1, iFldLen, aczTmp ));
      {
          iRet = UDO_AddField( opfsAddr, iFldId+1, pstMsgFldCfg->iFldType, aczField, iFldLen );
      }

      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " add the field %d value error[%d]\n", iFldId+1, iRet));
         return iRet;
      }
   }

   /**
    * it's possiable that the same transaction has the diffrent bitmap
    * so we do not check the field identify of the udo exactly equal the field identify of the config
    */
   for( iIdx = 0; iIdx < epstMsgFldCfgAry->iNum; iIdx++ )
   {
      pstMsgFldCfg = epstMsgFldCfgAry->pstMsgFldCfg + iIdx;
      if( UDO_FieldExist( opfsAddr, pstMsgFldCfg->iFldSeq ) == 0 )
      {
         UDO_AddField( opfsAddr, pstMsgFldCfg->iFldSeq, UDT_STRING, "", 0 );
      }
   }

   return 0;
}


