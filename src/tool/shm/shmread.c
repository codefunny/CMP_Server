/*
**file:  shmread.c
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shm_base.h"
#include "pub_cfg.h"
int iShmId;


ChkCfg          stChkCfg;
RelaCond        stRelaCond;
MsgFldCfg       stMsgFldCfg;
MsgNameId       stMsgNameId;
TranFldCfg      stTranFldCfg;
SubSvcFldSrcCfg stSubSvcFldSrcCfg;
SubSvcReqCfg    stSubSvcReqCfg;
Service         stService;
SubService      stSubService;
ServiceCfg      stServiceCfg;
NodeTranCfg     stNodeTranCfg;
NodeCommCfg     stNodeCommCfg;
RetCodeMap      stRetCodeMap;
TranCodeMap     stTranCodeMap;
KernSvc         stKernSvc;
CommRoute       stCommRoute;
FldMap		stFldMap;
LogWatch	stLogWatch;
/*ConstCond       stConstCond;*/

int main( int argc, char *argv[] )
{
   void *pcShmAddr,*pcDataStartAddr;
   int  iIpcKey, i;
   char aczTableName[100]; 
   int  iTableNo;
   int  iTimeStamp;


   if( argc == 1 )
   {
      exit(1);
   }

   /****修改　为支持动态配置数据装载   
   iIpcKey = atoi( argv[1] );
   iShmId = shmget( iIpcKey, 0, 0660 );
   if ( iShmId < 0 )
   {
       printf( " get share memory ID err\n" );
       exit(1);
   }

   pcShmAddr = shmat( iShmId, 0, 0 );
   if( pcShmAddr == (void *)(-1) )
   {
      printf( "attach error!\n" );
      exit(1);
   }

   printf( "id %d addr %x\n", iShmId, pcShmAddr );
   ***/

   /*** 2005-5-31 modify 
   iIpcKey = Shm_GetCfgShmKey (atoi(argv[1]), &iTimeStamp);
   if (iIpcKey <=0 ) {
       printf ("Get Config Share Memory key error\n");
       exit (1);
   }

   printf("Config share memory, Key is [%d]\n", iIpcKey );
   printf("Last Update Time is         %s\n", ctime((long*)&iTimeStamp) );

   iShmId = shmget( iIpcKey, 0, CFGSHMMODE );
   if ( iShmId < 0 )
   {
       printf( " get share memory ID err\n" );
       exit(1);
   }

   pcShmAddr = shmat( iShmId, 0, 0 );
   if( pcShmAddr == (void *)(-1) )
   {
      printf( "attach error!\n" );
      exit(1);
   }
   *************************/
   pcShmAddr = Shm_GetCfgShm (atoi(argv[1]), &iIpcKey);
   if (pcShmAddr == NULL ) {
       printf ("Get Config Share Memory error\n");
       exit (1);
   }
   printf("Config share memory, Key is [%d]\n", iIpcKey );
   /************************/

   if (argc < 3) {
       strcpy (aczTableName, "all");
   } else {
       for (i = 0 ; i < strlen (argv[2]); i++) {
           aczTableName[i] = tolower (argv[2][i]);
       }
       aczTableName[i] = '\0';
   }

   printf ("Table Name :%s\n", aczTableName);
   if (strcmp (aczTableName, "all") == 0) {
       for (i = 0; i < sizeof (paczTableName) / sizeof (char *); i++) {
           printf ("\n\n");
           show_table (pcShmAddr, i);
       }
   } else { 
       for (i = 0; i < sizeof (paczTableName) / sizeof (char *); i++) {
           if (strcmp (paczTableName[i], aczTableName) == 0) {
               show_table (pcShmAddr, i);
               break;
           }
       }
   }
   shmdt( pcShmAddr );

   return 0;

} 	/*end of main*/

