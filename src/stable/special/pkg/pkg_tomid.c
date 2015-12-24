/*
**  file: pkg_tomid.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include "pkg/pkg_tomid.h"
#include "getStrAlignNums.h"
#include "log.h"

extern int nestedLevel;
extern int posInStruct;
extern int structStartMoveLen;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];

FILE *fpPkgLog = NULL;
FILE * pkgOpenPktLog (char * epczFileName);

/*
** 函数功能: 取拆包函数的交易代码的0号函数
**
** 函数名:   PKG_GetToMidTranCode0
**
** 输入参数: char * epcOrgMsg                 // 源报文
**           int    eiOrgMsgLen               // 源报文长度 
**           int    eiNodeId                  // 节点号
**           int    eiMsgDscrb                // 报文描述符
**           MsgFldCfgAry * epstMsgFldCfgAry  // 域空间起始地址及域内记录数
**      
** 输出参数: char ** opczTranCode             // 交易代码
**         
** 返回值: int      // 0成功,其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
*/
int PKG_GetToMidTranCode0( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry *epstMsgFldCfgAry ,  
                           char *opczTranCode )
{
   int i, j, iVal, iRet, iCrtNum, iWrgNum, iFixFldNum;
   int iType, iSeq, iPos;
   int iSepMoveLen, iMoveLen, iMsgLeftLen;
   char aczSepSymbol[PKG_SEP_SYMBOL_LEN];
   NodeTranCfg stNodeTranCfg;
   FldCfgValNode stFldCfgValNode ;
   MsgFldCfgAry  stMsgFldCfgAry2;
   PKG_DataValue stDataValue;

   assert( epcOrgMsg != NULL );
   assert ( epstMsgFldCfgAry != NULL ) ;

   memset( &stDataValue, 0x00, sizeof( PKG_DataValue ) );

   memset( aczSepSymbol, 0x00, sizeof(aczSepSymbol) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(stFldCfgValNode) );
   memset( &stMsgFldCfgAry2 , 0x00, sizeof(stMsgFldCfgAry2 ) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d]\n", 
                   iRet, eiNodeId)); 
      return iRet;
   }

   /* 判断报文是否为机构体 */ 
   iToMidIsSt = stNodeTranCfg.iToMidIsSt;
   iToMsgIsSt = stNodeTranCfg.iToMsgIsSt;

   iCrtNum = PKG_GetNumsOfFld( stNodeTranCfg.aczCrtTranCodeType );
   iWrgNum = PKG_GetNumsOfFld( stNodeTranCfg.aczWrgTranCodeType ); 
   if( (eiMsgDscrb > 0 && iCrtNum < eiMsgDscrb) || 
       (eiMsgDscrb < 0 && iWrgNum < eiMsgDscrb) )
   {
      LOG4C ((LOG_FATAL,
                  "The Input 'eiMsgDscrb:[%d]' (in PKG_GetToMidTranCode0) Has Overflowed! NodeId:[%d]\n",
                   eiMsgDscrb, eiNodeId));
      return PKG_MSGDSCRB_ERR;
   }

   if( eiMsgDscrb > 0 ) /* 正确的交易 */
   {
      iType = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeType, eiMsgDscrb );
   } 
   else /* 错误的交易 */
   {
      iType = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeType, 
                                 abs(eiMsgDscrb) );
   }

   switch( iType ) 
   {
   case PKG_NO_TRAN_CODE: /* 没有交易代码的情况 */
      strcpy( opczTranCode, "-100" );
      break;
   case PKG_TRAN_CODE_POS_FIX:  /* 偏移量固定 */
      if( eiMsgDscrb > 0 )
      {
         iPos = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodePos, 
                                   eiMsgDscrb );
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeSeq, 
                                   eiMsgDscrb );
      }
      else
      {
         iPos = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodePos, 
                                   abs(eiMsgDscrb) );
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeSeq, 
                                   abs(eiMsgDscrb) );
      }

      /*该函数不申请空间,只是将stMsgFldCfgAry2的pstMsgFldCfg指向配置信息数组*/
      iRet = PKG_GetInfoByFldSeq ( epstMsgFldCfgAry, stNodeTranCfg.iMsgAppId,
                                   iSeq, &stMsgFldCfgAry2 );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_GetInfoByFldSeq (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d], Seq:[%d]\n",
                      iRet, eiNodeId, iSeq));
         return iRet;
      }

      iRet = PKG_GetFldCfgValNode ( &stMsgFldCfgAry2,
                                    stNodeTranCfg.iMsgAppId, iSeq,
                                    1, &stFldCfgValNode );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_GetFldCfgValNode (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d], Seq:[%d] iSepFldSeq:[1]\n",
                     iRet, eiNodeId, iSeq));
         return iRet;
      }

      iMoveLen    = iPos; 
      iMsgLeftLen = eiOrgMsgLen - iPos;

      iRet = PKG_GetSepFldContentFromMsg( epcOrgMsg+iMoveLen, &iMoveLen, 
                                          &iMsgLeftLen, 
                                          &stFldCfgValNode, 
                                          &stDataValue,
                                          &iSepMoveLen );
      if( iRet != PKG_SUCC )
      { 
         LOG4C ((LOG_FATAL,
                     "PKG_GetSepFldContentFromMsg (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d], MsgDscrb:[%d], Sep:[%d], SepSeq:[%d]\n",
                      iRet, eiNodeId, eiMsgDscrb, iSeq, 
                     stFldCfgValNode.iSepFldSeq));
         return iRet;
      }

      /* 判断是否越界 */
      if( iMoveLen > eiOrgMsgLen )
      {
         LOG4C ((LOG_FATAL,
                     "The Configuration Of Pos:[%d] Or Seq:[%d] in Table nodetrancfg Has Error[%d]! So That Msg Pos Has Out Of Msgs Length!\n",
                      iPos, iSeq, PKG_POS_OVERFLOW));
         return PKG_POS_OVERFLOW;
      }

      switch( stFldCfgValNode.iFldType )
      {
      case UDT_STRING:  
         if( stDataValue.iLen > PKG_TRANCODE_LEN )
         {
            strncpy( opczTranCode, stDataValue.nData.pcVal, PKG_TRANCODE_LEN );
         }
         else
         {
            strcpy( opczTranCode, stDataValue.nData.pcVal );
         }
         free( stDataValue.nData.pcVal );
         break;
      case UDT_SHORT:  
         sprintf( opczTranCode, "%d", stDataValue.nData.shVal ); 
         break;
      case UDT_INT:  
         sprintf( opczTranCode, "%d", stDataValue.nData.iVal ); 
         break;
      case UDT_LONG: 
         sprintf( opczTranCode, "%ld", stDataValue.nData.lVal ); 
         break;
      case UDT_DOUBLE:   
         sprintf( opczTranCode, "%.0lf", stDataValue.nData.dVal ); 
         break;
      case UDT_FLOAT: 
         sprintf( opczTranCode, "%.0f", stDataValue.nData.fVal ); 
         break;
      } 
      break;
   case PKG_8583_TRAN_CODE:  /* 8583 */
      break;
   case PKG_SEP_SYMBOL_TRAN_CODE:  /* 分割符 */
      if( eiMsgDscrb > 0 )
      {
         iFixFldNum = PKG_GetNumsOfFld( stNodeTranCfg.aczCrtMsgFixFld );
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeSeq, 
                                   eiMsgDscrb );
      }  
      else
      {
         iFixFldNum = PKG_GetNumsOfFld( stNodeTranCfg.aczWrgMsgFixFld );
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeSeq, 
                                   abs(eiMsgDscrb) );
      }  

      for( i = 1, iPos = 0; i <= iFixFldNum; i++ )
      {
         if( eiMsgDscrb > 0 )
         {
            iVal = PKG_GetValFromFld( stNodeTranCfg.aczCrtMsgFixFld, i );
         }
         else
         {
            iVal = PKG_GetValFromFld( stNodeTranCfg.aczWrgMsgFixFld, i );
         }

        /*该函数不申请空间,只是将stMsgFldCfgAry2的pstMsgFldCfg指向配置信息数组*/
         iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry,
                                     stNodeTranCfg.iMsgAppId, iVal,
                                     &stMsgFldCfgAry2 );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetInfoByFldSeq (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d], Seq:[%d]\n",
                        iRet, eiNodeId, iVal));
            return iRet;
         }

         iRet = PKG_GetFldCfgValNode( &stMsgFldCfgAry2 ,
                                      stNodeTranCfg.iMsgAppId, iVal,
                                      1, &stFldCfgValNode );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetFldCfgValNode (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d], Seq:[%d] iSepFldSeq:[1]\n",
                        iRet, eiNodeId, iVal));
            return iRet;
         }

         /* 判断是否属于2型分割符字段 */
         if( stFldCfgValNode.iSepFldFlag == PKG_SEPSYMBOL2_FLD )
         {
            memcpy( stFldCfgValNode.aczFldAlign, "R", 1 );
            *(stFldCfgValNode.aczFldAlign+1) = '\0';
         }

         /* 取得分割符 */
         iRet = PKG_XToA( stFldCfgValNode.aczSepSymbol, aczSepSymbol );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_XToA (in PKG_GetToMidTranCode0) Has Error[%d]! The Input Is SepSymbol:[%s]! MsgAppId:[%d], Seq:[%d], SepSeq:[%d]\n",
                         iRet, 
                        stFldCfgValNode.aczSepSymbol, 
                        stFldCfgValNode.iMsgAppId, iSeq, 
                        stFldCfgValNode.iSepFldSeq));
         }

         if( stFldCfgValNode.aczFldAlign[0] == 'L' ||
             stFldCfgValNode.aczFldAlign[0] == 'l' )
         {
            iPos += strlen( aczSepSymbol );
         } 

         j = 0;
         switch( stFldCfgValNode.iFldType )
         {
         case UDT_STRING:
            while ( strcmp( epcOrgMsg+iPos+j, 
                    strstr(epcOrgMsg+iPos, aczSepSymbol) ) != 0 
                  && j < eiOrgMsgLen - iPos )
            {
               j++;
            }
            break;
         case UDT_SHORT:  j = sizeof(short); break;
         case UDT_INT:    j = sizeof(int); break;
         case UDT_LONG:   j = sizeof(long); break;
         case UDT_DOUBLE: j = sizeof(double); break;
         case UDT_FLOAT:  j = sizeof(float); break;
         default:
            LOG4C ((LOG_FATAL,
                        "The Input 'iFldType:[%d]' (in PKG_GetToMidTranCode) Has Error[%d]! MsgAppId:[%d], Seq:[%d], SepSeq:[%d]\n", 
                         stFldCfgValNode.iFldType, 
                        PKG_DATATYPE_INVALID, stFldCfgValNode.iMsgAppId,
                        iSeq, stFldCfgValNode.iSepFldSeq )); 
            break; 
         }

         /* 判断该与是否为交易代码域段 */
         if( iVal == iSeq )  
         {
            iRet = PKG_GetValFromBuf( epcOrgMsg+iPos, 
                                      stFldCfgValNode.iFldType, j,
                                      &stDataValue );
            if( iRet != PKG_SUCC )
            { 
               LOG4C ((LOG_FATAL,
                           "PKG_GetValFromBuf (in PKG_GetToMidTranCode0) Has Error[%d]! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], FldType:[%d]\n",
                            iRet, 
                           stFldCfgValNode.iMsgAppId, iSeq, 
                           stFldCfgValNode.iSepFldFlag, 
                           stFldCfgValNode.iFldType)); 
               return iRet;
            }

            /* 判断是否越界 */
            if( (stDataValue.iLen+iPos) > eiOrgMsgLen )
            {
               LOG4C ((LOG_FATAL,
                           "The Configuration Of Pos:[%d] Or Seq:[%d] in Table nodetrancfg Has Error[%d]! So That Msg Pos Has Out Of Msgs Length!\n",
                            iPos, iSeq, 
                           PKG_POS_OVERFLOW));
               return PKG_POS_OVERFLOW;
            }

            switch( stFldCfgValNode.iFldType )
            {
            case UDT_STRING:
               if( stDataValue.iLen > PKG_TRANCODE_LEN )
               {
                  strncpy( opczTranCode, stDataValue.nData.pcVal,
                           PKG_TRANCODE_LEN );
               }
               else
               {
                  strcpy( opczTranCode, stDataValue.nData.pcVal );
               }
               free( stDataValue.nData.pcVal );
               break;
            case UDT_SHORT:  
               sprintf( opczTranCode, "%d", stDataValue.nData.shVal ); 
               break;
            case UDT_INT:  
               sprintf( opczTranCode, "%d", stDataValue.nData.iVal ); 
               break;
            case UDT_LONG: 
               sprintf( opczTranCode, "%ld", stDataValue.nData.lVal ); 
               break;
            case UDT_DOUBLE:   
               sprintf( opczTranCode, "%.0lf", stDataValue.nData.dVal ); 
               break;
            case UDT_FLOAT: 
               sprintf( opczTranCode, "%.0f", stDataValue.nData.fVal ); 
               break;
            } 

            return PKG_SUCC;
         } 

         iPos += j;

         if( stFldCfgValNode.aczFldAlign[0] == 'R' ||
             stFldCfgValNode.aczFldAlign[0] == 'r' ) 
         {
            iPos += strlen( aczSepSymbol );
         }
      }
      break;      
   }

   return PKG_SUCC;
}

