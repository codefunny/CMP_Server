/*
** file:svc_base.c
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "tool_base.h"
#include "udo.h"
#include "shm_base.h"
#include "pub_base.h"

#include "sub_base.h"
#include "svc_base.h"
#include "log.h"

/*
** 功能: 从服务结构取数据
** 输入: int         eiSubSvcSeq     子服务内部号
**       int         eiMsgSscrb      报文描述
**       int         eiFldSeq        报文域号
**       int         eiNum           序号
**       int         eiType          域类型
**       SVC_Content *epstSvcList    指向服务结构
** 输出: char        *opczValue      域值
**       int         *opiLen         域值长度
** 返回: int 0成功 其他不成功
*/
int SVC_GetValueFromSub( int eiSubSvcSeq, int eiMsgDesc, int eiFldSeq,
                         int eiNum,       int eiType,
                         SVC_Content *epstSvcList, char *opczValue, int *opiLen  )
{
   SUB_SvcList *pstSubSvc;
   UDP          pfInfo;
   char aczVal[100];
   int iRet, iLen, *piVal;
   short *pshVal;
   long *plVal;
   float *pfVal;
   double *pdVal;

   assert( epstSvcList != NULL );
   assert( opczValue   != NULL );

   pstSubSvc = &epstSvcList->stSubSvcHead;
   while( pstSubSvc != NULL && pstSubSvc->iSubSvcSeq != eiSubSvcSeq )
   {
      pstSubSvc = pstSubSvc->pstNextSubSvc;
   }

   if( pstSubSvc == NULL )
   {
      return SVC_SUB_NOTFOUND;
   }

   /* 如果来源于0号子服务,只能来源于响应 */
   if( eiSubSvcSeq == 0 )
   {
      eiMsgDesc = 2;
   }

   if( eiMsgDesc == 1 )
   {
      pfInfo = pstSubSvc->pfReqList;
   }
   else
   {
      pfInfo = pstSubSvc->pfAnswList;
   }

   /**
    * modi for support more type
    **/
   iLen = 0;
   iRet = UDO_SafeGetField (pfInfo, eiFldSeq, eiNum, eiType, opczValue, &iLen);
   if (iRet != 0) {
       LOG4C  ((LOG_FATAL, 
                 "取域值失败."
                 "域位置:服务号=[%d],子服务内部序号=[%d],报文方向=[%d],"
                 "域号=[%d],次数=[%d]",
                 epstSvcList->iSvcId, eiSubSvcSeq, eiMsgDesc, eiFldSeq, eiNum));
   }
   *opiLen = iLen;

   /*******
   memset( aczVal, 0, 100 );
   iRet = UDO_GetField( pfInfo, eiFldSeq, eiNum, aczVal, &iLen );
   if( iRet == 0 )
   {
      switch( UDO_FieldType( pfInfo, eiFldSeq ) )
      {
      case UDT_SHORT:
         pshVal = (short*)aczVal;
         sprintf( opczValue, "%d", *pshVal );
         break;
      case UDT_INT:
         piVal = (int*)aczVal;
         sprintf( opczValue, "%d", *piVal );
         break;
      case UDT_LONG:
         plVal = (long*)aczVal;
         sprintf( opczValue, "%ld", *plVal );
         break;
      case UDT_FLOAT:
         pfVal = (float*)aczVal;
         sprintf( opczValue, "%f", *pfVal );
         break;
      case UDT_DOUBLE:
         pdVal = (double*)aczVal;
         sprintf( opczValue, "%lf", *pdVal );
         break;
      default:
         memcpy( opczValue, aczVal, iLen );
      }
   }
   ************/

   return iRet;
}

/*
** 功能：初始化子服务
** 输入参数：1 子服务在服务中编号
**           2 服务结构指针
** 输出参数：1 子服务结构指针
** 返回值：    0成功，其他失败
*/
int SVC_SubInit( int eiSubInternalId, SVC_Content *epstService,
                 SUB_SvcList *opstSubSvc )
{
   assert( epstService != NULL );
   assert( opstSubSvc  != NULL );

   LOG4C ((LOG_DEBUG, "enter the service initiate model"));

   opstSubSvc->iSubSvcSeq     = eiSubInternalId;
   opstSubSvc->iRunResultCode = -100; /*表明这个子服务没有运行*/
   memset( &opstSubSvc->stCorrSubCfg, 0, sizeof( opstSubSvc->stCorrSubCfg ) );
   memset( &opstSubSvc->stErrSubCfg, 0, sizeof( opstSubSvc->stErrSubCfg ) );
   memset( opstSubSvc->aczErrorMessage, 0, SVC_ERRMSG_LEN );
   do
   {
      opstSubSvc->pfReqList = UDO_Alloc( 50*1024 ); /*分配30K空间*/
   } while( opstSubSvc->pfReqList == NULL );
   do
   {
      opstSubSvc->pfAnswList = UDO_Alloc( 50*1024 );
   } while( opstSubSvc->pfAnswList == NULL );
   opstSubSvc->pstService    = epstService;
   opstSubSvc->pstNextSubSvc = NULL;
   opstSubSvc->pfBalanceList = NULL;

   return SVC_SUCC;
}

