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
**����:���ļ��еĶ�����������Ϣ
**����:1 �ļ�ָ��
**���:1 ������Ϣ�ṹ
**����   ��
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
**����:�����������ֺͱ�־ȡ�ڵ�ID�Ź����ڴ�IPCKEY�ͽڵ㱨��Ӧ�ú�
**����:1 �����ڵ���
**     2 ��־(һ��ڵ�Ϊ0,����BTOCΪ0,����BTOBΪ1)
**���:1 �����ڵ��
**     2 ����������Ϣ�����ڴ�IPCKEY
**     3 �����ڵ㱨��Ӧ�ú�
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
   /*���һ���ַ�����'/'*/
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
