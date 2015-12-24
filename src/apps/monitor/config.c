#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shm_api.h"
#include "shm_base.h"
#include "easysock.h"

extern int giConfigKey;
extern char *pcShmAddr;

char * getShmAddr (int iConfigkey)
{
   int    iCfgShmKey;
   time_t iCfgShmTm;
   int    iShmId;
   void  *pcCfgShmAddr;

   /* ���ӹ����ڴ�:���������Ϣ�Ĺ����ڴ��IPCKEY�͸ÿ鹲���ڴ��ʱ��� */

/****
   iCfgShmKey = Shm_GetCfgShmKey( iConfigkey, &iCfgShmTm );
   if( iCfgShmKey <= 0 )
   {
      fprintf(stderr,
              "Get config share memory key error, KeyShmKey is %d\n",
               iConfigkey);
      return NULL;
   }

   iShmId = shmget( iCfgShmKey, 0, 0660 );
   if( iShmId == -1 )
   {
      fprintf (stderr,
               "Get Cfg share memory for channel error!"
               "IPCKEY = %d\n", iCfgShmKey );

      return NULL;
   }

   pcCfgShmAddr = NULL;
   pcCfgShmAddr = shmat( iShmId, 0, 0660 );
   if(pcCfgShmAddr == (void *)(-1))
   {
      fprintf (stderr,
               "attach config memory error\n"
               "IPCKEY = %d %s\n", iCfgShmKey, strerror (errno));

      return NULL;
   }

****/
    /** 
      * add for dynamic change config shm 
      **/
    pcCfgShmAddr = (char *)Shm_GetCfgShm (giConfigKey, &iCfgShmKey);
    if (pcCfgShmAddr == NULL) 
    {
        fprintf (stderr,
               "attach get config memory error\n"
               "KEYSHM IPCKEY = %d\n", giConfigKey);
        return NULL;
    }

    if( pcShmAddr != NULL ) 
    {
        shmdt( pcShmAddr );
        pcShmAddr = NULL;
    }

   pcShmAddr = pcCfgShmAddr;

   return pcCfgShmAddr;
}

void readWatchNodes( int client )
{
   char aczError[10] = "-1", aczNodeList[5000];
   int iShmId, i, j=0, iRowNum;
   NodeCommCfg *pstNodeCommCfg;

   pcShmAddr = getShmAddr (giConfigKey);
   if( pcShmAddr == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      return;
   }

   /** ���ڵ�ͨ�����ñ�, �鿴��ض˿��ֶ�(ʵ������Ϊ�Ƿ���)�Ƿ�Ϊ 1 */
   pstNodeCommCfg = (NodeCommCfg*)SHM_ReadData( pcShmAddr, NODECOMMCFG,
                                                &iRowNum );
   if( pstNodeCommCfg == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt( pcShmAddr );
      pcShmAddr = NULL;
      return;
   }
   memset( aczNodeList, 0, sizeof(aczNodeList) );
   for( i = 0; i < iRowNum; i++ )
   {
      if( pstNodeCommCfg->iMtrPort == 1 )
      {
         if( j == 0 )
         {
            sprintf( aczNodeList, "%d=%s", pstNodeCommCfg->iNodeId, 
                     pstNodeCommCfg->aczNodeName );
         }
         else
         {
            sprintf( aczNodeList, "%s,%d=%s", aczNodeList,
                     pstNodeCommCfg->iNodeId, 
                     pstNodeCommCfg->aczNodeName );
         }
         j++;
      }
      pstNodeCommCfg++;
   }

   if( tcp_wait_send( client, 5000 ) == true )
   {
      tcp_raw_send( client, aczNodeList, strlen(aczNodeList) );
   }

   shmdt( pcShmAddr );
   pcShmAddr = NULL;
}

typedef struct _ItemKind
{
   int  m_NodeId;
   int  m_SubNodeId;
   char m_Name[41];
} ItemKind;

/**
  * ��ָ���ڵ�Ľ�����
  * ��������򷵻� 0, �������򷵻� -1
  */