/*
** 功能：初始化第0个子服务和服务参数
** 输入参数：1 服务指针
**           2 服务请求参数链表
**           3 服务配置
**           4 报文应用号
**           5 读队列号
**           6 写队列号
** 输出参数：  无
** 返回值：  0成功，其他失败
*/
int SVC_Init( SVC_Content *epstService, UDP epfReqList,
              Service  *epstSvcConfig, int eiPkgAppId )
{
   int iRet;

   assert( epstService   != NULL );
   assert( epfReqList    != NULL );
   assert( epstSvcConfig != NULL );

   epstService->iSvcId      = epstSvcConfig->iSvcId;
   epstService->iPkgAppId   = eiPkgAppId;
   /*设置系统运行错误状态，0表示没有错误，1表示系统应用错误，-1表示系统不能*/
   /*正常运行的错误，错误信息只能从保存在服务错误原因中得到*/
   epstService->iErrorType  = 0;
   memset( epstService->aczErrorMessage, 0, SVC_ERRMSG_LEN );

   do
   {
      epstService->pfReqList = UDO_Alloc( 50*1024 );  /**30K的报文空间**/
   } while( epstService->pfReqList == NULL );
   do
   {
      epstService->pfAnswList = UDO_Alloc( 50*1024 );
   } while( epstService->pfAnswList == NULL );
   do
   {
      epstService->pfLogList = UDO_Alloc( 20*1024 );
   } while( epstService->pfLogList == NULL );
   
   epstService->pstSubSvcTail = &epstService->stSubSvcHead;
   epstService->pstSubSvcCurr = &epstService->stSubSvcHead;

   /*初始化子服务链表，同时将第0个子服务结构初始化*/
   iRet = SVC_SubInit( 0, epstService, &epstService->stSubSvcHead );
   if( iRet != SVC_SUCC )
   {
      epstService->iErrorType = -1; /*发生严重的系统错误*/
      LOG4C ((LOG_FATAL,
             "初始化服务第0个子服务结构失败,服务号=[%d]",
             epstService->iSvcId));

      return SVC_SUBINIT_ERR;
   }
   epstService->stSubSvcHead.iMsgAppId = eiPkgAppId;
   /*将服务请求参数作为第0个子服务响应*/
   UDO_Copy( epstService->stSubSvcHead.pfAnswList, epfReqList );

   return SVC_SUCC;
}

/*
** 修改前功能：从数据库取内部服务号
** 修改后功能：从共享内存取内部服务号
** 输入参数：1 渠道节点号
**           2 报文应用号
**           3 交易代码
** 输出参数：1 内部服务配置
** 返回值：0-成功，其它错误
*/
int SVC_GetConfig( int eiChanId, int eiPkgAppId,
                   char *epczTranCode, Service *opstServiceCfg )
{
   int  iTemp, iRowNum;
   Service *pstService = NULL;

   assert( epczTranCode   != NULL );
   assert( opstServiceCfg != NULL );

   iRowNum = 0;
   pstService = (Service *)SHM_ReadData ( (void *)pcShmAddr, 
                                          SERVICE, &iRowNum );
   if( pstService == NULL )
   {
      LOG4C ((LOG_FATAL, "从共享内存读取服务表数据失败"));
      return -1;
   }

   for( iTemp = 0; iTemp < iRowNum; iTemp++ )
   {
      if( pstService->iNodeId == eiChanId &&
          strcmp( pstService->aczTranCode, epczTranCode ) == 0 )
      {
         *opstServiceCfg = *pstService;
         return SVC_SUCC;
      }
      pstService++;
   }

   return SVC_GETCFG_ERR;
}

int SVC_SafeGetInt( UDP epfReq, int eiFieldId, int *opiFldVal )
{
   int iFieldLen, iFieldType;
   short shVal;
   float fVal;
   double dVal;
   char *pczStr;

   assert( epfReq    != NULL );
   assert( opiFldVal != NULL );

   iFieldLen = UDO_FieldLen( epfReq, eiFieldId, 0 );
   if( iFieldLen < 0 )
   {
      return -1;
   }

   iFieldType = UDO_FieldType( epfReq, eiFieldId );
   switch( iFieldType )
   {
   case UDT_SHORT:
      UDO_GetField( epfReq, eiFieldId, 0, (char*)&shVal, 0 );
      *opiFldVal = (int)shVal;
      break;
   case UDT_INT:
      UDO_GetField( epfReq, eiFieldId, 0, (char*)opiFldVal, 0 );
      break;
   case UDT_LONG:
      UDO_GetField( epfReq, eiFieldId, 0, (char*)opiFldVal, 0 );
      break;
   case UDT_FLOAT:
      UDO_GetField( epfReq, eiFieldId, 0, (char*)&fVal, 0 );
      *opiFldVal = (int)fVal;
      break;
   case UDT_DOUBLE:
      UDO_GetField( epfReq, eiFieldId, 0, (char*)&dVal, 0 );
      *opiFldVal = (int)dVal;
      break;
   case UDT_STRING:
      pczStr = (char *)malloc( iFieldLen + 1 );
      memset( pczStr, 0, iFieldLen + 1 );
      UDO_GetField( epfReq, eiFieldId, 0, pczStr, 0 );
      *opiFldVal = atoi( pczStr );
      free( pczStr );
   }

   return 0; 
}

