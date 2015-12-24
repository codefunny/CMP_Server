#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <assert.h>
#include <errno.h>

#include "shm_base.h"
#include "pkg/pkg_base.h"
#include "shm_api.h"

#include "tool_base.h"
#include "log.h"

/*
** ���ܣ��ӹ����ڴ��ж�ȡָ��������
** ���������1 void *epShmAddr �����ڴ��ַ 
**           2 int eiTblSerialNum �����к�
** ���������1 *opiNum ��¼����
** ����ֵ��    �ɹ����ر����ݵ�ַ 
**             ʧ�ܷ��� NULL ����*opiNumΪ -1;
** ˵��������Ϊ��ʱ������ֵ��Ϊ�գ�*opiNum����0
*/
void * SHM_ReadData ( char * epShmAddr, int eiTblSerialNum, int * opiNum )
{
   int iOffset;

   assert( epShmAddr != NULL );
   assert( opiNum != NULL );

   *opiNum = -1;

   if( eiTblSerialNum < 0 ||
       (size_t)eiTblSerialNum > sizeof(aiTableSize)/sizeof(int) )
   {
      return NULL;
   }

   /****  ��̬�ڴ�װ�� 
   memcpy( &iOffset, epShmAddr + sizeof(ShmCa) + sizeof(int)*eiTblSerialNum,
           sizeof(int) );
   memcpy( opiNum, epShmAddr + iOffset + sizeof(ShmCa), sizeof(int) );
   iOffset += sizeof(long) + sizeof(ShmCa);
   ********/
   memcpy( &iOffset, epShmAddr + sizeof(int) * eiTblSerialNum, sizeof(int) );
   memcpy( opiNum, epShmAddr + iOffset, sizeof(int) );
   iOffset += sizeof(int);
   /********/

   return (void*)(epShmAddr+iOffset);
}

/**
  *   ֧�ֶ�̬�����ڴ���������װ�أ����Ӻ��� 
  */ 

/**
  * �������ƣ�Shm_GetCfgShmKey
  * ��    �ܣ�ȡ�õ�ǰ�������ݹ����ڴ�IPCKEY
  * ���������int eiKeyShmKey  Key�����ڴ�IPCKEY
  * ���������time_t *opiCfgShmTm ��ǰ�������ݹ����ڴ�ĸ���ʱ�� 
  * �� �� ֵ�� >=0    �ɹ� ��ǰ�������ݹ����ڴ��IPCKEY
  *            =����  ʧ��
  */
int Shm_GetCfgShmKey (int eiKeyShmKey, time_t *opiCfgShmTm)
{
   int           iCfgShmKey, iKeyShmId;
   int           iLookupTimes;
   void          *pcKeyShmAddr;
   CfgKeyShmHead *pstKeyShmHead;
   CfgKeyShmRec  *pstKeyShmRec;

   iKeyShmId = shmget (eiKeyShmKey, 0, KEYSHMMODE);
   if (iKeyShmId < 0) {
       return (-1);
   }

   pcKeyShmAddr = shmat (iKeyShmId, 0, 0);
   if (pcKeyShmAddr == (void *)(-1)) {
       return (-1);
   }

   pstKeyShmHead = (CfgKeyShmHead *) pcKeyShmAddr;
   if (memcmp (pstKeyShmHead->acMagic, MAGICNUM, MAGICNUM_LEN) != 0) {
       shmdt (pcKeyShmAddr);
       return (-1);
   }

   pstKeyShmRec=(CfgKeyShmRec *)((char *)pcKeyShmAddr+sizeof(CfgKeyShmHead));
  
LOOK_UP:
   iLookupTimes = 0;
   iCfgShmKey = 0;
   if (pstKeyShmRec[0].iCurFlag == 1) {
       iCfgShmKey = pstKeyShmRec[0].iKey;
       if (opiCfgShmTm != NULL) {
           *opiCfgShmTm = pstKeyShmRec[0].iTimeStamp;
       }
   } else if (pstKeyShmRec[1].iCurFlag == 1) {
       iCfgShmKey = pstKeyShmRec[1].iKey;
       if (opiCfgShmTm != NULL) {
           *opiCfgShmTm = pstKeyShmRec[1].iTimeStamp;
       }
   }

   if (iCfgShmKey == 0) {
       usleep (1000);
       iLookupTimes++;
       if (iLookupTimes == 5) {
           shmdt (pcKeyShmAddr);
           return (-1);
       }
       goto LOOK_UP;
   }

   shmdt (pcKeyShmAddr);

   return (iCfgShmKey);
}

