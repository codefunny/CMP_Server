#include <stdio.h>      /* UNIX */
#include <stdlib.h>
#include <unistd.h>

#include <atmi.h>       /* TUXEDO */
#include <Uunix.h>      /* TUXEDO */
#include <userlog.h>    /* TUXEDO */

#include <fml32.h>
#include <atmi.h>

#include "ap_base.h"
#include "ap_handle.h"
#include "ap_global.h"

#include "transctrl.h"

#include "global.h"
#include "svc_control.h"
#include "udo.h"
#include "pkg_api.h"
#include "pkg_tuxapi.h"
#include "log.h"
#include "shm_base.h"

apr_pool_t *gpstLocalPool;
apr_pool_t *gpstRootPool;
apr_socket_t *pstClient;

AxpTxnPara stAxpTxnPara;

char *pcShmAddr = NULL;

extern TPSVCINFO gstSendInfo;

static struct 
{
    int  iNodeId;
    int  iCfgShmKey;
    int  iMaxRunTimes;
    int  iDictId;
    char aczName[64];
    char aczLogIni[128];
    char aczSysEnv[128];
} stChCfgInfo;

extern "C" {
int ADP_GetDictId( int iNodeId );
void Axp (TPSVCINFO *rqst);
}

int
#if defined(__STDC__) || defined(__cplusplus)
tpsvrinit(int argc, char **argv)
#else
tpsvrinit(argc,argv)
int argc;
char **argv;
#endif

{
   int iDictId;
   int iCfgShmKey;
   char *pczTmp;
   ItemConfig stItemConfig;

   memset (&stChCfgInfo, 0, sizeof (stChCfgInfo));

   pczTmp = strrchr (argv[0], '/');
   if (pczTmp != NULL)
   {
       pczTmp += 1;
   }
   else 
   {
       pczTmp = argv[0];
   }
   strcpy (stChCfgInfo.aczName, pczTmp);

   /*初始APR环境及缓冲池*/
   apr_initialize();
   apr_pool_create( &gpstRootPool, NULL );
   apr_pool_create( &gpstGlobalVariablePool, gpstRootPool );
   apr_pool_create( &gpstLocalPool, gpstRootPool );

   /*从etc/下读取渠道的配置文件内容:aczName.xml */
   if (loadConfig (stChCfgInfo.aczName, &stItemConfig) == -1)
   {
       userlog ("读取渠道配置文件出错,渠道名称[%s]\n", stChCfgInfo.aczName);
       LOG4C ((LOG_FATAL, "读取渠道配置文件出错,渠道名称[%s]\n", stChCfgInfo.aczName));
       return -1;
   }

   stChCfgInfo.iNodeId = atoi (getConfigValue (stItemConfig, "nodeid"));
   stChCfgInfo.iCfgShmKey = atoi (getConfigValue (stItemConfig, "shmkey"));
   stChCfgInfo.iMaxRunTimes = atoi (getConfigValue (stItemConfig, "runtimes"));
   strcpy (stChCfgInfo.aczLogIni, getConfigValue (stItemConfig, "logini"));
   strcpy (stChCfgInfo.aczSysEnv, getConfigValue (stItemConfig, "sysenv"));

   LOG4C_INIT( stChCfgInfo.aczLogIni );
   LOG4C_OPEN();

   pcShmAddr = (char *)Shm_GetCfgShm (stChCfgInfo.iCfgShmKey, &iCfgShmKey);
   if( pcShmAddr == NULL )
   {
       userlog ("附接配置数据共享内存出错,渠道名称[%s]\n", stChCfgInfo.aczName);
       LOG4C ((LOG_FATAL, "附接配置数据共享内存出错,渠道名称[%s]\n", stChCfgInfo.aczName));
      return -1;
   }

   if( (stChCfgInfo.iDictId = ADP_GetDictId(stChCfgInfo.iNodeId) ) == -1 )
   {
      userlog ("严重错误, 节点%d不存在\n", stChCfgInfo.iNodeId );
      LOG4C(( LOG_FATAL, "严重错误, 节点%d不存在\n", stChCfgInfo.iNodeId ));
      return -1;
   }

   if (onSysStart(stChCfgInfo.aczName) == -1)
   {
       return -1;
   }

   if( pcShmAddr != NULL )
   {
        shmdt( pcShmAddr );
        pcShmAddr = NULL;
   }

   return 0;
}

void
tpsvrdone()
{
   onSysEnd();

   if( pcShmAddr != NULL )
   {
        shmdt( pcShmAddr );
        pcShmAddr = NULL;
   }

   apr_pool_destroy( gpstRootPool );

   apr_terminate();
      
   LOG4C_CLOSE();
}