/*
**功能:根据渠道节点号设置服务需要的4个关键值
**输入:1 渠道节点号
**     2 服务请求参数
**输出:1 服务结构指针
**返回:无
*/
void SVC_SetKeyValue( int eiChanNodeId, UDP epfSvcReq,
                      SVC_Content *opstService )
{
   NodeCommCfg stNodeCommCfg;
   int iRowNum, iTemp;

   assert( opstService != NULL );

   iRowNum = UDO_FieldOccur( pcShmAddr, NODECOMMCFG );
   for( iTemp = 0; iTemp < iRowNum; iTemp++ )
   {
      UDO_GetField( pcShmAddr, NODECOMMCFG, iTemp, (char*)&stNodeCommCfg, 0 );
      if( stNodeCommCfg.iNodeId == eiChanNodeId )
      {
      }
   }
}

/*
**功能:定位节点链表
**输入:1 服务指针
**     2 节点号
**     3 次数
**     4 报文方向
**返回:查找的FSS,失败返回NULL
*/
UDP SVC_SeekList( SVC_Content *epstService, int eiNodeId,
                  int eiTime, int eiMsgDscrb )
{
   UDP pfInfo = NULL;
   int iTemp = 0;
   SUB_SvcList *pstSubSvc = NULL;
   SUB_TimeCount astTimeCount[10];   /* 最大支持10个子服务 */

   memset( astTimeCount, 0x00, sizeof( astTimeCount ) );
   if( eiNodeId == epstService->iChannelId )
   {
      if( eiMsgDscrb == 1 )
      {
         return epstService->stSubSvcHead.pfAnswList;
      }
   }

   pstSubSvc = epstService->stSubSvcHead.pstNextSubSvc;
   while( pstSubSvc != NULL )
   {
      if( pstSubSvc->iHostId == eiNodeId )
      {
         if( eiNodeId == epstService->iChannelId && eiMsgDscrb == 2 )
         {
            return pstSubSvc->pfReqList;
         }
         if( eiMsgDscrb == 1 )
         {
            pfInfo = pstSubSvc->pfReqList;
         }
         else
         {
            pfInfo = pstSubSvc->pfAnswList;
         }
         for( iTemp = 0; iTemp < 10; iTemp++ )
         {
            if( astTimeCount[iTemp].iHostId == eiNodeId )
            {
               if( astTimeCount[iTemp].iTime == eiTime )
               {
                  return pfInfo;
               }
               else
               {
                  astTimeCount[iTemp].iTime++;
               }
            }
            if( astTimeCount[iTemp].iHostId == 0 )
            {
               astTimeCount[iTemp].iHostId == eiNodeId;
               if( eiTime == 0 )
               {
                  return pfInfo;
               }
               else
               {
                  astTimeCount[iTemp].iTime = 1;
               }
            }
         }
      }
      pstSubSvc = pstSubSvc->pstNextSubSvc;
   }

   return pfInfo;
}

/*
**功能:取指定节点指定交易次数字段的类型,如果指定的次数大于最大次数,返回最后
**     一次该节点的域类型
**输入:1 服务指针对
**     2 主机节点号
**     2 上主机的次数
**     3 报文类型
**     4 域号
**返回:域类型
*/
int SVC_FieldType( SVC_Content *epstService, int eiNodeId, int eiTime,
                   int eiMsgDscrb, int eiFldId )
{
   UDP pfInfo;

   pfInfo = SVC_SeekList( epstService, eiNodeId, eiTime, eiMsgDscrb );
   if( pfInfo == NULL )
   {
      return -1;
   }

   return UDO_FieldType( pfInfo, eiFldId );
}

/*
**功能:从服务的链表取字段值
**输入:1 服务指针
**     2 节点号
**     3 第几次上主机,从0开始
**     4 报文方向
**     5 域编号
**     6 重复次数
**输出:1 值
**     2 值长度
**返回:0-成功,其它-失败(具体值和FSS相同)
*/
int SVC_GetField( SVC_Content *epstService, int eiNodeId,  int eiTime,
                  int eiMsgDscrb,           int eiFieldId, int eiOccur,
                  char *opcValue,           int *opiLen )
{
   int iRet;
   UDP pfInfo;

   assert( epstService != NULL );

   pfInfo = SVC_SeekList( epstService, eiNodeId, eiTime, eiMsgDscrb );
   if( pfInfo == NULL )
   {
      if( opiLen )
      {
         *opiLen = 0;
      }
      return -8;
   }

   iRet = UDO_GetField( pfInfo, eiFieldId, eiOccur, opcValue, opiLen );

   return iRet;
}

/*
** end of file
*/

