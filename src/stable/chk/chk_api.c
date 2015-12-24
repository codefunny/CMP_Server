/*
**   chk_api.c
*/
#include <stdlib.h>

#include "global.h"
#include "chk.h"
#include "chk_api.h"
#include "svc_base.h"
#include "tool_base.h"
#include "shm_base.h"
#include "shm_api.h"
#include "pub_base.h"

#include "log.h"

#define DBG_FILE  "chk_api_dbg.log"
#define NO_DFLT_SVC_ID  -9999

char * chkexpstr;
int    opt;
int    parentheses;
extern int    yyparse();

static SVC_Content *pstSvcList;
static RelaCond *pstRelaCondAddr;
static int iRelaCondNum;

static int CHK_GetRelaCondAddr (int iCondId);
static int CHK_GetChkCfg (int eiSvcId, int eiCurSubSvcSeq,
                          ChkCfg *epstChkCfg, int *opiCount);
static int CHK_DataChkRela (char *epczLVal, char *epczRVal,
                            int  eiType, int eiOp);
static int CHK_GetVal (int eiSubSvcSeq, int eiMsgDscrb, int eiFldSeq,
                       int eiNum, char *epczType, int eiStartBit, int eiEndBit,
                       SVC_Content *epstSvcList, char *opczValue );

static int iCurSubSvc;     /* ģ����� */

/***************************************************
����: ����������š���ǰ�ӷ����ڲ���,
       �Է�����Ϣ�������������� 
����:  int      eiSvcId          ���������
       int      eiCurSubSvcseq   ��ǰ�ӷ����ڲ���
       SVC_Content *epstsvcList  ������Ϣ����
       int      eiNum            ���������( ��0��ʼ )
���: int      *oiNextsubSvcseq ��һ���ӷ����ڲ���
����: 0   �ɹ�
       !=0 ʧ��
***************************************************/
int CHK_GetSubSvcSeq (int eiSvcId,
                      int eiCurSubSvcSeq,
                      SVC_Content *epstSvcList,
                      int eiNum, /* no used now */
                      int *oiNextSubSvcSeq)
{
    ChkCfg stChkCfg[CHK_COND_RECORDS_MAX];
    int i, iCount, iRet;
    int iDfltNextSubSvcSeq;
    struct timeval tmS, tmE;

    pstSvcList = epstSvcList;

    memset( &stChkCfg, 0, sizeof( stChkCfg ) );

    /* ���������ñ�ChkCfg */
    iCount = 0;
    iRet = CHK_GetChkCfg (eiSvcId, eiCurSubSvcSeq, stChkCfg, &iCount);
    if( iRet != 0 )
    {
       LOG4C ((LOG_FATAL, 
               "���ҵ�ǰ�ӷ������̾������ó���,"
               "�����=[%d],��ǰ�ӷ����ڲ����=[%d]",
               eiSvcId, eiCurSubSvcSeq));
       return (iRet);
    }

    if (iCount == 0) {
        LOG4C ((LOG_FATAL,
                "�ӷ���ĩ�������̾�������"
                "�����=[%d],��ǰ�ӷ����ڲ����=[%d]",
                eiSvcId, eiCurSubSvcSeq));
        return (CHK_ERR_NO_RECORD);
    }

    iDfltNextSubSvcSeq = NO_DFLT_SVC_ID;
    for (i = 0; i < iCount; i++) {

#ifdef DEBUG3
        LOG4C ((LOG_DEBUG,
                "SvcId %d cursubseq %d condid %d next %d exp %s",
                stChkCfg[i].iSvcId, stChkCfg[i].iCurSubSeq,
                stChkCfg[i].iCondId, 
                stChkCfg[i].iNextSubSeq, stChkCfg[i].aczChkExp));
        gettimeofday (&tmS, NULL);
#endif
    
        if (stChkCfg[i].iDfltFlag == 1) {
            if (iDfltNextSubSvcSeq != NO_DFLT_SVC_ID) {
                LOG4C ((LOG_FATAL, 
                       "�����������ñ�(ChkCfg)���ݴ�:"
                       "һ���ӷ����ж��Ĭ������,"
                       "�����=[%d],�ӷ����ڲ����=[%d]",
                       stChkCfg[i].iSvcId, stChkCfg[i].iCurSubSeq));
                return CHK_ERR_DFLT_COND_IS_TOO_MUCH;
            }
            iDfltNextSubSvcSeq = stChkCfg[i].iNextSubSeq;
            continue;
        }

        pstRelaCondAddr = NULL;
        iRelaCondNum = 0;
        iRet = CHK_GetRelaCondAddr (stChkCfg[i].iCondId);
        if (iRet !=0 || iRelaCondNum == 0) {
            LOG4C ((LOG_FATAL, 
                    "���̾���������Ӧ�Ĺ�ϵ���ʽ������."
                    "�����=[%d],��ǰ�ӷ����ڲ����=[%d],������=[%d]",
                    stChkCfg[i].iSvcId, stChkCfg[i].iCurSubSeq, 
                    stChkCfg[i].iCondId));
            return (CHK_ERR_NO_RECORD);
        }
       
        chkexpstr = stChkCfg[i].aczChkExp;
        opt = 0;
        parentheses = 0;
  

        iRet = yyparse ();   
        if (iRet == 1) { /* �����﷨���� */
            LOG4C ((LOG_FATAL,
                    "���̾������������﷨����."
                    "�����=[%d],��ǰ�ӷ����ڲ����=[%d],������=[%d]",
                    stChkCfg[i].iSvcId, stChkCfg[i].iCurSubSeq, 
                    stChkCfg[i].iCondId));
            return (CHK_ERR_DATA_IS_ERR);
        }

#ifdef DEBUG3
        gettimeofday (&tmE, NULL);
        if (tmE.tv_usec < tmS.tv_usec) {
            tmE.tv_sec -= 1;
            tmE.tv_usec += 1000000;
        }
        LOG4C ((LOG_DEBUG,
                "time: %d\n", (tmE.tv_sec - tmS.tv_sec) * 1000000 + (tmE.tv_usec
- tmS.tv_usec)));
#endif


        if (iRet == 16) { /* ����Ϊ�� */
#ifdef DEBUG
            LOG4C ((LOG_DEBUG,
                    "Next SubSeq %d", stChkCfg[i].iNextSubSeq)); 
#endif
            *oiNextSubSvcSeq = stChkCfg[i].iNextSubSeq;
            return 0 ;
        }
    }
                
    if (iDfltNextSubSvcSeq == NO_DFLT_SVC_ID) { /* û��Ĭ������ */
        return CHK_ERR_NO_COND_IS_TRUE;
    } else {                                   /* ��Ĭ������ */
#ifdef DEBUG
        LOG4C ((LOG_DEBUG,
                "Next SubSeq %d", iDfltNextSubSvcSeq)); 
#endif
        *oiNextSubSvcSeq = iDfltNextSubSvcSeq;
        return 0;
    }
}

