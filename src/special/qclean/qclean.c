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
    
    /* ��������õ�buf */
    QCLEANMSGBUF  stQBuf;

    /* ����Ӧ��Ϣ�õ�buf */
    JAMMSGBUF stJBuf;
    
    memset(aczLogFilePath, 0x00, sizeof aczLogFilePath);
    memset(&stQBuf, 0x00, sizeof stQBuf);
    memset(&stJBuf, 0x00, sizeof stJBuf);

    i=0;
    if( argc == 6 ) 
    {
        iQueKeyOfReadFrom = atoi( argv[++i] );
        iQueKeyToClean = atoi( argv[++i] );
        iQueKeyToClean2 = atoi( argv[++i] ); /*���û�еĻ�����-1��ʾ*/
        iTimeToCheck = 1000 * atoi( argv[++i] ); /*60�� ����60000 ��λ�Ǻ���*/
        strcpy(aczLogFilePath, argv[++i]);
        
        pczExecFileName=aczLogFilePath;
        
        TOOL_Trace( 2, JAM_LOG_FILE,
             "��%4d�� in %10s:���������������:������Ϣ�ö���key[%d]�����������key[%d]�����������key2[%d], ���ʱ����[%d]����.",
             __LINE__, __FILE__ ,iQueKeyOfReadFrom,iQueKeyToClean,iQueKeyToClean2, iTimeToCheck);
    } 
    else
    {
        fprintf(stderr,"\nUsage:%s [������Ϣ�ö���key�����������key, ���������key2�����ʱ����(����) ��־���·��].\n\n", argv[0]);
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
             "��%4d�� in %10s:fork error:[%d][%s]",
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
             "��%4d�� in %10s:can't change process group!:[%d][%s]",
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
             "��%4d�� in %10s:fork error:[%d][%s]",
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
    	fprintf (stderr, "ȡ�������ʱ����!:[%d][%s]",errno,strerror (errno));
    	
    	TOOL_Trace( 2, JAM_LOG_FILE,
             "��%4d�� in %10s:ȡ�������ʱ����!:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
        return -1;
    }
    
    msgidToClean = msgget(iQueKeyToClean,MSGMOD);
    if(msgidToClean == -1)
    {
    	fprintf (stderr, "ȡ��Ӧ����ʱ����!:[%d][%s]",errno,strerror (errno));
    	TOOL_Trace( 2, JAM_LOG_FILE,
             "��%4d�� in %10s:ȡ��Ӧ����ʱ����!:[%d][%s]",
             __LINE__, __FILE__ ,errno,strerror (errno));
      return -1;
    }    
    
    if( iQueKeyToClean2 >=0 )
    {
        msgidToClean2 = msgget(iQueKeyToClean2,MSGMOD);
        if(msgidToClean2 == -1)
        {
        	fprintf (stderr, "ȡ��Ӧ����2ʱ����!:[%d][%s]",errno,strerror (errno));
        	TOOL_Trace( 2, JAM_LOG_FILE,
                 "��%4d�� in %10s:ȡ��Ӧ����2ʱ����!:[%d][%s]",
                 __LINE__, __FILE__ ,errno,strerror (errno));
            return -1;
        }    
    }

    /* ����д��� */
    while(1)
    {
        memset(&stQBuf, 0x00, sizeof stQBuf);
        memset(&stJBuf, 0x00, sizeof stJBuf);
        
        iRet = msgrcv(msgidOfReadFrom,&stQBuf,sizeof(stQBuf.aczInTime),0,0);/* ����ʽ�Ķ� */
        if(iRet < 0)
        {
            /* ���ȡ���г��ִ�����ô�˳����� */
            TOOL_Trace( 2, JAM_LOG_FILE,
                 "��%4d�� in %10s:ȡ�������ʱ���������Զ��˳�!:[%d][%s]",
                 __LINE__, __FILE__ ,errno,strerror (errno));
            return -1;
        }
        
        TimeNow = GetCurrentMillTime();
        
        iTempTime = TimeNow - atof(stQBuf.aczInTime); /*��ʱ2�� ��iTempTimeΪ2000*/

        if(iTempTime < iTimeToCheck)
        {
            Myusleep((iTimeToCheck-iTempTime)*1000); /* ��΢��Ϊ��λsleep: 1000*1000��ʾ1��  */
        }

        iRet = msgrcv(msgidToClean,&stJBuf,sizeof(stJBuf.aczText),stQBuf.mType,IPC_NOWAIT);/* �Ƕ����� */
        if(iRet >= 0)
        {
        	  TimeNow = GetCurrentMillTime();
        	  for(i=0;i<iRet;i++)
        	  {
        	      if(stJBuf.aczText[i]=='\0')
        	          stJBuf.aczText[i]='^';
        	  }
        	  stJBuf.aczText[iRet] = '\0';
        	  /* ���ֶ������е���Ϣ�󣬽����¼����־�ļ��� */
        	  TOOL_Trace( 2, JAM_LOG_FILE,
                 "��%4d�� in %10s:\n�ڶ���[%d]�ڷ��ֶ�����Ϣ(��Ϣ�����е�'\\0'����ӡΪ'^'):\nmtype=[%ld],д�����ʱ��[%s],������ʱ��[%.0lf]\nmtext=[%s]",
                 __LINE__, __FILE__ ,iQueKeyToClean,stJBuf.mType,stQBuf.aczInTime,TimeNow,stJBuf.aczText);
        }

        if( iQueKeyToClean2 >=0 ) /*�����Ҫ�������Ϣ����2*/
        {
            memset(&stJBuf, 0x00, sizeof stJBuf);
            iRet = msgrcv(msgidToClean2,&stJBuf,sizeof(stJBuf.aczText),stQBuf.mType,IPC_NOWAIT);/* �Ƕ����� */
            if(iRet >= 0)
            {
            	  TimeNow = GetCurrentMillTime();
            	  for(i=0;i<iRet;i++)
            	  {
            	      if(stJBuf.aczText[i]=='\0')
            	          stJBuf.aczText[i]='^';
            	  }
            	  stJBuf.aczText[iRet] = '\0';
            	  /* ���ֶ������е���Ϣ�󣬽����¼����־�ļ��� */
            	  TOOL_Trace( 2, JAM_LOG_FILE,
                     "��%4d�� in %10s:\n�ڶ���[%d]�ڷ��ֶ�����Ϣ(��Ϣ�����е�'\\0'����ӡΪ'^'):\nmtype=[%ld],д�����ʱ��[%s],������ʱ��[%.0lf]\nmtext=[%s]",
                     __LINE__, __FILE__ ,iQueKeyToClean2,stJBuf.mType,stQBuf.aczInTime,TimeNow,stJBuf.aczText);
            }
        }
    }
}

