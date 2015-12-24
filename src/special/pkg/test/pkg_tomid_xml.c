/*
**  file: pkg_tomsg.c
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
#include "pkg/pkg_tomid_xml.h"
#include "pkg/pkg_tomidbase.h"
#include "tempadd.h"
#include "log.h"

static char nestedName[50];

#ifdef __cplusplus
extern "C" {
#endif

static int PKG_GetRepTimesVal_XML (char *epcOrgMsg, int eiMsgAppId, int eiSeq, 
                                   MsgFldCfgAry *epstMsgFldCfgAry,int *oiRepTm);

static int PKG_GetXmlItem (char *epcOrgMsg, char *epczItem, int iFldType, 
                           char *opczValue, int *oiOffset, int *oiLen);

#ifdef __cplusplus
}
#endif

static int iNestedLevel = 0;

int  PKG_ToMid_XML( char * epcOrgMsg, int eiOrgMsgLen, 
                    int eiNodeId, char *epczTranCode, int eiMsgDscrb, 
   		    MsgFldCfgAry *epstMsgFldCfgAry ,
                    UDP opfsAddr )
{
   int iRet, iLen;
   int i, m, n, x, y;
   int iSeq, iRepSeq;
   int iMoveLen = 0, iMsgLeftLen = 0, iSepMoveLen;
   int iMoveLen_xml = 0;
   int iOffset = 0;
   int iFldType ;
   int iRepTimesSeqNums, iRepTimesNumNums, iRepTypeNums, iRepTimesVal,
       iTranFldNums, iRepFldNums;
   int iRepTimesSeq[PKG_REP_PARA_LEN], iRepTimesNum[PKG_REP_PARA_LEN], 
       iRepType[PKG_REP_PARA_LEN]; 
   char aczRepFld[PKG_TRAN_FLD_LEN];
   int iRepTimes;
   TranFldCfg stTranFldCfg;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfg stMsgFldCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   PKG_DataValue stDataValue;

   char aczItem[ PKG_SELECT_LEN ];
   char aczValue[ PKG_SELECT_LEN ];

   iMsgLeftLen = eiOrgMsgLen;

   memset( aczRepFld, 0x00, PKG_TRAN_FLD_LEN );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stMsgFldCfg, 0x00, sizeof( stMsgFldCfg ) );

   memset( aczItem, 0x00, sizeof( aczItem ) );
   memset( aczValue, 0x00, sizeof( aczValue ) );

   iNestedLevel = 0;

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg Has Error[%d]! NodeId:[%d]\n",
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
                  epczTranCode, eiNodeId, eiMsgDscrb ));
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

   /* 对每一个域进行操作, 将对应值插入链表 */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );
      if( iSeq != PKG_REP_SYMBOL )  /* 非重复域 */ 
      {
          if (iSeq >= NESTED_XML_BEGINTAG_E && iSeq <= NESTED_XML_BEGINTAG_S)
          {
              iNestedLevel += 1;
              continue;
          }
          if (iSeq >= NESTED_XML_ENDTAG_E && iSeq <= NESTED_XML_ENDTAG_S)
          {
              iNestedLevel -= 1;
              continue;
          }

         iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry, stNodeTranCfg.iMsgAppId,
                                     iSeq, &stMsgFldCfgAry );
         if( iRet < 0 )
         {
            LOG4C ((LOG_FATAL, "PKG_GetInfoByFldSeq "
                        "failed, err code[%d]",  iRet ));
            return -1;
         }

         stMsgFldCfg = *( stMsgFldCfgAry.pstMsgFldCfg );
         strcpy( aczItem, stMsgFldCfg.aczFldEngName );
         iFldType = stMsgFldCfg.iFldType;

         if (iNestedLevel != 0)
         {
             iRet = PKG_GetXmlItem( epcOrgMsg + iMoveLen, aczItem, iFldType, aczValue,
                                    &iOffset, &iLen );
             if( iRet != PKG_SUCC )
             {
                 continue;
             }

             iRet = UDO_AddField( opfsAddr, iSeq, iFldType, aczValue, iLen );
             if( iRet != PKG_SUCC )
             {
                LOG4C ((LOG_FATAL,
                        "PKG_AddFldContentToLST "
                        "Has Error[%d]! NodeId:[%d], TranCode:[%s], "
                        "MsgDscrb:[%d], Seq:[%d], SepSeq:[%d]\n",
                         iRet, eiNodeId, epczTranCode, 
                         eiMsgDscrb, iSeq, i ));
                return iRet;
             }
         }
         else
         {
             memset (&stDataValue, 0, sizeof (stDataValue));
             iRet = PKG_GetSepFldContentFromMsg( epcOrgMsg + iMoveLen,
                                                 &iMoveLen,
                                                 &iMsgLeftLen, &stMsgFldCfg,
                                                 &stDataValue, &iSepMoveLen );
             iRet = PKG_AddSepFldContentToLST( iSeq, 0, stMsgFldCfg.iFldType,
                                                &stDataValue, opfsAddr );
         }
      }
      else /* 重复域 */
      {
         /* 取得重复域中的编号集合 */
         iRet = PKG_GetRepFld( stTranFldCfg.aczTranFld, i, aczRepFld );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetRepFld "
                        "Has Error[%d]! NodeId:[%d], TranCode:[%s], "
                        "MsgDscrb:[%d], The Start Num Of TranFld is [%d]\n",
                         iRet, eiNodeId, epczTranCode,
                        eiMsgDscrb, i+1 ));
            return iRet;
         }

         iRepFldNums = PKG_GetNumsOfFld( aczRepFld );
         i += iRepFldNums + 1 + 1;   /* 将i值对应到下一个-1所在的位置 */

         for( x = 1; x <= iRepFldNums; x++ )
         {
            iRepSeq = PKG_GetValFromFld( aczRepFld, x ); 

            if (iSeq >= NESTED_XML_BEGINTAG_E && iSeq <= NESTED_XML_BEGINTAG_S)
            {
                  iNestedLevel += 1;
                  continue;
            }
            if (iSeq >= NESTED_XML_ENDTAG_E && iSeq <= NESTED_XML_ENDTAG_S)
            {
                  iNestedLevel -= 1;
                  continue;
            }

            iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry, 
                                        stNodeTranCfg.iMsgAppId, iRepSeq, 
                                        &stMsgFldCfgAry );
            if( iRet < 0 )
            {
               LOG4C ((LOG_FATAL, "PKG_GetInfoByFldSeq"
                           " failed, err code[%d]",  iRet ));
               return -1;
            }

            stMsgFldCfg = *( stMsgFldCfgAry.pstMsgFldCfg );
            strcpy( aczItem, stMsgFldCfg.aczFldEngName );
            iFldType = stMsgFldCfg.iFldType;

            iMoveLen_xml = iMoveLen;
            iRepTimes = 0;
            while (iMoveLen_xml  < eiOrgMsgLen)
            {
               iOffset = 0;
               /* 将该重复域的信息加入中间信息链表 */
               iRet = PKG_GetXmlItem( epcOrgMsg+iMoveLen_xml, aczItem, iFldType,
                                      aczValue, &iOffset, &iLen );
               if( iRet != PKG_SUCC )
               {
                   break;
               }

               iRet = UDO_AddField(opfsAddr, iRepSeq,iFldType,
                                   aczValue, iLen);
               if( iRet < 0 )
               {
                  LOG4C ((LOG_FATAL, "UDO_AddField "
                              "failed, err code[%d]", iRet ));
                  return iRet;
               }

               iRepTimes += 1;
               iMoveLen_xml += iOffset;
            }
            {
            char aczTmp[10];
            sprintf (aczTmp, "%d", iRepTimes);
            UDO_ACField (opfsAddr, iRepTimesSeq[n], UDT_STRING, aczTmp, strlen (aczTmp));
            }
         }
         n++;
      }
   }
   return PKG_SUCC;
}

