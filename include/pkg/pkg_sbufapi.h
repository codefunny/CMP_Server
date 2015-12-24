#include "ServiceBuf.h"

#ifndef __PKG_SBUF_API_H__
#define __PKG_SBUF_API_H__

#define MAX_FLD_LEN 4096

int PKG_Sbuf2Udp (MsgFldCfgAry1 *epstFmlMsgFldCfgAry,
                  SERVICEBUF *epstSbuf, UDP opfFss);

int PKG_Udp2Sbuf ( MsgFldCfgAry1 *epstMsgFldCfgAry,
                 UDP epfFss, SERVICEBUF *opstSbuf);

/*
** 处理使用文件传输明细的情况
** chanflag:1-网银, 2-95533
*/
int PKG_Sbuf2UdpWithMx( int eiChanFlag, MsgFldCfgAry1 *epstMsgFldCfgAry1, SERVICEBUF *epstSbuf, RECORD_INFO *epstRecordInfo,
                        int iRecordNum, int eiNowPage, char *epczFileName, UDP opfInfoList);
#endif