static int CHK_ChkCfgCmp (const void * pNode1, const void * pNode2)
{
     ChkCfg *pstC1, *pstC2;

     pstC1 = (ChkCfg *)pNode1;
     pstC2 = (ChkCfg *)pNode2;

     if (pstC1->iSvcId == pstC2->iSvcId) {
         return (pstC1->iCurSubSeq - pstC2->iCurSubSeq);
     } else {
         return (pstC1->iSvcId - pstC2->iSvcId);
     }
}

/***************************************************
����: ������������š���ǰ�ӷ����ڲ��Ŷ��������ñ�
����:  int      eiSvcId         ���������
       int      eiCurSubSvcSeq  ��ǰ�ӷ����ڲ��� 
���:  ChkCfg   *epstChkCfg     ��������
       int      *opiCount       ��������
����:
***************************************************/
static int CHK_GetChkCfg (int eiSvcId, int eiCurSubSvcSeq, 
                          ChkCfg *epstChkCfg, int *opiCount)
{
    int iCount, iRowNum;
    ChkCfg *pstChkCfgAddr, *pstChkCfg, stChkCfg;

    assert (epstChkCfg != NULL);
    assert (opiCount != NULL);

    iCount = iRowNum = 0;
    *opiCount = 0;
    pstChkCfgAddr = (ChkCfg *) SHM_ReadData (pcShmAddr, CHKCFG, &iRowNum);
    if (pstChkCfgAddr == NULL) {
        LOG4C ((LOG_FATAL,
                "�ӹ����ڴ��ȡ���̾�������ʧ��"));
        return -1;
    }

    memset (&stChkCfg, 0, sizeof (stChkCfg));
    stChkCfg.iSvcId = eiSvcId;
    stChkCfg.iCurSubSeq = eiCurSubSvcSeq;
    pstChkCfg = (ChkCfg *)bsearch (&stChkCfg, pstChkCfgAddr, iRowNum,
                         sizeof (ChkCfg), CHK_ChkCfgCmp);
    if (pstChkCfg == NULL) {
        return 0;
    }

    if (pstChkCfg != pstChkCfgAddr) {
        do {
            --pstChkCfg;
            if (pstChkCfg < pstChkCfgAddr) {
                break;
            }
        } while (pstChkCfg->iSvcId == eiSvcId &&
                 pstChkCfg->iCurSubSeq == eiCurSubSvcSeq);
        pstChkCfg++;
    }

    while (pstChkCfg->iSvcId == eiSvcId &&
           pstChkCfg->iCurSubSeq == eiCurSubSvcSeq) {
        iCount++;
        if (iCount > CHK_COND_RECORDS_MAX) {
            LOG4C ((LOG_FATAL, 
                    "��������[%d]�������������¼����CHK_COND_RECORDS_MAX[%d]"
                    "�����=[%d],��ǰ�ӷ����ڲ����=[%d]",
                    iCount, CHK_COND_RECORDS_MAX, eiSvcId, eiCurSubSvcSeq));

            return CHK_ERR_COND_IS_TOO_MUCH;
        }
        *epstChkCfg = *pstChkCfg;
        pstChkCfg++;
        if (pstChkCfg - pstChkCfgAddr > iRowNum) {
            break;
        }
        epstChkCfg++;
    }

    *opiCount = iCount;

    return 0;
}