int handleTxn( int iNodeId, int iDictId, char *pczTxCode,
               FBFR32 *pczRecvBuff, int iRecvLen )
{
   char aczErrorMsg[512];
   int iToMidTranCodeId, iToMsgTranCodeId, iToMidId, iToMsgId, iLen;
   int iRet;
   char acSvcRecvHead[30*1024];

   SVC_Content stService;
   MsgFldCfgAry stMsgFldCfgAry;

   memset (&stService, 0, sizeof (stService));

   UDO_Init( acSvcRecvHead, sizeof(acSvcRecvHead) );

   memset( &stMsgFldCfgAry, 0x00, sizeof(stMsgFldCfgAry) );

   iRet = PKG_GetInfoByTranCode( iNodeId, pczTxCode, 1, &stMsgFldCfgAry );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "取节点%d域配置错,返回码%d",
                 iNodeId, iRet ));
      sprintf( aczErrorMsg, "取域配置错" );
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1001;
   }

   iRet = PKG_Fml2Udo (&stMsgFldCfgAry, pczRecvBuff, acSvcRecvHead);
   free( stMsgFldCfgAry.pstMsgFldCfg );
   if( iRet != 0 && iRet != PKG_LENGTH_OVER )
   {
      sprintf( aczErrorMsg, "报文转换到UDO出错,返回码%d", iRet );
      LOG4C ((LOG_FATAL, " %s",  aczErrorMsg ));
      iLen = strlen( aczErrorMsg );
      sendBuffer( aczErrorMsg, iLen );
      return 1004;
   }

   if( postCheck( acSvcRecvHead, (char *)pczRecvBuff, iRecvLen ) == -1 )
   {
      UDO_Free( acSvcRecvHead );
      return -1;
   }

   UDO_Prt2File( LOG_INFO, "commudo.log", acSvcRecvHead, 
                 "From Channel Node [%d]Serail[%s]\n", iNodeId ,aczagserial );

   SVC_Prepare( (UDP)acSvcRecvHead, iNodeId, iDictId, pczTxCode, &stService );
   UDO_Free( acSvcRecvHead );

   return 0;
}

void Axp (TPSVCINFO *rqst)
{
   char aczTxCode[100];
   NodeTranCfg *pstNodeTranCfg;
   int iCfgShmKey;

   memset( aczTxCode, 0, sizeof(aczTxCode) );

   Fprint32((FBFR32 *)rqst->data);

   gstSendInfo.data = tpalloc ("FML32", 0, 4096);

   LOG4C_CLOSE();
   LOG4C_OPEN();

   pcShmAddr = (char *)Shm_GetCfgShm (stChCfgInfo.iCfgShmKey, &iCfgShmKey);
   if (pcShmAddr == NULL)
   {
          LOG4C ((LOG_FATAL,
                 "attach get channal node %d config memory error\n"
                 "KEYSHM IPCKEY = %d\n", stChCfgInfo.iNodeId, stChCfgInfo.iCfgShmKey));
          goto ret_channel;
   }

   if (onTxStart (stChCfgInfo.aczName) == -1)
   {
       LOG4C(( LOG_FATAL, "交易开始处理函数失败!\n"));
       goto ret_channel;
   }

   if (preCheck (rqst->data, rqst->len) == -1)
   {
      LOG4C(( LOG_FATAL, "报文预检查失败\n"));
      goto ret_channel;
   }

   if( getTxCode (rqst->data, aczTxCode) == -1 )
   {
      LOG4C(( LOG_FATAL, "取交易码失败\n"));
      goto ret_channel;
   }

   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX (aczTxCode);
   LOG4C_OPEN ();

   if (transCtrl (iNodeId, aczTxCode)) {
      LOG4C(( LOG_FATAL, "交易已被设置为停止运行\n"));
      goto ret_channel;
   }

   strcpy (stAxpTxnPara.m_aczTranCode, aczTxCode);
   stAxpTxnPara.m_iNodeId = stChCfgInfo.iNodeId;
   stAxpTxnPara.m_iDictId = stChCfgInfo.iDictId;

   LOG4C(( LOG_DEBUG, "开始执行交易 [%s]\n", aczTxCode ));
   handleTxn( stChCfgInfo.iNodeId, stChCfgInfo.iDictId, aczTxCode, (FBFR32 *)rqst->data, rqst->len);

ret_channel:

   onTxEnd();

   LOG4C_CLOSE ();
   LOG4C_CHGSUFFIX ("");
   LOG4C_OPEN ();

   Fprint32((FBFR32 *)gstSendInfo.data);

   if( pcShmAddr != NULL )
   {
        shmdt( pcShmAddr );
        pcShmAddr = NULL;
   }

   tpreturn (TPSUCCESS, 0, gstSendInfo.data, 0, 0);
}
