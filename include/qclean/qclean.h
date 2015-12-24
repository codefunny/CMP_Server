#ifndef  _QCLEAN_H_
#define  _QCLEAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <stddef.h>
#include <poll.h>
#include "tool_base.h"

/****************�궨��*********************************************/

#define MSGMOD (0660 | IPC_CREAT)      /* message mode */
#define JAM_LOG_FILE "quejam.log"
#define TIME_LENGTH 13
#define MAXBUFSIZE 15000

/****************�ṹ����*******************************************/
/* ��������õ�buf */
typedef struct _qcleanmsgbuf
{
        long    mType   ;           /* ����type */
        char    aczInTime[TIME_LENGTH+1];      /* ¼��ʱ�� */
} QCLEANMSGBUF ;
/* ����Ӧ��Ϣ�õ�buf */
typedef struct _jammsgbuf
{
        long    mType   ;           /* ����type */
        char    aczText[MAXBUFSIZE];     /* ��Ӧ��Ϣ�����ڼ�¼��Ϣ */
} JAMMSGBUF ;

#ifdef __cplusplus
extern "C"
{
#endif 

/* ȡ�õ�ǰ����ʱ�� */
double GetCurrentMillTime();
/* sleep΢��ʱ�� */
int Myusleep(unsigned int usec);
#ifdef __cplusplus
}
#endif 

#endif
