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

#include "pkg/pkg_tomsg.h"
#include "getStrAlignNums.h"
#include "log.h"

extern int nestedLevel;
extern int posInStruct;
extern int structStartMoveLen;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];


/*
**  函数功能: 取打包函数交易代码的0号函数
**
**  函数说明: PKG_GetToMsgTranCode0
**
*  输入参数: UDPnepfsAddr 
**            int eiNodeId
**            int eiMsgDscrb
**            
**  输出参数: char ** opczTranCode
**
**  返回值:   int 0 成功, 其他失败
**
**  操作说明: 函数内部分配了空间, 请调用者注意释放 
*/
int  PKG_GetToMsgTranCode0( UDP epfsAddr, 
                            int eiNodeId, int eiMsgDscrb, 
                            char ** opczTranCode )
{
   int iRet, iCrtNum, iWrgNum, iDataType, iValLen;
   int iType, iSeq;
   int iVal;
   short shVal;
   long lVal;
   double dVal;
   float fVal;
   char * pczVal = NULL;
   NodeTranCfg stNodeTranCfg;

   assert( epfsAddr != NULL );

   memset( &stNodeTranCfg, 0x00, sizeof(NodeTranCfg) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMsgTranCode0) Has Error[%d]! NodeId:[%d]\n", 
                   iRet, eiNodeId)); 
      return iRet;
   }

   iCrtNum = PKG_GetNumsOfFld( stNodeTranCfg.aczCrtTranCodeType );
   iWrgNum = PKG_GetNumsOfFld( stNodeTranCfg.aczWrgTranCodeType ); 
   if( (eiMsgDscrb > 0 && iCrtNum < eiMsgDscrb) || 
       (eiMsgDscrb < 0 && iWrgNum < eiMsgDscrb) )
   {
      LOG4C ((LOG_FATAL,
                  "The Input 'eiMsgDscrb:[%d]' (in PKG_GetToMsgTranCode0) Has OverFlowed! NodeId:[%d]\n",
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

   if( iType == PKG_NO_TRAN_CODE ) /* 没有交易代码的情况 */
   {
      do
      {
         pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
      }while( pczVal == NULL );
      memset( pczVal, 0x00, PKG_TRAN_CODE_LEN + 1 );
      strcpy( pczVal, "-100" );
      *opczTranCode = pczVal;
   }
   else
   {
      if( eiMsgDscrb > 0 )
      {
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeSeq, 
                                   eiMsgDscrb );
      }
      else
      {
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeSeq, 
                                   abs(eiMsgDscrb) );
      }
   }

   /* 从域号为iSeq的域中取得交易代码 */
   iDataType = UDO_FieldType( epfsAddr, iSeq );

   iRet = PKG_GetFldFromLST( epfsAddr, iSeq, 1, 0, iDataType,
                             &pczVal, &iValLen, &shVal, &iVal, &lVal, 
                             &dVal, &fVal );
   if( iRet != PKG_SUCC )
   { 
      LOG4C ((LOG_FATAL,
                  "PKG_GetFldFromLST (in PKG_GetToMsgTranCode) Has Error[%d] When Getting TranCode From A LST! NodeId:[%d], MsgDscrb:[%d], Seq:[%d]\n",
                   iRet, eiNodeId, eiMsgDscrb, iSeq));
      return iRet;
   }

   switch( iDataType )
   {
      case UDT_STRING:
         *opczTranCode = pczVal;
         break;
      case UDT_SHORT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%d", shVal );
         *opczTranCode = pczVal;
         break;
      case UDT_INT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%d", iVal );
         *opczTranCode = pczVal;
         break;
      case UDT_LONG:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%ld", lVal );
         *opczTranCode = pczVal;
         break;
      case UDT_DOUBLE:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%.0lf", dVal );
         *opczTranCode = pczVal;
         break;
      case UDT_FLOAT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%.0f", fVal );
         *opczTranCode = pczVal;
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The 'iDataType:[%d]' Of TranCodeFld (in PKG_GetToMsgTranCode) Has Error[%d]! NodeId:[%d], MsgDscrb:[%d]\n",
                      iDataType, PKG_DATATYPE_INVALID, 
                     eiNodeId, eiMsgDscrb));
         return PKG_DATATYPE_INVALID;
   }

   return PKG_SUCC;
}

/*
**  函数功能: 将中间信息链表转化为输出报文的0号函数
**
**  函数说明: PKG_ToMsg0
**
**  输入参数: int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**            MsgFldCfgAry * epstMsgFldCfgAry
**            UDP  epfsAddr 
**
**  输出参数: char * opcOutMsg
**            int  * opiOutMsgLen
**
**  返回值:   int  0成功, 其他失败
*/

