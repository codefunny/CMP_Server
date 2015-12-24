#include <sys/types.h>
#include <unistd.h>

#include "apr.h"
#include "apr_portable.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "easysock.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifndef __APPLICATION_BASE_H__
#define __APPLICATION_BASE_H__

typedef struct ItemConfig
{
   char              *m_pczKey;
   char              *m_pczVal;
   struct ItemConfig *m_pstNext;
} ItemConfig;

/**
  * ��ϵͳ�Ĵ���
  */
struct SystemRegister
{
   bool  m_bActive;           /*ϵͳ�״̬*/
   char  m_aczName[128];      /*��ϵͳ��*/
   int   m_iPort;             /*��ϵͳʹ�ö˿�*/
   int   m_iMinProcess;       /*��С������*/
   int   m_iMaxProcess;       /*��������*/
   int   m_iActiveProcess;    /*�������*/
   pid_t m_tManagerPid;       /*��ϵͳ������� pid*/
};

enum COMMAND1 { NO_CMD = 0, LOAD_CONFIG, LOAD_LIBRARY, CONNECT_DB };
typedef enum COMMAND1 COMMAND;
/**
  * �ӽ��̵ǼǱ�
  */
struct ProcessRegister
{
   bool    m_bActive;      /*�ӽ��̻״̬, ����������*/
   bool    m_bDoTxn;       /*�Ƿ����ڴ���ҵ��*/
   COMMAND m_tCommand;     /*�������������, ��ʱ����, ������*/
   pid_t   m_tPid;         /*�ӽ�������*/
   int     m_iRunTimes;    /*��ǰ�������д���, �ӽ�������*/
   time_t  m_tBegin;       /*��ǰ���̿�ʼ���յ����ݺ�Ŀ�ʼʱ��, �ӽ�������*/
};

#ifdef __cplusplus
extern "C" { 
#endif

int makeListener( apr_socket_t **pstListner, apr_pool_t *pstPool,
                  int iPort, int iFamily, int iType );

int makeUdpSvrSock( apr_socket_t **pstListener, apr_pool_t *pstPool, 
                    int iPort);

int setSockOpts (apr_socket_t *pstSock);

int makeClient( apr_socket_t **pstClient, apr_pool_t *pstPool, char *pczServer,
                int iPort, int iFamily, int iType, int iTimeOut );

int ap_tcp_write ( apr_socket_t *sock, char *buf, int len );

int ap_tcp_read ( apr_socket_t *sock, char *buf, int len );

char *ap_basename(char *str);

char *ap_dirname(char *str);
/**
  * �����������ӹ����ڴ�
  */
void * shm_get( int iShmKey, int iSize );

void loadEnviorment( char *moduleName );

char * getConfigValue(ItemConfig stConfig, const char *pczItem);

int  loadConfig(const char *pczName, ItemConfig *pstConfig);

/**
  * ����ļ��� $PATH �����������Ƿ����
  */
bool fileExistAndExec( char *pczFile );


#ifdef __cplusplus
}
#endif

#endif

