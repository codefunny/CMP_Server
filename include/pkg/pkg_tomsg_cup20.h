/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG_CUP20__
#define __PKG_TOMSG_CUP20__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_base.h"
#include "pkg_base_cup20.h"

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_8583_1( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );


/*��8583 �����в�����Ӧ�����ְ  
epaData 8583���ݰ�   
eiFieldid ȡ����id
opaczData �������
epaczFiledLen ��ĳ���4���ַ����ȣ������һ���ַ�ΪB��Ϊ�䳤����Ϊ�䳤�ַ�����  */
int special8583getfiled(char * epaData ,int eiFieldid,char * opaczData ,char * epaczFiledLen);


#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