int PKG_ToMsg0( char * opcOutMsg, int *opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
                MsgFldCfgAry *epstMsgFldCfgAry , UDP epfsAddr )
{
   int iRet;
   int a=0,i,m,n,x,y,s,t;
   int iSeq, iSeqNum = 0;
   int iMoveLen = 0; /* 报文指针总偏移量 */
   int iRepTimesVal, iRepTimesSeqNums, iRepTimesNumNums, iRepTypeNums; 
   int iTranFldNums, iRepFldNums;
   int iRepTimesSeq[PKG_REP_PARA_LEN], iRepTimesNum[PKG_REP_PARA_LEN],
       iRepType[PKG_REP_PARA_LEN];
   int iSepSymbolLen;
   char aczRepFld[PKG_TRAN_FLD_LEN];
   TranFldCfg stTranFldCfg;
   FldCfgValNode stFldCfgValNode ;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   /** 假定报文组成不超过200个域 **/
   int iTranFlds[1024];

   /** 增加重复次数为0的处理 **/
   int iRepFieldId;

   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( epfsAddr != NULL );



   nestedLevel = -1;
   iToMsgIsSt = 0;
   memset (alignStructNums, 0, sizeof (alignStructNums));
   memset (structNums,      0, sizeof (structNums));

   memset (iTranFlds,       0, sizeof (iTranFlds));

   memset( aczRepFld, 0x00, PKG_TRAN_FLD_LEN );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );

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


   {
      char aczRepTimeFld[20];
      int i, len;
      strcpy( aczRepTimeFld, stTranFldCfg.aczRepTimesSeq );
      len = strlen(aczRepTimeFld);
      for( i = 0; i < len; i++ )
      {
         if( aczRepTimeFld[i] == ',' )
         {
            aczRepTimeFld[i] = 0;
            break;
         }
      }
      iRepFieldId = atoi(aczRepTimeFld);
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
         /* 判断该域号是否为重复域，根据重复次数序号得到iSepNum */
         if( iSeq != iRepTimesSeq[m] ) 
         {
            iSeqNum = 0;
         } 
         else
         {
            iSeqNum = 0;
            /* 判断在该重复域号之前是否由于其相同的域号 */
            for( a = 0; a < m; a++ )
            {
               if( iRepTimesSeq[a] == iSeq )
                  iSeqNum++;
            }
            m++;
         }

         /* 将该域的内容写入输出报文 */ 
         iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, &iMoveLen,
                                          stNodeTranCfg.iMsgAppId, iSeq,
                                          iSeqNum, epstMsgFldCfgAry, 
					  epfsAddr ); 
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL, "PKG_WriteFldContentToMsg (in PKG_"
                      "ToMsg0) Has Error[%d] When Getting Content From RepTim"
                      "esFld! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:"
                      "[%d], Num:[%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, iSeq, iSeqNum));
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
                      "PKG_GetRepFld (in PKG_ToMsg0) Has Error[%d]!"
                      " NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], The Start "
                      "Num Of TranFld Is [%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, i+1));
            return iRet;
         }

         iRepFldNums = PKG_GetNumsOfFld( aczRepFld );
         i += iRepFldNums + 1;   /* 将i值对应到下一个-1所在的位置 */

         /* 计算该重复区域的重复次数 */
         iSeq = PKG_GetValFromFld( aczRepFld, 1 );
/***替换为以下一行***
         iRepTimesVal = UDO_FieldOccur( epfsAddr, iSeq );
         if( iRepTimesVal == PKG_NO_FIELD )
         {
            LOG4C ((LOG_FATAL,
                        "UDO_FieldOccur (in PKG_ToMsg) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d]\n",
                         PKG_NO_FIELD, eiNodeId,
                        epczTranCode, eiMsgDscrb, iSeq));
            return PKG_NO_FIELD;
         }
*********************/

/**修改取重复次数域的内容 多余一个循环报文情况 **/
         {
         char aczVal[50];
         memset( aczVal, 0, sizeof(aczVal) );
         UDO_GetField( epfsAddr, iRepTimesSeq[n], 0, aczVal, 0 );
         iRepTimesVal = atoi(aczVal);
         }


         if( iRepType[n] == PKG_CYC_REP )  /* 循环重复 */
         {
            for( x = 1; x <= iRepTimesVal; x++ )
            {
               for( y = 1; y <= iRepFldNums; y++ )
               {
                  iSeq = PKG_GetValFromFld( aczRepFld, y ); 

                  /* 将该重复域的信息写入输出报文 */
                  iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId,
                                                   iSeq, x-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr ); 
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg (in PKG_ToMsg0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, x-1));
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
                  iSeq = PKG_GetValFromFld( aczRepFld, s ); 
    
                  /* 将该重复域的信息写入输出报文 */
                  iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId, 
                                                   iSeq, t-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr );
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg (in PKG_ToMsg0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, t-1));
                     return iRet;
                  } 
               }
            }
         }
         n++;
      }
   }


   if (iSeq < 0)
   {
       goto tomsg_end;
   }

   /* 如果该报文为2号分割符在右侧的报文, 以下处理最后一个域的情况 */
   iRet = PKG_GetInfoByFldSeq ( epstMsgFldCfgAry, stNodeTranCfg.iMsgAppId,
                                iSeq , &stMsgFldCfgAry ); 
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_ToMsg0  Has Error[%d], There Is No FldCfgNode --- Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, iSeq));
      return PKG_NO_FLDCFGNODE;
   }


   iRet = PKG_GetFldCfgValNode ( &stMsgFldCfgAry , 
                                 stNodeTranCfg.iMsgAppId,
                                 iSeq, stMsgFldCfgAry.iNum , &stFldCfgValNode );


   if( stFldCfgValNode.iSepFldFlag == PKG_SEPSYMBOL2_FLD ) 
   {
      iSepSymbolLen = (strlen(stFldCfgValNode.aczSepSymbol))/2; 
      /* 报文长度减去最后一个分割符的长度 */
      iMoveLen = iMoveLen - iSepSymbolLen;  
      memset(opcOutMsg+iMoveLen, 0x00, (size_t)iSepSymbolLen ); 
   }


   /* 如果输出报文是结构体, iMoveLen应该是4的倍数 */
   /* not use now ******
   if( iToMsgIsSt == PKG_IS_STRUCT )
   {
      iMoveLen += ( PKG_MOD - iMoveLen % PKG_MOD ) % PKG_MOD;
   }
   *********************/

