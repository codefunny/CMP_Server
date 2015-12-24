#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ap_base.h"
#include "log.h"
#include "apr_hash.h"
#include "pub_base.h"

int makeListener( apr_socket_t **pstListener, apr_pool_t *pstPool,
                  int iPort, int iFamily, int iType )
{
   apr_status_t   tStatus;
   apr_sockaddr_t *pstAddr;

   tStatus = apr_socket_create( pstListener, iFamily, iType,
                                APR_PROTO_TCP, pstPool );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to create socket %s\n", strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( *pstListener, APR_SO_REUSEADDR, 1 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_REUSEADDR %s\n",
              strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( *pstListener, APR_SO_KEEPALIVE, 1 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_KEEPALIVE %s\n",
              strerror(errno) ));
      return -1;
   }

   /**
     * 设置 socket 传输缓冲区大小为 2048
     */
   tStatus = apr_socket_opt_set( *pstListener, APR_SO_SNDBUF, 2048 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_SNDBUF %s\n",
              strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( *pstListener, APR_TCP_NODELAY, 1);
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_TCP_NODELAY %s\n",
              strerror(errno) ));
   }

   apr_sockaddr_info_get( &pstAddr, NULL, iFamily, iPort, 0, pstPool );

   tStatus = apr_socket_bind( *pstListener, pstAddr );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to bind socket %s at %d\n",
              strerror(errno), iPort ));
      return -1;
   }

   /** the 511 from apache **/
   tStatus = apr_socket_listen( *pstListener, 511 );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_listen %s\n", strerror(errno) ));
      return -1;
   }

   return 0;
}

int makeUdpSvrSock( apr_socket_t **pstListener, apr_pool_t *pstPool, int iPort)
{
   apr_status_t   tStatus;
   apr_sockaddr_t *pstAddr;

   tStatus = apr_socket_create( pstListener, APR_INET, SOCK_DGRAM,
                                APR_PROTO_UDP, pstPool );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to create socket %s\n", strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( *pstListener, APR_SO_REUSEADDR, 1 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_REUSEADDR %s\n",
              strerror(errno) ));
      return -1;
   }

   apr_sockaddr_info_get( &pstAddr, NULL, APR_INET, iPort, 0, pstPool );

   tStatus = apr_socket_bind( *pstListener, pstAddr );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to bind socket %s at %d\n",
              strerror(errno), iPort ));
      return -1;
   }

   return 0;
}

int setSockOpts (apr_socket_t *pstSock)
{
   apr_status_t   tStatus;

   tStatus = apr_socket_opt_set( pstSock, APR_SO_REUSEADDR, 1 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_REUSEADDR %s\n",
              strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( pstSock, APR_SO_KEEPALIVE, 1 );
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_SO_KEEPALIVE %s\n",
              strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( pstSock, APR_TCP_NODELAY, 1);
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_TCP_NODELAY %s\n",
              strerror(errno) ));
   }

   return 0;
}

int makeClient( apr_socket_t **pstClient, apr_pool_t *pstPool, char *pczServer,
                int iPort, int iFamily, int iType, int iTimeOut )
{
   apr_status_t   tStatus;
   apr_sockaddr_t *pstRemote;

   tStatus = apr_sockaddr_info_get( &pstRemote, pczServer, iFamily,
                                    iPort, 0, pstPool );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to sockaddr_info_get %s\n", strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_create( pstClient, iFamily, iType,
                                APR_PROTO_TCP, pstPool );
   if( tStatus != APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to create socket %s\n", strerror(errno) ));
      return -1;
   }

   /** 联接只等待5秒钟 **/
   if( apr_socket_timeout_set( *pstClient, apr_time_from_sec(5) ) !=
       APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to set timeout %s\n", strerror(errno) ));
      return -1;
   }

   if( apr_socket_connect( *pstClient, pstRemote ) != APR_SUCCESS )
   {
      apr_socket_close( *pstClient );
      LOG4C(( LOG_FATAL, "failed to connect: %s\n", strerror(errno) ));
      return -1;
   }

   tStatus = apr_socket_opt_set( *pstClient, APR_TCP_NODELAY, 1);
   if( tStatus != APR_SUCCESS && tStatus != APR_ENOTIMPL )
   {
      LOG4C(( LOG_FATAL, "failed to apr_socket_opt_set APR_TCP_NODELAY %s\n",
              strerror(errno) ));
   }

   if( apr_socket_timeout_set( *pstClient, apr_time_from_sec(iTimeOut) ) !=
       APR_SUCCESS )
   {
      LOG4C(( LOG_FATAL, "failed to set timeout %s\n", strerror(errno) ));
      return -1;
   }

   return 0;
}