/**
  * �������ƣ�Shm_GetCfgShm
  * ��    �ܣ�ȡ�õ�ǰ�������ݹ����ڴ�
  * ���������int iKeyShmKey   Key�����ڴ�IPCKEY
  * ���������int *piCfgShmKey ���ù����ڴ�IPCKEY
  * �� �� ֵ��<>NULL  �ɹ� ��ǰ�������ݹ����ڴ渽�ӵ�ַ
  *             NULL  ʧ��
  */
void * Shm_GetCfgShm(int iKeyShmKey, int *piCfgShmKey)
{
    static int     iCurCfgShmKey = 0;
    static time_t  iCurCfgShmTm  = 0;
    int            iCfgShmKey;
    time_t         iCfgShmTm;
    int            iShmId;
    void           *pcCfgShmAddr = NULL;

    iCfgShmKey = Shm_GetCfgShmKey (iKeyShmKey, &iCfgShmTm);
    if (iCfgShmKey <= 0) 
    {
        fprintf (stderr,
                 "Get config share memory key error, KeyShmKey is %d\n",
                  iKeyShmKey);
        LOG4C ((LOG_FATAL,
                "Get config share memory key error, KeyShmKey is %d\n",
                iKeyShmKey));
        return (NULL);
    }

/*    LOG4C ((LOG_DEBUG, 
            "Configure share memory key is %d", iCfgShmKey));
*/
    iShmId = shmget (iCfgShmKey, 0, CFGSHMMODE);
    if (iShmId == -1) 
    {
        fprintf (stderr, 
                 "Get Cfg share memory error! IPCKEY = %d\n", iCfgShmKey);
        LOG4C ((LOG_FATAL,
                "Get Cfg share memory error! IPCKEY = %d\n", iCfgShmKey));

        return (NULL);
    }

    pcCfgShmAddr = shmat (iShmId, 0, CFGSHMMODE);
    if (pcCfgShmAddr == (void *)(-1)) 
    {
        fprintf (stderr,
                 "attach config memory error\n IPCKEY = %d\n", iCfgShmKey);
        LOG4C ((LOG_FATAL,
                "attach config memory error\n IPCKEY = %d\n", iCfgShmKey));

        return (NULL);
    }

    iCurCfgShmKey = iCfgShmKey;
    iCurCfgShmTm  = iCfgShmTm; 

    *piCfgShmKey = iCurCfgShmKey;

    return pcCfgShmAddr;
}

/**
  * �������ƣ�Shm_FindKeysFromFile
  * ��    �ܣ��������ı��ļ���ȡ���������ݹ����ڴ�IPCKEY
  * ���������int eiKeyShmKey       Key�����ڴ�IPCKEY
  * ���������int *opiCfgShmKey1    ���ù����ڴ�IPCKEY1
  *           int *opiCfgShmKey2    ���ù����ڴ�IPCKEY2
  * �� �� ֵ��>=0  �ɹ�
  *            <0  ʧ�� 
  */