int readService( int client, int nodeid, char *pcShmAddr, ItemKind *pstItemKind,
                 int iSubNum )
{
   Service *pstService, *pstTmp;
   int iRowNum, i, j, k;
   char aczTranCode[200];

   pstService = (Service*)SHM_ReadData( pcShmAddr, SERVICE, &iRowNum );
   if( pstService == NULL )
   {
      return -1;
   }

   for( i = 0; i < iSubNum; i++ )
   {
      pstTmp = pstService;
      k = 0;
      for( j = 0; j < iRowNum; j++ )
      {
         if( pstTmp->iNodeId == nodeid &&
             pstTmp->iTimeOut == pstItemKind[i].m_SubNodeId )
         {
            if( k == 0 )
            {
               sprintf( aczTranCode, "%d=%s:%s=%s",
                        pstItemKind[i].m_SubNodeId,
                        pstItemKind[i].m_Name,
                        pstTmp->aczTranCode,
                        pstTmp->aczSvcName );
            }
            else
            {
               sprintf( aczTranCode, ",%s=%s",
                        pstTmp->aczTranCode,
                        pstTmp->aczSvcName );
            }
            if( tcp_wait_send( client, 5000 ) == false )
            {
               return 0;
            }
            tcp_raw_send( client, aczTranCode, strlen(aczTranCode) );
            k++;
         }
         pstTmp++;
      }
      if( k > 0 )
      {
         if( tcp_wait_send( client, 5000 ) == false )
         {
            return 0;
         }
         tcp_raw_send( client, ";", 1 );
      }
   }

   return 0;
}

/**
  * ��ָ���ڵ�Ľ�����
  * ��������򷵻� 0, �������򷵻� -1
  */
int readSubService( int client, int nodeid, char *pcShmAddr,
                    ItemKind *pstItemKind, int iSubNum )
{
   SubService *pstSubService, *pstTmp;
   int iRowNum, i, j, k;
   char aczTranCode[200];

   pstSubService = (SubService*)SHM_ReadData( pcShmAddr, SUBSERVICE, &iRowNum );
   if( pstSubService == NULL )
   {
      return -1;
   }

   for( i = 0; i < iSubNum; i++ )
   {
      pstTmp = pstSubService;
      k = 0;
      for( j = 0; j < iRowNum; j++ )
      {
         if( pstTmp->iNodeId == nodeid &&
             pstTmp->iRecordPerTime == pstItemKind[i].m_SubNodeId )
         {
            if( k == 0 )
            {
               sprintf( aczTranCode, "%d=%s:%s=%s",
                        pstItemKind[i].m_SubNodeId,
                        pstItemKind[i].m_Name,
                        pstTmp->aczTranCode,
                        pstTmp->aczSubName );
            }
            else
            {
               sprintf( aczTranCode, ",%s=%s",
                        pstTmp->aczTranCode,
                        pstTmp->aczSubName );
            }
            if( tcp_wait_send( client, 5000 ) == false )
            {
               return 0;
            }
            tcp_raw_send( client, aczTranCode, strlen(aczTranCode) );
            k++;
         }
         pstTmp++;
      }
      if( k > 0 )
      {
         if( tcp_wait_send( client, 5000 ) == false )
         {
            return 0;
         }
         tcp_raw_send( client, ";", 1 );
      }
   }

   return 0;
}

