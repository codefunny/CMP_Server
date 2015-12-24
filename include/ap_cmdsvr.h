#include <sys/types.h>
#include <unistd.h>

#include "apr.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_shm.h"
#include "apr_signal.h"
#include "apr_hash.h"
#include "ap_manager.h"

#include "ap_base.h"

#ifndef __APPLICATION_COMMAND_SERVER_H__
#define __APPLICATION_COMMAND_SERVER_H__

struct CommandConfig
{
   int m_iPort;                /*ϵͳ����˿�*/
   int m_iGrpShmKey;           /*ϵͳ�������ڴ� KEY*/
   int m_iPrcShmKey;           /*ϵͳ������̹����ڴ� KEY*/
   int m_iMaxSys;              /*�����ϵͳ����*/
   int m_iMaxChildPerSys;      /*ÿ����ϵͳ��������*/
};

/**
  * ϵͳ������
  */
class CommandServer
{
private:
   apr_socket_t    *m_pstListen;              /*���������˿�*/
   apr_pool_t      *m_pstRoot;                /*ϵͳȫ�ֻ��� root*/
   apr_pool_t      *m_pstLocal;               /*ϵͳ�ֲ����� root*/
   char             m_aczCommand[1024];       /*ϵͳ���յ�����*/
   SystemConfig    *m_pstConfig;              /*ϵͳ������Ϣ*/
   SystemRegister  *m_pstSystem;              /*��ϵͳ��Ϣ*/
   ProcessRegister *m_pstProcess;             /*ϵͳ���̵Ǽ���*/

private:
   int setupSignals( void );                /*��ʼ���ź�����*/
   int initShareMemory( void );             /*��ʼ�������ڴ�*/

   int doCommand( apr_socket_t * );         /*������ִ������*/
   int start( apr_socket_t *, char * );     /*������ϵͳ*/
   int restart( apr_socket_t *, char * );   /*������ϵͳ*/
   int stop( apr_socket_t *, char * );      /*ֹͣ��ϵͳ*/
   int report( apr_socket_t *, char * );    /*�г�������ϵͳ*/

   pid_t runAnyManager( char *, int );        /*����*/

   void maintenance( void );                /*ά��ϵͳ��Ϣ*/

   int getEmptyIndex( void );
   int getRunningIndex( char *pczName );
   void saveBusiManager( int iIndex, char *pczName, pid_t );

   void checkExitedSystem( void );

public:
   CommandServer();
   ~CommandServer();

   /**
     * ��ϵͳ������������Ϣ: ����˿�
     *                       �����ϵͳ��
     *                       ÿ����ϵͳ������������
     *                       �����ڴ� IPCKEY
     *                       ��ϵͳ������ʱ��(��λ��)
     */
   int loadConfig( char *pczXmlConfig );

   /**
     * ����ϵͳ������
     */
   int run( void );

   /**
     * ϵͳ����
     */
   void cleanup( void );
};

#endif

