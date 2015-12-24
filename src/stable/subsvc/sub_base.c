/*
** file: sub_base.c
*/
#include <stdio.h>
#include <stdlib.h>

#include "tool_base.h"
#include "sub_base.h"
#include "svc_base.h"
#include "global.h"
#include "udo.h"
#include "shm_base.h"
#include "log.h"

int SUB_SearchSvcCfg( const void *p1, const void *p2 )
{
   ServiceCfg *pstL = (ServiceCfg *)p1;
   ServiceCfg *pstR = (ServiceCfg *)p2;

   if( pstL->iSvcId != pstR->iSvcId )
   {
      return pstL->iSvcId - pstR->iSvcId;
   }
   else
   {
      return pstL->iSubSeq - pstR->iSubSeq;
   }
}

int SUB_SearchSubSvc( const void *p1, const void *p2 )
{
   SubService *pstL = (SubService *)p1;
   SubService *pstR = (SubService *)p2;

   return pstL->iSubId - pstR->iSubId;
}

/*
** ����: ���ݷ���ź��ڲ��ӷ���Ŷ��ӷ���������Ϣ
** �������: 1 �����
**           2 �ڲ��ӷ����
** �������: 1 �ӷ�������ָ��
** ����ֵ:     0�ɹ�,����ʧ��
*/
int SUB_GetSubConfig( int eiSvcId, int eiInternalId,
                      SUB_Config *opstSubSvcConfig )
{
   int  iTemp, iRowNum, iAssignTime;
   ServiceCfg *pstServiceCfg, *pstSCfg, stSCfgKey;
   SubService *pstSubService, *pstSSvc, stSSvcKey;

   assert( opstSubSvcConfig != NULL );

   pstServiceCfg = (ServiceCfg *)SHM_ReadData( pcShmAddr, SERVICECFG,
                                               &iRowNum );
   stSCfgKey.iSvcId = eiSvcId;
   stSCfgKey.iSubSeq = eiInternalId;
   pstSCfg = bsearch( &stSCfgKey, pstServiceCfg, iRowNum, sizeof(stSCfgKey),
                      SUB_SearchSvcCfg );
   if( pstSCfg == NULL )
   {
      LOG4C ((LOG_FATAL,
              "ĩ�ҵ���Ӧ�����������,�����=[%d],�ӷ����ڲ����=[%d]",
              eiSvcId, eiInternalId));

      return SUB_GETCFG_ERR;
   }
   pstServiceCfg = pstSCfg;
   opstSubSvcConfig->iSvcId           = pstServiceCfg->iSvcId;
   opstSubSvcConfig->iHostNodeId      = pstServiceCfg->iHostNodeId;
   opstSubSvcConfig->iSubSeq          = pstServiceCfg->iSubSeq;
   opstSubSvcConfig->iOutTime         = pstServiceCfg->iOutTime;
   opstSubSvcConfig->iRetryTime       = pstServiceCfg->iRetryTime;
   opstSubSvcConfig->iBatFlag         = pstServiceCfg->iBatFlag;
   opstSubSvcConfig->iBatMode         = pstServiceCfg->iBatMode;
   opstSubSvcConfig->iLstId           = pstServiceCfg->iLstId;
   opstSubSvcConfig->iErrorHandleType = pstServiceCfg->iErrorHandleType;

   /**
     * �������������ӷ�����Ϣ
     */
   stSSvcKey.iSubId = pstServiceCfg->iCorrSubId;
   pstSubService= (SubService*)SHM_ReadData( pcShmAddr, SUBSERVICE, &iRowNum );
   pstSSvc = bsearch( &stSSvcKey, pstSubService, iRowNum, sizeof(stSSvcKey),
                      SUB_SearchSubSvc );
   if( pstSSvc == NULL )
   {
      LOG4C ((LOG_FATAL,
              "��Ӧ���ӷ�������δ�ҵ�,"
              "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
               eiSvcId, eiInternalId,  pstServiceCfg->iCorrSubId));

      return SUB_GETCFG_ERR;
   }
   strcpy( opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
           pstSSvc->aczTranCode);
   opstSubSvcConfig->stCorrSubCfg.iSubId = pstSSvc->iSubId;
   opstSubSvcConfig->iMsgAppId = pstSSvc->iMsgAppId;
   opstSubSvcConfig->stCorrSubCfg.iTempletId = pstSSvc->iTempletId;
   opstSubSvcConfig->stCorrSubCfg.iRecordsPerTime = pstSSvc->iRecordPerTime;
   strcpy( opstSubSvcConfig->stCorrSubCfg.aczSubName, pstSSvc->aczSubName );

   /**
     * ���������������Ҫͬ������(�����ӷ���)
     * �ҳ����ӷ�������
     */
   if( pstServiceCfg->iErrorHandleType != SUB_ERR_NOOP && eiInternalId > 0 )
   {
      stSSvcKey.iSubId = pstServiceCfg->iErrSubId;
      pstSSvc = bsearch( &stSSvcKey, pstSubService, iRowNum, sizeof(stSSvcKey),
                         SUB_SearchSubSvc );
      if( pstSSvc == NULL )
      {
         LOG4C ((LOG_FATAL,
                 "��Ӧ�Ĵ������ӷ�������δ�ҵ�,"
                 "�����=[%d],�ӷ����ڲ����=[%d],�ӷ����=[%d]",
                 eiSvcId, eiInternalId,  pstServiceCfg->iErrSubId));

         return SUB_GETCFG_ERR;
      }
      strcpy( opstSubSvcConfig->stErrSubCfg.aczSubTranCode,
              pstSSvc->aczTranCode);
      opstSubSvcConfig->stErrSubCfg.iSubId = pstSSvc->iSubId;
      opstSubSvcConfig->stErrSubCfg.iTempletId = pstSSvc->iTempletId;
      opstSubSvcConfig->stErrSubCfg.iRecordsPerTime = pstSSvc->iRecordPerTime;
      strcpy( opstSubSvcConfig->stErrSubCfg.aczSubName, pstSSvc->aczSubName );

      /*������Ϣ,������ĵ�ǰ�ӷ������ô�ӡ����*/
      LOG4C ((LOG_DEBUG, "����%d���ڲ��ӷ����%d������Ϣ: \n�����ڵ�%d,������"
                "־%d����ģʽ%d,�����ֵ��%d,����������%d,���״���%s,�ӷ���"
                "��%s,�ӷ����%d,������%d,�������״���%s,�����ӷ�����%s,����"
                "�ӷ����%d,����������%d,��ʱʱ��%d",
                eiSvcId, eiInternalId,
                opstSubSvcConfig->iHostNodeId, opstSubSvcConfig->iBatFlag,
                opstSubSvcConfig->iBatMode, opstSubSvcConfig->iMsgAppId,
                opstSubSvcConfig->iErrorHandleType,
                opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
                opstSubSvcConfig->stCorrSubCfg.aczSubName,
                opstSubSvcConfig->stCorrSubCfg.iSubId,
                opstSubSvcConfig->stCorrSubCfg.iTempletId,
                opstSubSvcConfig->stErrSubCfg.aczSubTranCode,
                opstSubSvcConfig->stErrSubCfg.aczSubName,
                opstSubSvcConfig->stErrSubCfg.iSubId,
                opstSubSvcConfig->stErrSubCfg.iTempletId,
                opstSubSvcConfig->iOutTime));
   }
   else
   {
      /*������Ϣ,������ĵ�ǰ�ӷ������ô�ӡ����*/
      LOG4C ((LOG_DEBUG, 
              "����%d���ڲ��ӷ����%d������Ϣ:"
              "�����ڵ�%d,������־%d ����ģʽ%d, �����ֵ��%d,"
              "�޴���������,���״���%s,�ӷ�����%s,�ӷ����%d,������%d",
              eiSvcId, eiInternalId,
              opstSubSvcConfig->iHostNodeId, opstSubSvcConfig->iBatFlag,
              opstSubSvcConfig->iBatMode, opstSubSvcConfig->iMsgAppId,
              opstSubSvcConfig->stCorrSubCfg.aczSubTranCode,
              opstSubSvcConfig->stCorrSubCfg.aczSubName,
              opstSubSvcConfig->stCorrSubCfg.iSubId,
              opstSubSvcConfig->stCorrSubCfg.iTempletId));
   }

   return SVC_SUCC;
}

