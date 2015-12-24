/*
**In this stage,we just define some main struct.
**First,our frame model is that there is a process manager and it forks
**some processes defined by MIN_HANDLE.They all accept the client request.
**If the handle processes are too little to do the request,then the manager
**fork an other process.After it has done the things,then clean it if necessary
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <termio.h>

#ifndef __CMP_COMM_H__
#define __CMP_COMM_H__

#define LOGFILE "/tmp/comm.log"

/* 监听进程的状态定义 */
#define P_LISTEN	0	/* 监听           */
#define P_RECEIVE	1	/* 接收           */
#define P_DISPOSE	2	/* 处理           */
#define P_REBUILD	3       /* 重建           */

/* 管理进程巡检时间间隔 */
#define PATROLTIME	2

/* 进程状态 */
#define P_DEAD		0
#define P_ALIVE		1

/* STOP命令将停止所有的子进程,并使管理进程也退出                      */
/* PAUSE将使子进程下一状态改为WAIT,并且子进程每一分钟查询一次下一状态 */
/* 直到管理进程收到重新开始的命令,子进程将重新开始处理                */
/* REPORT命令不影响子进程处理                                         */
/* 现在系统只支持REPORT和STOP命令                                     */
#define AP_CMD_STOP     0
#define AP_CMD_RUN      1
#define AP_CMD_REPORT   2
#define AP_CMD_PAUSE    3
#define AP_CMD_RESTART  4

/* 新增部分全局变量 */
typedef struct _GlobalInfo GlobalInfo;
struct _GlobalInfo
{
   char cExecFile[51];
   int  iPort;
   long iCfgShmKey;
   int  iNodeId;
   int  iTimeOut;
   int  iRunTimes;
   int  iMinProc;
   int  iMaxProc;
   char cLogIniFile[128];
   char cSysEnvFile[128];
   char cSysCommand[256];
   char *pShm;                   /* 配置共享内存地址       */
};

typedef struct _ProcessManager ProcessManager;
struct _ProcessManager
{
   pid_t  tPid;              /* 处理进程PID                          */
   time_t tStartTime;        /* 处于某个状态的时间                   */
   int    iAlive;            /* if the process alive                 */
   int    iAliveStatus;      /* 子进程活动状态                       */
   short  shRunTimes;        /* 当前进程运行次数                     */
   char   aczStanId[21];     /* 内部交易流水号 当前时间+PID+运行次数 */
};
/* iAlive只能由父进程填写,当iAlive=P_ALIVE时,iAlvieStatus只能由子进程填写 */
/* 现在暂时不考虑日志由单独进程完成的问题 */

void AP_SetGlobalVar();
void AP_MutexP( int iMutexId );
void AP_MutexV( int iMutexId );
void AP_Init();
int  AP_MakeManager( ProcessManager *,GlobalInfo *);

int  AP_MakeChild( ProcessManager *, int, int );
int  AP_MakeSocket( int eiListenPort );
void AP_ChildMain( ProcessManager *, int );
void ServerExit( int );

int    SHM_DetachShareMemory( void * );
void * SHM_MakeShm();
int    SHM_GetShmSize( FILE *, int *, int * );

int    SEM_get( int );
int    SEM_Create( int );
int    SEM_Lock( int );
int    SEM_Unlock( int );
void   SEM_Remove( int );

int  AP_SetupHandler( GlobalInfo * );

void Delay( int );

#endif
