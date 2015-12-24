/*
**  file: pkg_tomsg_tms.c
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
#include "pkg/pkg_tomsg_tms.h"
#include "log.h"
#include "tempadd.h"

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];
extern int FLOWCODE_ID;

int PKG_CreateTmsField( MsgFldCfgAry *pstMsgFldCfgAry, int iFldId,
                            UDP epfAddr, char *pczFldContent )
{
   MsgFldCfg *pstMsgFldCfg;
   char aczFieldContent[8000], aczLen[20], aczFormat[20], aczFldDef[10];
   int iLen;
   int iLenLen=0;

   pstMsgFldCfg = (MsgFldCfg *)PKG_GetTheFieldConfig( pstMsgFldCfgAry, iFldId );

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
      {
         sprintf( aczFormat, "%c0%dd", '%', pstMsgFldCfg->iLenFldLen );
          /* 计算长度域的长度 */
         if( pstMsgFldCfg->iLenFldType == UDT_STRING )
         {
            sprintf( aczFormat, "%c0%dd", '%', pstMsgFldCfg->iLenFldLen );
            sprintf( aczLen, aczFormat, iLen );
            memcpy( pczFldContent, aczLen, pstMsgFldCfg->iLenFldLen );
         }
         else
         {
            if( pstMsgFldCfg->iLenFldLen == 2 )
            {
                aczLen[0]=iLen/256;
                aczLen[1]=iLen%256;
                iLenLen=2;
            }
            else
            {
                aczLen[0]=iLen%256;
                iLenLen=1;
            }
            memcpy( pczFldContent, aczLen, pstMsgFldCfg->iLenFldLen );
         }
         memcpy( pczFldContent + pstMsgFldCfg->iLenFldLen, aczFieldContent, iLen );
         iLen += iLenLen; 
      }
      return iLen;
   default:
      LOG4C ((LOG_FATAL, "Invalid field type %d\n",
                pstMsgFldCfg->iSepFldSeq));
      return -1;
   }
}

int PKG_ToMsg_Tms( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   int iRet=0;
   TranFldCfg stTranFldCfg;
   FldCfgValNode stFldCfgValNode ;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   char aczFldContent[8000];
   int iTranFldNums=0;
   int iTranFlds[1024];
   int i=0;
   int iSeq=0;
   int iFldLen=0;
   int iMsgPos=0;

   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
   memset( iTranFlds, 0, sizeof(iTranFlds) );
   memset (alignStructNums, 0, sizeof (alignStructNums));
   memset (structNums,      0, sizeof (structNums));

   *opiOutMsgLen = 0;

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

   /* 对每一个域进行操作, 将对应值插入链表 */
   for( i = 0; i < iTranFldNums; i++ )
   {
      iSeq = iTranFlds[i];
      memset( aczFldContent, 0, sizeof(aczFldContent) );
      iFldLen = PKG_CreateTmsField( epstMsgFldCfgAry, iSeq,
                                        epfsAddr, aczFldContent );
      LOG4C(( LOG_DEBUG, "[%d]FIELD[%03d][LEN=%03d][%s]", iMsgPos, iSeq, iFldLen, aczFldContent ));
      memcpy( opcOutMsg + iMsgPos, aczFldContent, iFldLen );
      iMsgPos += iFldLen;
   }
   *opiOutMsgLen  = iMsgPos;
   return PKG_SUCC;
}
