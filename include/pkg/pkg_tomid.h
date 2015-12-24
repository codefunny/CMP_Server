/*
**  file: pkg_tomid.h
*/

#ifndef __PKG_TOMID__
#define __PKG_TOMID__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_tomidbase.h"

#define PKG_TRANCODE_LEN 128

#ifdef __cplusplus
extern "C" {
#endif

/*
** ��������: ȡ��������Ľ��״����0�ź���
**
** ������:   PKG_GetToMidTranCode0
**
** �������: char * epcOrgMsg                 // Դ����
**           int    eiOrgMsgLen               // Դ���ĳ��� 
**           int    eiNodeId                  // �ڵ��
**           int    eiMsgDscrb                // ����������
**           MsgFldCfgAry * epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                             �����ڼ�¼��
**      
** �������: char ** opczTranCode             // ���״���
**         
** ����ֵ: int      // 0�ɹ�,����ʧ��
**
*/
int PKG_GetToMidTranCode0( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry * epstMsgFldCfgAry ,  
                           char *opczTranCode );

/*
** ��������: ȡ��������Ľ��״����0�ź���
**
** ������:   PKG_GetToMidTranCode1
**
** �������: char * epcOrgMsg                 // Դ����
**           int    eiOrgMsgLen               // Դ���ĳ��� 
**           int    eiNodeId                  // �ڵ��
**           int    eiMsgDscrb                // ����������
**           MsgFldCfgAry * epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                             �����ڼ�¼��
**      
** �������: char ** opczTranCode             // ���״���
**         
** ����ֵ: int      // 0�ɹ�,����ʧ��
** note:just only for shanghai ccb,added by yinzhuhua,2001/11/14
*/
int PKG_GetToMidTranCode1( char * epcOrgMsg, int eiOrgMsgLen, 
                           int eiNodeId, int eiMsgDscrb,
                           MsgFldCfgAry * epstMsgFldCfgAry ,  
                           char *opczTranCode );

/*
**  ��������: ��Դ����ת��Ϊ�м���Ϣ�����0�ź���
**
**  ��������: PKG_ToMid0
**
**  �������: char * epcOrgMsg                // Դ����
**            int    eiOrgMsgLen              // Դ���ĵĳ���
**            int    iConnectId               // ���Ӻ�
**            int    eiNodeId                 // �ڵ��
**            int    eiMsgDscrb               // ����������
**            char * epczTranCode             // ���״���
**            MsgFldCfgAry *epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                             �����¼��
**  �������: UDP opfsAddr                    // �м���Ϣ����
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
*/
int  PKG_ToMid0( char * epcOrgMsg, int eiOrgMsgLen, 
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
		 MsgFldCfgAry *epstmsgFldCfgAry , 
                 UDP opfsAddr );

/*
**  ��������: ��Դ����ת��Ϊ�м���Ϣ�����0�ź���
**
**  ��������: PKG_ToMid0
**
**  �������: char * epcOrgMsg                // Դ����
**            int    eiOrgMsgLen              // Դ���ĵĳ���
**            int    iConnectId               // ���Ӻ�
**            int    eiNodeId                 // �ڵ��
**            int    eiMsgDscrb               // ����������
**            char * epczTranCode             // ���״���
**            MsgFldCfgAry *epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                             �����¼��
**  �������: UDP opfsAddr                    // �м���Ϣ����
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
**  Note:just only for shanghai new genaretion message package,added by yinzhuhua
**  Date:2001/11/14
*/
int  PKG_ToMid2( char * epcOrgMsg, int eiOrgMsgLen, 
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
		 MsgFldCfgAry *epstmsgFldCfgAry , 
                 UDP opfsAddr );

/*
**  ��������: ��Դ����ת��Ϊ�м���Ϣ�����1�ź���(��Ա�׼8583)
**
**  ��������: PKG_ToMid1
**
**  �������: char * epcOrgMsg// Դ����
**            int    eiOrgMsgLen// Դ���ĵĳ���
**            int    iConnectId// ���Ӻ�
**            int    eiNodeId// �ڵ��
**            int    eiMsgDscrb// ����������
**            char * epczTranCode             // ���״���(������*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                              �����¼��
**  �������: UDP opfsAddr          // �м���Ϣ����
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
*/
int  PKG_ToMid1( char * epcOrgMsg, int eiOrgMsgLen,
                 int eiNodeId, char * epczTranCode, int eiMsgDscrb,
		 MsgFldCfgAry *epstmsgFldCfgAry ,
                 UDP opfsAddr );

/*
**  ��������: ��Դ����ת��Ϊ�м���Ϣ�����1�ź���(�������)
**
**  ��������: PKG_ToMidWY
**
**  �������: char * epcOrgMsg// Դ����
**            int    eiOrgMsgLen// Դ���ĵĳ���
**            int    iConnectId// ���Ӻ�
**            int    eiNodeId// �ڵ��
**            int    eiMsgDscrb// ����������
**            char * epczTranCode             // ���״���(������*)
**            MsgFldCfgAry *epstMsgFldCfgAry  // ��ռ���ʼ��ַ
**                                             �����¼��
**  �������: UDP opfsAddr          // �м���Ϣ����
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
*/
int  PKG_ToMidWY( char * epcOrgMsg, int eiOrgMsgLen,
                  int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                  MsgFldCfgAry *epstMsgFldCfgAry ,
                  UDP opfsAddr );
                  
                  

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomid.h
*/
