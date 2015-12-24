/*
**file:   tpl_base.h
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#include "tool_base.h"
#include "global.h"
#include "udo.h"
#include "shm_base.h"
#include "svc_base.h"

#ifndef __ADP_BASE_H__
#define __ADP_BASE_H__

#define ADP_SUCC                               0
#define ADP_FAIL                               -1
#define TPCALL_SERVICE_IN_HOST_ERR             -3001
#define ADP_NO_SUB_DEF                         -3002
#define ADP_NO_HOST_DEF                        -3003
#define ADP_TCBSTI_ERR                         -3004
#define ADP_SMTS_ERR                           -3005
#define ADP_RMFS_ERR                           -3006
#define ADP_GETNODEID_ERR                      -3007
#define ADP_GIFMFC_ERR                         -3008
#define ADP_PKGTOMSG_ERR                       -3009
#define ADP_GPNI_ERR                           -3010
#define ADP_PKGTOMID_ERR                       -3011
#define ADP_SENDLEN_ERR                        -3012
#define ADP_RECVLEN_ERR                        -3013
#define ADP_RFDBEFORE_ERR                      -3014
#define ADP_UDO2FMLBEFORE_ERR                  -3015
#define ADP_RFDAFTER_ERR                       -3016
#define ADP_FML2UDOAFTER_ERR                   -3017
#define ADP_GETRETCODE_ERR                     -3018
#define ADP_GETWMSG_ERR                        -3019
#define ADP_MSGSEND_ERR                        -3020
#define ADP_GETRMSG_ERR                        -3021
#define ADP_MSGRCV_ERR                         -3022
#define ADP_CALLSVC_ERR                        -3023
#define ADP_UDOSIZE_ERR                        -4002
#define ADP_OPEN_ERR                           -4004
#define ADP_WRITE_ERR                          -4005
#define ADP_INSERT_ERR                         -4006
#define ADP_HANDLE_TYPE_ERR                    -4007
#define ADP_SEND_BAL_ERR                       -4008
#define ADP_TPINIT_ERR                         -4009
#define ADP_FINDSVC_ERR                        -4010
#define ADP_COMM_ERR                           -4020
#define ADP_COMMTYPE_ERR                       -4030
#define ADP_COMMMAC_ERR                        -4040
#define ADP_TIMEOUTQTYPE_ERR                   -4050
#define ADP_MSGRCV_AND_WTTBL_ERR               -4060
#define ADP_HOST_DEAL_FAIL                      3001
#define ADP_FIND_TXCODE_ERR                     3002

#define TRAN_CODE_LEN   20


typedef struct
{
   int iNodeId;
   int iTempletId;
   int iToMsgId;
   int iToMidId;
   int iPkgAppId;
   char aczTranCode[TRAN_CODE_LEN + 1];
} PkgNeedInfo;

typedef struct
{
   char aczAddr[16];
   int  iPort;
} ComAddr;

typedef struct _ADPService
{
   char aczServerIp[24];
   int  iServerPort;
   int  iTimeOut;
   char aczService[61];
   char aczTxnCode[41];
} ADPService;

#ifdef __cplusplus
extern "C" {
#endif

extern int  iHostNodeId;

/*
**function:get the trancode,nodeid,templetid and tomidid,tomsgid
**input:   1 subsvcid
**output:  2 all neede information
**return:  0-success,other-error,use database error
*/
int ADP_GetPkgNeedInfo( int eiSubSvcId, PkgNeedInfo *opstPkgNeedInfo );

/*
**功能:根据主机返回码得到子服务返回码
**输入:1 提供子服务的主机节点编号
**     2 返回码值(强制转化为字符串,如整数1转换为"1",不考虑浮点数)
**输出:1 子服务返回码(所有正常完成子服务返回都不用调用该函数,返回码为0)
**返回   0-成功,1-没有定义主机节点或渠道节点返回码对应
*/
int ADP_GetRetByCode( int eiHostNodeId, char *epczHostRetCode, int *opiSubRet );

/*
**功能:取主机节点号
**输入:1 子服务序号
**2 服务结构信息
**输出:1 主机节点号
**返回 0-成功,其他-失败
*/
int ADP_GetHostNodeId( int eiSubSeq, SVC_Content *epstService,
                       int *opiHostNodeId );

