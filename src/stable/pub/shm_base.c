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
** 功能：从共享内存中读取指定表数据
** 输入参数：1 void *epShmAddr 共享内存地址 
**           2 int eiTblSerialNum 表序列号
** 输出参数：1 *opiNum 记录条数
** 返回值：    成功返回表数据地址 
**             失败返回 NULL 并置*opiNum为 -1;
** 说明：当表为空时，返回值不为空，*opiNum等于0
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

   /****  动态内存装载 
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
  *   支持动态共享内存配置数据装载，增加函数 
  */ 

/**
  * 函数名称：Shm_GetCfgShmKey
  * 功    能：取得当前配置数据共享内存IPCKEY
  * 输入参数：int eiKeyShmKey  Key共享内存IPCKEY
  * 输出参数：time_t *opiCfgShmTm 当前配置数据共享内存的更新时间 
  * 返 回 值： >=0    成功 当前配置数据共享内存的IPCKEY
  *            =其他  失败
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
  * 函数名称：Shm_GetCfgShm
  * 功    能：取得当前配置数据共享内存
  * 输入参数：int iKeyShmKey   Key共享内存IPCKEY
  * 输出参数：int *piCfgShmKey 配置共享内存IPCKEY
  * 返 回 值：<>NULL  成功 当前配置数据共享内存附接地址
  *             NULL  失败
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
  * 函数名称：Shm_FindKeysFromFile
  * 功    能：从配置文本文件里取得配置数据共享内存IPCKEY
  * 输入参数：int eiKeyShmKey       Key共享内存IPCKEY
  * 输出参数：int *opiCfgShmKey1    配置共享内存IPCKEY1
  *           int *opiCfgShmKey2    配置共享内存IPCKEY2
  * 返 回 值：>=0  成功
  *            <0  失败 
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
  * 函数名称：Shm_CreatKeyShm
  * 功    能：创建KEY共享内存 
  * 输入参数：int eiKeyShmKey       Key共享内存IPCKEY
  * 输出参数：void **oppcKeyShmAddr KEY共享内存附接地址
  * 返 回 值：>=0  成功 KEY共享内存的ID
  *            <0  失败 
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
  * 函数名称：Shm_GetKeyShm
  * 功    能：取得KEY共享内存地址
  * 输入参数：int iKeyShmKey   Key共享内存IPCKEY
  * 返 回 值：<> NULL    成功 当前KEY共享内存的附接地址 
  *            = NULL    失败 
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
  * 函数名称：Shm_GetIdleCfgShmKey
  * 功    能：取得空闲配置共享内存的KEY及记录索引
  * 输入参数：void *pcKeyShmAddr KEY共享内存地址
  * 输出参数：int  *piCfgShmIdx  配置共享内存在KEY共享内存中的索引
  * 返 回 值：>=0    成功        配置共享内存IPCKEY
  *            其他  失败 
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
  * 函数名称：Shm_SetCfgShmCurFlag
  * 功    能：设置当前配置共享内存标志
  * 输入参数：void *pcKeyShmAddr KEY共享内存地址
  *           int  iCfgShmIdx    最新配置共享内存在KEY共享内存中的索引
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

