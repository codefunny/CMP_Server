/*
**  file: pkg_tomsgbase.h
*/

#ifndef __PKG_TOMSGBASE__
#define __PKG_TOMSGBASE__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ��������: ����BitMap���ض�λ,������Ϊ1 */
int PKG_SetBitMap( unsigned char * eupczBitMap, int eiNum );

/* ��������: ���������ͽ�ֵд��Buf */
int PKG_WriteValToBuf( char * opcBuf, int eiDataType, 
                       char * epczVal, int eiValLen, short eshVal, int eiVal, 
                       long elVal, double edVal, float efVal, 
                       int * opiValContentLen );


/* ��������:����eiSepFldSeq���������Ϣд��������� */
/* �ú����е�opiSepMoveLenֻ�е�һ�����з���ʱ��������,�˴ο���û���� */
int PKG_WriteSepFldContentToMsg( char * opcMsgPos, int * eopiMoveLen,
                                 FldCfgValNode estFldCfgValNode,
                                 char * epczVal, int eiValLen, short eshVal, 
                                 int eiVal, long elVal, double edVal, 
                                 float efVal, int * opiSepMoveLen );

/* ��������: ������Ϣд��������� */
int PKG_WriteFldContentToMsg( char * opcMsgPos, int * eopiMoveLen, 
                              int eiMsgAppId, int eiFldSeq, int eiRepNum,
                              MsgFldCfgAry *epstMsgFldCfgAry,
                              UDP epfsAddr ); 

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsgbase.h
*/
