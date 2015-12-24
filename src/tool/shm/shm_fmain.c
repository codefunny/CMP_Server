/*
**file:  shm_fmain.c
*/
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "shm_base.h"
#include "pub_cfg.h"

int iShmId;
char cDelimiter = '|';

extern char *pczExecFileName;
int aiTableRows[]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

extern void SHM_WriteData( char *epcShmAddr );

/*
**function:write the tablenum and row num of table to share memory
**input:1 share memory address
**      2 file pointer that points the record offset file
**output:none
**return:0-succ,other-failure
*/
int SHM_FGetSize( )
{
   int iRow;
   FILE *fp;
   char aczDataPath[200], aczLine[2000];
   char *pczDataPath, *pcTemp;
   char aczDataFile[200];
   int iTemp, iSize;

   iSize = sizeof( aiTableSize )*2;
   pczDataPath = getenv( "MYROOT" );
   if( pczDataPath == NULL )
   {
      strcpy( aczDataPath, getenv("HOME") );
      /*strcat( aczDataPath, "/data" );*/
      pczDataPath = aczDataPath;
   }
   for( iTemp = 0; iTemp < sizeof( aiTableSize )/sizeof( int ); iTemp++ )
   {

      pcTemp = strrchr( pczDataPath, '/' );
      /*最后一个字符不是'/'*/
      if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
      {
         sprintf( aczDataFile, "%s/data/%s.txt",
                  pczDataPath, paczTableName[iTemp] );
      }
      else
      {
         sprintf( aczDataFile, "%sdata/%s.txt",
                  pczDataPath, paczTableName[iTemp]  );
      }
      fp = fopen( aczDataFile, "rb" );
      if( fp == NULL )
      {
         printf( "error, Can't open the file %s", aczDataFile );
         iRow = 0;
      }
      else
      {
         iRow = 0;
         do
         {
            fgets( aczLine, 2000, fp );
            if( feof( fp ) == 0 )
            {
               iRow++;
            }
         } while( feof( fp ) == 0 );
         fclose( fp );
      }
      printf( "file %s.txt has %d rows\n",
              paczTableName[iTemp], iRow );
      aiTableRows[iTemp] = iRow;
      iSize +=  aiTableSize[iTemp] * iRow + sizeof(long);
   }

   return iSize;
}

/**
  * 为支持动态配置数据装载 增加函数cfgShmLoad
  */
int CfgShmLoad (int iIpcKey)
{
   void *pcShmAddr;
   int  iRet, i;
   int  iTemp;
   int  iTableNum;
   long lShmSize;
   struct shmid_ds stShmDs;

   printf ("Delimiter[%c]\n", cDelimiter);

   lShmSize = SHM_FGetSize ();
   printf ("size %d\n" ,lShmSize);

   iShmId = shmget (iIpcKey, 0, CFGSHMMODE);
   if (iShmId > 0) {
       shmctl (iShmId, IPC_STAT, &stShmDs);
       if (stShmDs.shm_nattch > 0) {
           fprintf (stderr, "config share memory attched by %d processes"
                    ", please wait a monment and try again. Key is %d\n",
                    stShmDs.shm_nattch, iIpcKey);
           return (-1);
       }
       if (shmctl (iShmId, IPC_RMID, NULL) < 0) {
	   fprintf (stderr, "please delete share memory fisrt, Key is [%d] \n",
                    iIpcKey);
           return (-1);
       }
   } 

   iShmId = shmget (iIpcKey, lShmSize, IPC_CREAT | CFGSHMMODE);
   if (iShmId == -1) {
	 fprintf (stderr, "create share memory error, Key Is [%d]\n", iIpcKey );
         return (-1);
   }

   pcShmAddr = shmat (iShmId, 0, 0);
   if (pcShmAddr == (void *)(-1)) {
       fprintf (stderr, "attatch the share memory error, Key Is [%d]\n",
                iIpcKey);
       shmctl (iShmId, IPC_RMID, NULL);
       return (-1);
   }

   printf( "lShmSize %d\n", lShmSize );
   memset( pcShmAddr, 0x00, lShmSize );

   SHM_WriteData( pcShmAddr );
   shmdt( pcShmAddr );

   return (0);
} 

/***
  * 为支持动态配置数据装载，main移到shmload.c中 
  *
int main( int argc, char *argv[] )
{
   char *pcShmAddr;
   int  iShmSize, iIpcKey, i;
   int  iShmKeyNum, aiShmKey[ 200 ];
   ShmCa *pstShmCa;
   time_t tNow;

   if( argc != 2 )
   {
      printf( "usage: %s IpcKey [Delimiter]\n", argv[0] );
      exit( 1 );
   }

   pczExecFileName = argv[0];

   iShmSize = SHM_FGetSize( )+1+sizeof(ShmCa);
   printf( "size %d\n" ,iShmSize );

   iIpcKey = atoi( argv[1] );   
   if (argc >= 3)
   {
       cDelimiter = argv[2][0];
   }
   else
   {
       cDelimiter = '|';
   }

   iShmId = shmget( iIpcKey, 0, 0660 );
   if( iShmId > 0 )
   {
      pcShmAddr = shmat( iShmId, 0, 0 );
      if( pcShmAddr == (void *)(-1) )
      {
         printf( "attatch the share memory error, Key Is [%d]\n", iIpcKey );
         shmctl( iShmId, IPC_RMID, NULL );
         return -1;
      }
      pstShmCa = (ShmCa*)pcShmAddr;
      time( &(pstShmCa->m_tUpdateTime) );
      pstShmCa->m_iStatus = SHM_UPDATED;

      shmdt( pcShmAddr );

      if( shmctl( iShmId, IPC_RMID, NULL ) < 0 )
      {
         printf( "please delete share memory fisrt, Key is [%d] \n",
                 iIpcKey );
         return -1;
      }
   } 

   iShmId = shmget( (key_t)iIpcKey, (size_t)iShmSize, IPC_CREAT | 0777 );
   if( iShmId == -1 )
   {
      printf( "create share memory error, Key Is [%d]\n", iIpcKey );
      return -1;
   }

   pcShmAddr = shmat( iShmId, 0, 0 );
   if( pcShmAddr == (void *)(-1) )
   {
      printf( "attatch the share memory error, Key Is [%d]\n", iIpcKey );
      shmctl( iShmId, IPC_RMID, NULL );
      return -1;
   }

   printf( "iShmSize %d id %d  addr %x\n", iShmSize, iShmId, pcShmAddr );
   memset( pcShmAddr, 0x00, iShmSize );

   pstShmCa = (ShmCa*)pcShmAddr;
   pstShmCa->m_iStatus = SHM_LOADING;
   SHM_WriteData( pcShmAddr + sizeof (ShmCa) );
   pstShmCa->m_iStatus = SHM_LOADED;
   shmdt( pcShmAddr );

   return 0;
} 
**************************************/

/*
**end of file
*/