/*
** ����: ����µ��ӷ���
** �������: 1 �ӷ�������ָ��
**           2 �����к�
**           3 д���к�
** �������: 1 ����ṹ
** ����ֵ      0�ɹ�,����ʧ��
*/
int SUB_AddNew( SVC_Content *eopstService, SUB_Config *epstSubSvcConfig,
                int eiReadKeyId, int eiWriteKeyId )
{
   SUB_SvcList *pstSubSvc;

   assert( eopstService     != NULL );
   assert( epstSubSvcConfig != NULL );

   do
   {
      pstSubSvc = (SUB_SvcList *)malloc( sizeof( SUB_SvcList ) );
   }while( pstSubSvc == NULL );
   pstSubSvc->pstNextSubSvc = NULL;
   pstSubSvc->pstService    = eopstService;
   pstSubSvc->iSubSvcSeq    = epstSubSvcConfig->iSubSeq;
   pstSubSvc->iHostId       = epstSubSvcConfig->iHostNodeId;
   pstSubSvc->iMsgAppId     = epstSubSvcConfig->iMsgAppId;
   pstSubSvc->iIsBatch      = epstSubSvcConfig->iBatFlag;
   pstSubSvc->iBatchMode    = epstSubSvcConfig->iBatMode;
   pstSubSvc->iOutTime      = epstSubSvcConfig->iOutTime;   
   /**********************************************************/
   pstSubSvc->iLstId        = epstSubSvcConfig->iLstId;
   /***********************************************************/
   pstSubSvc->iReadKeyId    = eiReadKeyId;
   pstSubSvc->iWriteKeyId   = eiWriteKeyId;
   pstSubSvc->iRunResultCode= 0;
   pstSubSvc->iErrorHandleType = epstSubSvcConfig->iErrorHandleType;

   pstSubSvc->iStartErrorFlow  = 1;
   memcpy( &pstSubSvc->stCorrSubCfg, &epstSubSvcConfig->stCorrSubCfg,
            sizeof( epstSubSvcConfig->stCorrSubCfg ) );
   memcpy( &pstSubSvc->stErrSubCfg, &epstSubSvcConfig->stErrSubCfg,
            sizeof( epstSubSvcConfig->stErrSubCfg ) );
   do
   {
      pstSubSvc->pfReqList = UDO_Alloc( 50*1024 );
   } while( pstSubSvc->pfReqList == NULL );
   do
   {
      pstSubSvc->pfAnswList = UDO_Alloc( 50*1024 );
   } while( pstSubSvc->pfAnswList == NULL );
   pstSubSvc->pfBalanceList = NULL;
   /*�޸��ӷ�������ָ��ָ��,�Ȱ��½ڵ����ӽ�ȥ,Ȼ���޸�β�ڵ�ָ��*/
   eopstService->pstSubSvcTail->pstNextSubSvc = pstSubSvc;
   eopstService->pstSubSvcTail = pstSubSvc;
   eopstService->pstSubSvcCurr = pstSubSvc;

   return SVC_SUCC;
}

/*
**���ܣ��ͷ��ӷ����������Ϣ����Ӧ��Ϣ����
**���������1 �ӷ���ڵ�ָ��
**���������  ��
**����ֵ��    ��
*/
void SUB_Free( SUB_SvcList *epstSubService )
{
   assert( epstSubService != NULL );

   UDO_Free( epstSubService->pfReqList );
   UDO_Free( epstSubService->pfAnswList );
   if( epstSubService->pfBalanceList != NULL )
   {
      UDO_Free( epstSubService->pfBalanceList );
   }
}

/*
** end of file
*/