int show_table (void * pcShmAddr, int iTableNo)
{ 
   int *opiNum, i;
   int rownum;
   void * pcDataStartAddr;

   opiNum  = &rownum;

   switch (iTableNo)
   {
   case CHKCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, CHKCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
        printf(" SHM_ReadData CHKCFG error!\n");
        exit(1);
     }
     else
     {
        printf( "table chkcfg:row %d\n", *opiNum );
        for (i=0; i< (*opiNum);i++)
        {
        memset( &stChkCfg, 0x00, sizeof(stChkCfg) );
        memcpy(&stChkCfg,pcDataStartAddr,sizeof(stChkCfg) );
        pcDataStartAddr = (char*)((long)pcDataStartAddr + sizeof(stChkCfg));

        printf( "%d|%d|%d|%d|%d|%s\n",
                  stChkCfg.iSvcId,
                  stChkCfg.iCurSubSeq,
                  stChkCfg.iCondId,
                  stChkCfg.iNextSubSeq,
                  stChkCfg.iDfltFlag,
                  stChkCfg.aczChkExp );
       }
     }
     break;

   case MSGFLDCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, MSGFLDCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData MSGFLDCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table MSGFLDCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stMsgFldCfg, 0x00, sizeof(stMsgFldCfg) );
     memcpy(&stMsgFldCfg,pcDataStartAddr,sizeof(stMsgFldCfg));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stMsgFldCfg) );
     printf( "%d|%d|%d|%s|%s|%d|%d|%d|%d|%d|%s|%s|%s|%s|%s\n",
              stMsgFldCfg.iMsgAppId,
              stMsgFldCfg.iFldSeq,
              stMsgFldCfg.iSepFldSeq,
              stMsgFldCfg.aczFldEngName,
              stMsgFldCfg.aczFldChnName,
              stMsgFldCfg.iSepFldFlag,
              stMsgFldCfg.iLenFldLen,
              stMsgFldCfg.iFldLen,
              stMsgFldCfg.iLenFldType,
              stMsgFldCfg.iFldType,
              stMsgFldCfg.aczLenFldDef,
              stMsgFldCfg.aczFldDef,
              stMsgFldCfg.aczLenFldAlign,
              stMsgFldCfg.aczFldAlign,
              stMsgFldCfg.aczSepSymbol );
     }
     }
   break;

   case MSGNAMEID:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr,  MSGNAMEID, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData  MSGNAMEID  error!\n");
     exit(1);
     }
     else
     {
      printf( "table  MSGNAMEID:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stMsgNameId, 0x00, sizeof(stMsgNameId) );
     memcpy(&stMsgNameId,pcDataStartAddr,sizeof(stMsgNameId));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stMsgNameId) );
      printf( "%d|%s\n",
              stMsgNameId.iMsgAppId,
              stMsgNameId.aczMsgAppName);
     }
     }
   break;

   case NODECOMMCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, NODECOMMCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData NODECOMMCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table NODECOMMCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stNodeCommCfg, 0x00, sizeof(stNodeCommCfg) );
      memcpy(&stNodeCommCfg,pcDataStartAddr,sizeof(stNodeCommCfg));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stNodeCommCfg) );
      printf( "%d|%s|%d|%d|%d|%s|%s|%s|%d|%s|%s|%d|%d|%s|%d|%d|%d|%d|%d\n",
              stNodeCommCfg.iNodeId,
              stNodeCommCfg.aczNodeName,
              stNodeCommCfg.iNodeDscrb,
              stNodeCommCfg.iNetProto,
              stNodeCommCfg.iLinkType,
              stNodeCommCfg.aczSyncFlag,
              stNodeCommCfg.aczHostIp,
              stNodeCommCfg.aczHostName,
              stNodeCommCfg.iHostCommPort,
              stNodeCommCfg.aczMonitorIp,
              stNodeCommCfg.aczMtrHostName,
              stNodeCommCfg.iMtrPort,
              stNodeCommCfg.iMsgAppId,
              stNodeCommCfg.aczEncrptType,
              stNodeCommCfg.iMinProcess,
              stNodeCommCfg.iMaxProcess,
              stNodeCommCfg.iUsableFlag,
              stNodeCommCfg.iSvrKeyId,
              stNodeCommCfg.iSvwKeyId );
    }
    }
   break;

   case NODETRANCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, NODETRANCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData NODETRANCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table NODETRANCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );
     memcpy(&stNodeTranCfg,pcDataStartAddr,sizeof(stNodeTranCfg));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stNodeTranCfg) );
     printf( "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d\n",
              stNodeTranCfg.iNodeId,
              stNodeTranCfg.iMsgAppId,
              stNodeTranCfg.aczMsgAppName,
              stNodeTranCfg.iToMidTranCodeId,
              stNodeTranCfg.iToMsgTranCodeId,
              stNodeTranCfg.iToMidId,
              stNodeTranCfg.iToMsgId,
              stNodeTranCfg.iToMidIsSt,
              stNodeTranCfg.iToMsgIsSt,
              stNodeTranCfg.aczCrtTranCodeType,
              stNodeTranCfg.aczCrtTranCodePos,
              stNodeTranCfg.aczCrtTranCodeSeq,
              stNodeTranCfg.aczCrtMsgFixFld,
              stNodeTranCfg.aczWrgTranCodeType,
              stNodeTranCfg.aczWrgTranCodePos,
              stNodeTranCfg.aczWrgTranCodeSeq,
              stNodeTranCfg.aczWrgMsgFixFld,
              stNodeTranCfg.aczLogTabName,
              stNodeTranCfg.aczLogFld,
              stNodeTranCfg.iMaxRetryTime,
              stNodeTranCfg.iDelayTime );
      }
      }
    break;

    case RELACOND:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, RELACOND, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData RELACOND  error!\n");
     exit(1);
     }
     else
     {
      printf( "table RELACOND:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stRelaCond, 0x00, sizeof(stRelaCond) );
     memcpy(&stRelaCond,pcDataStartAddr,sizeof(stRelaCond));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stRelaCond) );
      printf("%d|%d|%d|%d|%d|%s|%d|%d|%s|%d|%d|%d|%s|%d|%d\n",
              stRelaCond.iCondId,
              stRelaCond.iExprId,
              stRelaCond.iLSubSeq,
              stRelaCond.iLMsgDscrb,
              stRelaCond.iLFldSeq,
              stRelaCond.aczLFldType,
              stRelaCond.iLStartBit,
              stRelaCond.iLEndBit,
              stRelaCond.aczRelation,
              stRelaCond.iRSubSeq,
              stRelaCond.iRMsgDscrb,
              stRelaCond.iRFldSeq,
              stRelaCond.aczRFldType,
              stRelaCond.iRStartBit,
              stRelaCond.iREndBit );
     }
     }
   break;

   case SERVICE:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, SERVICE, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData SERVICE  error!\n");
     exit(1);
     }
     else
     {
      printf( "table SERVICE:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stService, 0x00, sizeof(stService) );
     memcpy(&stService,pcDataStartAddr,sizeof(stService));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stService) );
      printf( "%d|%s|%d|%s|%d|%d|%s\n",
              stService.iNodeId, 
              stService.aczTranCode, 
              stService.iSvcId, 
              stService.aczSvcName, 
              stService.iTimeOut, 
              stService.iBalanceFlag, 
              stService.aczSvcNote );
     }
     }
   break;

   case SERVICECFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, SERVICECFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData SERVICECFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table SERVICECFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stServiceCfg, 0x00, sizeof(stServiceCfg) );
     memcpy(&stServiceCfg,pcDataStartAddr,sizeof(stServiceCfg));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stServiceCfg) );
     printf( "%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
              stServiceCfg.iChnNodeId,
              stServiceCfg.aczSvcTranCode,
              stServiceCfg.iSvcId,
              stServiceCfg.iHostNodeId,
              stServiceCfg.iSubSeq,
              stServiceCfg.iCorrSubId,
              stServiceCfg.iErrorHandleType,
              stServiceCfg.iErrSubId,
              stServiceCfg.iRetryTime,
              stServiceCfg.iOutTime,
              stServiceCfg.iParentSubSeq,
              stServiceCfg.iSubLayer,
              stServiceCfg.iBatFlag,
              stServiceCfg.iBatMode,
              stServiceCfg.iLstId );
     }
     }
   break;

   case SUBSERVICE:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, SUBSERVICE, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData SUBSERVICE  error!\n");
     exit(1);
     }
     else
     {
      printf( "table SUBSERVICE:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stSubService, 0x00, sizeof(stSubService) );
     memcpy(&stSubService,pcDataStartAddr,sizeof(stSubService));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stSubService) );

     printf( "%d|%s|%d|%s|%d|%d|%s|%d|%d|%s|\n",
              stSubService.iNodeId,
              stSubService.aczTranCode,
              stSubService.iSubId,
              stSubService.aczSubName,
              stSubService.iTempletId,
              stSubService.iTranCodeSeq,
              stSubService.aczSuccCode,
              stSubService.iRecordPerTime,
              stSubService.iMsgAppId,
              stSubService.aczNotes );
     }
     }
  break;

  case SUBSVCFLDSRCCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, SUBSVCFLDSRCCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData  SUBSVCFLDSRCCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table  SUBSVCFLDSRCCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stSubSvcFldSrcCfg, 0x00, sizeof(stSubSvcFldSrcCfg) );
     memcpy(&stSubSvcFldSrcCfg,pcDataStartAddr,sizeof(stSubSvcFldSrcCfg));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stSubSvcFldSrcCfg) );
      printf( "%d|%d|%d|%d|%d|%d|%d|%d|%s\n",
              stSubSvcFldSrcCfg.iFldAsmId,
              stSubSvcFldSrcCfg.iFldFragSeq,
              stSubSvcFldSrcCfg.iSrcInfType,
              stSubSvcFldSrcCfg.iSrcSubSeq,
              stSubSvcFldSrcCfg.iSrcFldSeq,
              stSubSvcFldSrcCfg.iSrcRepFldFlag,
              stSubSvcFldSrcCfg.iSrcBeginPos,
              stSubSvcFldSrcCfg.iSrcLength,
              stSubSvcFldSrcCfg.aczSrcNote );
    }
    }
  break;

  case SUBSVCREQCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, SUBSVCREQCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData SUBSVCREQCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table SUBSVCREQCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stSubSvcReqCfg, 0x00, sizeof(stSubSvcReqCfg) );
      memcpy(&stSubSvcReqCfg,pcDataStartAddr,sizeof(stSubSvcReqCfg));
      pcDataStartAddr =  (char *)((long)pcDataStartAddr + sizeof(stSubSvcReqCfg) );
      printf( "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
              stSubSvcReqCfg.iSvcId,
              stSubSvcReqCfg.iSubSeq,
              stSubSvcReqCfg.iFldSeq,
              stSubSvcReqCfg.iBatSubSeq,
              stSubSvcReqCfg.iBatSubFldSeq,
              stSubSvcReqCfg.iFldType,
              stSubSvcReqCfg.iRepFldFlag,
              stSubSvcReqCfg.iRepTmSubSeq,
              stSubSvcReqCfg.iRepTmFld,
              stSubSvcReqCfg.iRepTmType,
              stSubSvcReqCfg.iFldFragCalFlag,
              stSubSvcReqCfg.iFldAsmId,
              stSubSvcReqCfg.iBalFlag );
    }
    }
  break;

  case  TRANFLDCFG:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, TRANFLDCFG, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData TRANFLDCFG  error!\n");
     exit(1);
     }
     else
     {
      printf( "table TRANFLDCFG:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
     memset( &stTranFldCfg, 0x00, sizeof(stTranFldCfg) );
     memcpy(&stTranFldCfg,pcDataStartAddr,sizeof(stTranFldCfg));
     pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stTranFldCfg) );

      printf( "%d|%s|%d|%s|%s|%s|%s\n",
              stTranFldCfg.iNodeId,
              stTranFldCfg.aczTranCode,
              stTranFldCfg.iMsgDscrb,
              stTranFldCfg.aczRepTimesSeq,
              stTranFldCfg.aczRepTimesNum,
              stTranFldCfg.aczRepType,
              stTranFldCfg.aczTranFld );
     }
     }
  break;

  case RETCODEMAP:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, RETCODEMAP, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData RETCODEMAP  error!\n");
     exit(1);
     }
     else
     {
      printf( "table RETCODEMAP:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stRetCodeMap, 0x00, sizeof(stRetCodeMap) );
      memcpy(&stRetCodeMap,pcDataStartAddr,sizeof(stRetCodeMap));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stRetCodeMap));
