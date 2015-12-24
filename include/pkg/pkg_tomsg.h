/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG__
#define __PKG_TOMSG__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_tomsgbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ��������: ȡ����������״����0�ź��� */
int PKG_GetToMsgTranCode0( UDP epfsAddr, 
                           int eiNodeId, int eiMsgDscrb,
                           char ** opczTranCode );

/* ��������: ���м���Ϣ����ת����������ĵ�0�ź��� */
int PKG_ToMsg0( char * opcOutMsg, int * opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

/* ��������: ���м���Ϣ����ת����������ĵ�1�ź��� */
int PKG_ToMsg1( char * opcOutMsg, int * opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

/* ��������: ���м���Ϣ����ת����������ĵ�1�ź��� */
int PKG_ToMsg2( char * opcOutMsg, int * opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry,  UDP epfAddr );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
