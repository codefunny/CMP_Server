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
  * 子系统寄存器
  */
struct SystemRegister
{
   bool  m_bActive;           /*系统活动状态*/
   char  m_aczName[128];      /*子系统名*/
   int   m_iPort;             /*字系统使用端口*/
   int   m_iMinProcess;       /*最小进程数*/
   int   m_iMaxProcess;       /*最大进程数*/
   int   m_iActiveProcess;    /*活动进程数*/
   pid_t m_tManagerPid;       /*子系统管理进程 pid*/
};

enum COMMAND1 { NO_CMD = 0, LOAD_CONFIG, LOAD_LIBRARY, CONNECT_DB };
typedef enum COMMAND1 COMMAND;
/**
  * 子进程登记表
  */
struct ProcessRegister
{
   bool    m_bActive;      /*子进程活动状态, 父进程设置*/
   bool    m_bDoTxn;       /*是否正在处理业务*/
   COMMAND m_tCommand;     /*即将处理的命令, 暂时不用, 待扩充*/
   pid_t   m_tPid;         /*子进程设置*/
   int     m_iRunTimes;    /*当前进程运行次数, 子进程设置*/
   time_t  m_tBegin;       /*当前进程开始接收到数据后的开始时间, 子进程设置*/
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
  * 创建或者连接共享内存
  */
void * shm_get( int iShmKey, int iSize );

void loadEnviorment( char *moduleName );

char * getConfigValue(ItemConfig stConfig, const char *pczItem);

int  loadConfig(const char *pczName, ItemConfig *pstConfig);

/**
  * 检查文件在 $PATH 环境变量中是否存在
  */
bool fileExistAndExec( char *pczFile );


#ifdef __cplusplus
}
#endif

#endif

