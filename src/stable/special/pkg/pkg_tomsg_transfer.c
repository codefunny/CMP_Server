/*
**  file: pkg_tomsg_transfer.c
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
#include "pkg/pkg_tomsg_transfer.h"
#include "log.h"
#include "tempadd.h"

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];

int PKG_ToMsg_Transfer( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   int iRet=0;
   TranFldCfg stTranFldCfg;
   FldCfgValNode stFldCfgValNode ;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   char aczFldContent[2048];
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
      iFldLen = 0;
      memset( aczFldContent, 0, sizeof(aczFldContent) );
      UDO_GetField( epfsAddr, iSeq, 0, aczFldContent, &iFldLen );
      memcpy( opcOutMsg + iMsgPos, aczFldContent, iFldLen );
      LOG4C(( LOG_DEBUG, "[%d]FIELD[%03d][LEN=%03d][%s]", iMsgPos, iSeq, iFldLen, aczFldContent ));
      iMsgPos += iFldLen;
   }
   *opiOutMsgLen  = iMsgPos;
   return PKG_SUCC;
}