tomsg_end:

   *opiOutMsgLen = iMoveLen;


   return PKG_SUCC;
}

/*
**  函数功能: 将中间信息链表转化为输出报文的1号函数(针对标准8583)
**
**  函数说明: PKG_ToMsg1
**
**  输入参数: int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**            MsgFldCfg * epstMsgFldCfg
**            int eiRowNum
**            UDP  epfsAddr
**
**  输出参数: char * opcOutMsg
**            int  * opiOutMsgLen
**
**  返回值:   int  0成功, 其他失败
*/

int PKG_ToMsg1( char * opcOutMsg, int *opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   int i, iRet, iSeq, iLen, iBitMapLen=0;
   int iMoveLen = 0; /* 报文指针总偏移量 */
   int iTranFldNums;
   char aczHead[PKG_MSGTYPE_LEN + 1];
   char aczBitMap[PKG_BITMAP_MAX + 1];
   TranFldCfg stTranFldCfg;
   NodeTranCfg stNodeTranCfg;

   assert( epstMsgFldCfgAry != NULL );
   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epfsAddr != NULL );

   memset( aczHead, 0x00, PKG_MSGTYPE_LEN+1 );
   memset( aczBitMap, 0x00, PKG_BITMAP_MAX+1 );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );

   *opiOutMsgLen = 0;

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
                  "PKG_GetInfoFromTranFldCfg(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                   iRet, eiNodeId, epczTranCode, 
                  eiMsgDscrb)); 
      return iRet;
   }

   /* 将报文类型域写入输出报文 */
   iRet = UDO_GetField( epfsAddr, 0, 0, aczHead, &iLen );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,  
                  "UDO_GetField(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n", 
                   iRet, eiNodeId, epczTranCode, 
                  eiMsgDscrb));
      return iRet;
   }
   
   /**
   {
      char c[2];
      c[0] = aczHead[2];
      c[1] = '\0';
 
      if (atoi (c) % 2 == 0)
          aczHead[2] += 1;
   }
   */

   memcpy( opcOutMsg, aczHead, PKG_MSGTYPE_LEN );
   iMoveLen += PKG_MSGTYPE_LEN;

   /* 计算交易域段中的域号总数(包括-1) */
   iTranFldNums = PKG_GetNumsOfFld( stTranFldCfg.aczTranFld );

   /* 计算输出报文BitMap的长度 */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );
      if( iSeq > 64 )
      {
         iRet = PKG_SetBitMap( (unsigned char *)aczBitMap, 1 );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_SetBitMap(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                         iRet, 
                        eiNodeId, epczTranCode, eiMsgDscrb));
            return iRet;
         }
         iBitMapLen = 16;
         break;
      }
   }
   iBitMapLen = (iBitMapLen == 16)?16:8;

   /* 生成输出报文的BitMap */
   for( i = 3; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );
      iRet = PKG_SetBitMap( (unsigned char *)aczBitMap, iSeq );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_SetBitMap(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",  iRet, 
                      eiNodeId, epczTranCode, eiMsgDscrb));

         return iRet;
      }
   }

   memcpy( opcOutMsg+iMoveLen, aczBitMap, (size_t)iBitMapLen );
   iMoveLen += iBitMapLen;


   /* 对每一个域进行操作, 将对应值插入链表 */
   for( i = 3; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );

      /* 将该域的内容写入输出报文 */
      iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, &iMoveLen,
                                       stNodeTranCfg.iMsgAppId, iSeq, 0,
                                       epstMsgFldCfgAry, 
				       epfsAddr );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_WriteFldContentToMsg(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d]\n", 
                     iRet, eiNodeId, epczTranCode,
                    eiMsgDscrb, iSeq));

         return iRet;
      }
   }

   *opiOutMsgLen = iMoveLen;

   return PKG_SUCC;
}

/*
** end of file: pkg_tomsg.c
*/
