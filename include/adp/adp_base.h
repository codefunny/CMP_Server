/*
**file:   tpl_base.h
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#include "tool_base.h"
#include "global.h"
#include "udo.h"
#include "shm_base.h"
#include "svc_base.h"

#ifndef __ADP_BASE_H__
#define __ADP_BASE_H__

#define ADP_SUCC                               0
#define ADP_FAIL                               -1
#define TPCALL_SERVICE_IN_HOST_ERR             -3001
#define ADP_NO_SUB_DEF                         -3002
#define ADP_NO_HOST_DEF                        -3003
#define ADP_TCBSTI_ERR                         -3004
#define ADP_SMTS_ERR                           -3005
#define ADP_RMFS_ERR                           -3006
#define ADP_GETNODEID_ERR                      -3007
#define ADP_GIFMFC_ERR                         -3008
#define ADP_PKGTOMSG_ERR                       -3009
#define ADP_GPNI_ERR                           -3010
#define ADP_PKGTOMID_ERR                       -3011
#define ADP_SENDLEN_ERR                        -3012
#define ADP_RECVLEN_ERR                        -3013
#define ADP_RFDBEFORE_ERR                      -3014
#define ADP_UDO2FMLBEFORE_ERR                  -3015
#define ADP_RFDAFTER_ERR                       -3016
#define ADP_FML2UDOAFTER_ERR                   -3017
#define ADP_GETRETCODE_ERR                     -3018
#define ADP_GETWMSG_ERR                        -3019
#define ADP_MSGSEND_ERR                        -3020
#define ADP_GETRMSG_ERR                        -3021
#define ADP_MSGRCV_ERR                         -3022
#define ADP_CALLSVC_ERR                        -3023
#define ADP_UDOSIZE_ERR                        -4002
#define ADP_OPEN_ERR                           -4004
#define ADP_WRITE_ERR                          -4005
#define ADP_INSERT_ERR                         -4006
#define ADP_HANDLE_TYPE_ERR                    -4007
#define ADP_SEND_BAL_ERR                       -4008
#define ADP_TPINIT_ERR                         -4009
#define ADP_FINDSVC_ERR                        -4010
#define ADP_COMM_ERR                           -4020
#define ADP_COMMTYPE_ERR                       -4030
#define ADP_COMMMAC_ERR                        -4040
#define ADP_TIMEOUTQTYPE_ERR                   -4050
#define ADP_MSGRCV_AND_WTTBL_ERR               -4060
#define ADP_HOST_DEAL_FAIL                      3001
#define ADP_FIND_TXCODE_ERR                     3002

#define TRAN_CODE_LEN   20


typedef struct
{
   int iNodeId;
   int iTempletId;
   int iToMsgId;
   int iToMidId;
   int iPkgAppId;
   char aczTranCode[TRAN_CODE_LEN + 1];
} PkgNeedInfo;

typedef struct
{
   char aczAddr[16];
   int  iPort;
} ComAddr;

typedef struct _ADPService
{
   char aczServerIp[24];
   int  iServerPort;
   int  iTimeOut;
   char aczService[61];
   char aczTxnCode[41];
} ADPService;

#ifdef __cplusplus
extern "C" {
#endif

extern int  iHostNodeId;

/*
**function:get the trancode,nodeid,templetid and tomidid,tomsgid
**input:   1 subsvcid
**output:  2 all neede information
**return:  0-success,other-error,use database error
*/
int ADP_GetPkgNeedInfo( int eiSubSvcId, PkgNeedInfo *opstPkgNeedInfo );

/*
**����:��������������õ��ӷ��񷵻���
**����:1 �ṩ�ӷ���������ڵ���
**     2 ������ֵ(ǿ��ת��Ϊ�ַ���,������1ת��Ϊ"1",�����Ǹ�����)
**���:1 �ӷ��񷵻���(������������ӷ��񷵻ض����õ��øú���,������Ϊ0)
**����   0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/
int ADP_GetRetByCode( int eiHostNodeId, char *epczHostRetCode, int *opiSubRet );

/*
**����:ȡ�����ڵ��
**����:1 �ӷ������
**2 ����ṹ��Ϣ
**���:1 �����ڵ��
**���� 0-�ɹ�,����-ʧ��
*/
int ADP_GetHostNodeId( int eiSubSeq, SVC_Content *epstService,
                       int *opiHostNodeId );

