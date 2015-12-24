/*
**    tool_base.h
*/
#include <stdarg.h>
#include <string.h>
#ifndef _TOOL_TRACE_H_
#define _TOOL_TRACE_H_

/*****
#ifndef SYS_HAED_ERR
#define SYS_HAED_ERR   1
#endif

#ifndef SYS_LOGIC_ERR
#define SYS_LOGIC_ERR  2
#endif

#ifndef KEY_DEBUG
#define KEY_DEBUG      3
#endif

#ifndef GENL_DEBUG
#define GENL_DEBUG     4
#endif
*****/

/*
**  ������붨�塣
*/
#define TOOL_FILE_OPEN_ERROR         -501 
#define TOOL_FILE_SEEK_ERROR         -502
#define TOOL_RENAME_ERROR            -503


/*
**  ��������
*/
#define TOOL_DATE_AND_TIME_LEN       30
#define TOOL_COMMON_MSG_LEN          2048
#define TOOL_MAX_MSG_LEN	     4096
#define TOOL_LINE_LENGTH	     80
#define TOOL_FILE_NAME_LEN           80

#define TOOL_FILE_SIZE               2000000

#define TOOL_SHMKEYFILE  "Shm.key"
#define TOOL_ERR_SHMKEYFILE   "ErrShm.key"
#define TOOL_ERR_MSGKEYFILE   "ErrMsg.key"
#define TOOL_SEMKEYFILE  "Sem.key"

#define TOOL_MULT_SEMKEYFILE  "MultSem.key"
#define TOOL_MAX_SUB_MESG  300

#define TOOL_KEYID  8
#define TOOL_PATH_LEN   80
#define TOOL_SQNO_LEN   12
#define TOOL_NODE_GAP   10

#define TOOL_SYS_TRACE_LEVEL      0
#define TOOL_DISPLAY_TRACE_LEVEL  1
#define TOOL_DATABASE_TRACE_LEVEL 2
#define TOOL_TRAN_TRACE_LEVEL     3

#define TOOL_LOG_LEVEL0    TOOL_SYS_TRACE_LEVEL
#define TOOL_LOG_LEVEL1    TOOL_DISPLAY_TRACE_LEVEL
#define TOOL_LOG_LEVEL2    TOOL_DATABASE_TRACE_LEVEL
#define TOOL_LOG_LEVEL3    TOOL_TRAN_TRACE_LEVEL

#ifdef DISPLAY
#undef DISPLAY
#endif
#define DISPLAY 1

#ifdef TRACE
#undef TRACE
#endif

/*ȫ�ֱ���,�����ִ���ļ�����,ΪTOOL_Trace�ṩ����ִ���ļ�����־�Ĺ���*/

typedef struct _TOOL_LogInfo
{
   int iPid;                    /* ��ǰ���ý��̵�PID                */
   int iCanWrite;               /* �жϵ�ǰָ������־�ļ��Ƿ��ܴ� */
   int iLogLevel;               /* ��������TRACE_LEVELָ����ָ����ֵ*/
   char aczFileName[256];       /* TOOL_Logʹ�õ��ļ���             */
   char aczStanId[64];          /* ��ˮ��                           */
} TOOL_LogInfo;

#define TRACE 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *SigFunc )( int );

/*
**  �����������ȫ�ֱ�������
*/
extern int iTOOL_ErrorNO;

extern char *pczExecFileName;

/* 
**  �ⲿ����˵��
*/
void TOOL_LogInit( char *epczSerial, char *epczBussInfo );

void TOOL_Log( int eiTraceLevel, char *epcMessage, ... );

void TOOL_Trace( int eiTraceLevel, 
                 char *epcFileName, char *epcMessage, ... );

void TOOL_Dump( int eiTraceLevel,
                char *epcFileName,
                unsigned char *epbyDumpedData,
                int eiDumpedDataSize );

void TOOL_GetSystemYYYYMMDD( char *pcYYYYMMDD );
void TOOL_GetSystemhhmmss( char *pcYYYYMMDD );

void TOOL_Debug( char *epcFileName, char *epcMessage, ... );

void TOOL_DebugDump( char *epcFileName,
                     unsigned char *epbyDumpedData,
                     int eiDumpedDataSize );

void TOOL_GetCurrTime( char *epcDateAndTime );

void TOOL_GetTimeString( char *epcTime );


void TOOL_BinaryDump( int eiFileDes,
                      unsigned char *epbyMessage,
                      int eiDumpedDataSize );

void TOOL_Message( int eiTraceLevel,
                   char *epcSender, char *epcMessage );


/*
**  ȥ���ַ���ǰ��ո� 
*/
char *TOOL_StripSpace( char *);

/*
** ��������Ϣ���С��źŵơ������ڴ����õ� KEY ֵ
*/
/*extern key_t TOOL_Ftok ( char * );*/

/*
**   ȡÿһ������������Ϣ�����õ���Ϣ��ʶ
*/
int TOOL_GetMsgType( int , long *);

/*
**  �����źŹ���
*/
SigFunc TOOL_Signal( int , SigFunc );


/*
**   ��־����
*/
void TOOL_Printf(va_list, ...);

/*
**    ���ú����źż�
*/
int TOOL_IgnSignal ( void );

/*
** ȡ��ȷʱ��
*/
char *TOOL_TimeTell( char * );

/*
**  ȡʱ�䵽�� hh:mm:ss
*/
void TOOL_GetTime ( char * );

/*
**  ȡʱ�䵽���� hh:mm:ss:ms
*/
void TOOL_GetMicroTime ( char * );

int pubParseStr(char *epczStr, char chSep, char *oppczRet[],
                unsigned int max_flds);

#ifdef __cplusplus
}
#endif


#endif




/*
**	end of file tool_base.h
*/
