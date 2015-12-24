/**
  * 所有自动冲正主机适配器函数申明都在此
  * 如果有其它特殊的申明,单独创建头文件
  */
#include "udo.h"
#include "shm_base.h"

#ifndef __REVERSE_FUNCTION_H__
#define __REVERSE_FUNCTION_H__

EXEC SQL BEGIN DECLARE SECTION;
typedef struct _RevRecord
{
   char  m_AgDate[9];    /* 插入记录时的日期 YYYYMMDD                   */
   int   m_AgSerial;     /* 平台流水号                                  */
   int   m_RevSerial;    /* 冲正流水号                                  */
   char  m_Trancode[21]; /* 主机交易码                                  */
   char  m_RetCode[41];  /* 主机返回码                                  */
   int   m_ChnNodeId;    /* 渠道节点号                                  */
   int   m_HostNodeId;   /* 主机节点号                                  */
   int   m_SvcId;        /* 服务号                                      */
   int   m_SubSeq;       /* 内部子服务序号                              */
   int   m_SubId;        /* 冲正子服务号                                */
   int   m_MaxRetryTimes;/* 最大重试次数, 如果只需要同步冲正, 设置为 0  */
   int   m_DelayTime;    /* 两次重试之间的最小间隔时间(s)               */
   int   m_RetryTimes;   /* 已经重试次数                                */
   char  m_SuccFlag[2];     /* 是否成功标志, 1-成功, 0-不成功, 9-强行终止  */
   int   m_LastRevTime;  /* 上次重试时间, 直接是 time_t 类型            */
   int   m_UdoLen;       /* UDO字符串长度                               */
   char  m_UdoStr[2001]; /* 待冲正报文 udo 字符串                       */
} RevRecord;
typedef struct _RevRecordInd
{
   short  m_AgDate;    /* 插入记录时的日期 YYYYMMDD                   */
   short   m_AgSerial;     /* 平台流水号                                  */
   short   m_RevSerial;    /* 冲正流水号                                  */
   short  m_Trancode; /* 主机交易码                                  */
   short  m_RetCode;  /* 主机返回码                                  */
   short   m_ChnNodeId;    /* 渠道节点号                                  */
   short   m_HostNodeId;   /* 主机节点号                                  */
   short   m_SvcId;        /* 服务号                                      */
   short   m_SubSeq;       /* 内部子服务序号                              */
   short   m_SubId;        /* 冲正子服务号                                */
   short   m_MaxRetryTimes;/* 最大重试次数, 如果只需要同步冲正, 设置为 0  */
   short   m_DelayTime;    /* 两次重试之间的最小间隔时间(s)               */
   short   m_RetryTimes;   /* 已经重试次数                                */
   short  m_SuccFlag;     /* 是否成功标志, 1-成功, 0-不成功, 9-强行终止  */
   short   m_LastRevTime;  /* 上次重试时间, 直接是 time_t 类型            */
   short   m_UdoLen;       /* UDO字符串长度                               */
   short  m_UdoStr; /* 待冲正报文 udo 字符串                       */
} RevRecordInd;
EXEC SQL END   DECLARE SECTION;

typedef struct _ReverseRegister
{
   int m_HostId;
   int (*m_Function)( int eiRevSerial, int eiSubId, UDP pfReq, RevRecord *pstRevRecord );
} ReverseRegister;

#ifdef __cplusplus
extern "C" {
#endif

/**
  * 核心主机冲正适配器
  */
int REV_AxpKernel( int eiRevSerial, int eiSubId, UDP pfReq, RevRecord *pstRevRecord );

/**
  * ADP主机节点冲正适配器
  */
int REV_AdpHost( int eiRevSerial, int erSubId, UDP pfReq, RevRecord *pstRevRecord  );

/**
  * ADP 冲正交易
  */
int REV_AdpTrans( char *epczHostIp, int eiPort, char *transName, UDP pfReq );

/**
  * 根据子服务号查找子服务配置
  */
SubService * REV_SearchSubService( int eiSubId );

/**
  * 根据子服务号查找节点通信配置
  */
NodeCommCfg * REV_SearchNodeCommCfgBySubId( int iSubId );


#ifdef __cplusplus
}
#endif

#endif