int ap_tcp_write ( apr_socket_t *sock, char *buf, int len )
{
        int  left;
        apr_size_t  nwrite;
        apr_status_t stat;
        
        left = len;
        while (left > 0)
        {
                nwrite = left;
                stat  = apr_socket_send (sock, buf, &nwrite);
                if (stat != APR_SUCCESS) return ( -1 );

                left -= nwrite;
                buf  += nwrite;
        }

        return ( 0 );
}

int ap_tcp_read ( apr_socket_t *sock, char *buf, int len )
{
        int  left;
        apr_size_t nread;
        apr_status_t stat;

        left = len;
        while ( left > 0 )
        {
                nread = left;
                stat = apr_socket_recv ( sock, buf, &nread);
                if ( stat != APR_SUCCESS ) return( -1 );
                left -= nread;
                buf += nread;
        }

        return 0;
}

char *ap_basename(char *str)
{
        int i;
        for(i=strlen(str)-1; i>=0; i--)
                if (str[i]=='/') break;
        return &str[i+1];
}

char *ap_dirname(char *str)
{
    int i;
    static char dirStr[128];

    for(i=strlen(str)-1; i>=0; i--) {
        if (str[i]=='/') {
            break;
        }
    }

    strncpy (dirStr, str, i + 1);
    dirStr[i] = '\0';

    return dirStr;
}

/**
  * 连接共享内存
  */
void * shm_get( int iShmKey, int iSize )
{
   bool bCreateNew = false;
   int iShmId;
   void *pShmAddr;

   iShmId = shmget( iShmKey, 0, 0600 );
   if( iShmId == -1 )
   {
      if( iSize == 0 )
      {
         LOG4C(( LOG_FATAL, "failed to shmget %s\n", strerror(errno) ));
         return NULL;
      }

#ifdef SHM_HUGETLB
      iShmId = shmget( iShmKey, iSize,
                       0700|IPC_CREAT|IPC_EXCL );
#else
      iShmId = shmget( iShmKey, iSize,
                       0700|IPC_CREAT|IPC_EXCL );
#endif
      bCreateNew = true;
   }

   if( iShmId == -1 )
   {
      LOG4C(( LOG_FATAL, "failed to shmget %s\n", strerror(errno) ));
      return NULL;
   }

   pShmAddr = (ProcessRegister*)shmat( iShmId, 0, 0 );
   if( (long)pShmAddr == -1 )
   {
      LOG4C(( LOG_FATAL, "failed to shmat %s\n", strerror(errno) ));
      return NULL;
   }
   
   if( bCreateNew )
   {
      memset( pShmAddr, 0, iSize );
   }

   return pShmAddr;
}

bool fileExistAndExec( char *pczFile )
{
   char aczPath[8193], *p1, *p2, aczFile[128];
   bool bFound = false, bExecute = false;
   struct stat stStat;

   /**
     * 检查可执行文件名字在 PATH 指定的环境变量中是否存在
     */
   memset( aczPath, 0, sizeof(aczPath) );
   if( getenv( "PATH" ) != NULL )
   {
      strcpy( aczPath, getenv( "PATH" ) );
   }

   p1 = aczPath;
   do
   {
      p2 = strchr( p1, ':' );
      if( p2 != NULL )
      {
         *p2 = 0;
         sprintf( aczFile, "%s/%s", p1, pczFile );
         if( lstat( aczFile, &stStat ) == 0 )
         {
            /** 已经找到文件, 但是还不知道是否可执行 **/
            bFound = true;
            if( stStat.st_mode&S_IXUSR != 0 )
            {
               bExecute = true;
               break;
            }
         }
         p1 = p2+1;
      }
      else
      {
         sprintf( aczFile, "%s/%s", p1, pczFile );
         if( lstat( aczFile, &stStat ) == 0 )
         {
            /** 已经找到文件, 但是还不知道是否可执行 **/
            bFound = true;
            if( stStat.st_mode&S_IXUSR != 0 )
            {
               bExecute = true;
               break;
            }
         }
      }
   } while( p2 != NULL );

   if( bFound == false )
   {
      return false;
   }

   if( bExecute == false )
   {
      return false;
   }

   return true;
}

