/*
** file    : asm_api.c
*/

#include "asm/asm_api.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif


extern int ASM_RunFunc( int iFldFragCalFlag, SVC_Content *epstService, 
                         UDP pcSrcList, int iMsgAppId, int iFldSeq,
                         UDP pcResult );
#ifdef __cplusplus
}
#endif

/**
#define DEBUG_FATAL
**/

#define ASM_GET_ERROR(eiSvcId, eiSubSeq, iFldSeq, iRet, iHasError )\
   LOG4C ((LOG_FATAL, \
           "取域重组结果失败." \
           "服务号=[%d] 内部子服务序号=[%d] 域号=[%d]", \
           eiSvcId, eiSubSeq, iFldSeq)); \
   sprintf( opczErrorMsg,\
            "服务%d内部子服务%d报文重组从链表中取节点值失败 iRet=%d",\
            eiSvcId, eiSubSeq, iRet );\
      iHasError = 1;

/*
**  函数功能: 生成子服务请求链表
**  函数名:   CreateSubReqList
**  输入参数: 1 服务号
**            2 子服务序号
**            3 是否冲正标识
**            4 服务信息链表
**  输出参数: 创建的子服务请求链表
**            错误信息描述，只需描述错误信息即可
**  返回值:   成功返回0，其他值失败
*/
int  ASM_CreateSubReqList( int eiSvcId, int eiSubSeq, int eiBalFlag,
                           SVC_Content * epstService, 
                           UDP opcSubReqPkgList, char *opczErrorMsg )
{
   int        iRet, iCount, i;
   int        iValue;
   long       lValue;
   float      fValue;
   double     dValue;
   short      sValue;
   char       aczValue[MAX_STR_NODE_LEN];
   int        iLength;
   int        iNum;
   int        iRow;
   int        iHasError = 0;    /* 在处理中是否发生错误 */
 
   int        iFldSeq;          /*子服务报文域编号*/
   int        iFldType;         /*子服务报文域类型*/
   int        iRepFldFlag;      /*重复域标识*/
   int        iRepTmSubSeq;     /*重复次数域所在子服务序号*/
   int        iRepTmFld;        /*重复域的重复次数域域号*/
   int        iRepTmType;       /*重复次数域的数据类型*/
   int        iFldFragCalFlag;  /*域碎片计算类型*/
   int        iFldAsmId;        /*域重组ID*/
   int        iMsgAppId;        /*子服务数据字典编号*/
 
 
   ReqCfgAry stReqCfgAry;
   FldSrcCfgAry stFldSrcCfgAry;
   char pcSrcList[2048]; 
   char pcResult[2048];
 
   assert( epstService != NULL );
   assert( opcSubReqPkgList != NULL );

   UDO_Init( pcSrcList, sizeof(pcSrcList) );
   UDO_Init( pcResult,  sizeof(pcResult) );

   stReqCfgAry.pstReqCfgNode = NULL;
   stFldSrcCfgAry.pstFldSrcCfgNode = NULL;
   /*如果是响应的时候,数据字典编号从服务结构得到,否则从最后一个子服务得到*/
   if( eiSubSeq > 0 )
   {
      iMsgAppId = epstService->pstSubSvcCurr->iMsgAppId;
   }
   else
   {
      iMsgAppId = epstService->iPkgAppId;
   }

   iRet = ASM_CreateReqCfgAry( eiSvcId, eiSubSeq, eiBalFlag, 
                               &stReqCfgAry );
  
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL,
              "创建请求参数配置信息链表失败 服务号=[%d] 内部子服务序号=[%d]",
              eiSvcId, eiSubSeq));

      if( iRet == -1 )   /* fetch no data */
      {
         sprintf( opczErrorMsg,
                  "没有配置服务%d内部子服务%d报文重组信息", eiSvcId, eiSubSeq );
      }
      else
      {
         sprintf( opczErrorMsg,
                  "创建服务%d内部子服务%d报文重组配置信息失败",
                  eiSvcId, eiSubSeq );
      }
      return ASM_CRT_REQLIST_ERR;
   }


   for( iRow=0; iRow < stReqCfgAry.iNum; iRow++ )
   {
      iFldSeq = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iFldSeq;
      iFldType = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iFldType;
      iRepFldFlag = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iRepFldFlag;
      iRepTmSubSeq = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iRepTmSubSeq;
      iRepTmFld = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iRepTmFld;
      iRepTmType = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iRepTmType;
      iFldFragCalFlag = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iFldFragCalFlag;
      iFldAsmId = (*((stReqCfgAry.pstReqCfgNode)+iRow)).iFldAsmId;

      
      if( iHasError == 1 )
      {
            goto ASM_ERROR_ACTION;
      }

      if( iRepFldFlag == 0 )/*FldSeq域是非重复域*/
      {
         iCount = 1;
      }
      else if( iRepFldFlag == 1 )/*FldSeq域是重复域*/
      {
      
            /** 取重复域发生错误不做处理 **/
         iRet =  ASM_GetRepTime( epstService, (stReqCfgAry.pstReqCfgNode)+iRow,
                                    &iCount );
         if( iRet != ASM_SUCC || iCount < 0 )
         {
            LOG4C ((LOG_FATAL,
                    "取域重复次数错误."
                    "服务号=[%d] 内部子服务序号=[%d] 域号=[%d]",
                     eiSvcId, eiSubSeq, iFldSeq));

            sprintf( opczErrorMsg,
                     "服务%d内部子服务%d报文重组取重复次数错误",
                     eiSvcId, eiSubSeq );
            free( stReqCfgAry.pstReqCfgNode );
            return ASM_FAIL;
         }
      }
      else 
      {
          LOG4C ((LOG_FATAL,
                  "域重复标识错误."
                  "服务号=[%d] 内部子服务序号=[%d] 域号=[%d]",
                  eiSvcId, eiSubSeq, iFldSeq));

         sprintf( opczErrorMsg,
                  "服务%d内部子服务%d报文重组错误的重复域标识",
                  eiSvcId, eiSubSeq );
         free( stReqCfgAry.pstReqCfgNode );
         return ASM_FAIL;
      }

      /** 创建来源错误，也认为是系统错误，不做处理 **/
      iRet =  ASM_CreateFldSrcCfgAry( iFldAsmId, &stFldSrcCfgAry );
      if( iRet != ASM_SUCC )
      {
          LOG4C ((LOG_FATAL,
                  "创建域来源配置信息链表错误."
                  "服务号=[%d] 内部子服务序号=[%d] 域号=[%d]",
                  eiSvcId, eiSubSeq, iFldSeq));

         sprintf( opczErrorMsg,
                  "服务%d内部子服务%d报文重组创建域来源配置信息链表错误",
                  eiSvcId, eiSubSeq );
         free( stReqCfgAry.pstReqCfgNode );
         return ASM_FAIL;
      }

      for( i = 0; i < iCount; i++ )
      {
         UDO_Init( pcSrcList, sizeof(pcSrcList) );
         iRet =  ASM_CreateFldSrcValList( epstService, &stFldSrcCfgAry, i,
                                          &iNum, pcSrcList );
         if( iRet != ASM_SUCC )
         {
            LOG4C ((LOG_FATAL,
                    "创建域来源值链表错误."
                    "服务号=[%d] 内部子服务序号=[%d] 域号=[%d]",
                     eiSvcId, eiSubSeq, iFldSeq));

            sprintf( opczErrorMsg,
                     "服务%d内部子服务%d报文重组创建域来源值链表错误",
                     eiSvcId, eiSubSeq );
            iHasError = 1;
            break;
         }
         UDO_Init( pcResult, sizeof(pcResult) );

         iRet = ASM_RunFunc( iFldFragCalFlag, epstService, pcSrcList,
                             iMsgAppId, iFldSeq, pcResult );
         if( iRet != ASM_SUCC )
         {
             LOG4C ((LOG_FATAL,
                    "函数执行错误."
                    "服务号=[%d] 内部子服务序号=[%d] 域号=[%d] 重组函数号=[%d]",
                    eiSvcId, eiSubSeq, iFldSeq, iFldFragCalFlag));

            strcpy( opczErrorMsg, epstService->aczErrorMessage );
            iHasError = 1;
            break;
         }

         memset( aczValue, 0x00, MAX_STR_NODE_LEN );
         iRet = UDO_SafeGetField (pcResult, 1, 0, iFldType, aczValue, &iLength);
         if( iRet != 0 )
         {
             ASM_GET_ERROR(eiSvcId, eiSubSeq, iFldSeq, iRet, iHasError );
         }
         else
         {
             UDO_AddField(opcSubReqPkgList,iFldSeq,iFldType,aczValue,iLength);
         }

         UDO_Free( pcSrcList );
         UDO_Free( pcResult );

         if( iHasError == 1 )
         {
               break;
         }
      }

      free( stFldSrcCfgAry.pstFldSrcCfgNode );
      stFldSrcCfgAry.pstFldSrcCfgNode = NULL;

      /**
       * 当重组发生错误时，剩余字段处理都填空值
       */
ASM_ERROR_ACTION:
      if( iHasError == 1 )
      {
         memset (aczValue, 0, sizeof (aczValue));
         UDO_AddField( opcSubReqPkgList, iFldSeq, iFldType, aczValue, 0 );
      }
   }
   free( stReqCfgAry.pstReqCfgNode );

   return (iHasError == 0)? ASM_SUCC: ASM_FAIL;
}

/*
** end of file: asm_api.c
*/