/*
**function:get the trancode,nodeid,templetid and tomidid,tomsgid
**input:   1 svcid
**         2 subsvcid
**output:  1 all neede information
**return:  0-success,other-error
*/
int ADP_GetChanPkgNeedInfo( int eiSvcId, int eiSubSvcId,
                            PkgNeedInfo *opstPkgNeedInfo );

/*
**����:    ���ݽڵ�š�������ͱ��ķ����־���ɸ��ڵ�ֵΪ�յ�����
**�������:1 �ڵ��
**         2 ������
**         3 ���ķ����־
**�������:1 ������
**����ֵ   0 - �ɹ�,-1 - ʧ��
*/
int ADP_CreateNullListByTranCode( int eiNodeId, char *epczTranCode,
                                  int eiMsgDescrb, UDP opfList );

/*
**���� ���ݽ�����������ڵ�Ų�����Ӧ�ķ�����
**���� 1 �����ڵ��
**     2 ���״���
**��� 1 ������
**���� 0-�ɹ�,����-ʧ��
*/
int ADP_FindService( int eiHostNodeId, char *epczTranCode, char *opczSvcName );

/*
**����:��������������õ�����������
**����:1 �ṩ�ӷ���������ڵ���
**     2 ������ֵ(ǿ��ת��Ϊ�ַ���,������1ת��Ϊ"1",�����Ǹ�����)
**���:1 ����������(������������ӷ��񷵻ض����õ��øú���,������Ϊ0)
**���� 0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/

int ADP_GetChanRetCode( int eiHostNodeId, int eiChanNodeId,
                        char *epczHostRetCode, char *opczChanRet );

/*
**����:��������������õ�����������Ϣ
**����:1 �ṩ�ӷ���������ڵ���
**     2 ������ֵ(ǿ��ת��Ϊ�ַ���,������1ת��Ϊ"1",�����Ǹ�����)
**���:1 ����������Ϣ
**����   0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/

int ADP_GetChanRetMsg( int eiHostNodeId, int eiChanNodeId,
                       char *epczHostRetCode, char *opczErrMsg );
int ADP_GetChanRetCode( int eiHostNodeId, int eiChanNodeId,
                        char *epczHostRetCode, char *opczChanRet );


char * ADP_GetNodeName( int eiHostId );


int ADP_GetBalNeedInfo( int eiSubSvcId, NodeTranCfg *opstNodeTranCfg );

int ADP_GetCicsService( char *epczSubSvcName, char *opczSysId, 
                        char *opczServiceName );
/*
**���ܣ����ݷ���ź��ڲ��ӷ���Ų��ұ�������
**���룺1 �����
**      2 �ڲ��ӷ����
**���أ���������
*/
int ADP_FindMsgDscrb( int eiSvcId, int eiSubSeq );


/* ��Ҫ���ݲ�ͬ����ϵͳ������ͬ�Ķ���               */
/* �ر�Ҫ��: �����м�ҵ��ʱ������ϵͳ������ͬ�Ľڵ� */
int ADP_Transaction_Que( int eiNodeId, char *epczSendBuf, int eiSendLen,
                         char *opczRecvBuf, int *opiRecvLen );

/* �����ӷ���Ų齻�״��� */
int ADP_FindTranCode( int eiSubSvcId, char *opczTranCode );

int ADP_RetCodeMap( int eiHostNodeId, int eiChanNodeId, char *epczHostRetCode,
                      char *opczChanRet, char *opczChanMsg );

int ADP_Transaction_WBPP (int eiNodeId, long lType,
                          char *epczSendBuf, int eiSendLen,
                          char *opczRecvBuf, int *opiRecvLen);

int ADP_Transaction_As400Tcp( int eiNodeId, char *epczSendBuf, int eiSendLen,
                              char *opczRecvBuf, int *opiRecvLen );

/**
  * �����ӷ����Ų���ADP��������ַ,�˿ںͷ�����
  * �ɹ�����0, ʧ�ܷ��� -1
  */
int ADP_GetService( int eiSubSvcId, ADPService *opstAdpService );

long GenSysSer(  );

int ADP_GetDictId( int iNodeId );

int ADP_SysError( int eiSubID, int eiSubSeq,
                  UDP epfReqList, UDP opfRspList,
                  SVC_Content * epstService );

extern double GetCurrentMillTime();

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
