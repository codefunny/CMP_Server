/*
** file:svc_control.c
*/
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "tool_base.h"
#include "svc_base.h"
#include "sub_control.h"
#include "sub_base.h"
#include "svc_control.h"
#include "log.h"

/*
** ���ܣ�׼��������صĲ�����ͬʱ���õ�0���ӷ���
** ���������1 ������������
**           2 �����ڵ�
**           3 ����Ӧ�ú�
**           4 ���״���
**           5 ������KEYֵ
**           6 д����KEYֵ
** ���������1 ������Ӧ����
**           2 ����ָ��
** ����ֵ��    0�ɹ�������ʧ��
*/
int SVC_Prepare( UDP epfReqList, int eiChanId, int eiPkgAppId,
                 char *epczTranCode, SVC_Content *opstService )
{
   Service stServiceCfg;
   int iRet;
   time_t tSysSeconds;
   pid_t shPid;

   assert( epfReqList   != NULL );
   assert( epczTranCode != NULL );
   assert( opstService  != NULL );
/**
   LOG4C ((LOG_DEBUG, "enter the service control model"));
****/
   /*���������ڵ㣬����Ӧ�úźͽ��״�����ڲ���������*/
   iRet = SVC_GetConfig( eiChanId, eiPkgAppId, epczTranCode, &stServiceCfg );
   if( iRet != SVC_SUCC )
   {
     sprintf( opstService->aczErrorMessage,
              "%d||%d|ϵͳ����",
              CMP_NODE_ID, CMP_SYS_ERR);

     LOG4C ((LOG_FATAL,
               "û�д˷���.��������=[%d],������=[%s],����Ӧ�ú�=[%d]",
                eiChanId, epczTranCode, eiPkgAppId));

      opstService->iErrorType = -1;
   }
   else
   {
      opstService->iErrorType = 0;
   
      /*��ʼ���ṹͬʱ��ʼ����0���ӷ���ṹ*/
      iRet = SVC_Init( opstService, epfReqList, &stServiceCfg, eiPkgAppId );
      if( iRet != SVC_SUCC )
      {
         LOG4C ((LOG_FATAL,
                 "��ʼ������ṹ����."
                 "��������=[%d],������=[%s],����Ӧ�ú�=[%d]",
                 eiChanId, epczTranCode, eiPkgAppId));

         opstService->iErrorType = -1;
      }
      strcpy( opstService->stSubSvcHead.aczTranCode, epczTranCode );
      opstService->iChannelId = eiChanId;
      time( &tSysSeconds );
      shPid = getpid();
      /* ��ˮ��=��ǰ����+pid+���д��� */
      sprintf( opstService->aczElSerial, "%010d%05d%05d",
               (int)tSysSeconds, (short)shPid, 
                1);
               /*gpstProcess->shRunTimes );*/
      /*��������Ҫ����־����*/
      /*�������Ľ���Ҫ����Ϣ*/

/**********************************************************************
      LOG_ChgLogFss( opstService, opstService->pfLogList, 2 );
      
      TOOL_MonInit  ();
      TOOL_MonSendMsg (eiChanId, epfReqList, 1, opstService->aczElSerial);
***************************/
   }
  
   /*���濪ʼ�ӷ����������,�ӷ������й����������ӷ�����������*/
   iRet = SUB_StartFlow( opstService );
   if( iRet != SVC_SUCC )
   {
      LOG4C ((LOG_FATAL,
              "�����������н������."
              "��������=[%d],������=[%s],����Ӧ�ú�=[%d]",
               eiChanId, epczTranCode, eiPkgAppId));

      iRet =  SVC_SUBFLOW_ERR;
   }

   /*�ͷŷ���ʹ�õ�����ڴ�*/
   SVC_Free( opstService );

   return iRet;
}

/*
**���ܣ��ͷŷ���ռ�õĿռ�
**���������1 ����ָ��
**���������  ��
**����ֵ��    ��
*/
void SVC_Free( SVC_Content *epstService )
{
   SUB_SvcList *pstTemp;

   assert( epstService != NULL );

   /*�����ͷŵ�һ���ӷ���Ȼ�󽫵ڶ����ӷ����Ϊ��һ���ӷ���*/
   for( pstTemp = epstService->stSubSvcHead.pstNextSubSvc;
        pstTemp != NULL;
        pstTemp = epstService->stSubSvcHead.pstNextSubSvc )
   {
      epstService->stSubSvcHead.pstNextSubSvc = pstTemp->pstNextSubSvc;
      SUB_Free( pstTemp );
      free( pstTemp );
   }

   /*�ͷŵ�0���ӷ��������ռ�*/
   SUB_Free( &epstService->stSubSvcHead );

   /*�ͷŷ�����������Ӧ��Ϣ����,����Ҫ����־����*/
   UDO_Free( epstService->pfReqList );
   UDO_Free( epstService->pfAnswList );
   UDO_Free( epstService->pfLogList );
}

/*
** end of file
*/