static int CHK_RelaCondCmp (const void * pNode1, const void * pNode2)
{
     RelaCond *pstR1, *pstR2;

     pstR1 = (RelaCond *)pNode1;
     pstR2 = (RelaCond *)pNode2;

     return (pstR1->iCondId - pstR2->iCondId);
}

/***************************************************
����:  ���������Ų��ҹ�ϵ���ʽ����
����:  int      iCondId         ������
���:  ����ȫ�ֱ���pstRelaCondAddrΪ����������¼��ַ
           ȫ�ֱ���iRelaCondNumΪ����������¼����
����:  0 �ɹ�    ���� ʧ��
***************************************************/
int CHK_GetRelaCondAddr (int iCondId)
{
    int  iRowNum, iCount = 0;
    RelaCond *pstTemp, *pstRelaCond, stRelaCond;

    pstTemp = (RelaCond *) SHM_ReadData (pcShmAddr, RELACOND, &iRowNum);
    if (pstTemp == NULL) {
        LOG4C ((LOG_FATAL, 
               "�ӹ����ڴ��ȡ��ϵ���ʽ����ʧ��"));
        return (-1);
    }

    memset (&stRelaCond, 0, sizeof (stRelaCond));
    stRelaCond.iCondId = iCondId;
    pstRelaCond = (RelaCond *)bsearch (&stRelaCond, pstTemp, iRowNum,
                           sizeof (RelaCond), CHK_RelaCondCmp);
    if (pstRelaCond == NULL) {
        return 0;
    }

    if (pstRelaCond != pstTemp) {
        do {
            --pstRelaCond;
            if (pstRelaCond < pstTemp) {
                break;
            }
        } while (pstRelaCond->iCondId == iCondId);
        pstRelaCond++;
    }

    pstRelaCondAddr = pstRelaCond;

    while (pstRelaCond->iCondId == iCondId) {
        iCount++;
        pstRelaCond++;
        if (pstRelaCond - pstTemp > iRowNum) {
            break;
        }
    }

    iRelaCondNum = iCount;
    
    return (0);
}

