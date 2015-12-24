#ifndef _ASM_OPERDEF_H_
#define _ASM_OPERDEF_H_

typedef struct _operate_code
{
   char m_sId[5];           /* ������ID */
   int  (*pfnFun)( char *opczBuf, int *opiBufLen );
   int  m_nInType;          /* ��������0-���� 1-�䳤 */
   int  m_nInAB;            /* �����ֶ����� 0-ASCII 1-BCD */
   int  m_nInLen;           /* �����ֶγ��� */
   int  m_nInLenType;       /* �����ֶγ����ֶ����� */
   int  m_nInLenLen;        /* �����ֶγ����ֶγ��� */
   int  m_nOutType;         /* �������0-���� 1-�䳤 */
   int  m_nOutAB;           /* ����ֶ����� 0-ASCII 1-BCD */
   int  m_nOutLen;          /* ����ֶγ��� */
   int  m_nOutLenType;      /* ����ֶγ����ֶ����� */
   int  m_nOutLenLen;       /* ����ֶγ����ֶγ��� */
} OperateCode;

EXEC SQL BEGIN DECLARE SECTION;
typedef struct _flow_info
{
   char       m_sProcode[11];
   char       m_sOperCode[3];
   char       m_sOperHintInd[3];
   char       m_sCryptType[3];
   char       m_sOperData[101];
   int        m_nSerialNo;
   char       m_sOperType[2];
   char       m_sStatus[2];
   char       m_sDac[17];
}FlowInfo;
typedef struct _flow_info_ind
{
   short      m_nProcode;
   short      m_nOperCode;
   short      m_nOperHintInd;
   short      m_nCryptType;
   short      m_nOperData;
   short      m_nSerialNo;
   short      m_nOperType;
   short      m_nStatus;
   short      m_nDac;
}FlowInfoInd;
EXEC SQL END DECLARE SECTION;

#endif
