/*
** file:   svc_basic.h
*/
#include "udo.h"
#include "cmp.h"
#include "shm_base.h"

#ifndef __SERVICE_BASIC_H__
#define __SERVICE_BASIC_H__

/*��CMP�������ṩ�����һ������,ÿ����Ҫ������Ҫ���ĸ�ֵ*/
#define CMP_NODE_ID                           100

#define CMP_SYS_ERR                          -5010
#define CMP_PKG_ERR                          -5020
#define CMP_COM_ERR                          -5030
#define CMP_TUX_ERR                          -5040
#define CMP_CHK_ERR                          -5050
#define CMP_GSUBCFG_ERR                      -5050
#define CMP_ASM_ERR                          -5060
#define CMP_ASM_STRFPIN_ERR                  -5061
#define CMP_ASM_SENCPIN_ERR                  -5062
#define CMP_ASM_STRFPWD_ERR                  -5063
#define CMP_ASM_SENCPWD_ERR                  -5064
#define CMP_COMMAC_ERR                       -5070
#define CMP_TUXTMOUT_ERR                     -5080

#define CMP_HOST_ERR                         8888

#define SVC_SUCC                 0
#define SUB_SUCC                 0
#define SVC_BASE                 800
#define SVC_GETCFG_ERR           SVC_BASE + 1
#define SUB_KEYCFG_ERR           SVC_BASE + 2
#define SUB_GETCFG_ERR           SVC_BASE + 3
#define SUB_CHK_ERR              SVC_BASE + 4
#define SUB_ADDNEW_ERR           SVC_BASE + 5
#define SUB_ASM_ERR              SVC_BASE + 6
#define SUB_SAVE_ERR             SVC_BASE + 7
#define SVC_NO_MEM               SVC_BASE + 8
#define SVC_SUBINIT_ERR          SVC_BASE + 9
#define SVC_INIT_ERR             SVC_BASE + 10
#define SVC_SUBFLOW_ERR          SVC_BASE + 11
#define SVC_SUB_NOTFOUND         SVC_BASE + 12
#define SUB_SYS_ERR              SVC_BASE + 13

#define SUB_ERR_NOOP             0 /*�޴���                 */
#define SUB_ERR_TRANLOG          1 /*ֻ�ǳ�����־           */
#define SUB_ERR_BALANCE          2 /*��Ҫ����,ʧ����Ҫ����־*/
#define SVC_CORRECT_RET          3 /*��������������Ҫ����   */
#define SVC_ERR_RETURN           4 /*���������Ӧ,����Ҫ����*/
#define SVC_ERR_RSP_ASM          5 /*���������Ӧ*/

#define SVC_TRANCODE_LEN         20
#define SVC_NAME_LEN             40
#define SVC_NOTE_LEN             100
#define SVC_ERRMSG_LEN           200
#define SVC_STAN_LEN             27
#define SUB_NAME_LEN             40

#define SUB_ERR_NOOP             0 /*�޴���                 */
#define SUB_ERR_TRANLOG          1 /*ֻ�ǳ�����־           */
#define SUB_ERR_BALANCE          2 /*��Ҫ����,ʧ����Ҫ����־*/
#define SVC_ERR_RSP_ASM          5 /*���������Ӧ           */

extern ProcessManager     *gpstProcess;


typedef struct _SUB_SvcList SUB_SvcList;
typedef struct _SVC_Content SVC_Content;
typedef struct _SVC_Config  SVC_Config;

extern char aczChannelLog[2048];

typedef struct _SUB_SvcCfg
{
   int  iSubId;                        /*ȫ���ӷ����        */
   int  iTempletId;                    /*�ӷ���ģ��          */
   int  iRecordsPerTime;               /*ÿ�η��͵�����ظ���*/
   char aczSubTranCode[20 + 1];        /*�ӷ����״���      */
   char aczSubName[SUB_NAME_LEN + 1];  /*�ӷ����߼�����      */
} SUB_SvcCfg;

struct _SVC_Config
{
   int  iNodeId;
   char aczTranCode[SVC_TRANCODE_LEN + 1];
   int  iSvcId;
   char aczSvcName[SVC_NAME_LEN + 1];
   int  iRepTimes;
   int  iTimeOut;
   int  iBalanceFlag;
   char aczSvcNote[SVC_NOTE_LEN + 1];
};

