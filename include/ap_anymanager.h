#include <sys/types.h>
#include <unistd.h>

#include "apr.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_shm.h"
#include "apr_signal.h"
#include "apr_hash.h"

#include "ap_base.h"
#include "pub_base.h"

#ifndef __APPLICATION_FRAMEWORK_H__
#define __APPLICATION_FRAMEWORK_H__

/**
  * ������Ϣ
  *
struct ItemConfig
{
   char              *m_pczKey;
   char              *m_pczVal;
   struct ItemConfig *m_pstNext;
};
***/

/**
  * ҵ����ϵͳ������
  */
class AnyManager
{
private:
   int    m_iIndex;                   //ϵͳ���
   int    m_iGrpShmKey;               //����Ϣ�������ڴ� KEY
   int    m_iProcShmKey;              //���̹������ڴ� KEY
   int    m_iCfgShmKey;               //������Ϣ�����ڴ� KEY
   int    m_iMinProcess;              //��С������
   int    m_iActiveProcess;           //�������
   int    m_iMaxProcess;              //��������
   int    m_iMaxAllowProcess;         //������������
   char   m_aczName[128];             //��ϵͳ����

   ProcessRegister *m_pstProcess;     //���̵Ǽǹ�����
   SystemRegister  *m_pstSystem;      //ϵͳ��Ϣ������

   apr_socket_t *m_pstListener;       //ҵ������ socket

   apr_pool_t  *m_pstPool;            //��ϵͳ�Ļ����
   apr_pool_t  *m_pstMaintanence;       //ά�����̵Ļ����

   ItemConfig   m_stConfig;           //������Ϣ

private:
   void maintenance( void );          //ά����ϵͳ��Ϣ

   int  closeListener( void );

   void doCommand( void );
   void doStopCmd( void );
   void doLoadCfgCmd( void );

   int  makeChild( int iSlot );  //�����ӽ���
   void childMain( int iOffset );//������ϵͳ

   bool creatMoreChildren( void );
   void decreaseIdleChildren( void );
   void checkExitedChildren( bool bStoping );
   void killTimeoutChildren( void );
   void setupSignal( void );    //�����ź�

public:
   AnyManager( char *pczName, int iIndex, int iMaxChild );
   ~AnyManager();

   int loadConfig( void );                   //װ����ϵͳ������Ϣ
   char *getValue( const char *pczItem );    //ȡ������

   int prepare( int iGrpShmKey, int iProcShmKey );

   /**
     * ������ϵͳ
     */
   int run( void );    //������ϵͳ
};

#endif

