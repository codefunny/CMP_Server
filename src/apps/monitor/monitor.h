#include "udo.h"
#include "shm_base.h"

#ifndef __MONITOR_FUNCTION_H__
#define __MONITOR_FUNCTION_H__

EXEC SQL BEGIN DECLARE SECTION;
typedef struct 
{
   char  m_sPtDate[9];   
   int   m_nPtSerial;    
   int   m_nChCode;       /*ÇþµÀºÅ*/
   char  m_sChName[65];
   char  m_sChMsgid[5];
   char  m_sChTxCode[13];
   char  m_sChTxName[65];
   char  m_sAcctNo[33];
   char  m_sAcctNoType[9];
   double m_fTxAmt;
   int   m_nTxNum;
   char  m_sSysDate[11];
   char  m_sSysTime[9];
   char  m_sChRetCode[11];
   char  m_sChRetMsg[101];
   char  m_sMerchantId[16];
   char  m_sShortName[65];
   char  m_sTerminalId[9];
   char  m_sMachineId[33];
   char  m_sHoTxCode[13];
   char  m_sHoRetCode[13];
   char  m_sHoRetMsg[101];
   double m_fRealTxAmt;
   int   m_nRealTxNum;
   char  m_sTeller[21];
   char  m_sCmdStatus[2];
   char  m_sSuccFlag[2];
} stChMonitor;
typedef struct 
{
   short  m_nPtDate;   
   short   m_nPtSerial;    
   short   m_nChCode;       /*ÇþµÀºÅ*/
   short  m_nChName;
   short  m_nChMsgid;
   short  m_nChTxCode;
   short  m_nChTxName;
   short  m_nAcctNo;
   short  m_nAcctNoType;
   short  m_nTxAmt;
   short   m_nTxNum;
   short  m_nSysDate;
   short  m_nSysTime;
   short  m_nChRetCode;
   short  m_nChRetMsg;
   short  m_nMerchantId;
   short  m_nShortName;
   short  m_nTerminalId;
   short  m_nMachineId;
   short  m_nHoTxCode;
   short  m_nHoRetCode;
   short  m_nHoRetMsg;
   short m_nRealTxAmt;
   short   m_nRealTxNum;
   short  m_nTeller;
   short  m_nCmdStatus;
   short  m_nSuccFlag;
} stChMonitorInd;
EXEC SQL END   DECLARE SECTION;

#endif
