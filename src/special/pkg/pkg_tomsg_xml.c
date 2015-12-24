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
#include "pkg/pkg_tomsg_xml.h"
#include "tempadd.h"
#include "log.h"

static char nestedName[50];

#ifdef __cplusplus
extern "C" {
#endif
static int  PKG_WriteFldContentToMsg_XML( char * opcMsgPos, int * eopiMoveLen, 
				   int eiMsgAppId, int eiSeq, int eiRepNum,
				   MsgFldCfgAry *epstMsgFldCfgAry,
				   UDP epfsAddr );

static int PKG_WriteXmlItemToMsg( char *opcMsgPos, int *eopiMoveLen, 
                           MsgFldCfg stFldCfgValNode, char *pczVal, int iValLen,
                           short shVal, int iVal, long lVal, double dVal, 
                           float fVal );

#ifdef __cplusplus
}
#endif

static int  iNestedLevel = 0;

int PKG_ToMsg_XML (char * opcOutMsg, int *opiOutMsgLen, 
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

   /** 增加重复次数为0的处理 **/
   int iRepFieldId;

   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( epfsAddr != NULL );
   
#ifdef DEBUG
   LOG4C ((LOG_DEBUG, " Enter PKG_ToMsg0" ));
#endif
   memset( aczRepFld, 0x00, PKG_TRAN_FLD_LEN );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );

   iNestedLevel = 0;

   /*strcpy( opcOutMsg, "<?xml version=\"1.0\" encoding=\"GB2312\"?>" );*/
   iMoveLen = strlen( opcOutMsg );
   *opiOutMsgLen = 0;

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg  Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId ));
       return iRet;
   }

   /* 根据节点号,交易代码,报文描述符,读表TranFldCfg,取得对应的纪录 */
   iRet = PKG_GetInfoFromTranFldCfg( eiNodeId, epczTranCode, 
                                     eiMsgDscrb, &stTranFldCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "PKG_GetInfoFromTranFldCfg Has "
                "Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                 iRet, eiNodeId, epczTranCode, eiMsgDscrb ));
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

   /* 对每一个域进行操作, 将对应值插入链表 */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      memset (nestedName, 0, sizeof (nestedName));
      iSeq = PKG_GetValFromFld_Nested (stTranFldCfg.aczTranFld, i, nestedName);

#ifdef DEBUG
      LOG4C ((LOG_DEBUG,
                " Before GetValFromFld, iTranFldNums %d, i %d, iSeq %d",
                 iTranFldNums, i, iSeq ));
#endif

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

#ifdef DEBUG
         LOG4C ((LOG_DEBUG, " Before WriteFldContentToMsg,\niSeq[%d], iSeq"
                   "Num[%d]", iSeq, iSeqNum ));
#endif
         /* 将该域的内容写入输出报文 */ 
         iRet = PKG_WriteFldContentToMsg_XML( opcOutMsg+iMoveLen, &iMoveLen,
                                          stNodeTranCfg.iMsgAppId, iSeq,
                                          iSeqNum, epstMsgFldCfgAry, 
					  epfsAddr ); 
#ifdef DEBUG
TOOL_Log(( LOG_DEBUG, " After WriteFldContentToMsg,\niSeq[%d], iSeq"
         "Num[%d]", iSeq, iSeqNum ));
#endif
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL, "PKG_WriteFldContentToMsg "
                      "Has Error[%d] When Getting Content From RepTim"
                      "esFld! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:"
                      "[%d], Num:[%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, iSeq, iSeqNum ));
            return iRet;
         } 
      }
      else /* 重复域 */
      {
         /* 取得重复域中的编号集合 */
         iRet = PKG_GetRepFld_Nested( stTranFldCfg.aczTranFld, i, aczRepFld );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                      "PKG_GetRepFld Has Error[%d]!"
                      " NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], The Start "
                      "Num Of TranFld Is [%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, i+1 ));
            return iRet;
         }

         iRepFldNums = PKG_GetNumsOfFld( aczRepFld );
         i += iRepFldNums + 1;   /* 将i值对应到下一个-1所在的位置 */

         /* 计算该重复区域的重复次数 */
         iSeq = PKG_GetValFromFld( aczRepFld, 1 );

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
                  memset (nestedName, 0, sizeof (nestedName));
                  iSeq = PKG_GetValFromFld_Nested (aczRepFld, y, nestedName);

                  /* 将该重复域的信息写入输出报文 */
                  iRet = PKG_WriteFldContentToMsg_XML( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId,
                                                   iSeq, x-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr ); 
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, x-1 ));
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
                  memset (nestedName, 0, sizeof (nestedName));
                  iSeq = PKG_GetValFromFld_Nested (aczRepFld, s, nestedName);
    
                  /* 将该重复域的信息写入输出报文 */
                  iRet = PKG_WriteFldContentToMsg_XML( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId, 
                                                   iSeq, t-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr );
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, t-1 ));
                     return iRet;
                  } 
               }
            }
         }
         n++;
      }
   }

   *opiOutMsgLen = iMoveLen;
   return PKG_SUCC;
}


