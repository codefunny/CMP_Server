/*
**  file: pkg_tomsg_transfer.c
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
#include "pkg/pkg_tomid_transfer.h"
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
extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];

int PKG_ToMid_Transfer(  char * epcOrgMsg, int eiOrgMsgLen,
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
   char aczTmp[1024];
   int iTranFldNums=0;
   int iTranFlds[1024];
   int i=0,j=0;
   int iSeq=0;
   int iFldLen=0;
   int iMsgPos=0;
   MsgFldCfg *pstMsgFldCfg;
   int  nFlowNum=0;

   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
   memset( iTranFlds, 0, sizeof(iTranFlds) );
   memset (alignStructNums, 0, sizeof (alignStructNums));

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
      UDO_AddField( opfsAddr, iSeq, UDT_BIN, epcOrgMsg, eiOrgMsgLen );/*body*/
   }

   return PKG_SUCC;
}
