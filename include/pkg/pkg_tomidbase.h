/*
**  file: pkg_tomidbase.h
*/

#ifndef __PKG_TOMIDBASE__
#define __PKG_TOMIDBASE__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_base.h"

typedef union _PKG_RawData
{
   short  shVal;
   int    iVal;
   long   lVal;
   float  fVal;
   double dVal;
   char  *pcVal;
} PKG_RawData;

typedef struct _PKG_DataValue
{
   int iType;
   int iLen;
   PKG_RawData nData;
} PKG_DataValue;

#ifdef __cplusplus
extern "C" {
#endif

/* ��������: ��ȡBitMap���ض�λ,�����Ƿ�Ϊ1 */
int PKG_IsBitMapSet( unsigned char * epczBitMap, int eiNum );


/* ��������: �����ڽṹ����һ����֮ǰ��ƫ���� */
void PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                             int * opiPtrMoveLen ); 

/* ��������: ����һ���ظ�������ظ����� */
int PKG_GetRepTimesVal( UDP epfAddr, int eiRepFldNum,
                        char * epczRepTimesSeq, char * epczRepTimesNum, 
                        int * opiRepTimesVal );


/* ��������: �����������ʹ�Դ������ȡֵ */
/**********
int PKG_GetValFromBuf( char * epcBuf, int eiDataType, int eiValLen,
                       char ** opczVal, short * opshVal, int * opiVal, 
                       long * oplVal, double * opdVal, float * opfVal, 
                       int * opiValContentLen );
**********/
int PKG_GetValFromBuf( char * epcBuf, int eiDataType, int eiValLen,
                       PKG_DataValue *opstDataVal );

/* ��������: ���������Ա�־,ȡ�ò�ͬ���ֵ */
/* �ú����е�opiSepMoveLenֻ�е�һ�����з���ʱ��������,�˴ο���û���� */
/***********
int PKG_GetSepFldContentFromMsg( char * epcMsgPos, int * eopiMoveLen, 
                                 int * eopiMsgLeftLen,
                                 FldCfgValNode estFldCfgValNode,
                                 char ** opczVal, int * opiValLen, 
                                 short * opshVal, int * opiVal, long * oplVal, 
                                 double * opdVal, float * opfVal, 
                                 int * opiSepMoveLen ); 
***********/
int PKG_GetSepFldContentFromMsg( char * epcMsgPos, int * eopiMoveLen, 
                                 int * eopiMsgLeftLen,
                                 MsgFldCfg *epstMsgFldCfg,
				 PKG_DataValue *opstDataVal,
				 int * opiSepMoveLen );

/* ��������: ��eiSeq�����eiSepSeq�ŷ����ֵ�������Ͳ�������(Ŀǰ��֧�ַ���) */
/********
int PKG_AddSepFldContentToLST( int eiSeq, int eiSepSeq, int eiDataType, 
                               char * epczVal, int eiValLen, short eshVal, 
                               int eiVal, long elVal, double edVal, 
                               float efVal, UDP opfsAddr );
********/
int PKG_AddSepFldContentToLST( int eiSeq, int eiSepSeq, int eiDataType,
                               PKG_DataValue *pstDataValue,
                               UDP opfsAddr );
/*
**  ��������: ��eiSeq������Ϣ��������
**
**  ������:  PKG_AddFldContentToLST
**
**  �������: char * epcMsgPos            // �Ѿ�ָ��ָ��eiFldSeq�����׵�Դ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int  * eopiMsgLeftLen       // ����ָ����β���ĵĳ���
**            int    eiSeq                // ���
**            MsgFldCfgAry *epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                          �����¼��
**
**  �������: UDP opfsAddr      // �м���Ϣ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int * eopiMsgLeftLen
**
**  ����ֵ:   int 0�ɹ�, ����ʧ�� 
*/

int  PKG_AddFldContentToLST( char * epcMsgPos, int * eopiMoveLen, 
                             int * eopiMsgLeftLen, int eiMsgAppId,
			     int eiSeq, MsgFldCfgAry *epstMsgFldCfgAry,
                             UDP opfsAddr );


#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomidbase.h
*/
