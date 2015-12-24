/*
** file:sub_base.h
*/
#include "svc_base.h"
#ifndef __SUB_BASE_H__
#define __SUB_BASE_H__

typedef struct _SUB_Config
{
   int  iSvcId;                        /*�����              */
   int  iMsgAppId;                     /*�ӷ���ʹ�ñ���Ӧ�ú�*/
   int  iHostNodeId;                   /*�ӷ��������ڵ��    */
   int  iSubSeq;                       /*�ڲ��ӷ����        */
   int  iErrorHandleType;               /*����������       */
   int  iOutTime;                      /*�ӷ���ʱʱ��      */
   int  iRetryTime;                    /*��ʱ���Դ���        */
   int  iBatFlag;                      /*������־            */
   int  iBatMode;                      /*����ģʽ            */
   int  iLstId;                        /*����                */
   SUB_SvcCfg stCorrSubCfg;            /*��ȷ�ӷ�������      */
   SUB_SvcCfg stErrSubCfg;             /*�����ӷ�������      */
} SUB_Config;

typedef struct
{
   int iReadKeyId;               /*������KEYֵ         */
   int iWriteKeyId;              /*д����KEYֵ         */
} SUB_RWKey;

typedef struct _SUB_TimeCount
{
   int iTime;
   int iHostId;
} SUB_TimeCount;

#ifdef __cplusplus
extern "C" {
#endif

/*
** ���ܣ����ݷ���ź��ڲ��ӷ���Ŷ��ӷ���������Ϣ
** ���������1 �����
**           2 �ڲ��ӷ����
** ���������1 �ӷ�������ָ��
** ����ֵ��    0�ɹ�������ʧ��
*/
int SUB_GetSubConfig( int eiSvcId, int eiInternalId,
                      SUB_Config *opstSubSvcConfig );

/*
** ���ܣ�����µ��ӷ���
** ���������1 �ӷ�������ָ��
**           3 �����к�
**           4 д���к�
** ���������1 ����ṹ
** ����ֵ      0�ɹ�������ʧ��
*/
int SUB_AddNew( SVC_Content *eopstService, SUB_Config *epstSubSvcConfig,
                int eiReadQueId, int eiWriteQueId );

/*
** ���ܣ����ݽڵ�ŵõ���д����KEYֵ
** ���������1 �ڵ��
** ���������1 ������KEYֵ
**           2 д����KEYֵ
** ����ֵ��    0�ɹ�������ʧ��
*/
int SUB_GetKeyVal( int eiNodeId, int *opiReadKeyId, int *opiWriteKeyId );

/*
**���ܣ��ͷ��ӷ����������Ϣ����Ӧ��Ϣ����
**���������1 �ӷ���ڵ�ָ��
**���������  ��
**����ֵ��    ��
*/
void SUB_Free( SUB_SvcList *epstSubServie );

#ifdef __cplusplus
}
#endif

#endif
