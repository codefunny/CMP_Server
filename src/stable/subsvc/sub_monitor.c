#include <string.h>
#include <stdlib.h>

#include "tool_base.h"
#include "easysock.h"
#include "chk_api.h"
#include "svc_base.h"
#include "sub_base.h"
#include "sub_control.h"
#include "udo.h"
#include "asm/asm_api.h"
#include "adp/adp_control.h"
#include "log.h"

int SUB_SearchNode( const void *p1, const void *p2 )
{
   NodeCommCfg *pstP1 = (NodeCommCfg*)p1;
   NodeCommCfg *pstP2 = (NodeCommCfg*)p2;

   return pstP1->iNodeId - pstP2->iNodeId;
}

/**
  * 检查节点是否需要监控
  * 需要返回1 失败或者出错返回0
  */
int SUB_NeedMonitor( int eiNodeId )
{
   NodeCommCfg *pstNodeCommCfg, *pstNodeSearch, stNodeKey;
   int iNodeNum;

   stNodeKey.iNodeId = eiNodeId;
   pstNodeCommCfg = (NodeCommCfg*)SHM_ReadData( pcShmAddr, NODECOMMCFG,
                                                &iNodeNum );
   pstNodeSearch = (NodeCommCfg*)bsearch( &stNodeKey,
                                          pstNodeCommCfg,
                                          iNodeNum,
                                          sizeof(stNodeKey),
                                          SUB_SearchNode );
   if( pstNodeSearch == NULL )
   {
      return 0;
   }

   /** 将监控端口修改为是否监控, 0-表示不监控, 1-表示监控 **/
   return pstNodeSearch->iMtrPort;
}

int SUB_SearchLogField( const void *p1, const void *p2 )
{
   LogWatch *pstP1 = (LogWatch *)p1;
   LogWatch *pstP2 = (LogWatch *)p2;

   if( pstP1->iTag != pstP2->iTag )
   {
      return pstP1->iTag - pstP2->iTag;
   }

   if( pstP1->iNodeId != pstP2->iNodeId )
   {
      return pstP1->iNodeId - pstP2->iNodeId;
   }

   return pstP1->iDictId - pstP2->iDictId;
}

extern int iRunTimes;
void SUB_CreateMonitorBuffer( int eiNodeId, char *epczTranCode, int eiFlag,
                              int eiSuccFlag, UDP pfList, char *epczBuffer )
{
   int       i, iRowNum, iRet;
   LogWatch *pstLogWatch, *pstLogSch, stLogKey;
   char aczFieldValue[1024],aczValue[512];
   short  shVal;
   int    iVal;
   long   lVal;
   float  fVal;
   double dVal;
    char aczAgDate[9], aczAgSerial[11];
   strcpy( epczBuffer, "" );
   pstLogWatch = (LogWatch*)SHM_ReadData( pcShmAddr, LOGWATCH, &iRowNum );
   if( pstLogWatch == NULL )
   {
      return;
   }

   stLogKey.iTag    = 1;
   stLogKey.iNodeId = eiNodeId;
   stLogKey.iDictId = -1;
   pstLogSch = (LogWatch*)bsearch( &stLogKey, pstLogWatch, iRowNum,
                                   sizeof(stLogKey), SUB_SearchLogField );
   if( pstLogSch == NULL )
   {
      return;
   }
   iRowNum = pstLogSch->iFldId;
   pstLogSch++;
  /*
   sprintf( epczBuffer, "%d%d,%d,%s,%d,%d:",
            getpid(), iRunTimes, eiNodeId, epczTranCode, eiFlag, eiSuccFlag );
   //////////////////modify on jan 24 2007 ///////////////////////
*/
   memset( aczAgDate, 0, sizeof(aczAgDate) );
   memset( aczAgSerial, 0, sizeof(aczAgSerial) );
   getString( "agserial", aczAgSerial );
   getString( "agtxdate", aczAgDate );
   sprintf( epczBuffer, "%s%08d,%d,%s,%d,%d:",
            aczAgDate, atol(aczAgSerial), eiNodeId, epczTranCode, eiFlag, eiSuccFlag );
   for( i = 0; i < iRowNum; i++ )
   {
      memset( aczFieldValue, 0, sizeof( aczFieldValue ) );
      switch( pstLogSch->iFldType )
      {
      case UDT_STRING:
         memset( aczValue, 0, sizeof(aczValue) );
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, aczValue, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "\"%d=%s\"", pstLogSch->iFldId, aczValue );
         }
         break;
      case UDT_SHORT:
         shVal = 0;
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, (char*)&shVal, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "%d=%d", pstLogSch->iFldId, shVal );
         }
         break;
      case UDT_INT:
         iVal = 0;
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, (char*)&iVal, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "%d=%d", pstLogSch->iFldId, iVal );
         }
         break;
      case UDT_LONG:
         lVal = 0;
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, (char*)&lVal, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "%d=%ld", pstLogSch->iFldId, lVal );
         }
         break;
      case UDT_FLOAT:
         fVal = 0.0;
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, (char*)&fVal, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "%d=%f", pstLogSch->iFldId, fVal );
         }
         break;
      case UDT_DOUBLE:
         dVal = 0.0;
         iRet = UDO_GetField( pfList, pstLogSch->iFldId, 0, (char*)&dVal, 0 );
         if( iRet == 0 )
         {
            sprintf( aczFieldValue, "%d=%lf", pstLogSch->iFldId, dVal );
         }
         break;
      default:                     /* 其它类型不监控 */
         sprintf( aczFieldValue, "%d=******", pstLogSch->iFldId );
         break;
      }
      pstLogSch++;
      if ( strlen( aczFieldValue ) == 0 )
      {
         continue;
      }
      if( i < iRowNum-1 )
      {
         sprintf( epczBuffer, "%s%s,", epczBuffer, aczFieldValue );
      }
      else
      {
         sprintf( epczBuffer, "%s%s;", epczBuffer, aczFieldValue );
         break;
      }
   }
}

/**
  * 发送需要监控的字段
  */
void SUB_Monitor( int eiNodeId, char *epczTranCode, int eiFlag,
                  int eiSuccFlag, UDP pfList )
{
   char aczBuffer[2048];
   char *pczMonitorIp   = (char*)getenv( "MONITOR_IP" );
   char *pczMonitorPort = (char*)getenv( "MONITOR_PORT" );
   int iPort;
   int iMonFd;

   if( pczMonitorIp == NULL || pczMonitorPort == NULL )
   {
      return;
   }

   iPort = atoi(pczMonitorPort);
   if( SUB_NeedMonitor( eiNodeId ) == 0 )
   {
      return;
   }

   memset( aczBuffer, 0, sizeof(aczBuffer) );
   SUB_CreateMonitorBuffer( eiNodeId, epczTranCode, eiFlag, eiSuccFlag,
                            pfList, aczBuffer );
   if( strlen(aczBuffer) == 0 )
   {
      return;
   }
   aczBuffer[1023] = 0;
   if( (iMonFd = tcp_connect( pczMonitorIp, iPort )) == -1 )
   {
      return;
   }
   if( tcp_wait_send( iMonFd, 1000 ) == -1 )
   {
      tcp_close( iMonFd );
      return;
   }
   if( tcp_raw_send( iMonFd, aczBuffer, strlen(aczBuffer) ) == -1 )
   {
      tcp_close( iMonFd );
      return;
   }
   if( tcp_wait_recv( iMonFd, 500 ) == -1 )
   {
      tcp_close( iMonFd );
      return;
   }
   tcp_raw_recv( iMonFd, aczBuffer, 1 );
#if 0
#endif
   tcp_close( iMonFd );
}