static int  PKG_GetRepTimesVal_XML (char *epcOrgMsg, int eiMsgAppId, int eiSeq, 
                                    MsgFldCfgAry *epstMsgFldCfgAry,int *oiRepTm)
{
   int iRet;
   int iLen;
   int iRepTm;
   int iOffset;
   int iFldType;
   char aczItem[100];
   union 
   {
       short  shVal;
       int    iVal;
       long   lVal;
       float  fVal;
       double dVal;
       char   aczValue[ PKG_SELECT_LEN ];
   } tmpVal;
   
   MsgFldCfg stMsgFldCfg;
   MsgFldCfgAry stMsgFldCfgAry;

   memset( &stMsgFldCfg, 0x00, sizeof( MsgFldCfg ) );
   memset( aczItem, 0x00, sizeof( aczItem ) );
   memset( tmpVal.aczValue, 0x00, sizeof( tmpVal.aczValue ) );

   iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry, eiMsgAppId, eiSeq, 
                               &stMsgFldCfgAry );
   if( iRet < 0 )
   {
      LOG4C ((LOG_FATAL, ", PKG_GetRepTimesVal_XML "
                  "PKG_GetInfoByFldSeq failed, err code[%d]",
                   iRet ));
      return -1;
   }

   stMsgFldCfg = *(stMsgFldCfgAry.pstMsgFldCfg);
   strcpy( aczItem, stMsgFldCfg.aczFldEngName );
   iFldType = stMsgFldCfg.iFldType;

   iRet = PKG_GetXmlItem( epcOrgMsg, aczItem, iFldType, tmpVal.aczValue, 
                          &iOffset, &iLen );
   if( iRet < 0 )
   {
      LOG4C ((LOG_FATAL, ", PKG_GetRepTimesVal_XML "
                  "PKG_GetXmlItem failed, err code[%d]", 
                  iRet ));
      return -1;
   }

   switch (iFldType)
   {
   case UDT_STRING:
   case UDT_BIN:
       iRepTm = atoi (tmpVal.aczValue);
       break;
   case UDT_SHORT:
       iRepTm = (int)tmpVal.shVal;
       break;
   case UDT_INT:
       iRepTm = tmpVal.iVal;
       break;
   case UDT_LONG:
       iRepTm = (int)tmpVal.lVal;
       break;
   case UDT_DOUBLE:
       iRepTm = (int)tmpVal.dVal;
       break;
   case UDT_FLOAT:
       iRepTm = (int)tmpVal.fVal;
       break;
   }
   *oiRepTm = iRepTm;

   return 0;
}

