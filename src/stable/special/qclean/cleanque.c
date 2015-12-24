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


#define MSGMOD (0660 | IPC_CREAT)      /* message mode */

int main(int argc, char *argv[])
{
	
	
    int msgid = 0;
    int iRet = 0;
    char buf[20000];
    int iTime; 
    int i; 
 
    if(argc<3)
    {
        fprintf(stderr,"\nUseage:cleanque QueKey Number\n",strerror(errno));
        return 0;
    }

    
    msgid = msgget(atoi(argv[1]),MSGMOD);
    if(msgid == -1)
    {
        if(errno == ENOENT)
        {
            return 0;
        }
        
        fprintf(stderr,"msgget![%s]\n",strerror(errno));
        goto Exit;
    }

    iTime = atol(argv[2]);

    for(i=0;i<iTime;i++)
    {
        iRet = msgrcv(msgid,buf,sizeof(buf),0,IPC_NOWAIT);
        if(iRet < 0)
        {
            fprintf(stderr,"msgrcv![%s]\n",strerror(errno));
               goto Exit;
        }
    }

    return 0; 
Exit:
    return -1;
}
