#ifndef __ADP_COMM_H__
#define __ADP_COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

int ADP_HostComm( char *epczHost, int eiPort,
                  char *epcSend,  int eiSendLen,
                  char *opcRecv,  int *opiRecvLen );

int ADP_AdpComm( char *epczHost, int eiPort,
                 char *req,  char *res,
                 char *epcFun, int eiTime );

#ifdef __cplusplus
}
#endif

#endif