/*
** 函数功能: 取拆包函数的交易代码的0号函数
**
** 函数名:   PKG_GetToMidTranCode1
**
** 输入参数: char * epcOrgMsg                 // 源报文
**           int    eiOrgMsgLen               // 源报文长度 
**           int    eiNodeId                  // 节点号
**           int    eiMsgDscrb                // 报文描述符
**           MsgFldCfgAry * epstMsgFldCfgAry  // 域空间起始地址及域内记录数
**      
** 输出参数: char ** opczTranCode             // 交易代码
**         
** 返回值: int      // 0成功,其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
** Note:just only for shanghai ccb new generation message package,by yinzhuhua
** Date:2001/11/14
*/
int PKG_GetToMidTranCode1( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry *epstMsgFldCfgAry ,  
                           char *opczTranCode )
{
   long lTranCodeOffset;
   int  iTranCodeLen;

   assert( epcOrgMsg        != NULL );
   assert( opczTranCode     != NULL );

   lTranCodeOffset = 28;
   iTranCodeLen    = 6;
   memcpy( opczTranCode, epcOrgMsg + lTranCodeOffset, (size_t)iTranCodeLen );

   return PKG_SUCC;
}

/*
**  函数功能: 将源报文转化为中间信息链表的0号函数
**
**  函数名称: PKG_ToMid0
**
**  输入参数: char * epcOrgMsg                // 源报文
**            int    eiOrgMsgLen              // 源报文的长度
**            int    iConnectId               // 连接号
**            int    eiNodeId                 // 节点号
**            int    eiMsgDscrb               // 报文描述符
**            char * epczTranCode             // 交易代码
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr                    // 中间信息链表
**
**  返回值:   int  0成功, 其他失败
*/
int  PKG_ToMid0( char * epcOrgMsg, int eiOrgMsgLen, 
                 int eiNodeId, char *epczTranCode, int eiMsgDscrb, 
		 MsgFldCfgAry *epstMsgFldCfgAry ,
                 UDP opfsAddr )
{
   int iRet;
   int i,m,n,x,y,s,t;
   int iSeq, iRepSeq;
   int iMoveLen = 0;
   int iMsgLeftLen = eiOrgMsgLen;
   int iRepTimesSeqNums, iRepTimesNumNums, iRepTypeNums, iRepTimesVal,
       iTranFldNums, iRepFldNums;
   int iRepTimesSeq[PKG_REP_PARA_LEN], iRepTimesNum[PKG_REP_PARA_LEN], 
       iRepType[PKG_REP_PARA_LEN]; 
   char aczRepFld[PKG_TRAN_FLD_LEN];
   TranFldCfg stTranFldCfg;
   NodeTranCfg stNodeTranCfg;
   /** 假定报文组成不超过200个域 **/
   int iTranFlds[1200];

   assert( epcOrgMsg != NULL ); 
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( opfsAddr != NULL );



   nestedLevel = -1;
   iToMidIsSt = 0;
   memset (alignStructNums, 0, sizeof (alignStructNums));
   memset (structNums,      0, sizeof (structNums));

   memset( aczRepFld, 0x00, PKG_TRAN_FLD_LEN );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
       return iRet;
   }

   /* 根据节点号,交易代码,报文描述符,读表TranFldCfg,取得对应的纪录 */
   iRet = PKG_GetInfoFromTranFldCfg( eiNodeId, epczTranCode, 
                                     eiMsgDscrb, &stTranFldCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "根据交易码%s取节点%d报文MsgDscrb:[%d]出错\n",
                  epczTranCode, eiNodeId, eiMsgDscrb));
      return iRet;
   }

   /* 将重复次数域段编号组成中的值一一对应到数组iRepTimesSeq中 */
   iRepTimesSeqNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepTimesSeq );

   for( m = 0; m < iRepTimesSeqNums; m++ )
   {
      iRepTimesSeq[m] = PKG_GetValFromFld( stTranFldCfg.aczRepTimesSeq, m+1 );
   }
   m = 0;

   /* 将重复次数序号组成中的值一一对应到数组iRepTimesNum中 */
   iRepTimesNumNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepTimesNum );
   for( m = 0; m < iRepTimesNumNums; m++ )
   {
      iRepTimesNum[m] = PKG_GetValFromFld( stTranFldCfg.aczRepTimesNum, m+1 );
   }
   m = 0;

   /* 将重复域的重复类型组成中的值一一对应到数组iRepType中 */
   iRepTypeNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepType );
   for( n = 0; n < iRepTypeNums; n++ )
   {
      iRepType[n] = PKG_GetValFromFld( stTranFldCfg.aczRepType, n+1 );
   }
   n = 0;

   /* 计算交易域段中的域号总数(包括-1) */
   iTranFldNums = PKG_GetNumsOfFld( stTranFldCfg.aczTranFld );

   getTranFlds (iTranFldNums, stTranFldCfg.aczTranFld, iTranFlds);

   getStructsAlignNums (iTranFldNums, iTranFlds, 
                        stNodeTranCfg.iMsgAppId,
                        epstMsgFldCfgAry,
                        alignStructNums);

   /* 对每一个域进行操作, 将对应值插入链表 */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      iSeq = iTranFlds[i-1];

      if( iSeq != PKG_REP_SYMBOL )  /* 非重复域 */ 
      {
         iRet = PKG_AddFldContentToLST( epcOrgMsg+iMoveLen, &iMoveLen, 
                                        &iMsgLeftLen, stNodeTranCfg.iMsgAppId,
                                        iSeq, epstMsgFldCfgAry, opfsAddr );
         if( iRet == PKG_IS_BITMAP )
         {
            continue;
         }

         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_AddFldContentToLST (in PKG_ToMid0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], SepSeq:[%d]\n",
                         iRet, eiNodeId, epczTranCode, 
                        eiMsgDscrb, iSeq, i));
            return iRet;
         }
      }
      else /* 重复域 */
      {
         /* 取得重复域中的编号集合 */
         iRet = PKG_GetRepFld( stTranFldCfg.aczTranFld, i, aczRepFld );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetRepFld (in PKG_ToMid0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], The Start Num Of TranFld is [%d]\n",
                         iRet, eiNodeId, epczTranCode,
                        eiMsgDscrb, i+1));
            return iRet;
         }

         iRepFldNums = PKG_GetNumsOfFld( aczRepFld );
         i += iRepFldNums + 1;   /* 将i值对应到下一个-1所在的位置 */

         /* 计算该区域的重复次数 */
         iRet = PKG_GetRepTimesVal( opfsAddr, n, 
                                    stTranFldCfg.aczRepTimesSeq,
                                    stTranFldCfg.aczRepTimesNum,
                                    &iRepTimesVal );
         if( iRet != PKG_SUCC )
         {
	    LOG4C ((LOG_FATAL,
		       "PKG_GetRepTimesVal (in PKG_ToMid0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], RepRegion:[%d] Of The Msg\n",
		        iRet, eiNodeId, epczTranCode,
		       eiMsgDscrb, n+1));
            return iRet;
         }


         if( iRepType[n] == PKG_CYC_REP )  /* 循环重复 */
         {
            for( x = 1; x <= iRepTimesVal; x++ )
            {
               for( y = 1; y <= iRepFldNums; y++ )
               {
                  iRepSeq = PKG_GetValFromFld( aczRepFld, y ); 
    
                  /* 将该重复域的信息加入中间信息链表 */
                  iRet = PKG_AddFldContentToLST( epcOrgMsg+iMoveLen, 
                                                 &iMoveLen, 
                                                 &iMsgLeftLen,
                                                 stNodeTranCfg.iMsgAppId,
                                                 iRepSeq, epstMsgFldCfgAry,
                                                 opfsAddr );
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_AddFldContentToLST (in PKG_ToMid0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], SepSeq:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, i));
                     return iRet;
                  }
               }
            }
         }
         else /* 单域循环 */
         {
            for( s = 1; s <= iRepFldNums; s++ )
            {
               for( t = 1; t <= iRepTimesVal; t++ )
               {
                  iRepSeq = PKG_GetValFromFld( aczRepFld, s ); 

                  /* 将该重复域的信息加入中间信息链表 */
                  iRet = PKG_AddFldContentToLST( epcOrgMsg+iMoveLen, 
                                                 &iMoveLen, 
                                                 &iMsgLeftLen,
                                                 stNodeTranCfg.iMsgAppId,
                                                 iRepSeq,
                                                 epstMsgFldCfgAry,
                                                 opfsAddr );
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_AddFldContentToLST (in PKG_ToMid0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], SepSeq:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, i));
                     return iRet;
                  }
               }
            }
         }
         n++;
      }
   }

   /* 判断输入报文的长度是否超过了实际长度 */
   if( iMoveLen < eiOrgMsgLen )
   {
      LOG4C ((LOG_WARN,
                  "The Input Org Msg Length Is Longer Than It Should Be! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                   eiNodeId, epczTranCode, eiMsgDscrb));
      return PKG_LENGTH_OVER;
   }

   return PKG_SUCC;
}

/*
**  函数功能: 将源报文转化为中间信息链表的1号函数(针对标准8583)
**
**  函数名称: PKG_ToMid1
**
**  输入参数: char * epcOrgMsg// 源报文
**            int    eiOrgMsgLen// 源报文的长度
**            int    iConnectId// 连接号
**            int    eiNodeId// 节点号
**            int    eiMsgDscrb// 报文描述符
**            char * epczTranCode             // 交易代码(可以填*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr          // 中间信息报文
**
**  返回值:   int  0成功, 其他失败
*/
int  PKG_ToMid1( char * epcOrgMsg, int eiOrgMsgLen,
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb,
		 MsgFldCfgAry *epstMsgFldCfgAry ,
                 UDP opfsAddr )
{
   int i, iBitNums, iRet;
   int iMoveLen = 0;
   int iMsgLeftLen = eiOrgMsgLen;
   NodeTranCfg stNodeTranCfg;
   char aczBitMap[PKG_BITMAP_MAX+1];
   char aczHead[PKG_MSGTYPE_LEN + 1];
   MsgFldCfgAry stMsgFldCfgAry;
   int  iFldSeq;
   int  iFldType;
   char aczTemp[64];

   assert( epcOrgMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( opfsAddr != NULL );

   memset( aczHead, 0x00, sizeof(aczHead) );
   memset( aczBitMap, 0x00, sizeof(aczBitMap) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
       return iRet;
   }

   memset (&stMsgFldCfgAry, 0, sizeof (stMsgFldCfgAry));
   iRet = PKG_GetInfoFromMsgFldCfg (stNodeTranCfg.iMsgAppId, &stMsgFldCfgAry);
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromMsgFldCfg Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
       return iRet;
   }

   memcpy( aczHead, epcOrgMsg, PKG_MSGTYPE_LEN );
   iMoveLen += PKG_MSGTYPE_LEN;
   iRet = UDO_AddField( opfsAddr, 0, UDT_STRING, aczHead, PKG_MSGTYPE_LEN);
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "UDO_AddField(in PKG_ToMid1) Has Error[%d]\n",
                   iRet));
      free (stMsgFldCfgAry.pstMsgFldCfg);
      return iRet;
   }

   /* 判断BitMap包含的位数 */
   iRet = PKG_IsBitMapSet( (unsigned char*)(epcOrgMsg+iMoveLen), 1 );
   if( iRet == PKG_SUCC )       /* 128位 */
   {
      iBitNums = 128;
      memcpy( aczBitMap, epcOrgMsg+iMoveLen, 16 );
      iMoveLen += 16;
      UDO_AddField( opfsAddr, 1, UDT_STRING, aczBitMap, 16);
   }
   else if( iRet == PKG_FAIL )  /* 64位 */
   {
      iBitNums = 64;
      memcpy( aczBitMap, epcOrgMsg+iMoveLen, 8 );
      iMoveLen += 8;
      UDO_AddField( opfsAddr, 1, UDT_STRING, aczBitMap, 8);
   }


   /* 判断报文中是否存在该域, 计算指针偏移量 */
   for( i = 2; i <= iBitNums; i++ )
   {
      iRet = PKG_IsBitMapSet( (unsigned char*)aczBitMap, i );
      if( iRet == PKG_SUCC )
      {
         iRet = PKG_AddFldContentToLST( epcOrgMsg+iMoveLen, &iMoveLen, 
                                       &iMsgLeftLen, stNodeTranCfg.iMsgAppId, i,
				       &stMsgFldCfgAry, opfsAddr );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_AddFldContentToLST(in PKG_ToMid1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d]\n",
                         iRet, eiNodeId, 
                        epczTranCode, eiMsgDscrb, i));
            free (stMsgFldCfgAry.pstMsgFldCfg);
            return iRet;
         }
      }
      else
      {
         continue;
      }
   }

   free (stMsgFldCfgAry.pstMsgFldCfg);

   /** 增加报文配置中存在，但在8583报文中不存在的域 */
   memset (aczTemp, 0, sizeof (aczTemp));
   for( i = 0; i < epstMsgFldCfgAry->iNum; i++ ) {
      iFldSeq = epstMsgFldCfgAry->pstMsgFldCfg[i].iFldSeq;
      iFldType = epstMsgFldCfgAry->pstMsgFldCfg[i].iFldType;

      iRet = PKG_IsBitMapSet ((unsigned char*)aczBitMap, i);
      if( iRet != PKG_SUCC ) {
          UDO_AddField( opfsAddr, iFldSeq, iFldType, aczTemp, 0);
      }
   }
   /*********************************************/

   /* 判断输入报文的长度是否超过了实际长度 */
   if( iMoveLen < eiOrgMsgLen )
   {
      LOG4C ((LOG_FATAL, 
                  "The Input Org Msg Length Is Longer Than It Should Be! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                   eiNodeId, epczTranCode, eiMsgDscrb));
      return PKG_LENGTH_OVER;
   }


   return PKG_SUCC;
}


/*
**  函数功能: 将源报文转化为中间信息链表的1号函数(针对网银)
**
**  函数名称: PKG_ToMidWY
**
**  输入参数: char * epcOrgMsg// 源报文
**            int    eiOrgMsgLen// 源报文的长度
**            int    iConnectId// 连接号
**            int    eiNodeId// 节点号
**            int    eiMsgDscrb// 报文描述符
**            char * epczTranCode             // 交易代码(可以填*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                             及域记录数
**  输出参数: UDP opfsAddr          // 中间信息报文
**
**  返回值:   int  0成功, 其他失败
*/
/****
int  PKG_ToMidWY( char * epcOrgMsg, int eiOrgMsgLen,
                  int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                  MsgFldCfgAry *epstMsgFldCfgAry ,
                  UDP opfsAddr )
{
   int i, iBitNums, iRet;
   int iMoveLen = 0;
   int iMsgLeftLen = eiOrgMsgLen;
   NodeTranCfg stNodeTranCfg;

   assert( epcOrgMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( opfsAddr != NULL );

   iRet = PKG_ToMid_WY( epcOrgMsg, epstMsgFldCfgAry , opfsAddr );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, 
                  "PKG_ToMid_WY Error, iRet[%d]",
                   iRet));
      return iRet;
   }

   return PKG_SUCC;
}
***/

FILE * pkgOpenPktLog (char * epczFileName)
{
    FILE *fp;
    char aczDateTime[20];
    char aczFileSuffix[60];
    char aczFile[128];

    char aczBakFileName[256];
    time_t tNow;
    struct stat stStat;
    struct tm   *pstTime;

    memset (aczDateTime, 0, sizeof( aczDateTime ) );
    TOOL_GetSystemYYYYMMDD( aczDateTime );

    memset (aczFileSuffix, 0, sizeof (aczFileSuffix));
    strcpy (aczFileSuffix, LOG4C_SUFFIX());

    memset (aczFile, 0, sizeof (aczFile));
    if (strlen (aczFileSuffix) != 0)
    {
        sprintf (aczFile, "%s/%s.%s.%s",
                 LOG4C_DIR(), epczFileName, aczDateTime, aczFileSuffix);
    }
    else
    {
        sprintf (aczFile, "%s/%s.%s",
                 LOG4C_DIR(), epczFileName, aczDateTime);
    }

    stat(aczFile, &stStat);
    if( stStat.st_size > 100000000 ) /* 100M */
    {
       time( &tNow );
       pstTime = localtime( &tNow );
       sprintf( aczBakFileName, "%s.%02d%02d%02dbak",
                aczFile, pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
       //rename( aczFile, aczBakFileName );
       fp = fopen (aczFile, "w");
       if (fp != NULL) fclose (fp);
    }

    fp = fopen (aczFile, "a+");

    return fp;
}

/*
** end of file: pkg_tomid.c
*/
