
/*
**  file: pkg_api.h
*/

#ifndef __PKG_API__
#define __PKG_API__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_tomid.h"
#include "pkg_tomsg.h"

/* ������ȡ���״��뺯��ָ��ṹ */
typedef struct 
{
   char * pczFunName;
   int    (*pfnFun)( char * epcOrgMsg, int eiOrgMsgLen, 
                     int eiNodeId, int eiMsgDscrb, 
                     MsgFldCfgAry *epstMsgFldCfgAry,
                     char *opczTranCode );
}PKG_GET_TOMID_TRANCODE;

/* ����������ָ�� */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( char * epcOrgMsg, int eiOrgMsgLen,
                     int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
                     MsgFldCfgAry *epstMsgFldCfgAry, UDP opsAddr );
}PKG_TOMID;

/* ������ȡ���״��뺯��ָ��ṹ */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( UDP epfsAddr,
                     int eiNodeId, int eiMsgDscrb,
                     char ** opczTranCode ); 
}PKG_GET_TOMSG_TRANCODE;

/* ����������ָ��ṹ */
typedef struct
{
   char * pczFunName;
   int    (*pfnFun)( char * opcOutMsg, int * opiOutMsgLen,
                     int eiNodeid, char * epczTranCode, int eiMsgDscrb,  
                     MsgFldCfgAry *pstMsgFldCfgAry, UDP epsAddr );
}PKG_TOMSG;

#ifdef __cplusplus
extern "C" {
#endif

/* ��������: �õ����к�����Id�� */
int  PKG_GetFunId( int eiNodeId, 
                   int * opiToMidTranCodeId, int * opiToMsgTranCodeId, 
                   int * opiToMidId, int * opiToMsgId );

/* ��������: ȡ�ò���������״��� */
int PKG_GetToMidTranCode( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen,
                          int eiNodeId, int eiMsgDscrb, 
                          MsgFldCfgAry * opstMsgFldCfgAry,
                          char *opczTranCode );

/* ��������: ��Դ����ת��Ϊ�м���Ϣ���� */
int  PKG_ToMid( int eiFunId, char * epcOrgMsg, int eiOrgMsgLen, int eiNodeId,
                char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry, UDP opfsAddr );

/* ��������: ȡ�ô���������״��� */
int  PKG_GetToMsgTranCode( int eiFunId, UDP epfsAddr,  
                           int eiNodeId, int eiMsgDscrb, char ** opczTranCode );

/* ��������: ���м���Ϣ����ת��Ϊ������� */
int  PKG_ToMsg( int eiFunId, char * opcOutMsg, int * opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry *epstMsgFldCfgAry, UDP epfsAddr );

int PKG_Disp8583Pkg (char * pczTitle, char * pczBuffer, int iPktLen);

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_api.h
*/
