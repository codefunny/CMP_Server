/*
** file:   svc_basic.h
*/
#include "udo.h"
#include "cmp.h"
#include "shm_base.h"

#ifndef __SERVICE_BASIC_H__
#define __SERVICE_BASIC_H__

/*将CMP看做是提供服务的一个主机,每次需要根据需要更改该值*/
#define CMP_NODE_ID                           100

#define CMP_SYS_ERR                          -5010
#define CMP_PKG_ERR                          -5020
#define CMP_COM_ERR                          -5030
#define CMP_TUX_ERR                          -5040
#define CMP_CHK_ERR                          -5050
#define CMP_GSUBCFG_ERR                      -5050
#define CMP_ASM_ERR                          -5060
#define CMP_ASM_STRFPIN_ERR                  -5061
#define CMP_ASM_SENCPIN_ERR                  -5062
#define CMP_ASM_STRFPWD_ERR                  -5063
#define CMP_ASM_SENCPWD_ERR                  -5064
#define CMP_COMMAC_ERR                       -5070
#define CMP_TUXTMOUT_ERR                     -5080

#define CMP_HOST_ERR                         8888

#define SVC_SUCC                 0
#define SUB_SUCC                 0
#define SVC_BASE                 800
#define SVC_GETCFG_ERR           SVC_BASE + 1
#define SUB_KEYCFG_ERR           SVC_BASE + 2
#define SUB_GETCFG_ERR           SVC_BASE + 3
#define SUB_CHK_ERR              SVC_BASE + 4
#define SUB_ADDNEW_ERR           SVC_BASE + 5
#define SUB_ASM_ERR              SVC_BASE + 6
#define SUB_SAVE_ERR             SVC_BASE + 7
#define SVC_NO_MEM               SVC_BASE + 8
#define SVC_SUBINIT_ERR          SVC_BASE + 9
#define SVC_INIT_ERR             SVC_BASE + 10
#define SVC_SUBFLOW_ERR          SVC_BASE + 11
#define SVC_SUB_NOTFOUND         SVC_BASE + 12
#define SUB_SYS_ERR              SVC_BASE + 13

#define SUB_ERR_NOOP             0 /*无处理                 */
#define SUB_ERR_TRANLOG          1 /*只记冲正日志           */
#define SUB_ERR_BALANCE          2 /*需要冲正,失败需要记日志*/
#define SVC_CORRECT_RET          3 /*服务正常返回需要重组   */
#define SVC_ERR_RETURN           4 /*服务错误响应,不需要重组*/
#define SVC_ERR_RSP_ASM          5 /*服务错误响应*/

#define SVC_TRANCODE_LEN         20
#define SVC_NAME_LEN             40
#define SVC_NOTE_LEN             100
#define SVC_ERRMSG_LEN           200
#define SVC_STAN_LEN             27
#define SUB_NAME_LEN             40

#define SUB_ERR_NOOP             0 /*无处理                 */
#define SUB_ERR_TRANLOG          1 /*只记冲正日志           */
#define SUB_ERR_BALANCE          2 /*需要冲正,失败需要记日志*/
#define SVC_ERR_RSP_ASM          5 /*服务错误响应           */

extern ProcessManager     *gpstProcess;


typedef struct _SUB_SvcList SUB_SvcList;
typedef struct _SVC_Content SVC_Content;
typedef struct _SVC_Config  SVC_Config;

extern char aczChannelLog[2048];

typedef struct _SUB_SvcCfg
{
   int  iSubId;                        /*全局子服务号        */
   int  iTempletId;                    /*子服务模板          */
   int  iRecordsPerTime;               /*每次发送的最大重复数*/
   char aczSubTranCode[20 + 1];        /*子服务交易代码      */
   char aczSubName[SUB_NAME_LEN + 1];  /*子服务逻辑名字      */
} SUB_SvcCfg;

struct _SVC_Config
{
   int  iNodeId;
   char aczTranCode[SVC_TRANCODE_LEN + 1];
   int  iSvcId;
   char aczSvcName[SVC_NAME_LEN + 1];
   int  iRepTimes;
   int  iTimeOut;
   int  iBalanceFlag;
   char aczSvcNote[SVC_NOTE_LEN + 1];
};

