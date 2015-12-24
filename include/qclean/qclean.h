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

/****************宏定义*********************************************/

#define MSGMOD (0660 | IPC_CREAT)      /* message mode */
#define JAM_LOG_FILE "quejam.log"
#define TIME_LENGTH 13
#define MAXBUFSIZE 15000

/****************结构定义*******************************************/
/* 清理队列用的buf */
typedef struct _qcleanmsgbuf
{
        long    mType   ;           /* 队列type */
        char    aczInTime[TIME_LENGTH+1];      /* 录入时间 */
} QCLEANMSGBUF ;
/* 读响应信息用的buf */
typedef struct _jammsgbuf
{
        long    mType   ;           /* 队列type */
        char    aczText[MAXBUFSIZE];     /* 响应消息队列内记录信息 */
} JAMMSGBUF ;

#ifdef __cplusplus
extern "C"
{
#endif 

/* 取得当前毫秒时间 */
double GetCurrentMillTime();
/* sleep微秒时间 */
int Myusleep(unsigned int usec);
#ifdef __cplusplus
}
#endif 

#endif
