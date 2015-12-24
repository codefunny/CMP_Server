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
#include "pkg/pkg_tomid_cup20.h"
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
int PKG_ToMid_8583_1(  char * epcOrgMsg, int eiOrgMsgLen,
                       int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                       MsgFldCfgAry *epstMsgFldCfgAry ,
                       UDP opfsAddr )
{
   char aczBitFlag[129];
   char aczField[1000];
   char aczTmp[3000];
   char aczAscii[2000];
   int  iFldId, iFldLen, iMsgPos = 0, iRet, iIdx, i;
   MsgFldCfg *pstMsgFldCfg;

   /*解析报文头*/

   UDO_AddField( opfsAddr, 1000, UDT_BIN, epcOrgMsg, eiOrgMsgLen );
   UDO_AddField( opfsAddr, 1003, UDT_BIN, epcOrgMsg, 6 );
   iMsgPos += 6;

   UDO_AddField( opfsAddr, 0, UDT_BIN, epcOrgMsg+iMsgPos, 2 );
   iMsgPos += 2;

   UDO_AddField( opfsAddr, 1, UDT_BIN, epcOrgMsg+iMsgPos, 8 );

   memset( aczBitFlag, 0, sizeof(aczBitFlag) );
   if( PKG_ChangeBitmapToFlag( epcOrgMsg+iMsgPos, 0, aczBitFlag ) == -1 )
   {
      LOG4C ((LOG_FATAL, " Invalid BITMAP"));
      return -1;
   }

   iMsgPos += 8;

   /**
    * split the message to udo according to the bitmap
    */
   for( iFldId = 1; iFldId < 64; iFldId++ )
   {
      if( aczBitFlag[iFldId] == '0' )  /** no field **/
      {
         continue;
      }
      /** has the field value **/
      pstMsgFldCfg = PKG_GetTheFieldConfig( epstMsgFldCfgAry, iFldId+1 );
      if( pstMsgFldCfg == NULL )
      {
         LOG4C ((LOG_FATAL, " cann't find the field %d at node %d %s,but continue",
                  iFldId+1, eiNodeId, pczExecFileName));
         return -1;
      }
      memset( aczField, 0, sizeof(aczField) );
      iRet = PKG_Get8583FieldContent( epcOrgMsg, eiOrgMsgLen, &iMsgPos,
                                      pstMsgFldCfg, aczField, &iFldLen );
      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " get the field %d value error at message position %d\n",
                  iFldId+1, iMsgPos));
         return iRet;
      }
      /* all the field is charactor */
      memset( aczTmp, 0, sizeof(aczTmp) );
      for( i=0; i<iFldLen; i++ )
      {
          sprintf( aczTmp, "%s%02X ", aczTmp, aczField[i]&0xff );
      }
      LOG4C(( LOG_DEBUG, "FIELD[%03d][LEN=%03d][%s]", iFldId+1, iFldLen, aczTmp ));

      /*
      if( pstMsgFldCfg->iFldType == UDT_BIN )
      {
          if( pstMsgFldCfg->iSepFldFlag == PKG_UNFIXLEN_FLD )
          {
              memset( aczAscii, 0, sizeof(aczAscii) );
              TOOL_ConvertBCD2Ascii( aczField, iFldLen, aczAscii );
              iRet = UDO_AddField( opfsAddr, iFldId+1+200, UDT_STRING, aczAscii, iFldLen );
              iFldLen = (iFldLen+1)/2;
              iRet = UDO_AddField( opfsAddr, iFldId+1, pstMsgFldCfg->iFldType, aczField, iFldLen );
          }
          else
          {
              iRet = UDO_AddField( opfsAddr, iFldId+1, pstMsgFldCfg->iFldType, aczField, iFldLen );
          }
      }
      else
      */
      {
          iRet = UDO_AddField( opfsAddr, iFldId+1, pstMsgFldCfg->iFldType, aczField, iFldLen );
      }

      if( iRet != 0 )
      {
         LOG4C ((LOG_FATAL, " add the field %d value error[%d]\n", iFldId+1, iRet));
         return iRet;
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

/* 从8583 报文中拆解出相应的域的职  
epaData 8583数据包   
eiFieldid 取数据id
opaczData 数据输出
epaczFiledLen 域的长度4个字符长度，如果第一个字符为B则为变长后面为变长字符长度 
epaczFiledLen 数据从0 域开始填充 如 0004 0016 B002 */

int special8583getfiled(char * epaData ,int eiFieldid,char * opaczData ,char *  epaczFiledLen)
{
   char aczBitFlag[129];
   char aczField[1000];
   int  iFldId, iFldLen, iMsgPos = 0, iRet, iIdx, i;
   MsgFldCfg *pstMsgFldCfg;



   memset( aczBitFlag, 0, sizeof(aczBitFlag) );
   if( PKG_ChangeBitmapToFlag( epaData + 4, 0, aczBitFlag ) == -1 )
   {
      LOG4C ((LOG_FATAL, " Invalid BITMAP"));
      LOG4C ((LOG_DEBUG,
                "special8583getfiled Invalid BITMAP ",
                iMsgPos ));

      return -1;
   }
   iMsgPos += 20;
   /*TOOL_Trace( 2, COMM_LOG_FILE,
               "%d in %s special8583getfiled iMsgPos [%d]aczBitFlag [%s] ",
               __LINE__, __FILE__, iMsgPos , aczBitFlag);*/

   /**
    * split the message to udo according to the bitmap
    */
   for( iFldId = 1; iFldId < eiFieldid - 1; iFldId++ )
   {
      /*TOOL_Trace( 2, COMM_LOG_FILE,
               "%d in %s special8583getfiled  aczBitFlag[iFldId][%c]",
               __LINE__, __FILE__, aczBitFlag[iFldId]  );*/
      if( aczBitFlag[iFldId] == '0' )  /** no field **/
      {
         continue;
      }
      /*TOOL_Trace( 2, COMM_LOG_FILE,
               "%d in %s special8583getfiled  iMsgPos[%d]epaczFiledLen[%d]= [%c]",
               __LINE__, __FILE__, iMsgPos ,iFldId + 1, epaczFiledLen[ 5 * (iFldId + 1 )] );*/

      if((epaczFiledLen + 5 * (iFldId + 1) )[0] == 'B' ) /*变长字段处理*/
      {

         int iTlen =0;
         char  aczTlen[5];
         /*变长的头长度*/
         memset(aczTlen,0,sizeof(aczTlen));
         memcpy(aczTlen,epaczFiledLen +  5 * (iFldId + 1)  + 1 ,3);
         iTlen = atoi(aczTlen);
         /*变长的实际数据长度*/
         memset(aczTlen,0,sizeof(aczTlen));
         LOG4C ((LOG_DEBUG,
               "special8583getfiled  epaData + iMsgPos [%s]",
                epaData + iMsgPos  ));
         memcpy(aczTlen,epaData + iMsgPos  ,iTlen );
         iMsgPos += atoi(aczTlen) + iTlen;
      }
      else
      {
         int iTlen =0;
         char  aczTlen[5];
         memset(aczTlen,0,sizeof(aczTlen));
         memcpy(aczTlen,epaczFiledLen +  5 * (iFldId + 1)  ,4);
         iMsgPos += atoi(aczTlen) ;
      }

   }
   /*TOOL_Trace( 2, COMM_LOG_FILE,
               "%d in %s special8583getfiled  iMsgPos[%d]",
               __LINE__, __FILE__, iMsgPos );*/


   if(epaczFiledLen[ 5 * (iFldId + 1) ] == 'B' ) /*变长字段处理*/
   {
      int iTlen =0;
      char  aczTlen[5];
      /*变长的头长度*/
      memset(aczTlen,0,sizeof(aczTlen));
      memcpy(aczTlen,epaczFiledLen +  5 * (iFldId + 1)  + 1 ,3);
      iTlen = atoi(aczTlen);
      /*变长的实际数据长度*/
      memset(aczTlen,0,sizeof(aczTlen));
      memcpy(aczTlen,epaData + iMsgPos  ,iTlen );
      memcpy( opaczData, epaData + iMsgPos + iTlen , atoi(aczTlen) );
   }
   else
   {
      int iTlen =0;
      char  aczTlen[5];
      memset(aczTlen,0,sizeof(aczTlen));
      memcpy(aczTlen,epaczFiledLen +  5 * (iFldId + 1)  ,4);
      memcpy( opaczData, epaData + iMsgPos  , atoi(aczTlen) );
   }

   LOG4C ((LOG_DEBUG,
               "special8583getfiled  iMsgPos[%d]opaczData[%s]",
                iMsgPos ,opaczData ));

   return 0;
}