/**********************************************************
      printf( "%d|%d|%d|%d|%d|%s|%s|%s\n",
              stRetCodeMap.iHostNodeId,
              stRetCodeMap.iHostRetCodeType,
              stRetCodeMap.iChanNodeId,
              stRetCodeMap.iChanRetCodeType,
              stRetCodeMap.iELRetCode,
              stRetCodeMap.aczChanRetCode,
              stRetCodeMap.aczHostRetCode,
              stRetCodeMap.aczErrorMessage );
**********************************************************/
    }
    }
  break;

  case TRANCODEMAP:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, TRANCODEMAP, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData TRANCODEMAP  error!\n");
     exit(1);
     }
     else
     {
      printf( "table TRANCODEMAP:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stTranCodeMap, 0x00, sizeof(stTranCodeMap) );
      memcpy(&stTranCodeMap,pcDataStartAddr,sizeof(stTranCodeMap));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stTranCodeMap) );
      printf( "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
              stTranCodeMap.aczCityCode,
              stTranCodeMap.aczTTxKind, 
              stTranCodeMap.aczTTxCode, 
              stTranCodeMap.aczTTxName, 
              stTranCodeMap.aczVTxCode, 
              stTranCodeMap.aczVReqCode,
              stTranCodeMap.aczVTxName, 
              stTranCodeMap.aczCashFlag,
              stTranCodeMap.aczKTxCode, 
              stTranCodeMap.aczKTxName, 
              stTranCodeMap.aczOTxCode, 
              stTranCodeMap.aczOTxName, 
              stTranCodeMap.aczBusiCode,
              stTranCodeMap.aczAgUnitNo,
              stTranCodeMap.aczState );

    }
    }
  break;

  case KERNSVC:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, KERNSVC, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData KERNSVC  error!\n");
     exit(1);
     }
     else
     {
      printf( "table KERNSVC:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stKernSvc, 0x00, sizeof(stKernSvc) );
      memcpy(&stKernSvc,pcDataStartAddr,sizeof(stKernSvc));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stKernSvc) );
      printf( "%d|%s|%s\n",
              stKernSvc.iNodeId,
              stKernSvc.aczTranCode,
              stKernSvc.aczSvcName );
    }
    }
  break;

  case COMMROUTE:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, COMMROUTE, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData COMMROUTE  error!\n");
     exit(1);
     }
     else
     {
      printf( "table COMMROUTE:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stCommRoute, 0x00, sizeof(stCommRoute) );
      memcpy(&stCommRoute,pcDataStartAddr,sizeof(stCommRoute));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stCommRoute) );
      printf( "%d|%d|%s|%s\n",
              stCommRoute.iNodeId,
              stCommRoute.iCommType,
              stCommRoute.aczCond,
              stCommRoute.aczPara );
    }
    }
  break;

  case FLDMAP:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, FLDMAP, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData FLDMAP  error!\n");
     exit(1);
     }
     else
     {
      printf( "table FLDMAP:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stFldMap, 0x00, sizeof(stFldMap) );
      memcpy(&stFldMap,pcDataStartAddr,sizeof(stFldMap));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stFldMap) );
      printf( "%s|%s\n",
              stFldMap.aczSource,
              stFldMap.aczTarget );
    }
   }
  break;

  case LOGWATCH:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, LOGWATCH, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
        printf(" SHM_ReadData LOGWATCH  error!\n");
        exit(1);
     }
     else
     {
        printf( "table LOGWATCH:row %d\n", *opiNum );
        for (i=0; i< (*opiNum);i++)
        {
           memset( &stLogWatch, 0x00, sizeof(stLogWatch) );
           memcpy(&stLogWatch,pcDataStartAddr,sizeof(stLogWatch));
           pcDataStartAddr = (char *)((long)pcDataStartAddr +
                                            sizeof(stLogWatch) );
           printf( "%d|%d|%d|%d|%d\n",
                   stLogWatch.iTag,
                   stLogWatch.iNodeId,
                   stLogWatch.iDictId,
                   stLogWatch.iFldId,
                   stLogWatch.iFldType );
    }
    }
  break;

