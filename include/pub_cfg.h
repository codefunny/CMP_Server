/*
**file:pub_cfg.h
*/

#ifndef __PUB_CFG_H__
#define __PUB_CFG_H__

typedef struct _NodeInitCfg
{
   char aczNodeName[41];  /*实际上是进程名                            */
   int  iNodeId;          /*渠道节点号                                */
   int  iMsgAppId;        /*渠道报文应用号                            */
   int  iShmIpcKey;       /*配置信息存放共享内存IPCKEY                */
   int  iTag;             /*标志,一般节点没有用,用于区分网银BTOB和BTOC*/
} NodeInitCfg;

typedef struct _NodePros
{
   int iChannelNum;
   NodeInitCfg stNodeInitCfg[200];
} NodePros;

/*
**功能:读文件中的二进制配置信息
**输入:1 文件指针
**输出:1 配置信息结构
**返回   无
*/
void PUB_Read( FILE *efp, NodePros *epstNodePros );

/*
**功能:根据渠道名字和标志取节点ID号共享内存IPCKEY和节点报文应用号
**输入:1 渠道节点名
**     2 标志(一般节点为0,网银BTOC为0,网银BTOB为1)
**输出:1 渠道节点号
**     2 渠道配置信息共享内存IPCKEY
**     3 渠道节点报文应用号
*/
int PUB_GetValByName( char *epcChanlName, int eiTag,
                      int *opiNodeId, int *opiShmIpcKey,
                      int *opiMsgAppId );

#endif
/*
** end of file
*/