void loadEnviorment( char *moduleName )
{
   char aczCompleteFileName[128];
   char aczLine[1024], *p;
   FILE *fp;

   /**
   sprintf( aczCompleteFileName, "%s/etc/%s.env",
            getenv( "MYROOT" ), moduleName );
   ***/
   strcpy (aczCompleteFileName, moduleName);
   fp = fopen( aczCompleteFileName, "r" );
   if( fp == NULL )
   {
      return;
   }

   /**
     * 下面处理环境变量
     */
   memset( aczLine, 0, sizeof(aczLine) );
   while( fgets( aczLine, sizeof(aczLine), fp ) != NULL )
   {
      PUB_StrTrim( aczLine, '\n' );
      PUB_StrTrim( aczLine, ' ' );
      if( aczLine[0] == '#' )
      {
         continue;
      }

      if( (p = strchr( aczLine, '=' )) == NULL )
      {
         continue;
      }

      if( strlen( aczLine ) < 3 )
      {
         continue;
      }
         LOG4C(( LOG_DEBUG, "setenv: %s" ,  aczLine));
#ifndef __hpux
      *p = 0;
      if( setenv( aczLine, p+1, 1 ) != 0 )
      {
         LOG4C(( LOG_WARN, "setenv: %s" , strerror(errno) ));
      }
#else
     p = (char *)malloc (strlen (aczLine) + 1);
     strcpy (p, aczLine); 
     
      if( putenv( p ) != 0 )
      {
         LOG4C(( LOG_WARN, "putenv: %s" , strerror(errno) ));
      }

     free(p);/*zhd add 2010-09-15*/
#endif
   }

   fclose( fp );
}


char * getConfigValue(ItemConfig stConfig, const char *pczItem)
{
   ItemConfig *pstConfig;

   pstConfig = stConfig.m_pstNext;
   while( pstConfig != NULL )
   {
      if( strcmp( pstConfig->m_pczKey, pczItem ) == 0 )
      {
         return pstConfig->m_pczVal;
      }
      pstConfig = pstConfig->m_pstNext;
   }

   return NULL;
}

static apr_pool_t *pstConfigPool = NULL; 

/**
  * 配置信息保存在 $MYROOT/etc 下以子系统为文件的 xml 文件中
  * 配置信息保存在以
  */
int  loadConfig(const char *pczName, ItemConfig *pstConfig)
{
   char aczConfig[128];
   ItemConfig *pstTmp, *pstTail;

   if (pstConfigPool == NULL)
   {
       apr_pool_create (&pstConfigPool, NULL);
   }

   sprintf (aczConfig, "%s/etc/%s.xml", getenv("MYROOT"), pczName);
   pstConfig->m_pstNext = NULL;

   xmlDocPtr  doc;
   xmlNodePtr node;

   LIBXML_TEST_VERSION
   xmlKeepBlanksDefault(0);

   xmlInitParser();

   LOG4C(( LOG_DEBUG, "子系统[%s]配置文件[%s]", aczConfig ));
   doc = xmlParseFile( aczConfig );
   if( doc == NULL )
   {
      LOG4C(( LOG_WARN, "xmlParseFile( %s ) error!!\n ", aczConfig ));
      xmlCleanupParser();
      return -1;
   }

   /* 取根节点 */
   node = xmlDocGetRootElement( doc );
   if( node == NULL )
   {
      LOG4C(( LOG_WARN, "failed to xmlDocGetRootElement %s\n ", aczConfig ));
      xmlFreeDoc( doc );
      xmlCleanupParser();
      return -1;
   }

   node = node->xmlChildrenNode;

   pstTail = pstConfig;
   pstTail->m_pstNext = NULL;

   while( node != NULL )
   {
      if( node->type == XML_ELEMENT_NODE )
      {
         xmlNodePtr txtNode = node->xmlChildrenNode;
         pstTmp = (ItemConfig*)apr_palloc( pstConfigPool, sizeof(ItemConfig) );
         pstTmp->m_pczKey = (char*)apr_palloc( pstConfigPool,
                                      xmlStrlen(node->name)+1 );
         pstTmp->m_pczVal = (char*)apr_palloc( pstConfigPool,
                                      xmlStrlen(txtNode->content)+1 );
         strcpy( pstTmp->m_pczKey, (char*)node->name );
         strcpy( pstTmp->m_pczVal, (char*)txtNode->content );
         pstTmp->m_pstNext = NULL;
         pstTail->m_pstNext = pstTmp;
         pstTail = pstTmp;
      }
      node = node->next;
   }

   LOG4C_INIT( getConfigValue((const ItemConfig)(*pstConfig), "logini") );
   LOG4C_OPEN();

   int iRet = 0;
   /*确保有以下配置信息*/
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"port" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "port"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"exec" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "exec"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"logini" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "logini"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"sysenv" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "sysenv"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"shmkey" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "shmkey"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"runtimes" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "runtimes"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"minproc" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "minproc"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"maxproc" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "maxproc"));
      iRet = -1;
   }
   if( getConfigValue( (const ItemConfig)(*pstConfig), (const char*)"nodeid" ) == NULL )
   {
      LOG4C(( LOG_FATAL, "缺少 %s 配置项\n", "nodeid"));
      iRet = -1;
   }
   else
   {
   }

   xmlFreeDoc( doc );
   xmlCleanupParser();

   return iRet;
}