/***************************************************
����: �����ַ�����ϵ�������
����: char       *epczLVal       ��ֵ����(�ַ�����ʽ)
      char       *epczRVal       ��ֵ����(�ַ�����ʽ)
      int        eiType          ��������
      int        eiOp            ��ϵʽ
���: ��
����: int
      1                    ��������
      0                    ����������
      CHK_ERR_DATA_IS_ERR  �������ô�
***************************************************/
static int CHK_DataChkRela( char *epczLVal, char *epczRVal,
                            int eiType, int eiOp) 
{
   int iRes;
   int iLVal, iRVal;
   long lLVal, lRVal;
   double dLVal, dRVal;
   float fLVal, fRVal;
   char aczLStr[50], aczRStr[50];

   switch( eiType )
   {
   case SHORTTYPE:
   case INTTYPE:
      iLVal = atoi( epczLVal );
      iRVal = atoi( epczRVal );
      LOG4C ((LOG_DEBUG, 
              "L:%d, R:%d comp %d\n", iLVal, iRVal, eiOp));

      switch( eiOp)
      {
      case EQ:
         iRes = ( iLVal == iRVal );
         break;
      case NE:
         iRes = ( iLVal != iRVal );
         break;
      case LT:
         iRes = ( iLVal < iRVal );
         break;
      case GT:
         iRes = ( iLVal > iRVal );
         break;
      case LE:
         iRes = ( iLVal <= iRVal );
         break;
      case GE:
         iRes = ( iLVal >= iRVal );
      }
      break;
   case LONGTYPE:
      lLVal = atol( epczLVal );
      lRVal = atol( epczRVal );
      LOG4C ((LOG_DEBUG, 
              "L:%d, R:%d\n",lLVal, lRVal));

      switch( eiOp )
      {
      case EQ:
         iRes = ( lLVal == lRVal );
         break;
      case NE:
         iRes = ( lLVal != lRVal );
         break;
      case LT:
         iRes = ( lLVal < lRVal );
         break;
      case GT:
         iRes = ( lLVal > lRVal );
         break;
      case LE:
         iRes = ( lLVal <= lRVal );
         break;
      case GE:
         iRes = ( lLVal >= lRVal );
      }
      break;
   case FLOATTYPE:
      fLVal = (float)atof( epczLVal );
      fRVal = (float)atof( epczRVal );
      LOG4C ((LOG_DEBUG, 
              "L:%f, R:%f\n", fLVal, fRVal));

      switch( eiOp)
      {
      case EQ:
         iRes = ( fLVal == fRVal );
         break;
      case NE:
         iRes = ( fLVal != fRVal );
         break;
      case LT:
         iRes = ( fLVal < fRVal );
         break;
      case GT:
         iRes = ( fLVal > fRVal );
         break;
      case LE:
         iRes = ( fLVal <= fRVal );
         break;
      case GE:
         iRes = ( fLVal >= fRVal );
      }
      break;
   case DOUBLETYPE:
      dLVal = (double)atof( epczLVal );
      dRVal = (double)atof( epczRVal );
      LOG4C ((LOG_DEBUG, 
              "L:%lf, R:%lf\n",dLVal, dRVal));

      switch( eiOp)
      {
      case EQ:
         iRes = ( dLVal == dRVal );
         break;
      case NE:
         iRes = ( dLVal != dRVal );
         break;
      case LT:
         iRes = ( dLVal < dRVal );
         break;
      case GT:
         iRes = ( dLVal > dRVal );
         break;
      case LE:
         iRes = ( dLVal <= dRVal );
         break;
      case GE:
         iRes = ( dLVal >= dRVal );
      }
      break;
   case STRTYPE:
      strcpy( aczLStr, epczLVal );
      strcpy( aczRStr, epczRVal );

      switch( eiOp )
      {
      case EQ:
         iRes = ( strcmp( aczLStr, aczRStr ) == 0 );
         break;
      case NE:
         iRes = ( strcmp( aczLStr, aczRStr ) != 0 );
         break;
      case LT:
         iRes = ( strcmp( aczLStr, aczRStr ) < 0 );
         break;
      case GT:
         iRes = ( strcmp( aczLStr, aczRStr ) > 0 );
         break;
      case LE:
         iRes = ( strcmp( aczLStr, aczRStr ) <= 0 );
         break;
      case GE:
         iRes = ( strcmp( aczLStr, aczRStr ) >= 0 );
      }
      break;
   default:
      fprintf( stderr, "Type Error.\n" );
      break;
   }
   return( iRes );
}

/***************************************************
����: �ӷ�����Ϣ������ȡ����
����: int       eiSubSvcSeq     �ӷ����ڲ���
      int       eiMsgSscrb      ��������
      int       eiFldSeq        �������
      char      *epczType       ������
      int       eiStartBit      ��ʼλ
      int       eiEndBit        ��ֹλ
      int       eiNum           ���
      SVC_Content *epstSvcList  ������Ϣ����
      char      *opczValue      �ַ�����ʽ����
���: ��
����: int
      0         �ɹ�
      <0        ���ɹ�
***************************************************/
static int CHK_GetVal( int eiSubSvcSeq, int eiMsgDscrb, int eiFldSeq,
                       int eiNum, char *epczType, int eiStartBit, int eiEndBit,
                       SVC_Content *epstSvcList, char *opczValue )
{
   char aczValue[CHK_STR_MAX_LEN + 1];
   int iRet, iStart, iLen, iFldLen, iFldType;

   memset( aczValue, 0, sizeof( aczValue ) );

   iFldType = atoi( epczType );

   /* ������������ȡ���� */
   iRet = SVC_GetValueFromSub( eiSubSvcSeq, eiMsgDscrb, eiFldSeq,
                               0, UDT_STRING,
                               epstSvcList, aczValue, &iFldLen );
   if( iRet != 0 )
   {
      return iRet;
   }
   
   /**�
     * eiStartBit -����ʼ�±�
     * eiEndBit   -  ȡ���ٸ��ַ�
     */
   if (iFldType == UDT_STRING || iFldType == UDT_BIN || iFldType == UDT_OTHER) {
        if (iFldLen <= eiStartBit) {
            opczValue[0] = '\0';
        } else {
            if (iFldLen < eiStartBit + eiEndBit) {
                eiEndBit = iFldLen - eiStartBit;
            }
            memcpy (opczValue, &aczValue [eiStartBit], eiEndBit);
        }
    } else {
        strcpy (opczValue, aczValue);
    }

   return 0;
}

