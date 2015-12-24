/*
**file:shm_ftable.c
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <assert.h>

#include "shm_base.h"
#include "pub_base.h"
#include "udo.h"
#include "chk.h"

extern int iShmId;
extern aiTableRows[];
extern char cDelimiter;

int sortTranFldCfg( const void *left, const void *right )
{
   int i;

   TranFldCfg *pstLeft  = (TranFldCfg*)left;
   TranFldCfg *pstRight = (TranFldCfg*)right;

   i = pstLeft->iNodeId - pstRight->iNodeId;
   if( i != 0 )
   {
      return i;
   }

   return strcmp( pstLeft->aczTranCode, pstRight->aczTranCode );
}

/*
**function:write file data to share memory
**input:1 share memory start address
**output:none
**return:void
*/
void SHM_WriteData( char *epcShmAddr )
{
   int  iRow, iOffset, iTemp, fldnum;
   char *pczDataPath, *pcTemp;
   char aczNum[1000];
   char *pcTmp1, *pcTmp2;
   char aczDataPath[200], aczDataFile[200];
   char *pcBegin;
   FILE *fp;
   char aczLine[2000];
   char *fldstrs[200];

   ChkCfg          stChkCfg;
   MsgFldCfg       stMsgFldCfg;
   MsgNameId       stMsgNameId;
   NodeCommCfg     stNodeCommCfg;
   NodeTranCfg     stNodeTranCfg;
   RelaCond        stRelaCond;
   Service         stService;
   ServiceCfg      stServiceCfg;
   SubService      stSubService;
   SubSvcFldSrcCfg stSubSvcFldSrcCfg;
   SubSvcReqCfg    stSubSvcReqCfg;
   TranFldCfg      stTranFldCfg;
   RetCodeMap      stRetCodeMap;
   TranCodeMap     stTranCodeMap;
   KernSvc         stKernSvc;
   CommRoute       stCommRoute;
   FldMap          stFldMap;
   LogWatch        stLogWatch;
/*   ConstCond       stConstCond;*/

   assert( epcShmAddr   != NULL );


printf( "shm_ftalbe started!\n" );

   pczDataPath = getenv( "MYROOT" );
   if( pczDataPath == NULL )
   {
      strcpy( aczDataPath, getenv("HOME") );
      strcat( aczDataPath, "/data" );
   }
   else
   {
      sprintf( aczDataPath, "%s/data", pczDataPath );
   }
   pczDataPath = aczDataPath;

   pcTemp = strrchr( pczDataPath, '/' );

   /*最后一个字符不是'/'*/
   iOffset = sizeof(aiTableSize);
   memcpy( epcShmAddr, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[CHKCFG], sizeof(int) );
   printf( "aiTableRows[CHKCFG]=[%d]\n", aiTableRows[CHKCFG] );
   iOffset += sizeof(int);

   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/chkcfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%schkcfg.txt", pczDataPath );
   }

   printf( "put the file chkcfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }


   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stChkCfg, 0x00, sizeof(stChkCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

	 iTemp = strlen (aczLine) - 1;
	 if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         /* 特殊处理 最后一个域是表达式域，或||运算符与分隔符冲突 */
         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 6);
         /*********************************************************/
         if (fldnum != 6)
	 {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 6, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
	 }

         stChkCfg.iSvcId = atoi(fldstrs[0]);
         stChkCfg.iCurSubSeq = atoi(fldstrs[1]);
         stChkCfg.iCondId = atoi(fldstrs[2]);
         stChkCfg.iNextSubSeq = atoi(fldstrs[3]);
         stChkCfg.iDfltFlag = atoi(fldstrs[4]);

         /* 特殊处理 最后一个域是表达式域，或||运算符与分隔符冲突 */
         PUB_StrRightTrim (aczLine, ' ');
         PUB_StrRightTrim (fldstrs[5], '|');
         /*********************************************************/

         strcpy (stChkCfg.aczChkExp, fldstrs[5]);

         memcpy( epcShmAddr+iOffset, &stChkCfg, sizeof(ChkCfg) );
	 iOffset += sizeof(ChkCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );







   memcpy( epcShmAddr+sizeof(int)*MSGFLDCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[MSGFLDCFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/msgfldcfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%smsgfldcfg.txt", pczDataPath );
   }

   printf( "put the file msgfldcfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stMsgFldCfg, 0x00, sizeof(stMsgFldCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 16)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 16, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

         stMsgFldCfg.iMsgAppId = atoi( fldstrs[0] );
         stMsgFldCfg.iFldSeq = atoi( fldstrs[1] );
         stMsgFldCfg.iSepFldSeq = atoi( fldstrs[2] );
         strcpy( stMsgFldCfg.aczFldEngName, fldstrs[3] );
         strcpy( stMsgFldCfg.aczFldChnName, fldstrs[4] );
         stMsgFldCfg.iSepFldFlag = atoi( fldstrs[5] );
         stMsgFldCfg.iLenFldLen = atoi( fldstrs[6] );
         stMsgFldCfg.iFldLen = atoi( fldstrs[7] );
         stMsgFldCfg.iLenFldType = atoi( fldstrs[8] );
         stMsgFldCfg.iFldType = atoi( fldstrs[9] );
         strcpy( stMsgFldCfg.aczLenFldDef, fldstrs[10] );
         strcpy( stMsgFldCfg.aczFldDef, fldstrs[11] );
         strcpy( stMsgFldCfg.aczLenFldAlign, fldstrs[12] );
         strcpy( stMsgFldCfg.aczFldAlign, fldstrs[13] );
         strcpy( stMsgFldCfg.aczSepSymbol, fldstrs[14] );
         strcpy( stMsgFldCfg.aczMacro, fldstrs[15] );

         PUB_StrTrim( stMsgFldCfg.aczFldEngName,  ' ' );
         PUB_StrTrim( stMsgFldCfg.aczFldChnName,  ' ' );
         PUB_StrTrim( stMsgFldCfg.aczLenFldDef,   ' ' );
         PUB_StrTrim( stMsgFldCfg.aczFldDef,      ' ' );
         PUB_StrTrim( stMsgFldCfg.aczLenFldAlign, ' ' );
         PUB_StrTrim( stMsgFldCfg.aczFldAlign,    ' ' );
         PUB_StrTrim( stMsgFldCfg.aczSepSymbol,   ' ' );

         memcpy( epcShmAddr+iOffset, &stMsgFldCfg, sizeof(MsgFldCfg) );
	 iOffset += sizeof(MsgFldCfg);
      }
      else
      {
         break;
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );





   memcpy( epcShmAddr+sizeof(int)*MSGNAMEID, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[MSGNAMEID], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/msgnameid.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%smsgnameid.txt", pczDataPath );
   }

   printf( "put the file msgnameid.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stMsgNameId, 0x00, sizeof(stMsgNameId) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 2)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 2, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

         stMsgNameId.iMsgAppId = atoi( fldstrs[0] );
         strcpy( stMsgNameId.aczMsgAppName, fldstrs[1] );

         PUB_StrTrim( stMsgNameId.aczMsgAppName, ' ' );

         memcpy( epcShmAddr+iOffset, &stMsgNameId, sizeof(MsgNameId) );
	 iOffset += sizeof(MsgNameId);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );




   memcpy( epcShmAddr+sizeof(int)*NODECOMMCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[NODECOMMCFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/nodecommcfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%snodecommcfg.txt", pczDataPath );
   }

   printf( "put the file nodecommcfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stNodeCommCfg, 0x00, sizeof(stNodeCommCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 19)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 19, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stNodeCommCfg.iNodeId = atoi( fldstrs[0] );
            strcpy( stNodeCommCfg.aczNodeName, fldstrs[1] );
            stNodeCommCfg.iNodeDscrb = atoi( fldstrs[2] );
            stNodeCommCfg.iNetProto = atoi( fldstrs[3] );
            stNodeCommCfg.iLinkType = atoi( fldstrs[4] );
            strcpy( stNodeCommCfg.aczSyncFlag, fldstrs[5] );
            strcpy( stNodeCommCfg.aczHostIp, fldstrs[6] );
            strcpy( stNodeCommCfg.aczHostName, fldstrs[7] );
            stNodeCommCfg.iHostCommPort = atoi( fldstrs[8] );
            strcpy( stNodeCommCfg.aczMonitorIp, fldstrs[9] );
            strcpy( stNodeCommCfg.aczMtrHostName, fldstrs[10] );
            stNodeCommCfg.iMtrPort = atoi( fldstrs[11] );
            stNodeCommCfg.iMsgAppId = atoi( fldstrs[12] );
            strcpy( stNodeCommCfg.aczEncrptType, fldstrs[13] );
            stNodeCommCfg.iMinProcess = atoi( fldstrs[14] );
            stNodeCommCfg.iMaxProcess = atoi( fldstrs[15] );
            stNodeCommCfg.iUsableFlag = atoi( fldstrs[16] );
            stNodeCommCfg.iSvrKeyId = atoi( fldstrs[17] );
            stNodeCommCfg.iSvwKeyId = atoi( fldstrs[18] );

         PUB_StrTrim( stNodeCommCfg.aczNodeName,    ' ' );
         PUB_StrTrim( stNodeCommCfg.aczSyncFlag,    ' ' );
         PUB_StrTrim( stNodeCommCfg.aczHostIp,      ' ' );
         PUB_StrTrim( stNodeCommCfg.aczHostName,    ' ' );
         PUB_StrTrim( stNodeCommCfg.aczMonitorIp,   ' ' );
         PUB_StrTrim( stNodeCommCfg.aczMtrHostName, ' ' );
         PUB_StrTrim( stNodeCommCfg.aczEncrptType,  ' ' );

         memcpy( epcShmAddr+iOffset, &stNodeCommCfg, sizeof(NodeCommCfg) );
	 iOffset += sizeof(NodeCommCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );





   memcpy( epcShmAddr+sizeof(int)*NODETRANCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[NODETRANCFG], sizeof(int) );
printf( "aiTableRows[NODETRANCFG]=[%d]\n", aiTableRows[NODETRANCFG] );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/nodetrancfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%snodetrancfg.txt", pczDataPath );
   }

   printf( "put the file nodetrancfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 21)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 21, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stNodeTranCfg.iNodeId = atoi( fldstrs[0] );
            stNodeTranCfg.iMsgAppId = atoi( fldstrs[1] );
            strcpy( stNodeTranCfg.aczMsgAppName, fldstrs[2] );
            stNodeTranCfg.iToMidTranCodeId = atoi( fldstrs[3] );
            stNodeTranCfg.iToMsgTranCodeId = atoi( fldstrs[4] );
            stNodeTranCfg.iToMidId = atoi( fldstrs[5] );
            stNodeTranCfg.iToMsgId = atoi( fldstrs[6] );
            stNodeTranCfg.iToMidIsSt = atoi( fldstrs[7] );
            stNodeTranCfg.iToMsgIsSt = atoi( fldstrs[8] );
            strcpy( stNodeTranCfg.aczCrtTranCodeType, fldstrs[9] );
            strcpy( stNodeTranCfg.aczCrtTranCodePos, fldstrs[10] );
            strcpy( stNodeTranCfg.aczCrtTranCodeSeq, fldstrs[11] );
            strcpy( stNodeTranCfg.aczCrtMsgFixFld, fldstrs[12] );
            strcpy( stNodeTranCfg.aczWrgTranCodeType, fldstrs[13] );
            strcpy( stNodeTranCfg.aczWrgTranCodePos, fldstrs[14] );
            strcpy( stNodeTranCfg.aczWrgTranCodeSeq, fldstrs[15] );
            strcpy( stNodeTranCfg.aczWrgMsgFixFld, fldstrs[16] );
            strcpy( stNodeTranCfg.aczLogTabName, fldstrs[17] );
            strcpy( stNodeTranCfg.aczLogFld, fldstrs[18] );
            stNodeTranCfg.iMaxRetryTime = atoi( fldstrs[19] );
            stNodeTranCfg.iDelayTime = atoi( fldstrs[20] );

         PUB_StrTrim( stNodeTranCfg.aczMsgAppName,      ' ' );
         PUB_StrTrim( stNodeTranCfg.aczCrtTranCodeType, ' ' );
         PUB_StrTrim( stNodeTranCfg.aczCrtTranCodePos,  ' ' );
         PUB_StrTrim( stNodeTranCfg.aczCrtTranCodeSeq,  ' ' );
         PUB_StrTrim( stNodeTranCfg.aczCrtMsgFixFld,    ' ' );
         PUB_StrTrim( stNodeTranCfg.aczWrgTranCodeType, ' ' );
         PUB_StrTrim( stNodeTranCfg.aczWrgTranCodePos,  ' ' );
         PUB_StrTrim( stNodeTranCfg.aczWrgTranCodeSeq,  ' ' );
         PUB_StrTrim( stNodeTranCfg.aczWrgMsgFixFld,    ' ' );
         PUB_StrTrim( stNodeTranCfg.aczLogTabName,      ' ' );
         PUB_StrTrim( stNodeTranCfg.aczLogFld,          ' ' );

         memcpy( epcShmAddr+iOffset, &stNodeTranCfg, sizeof(NodeTranCfg) );
	 iOffset += sizeof(NodeTranCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );



   memcpy( epcShmAddr+sizeof(int)*RELACOND, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[RELACOND], sizeof(int) );
printf( "relacond rows[%d]\n", aiTableRows[RELACOND] );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/relacond.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%srelacond.txt", pczDataPath );
   }

   printf( "put the file relacond.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stRelaCond, 0x00, sizeof(stRelaCond) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 21)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 21, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stRelaCond.iCondId = atoi( fldstrs[0] );
            stRelaCond.iExprId = atoi( fldstrs[1] );
            stRelaCond.iConstFlag = atoi( fldstrs[2] );
            stRelaCond.iLSubSeq = atoi( fldstrs[3] );
            stRelaCond.iLMsgDscrb = atoi( fldstrs[4] );
            stRelaCond.iLFldSeq = atoi( fldstrs[5] );
            strcpy( stRelaCond.aczLFldType, fldstrs[6] );
            stRelaCond.iLStartBit = atoi( fldstrs[7] );
            stRelaCond.iLEndBit = atoi( fldstrs[8] );
            strcpy( stRelaCond.aczMaxValue, fldstrs[9] );
            strcpy( stRelaCond.aczMinValue, fldstrs[10] );
            strcpy( stRelaCond.aczFlag, fldstrs[11] );
            strcpy( stRelaCond.aczMaxFlag, fldstrs[12] );
            strcpy( stRelaCond.aczMinFlag, fldstrs[13] );

#ifdef NEW_VERSION             /* 新版本EasyLink数据库 */
            strcpy( stRelaCond.aczRelation, fldstrs[14] );
#else                          /* 老版本EasyLink数据库 */
         /* 仅为支持湖北一类的条件判断,暂时支持'=','<>','<','>','<=','>=' */
         if( stRelaCond.aczFlag[0] == '0' )  /* 不取反 */
         {
            if( stRelaCond.aczMaxFlag[0] == '0' ) /* 不包括上限值 */
            {
               if( stRelaCond.aczMinFlag[0] == '0' ) /* 不包括下限值 */
               {
                  if(strcmp(stRelaCond.aczMinValue,stRelaCond.aczMaxValue) == 0)
                  {
                     stRelaCond.aczRelation[0] = EQ;
                  }
                  else
                  {
                     if( strlen( stRelaCond.aczMaxValue ) == 0 ) /* < */
                     {
                        stRelaCond.aczRelation[0] = LT;
                     }
                     if( strlen( stRelaCond.aczMinValue ) == 0 ) /* > */
                     {
                        printf( "%d in %s 非法的条件%d",
                                __LINE__, __FILE__, stRelaCond.iCondId );
                     }
                  }
               }
               else                                  /* 包括下限值   */
               {
                  if( strlen( stRelaCond.aczMaxValue ) == 0 ) /* < */
                  {
                     stRelaCond.aczRelation[0] = GE;
                  }
                  else
                  {
                     printf( "%d in %s 无效的条件%d\n",
                             __LINE__, __FILE__, stRelaCond.iCondId );
                  }
               }
            }
            else                                  /* 包括上限值   */
            {
               /* 包括下限值暂时认为是相等 */
               if( stRelaCond.aczMinFlag[0] == '1' )
               {
                  stRelaCond.aczRelation[0] = EQ;
               }
               else
               {
                  /* 暂时不处理 */
                  printf( "%d in %s 碰上一个需要额外处理的关系%d,请联系\n",
                          __LINE__, __FILE__, stRelaCond.iCondId );
               }
            }
         }
         else
         {
            if(strcmp(stRelaCond.aczMinValue,stRelaCond.aczMaxValue) == 0)
            {
               stRelaCond.aczRelation[0] = NE;
            }
            else
            {
               /* 暂时不处理 */
               printf( "%d in %s 碰上一个需要额外处理的关系%d,请联系\n",
                       __LINE__, __FILE__, stRelaCond.iCondId );
            }
         }
#endif
            stRelaCond.iRSubSeq = atoi( fldstrs[15] );
            stRelaCond.iRMsgDscrb = atoi( fldstrs[16] );
            stRelaCond.iRFldSeq = atoi( fldstrs[17] );
            strcpy( stRelaCond.aczRFldType, fldstrs[18] );
            stRelaCond.iRStartBit = atoi( fldstrs[19] );
            stRelaCond.iREndBit = atoi( fldstrs[20] );

         PUB_StrTrim( stRelaCond.aczLFldType, ' ' );
         PUB_StrTrim( stRelaCond.aczMaxValue, ' ' );
         PUB_StrTrim( stRelaCond.aczMinValue, ' ' );
         PUB_StrTrim( stRelaCond.aczFlag,     ' ' );
         PUB_StrTrim( stRelaCond.aczMaxFlag,  ' ' );
         PUB_StrTrim( stRelaCond.aczMinFlag,  ' ' );
         PUB_StrTrim( stRelaCond.aczRelation, ' ' );
         PUB_StrTrim( stRelaCond.aczRFldType, ' ' );
         memcpy( epcShmAddr+iOffset, &stRelaCond, sizeof(RelaCond) );
	 iOffset += sizeof(RelaCond);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );







   memcpy( epcShmAddr+sizeof(int)*SERVICE, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[SERVICE], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/service.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%sservice.txt", pczDataPath );
   }

   printf( "put the file service.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stService, 0x00, sizeof(stService) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 7)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 7, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stService.iNodeId = atoi( fldstrs[0] );
            strcpy( stService.aczTranCode, fldstrs[1] );
            stService.iSvcId = atoi( fldstrs[2] );
            strcpy( stService.aczSvcName, fldstrs[3] );
            stService.iTimeOut = atoi( fldstrs[4] );
            stService.iBalanceFlag = atoi( fldstrs[5] );
            strcpy( stService.aczSvcNote, fldstrs[6] );

         PUB_StrTrim( stService.aczTranCode, ' ' );
         PUB_StrTrim( stService.aczSvcName,  ' ' );
         PUB_StrTrim( stService.aczSvcNote,  ' ' );
         memcpy( epcShmAddr+iOffset, &stService, sizeof(Service) );
	 iOffset += sizeof(Service);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );






   memcpy( epcShmAddr+sizeof(int)*SERVICECFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[SERVICECFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/servicecfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%sservicecfg.txt", pczDataPath );
   }

   printf( "put the file servicecfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stServiceCfg, 0x00, sizeof(stServiceCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 15)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 15, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stServiceCfg.iChnNodeId = atoi( fldstrs[0] );
            strcpy( stServiceCfg.aczSvcTranCode, fldstrs[1] );
            stServiceCfg.iSvcId = atoi( fldstrs[2] );
            stServiceCfg.iHostNodeId = atoi( fldstrs[3] );
            stServiceCfg.iSubSeq = atoi( fldstrs[4] );
            stServiceCfg.iCorrSubId = atoi( fldstrs[5] );
            stServiceCfg.iErrorHandleType = atoi( fldstrs[6] );
            stServiceCfg.iErrSubId = atoi( fldstrs[7] );
            stServiceCfg.iRetryTime = atoi( fldstrs[8] );
            stServiceCfg.iOutTime = atoi( fldstrs[9] );
            stServiceCfg.iParentSubSeq = atoi( fldstrs[10] );
            stServiceCfg.iSubLayer = atoi( fldstrs[11] );
            stServiceCfg.iBatFlag = atoi( fldstrs[12] );
            stServiceCfg.iBatMode = atoi( fldstrs[13] );
            stServiceCfg.iLstId = atoi( fldstrs[14] );


         PUB_StrTrim( stServiceCfg.aczSvcTranCode, ' ' );
         memcpy( epcShmAddr+iOffset, &stServiceCfg, sizeof(ServiceCfg) );
	 iOffset += sizeof(ServiceCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );









   memcpy( epcShmAddr+sizeof(int)*SUBSERVICE, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[SUBSERVICE], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/subservice.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%ssubservice.txt", pczDataPath );
   }

   printf( "put the file subservice.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stSubService, 0x00, sizeof(stSubService) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 10)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 10, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stSubService.iNodeId = atoi( fldstrs[0] );
            strcpy( stSubService.aczTranCode, fldstrs[1] );
            stSubService.iSubId = atoi( fldstrs[2] );
            strcpy( stSubService.aczSubName, fldstrs[3] );
            stSubService.iTempletId = atoi( fldstrs[4] );
            stSubService.iTranCodeSeq = atoi( fldstrs[5] );
            strcpy( stSubService.aczSuccCode, fldstrs[6] );
            stSubService.iRecordPerTime = atoi( fldstrs[7] );
            stSubService.iMsgAppId = atoi( fldstrs[8] );
            strcpy( stSubService.aczNotes, fldstrs[9]);

         PUB_StrTrim( stSubService.aczTranCode, ' ' );
         PUB_StrTrim( stSubService.aczSubName,  ' ' );
         PUB_StrTrim( stSubService.aczSuccCode, ' ' );
         PUB_StrTrim( stSubService.aczNotes, ' ' );
         memcpy( epcShmAddr+iOffset, &stSubService, sizeof(SubService) );
	 iOffset += sizeof(SubService);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );






   memcpy( epcShmAddr+sizeof(int)*SUBSVCFLDSRCCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[SUBSVCFLDSRCCFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/subsvcfldsrccfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%ssubsvcfldsrccfg.txt", pczDataPath );
   }

   printf( "put the file subsvcfldsrccfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stSubSvcFldSrcCfg, 0x00, sizeof(stSubSvcFldSrcCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 9)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 9, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stSubSvcFldSrcCfg.iFldAsmId = atoi( fldstrs[0] );
            stSubSvcFldSrcCfg.iFldFragSeq = atoi( fldstrs[1] );
            stSubSvcFldSrcCfg.iSrcInfType = atoi( fldstrs[2] );
            stSubSvcFldSrcCfg.iSrcSubSeq = atoi( fldstrs[3] );
            stSubSvcFldSrcCfg.iSrcFldSeq = atoi( fldstrs[4] );
            stSubSvcFldSrcCfg.iSrcRepFldFlag = atoi( fldstrs[5] );
            stSubSvcFldSrcCfg.iSrcBeginPos = atoi( fldstrs[6] );
            stSubSvcFldSrcCfg.iSrcLength = atoi( fldstrs[7] );
            strcpy( stSubSvcFldSrcCfg.aczSrcNote, fldstrs[8] );

         memcpy( epcShmAddr+iOffset, &stSubSvcFldSrcCfg, sizeof(SubSvcFldSrcCfg) );
	 iOffset += sizeof(SubSvcFldSrcCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );









   memcpy( epcShmAddr+sizeof(int)*SUBSVCREQCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[SUBSVCREQCFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/subsvcreqcfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%ssubsvcreqcfg.txt", pczDataPath );
   }

   printf( "put the file subsvcreqcfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stSubSvcReqCfg, 0x00, sizeof(stSubSvcReqCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 13)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 13, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stSubSvcReqCfg.iSvcId = atoi( fldstrs[0] );
            stSubSvcReqCfg.iSubSeq = atoi( fldstrs[1] );
            stSubSvcReqCfg.iFldSeq = atoi( fldstrs[2] );
            stSubSvcReqCfg.iBatSubSeq = atoi( fldstrs[3] );
            stSubSvcReqCfg.iBatSubFldSeq = atoi( fldstrs[4] );
            stSubSvcReqCfg.iFldType = atoi( fldstrs[5] );
            stSubSvcReqCfg.iRepFldFlag = atoi( fldstrs[6] );
            stSubSvcReqCfg.iRepTmSubSeq = atoi( fldstrs[7] );
            stSubSvcReqCfg.iRepTmFld = atoi( fldstrs[8] );
            stSubSvcReqCfg.iRepTmType = atoi( fldstrs[9] );
            stSubSvcReqCfg.iFldFragCalFlag = atoi( fldstrs[10] );
            stSubSvcReqCfg.iFldAsmId = atoi( fldstrs[11] );
            stSubSvcReqCfg.iBalFlag = atoi( fldstrs[12] );

         memcpy( epcShmAddr+iOffset, &stSubSvcReqCfg, sizeof(SubSvcReqCfg) );
	 iOffset += sizeof(SubSvcReqCfg);

      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );








   memcpy( epcShmAddr+sizeof(int)*TRANFLDCFG, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[TRANFLDCFG], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/tranfldcfg.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%stranfldcfg.txt", pczDataPath );
   }

   printf( "put the file tranfldcfg.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   pcBegin = epcShmAddr+iOffset;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stTranFldCfg, 0x00, sizeof(stTranFldCfg) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 7)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 7, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stTranFldCfg.iNodeId = atoi( fldstrs[0] );
            strcpy( stTranFldCfg.aczTranCode, fldstrs[1] );
            stTranFldCfg.iMsgDscrb = atoi( fldstrs[2] );
            strcpy( stTranFldCfg.aczRepTimesSeq, fldstrs[3] );
            strcpy( stTranFldCfg.aczRepTimesNum, fldstrs[4] );
            strcpy( stTranFldCfg.aczRepType, fldstrs[5] );
            strcpy( stTranFldCfg.aczTranFld, fldstrs[6] );

         PUB_StrTrim( stTranFldCfg.aczTranCode,    ' ' );
         PUB_StrTrim( stTranFldCfg.aczRepTimesSeq, ' ' );
         PUB_StrTrim( stTranFldCfg.aczRepTimesNum, ' ' );
         PUB_StrTrim( stTranFldCfg.aczRepType,     ' ' );
         PUB_StrTrim( stTranFldCfg.aczTranFld,     ' ' );
         memcpy( epcShmAddr+iOffset, &stTranFldCfg, sizeof(TranFldCfg) );
	 iOffset += sizeof(TranFldCfg);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );

/****
   qsort( pcBegin, iRow, sizeof(stTranFldCfg), sortTranFldCfg );
****/

   printf( "put %d records into share memory\n", iRow );







   memcpy( epcShmAddr+sizeof(int)*RETCODEMAP, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[RETCODEMAP], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/retcodemap.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%sretcodemap.txt", pczDataPath );
   }








   memcpy( epcShmAddr+sizeof(int)*TRANCODEMAP, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[TRANCODEMAP], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/trancode.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%strancode.txt", pczDataPath );
   }

   printf( "put the file trancode.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stTranCodeMap, 0x00, sizeof(stTranCodeMap) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 15)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 15, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }
            strcpy( stTranCodeMap.aczCityCode, fldstrs[0] );
            strcpy( stTranCodeMap.aczTTxKind, fldstrs[1] );
            strcpy( stTranCodeMap.aczTTxCode, fldstrs[2] );
            strcpy( stTranCodeMap.aczTTxName, fldstrs[3] );
            strcpy( stTranCodeMap.aczVTxCode, fldstrs[4] );
            strcpy( stTranCodeMap.aczVReqCode, fldstrs[5] );
            strcpy( stTranCodeMap.aczVTxName, fldstrs[6] );
            strcpy( stTranCodeMap.aczCashFlag, fldstrs[7] );
            strcpy( stTranCodeMap.aczKTxCode, fldstrs[8] );
            strcpy( stTranCodeMap.aczKTxName, fldstrs[9] );
            strcpy( stTranCodeMap.aczOTxCode, fldstrs[10] );
            strcpy( stTranCodeMap.aczOTxName, fldstrs[11] );
            strcpy( stTranCodeMap.aczBusiCode, fldstrs[12] );
            strcpy( stTranCodeMap.aczAgUnitNo, fldstrs[13] );
            strcpy( stTranCodeMap.aczState, fldstrs[14] );

         PUB_StrTrim( stTranCodeMap.aczCityCode,  ' ' );
         PUB_StrTrim( stTranCodeMap.aczTTxKind,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczTTxCode,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczTTxName,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczVTxCode,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczVReqCode,  ' ' );
         PUB_StrTrim( stTranCodeMap.aczVTxName,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczCashFlag,  ' ' );
         PUB_StrTrim( stTranCodeMap.aczKTxCode,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczKTxName,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczOTxCode,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczOTxName,   ' ' );
         PUB_StrTrim( stTranCodeMap.aczBusiCode,  ' ' );
         PUB_StrTrim( stTranCodeMap.aczAgUnitNo,  ' ' );
         PUB_StrTrim( stTranCodeMap.aczState,     ' ' );
         memcpy( epcShmAddr+iOffset, &stTranCodeMap, sizeof(TranCodeMap) );
	 iOffset += sizeof(TranCodeMap);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );









   memcpy( epcShmAddr+sizeof(int)*KERNSVC, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[KERNSVC], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/kernsvc.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%skernsvc.txt", pczDataPath );
   }

   printf( "put the file kernsvc.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stKernSvc, 0x00, sizeof(stKernSvc) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 3)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 3, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stKernSvc.iNodeId = atoi( fldstrs[0] );
            strcpy( stKernSvc.aczTranCode, fldstrs[1] );
            strcpy( stKernSvc.aczSvcName, fldstrs[2] );

         PUB_StrTrim( stKernSvc.aczTranCode,  ' ' );
         PUB_StrTrim( stKernSvc.aczSvcName,  ' ' );
         memcpy( epcShmAddr+iOffset, &stKernSvc, sizeof(KernSvc) );
	 iOffset += sizeof(KernSvc);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );


/**********************************************************************/




/**********************************************************************/


   memcpy( epcShmAddr+sizeof(int)*COMMROUTE, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[COMMROUTE], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/commroute.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%scommroute.txt", pczDataPath );
   }

   printf( "put the file commroute.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stCommRoute, 0x00, sizeof(stCommRoute) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 4)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 4, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stCommRoute.iNodeId = atoi( fldstrs[0] );
            stCommRoute.iCommType = atoi( fldstrs[1] );
            strcpy( stCommRoute.aczCond, fldstrs[2] );
            strcpy( stCommRoute.aczPara, fldstrs[3] );

         PUB_StrTrim( stCommRoute.aczCond,  ' ' );
         PUB_StrTrim( stCommRoute.aczPara,  ' ' );
         memcpy( epcShmAddr+iOffset, &stCommRoute, sizeof(CommRoute) );
	 iOffset += sizeof(CommRoute);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );


/**********************************************************************/





/**********************************************************************/


   memcpy( epcShmAddr+sizeof(int)*FLDMAP, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[FLDMAP], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/fldmap.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%sfldmap.txt", pczDataPath );
   }

   printf( "put the file fldmap.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stFldMap, 0x00, sizeof(stFldMap) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 2)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 2, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            strcpy( stFldMap.aczSource, fldstrs[0] );
            strcpy( stFldMap.aczTarget, fldstrs[1] ); pcTmp1 = aczLine;


         PUB_StrTrim( stFldMap.aczSource,  ' ' );
         PUB_StrTrim( stFldMap.aczTarget,  ' ' );
         memcpy( epcShmAddr+iOffset, &stFldMap, sizeof(FldMap) );
	 iOffset += sizeof(FldMap);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );


/**********************************************************************/




/***********************************************************************
   memcpy( epcShmAddr+sizeof(int)*CONSTCOND, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[CONSTCOND], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/constcond.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%sconstcond.txt", pczDataPath );
   }

   printf( "put the file constcond.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stConstCond, 0x00, sizeof(stConstCond) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 13)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 13, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stConstCond.iCondId = atoi( fldstrs[0] );
            stConstCond.iLayerId = atoi( fldstrs[1] );
            stConstCond.iSubSeq = atoi( fldstrs[2] );
            stConstCond.iMsgDscrb = atoi( fldstrs[3] );
            stConstCond.iFldSeq = atoi( fldstrs[4] );
            strcpy( stConstCond.aczFldType, fldstrs[5] );
            stConstCond.iStartBit = atoi( fldstrs[6] );
            stConstCond.iEndBit = atoi( fldstrs[7] );
            strcpy( stConstCond.aczMaxValue, fldstrs[8] );
            strcpy( stConstCond.aczMinValue, fldstrs[9] );
            strcpy( stConstCond.aczFlag, fldstrs[10] );
            strcpy( stConstCond.aczMaxFlag, fldstrs[11] );
            strcpy( stConstCond.aczMinFlag, fldstrs[12] );

         PUB_StrTrim( stConstCond.aczFldType,  ' ' );
         PUB_StrTrim( stConstCond.aczMaxValue, ' ' );
         PUB_StrTrim( stConstCond.aczMinValue, ' ' );
         PUB_StrTrim( stConstCond.aczFlag,     ' ' );
         PUB_StrTrim( stConstCond.aczMaxFlag,  ' ' );
         PUB_StrTrim( stConstCond.aczMinFlag,  ' ' );

         memcpy( epcShmAddr+iOffset, &stConstCond, sizeof(ConstCond) );
	 iOffset += sizeof(ConstCond);

      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );
***********************************************************************/


{  /* LOGWATCH 配置数据 */
   memcpy( epcShmAddr+sizeof(int)*LOGWATCH, &iOffset, sizeof(int) );
   memcpy( epcShmAddr+iOffset, &aiTableRows[LOGWATCH], sizeof(int) );
   iOffset += sizeof(int);
   if( pcTemp == NULL || *(pcTemp + 1) != '\0' )
   {
      sprintf( aczDataFile, "%s/logwatch.txt", pczDataPath );
   }
   else
   {
      sprintf( aczDataFile, "%slogwatch.txt", pczDataPath );
   }

   printf( "put the file logwatch.txt data to share memory\n" );
   fp = fopen( aczDataFile, "rb" );
   if( fp == NULL )
   {
      printf( "error, Can't open the file %s", aczDataFile );
      printf( "unloading the share memroy\n" );
      shmctl( iShmId, IPC_RMID, NULL );
      exit( 1 );
   }

   iRow = 0;
   do
   {
      memset( aczLine, 0x00, sizeof(aczLine) );
      fgets( aczLine, 2000, fp );
      memset( &stLogWatch, 0x00, sizeof(stLogWatch) );
      if( feof( fp ) == 0 )
      {
         iRow++;

         iTemp = strlen (aczLine) - 1;
         if (aczLine[iTemp] == '\n') aczLine[iTemp] = '\0';

         fldnum = PUB_ParseStr (aczLine, cDelimiter, fldstrs, 0);
         if (fldnum != 5)
         {
             printf( "%s:%d fld_num not match! Abort!"
		     "rownum=%d need %d != have %d",
		     __FILE__, __LINE__, iRow, 5, fldnum);
             shmctl( iShmId, IPC_RMID, NULL );
             exit( 1 );
         }

            stLogWatch.iTag = atoi( fldstrs[0] );
            stLogWatch.iNodeId = atoi( fldstrs[1] );
            stLogWatch.iDictId = atoi( fldstrs[2] );
            stLogWatch.iFldId = atoi( fldstrs[3] );
            stLogWatch.iFldType = atoi( fldstrs[4] );

         memcpy( epcShmAddr+iOffset, &stLogWatch, sizeof(LogWatch) );
	 iOffset += sizeof(LogWatch);
      }
   } while( feof( fp ) == 0 );
   fclose( fp );
   printf( "put %d records into share memory\n", iRow );
}

}

/*
**end of file
*/
