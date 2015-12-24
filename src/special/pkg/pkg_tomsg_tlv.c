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
#include "tempadd.h"
#include "log.h"
#include "tlv.h"
#include "pkg/pkg_tomsg_tlv.h"

static char nestedName[50];

static MsgFldCfg * PKG_GetTheFieldConfig( MsgFldCfgAry *epstMsgFldCfgAry, int iFldId )
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


int PKG_ToMsg_tlv( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   int iIdx=0;
   int nTagNum;
   int nValueLen=0;
   char szValue[3000];
   MsgFldCfg *pstMsgFldCfg;
   int nTag=0;
   char aczFieldContent[1000];
   int iLen;
   unsigned char aczLen[20];
   char aczTmp[3000];
   int i=0;  
   int j=0;
   
   memset(szValue, 0, sizeof(szValue));
   nValueLen =0;
   nTagNum =0;

   for( iIdx = 0; iIdx < epstMsgFldCfgAry->iNum; iIdx++ )
   {   	   
	   nTag = epstMsgFldCfgAry->pstMsgFldCfg[iIdx].iFldSeq;

	   /*获取域内容*/
	   memset( aczFieldContent, 0, sizeof(aczFieldContent) );
	   UDO_GetField( epfsAddr, nTag, 0, aczFieldContent, &iLen );
	   
	   /*域说明*/
	   pstMsgFldCfg = PKG_GetTheFieldConfig( epstMsgFldCfgAry, nTag);
	   memset( aczTmp, 0, sizeof(aczTmp) );
	   for( j=0; j<iLen; j++ )
	   {
		   if( pstMsgFldCfg->iFldType == UDT_STRING )
		   {
			   sprintf( aczTmp, "%s%c", aczTmp, aczFieldContent[j]&0xff );
		   }
		   else
		   {
			   sprintf( aczTmp, "%s%02X ", aczTmp, aczFieldContent[j]&0xff );
		   }
	   }
	   LOG4C(( LOG_DEBUG, "FIELD[%03d][LEN=%03d][%s]", nTag, iLen, aczTmp ));
	   /*保存TLV*/
	   TlvSetValue(nTag, szValue, &nValueLen,aczFieldContent, iLen, &nTagNum);
  	}


    /*TLV标签域*/
    sprintf(opcOutMsg, "%03d%05d", nTagNum, nValueLen);
	memcpy(opcOutMsg+8, szValue, nValueLen);
   *opiOutMsgLen  = (nValueLen + 8);

   return 0;
}