struct _SUB_SvcList
{
   int           iSubSvcSeq;           /*�ӷ����ڷ����б��            */
   int           iHostId;              /*�������                      */
   int           iMsgAppId;            /*����Ӧ�ú�                    */
   int           iIsBatch;             /*�Ƿ������ӷ���                */
   int           iBatchMode;           /*����ģʽ                      */
   int           iReadKeyId;           /*������Keyֵ                   */
   int           iWriteKeyId;          /*д����Keyֵ                   */
   int           iRunResultCode;       /*�ӷ�������ֵ                  */
   int           iErrorHandleType;     /*����������                  */
   /*����ֶ�ָ�������������ʱ��Ĵ������,2-����,1-ֻ����־,0-�޴��� */
   char          aczErrorMessage[SVC_ERRMSG_LEN];
   int           iSubId;                         /*ȫ���ӷ����        */
   int           iTempletId;                     /*�ӷ���ģ��          */
   int           iRecordsPerTime;                /*ÿ�η��͵�����ظ���*/
   char          aczTranCode[20 + 1];            /*�ӷ����״���      */
   char          aczSubName[SUB_NAME_LEN + 1];   /*�ӷ����߼�����      */
   SUB_SvcCfg    stCorrSubCfg;         /*��ȷ�ӷ�������                */
   SUB_SvcCfg    stErrSubCfg;          /*�������ӷ�������            */
   UDP           pfReqList;            /*������Ϣ����                  */
   UDP           pfAnswList;           /*��Ӧ��Ϣ����                  */
   SVC_Content  *pstService;           /*����ṹָ��                  */
   UDP           pfBalanceList;        /*������������Ϣ����          */
   SUB_SvcList  *pstNextSubSvc;        /*��һ���ӷ���ָ��              */
   int           iStartErrorFlow;      /*��־���������ʱ���Ƿ�ʼ��������*/
   int           iOutTime;             /*��ʱʱ��*/
   int           iLstId;
};

struct _SVC_Content
{
   int           iSvcId;                         /*�����              */
   int           iChannelId;                     /*����������          */
   int           iPkgAppId;                      /*����Ӧ�ú�          */
   int           iReadKeyId;                     /*������Keyֵ         */
   int           iWriteKeyId;                    /*д����Keyֵ         */
   int           iSubListId;                     /*�ӷ���ʵ�����е�����*/
   short         shStep;                         /* �ӷ������в���     */
   char          aczErrorMessage[SVC_ERRMSG_LEN];/*�������д�����Ϣ    */
   char          aczElSerial[SVC_STAN_LEN];      /*CMP����ˮ��  */
   int           iErrorType;                     /*��־ϵͳ���д�������*/
   UDP           pfReqList;                      /*������Ϣ����        */
   UDP           pfAnswList;                     /*��Ӧ��Ϣ����        */
   UDP           pfLogList;                      /*����Ҫ����־����    */
   SUB_SvcList   stSubSvcHead;                   /*�ӷ���ͷ�ڵ�        */
   SUB_SvcList  *pstSubSvcTail;                  /*���һ���ӷ���ָ��  */
   SUB_SvcList  *pstSubSvcCurr;                  /*��ǰ��ӷ���ָ��  */
   /*������Ϊ�����������ṩ�ı�Ҫ��Ϣ*/
   int           iCommId;                        /*����ͨѶsocket      */
   int           iStatus;                        /*�����ʼ״̬        */
};

#ifdef __cplusplus
extern "C"
{
#endif

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
              Service  *epstSvcConfig, int eiPkgAppId );

/*
** ���ܣ������ݿ�ȡ�ڲ������
** ���������1 �����ڵ��
**           2 ����Ӧ�ú�
**           3 ���״���
** ���������1 �ڲ���������
** ����ֵ��0-�ɹ�����������
*/

int SVC_GetConfig( int eiChanId, int eiPkgAppId,
                   char *epczTranCode, Service *opstServiceCfg );

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
                         SVC_Content *epstSvcList, char *opczValue, int *opiLen  );

/*
**���ܣ��ͷŷ���ռ�õĿռ�
**���������1 ����ָ��
**���������  ��
**����ֵ��    ��
*/
void SVC_Free( SVC_Content *epstService );

/*
**����:���������ڵ�����÷�����Ҫ��4���ؼ�ֵ
**����:1 �����ڵ��
**     2 �����������
**���:1 ����ṹָ��
**����:��
*/
void SVC_SetKeyValue( int eiChanNodeId, UDP epfSvcReq,
                      SVC_Content *opstService );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
