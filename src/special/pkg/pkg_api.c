/*
**  file: pkg_api.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "pkg/pkg_api.h"
#include "tempadd.h"
#include "pkg/pkg_tomsg_xml.h"
#include "pkg/pkg_tomid_xml.h"
#include "pkg/pkg_tomsg_cup20.h"
#include "pkg/pkg_tomid_cup20.h"
#include "pkg/pkg_tomsg_bc.h"
#include "pkg/pkg_tomid_bc.h"
#include "pkg/pkg_tomid_tms.h"
#include "pkg/pkg_tomsg_tms.h"
#include "pkg/pkg_tomid_tms_limit.h"
#include "pkg/pkg_tomsg_tms_limit.h"
#include "pkg/pkg_tomid_transfer.h"
#include "pkg/pkg_tomsg_transfer.h"
#include "pkg/pkg_tomid_allinpay.h"
#include "pkg/pkg_tomsg_allinpay.h"
#include "pkg/pkg_tomid_tlv.h"
#include "pkg/pkg_tomsg_tlv.h"

#include "log.h"

int nestedLevel = 0;
int posInStruct = 0;
int structStartMoveLen = 0;

/* 定义全局变量iToMidIsSt, iToMsgIsSt */
int iToMidIsSt;         /* 输入报文是否为结构体 */
int iToMsgIsSt;         /* 输出报文是否为结构体 */

int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
int structNums[MAX_NESTED];

/* 拆包取交易代码函数指针结构值 */
PKG_GET_TOMID_TRANCODE stPkgGetToMidTranCode[] =
{
   { "PKG_GetToMidTranCode0", PKG_GetToMidTranCode0 },
   { "PKG_GetToMidTranCode1", PKG_GetToMidTranCode1 }
};

/* 拆包函数指针结构值 */
PKG_TOMID stPkgToMid[] = 
{
   { "PKG_ToMid0", PKG_ToMid0 },
   { "PKG_ToMid1", PKG_ToMid1 },
   { "PKG_ToMid_CUP20", PKG_ToMid_8583_1 },
   { "PKG_ToMid_XML", PKG_ToMid_XML },
   { "PKG_ToMid_TMS", PKG_ToMid_Tms },
   { "PKG_ToMid_LimitTms", PKG_ToMid_LimitTms },
   { "PKG_ToMid_Transfer", PKG_ToMid_Transfer },
   { "PKG_ToMid_BC", PKG_ToMid_8583_2 },
   { "PKG_ToMid_Allinpay", PKG_ToMid_8583_3 },
   { "PKG_ToMid_Tlv", PKG_ToMid_tlv },
};

/* 打包取交易代码函数指针结构值 */
PKG_GET_TOMSG_TRANCODE stPkgGetToMsgTranCode[] =
{
   { "PKG_GetToMsgTranCode0", PKG_GetToMsgTranCode0 }
};

/* 打包函数指针结构值 */
PKG_TOMSG stPkgToMsg[]=
{
   { "PKG_ToMsg0", PKG_ToMsg0 },
   { "PKG_ToMsg1", PKG_ToMsg1 },
   { "PKG_ToMsg_CUP20", PKG_ToMsg_8583_1 },
   { "PKG_ToMsg_XML", PKG_ToMsg_XML },
   { "PKG_ToMsg_TMS", PKG_ToMsg_Tms },
   { "PKG_ToMsg_LimitTms", PKG_ToMsg_LimitTms },
   { "PKG_ToMsg_Transfer", PKG_ToMsg_Transfer },   
   { "PKG_ToMsg_BC", PKG_ToMsg_8583_2 },
   { "PKG_ToMsg_Allinpay", PKG_ToMsg_8583_3 },   
   { "PKG_ToMsg_Tlv", PKG_ToMsg_tlv },   
};