static int  PKG_WriteFldContentToMsg_XML( char * opcMsgPos, int * eopiMoveLen, 
                                   int eiMsgAppId, int eiSeq, int eiRepNum,
                                   MsgFldCfgAry *epstMsgFldCfgAry,
                                   UDP epfsAddr ) 
{
   int  i, iRet, iDataType;
   int  iSepMoveLen = 0;    /* 分域偏移量 */
   char * pczVal = NULL;
   int  iVal, iValLen;
   short shVal;
   long lVal;
   float fVal;
   double dVal;
   FldCfgValNode stFldCfgValNode ;
   MsgFldCfgAry stMsgFldCfgAry;

   assert( opcMsgPos != NULL );
   assert( epfsAddr != NULL );
   assert( epstMsgFldCfgAry != NULL );

   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );

#ifdef DEBUG
   TOOL_Trace( GENL_DEBUG, "pkg_dbg.log",
               " eiSeq[%d],eiRepNum[%d]",
                eiSeq, eiRepNum );
#endif

   if (eiSeq >= NESTED_XML_BEGINTAG_E && eiSeq <= NESTED_XML_BEGINTAG_S)
   {
       memcpy( opcMsgPos, "<", 1 );
       memcpy( opcMsgPos+1, nestedName, strlen(nestedName) );
       memcpy( opcMsgPos+1+strlen(nestedName), ">", 1 );
       *eopiMoveLen += strlen(nestedName) + 2;

       iNestedLevel += 1;

       return PKG_SUCC;
   } 

   if (eiSeq >= NESTED_XML_ENDTAG_E && eiSeq <= NESTED_XML_ENDTAG_S)
   {

       memcpy( opcMsgPos, "</", 2 );
       memcpy( opcMsgPos+2, nestedName, strlen(nestedName) );
       memcpy( opcMsgPos+2+strlen(nestedName), ">", 1 );
       *eopiMoveLen += strlen(nestedName) + 3;

       iNestedLevel -= 1;

       return PKG_SUCC;
   }

   iRet = PKG_GetInfoByFldSeq ( epstMsgFldCfgAry, eiMsgAppId,
                                eiSeq, &stMsgFldCfgAry );  
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_WriteFldContentToMsg Has Error[%d], "
                  "There Is No FldCfgNode---Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, eiSeq ));
      return PKG_NO_FLDCFGNODE;
   }

   for( i = 1; i <= stMsgFldCfgAry.iNum; i++ )
   {
      iRet = PKG_GetFldCfgValNode( &stMsgFldCfgAry,
                                   eiMsgAppId, eiSeq, i, &stFldCfgValNode ); 
      if ( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                    "PKG_GetFldCfgValNode Has Error[%d], "
                    "There Is No FldCfgNode---Seq:[%d], SepFldSeq [%d] \n",
                      PKG_NO_FLDCFGNODE, eiSeq, i ));
         return PKG_NO_FLDCFGNODE;
      }

      /* 判断eiSeq域中的第i个分域的类型 */
      iDataType = stFldCfgValNode.iFldType;

      /* 从链表中取得eiSeq域中的第i个分域的第eiRepNum次重复内容 */
      if( stFldCfgValNode.iSepFldFlag != PKG_BITMAP_FLD )
      {
         iRet = PKG_GetFldFromLST( epfsAddr, eiSeq, i, eiRepNum, iDataType,
                                   &pczVal, &iValLen, &shVal, &iVal, &lVal, 
                                   &dVal, &fVal );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetFldFromLST Has Error[%d]! "
                        "MsgAppId:[%d], Seq:[%d], SepSeq:[%d], Num:[%d], "
                        "FldType:[%d]\n", iRet, 
                       stFldCfgValNode.iMsgAppId, eiSeq, i,eiRepNum,iDataType));
            return iRet;
         }
      }

      /* 将分域的内容写入输出报文 */ 
      if (iNestedLevel != 0)
      {
          PUB_StrTrim(pczVal,' ');
          iValLen = strlen( pczVal);

          iRet = PKG_WriteXmlItemToMsg( opcMsgPos, eopiMoveLen, stFldCfgValNode, 
                                        pczVal, iValLen, shVal, iVal, lVal, dVal, 
                                        fVal );
      }
      else
      {
          PUB_StrTrim(pczVal,' ');
          iValLen = strlen( pczVal);

          iRet = PKG_WriteSepFldContentToMsg( opcMsgPos, eopiMoveLen,
                                              stFldCfgValNode,
                                              pczVal, iValLen, shVal, iVal, lVal,
                                              dVal, fVal, &iSepMoveLen );
      }

      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_WriteSepFldContentToMsg Has Error[%d]! At "
                     "MsgAppId:[%d], Seq:[%d], SepSeq:[%d], Num:[%d]\n", 
                      iRet, stFldCfgValNode.iMsgAppId,
                     eiSeq, i, eiRepNum ));
         return iRet;
      }

      if( iDataType == UDT_STRING )
      {
         free( pczVal );
      }
   }