static int Shm_FindKeysFromFile (int eiKeyShmKey, int *opiCfgShmKey1, int *opiCfgShmKey2)
{
    int           iTmp1, iTmp2, iTmp3;
    int           iFindFlag;
    char          aczFile[200];
    char          aczLine[100];
    FILE          *fp;

    memset (aczFile, 0, sizeof (aczFile));
    sprintf (aczFile, "%s/etc/%s", getenv ("MYROOT"), CFGKEYFILE);

    fp = fopen (aczFile, "r");
    if (fp == NULL) {
        fprintf (stderr, "Open Config ShmKey File %s Error: %s\n",
                 aczFile, strerror (errno));
        return (-1);
    }

    iFindFlag = 0;
    memset (aczLine, 0, sizeof (aczLine));
    while (fgets (aczLine, sizeof (aczLine) - 2, fp) != NULL) {
        if (aczLine[0] == '#') {
            continue;
        }
        iTmp1 = iTmp2 = iTmp3 = 0;
        sscanf (aczLine, "%d %d %d", &iTmp1, &iTmp2, &iTmp3);
        if (iTmp1 == eiKeyShmKey) {
            iFindFlag = 1;
            *opiCfgShmKey1 = iTmp2;
            *opiCfgShmKey2 = iTmp3;
            break;
        }
    }
    fclose (fp);

    if (iFindFlag == 0) {
        fprintf (stderr, "Not Found Record In File %s, Key Is %d\n",
                 aczFile, eiKeyShmKey);
        return (-1);
    }

    return (0);
}

/**
  * �������ƣ�Shm_CreatKeyShm
  * ��    �ܣ�����KEY�����ڴ� 
  * ���������int eiKeyShmKey       Key�����ڴ�IPCKEY
  * ���������void **oppcKeyShmAddr KEY�����ڴ渽�ӵ�ַ
  * �� �� ֵ��>=0  �ɹ� KEY�����ڴ��ID
  *            <0  ʧ�� 
  */
static int Shm_CreatKeyShm (int eiKeyShmKey, void **oppcKeyShmAddr)
{
    int           iKeyShmId, iSize;
    int           iRet;
    void          *pcKeyShmAddr;
    CfgKeyShmHead stKeyShmHead, *pstKeyShmHead;
    CfgKeyShmRec  astKeyShmRec[2], *pstKeyShmRec;

    memset (&stKeyShmHead, 0, sizeof (stKeyShmHead));
    memset (&astKeyShmRec, 0, sizeof (astKeyShmRec));

    memcpy (stKeyShmHead.acMagic, MAGICNUM, MAGICNUM_LEN);
    stKeyShmHead.iKeyNum = 2;
    
    iRet = Shm_FindKeysFromFile (eiKeyShmKey, &astKeyShmRec[0].iKey, 
                             &astKeyShmRec[1].iKey);
    if (iRet != 0) {
        return (-1);
    }
    
    printf ("KeyShmKey:\t%d\nConfigShmKey1:\t%d\nConfigShmKey2:\t%d\n",
            eiKeyShmKey, astKeyShmRec[0].iKey, astKeyShmRec[1].iKey);

    iSize = sizeof (stKeyShmHead) + sizeof (astKeyShmRec); 
    iKeyShmId = shmget (eiKeyShmKey, iSize, KEYSHMMODE | IPC_CREAT | IPC_EXCL);
    if (iKeyShmId < 0) {
        fprintf(stderr,"Create Config Key Share Memory Error:%s,Key: %d\n",
                strerror (errno), eiKeyShmKey);
        return (-1);
    }
    
    pcKeyShmAddr = shmat (iKeyShmId, 0, 0);
    if (pcKeyShmAddr == (void *)(-1)) {
        fprintf(stderr,"Attach Config Key Share Memory Error:%s,Key: %d\n",
                strerror (errno), eiKeyShmKey);
        shmctl (iKeyShmId, IPC_RMID, NULL);
        return (-1);
    }    

    memcpy (pcKeyShmAddr, &stKeyShmHead, sizeof (stKeyShmHead));
    memcpy ((char *)(pcKeyShmAddr) + sizeof (stKeyShmHead), 
            astKeyShmRec, sizeof (astKeyShmRec));

    *oppcKeyShmAddr = pcKeyShmAddr;
    return (iKeyShmId);
}

/**
  * �������ƣ�Shm_GetKeyShm
  * ��    �ܣ�ȡ��KEY�����ڴ��ַ
  * ���������int iKeyShmKey   Key�����ڴ�IPCKEY
  * �� �� ֵ��<> NULL    �ɹ� ��ǰKEY�����ڴ�ĸ��ӵ�ַ 
  *            = NULL    ʧ�� 
  */
