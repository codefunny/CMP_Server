#include <sys/types.h>
#include <unistd.h>

#include "apr.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_shm.h"
#include "apr_signal.h"
#include "apr_hash.h"

#include "ap_base.h"

#include "udo.h"

#ifndef __APPLICATION_HANDLER_H__
#define __APPLICATION_HANDLER_H__

typedef struct _AxpTxnPara AxpTxnPara;
struct _AxpTxnPara
{
   int  m_iNodeId;
   char m_aczTranCode[21];
   int  m_iDictId;
};

#ifdef __cplusplus
extern "C" {
#endif

extern AxpTxnPara stAxpTxnPara;

/**
  * ҵ����ϵͳ������
  */
int onSysStart( char * );           /*ϵͳ��������*/
int onSysEnd( void );             /*ϵͳ��������*/
int onTxStart( char * );            /*���׿�ʼ����*/
int onTxEnd( void );              /*���׽�������*/
int preCheck( char *pczBuffer, int iLen );
int postCheck( UDP pUdo, char *pczBuffer, int iLen);

/**
  * pczBuffer �����ڱ����������, ��������ʼ���ȱ����� *ptLen ��
  * ����ĳ��Ȳ���, ����Ҫ�� recvBuffer ���������·���
  * �������ʹ�� apr_palloc ����, ��һ������������ gpstLocalPool
  * ϵͳ���Զ�����ͨ�� apr_palloc ����Ŀռ�
  * ��ֹʹ�� malloc �ȷ��亯��
  */
int recvBuffer( char **pczBuffer, apr_size_t *ptRecvLen );
int sendBuffer( char *pczBuffer, apr_size_t iSendLen );

int getTxCode( char *pczBuffer, char *pczTxCode );
int getTxCode_8583( char *pczBuffer, char *pczTxCode , int iRecvLen, int iDictId);
int getTxCodeByLen( char *pczBuffer, int nBufferLen,  char *pczTxCode );

#ifdef __cplusplus
}
#endif

#endif

