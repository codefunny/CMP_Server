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
  * 业务子系统管理器
  */
int onSysStart( char * );           /*系统启动调用*/
int onSysEnd( void );             /*系统结束调用*/
int onTxStart( char * );            /*交易开始调用*/
int onTxEnd( void );              /*交易结束调用*/
int preCheck( char *pczBuffer, int iLen );
int postCheck( UDP pUdo, char *pczBuffer, int iLen);

/**
  * pczBuffer 是用于保存接收内容, 缓冲区初始长度保存在 *ptLen 中
  * 如果改长度不够, 则需要在 recvBuffer 函数中重新分配
  * 分配必须使用 apr_palloc 函数, 第一个参数必需是 gpstLocalPool
  * 系统会自动回收通过 apr_palloc 分配的空间
  * 禁止使用 malloc 等分配函数
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

