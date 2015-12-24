#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>

#include "shm_base.h"

extern char cDelimiter;

main (int argc, char *argv[])
{
    int           iKeyShmKey;
    int           iCfgShmKey, iCfgShmIdx;
    int           iRet;
    void          *pcKeyShmAddr = NULL;

    if (argc < 2) {
        fprintf (stderr, "Usage: %s CfgKeyShmKey  [Delimiter]\n", argv[0]);
        exit (-1);
    }

    iKeyShmKey = atoi (argv[1]);
    if (argc > 2)
    {
        cDelimiter = argv[2][0];
    }

    pcKeyShmAddr = Shm_GetKeyShm (iKeyShmKey);
    if (pcKeyShmAddr == NULL)
    {
        fprintf (stderr, "Get Key Share Memory Error! Key:%d\n", iKeyShmKey); 
        exit (-1);
    }

    iCfgShmKey = Shm_GetIdleCfgShmKey (pcKeyShmAddr, &iCfgShmIdx);
    if (iCfgShmKey < 0)
    {
        shmdt (pcKeyShmAddr);
        fprintf (stderr, "Get Idle Cfg Share Memory Error! Key:%d\n", iKeyShmKey); 
        exit (-1);
    }

    printf ("The New Config Share Memory Key is %d, Idx %d\n", iCfgShmKey, iCfgShmIdx);

    iRet = CfgShmLoad (iCfgShmKey);
    if (iRet < 0) {
        shmdt (pcKeyShmAddr);
        fprintf (stderr, "Load Config Share Memory error!\n");
        exit (-1);
    }
  
    Shm_SetCfgShmCurFlag (pcKeyShmAddr, iCfgShmIdx);

    shmdt (pcKeyShmAddr);
}

