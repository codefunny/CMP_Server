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
           "ȡ��������ʧ��." \
           "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d]", \
           eiSvcId, eiSubSeq, iFldSeq)); \
   sprintf( opczErrorMsg,\
            "����%d�ڲ��ӷ���%d���������������ȡ�ڵ�ֵʧ�� iRet=%d",\
            eiSvcId, eiSubSeq, iRet );\
      iHasError = 1;

/*
**  ��������: �����ӷ�����������
**  ������:   CreateSubReqList
**  �������: 1 �����
**            2 �ӷ������
**            3 �Ƿ������ʶ
**            4 ������Ϣ����
**  �������: �������ӷ�����������
**            ������Ϣ������ֻ������������Ϣ����
**  ����ֵ:   �ɹ�����0������ֵʧ��
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
   int        iHasError = 0;    /* �ڴ������Ƿ������� */
 
   int        iFldSeq;          /*�ӷ���������*/
   int        iFldType;         /*�ӷ�����������*/
   int        iRepFldFlag;      /*�ظ����ʶ*/
   int        iRepTmSubSeq;     /*�ظ������������ӷ������*/
   int        iRepTmFld;        /*�ظ�����ظ����������*/
   int        iRepTmType;       /*�ظ����������������*/
   int        iFldFragCalFlag;  /*����Ƭ��������*/
   int        iFldAsmId;        /*������ID*/
   int        iMsgAppId;        /*�ӷ��������ֵ���*/
 
 
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
   /*�������Ӧ��ʱ��,�����ֵ��Ŵӷ���ṹ�õ�,��������һ���ӷ���õ�*/
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
              "�����������������Ϣ����ʧ�� �����=[%d] �ڲ��ӷ������=[%d]",
              eiSvcId, eiSubSeq));

      if( iRet == -1 )   /* fetch no data */
      {
         sprintf( opczErrorMsg,
                  "û�����÷���%d�ڲ��ӷ���%d����������Ϣ", eiSvcId, eiSubSeq );
      }
      else
      {
         sprintf( opczErrorMsg,
                  "��������%d�ڲ��ӷ���%d��������������Ϣʧ��",
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

      if( iRepFldFlag == 0 )/*FldSeq���Ƿ��ظ���*/
      {
         iCount = 1;
      }
      else if( iRepFldFlag == 1 )/*FldSeq�����ظ���*/
      {
      
            /** ȡ�ظ��������������� **/
         iRet =  ASM_GetRepTime( epstService, (stReqCfgAry.pstReqCfgNode)+iRow,
                                    &iCount );
         if( iRet != ASM_SUCC || iCount < 0 )
         {
            LOG4C ((LOG_FATAL,
                    "ȡ���ظ���������."
                    "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d]",
                     eiSvcId, eiSubSeq, iFldSeq));

            sprintf( opczErrorMsg,
                     "����%d�ڲ��ӷ���%d��������ȡ�ظ���������",
                     eiSvcId, eiSubSeq );
            free( stReqCfgAry.pstReqCfgNode );
            return ASM_FAIL;
         }
      }
      else 
      {
          LOG4C ((LOG_FATAL,
                  "���ظ���ʶ����."
                  "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d]",
                  eiSvcId, eiSubSeq, iFldSeq));

         sprintf( opczErrorMsg,
                  "����%d�ڲ��ӷ���%d�������������ظ����ʶ",
                  eiSvcId, eiSubSeq );
         free( stReqCfgAry.pstReqCfgNode );
         return ASM_FAIL;
      }

      /** ������Դ����Ҳ��Ϊ��ϵͳ���󣬲������� **/
      iRet =  ASM_CreateFldSrcCfgAry( iFldAsmId, &stFldSrcCfgAry );
      if( iRet != ASM_SUCC )
      {
          LOG4C ((LOG_FATAL,
                  "��������Դ������Ϣ�������."
                  "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d]",
                  eiSvcId, eiSubSeq, iFldSeq));

         sprintf( opczErrorMsg,
                  "����%d�ڲ��ӷ���%d�������鴴������Դ������Ϣ�������",
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
                    "��������Դֵ�������."
                    "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d]",
                     eiSvcId, eiSubSeq, iFldSeq));

            sprintf( opczErrorMsg,
                     "����%d�ڲ��ӷ���%d�������鴴������Դֵ�������",
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
                    "����ִ�д���."
                    "�����=[%d] �ڲ��ӷ������=[%d] ���=[%d] ���麯����=[%d]",
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
       * �����鷢������ʱ��ʣ���ֶδ������ֵ
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