#ifdef DEBUG
   TOOL_Trace( GENL_DEBUG, "pkg_dbg.log",
               " After Loop",
               __LINE__, __FILE__ );
#endif

   return PKG_SUCC;
}

static int PKG_WriteXmlItemToMsg( char *opcMsgPos, int *eopiMoveLen, 
                           MsgFldCfg stFldCfgValNode, char *pczVal, int iValLen,
                           short shVal, int iVal, long lVal, double dVal, 
                           float fVal )
{
   int iRet, iLen;
   char *pStart=NULL, *pEnd=NULL;
   char *pczItem=NULL;
   char strTemp[24];
   int  strLength;

   assert( opcMsgPos != NULL );

   pczItem = stFldCfgValNode.aczFldEngName;

   switch( stFldCfgValNode.iFldType )
   {
      case UDT_STRING:
           iValLen = strlen (pczVal);
           memcpy( opcMsgPos, "<", 1 );
           memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
           memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

           memcpy( opcMsgPos+2+strlen( pczItem ), pczVal, iValLen);

           memcpy( opcMsgPos+2+strlen(pczItem)+iValLen, "</", 2);
           memcpy( opcMsgPos+4+strlen(pczItem)+iValLen, pczItem, 
                    strlen( pczItem ) );
           memcpy( opcMsgPos+4+strlen(pczItem)*2+iValLen, ">", 1 );
           *eopiMoveLen += 5+strlen(pczItem)*2+iValLen;
         break;

      case UDT_SHORT:
         memcpy( opcMsgPos, "<", 1 );
         memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
         memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

         /**
         memcpy( opcMsgPos+2+strlen( pczItem ), &shVal, sizeof(short) );
         memcpy( opcMsgPos+2+strlen(pczItem)+sizeof(short), "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+sizeof(short), pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+sizeof(short), ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+sizeof(short);
         ***/

         strLength = sprintf (strTemp, "%d", shVal);
         memcpy( opcMsgPos+2+strlen( pczItem ), strTemp, strLength);
         memcpy( opcMsgPos+2+strlen(pczItem)+strLength, "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+strLength, pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+strLength, ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+strLength;

         /***/         
         break;

      case UDT_INT:
         memcpy( opcMsgPos, "<", 1 );
         memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
         memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

         /***
         memcpy( opcMsgPos+2+strlen( pczItem ), &iVal, sizeof(int) );
         memcpy( opcMsgPos+2+strlen(pczItem)+sizeof(int), "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+sizeof(int), pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+sizeof(int), ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+sizeof(int);
         ***/

         strLength = sprintf (strTemp, "%d", iVal);
         memcpy( opcMsgPos+2+strlen( pczItem ), strTemp, strLength);
         memcpy( opcMsgPos+2+strlen(pczItem)+strLength, "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+strLength, pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+strLength, ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+strLength;
         break;

      case UDT_LONG:
         memcpy( opcMsgPos, "<", 1 );
         memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
         memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

         /**
         memcpy( opcMsgPos+2+strlen( pczItem ), &lVal, sizeof(long) );
         memcpy( opcMsgPos+2+strlen(pczItem)+sizeof(long), "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+sizeof(long), pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+sizeof(long), ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+sizeof(long);
         ***/

         strLength = sprintf (strTemp, "%ld", lVal);
         memcpy( opcMsgPos+2+strlen( pczItem ), strTemp, strLength);
         memcpy( opcMsgPos+2+strlen(pczItem)+strLength, "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+strLength, pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+strLength, ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+strLength;
         break;

      case UDT_FLOAT:
         memcpy( opcMsgPos, "<", 1 );
         memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
         memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

         /***
         memcpy( opcMsgPos+2+strlen( pczItem ), &fVal, sizeof(float) );
         memcpy( opcMsgPos+2+strlen(pczItem)+sizeof(float), "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+sizeof(float), pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+sizeof(float), ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+sizeof(float);
         ***/

         strLength = sprintf (strTemp, "%f", fVal);
         memcpy( opcMsgPos+2+strlen( pczItem ), strTemp, strLength);
         memcpy( opcMsgPos+2+strlen(pczItem)+strLength, "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+strLength, pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+strLength, ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+strLength;
         break;

      case UDT_DOUBLE:

         memcpy( opcMsgPos, "<", 1 );
         memcpy( opcMsgPos+1, pczItem, strlen( pczItem ) );
         memcpy( opcMsgPos+1+strlen( pczItem ), ">", 1 );

         /***
         memcpy( opcMsgPos+2+strlen( pczItem ), &dVal, sizeof(double) );
         memcpy( opcMsgPos+2+strlen(pczItem)+sizeof(double), "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+sizeof(double), pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+sizeof(double), ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+sizeof(double);
         ***/

         strLength = sprintf (strTemp, "%lf", dVal);
         memcpy( opcMsgPos+2+strlen( pczItem ), strTemp, strLength);
         memcpy( opcMsgPos+2+strlen(pczItem)+strLength, "</", 2);
         memcpy( opcMsgPos+4+strlen(pczItem)+strLength, pczItem, 
                 strlen( pczItem ) );
         memcpy( opcMsgPos+4+strlen(pczItem)*2+strLength, ">", 1 );
         *eopiMoveLen += 5+strlen(pczItem)*2+strLength;
         break;
   }

   return 0;
}

/*
** end of file: pkg_tomsg.c
*/