/**
  *������ģ��sanf����
  *��֧��%f[i] �̶���ʽ
  *%l[ ] �ɼ����ȷ����ʽϵͳ��֧��0x00Ϊ�����
  *�˺����������ַ������ ʹ��ת���ַ�Ҫע���ʽ�����\
  *epaczsource ����������
  *epaczFormat ���ݸ�ʽ
  *������ ��1
  *ע:�������Ĳ����͸�ʽ����Ķ�Ӧ��ϵ�������ܳ���coredump���,ˡû�а취����
*/
char * fvsscanf(char * epaczsource, char * epaczFormat , ...) 
{
   va_list arg_ptr; 
   char * pacs=NULL; 
   char * pacsource=NULL; 
   char * pacPlace =NULL; 
   char * pacPlace2 =NULL; 
   char   aczLimet[30][2]; /*��һλд���ܼ��������*/
   int iNum = 0 ,ilen = 0 ,iflag = 0 ,i ,ilen2,iendflag =0;

   va_start(arg_ptr, epaczFormat); 

   pacPlace = epaczFormat ;

   /*�Ը�ʽ���ֽ��н���*/
   while(1)
   {
      pacPlace = strchr(pacPlace,'%');
      if(pacPlace == NULL && iNum !=0  )
      {
         break ;
      }
      if(pacPlace == NULL && iNum ==0 )
      {
         va_end(arg_ptr); 
         return epaczsource + iNum ;
      }
      if( iendflag == 1)
      {
         va_end(arg_ptr); 
         return epaczsource + iNum ;
      }
      /*�Զ���������д���*/
      if( pacPlace[1] == 'f' || pacPlace[1] == 'F' )
      {
         char * p;
         char *p2;
         char aczTmp[20];

         /*������[]��֮������ݽ��н����ж�*/
         if( pacPlace[2] != '[' )
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }
         p = strchr(pacPlace,']');
         if (p == NULL)
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }

         if( p - pacPlace - 3 > 10 )
         {
            va_end(arg_ptr); 
            return (char *) -1 ;;
         }
         memset(aczTmp,0x00,sizeof(aczTmp) );
         memset(aczTmp,0x20,sizeof(aczTmp) -1);
         memcpy(aczTmp ,pacPlace + 3 , p - pacPlace - 3 );
         ilen2 = strspn(aczTmp,"0123456789");
         p2 = aczTmp + ilen2 ;
         if((ilen2 - ( p - pacPlace - 3 )) != 0 )
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }
         
         ilen = atoi( aczTmp );
         pacPlace2 = p + 1;
         iflag = 1 ;
         
      }
      /*�Լ������������д���*/
      if( pacPlace[1] == 'l' || pacPlace[1] == 'L' )
      {
         char * p;
         char *p2;
         char aczTmp[30];
         iflag = 2 ;
         /*������[]��֮������ݽ��н����ж�*/
         if( pacPlace[2] != '[' )
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }
         p = strchr(pacPlace,']');
         if (p == NULL)
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }

         if( p - pacPlace - 3 > 28 )
         {
            va_end(arg_ptr); 
            return (char *) -1 ;
         }

         memset(aczLimet,0x00,sizeof(aczLimet) );
         memset(aczTmp,0x00,sizeof(aczTmp) -1);
         memcpy(aczTmp ,pacPlace + 3 , p - pacPlace - 3 );
         aczLimet[0][0] = 1 ;

         for ( i = 0 ; i < p - pacPlace - 3 ; i++ )
         {
            int iiii ;

            if( aczTmp[i] == '\\' )
            {
               if(aczTmp[i +1] == 'n')
               { 
                  iiii =0;
                  iiii = aczLimet[0][0] ;
                  aczLimet[iiii][0] = '\n' ;
                  aczLimet[0][0] = iiii + 1 ;
               }

               if(aczTmp[i +1] == 'r')
               { 
                  iiii = 0;
                  iiii = aczLimet[0][0] ;
                  aczLimet[iiii][0] = 0x00 ;
                  aczLimet[0][0] = iiii + 1 ;
               }
               i ++ ;
            }
            else
            {
               iiii = 0;
               iiii = aczLimet[0][0] ;
               aczLimet[iiii][0] = aczTmp[i] ;
               aczLimet[0][0] = iiii + 1 ;
            }
         }

         i = aczLimet[0][0] ;
         aczLimet[i ][0] = 0x00 ;
         aczLimet[0][0] = i  + 1 ;

         pacPlace = p + 1;
      }
      
      if(iflag == 0)
      {
         return (char *) -1 ;
      }
      pacs = va_arg(arg_ptr, char* );
      /*����Ƕ�����*/
      if( iflag == 1)
      {
         memcpy(pacs,epaczsource + iNum , ilen );
         pacs[ilen] =0x00 ;
         if(strlen(pacs) != ilen )
         {
            iendflag = 1;
         }
         iNum = iNum + ilen ;
         pacPlace = pacPlace2 ;
         continue;
      }
      
      /*������÷ָ���ȷ�������*/
      if( iflag == 2)
      {
         char attt;
         int iiii ,iflagttt = 0;
         ilen = 0 ;
         while(1)
         {
            attt = epaczsource [iNum + ilen] ;

            if(attt == 0x00)
            {
               iendflag = 1;
               break;
            }
            for(iiii = 0 ;iiii < aczLimet[0][0] - 1 ;iiii ++)
            {
               if(attt  == aczLimet[iiii + 1][0] )
               {
                  iflagttt = 1;
                  break ;
               }
            }
            /*������ǽ����ַ�*/
            if( iflagttt != 1)
            {
               pacs[ilen ] = attt  ;
               ilen ++ ;
            }
            else
            {
               break;
            }
         }
         pacs[ilen ] = 0x00;

         iNum = iNum + ilen + 1;
      }
   }

   va_end(arg_ptr); 

   return epaczsource + iNum ; 
} 

