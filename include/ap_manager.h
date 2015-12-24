#include <sys/types.h>
#include <unistd.h>

#include "apr.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_shm.h"
#include "apr_signal.h"
#include "apr_hash.h"

#include "ap_base.h"

#ifndef __APPLICATION_FRAMEWORK_H__
#define __APPLICATION_FRAMEWORK_H__

/**
  *
  */
struct SystemConfig
{
   int m_iListenPort;
   int m_iMaxSys;
   int m_iMaxChild;
   int m_iGrpShmKey;
   int m_iPrcShmKey;
};

/**
  * ϵͳ������
  */
class SystemManager
{
private:
   apr_socket_t    *m_pstListen;              /*���������˿�*/
   apr_pool_t      *m_pstRoot;                /*ϵͳȫ�ֻ��� root*/
   apr_pool_t      *m_pstLocal;               /*ϵͳ�ֲ����� root*/
   char             m_aczCommand[1024];       /*ϵͳ���յ�����*/
   SystemRegister  *m_pstSystem;              /*��ϵͳ��Ϣ*/
   SystemConfig    *m_pstConfig;              /*ϵͳ������Ϣ*/
   ProcessRegister *m_pstProcess;             /*ϵͳ���̵Ǽ���*/

private:
   int setupSignals( void );                /*��ʼ���ź�����*/
   int initShareMemory( void );             /*��ʼ�������ڴ�*/

   int doCommand( apr_socket_t * );         /*������ִ������*/
   int start( apr_socket_t *, char * );     /*������ϵͳ*/
   int stop( apr_socket_t *, char * );      /*ֹͣ��ϵͳ*/
   int report( apr_socket_t *, char * );    /*�г�������ϵͳ*/

   void maintenance( void );                  /*ά��ϵͳ��Ϣ*/

   int initSystemInfo( void );              /*��ʼ��ϵͳ������Ϣ*/

   int getEmptyIndex( void );
   int getRunningIndex( char *pczName );
   void saveBusiManager( int iIndex, char *pczName );

public:
   SystemManager();
   ~SystemManager();

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

/**
  * ������Ϣ
  */
struct BusiConfig
{
   char              *m_pczKey;
   char              *m_pczVal;
   struct BusiConfig *m_pstNext;
};

/**
  * ҵ����ϵͳ������
  */
class BusiManager
{
private:
   int    m_iSysNo;                   /*ϵͳ���*/
   int    m_iShmKey;                  /*�����ڴ� KEY*/
   int    m_iMinProcess;              /*��С������*/
   int    m_iActiveProcess;           /*�������*/
   int    m_iMaxProcess;              /*��������*/
   int    m_iMaxAllowChild;           /*������������*/
   char   m_aczName[128];             /*��ϵͳ����*/
   ProcessRegister *m_pstProcess;     /*ϵͳ���̵Ǽ���*/

   apr_socket_t *m_pstBusiListener;   /*ҵ������ socket*/
   apr_socket_t *m_pstCmdListener;    /*�������� socket*/

   apr_pool_t  *m_pstPool;            /*��ϵͳ�Ļ����*/
   apr_pool_t  *m_pMaintanence;       /*ά�����̵Ļ����*/
   apr_shm_t   *m_pstShm;             /*apache���͹����ڴ�*/
   BusiConfig   m_stConfig;           /*������Ϣ*/

private:
   void maintenance( void );            /*ά����ϵͳ��Ϣ*/

   int  closeListener( void );

   void doCommand( apr_socket_t *pstCommand );
   void doStopCmd( apr_socket_t *pstCommand );
   void doLoadCfgCmd( apr_socket_t *pstCommand );
   void doLoadLibCmd( apr_socket_t *pstCommand );
   void doConnectDbCmd( apr_socket_t *pstCommand );

   int  makeChild( int iSlot );  /*�����ӽ���*/
   void childMain( int iOffset );/*������ϵͳ*/

   bool creatMoreChildren( void );
   void decreaseIdleChildren( void );
   void checkExitedChildren( void );
   void killTimeoutChildren( void );

public:
   BusiManager( char *pczName, int iSysNo, int iShmKey,
                ProcessRegister *pstProcess );
   ~BusiManager();

   int loadConfig( void );                   /*װ����ϵͳ������Ϣ*/
   char *getValue( const char *pczItem );    /*ȡ������*/

   int prepare( void );

   /**
     * ������ϵͳ
     */
   int run( int iMaxAllocChild, SystemManager *pManager );    /*������ϵͳ*/
};

#endif