/* 
**  函数功能: 根据节点号, 取得所有函数的Id号
**
**  函数名:   PKG_GetFunId
**
**  输入参数: int eiNodeId 
**
**  输出参数: int *opiToMidTranCodeId
**            int *opiToMsgTranCodeId
**            int *opiToMidId
**            int *opiToMsgId
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_GetFunId( int eiNodeId, 
                   int *opiToMidTranCodeId, int * opiToMsgTranCodeId,
                   int *opiToMidId, int *opiToMsgId )
{
   int  iRet;
   NodeTranCfg stNodeTranCfg;

   memset( &stNodeTranCfg, 0x00, sizeof(NodeTranCfg) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "PKG_GetInfoFromNodeCfg(in PKG_GetFunId) Has Error"
                "[%d]! eiNodeId:[%d]\n", iRet, eiNodeId));
      return iRet;
   }

   *opiToMidTranCodeId = stNodeTranCfg.iToMidTranCodeId;
   *opiToMsgTranCodeId = stNodeTranCfg.iToMsgTranCodeId;
   *opiToMidId         = stNodeTranCfg.iToMidId;
   *opiToMsgId         = stNodeTranCfg.iToMsgId;
   
   return PKG_SUCC;
}

/*
**  函数功能: 取得拆包函数交易代码
**
**  函数名:   PKG_GetToMidTranCode
**
**  输入参数: int eiFunId
**            char * epcOrgMsg
**            int eiOrgMsgLen
**            int eiNodeId
**            int eiMsgDscrb
**            MsgFldCfgAry * epstMsgFldCfgAry 
**            
**  输出参数: char ** opczTranCode
**
**  返回值: int  0 成功, 其他失败
**
**  操作说明: 函数内部分配了空间,请调用者释放
*/

int PKG_GetToMidTranCode( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen,
                          int eiNodeId, int eiMsgDscrb, 
                          MsgFldCfgAry *epstMsgFldCfgAry,
                          char *opczTranCode )
{
   int iRet;

   assert( epcOrgMsg != NULL );
   assert( epstMsgFldCfgAry != NULL );

   if( eiFunId == PKG_NO_FUN_ID )
   {
      LOG4C ((LOG_FATAL,
             "The NodeId:[%d], MsgDscrb:[%d] Doesn't Need Function:[PKG_GetToMidTranCode] To Get TranCode\n", 
             eiNodeId, eiMsgDscrb));
      return PKG_NO_TOMID_TRANCODE_ID;
   }
   else if( eiFunId < 0 )
   {
      LOG4C ((LOG_FATAL,
                  "The FunId:[%d] (in PKG_GetToMidTranCode) Has Error! NodeId:[%d], MsgDscrb:[%d]\n", 
                  eiFunId, eiNodeId, eiMsgDscrb ));
      return PKG_TOMID_TRANCODE_ID_ERR;
   }

   iRet = (*stPkgGetToMidTranCode[eiFunId].pfnFun)( epcOrgMsg, eiOrgMsgLen,
                                                    eiNodeId, eiMsgDscrb,
                                                    epstMsgFldCfgAry, 
                                                    opczTranCode );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "Pkg_GetToMidTranCode[%d]:[%s] Has Error[%d]! NodeId:[%d], MsgDscrb:[%d]\n",
                eiFunId, 
                stPkgGetToMidTranCode[eiFunId].pczFunName, iRet, 
                eiNodeId, eiMsgDscrb));
      return iRet;
   }

   return PKG_SUCC;
}

/*
**  函数说明: 将源报文转换为中间信息链表
**
**  函数名:   PKG_ToMid
**
**  输入参数: int  eiFunId
**            char * epcOrgMsg
**            int  eiOrgMsgLen
**            int  eiNodeId
**            char * epczTranCode
**            int  eiMsgDscrb
**            MsgFldCfgAry * epstMsgFldCfgAry
**            int  eiRowNum
**  
**  输出参数: 
**            UDP opfsAddr
**
**  返回值:  int  0 成功, 其他失败
*/

int  PKG_ToMid( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen, int eiNodeId,
                char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry, UDP opfsAddr )
{
   int iRet;

   assert( epcOrgMsg != NULL );
   assert( epczTranCode != NULL );
   assert ( epstMsgFldCfgAry != NULL );
   assert( opfsAddr != NULL );

   if( eiFunId < 0 )
   {
      LOG4C ((LOG_FATAL,
                  "The FunId:[%d] (in PKG_GetToMid) Has Error! NodeId:[%d], TranCode:[%d], MsgDscrb:[%d]\n", 
                  eiFunId, 
                  eiNodeId, epczTranCode, eiMsgDscrb));
      return PKG_TOMID_ID_ERR;
   }

   iRet = (*stPkgToMid[eiFunId].pfnFun)( epcOrgMsg, eiOrgMsgLen, eiNodeId,
                                         epczTranCode, eiMsgDscrb,
                                         epstMsgFldCfgAry, opfsAddr ); 
   if( iRet != PKG_SUCC &&  iRet != PKG_LENGTH_OVER  )
   {
      LOG4C ((LOG_FATAL,
                  "Pkg_ToMid[%d]:[%s] Has Error[%d]\n",
                  eiFunId, 
                  stPkgToMid[eiFunId].pczFunName, iRet));
      return iRet;
   }

   return PKG_SUCC;
}