/********************************************************************

  case CONSTCOND:
     pcDataStartAddr =  (void *) SHM_ReadData ( pcShmAddr, CONSTCOND, opiNum);

     if ( ( pcDataStartAddr == NULL ) || ( opiNum == NULL ))
     {
     printf(" SHM_ReadData CONSTCOND  error!\n");
     exit(1);
     }
     else
     {
      printf( "table constcond:row %d\n", *opiNum );
     for (i=0; i< (*opiNum);i++)
     {
      memset( &stConstCond, 0x00, sizeof(stConstCond) );
      memcpy(&stConstCond,pcDataStartAddr,sizeof(stConstCond));
      pcDataStartAddr = (char *)((long)pcDataStartAddr + sizeof(stConstCond) );

      printf( "%d|%d|%d|%d|%d|%s|%d|%d|%s|%s|%s|%s|%s\n",
              stConstCond.iCondId,
              stConstCond.iExprId,
              stConstCond.iSubSeq,
              stConstCond.iMsgDscrb,
              stConstCond.iFldSeq,
              stConstCond.aczFldType,
              stConstCond.iStartBit,
              stConstCond.iEndBit,
              stConstCond.aczMaxValue,
              stConstCond.aczMinValue,
              stConstCond.aczFlag,
              stConstCond.aczMaxFlag,
              stConstCond.aczMinFlag );
    }
   }
  break;

*******************************************************************/
   }
   return 0;

}

/*
** end of this file
*/
