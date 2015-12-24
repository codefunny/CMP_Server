/**
  * �����Զ��������������������������ڴ�
  * ������������������,��������ͷ�ļ�
  */
#include "udo.h"
#include "shm_base.h"

#ifndef __REVERSE_FUNCTION_H__
#define __REVERSE_FUNCTION_H__

EXEC SQL BEGIN DECLARE SECTION;
typedef struct _RevRecord
{
   char  m_AgDate[9];    /* �����¼ʱ������ YYYYMMDD                   */
   int   m_AgSerial;     /* ƽ̨��ˮ��                                  */
   int   m_RevSerial;    /* ������ˮ��                                  */
   char  m_Trancode[21]; /* ����������                                  */
   char  m_RetCode[41];  /* ����������                                  */
   int   m_ChnNodeId;    /* �����ڵ��                                  */
   int   m_HostNodeId;   /* �����ڵ��                                  */
   int   m_SvcId;        /* �����                                      */
   int   m_SubSeq;       /* �ڲ��ӷ������                              */
   int   m_SubId;        /* �����ӷ����                                */
   int   m_MaxRetryTimes;/* ������Դ���, ���ֻ��Ҫͬ������, ����Ϊ 0  */
   int   m_DelayTime;    /* ��������֮�����С���ʱ��(s)               */
   int   m_RetryTimes;   /* �Ѿ����Դ���                                */
   char  m_SuccFlag[2];     /* �Ƿ�ɹ���־, 1-�ɹ�, 0-���ɹ�, 9-ǿ����ֹ  */
   int   m_LastRevTime;  /* �ϴ�����ʱ��, ֱ���� time_t ����            */
   int   m_UdoLen;       /* UDO�ַ�������                               */
   char  m_UdoStr[2001]; /* ���������� udo �ַ���                       */
} RevRecord;
typedef struct _RevRecordInd
{
   short  m_AgDate;    /* �����¼ʱ������ YYYYMMDD                   */
   short   m_AgSerial;     /* ƽ̨��ˮ��                                  */
   short   m_RevSerial;    /* ������ˮ��                                  */
   short  m_Trancode; /* ����������                                  */
   short  m_RetCode;  /* ����������                                  */
   short   m_ChnNodeId;    /* �����ڵ��                                  */
   short   m_HostNodeId;   /* �����ڵ��                                  */
   short   m_SvcId;        /* �����                                      */
   short   m_SubSeq;       /* �ڲ��ӷ������                              */
   short   m_SubId;        /* �����ӷ����                                */
   short   m_MaxRetryTimes;/* ������Դ���, ���ֻ��Ҫͬ������, ����Ϊ 0  */
   short   m_DelayTime;    /* ��������֮�����С���ʱ��(s)               */
   short   m_RetryTimes;   /* �Ѿ����Դ���                                */
   short  m_SuccFlag;     /* �Ƿ�ɹ���־, 1-�ɹ�, 0-���ɹ�, 9-ǿ����ֹ  */
   short   m_LastRevTime;  /* �ϴ�����ʱ��, ֱ���� time_t ����            */
   short   m_UdoLen;       /* UDO�ַ�������                               */
   short  m_UdoStr; /* ���������� udo �ַ���                       */
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
  * ������������������
  */
int REV_AxpKernel( int eiRevSerial, int eiSubId, UDP pfReq, RevRecord *pstRevRecord );

/**
  * ADP�����ڵ����������
  */
int REV_AdpHost( int eiRevSerial, int erSubId, UDP pfReq, RevRecord *pstRevRecord  );

/**
  * ADP ��������
  */
int REV_AdpTrans( char *epczHostIp, int eiPort, char *transName, UDP pfReq );

/**
  * �����ӷ���Ų����ӷ�������
  */
SubService * REV_SearchSubService( int eiSubId );

/**
  * �����ӷ���Ų��ҽڵ�ͨ������
  */
NodeCommCfg * REV_SearchNodeCommCfgBySubId( int iSubId );


#ifdef __cplusplus
}
#endif

#endif