int readSubNodeId( int nodeid, ItemKind *pstItemKind )
{
   int iNum = 0;
   FILE *fp;
   char aczFileName[128];
   char aczLine[1024], aczNodeId[20], aczSubNodeId[20], aczName[41];

   sprintf( aczFileName, "%s/data/itemkind.txt", getenv("MYROOT") );
   fp = fopen( aczFileName, "r" );
   if( fp == NULL )
   {
      fprintf( stderr, "fopen( %s, r ) error %s\n",
               aczFileName, strerror(errno) );
      return -1;
   }

   memset( aczLine, 0, sizeof(aczLine) );
   while( fgets( aczLine, 1023, fp ) != NULL )
   {
      memset( aczNodeId,    0, sizeof(aczNodeId) );
      memset( aczSubNodeId, 0, sizeof(aczSubNodeId) );
      memset( aczName,      0, sizeof(aczName) );
      fvsscanf( aczLine, "%l[|]%l[|]%l[|]", aczNodeId, aczSubNodeId, aczName );
      if( nodeid == atoi(aczNodeId) )
      {
         (pstItemKind+iNum)->m_NodeId = nodeid;
         (pstItemKind+iNum)->m_SubNodeId = atoi(aczSubNodeId);
         strcpy( (pstItemKind+iNum)->m_Name, aczName );
         iNum++;
      }
      memset( aczLine, 0, sizeof(aczLine) );
   }

   fclose( fp );

   return iNum;
}

/**
  * ��ָ���ڵ�Ľ�����
  */
void readNodesTrans( int client, int nodeid )
{
   char aczError[10] = "-1", aczNodeList[5000];
   int iShmId, i, j=0, iRowNum, iNodeType = -1, iSubNodeNum;
   ItemKind astItemKind[2000];
   NodeCommCfg *pstNodeCommCfg;

   pcShmAddr = getShmAddr (giConfigKey);
   if( pcShmAddr == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      return;
   }

   /* �ȶ��ڵ�ͨ�ű�, �ҵ��ýڵ��������ڵ㻹�������ڵ�    */
   /* Ȼ����ļ� data/itemkind.txt �ҳ����з��� */
   /* ��������/�ӷ���������Ϣ�ҳ�ÿ����������н���     */
   pstNodeCommCfg = (NodeCommCfg*)SHM_ReadData( pcShmAddr, NODECOMMCFG,
                                                &iRowNum );
   if( pstNodeCommCfg == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt (pcShmAddr);
      pcShmAddr = NULL;
      return;
   }

   for( i = 0; i < iRowNum; i++ )
   {
      if( pstNodeCommCfg->iNodeId == nodeid )
      {
         iNodeType = pstNodeCommCfg->iNodeDscrb;
         break;
      }
      pstNodeCommCfg++;
   }
   if( iNodeType == -1 )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt (pcShmAddr);
      pcShmAddr = NULL;
      return;
   }

   memset( astItemKind, 0, sizeof(astItemKind) );
   iSubNodeNum = readSubNodeId( nodeid, astItemKind );
   if( iSubNodeNum == -1 )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt (pcShmAddr);
      pcShmAddr = NULL;
      return;
   }

   if( iNodeType == 0 ) /* �����ڵ�, ����� */
   {
      if( readService( client, nodeid, pcShmAddr, astItemKind, iSubNodeNum )
          == 0 )
      {
         shmdt( pcShmAddr );
         pcShmAddr = NULL;
         return;
      }
   }
   else                 /* �����ڵ�, ���ӷ��� */
   {
      if( readSubService( client, nodeid, pcShmAddr, astItemKind, iSubNodeNum )
          == 0 )
      {
         shmdt( pcShmAddr );
         pcShmAddr = NULL;
         return;
      }
   }

   shmdt (pcShmAddr);
   pcShmAddr = NULL;
}