struct _SUB_SvcList
{
   int           iSubSvcSeq;           /*子服务在服务中编号            */
   int           iHostId;              /*主机编号                      */
   int           iMsgAppId;            /*报文应用号                    */
   int           iIsBatch;             /*是否批量子服务                */
   int           iBatchMode;           /*批量模式                      */
   int           iReadKeyId;           /*读队列Key值                   */
   int           iWriteKeyId;          /*写队列Key值                   */
   int           iRunResultCode;       /*子服务运行值                  */
   int           iErrorHandleType;     /*错误处理类型                  */
   /*这个字段指明当发生错误的时候的处理情况,2-冲正,1-只记日志,0-无处理 */
   char          aczErrorMessage[SVC_ERRMSG_LEN];
   int           iSubId;                         /*全局子服务号        */
   int           iTempletId;                     /*子服务模板          */
   int           iRecordsPerTime;                /*每次发送的最大重复数*/
   char          aczTranCode[20 + 1];            /*子服务交易代码      */
   char          aczSubName[SUB_NAME_LEN + 1];   /*子服务逻辑名字      */
   SUB_SvcCfg    stCorrSubCfg;         /*正确子服务配置                */
   SUB_SvcCfg    stErrSubCfg;          /*错误处理子服务配置            */
   UDP           pfReqList;            /*请求信息链表                  */
   UDP           pfAnswList;           /*响应信息链表                  */
   SVC_Content  *pstService;           /*服务结构指针                  */
   UDP           pfBalanceList;        /*错误处理请求信息链表          */
   SUB_SvcList  *pstNextSubSvc;        /*下一个子服务指针              */
   int           iStartErrorFlow;      /*标志发生错误的时候是否开始错误流程*/
   int           iOutTime;             /*超时时间*/
   int           iLstId;
};

struct _SVC_Content
{
   int           iSvcId;                         /*服务号              */
   int           iChannelId;                     /*服务渠道号          */
   int           iPkgAppId;                      /*报文应用号          */
   int           iReadKeyId;                     /*读队列Key值         */
   int           iWriteKeyId;                    /*写队列Key值         */
   int           iSubListId;                     /*子服务实际运行的链号*/
   short         shStep;                         /* 子服务运行步骤     */
   char          aczErrorMessage[SVC_ERRMSG_LEN];/*服务运行错误信息    */
   char          aczElSerial[SVC_STAN_LEN];      /*CMP部流水号  */
   int           iErrorType;                     /*标志系统运行错误类型*/
   UDP           pfReqList;                      /*请求信息链表        */
   UDP           pfAnswList;                     /*响应信息链表        */
   UDP           pfLogList;                      /*交易要素日志链表    */
   SUB_SvcList   stSubSvcHead;                   /*子服务头节点        */
   SUB_SvcList  *pstSubSvcTail;                  /*最后一个子服务指针  */
   SUB_SvcList  *pstSubSvcCurr;                  /*当前活动子服务指针  */
   /*以下是为冲正服务器提供的必要信息*/
   int           iCommId;                        /*渠道通讯socket      */
   int           iStatus;                        /*服务初始状态        */
};

#ifdef __cplusplus
extern "C"
{
#endif

/*
** 功能：初始化第0个子服务和服务参数
** 输入参数：1 服务指针
**           2 服务请求参数链表
**           3 服务配置
**           4 报文应用号
**           5 读队列号
**           6 写队列号
** 输出参数：  无
** 返回值：  0成功，其他失败
*/
int SVC_Init( SVC_Content *epstService, UDP epfReqList,
              Service  *epstSvcConfig, int eiPkgAppId );

/*
** 功能：从数据库取内部服务号
** 输入参数：1 渠道节点号
**           2 报文应用号
**           3 交易代码
** 输出参数：1 内部服务配置
** 返回值：0-成功，其它错误
*/

int SVC_GetConfig( int eiChanId, int eiPkgAppId,
                   char *epczTranCode, Service *opstServiceCfg );

/*
** 功能: 从服务结构取数据
** 输入: int         eiSubSvcSeq     子服务内部号
**       int         eiMsgSscrb      报文描述
**       int         eiFldSeq        报文域号
**       int         eiNum           序号
**       int         eiType          域类型
**       SVC_Content *epstSvcList    指向服务结构
** 输出: char        *opczValue      域值
**       int         *opiLen         域值长度
** 返回: int 0成功 其他不成功
*/
int SVC_GetValueFromSub( int eiSubSvcSeq, int eiMsgDesc, int eiFldSeq,
                         int eiNum,       int eiType,
                         SVC_Content *epstSvcList, char *opczValue, int *opiLen  );

/*
**功能：释放服务占用的空间
**输入参数：1 服务指针
**输出参数：  无
**返回值：    无
*/
void SVC_Free( SVC_Content *epstService );

/*
**功能:根据渠道节点号设置服务需要的4个关键值
**输入:1 渠道节点号
**     2 服务请求参数
**输出:1 服务结构指针
**返回:无
*/
void SVC_SetKeyValue( int eiChanNodeId, UDP epfSvcReq,
                      SVC_Content *opstService );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
