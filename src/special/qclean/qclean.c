#include "qclean/qclean.h"

extern char *pczExecFileName;

int main(int argc, char *argv[])
{
    int i;
    int iRet = 0;
    int childpid;
    
    int msgidOfReadFrom = 0;
    int msgidToClean = 0;
    int msgidToClean2 = 0;

    int iQueKeyOfReadFrom;
    int iQueKeyToClean;
    int iQueKeyToClean2;
    unsigned int iTimeToCheck;
    unsigned int iTempTime;
    
    char aczLogFilePath[50];
    
    double TimeNow;
    
    /* 清理队列用的buf */
    QCLEANMSGBUF  stQBuf;

    /* 读响应信息用的buf */
    JAMMSGBUF stJBuf;
    
    memset(aczLogFilePath, 0x00, sizeof aczLogFilePath);
    memset(&stQBuf, 0x00, sizeof stQBuf);
    memset(&stJBuf, 0x00, sizeof stJBuf);

    i=0;
    if( argc == 6 ) 
    {
        iQueKeyOfReadFrom = atoi( argv[++i] );
        iQueKeyToClean = atoi( argv[++i] );
        iQueKeyToClean2 = atoi( argv[++i] ); /*如果没有的话就用-1表示*/
        iTimeToCheck = 1000 * atoi( argv[++i] ); /*60秒 就是60000 单位是毫秒*/
        strcpy(aczLogFilePath, argv[++i]);
        
        pczExecFileName=aczLogFilePath;
        
        TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:队列清理进程启动:清理信息用队列key[%d]，被清理队列key[%d]，被清理队列key2[%d], 检查时间间隔[%d]豪秒.",
             __LINE__, __FILE__ ,iQueKeyOfReadFrom,iQueKeyToClean,iQueKeyToClean2, iTimeToCheck);
    } 
    else
    {
        fprintf(stderr,"\nUsage:%s [清理信息用队列key，被清理队列key, 被清理队列key2，检查时间间隔(豪秒) 日志存放路径].\n\n", argv[0]);
        return -1;
    }

    signal (SIGPIPE, SIG_IGN);
    signal (SIGHUP,  SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
    signal (SIGINT,  SIG_IGN);
    signal (SIGQUIT, SIG_IGN);

    childpid = fork();

    if( childpid<0 )
    {
    	  fprintf (stderr, "fork error:[%d][%s]",errno,strerror (errno));
    	  
        TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:fork error:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
             
        return -1;
    }
    else if( childpid>0 )
    {
        return 0;
    }

    if( setpgrp()==-1 )
    {
    	  fprintf (stderr, "can't change process group!:[%d][%s]",errno,strerror (errno));
    	  
        TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:can't change process group!:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
        return -1;
    }

    chdir("/");
    umask(0);

    childpid = fork();
    if( childpid<0 )
    {
    	  fprintf (stderr, "fork error:[%d][%s]",errno,strerror (errno));
    	  
        TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:fork error:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
        return -1;
    }

    if( childpid>0 )
    {
        return 0;
    }
     
    msgidOfReadFrom = msgget(iQueKeyOfReadFrom,MSGMOD);
    if(msgidOfReadFrom == -1)
    {
    	fprintf (stderr, "取清理队列时出错!:[%d][%s]",errno,strerror (errno));
    	
    	TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:取清理队列时出错!:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
        return -1;
    }
    
    msgidToClean = msgget(iQueKeyToClean,MSGMOD);
    if(msgidToClean == -1)
    {
    	fprintf (stderr, "取响应队列时出错!:[%d][%s]",errno,strerror (errno));
    	TOOL_Trace( 2, JAM_LOG_FILE,
             "第%4d行 in %10s:取响应队列时出错!:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
      return -1;
    }    
    
    if( iQueKeyToClean2 >=0 )
    {
        msgidToClean2 = msgget(iQueKeyToClean2,MSGMOD);
        if(msgidToClean2 == -1)
        {
        	fprintf (stderr, "取响应队列2时出错!:[%d][%s]",errno,strerror (errno));
        	TOOL_Trace( 2, JAM_LOG_FILE,
                 "第%4d行 in %10s:取响应队列2时出错!:[%d][%s]",
                 __LINE__, __FILE__ ,errno,strerror (errno));
            return -1;
        }    
    }

    /* 清队列处理 */
    while(1)
    {
        memset(&stQBuf, 0x00, sizeof stQBuf);
        memset(&stJBuf, 0x00, sizeof stJBuf);
        
        iRet = msgrcv(msgidOfReadFrom,&stQBuf,sizeof(stQBuf.aczInTime),0,0);/* 堵塞式的读 */
        if(iRet < 0)
        {
            /* 如果取队列出现错误，那么退出进程 */
            TOOL_Trace( 2, JAM_LOG_FILE,
                 "第%4d行 in %10s:取清理队列时出错，进程自动退出!:[%d][%s]",
                 __LINE__, __FILE__ ,errno,strerror (errno));
            return -1;
        }
        
        TimeNow = GetCurrentMillTime();
        
        iTempTime = TimeNow - atof(stQBuf.aczInTime); /*延时2秒 则iTempTime为2000*/

        if(iTempTime < iTimeToCheck)
        {
            Myusleep((iTimeToCheck-iTempTime)*1000); /* 以微秒为单位sleep: 1000*1000表示1秒  */
        }

        iRet = msgrcv(msgidToClean,&stJBuf,sizeof(stJBuf.aczText),stQBuf.mType,IPC_NOWAIT);/* 非堵塞读 */
        if(iRet >= 0)
        {
        	  TimeNow = GetCurrentMillTime();
        	  for(i=0;i<iRet;i++)
        	  {
        	      if(stJBuf.aczText[i]=='\0')
        	          stJBuf.aczText[i]='^';
        	  }
        	  stJBuf.aczText[iRet] = '\0';
        	  /* 发现堵塞队列的信息后，将其记录到日志文件内 */
        	  TOOL_Trace( 2, JAM_LOG_FILE,
                 "第%4d行 in %10s:\n在队列[%d]内发现堵塞信息(信息内容中的'\\0'被打印为'^'):\nmtype=[%ld],写入队列时间[%s],被读出时间[%.0lf]\nmtext=[%s]",
                 __LINE__, __FILE__ ,iQueKeyToClean,stJBuf.mType,stQBuf.aczInTime,TimeNow,stJBuf.aczText);
        }

        if( iQueKeyToClean2 >=0 ) /*如果有要求清除消息队列2*/
        {
            memset(&stJBuf, 0x00, sizeof stJBuf);
            iRet = msgrcv(msgidToClean2,&stJBuf,sizeof(stJBuf.aczText),stQBuf.mType,IPC_NOWAIT);/* 非堵塞读 */
            if(iRet >= 0)
            {
            	  TimeNow = GetCurrentMillTime();
            	  for(i=0;i<iRet;i++)
            	  {
            	      if(stJBuf.aczText[i]=='\0')
            	          stJBuf.aczText[i]='^';
            	  }
            	  stJBuf.aczText[iRet] = '\0';
            	  /* 发现堵塞队列的信息后，将其记录到日志文件内 */
            	  TOOL_Trace( 2, JAM_LOG_FILE,
                     "第%4d行 in %10s:\n在队列[%d]内发现堵塞信息(信息内容中的'\\0'被打印为'^'):\nmtype=[%ld],写入队列时间[%s],被读出时间[%.0lf]\nmtext=[%s]",
                     __LINE__, __FILE__ ,iQueKeyToClean2,stJBuf.mType,stQBuf.aczInTime,TimeNow,stJBuf.aczText);
            }
        }
    }
}