/*
**function:get the trancode,nodeid,templetid and tomidid,tomsgid
**input:   1 svcid
**         2 subsvcid
**output:  1 all neede information
**return:  0-success,other-error
*/
int ADP_GetChanPkgNeedInfo( int eiSvcId, int eiSubSvcId,
                            PkgNeedInfo *opstPkgNeedInfo );

/*
**功能:    根据节点号、交易码和报文方向标志生成各节点值为空的链表
**输入参数:1 节点号
**         2 交易码
**         3 报文方向标志
**输出参数:1 空链表
**返回值   0 - 成功,-1 - 失败
*/
int ADP_CreateNullListByTranCode( int eiNodeId, char *epczTranCode,
                                  int eiMsgDescrb, UDP opfList );

/*
**功能 根据交易码和主机节点号查找相应的服务名
**输入 1 主机节点号
**     2 交易代码
**输出 1 服务名
**返回 0-成功,其他-失败
*/
int ADP_FindService( int eiHostNodeId, char *epczTranCode, char *opczSvcName );

/*
**功能:根据主机返回码得到渠道返回码
**输入:1 提供子服务的主机节点编号
**     2 返回码值(强制转化为字符串,如整数1转换为"1",不考虑浮点数)
**输出:1 渠道返回码(所有正常完成子服务返回都不用调用该函数,返回码为0)
**返回 0-成功,1-没有定义主机节点或渠道节点返回码对应
*/

int ADP_GetChanRetCode( int eiHostNodeId, int eiChanNodeId,
                        char *epczHostRetCode, char *opczChanRet );

/*
**功能:根据主机返回码得到渠道返回信息
**输入:1 提供子服务的主机节点编号
**     2 返回码值(强制转化为字符串,如整数1转换为"1",不考虑浮点数)
**输出:1 渠道返回信息
**返回   0-成功,1-没有定义主机节点或渠道节点返回码对应
*/

int ADP_GetChanRetMsg( int eiHostNodeId, int eiChanNodeId,
                       char *epczHostRetCode, char *opczErrMsg );
int ADP_GetChanRetCode( int eiHostNodeId, int eiChanNodeId,
                        char *epczHostRetCode, char *opczChanRet );


char * ADP_GetNodeName( int eiHostId );


int ADP_GetBalNeedInfo( int eiSubSvcId, NodeTranCfg *opstNodeTranCfg );

int ADP_GetCicsService( char *epczSubSvcName, char *opczSysId, 
                        char *opczServiceName );
/*
**功能：根据服务号和内部子服务号查找报文类型
**输入：1 服务号
**      2 内部子服务号
**返回：报文类型
*/
int ADP_FindMsgDscrb( int eiSvcId, int eiSubSeq );


/* 需要根据不同的子系统建立不同的队列               */
/* 特别要求: 配置中间业务时各个子系统建立不同的节点 */
int ADP_Transaction_Que( int eiNodeId, char *epczSendBuf, int eiSendLen,
                         char *opczRecvBuf, int *opiRecvLen );

/* 根据子服务号查交易代码 */
int ADP_FindTranCode( int eiSubSvcId, char *opczTranCode );

int ADP_RetCodeMap( int eiHostNodeId, int eiChanNodeId, char *epczHostRetCode,
                      char *opczChanRet, char *opczChanMsg );

int ADP_Transaction_WBPP (int eiNodeId, long lType,
                          char *epczSendBuf, int eiSendLen,
                          char *opczRecvBuf, int *opiRecvLen);

int ADP_Transaction_As400Tcp( int eiNodeId, char *epczSendBuf, int eiSendLen,
                              char *opczRecvBuf, int *opiRecvLen );

/**
  * 根据子服务编号查找ADP服务器地址,端口和服务名
  * 成功返回0, 失败返回 -1
  */
int ADP_GetService( int eiSubSvcId, ADPService *opstAdpService );

long GenSysSer(  );

int ADP_GetDictId( int iNodeId );

int ADP_SysError( int eiSubID, int eiSubSeq,
                  UDP epfReqList, UDP opfRspList,
                  SVC_Content * epstService );

extern double GetCurrentMillTime();

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
