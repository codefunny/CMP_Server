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
** ����: �ӷ���ṹȡ����
** ����: int         eiSubSvcSeq     �ӷ����ڲ���
**       int         eiMsgSscrb      ��������
**       int         eiFldSeq        �������
**       int         eiNum           ���
**       int         eiType          ������
**       SVC_Content *epstSvcList    ָ�����ṹ
** ���: char        *opczValue      ��ֵ
**       int         *opiLen         ��ֵ����
** ����: int 0�ɹ� �������ɹ�
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

   /* �����Դ��0���ӷ���,ֻ����Դ����Ӧ */
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
                 "ȡ��ֵʧ��."
                 "��λ��:�����=[%d],�ӷ����ڲ����=[%d],���ķ���=[%d],"
                 "���=[%d],����=[%d]",
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
** ���ܣ���ʼ���ӷ���
** ���������1 �ӷ����ڷ����б��
**           2 ����ṹָ��
** ���������1 �ӷ���ṹָ��
** ����ֵ��    0�ɹ�������ʧ��
*/
int SVC_SubInit( int eiSubInternalId, SVC_Content *epstService,
                 SUB_SvcList *opstSubSvc )
{
   assert( epstService != NULL );
   assert( opstSubSvc  != NULL );

   LOG4C ((LOG_DEBUG, "enter the service initiate model"));

   opstSubSvc->iSubSvcSeq     = eiSubInternalId;
   opstSubSvc->iRunResultCode = -100; /*��������ӷ���û������*/
   memset( &opstSubSvc->stCorrSubCfg, 0, sizeof( opstSubSvc->stCorrSubCfg ) );
   memset( &opstSubSvc->stErrSubCfg, 0, sizeof( opstSubSvc->stErrSubCfg ) );
   memset( opstSubSvc->aczErrorMessage, 0, SVC_ERRMSG_LEN );
   do
   {
      opstSubSvc->pfReqList = UDO_Alloc( 50*1024 ); /*����30K�ռ�*/
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
** ���ܣ���ʼ����0���ӷ���ͷ������
** ���������1 ����ָ��
**           2 ���������������
**           3 ��������
**           4 ����Ӧ�ú�
**           5 �����к�
**           6 д���к�
** ���������  ��
** ����ֵ��  0�ɹ�������ʧ��
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
   /*����ϵͳ���д���״̬��0��ʾû�д���1��ʾϵͳӦ�ô���-1��ʾϵͳ����*/
   /*�������еĴ��󣬴�����Ϣֻ�ܴӱ����ڷ������ԭ���еõ�*/
   epstService->iErrorType  = 0;
   memset( epstService->aczErrorMessage, 0, SVC_ERRMSG_LEN );

   do
   {
      epstService->pfReqList = UDO_Alloc( 50*1024 );  /**30K�ı��Ŀռ�**/
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

   /*��ʼ���ӷ�������ͬʱ����0���ӷ���ṹ��ʼ��*/
   iRet = SVC_SubInit( 0, epstService, &epstService->stSubSvcHead );
   if( iRet != SVC_SUCC )
   {
      epstService->iErrorType = -1; /*�������ص�ϵͳ����*/
      LOG4C ((LOG_FATAL,
             "��ʼ�������0���ӷ���ṹʧ��,�����=[%d]",
             epstService->iSvcId));

      return SVC_SUBINIT_ERR;
   }
   epstService->stSubSvcHead.iMsgAppId = eiPkgAppId;
   /*���������������Ϊ��0���ӷ�����Ӧ*/
   UDO_Copy( epstService->stSubSvcHead.pfAnswList, epfReqList );

   return SVC_SUCC;
}

/*
** �޸�ǰ���ܣ������ݿ�ȡ�ڲ������
** �޸ĺ��ܣ��ӹ����ڴ�ȡ�ڲ������
** ���������1 �����ڵ��
**           2 ����Ӧ�ú�
**           3 ���״���
** ���������1 �ڲ���������
** ����ֵ��0-�ɹ�����������
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
      LOG4C ((LOG_FATAL, "�ӹ����ڴ��ȡ���������ʧ��"));
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
**����:���������ڵ�����÷�����Ҫ��4���ؼ�ֵ
**����:1 �����ڵ��
**     2 �����������
**���:1 ����ṹָ��
**����:��
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
**����:��λ�ڵ�����
**����:1 ����ָ��
**     2 �ڵ��
**     3 ����
**     4 ���ķ���
**����:���ҵ�FSS,ʧ�ܷ���NULL
*/
UDP SVC_SeekList( SVC_Content *epstService, int eiNodeId,
                  int eiTime, int eiMsgDscrb )
{
   UDP pfInfo = NULL;
   int iTemp = 0;
   SUB_SvcList *pstSubSvc = NULL;
   SUB_TimeCount astTimeCount[10];   /* ���֧��10���ӷ��� */

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
**����:ȡָ���ڵ�ָ�����״����ֶε�����,���ָ���Ĵ�������������,�������
**     һ�θýڵ��������
**����:1 ����ָ���
**     2 �����ڵ��
**     2 �������Ĵ���
**     3 ��������
**     4 ���
**����:������
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
**����:�ӷ��������ȡ�ֶ�ֵ
**����:1 ����ָ��
**     2 �ڵ��
**     3 �ڼ���������,��0��ʼ
**     4 ���ķ���
**     5 ����
**     6 �ظ�����
**���:1 ֵ
**     2 ֵ����
**����:0-�ɹ�,����-ʧ��(����ֵ��FSS��ͬ)
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

