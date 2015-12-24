#include "qclean/reqcleanque.h"
#include "log.h"

int nReqCleanQueMsg (int iQid, long mType)
{
	 int rc;
	 double currentTime;	 
	 
	 /* 清理队列用的buf */
   QCLEANMSGBUF  stQBuf;
	 
	 memset(&stQBuf, 0x00, sizeof stQBuf);
	 
	 currentTime = GetCurrentMillTime();
	 
	 stQBuf.mType = mType;
	 sprintf(stQBuf.aczInTime, "%013.0lf", currentTime);

   rc = msgsnd( iQid, &stQBuf, TIME_LENGTH, IPC_NOWAIT ); /*非阻塞式*/
   if( rc < 0 )
   {
   	  if( errno == EAGAIN )
   	  	return 0;
   	  	
      LOG4C(( LOG_DEBUG, "送入清除堵塞队列失败[%d]", rc ));
   }
   
   return rc;
}

