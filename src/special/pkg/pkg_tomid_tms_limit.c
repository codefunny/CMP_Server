/*
**  file: pkg_tomid_tms.c
**
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
#include "pkg/pkg_tomid_tms.h"
#include "log.h"
#include "tempadd.h"
#include "tool_base.h"

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];

int PKG_GetTmsLimitField( char *pczOrgMsg, int eiMsgLen, int *piCurrPos,
                             MsgFldCfg *pstMsgFldCfg, char *pczValue,
                             int *piValLen, int nFld )
{
   unsigned char aczLen[20];
   char cDefault;
   int  iLen,i,j;
   char ch=0;
   char aczTmp[20];
   char aczValue[3000];
   int  nNum=0;
   int  nTotalLen=0;

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
      memset( aczLen, 0, sizeof(aczLen) );
      memset( aczTmp, 0, sizeof(aczTmp) );
      *piValLen = 0;
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
              /*iLen = (aczLen[0] >> 4) * 10 + (aczLen[0] & 0x0f);*/
              iLen = aczLen[0];
          }
          else
          {
              /*
              iLen = (aczLen[0] >> 4) * 10 + (aczLen[0] & 0x0f);
              iLen = iLen*100 + (aczLen[1] >> 4) * 10 + (aczLen[1] & 0x0f);
              */
              iLen = aczLen[0]*256 + aczLen[1];
          }
      }
      else
      {
          iLen = atoi(aczLen);
      }

      if( (*piCurrPos + iLen) > eiMsgLen )
      {
         LOG4C ((LOG_FATAL, "PKG_LENGTH_SHORT, msg[%s],msglen[%d],fldlen[%d],offset[%d] ", pczOrgMsg + *piCurrPos, eiMsgLen, iLen, *piCurrPos));
         return PKG_LENGTH_SHORT;
      }
      memcpy( pczValue, pczOrgMsg + *piCurrPos + pstMsgFldCfg->iLenFldLen, iLen );
      *piCurrPos += pstMsgFldCfg->iLenFldLen + iLen;
      *piValLen = iLen;
      return 0;
   default:
      LOG4C ((LOG_FATAL, "Invalid field type %d", pstMsgFldCfg->iSepFldFlag));
      return -1;
   }
}

int PKG_ToMid_LimitTms(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr )
{
   int iRet=0;
   TranFldCfg stTranFldCfg;
   FldCfgValNode stFldCfgValNode ;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   char aczFldContent[1024];
   char aczTmp[3000];
   int iTranFldNums=0;
   int iTranFlds[1024];
   int i=0,j=0;
   int iSeq=0;
   int iFldLen=0;
   int iMsgPos=0;
   MsgFldCfg *pstMsgFldCfg;
   char aczDataBefore[1024];
   int iLenBefore=0;
   char aczFlowCode[1024];
   char aczFlowData[2000];
   int  nFlowNum=0;

   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
   memset( iTranFlds, 0, sizeof(iTranFlds) );
   memset (alignStructNums, 0, sizeof (alignStructNums));
   memset (structNums,      0, sizeof (structNums));

   memset( aczFlowCode, 0, sizeof(aczFlowCode) );
   memset( aczFlowData, 0, sizeof(aczFlowData) );

   UDO_AddField( opfsAddr, 31, UDT_BIN, epcOrgMsg, eiOrgMsgLen );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMidTran"
                   "Code0) Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
       return iRet;
   }

   /* 根据节点号,交易代码,报文描述符,读表TranFldCfg,取得对应的纪录 */
   iRet = PKG_GetInfoFromTranFldCfg( eiNodeId, epczTranCode,
                                     eiMsgDscrb, &stTranFldCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "PKG_GetInfoFromTranFldCfg (in PKG_ToMsg0) Has "
                "Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                 iRet, eiNodeId, epczTranCode, eiMsgDscrb));
      return iRet;
   }
   iTranFldNums = PKG_GetNumsOfFld( stTranFldCfg.aczTranFld );

   getTranFlds (iTranFldNums, stTranFldCfg.aczTranFld, iTranFlds);

   getStructsAlignNums (iTranFldNums, iTranFlds,
                        stNodeTranCfg.iMsgAppId,
                        epstMsgFldCfgAry,
                        alignStructNums);

   memset( aczDataBefore, 0, sizeof(aczDataBefore) );
   iLenBefore = 0;
   /* 从第二域开始对每一个域进行操作, 将对应值插入链表 */
   for( i = 1; i < iTranFldNums; i++ )
   {
      iSeq = iTranFlds[i];
      pstMsgFldCfg = (MsgFldCfg *)PKG_GetTheFieldConfig( epstMsgFldCfgAry, iSeq );
      if( pstMsgFldCfg == NULL )
      {
         LOG4C ((LOG_FATAL, " cann't find the field %d at node %d %s,but continue",
                  iSeq, eiNodeId, pczExecFileName));
         return -1;
      }
      memset( aczFldContent, 0, sizeof(aczFldContent) );
      iRet = PKG_GetTmsLimitField( epcOrgMsg, eiOrgMsgLen, &iMsgPos,
                         pstMsgFldCfg, aczFldContent, &iFldLen, iSeq );
      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " get the field %d value error at message position %d\n",
                  iSeq, iMsgPos));
         return iRet;
      }
      memset( aczTmp, 0, sizeof(aczTmp) );
      for( j=0; j<iFldLen; j++ )
      {
          sprintf( aczTmp, "%s%02X ", aczTmp, aczFldContent[j]&0xff );
      }
      LOG4C(( LOG_DEBUG, "[FIELD=%03d][LEN=%03d][%s]", iSeq,iFldLen, aczTmp ));

      iRet = UDO_AddField( opfsAddr, iSeq, pstMsgFldCfg->iFldType, aczFldContent, iFldLen );
      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " add the field %d value error[%d]\n",
                  iSeq, iRet));
         return iRet;
      }
   }

   return PKG_SUCC;
}
