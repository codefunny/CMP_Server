/*
**file:pub_cfg.h
*/

#ifndef __PUB_CFG_H__
#define __PUB_CFG_H__

typedef struct _NodeInitCfg
{
   char aczNodeName[41];  /*ʵ�����ǽ�����                            */
   int  iNodeId;          /*�����ڵ��                                */
   int  iMsgAppId;        /*��������Ӧ�ú�                            */
   int  iShmIpcKey;       /*������Ϣ��Ź����ڴ�IPCKEY                */
   int  iTag;             /*��־,һ��ڵ�û����,������������BTOB��BTOC*/
} NodeInitCfg;

typedef struct _NodePros
{
   int iChannelNum;
   NodeInitCfg stNodeInitCfg[200];
} NodePros;

/*
**����:���ļ��еĶ�����������Ϣ
**����:1 �ļ�ָ��
**���:1 ������Ϣ�ṹ
**����   ��
*/
void PUB_Read( FILE *efp, NodePros *epstNodePros );

/*
**����:�����������ֺͱ�־ȡ�ڵ�ID�Ź����ڴ�IPCKEY�ͽڵ㱨��Ӧ�ú�
**����:1 �����ڵ���
**     2 ��־(һ��ڵ�Ϊ0,����BTOCΪ0,����BTOBΪ1)
**���:1 �����ڵ��
**     2 ����������Ϣ�����ڴ�IPCKEY
**     3 �����ڵ㱨��Ӧ�ú�
*/
int PUB_GetValByName( char *epcChanlName, int eiTag,
                      int *opiNodeId, int *opiShmIpcKey,
                      int *opiMsgAppId );

#endif
/*
** end of file
*/
