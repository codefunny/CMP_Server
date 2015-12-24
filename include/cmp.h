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

/* �������̵�״̬���� */
#define P_LISTEN	0	/* ����           */
#define P_RECEIVE	1	/* ����           */
#define P_DISPOSE	2	/* ����           */
#define P_REBUILD	3       /* �ؽ�           */

/* �������Ѳ��ʱ���� */
#define PATROLTIME	2

/* ����״̬ */
#define P_DEAD		0
#define P_ALIVE		1

/* STOP���ֹͣ���е��ӽ���,��ʹ�������Ҳ�˳�                      */
/* PAUSE��ʹ�ӽ�����һ״̬��ΪWAIT,�����ӽ���ÿһ���Ӳ�ѯһ����һ״̬ */
/* ֱ����������յ����¿�ʼ������,�ӽ��̽����¿�ʼ����                */
/* REPORT���Ӱ���ӽ��̴���                                         */
/* ����ϵͳֻ֧��REPORT��STOP����                                     */
#define AP_CMD_STOP     0
#define AP_CMD_RUN      1
#define AP_CMD_REPORT   2
#define AP_CMD_PAUSE    3
#define AP_CMD_RESTART  4

/* ��������ȫ�ֱ��� */
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
   char *pShm;                   /* ���ù����ڴ��ַ       */
};

typedef struct _ProcessManager ProcessManager;
struct _ProcessManager
{
   pid_t  tPid;              /* �������PID                          */
   time_t tStartTime;        /* ����ĳ��״̬��ʱ��                   */
   int    iAlive;            /* if the process alive                 */
   int    iAliveStatus;      /* �ӽ��̻״̬                       */
   short  shRunTimes;        /* ��ǰ�������д���                     */
   char   aczStanId[21];     /* �ڲ�������ˮ�� ��ǰʱ��+PID+���д��� */
};
/* iAliveֻ���ɸ�������д,��iAlive=P_ALIVEʱ,iAlvieStatusֻ�����ӽ�����д */
/* ������ʱ��������־�ɵ���������ɵ����� */

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
