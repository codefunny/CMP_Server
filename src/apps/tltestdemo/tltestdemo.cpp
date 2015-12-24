#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "log.h"
#include "qclean/reqcleanque.h"

struct MsgBuf {
    long lType;
    char aczBuf[4096];
};



                                 
int SYS_Zdtj( const char *epczTxCode,  int iQueId, long lMsgType, int iCleanQueId)
{
  
  int  iSendLen;
  int nRet;

  struct MsgBuf stMsgBuf;  
  memset( stMsgBuf.aczBuf, 0, sizeof(stMsgBuf.aczBuf) );   
  stMsgBuf.lType = getpid ();   

   switch( atoi(epczTxCode) )
  {
    case 1:
        /*��ѯ*/
       //  iSendLen=412;   
       // memcpy(stMsgBuf.aczBuf, "\x30\x34\x30\x38\x2e\x01\x30\x34\x30\x38\x30\x30\x30\x31\x30\x30\x30\x30\x20\x20\x20\x30\x36\x30\x30\x30\x30\x30\x30\x30\x32\x20\x00\x00\x00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x32\x30\x30\xe2\x3a\x44\xc1\xb8\xc0\x98\x10\x00\x00\x00\x00\x10\x00\x00\x01\x31\x39\x36\x32\x32\x38\x34\x38\x30\x30\x33\x30\x31\x36\x31\x38\x36\x36\x39\x31\x35\x33\x30\x39\x30\x30\x30\x31\x32\x30\x32\x31\x32\x34\x30\x32\x32\x30\x39\x37\x32\x37\x31\x31\x32\x34\x30\x32\x32\x31\x32\x30\x32\x31\x30\x31\x39\x35\x33\x31\x31\x30\x32\x31\x30\x30\x30\x36\x31\x30\x31\x34\x32\x39\x30\x30\x30\x30\x30\x32\x30\x38\x30\x30\x30\x39\x30\x30\x30\x30\x33\x37\x36\x32\x32\x38\x34\x38\x30\x30\x33\x30\x31\x36\x31\x38\x36\x36\x39\x31\x35\x3d\x30\x30\x30\x30\x31\x32\x30\x30\x37\x31\x33\x39\x33\x30\x30\x30\x30\x31\x30\x34\x39\x39\x36\x32\x32\x38\x34\x38\x30\x30\x33\x30\x31\x36\x31\x38\x36\x36\x39\x31\x35\x3d\x31\x35\x36\x31\x35\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x31\x34\x31\x34\x31\x34\x30\x30\x30\x30\x3d\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x30\x37\x30\x38\x32\x38\x34\x30\x30\x30\x30\x30\x30\x30\x30\x39\x33\x33\x36\x31\x32\x30\x39\x37\x32\x37\x31\x31\x30\x30\x30\x30\x30\x34\x37\x39\x39\x39\x32\x39\x30\x30\x35\x33\x31\x31\x30\x32\x32\x32\x31\x35\x36\x3a\x31\x96\xda\x5e\x5b\x43\x4a\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x31\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x35\x34\x35\x30\x41\x32\x45\x44", iSendLen);
      //  iSendLen=325;   
     //    memcpy(stMsgBuf.aczBuf, "\x30\x33\x32\x31\x2e\x01\x30\x33\x32\x31\x30\x32\x38\x37\x31\x30\x30\x30\x31\x31\x20\x30\x32\x38\x37\x31\x30\x30\x30\x31\x31\x20\x00\x00\x00\x00\x41\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x32\x30\x30\xe2\x3c\x04\xc1\xa8\xe0\x98\x10\x00\x00\x00\x00\x00\x00\x00\x01\x31\x39\x38\x36\x36\x31\x30\x38\x36\x38\x38\x35\x30\x30\x30\x31\x39\x32\x33\x39\x30\x33\x30\x30\x30\x30\x30\x31\x32\x31\x31\x31\x39\x35\x32\x30\x31\x39\x38\x37\x31\x33\x37\x31\x39\x35\x32\x30\x31\x31\x32\x31\x31\x30\x32\x31\x20\x30\x32\x31\x30\x30\x30\x36\x30\x38\x39\x39\x39\x39\x30\x30\x30\x30\x31\x30\x30\x36\x30\x30\x30\x30\x30\x30\x30\x36\x33\x34\x38\x36\x36\x31\x30\x38\x36\x38\x38\x35\x30\x30\x30\x31\x39\x32\x33\x39\x30\x3d\x31\x38\x30\x38\x35\x30\x30\x31\x35\x39\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x37\x35\x30\x36\x31\x30\x32\x37\x30\x34\x30\x32\x35\x31\x30\x32\x32\x37\x30\x30\x35\x34\x31\x31\x30\x35\x31\x34\x45\x50\x41\x59\xbd\xbb\xd2\xd7\xd7\xaa\xcd\xa8\xc1\xaa\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x31\x35\x36\xdb\xf5\x76\x7b\x24\x19\x7a\x6b\x32\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x46\x44\x43\x31\x33\x37\x31\x39", iSendLen);
        iSendLen=410;  
    //    memcpy(stMsgBuf.aczBuf, "\x30\x34\x30\x36\x2e\x01\x30\x34\x30\x36\x30\x30\x30\x31\x30\x30\x30\x30\x20\x20\x20\x30\x36\x30\x30\x30\x30\x30\x30\x30\x32\x20\x00\x00\x00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x32\x30\x30\xe2\x3a\x44\xc1\xb8\xc0\x98\x10\x00\x00\x00\x00\x10\x00\x00\x01\x31\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x33\x30\x39\x30\x30\x30\x30\x32\x32\x37\x31\x35\x31\x31\x31\x31\x30\x31\x39\x30\x37\x33\x31\x35\x31\x31\x31\x31\x30\x32\x32\x37\x30\x31\x31\x30\x34\x38\x31\x34\x30\x32\x31\x30\x30\x30\x36\x30\x38\x39\x39\x39\x39\x30\x30\x30\x30\x30\x38\x30\x30\x30\x39\x30\x30\x30\x30\x33\x37\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x3d\x32\x30\x30\x31\x38\x39\x39\x34\x32\x31\x36\x35\x30\x30\x38\x34\x32\x31\x30\x34\x39\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x3d\x31\x35\x36\x31\x35\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x31\x30\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x35\x38\x31\x35\x30\x31\x39\x30\x37\x33\x30\x30\x30\x30\x30\x39\x33\x30\x39\x39\x39\x36\x34\x31\x30\x34\x38\x31\x34\x30\x30\x30\x31\x31\x35\x36\xe4\x0e\xb0\x5b\x48\x00\xde\x61\x32\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x31\x31\x30\x32\x32\x39\x30\x30\x30\x30\x30\x33\x20\x32\x42\x34\x46\x41\x31\x33\x45", iSendLen);
		memcpy(stMsgBuf.aczBuf, "\x30\x34\x30\x36\x2e\x01\x30\x34\x30\x36\x30\x30\x30\x31\x30\x30\x30\x30\x20\x20\x20\x30\x36\x30\x30\x30\x30\x30\x30\x30\x32\x20\x00\x00\x00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x32\x30\x30\xe2\x3a\x44\xc1\xb8\xc0\x98\x10\x00\x00\x00\x00\x10\x00\x00\x01\x31\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x33\x30\x39\x30\x30\x30\x30\x32\x32\x38\x31\x34\x33\x39\x30\x34\x30\x31\x39\x31\x37\x37\x31\x34\x33\x39\x30\x34\x30\x32\x32\x38\x30\x31\x31\x37\x34\x38\x31\x34\x30\x32\x31\x30\x30\x30\x36\x30\x38\x39\x39\x39\x39\x30\x30\x30\x30\x30\x38\x30\x30\x30\x39\x30\x30\x30\x30\x33\x37\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x3d\x32\x30\x30\x31\x38\x39\x39\x34\x32\x31\x36\x35\x30\x30\x38\x34\x32\x31\x30\x34\x39\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x34\x31\x3d\x31\x35\x36\x31\x35\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x31\x30\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x3d\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x35\x39\x31\x34\x30\x31\x39\x31\x37\x37\x30\x30\x30\x30\x30\x39\x33\x30\x39\x39\x39\x36\x34\x31\x30\x34\x38\x31\x34\x30\x30\x30\x31\x31\x35\x36\xe4\x0e\xb0\x5b\x48\x00\xde\x61\x32\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x31\x31\x30\x32\x32\x39\x30\x30\x30\x30\x30\x33\x20\x35\x44\x30\x46\x35\x35\x33\x44", iSendLen);
		break;
	case 2:
		iSendLen=317;  
		memcpy(stMsgBuf.aczBuf, "\x30\x33\x31\x33\x2e\x01\x30\x33\x31\x33\x30\x30\x30\x31\x30\x30\x30\x30\x20\x20\x20\x30\x36\x30\x30\x30\x30\x30\x30\x30\x32\x20\x00\x00\x00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x34\x32\x30\xf2\x3a\x44\x81\x88\xc0\x80\x10\x00\x00\x00\x40\x10\x00\x00\x41\x31\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x35\x38\x30\x30\x39\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x33\x30\x33\x30\x39\x31\x35\x30\x38\x34\x39\x30\x32\x30\x34\x37\x39\x31\x35\x30\x38\x30\x31\x30\x33\x30\x39\x30\x31\x33\x30\x34\x38\x31\x34\x30\x31\x32\x30\x30\x30\x38\x39\x39\x39\x39\x30\x30\x30\x30\x30\x38\x30\x30\x30\x39\x30\x30\x30\x30\x35\x30\x36\x38\x31\x35\x30\x32\x30\x34\x37\x38\x30\x30\x30\x30\x30\x39\x33\x30\x39\x39\x39\x36\x34\x31\x30\x34\x38\x31\x34\x30\x30\x30\x31\x31\x35\x36\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x32\x30\x30\x30\x32\x30\x34\x37\x38\x30\x33\x30\x39\x31\x35\x30\x38\x30\x31\x30\x30\x30\x39\x39\x39\x39\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x30\x30\x30\x30\x31\x31\x30\x32\x32\x39\x30\x30\x30\x30\x30\x33\x20\x30\x32\x32\x30\x31\x30\x30\x30\x32\x30\x30\x20\x20\x30\x30\x30\x30\x33\x39\x30\x30\x30\x30\x30\x31\x34\x42\x39\x46\x31\x41\x46\x37",  iSendLen);
		break;
	case 3:
			iSendLen=317;  
		memcpy(stMsgBuf.aczBuf,"\x30\x33\x31\x33\x2e\x01\x30\x33\x31\x33\x30\x30\x30\x31\x30\x30\x30\x30\x20\x20\x20\x30\x36\x30\x30\x30\x30\x30\x30\x30\x32\x20\x00\x00\x00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x00\x30\x30\x30\x30\x30\x30\x34\x32\x30\xf2\x3a\x44\x81\x88\xc0\x80\x10\x00\x00\x00\x40\x10\x00\x00\x41\x31\x39\x38\x37\x31\x35\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x37\x31\x35\x38\x32\x30\x39\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x33\x32\x30\x33\x30\x39\x31\x36\x30\x38\x31\x37\x30\x32\x30\x35\x35\x38\x31\x36\x30\x37\x32\x32\x30\x33\x30\x39\x30\x31\x33\x30\x34\x38\x31\x34\x30\x31\x32\x30\x30\x30\x38\x39\x39\x39\x39\x30\x30\x30\x30\x30\x38\x30\x30\x30\x39\x30\x30\x30\x30\x35\x30\x36\x38\x31\x36\x30\x32\x30\x35\x35\x36\x30\x30\x30\x30\x30\x39\x33\x30\x39\x39\x39\x36\x34\x31\x30\x34\x38\x31\x34\x30\x30\x30\x31\x31\x35\x36\x30\x32\x30\x30\x30\x30\x30\x30\x32\x30\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x32\x30\x30\x30\x32\x30\x35\x35\x37\x30\x33\x30\x39\x31\x36\x30\x37\x32\x32\x30\x30\x30\x39\x39\x39\x39\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x39\x30\x30\x30\x30\x31\x31\x30\x32\x32\x39\x30\x30\x30\x30\x30\x33\x20\x30\x32\x32\x30\x31\x30\x30\x30\x32\x30\x30\x20\x20\x30\x30\x30\x30\x36\x30\x30\x30\x30\x30\x30\x31\x46\x39\x39\x33\x41\x42\x39\x45", iSendLen);
		break;
   }


  nRet = msgsnd (iQueId, &stMsgBuf, iSendLen, ~IPC_NOWAIT);
  if (nRet)
  {
      return -1;
  }
  return 0;
}


int main( int argc, char *argv[] )
{
   int iInQueKey,  iCleanQueKey;
   int iInQueId,   iCleanQueId;

   if (argc < 3) {
       fprintf (stderr, "Usage: InQueKey ClearQueId\n");
       return -1;
   }

   iInQueKey = atoi (argv[1]); 
   iCleanQueKey = atoi (argv[2]); 

   iInQueId = msgget (iInQueKey, IPC_CREAT | 0660);
   if (iInQueId < 0) {
       fprintf (stderr, "��ȡ�����������ʧ��! [%s] Key=[%s]\n",
                strerror (errno), iInQueKey);
       return -1;
   }
   
   iCleanQueId = msgget (iCleanQueKey, IPC_CREAT | 0660);
   if (iCleanQueId < 0) {
       fprintf (stderr, "��ȡ�����������ʧ��! [%s] Key=[%s]\n",
                strerror (errno), iCleanQueId);
       return -1;
   }

   SYS_Zdtj(argv[3], iInQueId, getpid (), iCleanQueId);

   return 0;
}
