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
#include "pkg/pkg_tomid_tlv.h"
    
static char nestedName[50];

extern char *pczExecFileName;

int PKG_ToMid_tlv(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr )
{
	int nTagNum=0;
	int nTlvValueLen=0;
	char aczTmp[3000];
	char aczValue[3000];
	int iMsgPos=0;
	int i=0;
	int nRet;
	int nTag=0;
	int nValueLen=0;
	MsgFldCfg *pstMsgFldCfg;
    int iIdx;
	int j=0;

	iMsgPos=0;

    /*TLV 数量*/
    memset(aczTmp, 0, sizeof(aczTmp));
    memcpy(aczTmp, epcOrgMsg+iMsgPos, 3);
    iMsgPos +=3;
    nTagNum = atoi(aczTmp);   

    /*TLV 域长度*/
    memset(aczTmp, 0, sizeof(aczTmp));
    memcpy(aczTmp, epcOrgMsg+iMsgPos, 5);
    iMsgPos +=5;
    nTlvValueLen = atoi(aczTmp);  

   /*读取各个标签值*/
   for(i=0; i<nTagNum; i++)
   {
   		nValueLen = 0;
   		nTag = 0;
		memset(aczValue, 0, sizeof(aczValue));
 		nRet = TlvGetCurrentTag(&nTag, aczValue, &nValueLen, epcOrgMsg, &iMsgPos);
		if(nRet != 0)
		{
             LOG4C ((LOG_FATAL, " Invalid TLV"));
             return -1;
		}
		//LOG4C ((LOG_FATAL, "aatag[%d][%s][i]", nTag,aczValue,i));

		

		/*插入链表*/
		pstMsgFldCfg = PKG_GetTheFieldConfig( epstMsgFldCfgAry, nTag);
		if( pstMsgFldCfg == NULL )
        {
          LOG4C ((LOG_FATAL, " cann't find the field %d at node %d %s,but continue",
                  nTag, eiNodeId, pczExecFileName));
          return -1;
        }
		/* all the field is charactor */
		memset( aczTmp, 0, sizeof(aczTmp) );
		for( j=0; j<nValueLen; j++ )
		{
			if( pstMsgFldCfg->iFldType == UDT_STRING )
			{
				sprintf( aczTmp, "%s%c", aczTmp, aczValue[j]&0xff );
			}
			else
			{
				sprintf( aczTmp, "%s%02X ", aczTmp, aczValue[j]&0xff );
			}
		}
		LOG4C(( LOG_DEBUG, "FIELD[%03d][LEN=%03d][%s]", nTag, nValueLen, aczTmp ));
		nRet = UDO_AddField( opfsAddr, nTag, pstMsgFldCfg->iFldType, aczValue, nValueLen );
        if( nRet != 0 )
        {
           LOG4C ((LOG_FATAL, " add the field %d value error[%d]\n", nTag, nRet));
           return nRet;
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