void *Shm_GetKeyShm (int iKeyShmKey)
{
    int     iKeyShmId, iNeedCreat = 0;
    void    *pcKeyShmAddr;
    CfgKeyShmHead *pstKeyShmHead;

    iKeyShmId = shmget (iKeyShmKey, 0, KEYSHMMODE);
    if (iKeyShmId < 0) {
        iNeedCreat = 1;
    } else {
        pcKeyShmAddr = shmat (iKeyShmId, 0, 0);
        if (pcKeyShmAddr == (void *)(-1)) {
            fprintf(stderr,"Attach Config Key Share Memory Error:%s,Key: %d\n",
                     strerror (errno), iKeyShmKey);
            return (NULL);           
        }    

        pstKeyShmHead = (CfgKeyShmHead *)pcKeyShmAddr;

        if (memcmp (pstKeyShmHead->acMagic, MAGICNUM, MAGICNUM_LEN) != 0) {
            shmdt (pcKeyShmAddr);
            fprintf (stderr, "Config Key Share Memory Already Exists, "
                     "But It Seem Not Belong Config Memory,Check It,Key %d!\n",
                     iKeyShmKey);
            return (NULL);
        } else {
            iNeedCreat = 0; 
        }
    }

    if (iNeedCreat == 1) {
        iKeyShmId = Shm_CreatKeyShm (iKeyShmKey, &pcKeyShmAddr);
        if (iKeyShmId < 0) {
            return (NULL);
        }
    }

    return pcKeyShmAddr;
}

/**
  * �������ƣ�Shm_GetIdleCfgShmKey
  * ��    �ܣ�ȡ�ÿ������ù����ڴ��KEY����¼����
  * ���������void *pcKeyShmAddr KEY�����ڴ��ַ
  * ���������int  *piCfgShmIdx  ���ù����ڴ���KEY�����ڴ��е�����
  * �� �� ֵ��>=0    �ɹ�        ���ù����ڴ�IPCKEY
  *            ����  ʧ�� 
  */
int Shm_GetIdleCfgShmKey (void *pcKeyShmAddr, int *piCfgShmIdx)
{
    int  iCfgShmKey, iCfgShmIdx;

    CfgKeyShmHead *pstKeyShmHead;
    CfgKeyShmRec  *pstKeyShmRec;

    pstKeyShmRec=(CfgKeyShmRec *)((char *)pcKeyShmAddr+sizeof(CfgKeyShmHead));
    if (pstKeyShmRec[0].iCurFlag == 0) {
        iCfgShmIdx = 0;
    } else if (pstKeyShmRec[1].iCurFlag == 0) {
        iCfgShmIdx = 1;
    } else {
        fprintf (stderr, "The Tow Config Share Memory's curflag are set all,"
                 "Check It.");
        return (-1);
    }

    iCfgShmKey = pstKeyShmRec[iCfgShmIdx].iKey;
    *piCfgShmIdx = iCfgShmIdx;

    return iCfgShmKey;
}

/**
  * �������ƣ�Shm_SetCfgShmCurFlag
  * ��    �ܣ����õ�ǰ���ù����ڴ��־
  * ���������void *pcKeyShmAddr KEY�����ڴ��ַ
  *           int  iCfgShmIdx    �������ù����ڴ���KEY�����ڴ��е�����
  */
void Shm_SetCfgShmCurFlag (void *pcKeyShmAddr, int iCfgShmIdx)
{
    int  iCfgShmKey;

    CfgKeyShmHead *pstKeyShmHead;
    CfgKeyShmRec  *pstKeyShmRec;

    pstKeyShmRec=(CfgKeyShmRec *)((char *)pcKeyShmAddr+sizeof(CfgKeyShmHead));
    if (iCfgShmIdx == 0) {
        pstKeyShmRec[1].iCurFlag = 0;
    } else {
        pstKeyShmRec[0].iCurFlag = 0;
    }

    pstKeyShmRec[iCfgShmIdx].iCurFlag = 1;
    pstKeyShmRec[iCfgShmIdx].iTimeStamp = time (NULL);
    return;
}


/*
** end of file shm_base.c
*/