/*
**  函数功能: 取得打包函数的交易代码
**
**  函数名:   PKG_GetToMsgTranCode
**
**  输入参数: int eiFunId
**            UDP epfsAddr
**            int eiNodeId
**            int eiMsgDscrb
**
**  输出参数: char ** opczTranCode
**
**  返回值: int  0 成功, 其他失败
**
**  操作说明: 函数内部分配了空间,请调用者释放
*/

int  PKG_GetToMsgTranCode( int eiFunId, UDP epfsAddr, int eiNodeId, 
                           int eiMsgDscrb, char ** opczTranCode )
{
   int iRet;

   assert( epfsAddr != NULL );

   if( eiFunId == PKG_NO_FUN_ID )
   {
      LOG4C ((LOG_FATAL,
                  "The NodeId:[%d], MsgDscrb:[%d] Doesn't Need Function:[PKG_GetToMsgTranCode] To Get TranCode\n", 
                  eiNodeId, eiMsgDscrb));
      return PKG_NO_TOMSG_TRANCODE_ID;
   }
   else if( eiFunId < 0 )
   {
      LOG4C ((LOG_FATAL,
                  "The FunId:[%d] (in PKG_GetToMsgTranCode) Has Error! NodeId:[%d], MsgDscrb:[%d]\n", 
                  eiFunId, eiNodeId, eiMsgDscrb));
      return PKG_TOMSG_TRANCODE_ID_ERR;
   }

   iRet = (*stPkgGetToMsgTranCode[eiFunId].pfnFun)( epfsAddr, eiNodeId,
                                                    eiMsgDscrb, opczTranCode );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "Pkg_GetToMsgTranCode[%d]:[%s] Has Error[%d]! NodeId:[%d], MsgDscrb:[%d]\n",
                eiFunId, 
                stPkgGetToMidTranCode[eiFunId].pczFunName, iRet, 
                eiNodeId, eiMsgDscrb));
      return iRet;
   }

   return PKG_SUCC;
}

/*
**  函数功能: 将中间信息链表转换为输出报文
** 
**  函数名:   PKG_ToMsg
**
**  输入参数: int eiFunId
**            int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**            MsgFldCfgAry *epstMsgFldCfgAry
**            UDP epfsAddr
**
** 输出参数: 
**            char * opcOutMsg
**            int * opiOutMsgLen
**
**  返回值: int  0 成功, 其他失败
*/

int  PKG_ToMsg( int eiFunId, char * opcOutMsg, int * opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
                MsgFldCfgAry *epstMsgFldCfgAry, UDP epfsAddr )
{
   int iRet;

   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epfsAddr != NULL ); 

   if( eiFunId < 0 )
   {
      LOG4C ((LOG_FATAL,
                  "The FunId:[%d] (in PKG_GetToMsg) Has Error! NodeId:[%d], TranCode:[%d], MsgDscrb:[%d]\n", 
                  eiFunId, 
                  eiNodeId, epczTranCode, eiMsgDscrb));
      return PKG_TOMSG_ID_ERR;
   }
   iRet = (*stPkgToMsg[eiFunId].pfnFun)( opcOutMsg, opiOutMsgLen, eiNodeId, 
                                         epczTranCode, eiMsgDscrb,
                                         epstMsgFldCfgAry, epfsAddr );
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "Pkg_GetToMsg[%d]:[%s] Has Error[%d]!\n",
                eiFunId, 
                stPkgToMsg[eiFunId].pczFunName, iRet));
      return iRet;
   }

   return PKG_SUCC;
}


/*
** end of file: pkg_api.c
*/
