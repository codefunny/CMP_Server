/*
**file:pub_cfg.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#include "pub_base.h"
#include "pub_cfg.h"

/*
**功能:读文件中的二进制配置信息
**输入:1 文件指针
**输出:1 配置信息结构
**返回   无
*/
void PUB_Read( FILE *efp, NodePros *epstNodePros )
{
   int iNum, iSize;

   assert( efp != NULL );
   assert( epstNodePros != NULL );

   iNum = 0;

   fseek( efp, 0, SEEK_END );
   iSize = ftell( efp );
   iNum = iSize / sizeof( NodeInitCfg );
   fseek( efp, 0, SEEK_SET );
   fread( &epstNodePros->stNodeInitCfg, sizeof( NodeInitCfg ), (size_t)iNum, efp );

   epstNodePros->iChannelNum = iNum;
}

/*
**功能:根据渠道名字和标志取节点ID号共享内存IPCKEY和节点报文应用号
**输入:1 渠道节点名
**     2 标志(一般节点为0,网银BTOC为0,网银BTOB为1)
**输出:1 渠道节点号
**     2 渠道配置信息共享内存IPCKEY
**     3 渠道节点报文应用号
*/
int PUB_GetValByName( char *epcChanlName, int eiTag,
                      int *opiNodeId, int *opiShmIpcKey,
                      int *opiMsgAppId )
{
   FILE *fp;
   char aczMessage[256];
   char aczBinCfgFile[256];
   char *pcTemp, *pczCfgPath;
   NodePros stNodePros;
   int iTemp;

   pczCfgPath = getenv( "CHANNEL_CONFIG_PATH" );
   if( pczCfgPath == NULL )
   {
      printf( "Please set the environment CHANNEL_CONFIG_PATH\n" );
      return -1;
   }

   pcTemp = strrchr( pczCfgPath, '/' );
   /*最后一个字符不是'/'*/
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczBinCfgFile, "%s/easylink.cfg", pczCfgPath );
   }
   else
   {
      sprintf( aczBinCfgFile, "%seasylink.cfg", pczCfgPath );
   }

   fp = fopen( aczBinCfgFile, "rb" );
   if( fp == NULL )
   {
      sprintf( aczMessage, "Can't open the file %s\n", aczBinCfgFile );
      perror( aczMessage );
      return -1;
   }

   memset( &stNodePros, 0, sizeof( stNodePros ) );
   PUB_Read( fp, &stNodePros );
   fclose( fp );

   for( iTemp = 0; iTemp < stNodePros.iChannelNum; iTemp++ )
   {
      if( strcmp( stNodePros.stNodeInitCfg[iTemp].aczNodeName,
                  epcChanlName ) == 0 &&
          stNodePros.stNodeInitCfg[iTemp].iTag == eiTag )
      {
         *opiNodeId    = stNodePros.stNodeInitCfg[iTemp].iNodeId;
         *opiShmIpcKey = stNodePros.stNodeInitCfg[iTemp].iShmIpcKey;
         *opiMsgAppId  = stNodePros.stNodeInitCfg[iTemp].iMsgAppId;
         return 0;
      }
   }

   return -1;
}

/*
** end of file
*/