int bsearchFld( void *pLeft, void *pRight )
{
   MsgFldCfg *pstLeft, *pstRight;

   pstLeft  = (MsgFldCfg *)pLeft;
   pstRight = (MsgFldCfg *)pRight;

   if( pstLeft->iMsgAppId != pstRight->iMsgAppId )
   {
      return pstLeft->iMsgAppId - pstRight->iMsgAppId;
   }

   return pstLeft->iFldSeq - pstRight->iFldSeq;
}

int searchFieldByDictId( char *pcShmAddr, int iDictId, int iFldId,
                         char *pczFields )
{
   int iRowNum;
   MsgFldCfg *pstMsgFldCfg, stMsgFldCfgKey, *pstSearch;

   pstMsgFldCfg = (MsgFldCfg*)SHM_ReadData( pcShmAddr, MSGFLDCFG, &iRowNum );
   if( pstMsgFldCfg == NULL )
   {
      return -1;
   }

   memset( &stMsgFldCfgKey, 0, sizeof(stMsgFldCfgKey) );
   stMsgFldCfgKey.iMsgAppId = iDictId;
   stMsgFldCfgKey.iFldSeq   = iFldId;
   pstSearch = bsearch( &stMsgFldCfgKey, pstMsgFldCfg, iRowNum,
                        sizeof(stMsgFldCfgKey), bsearchFld );
   if( pstSearch != NULL )
   {
      if( strlen( pstMsgFldCfg->aczFldChnName ) > 0 )
      {
         sprintf( pczFields, "%d=%s,",
                  pstSearch->iFldSeq, pstSearch->aczFldChnName );
      }
      else
      {
         sprintf( pczFields, "%d=%s,",
                  pstSearch->iFldSeq, pstSearch->aczFldEngName );
      }
      return 0;
   }
   else
   {
      return -1;
   }
}

/**
  * ��ָ���ڵ���ֶ�
  */
void readNodeFields( int client, int nodeid )
{
   char aczError[10] = "-1", aczFields[5000];
   int iShmId, i, j=0, iRowNum, iDictId, iFind = 0;
   LogWatch *pstLogWatch;

   pcShmAddr = getShmAddr (giConfigKey);
   if( pcShmAddr == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      return;
   }

   pstLogWatch = (LogWatch*)SHM_ReadData( pcShmAddr, LOGWATCH, &iRowNum );
   if( pstLogWatch == NULL )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt( pcShmAddr );
      pcShmAddr = NULL;
      return;
   }

   iDictId == -1;
   for( i = 0; i < iRowNum; i++ )
   {
      /** Tag ���� 1 ��ʾ��� **/
      if( nodeid == pstLogWatch->iNodeId && pstLogWatch->iTag == 1 )
      {
         iRowNum = pstLogWatch->iFldId;
         pstLogWatch++;
         iDictId = pstLogWatch->iDictId;
         iFind = 1;
         break;
      }
      pstLogWatch++;
   }
   if( iFind == 0 )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt( pcShmAddr );
      pcShmAddr = NULL;
      return;
   }

   if( iDictId == -1 )
   {
      if( tcp_wait_send( client, 5000 ) == true )
      {
         tcp_raw_send( client, aczError, strlen(aczError) );
      }
      shmdt( pcShmAddr );
      pcShmAddr = NULL;
      return;
   }

   for( i = 0; i < iRowNum; i++ )
   {
      int iRet;
      memset( aczFields, 0, sizeof(aczFields) );
      /** ����ָ�������ֵ�,ָ��������� **/
      iRet = searchFieldByDictId( pcShmAddr, pstLogWatch->iDictId,
                                  pstLogWatch->iFldId, aczFields );
      if( iRet == 0 )
      {
         if( tcp_wait_send( client, 5000 ) == false )
         {
            shmdt( pcShmAddr );
            pcShmAddr = NULL;
            return;
         }
         tcp_raw_send( client, aczFields, strlen(aczFields) );
      }
      pstLogWatch++;
   }

   shmdt( pcShmAddr );
   pcShmAddr = NULL;

   return;
}