static int PKG_GetXmlItem (char *epcOrgMsg, char *epczItem, int iFldType,
                           char *opczValue, int *oiOffset, int *oiLen)
{
   int    iRet, iLen;
   short  shVal;
   int    iVal;
   long   lVal;
   float  fVal;
   double dVal;

   char aczItem[ PKG_SELECT_LEN ];
   char aczStartItem[ PKG_SELECT_LEN ];
   char aczEndItem[ PKG_SELECT_LEN ];

   char *pStart=NULL, *pEnd=NULL;
   char *pTmpStart=NULL, *pTmpEnd=NULL;

   sprintf( aczStartItem, "<%s>", epczItem );
   sprintf( aczEndItem, "</%s>", epczItem );

   pStart = strstr( epcOrgMsg, aczStartItem );
   pEnd = strstr( epcOrgMsg, aczEndItem );
   if( pStart == NULL || pEnd == NULL )
   {
      LOG4C ((LOG_FATAL, ", 请求报文中没有此标签"
                  "%s",  epczItem ));
      return -1;
   }
   pTmpStart = strchr( pStart, '>' );
   pTmpEnd = strchr( pEnd, '>' );

   if( pStart == NULL || pEnd == NULL || pTmpStart == NULL || pStart >= pEnd ||
       pStart >= pTmpStart || pTmpStart >= pEnd )
   {
      LOG4C ((LOG_FATAL, "invalid XML format, please"
                  " check the package message"));
      return -1;
   }

   if( pEnd >= pTmpEnd )
   {
      LOG4C ((LOG_FATAL, ", invalid XML format, please"
                  " check the package message"));
      return -1;
   }

   pTmpStart++;
   pTmpEnd++;

   iLen = pEnd - pTmpStart;
   switch (iFldType)
   {
   case UDT_STRING:
   case UDT_BIN:
       memcpy( opczValue, pTmpStart, iLen );
       *oiLen = iLen;
       break;
   case UDT_SHORT:
       shVal = (short)atoi(pTmpStart);     
       memcpy (opczValue, &shVal, sizeof (shVal));
       *oiLen = sizeof (shVal);
       break;
   case UDT_INT:
       iVal = atoi(pTmpStart);     
       memcpy (opczValue, &iVal, sizeof (iVal));
       *oiLen = sizeof (iVal);
       break;
   case UDT_LONG:
       lVal = atol(pTmpStart);     
       memcpy (opczValue, &lVal, sizeof (lVal));
       *oiLen = sizeof (lVal);
       break;
   case UDT_DOUBLE:
       dVal = (double)atof(pTmpStart);     
       memcpy (opczValue, &dVal, sizeof (dVal));
       *oiLen = sizeof (dVal);
       break;
   case UDT_FLOAT:
       fVal = atof(pTmpStart);     
       memcpy (opczValue, &fVal, sizeof (fVal));
       *oiLen = sizeof (fVal);
       break;
   }

   *oiOffset = pTmpEnd - epcOrgMsg;

   return 0;
}