/***************************************************
����:  ���ݱ��ʽ��ʶ������ʽ��ֵ ��yacc����
����:  int      iExprId  ���ʽ��ʶ
���:  
����:  16 ��    0 ��
***************************************************/
int CHK_ExprCalc (int iExprId)
{
    int   i, iResult;
    int   iLFldTypePri, iRFldTypePri, iChkFldType;
    char  aczLValue[CHK_STR_MAX_LEN+1], aczRValue[CHK_STR_MAX_LEN+1];
    RelaCond *pstRelaCond;
    struct timeval tmS, tmE;

    for (i = 0; i < iRelaCondNum; i++) {
        if (pstRelaCondAddr[i].iExprId == iExprId) {
            break; 
        }
    }

    if (i >= iRelaCondNum) {
        LOG4C ((LOG_FATAL, 
               "��������Ӧ�Ĺ�ϵ���ʽ! ���ʽ��ʶ=[%d]",
                iExprId));
        return (CHK_FALSE);
    }

    pstRelaCond = pstRelaCondAddr + i;

    memset (aczLValue, 0, sizeof (aczLValue));

    LOG4C(( LOG_DEBUG, "[%d][%d][%d][%s][%d][%d]",pstRelaCond->iLSubSeq, pstRelaCond->iLMsgDscrb,pstRelaCond->iLFldSeq,pstRelaCond->aczLFldType, pstRelaCond->iLStartBit, pstRelaCond->iLEndBit ));

    iResult = CHK_GetVal (pstRelaCond->iLSubSeq, pstRelaCond->iLMsgDscrb,
                pstRelaCond->iLFldSeq, 0,
                pstRelaCond->aczLFldType,
                pstRelaCond->iLStartBit, pstRelaCond->iLEndBit,
                pstSvcList, aczLValue);
    LOG4C(( LOG_DEBUG, "lvaule=[%s][%d]", aczLValue, iResult ));

    memset (aczRValue, 0, sizeof (aczRValue));
    if (pstRelaCond->iConstFlag == 1) {
        strcpy (aczRValue, pstRelaCond->aczMaxValue);
    } else {
        CHK_GetVal (pstRelaCond->iRSubSeq, pstRelaCond->iRMsgDscrb,
                    pstRelaCond->iRFldSeq, 0,
                    pstRelaCond->aczRFldType,
                    pstRelaCond->iRStartBit, pstRelaCond->iREndBit,
                    pstSvcList, aczRValue);
    }

    iLFldTypePri = UDO_GetTypePri (atoi (pstRelaCond->aczLFldType));
    iRFldTypePri = UDO_GetTypePri (atoi (pstRelaCond->aczRFldType));

    if (iLFldTypePri < iRFldTypePri) {
        iChkFldType = atoi (pstRelaCond->aczRFldType);
    } else {
        iChkFldType = atoi (pstRelaCond->aczLFldType);
    }

    iResult = CHK_DataChkRela(aczLValue, aczRValue, iChkFldType,
                              atoi (pstRelaCond->aczRelation));

#ifdef DEBUG3
    LOG4C ((LOG_DEBUG,
            "LSUBSEQ[%d] LD[%d] LFLD[%d] LTYPE[%s] LVAL[%s] "
            "RSUBSEQ[%d] RD[%d] RFLD[%d] RTYPE[%s] RVAL[%s] "
            "ChkType[%d] Ret [%d]",
            pstRelaCond->iLSubSeq, pstRelaCond->iLMsgDscrb, 
            pstRelaCond->iLFldSeq,  pstRelaCond->aczLFldType,aczLValue, 
            pstRelaCond->iRSubSeq, pstRelaCond->iRMsgDscrb, 
            pstRelaCond->iRFldSeq,  pstRelaCond->aczRFldType,aczRValue, 
            iChkFldType, iResult));
#endif

    return (iResult);
}

