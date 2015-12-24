/*
**  file    : asm_base.h
*/

#ifndef __ASM_BASE_H__
#define __ASM_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "global.h"
#include "fldanaly.h"
#include "tool_base.h"
#include "svc_base.h"
#include "pub_base.h"
#include "tool_datetime.h"
#include "udo.h"
#include "shm_base.h"

#define ASM_UDPSIZE 24576 /*24K*/
#define ASM_DATESIZE 14
#define MAX_STR_NODE_LEN 4096

#define ASM_SUCC 0
#define ASM_FAIL 501
#define ASM_NO_MEM 502
#define ASM_CRT_REQLIST_ERR 503
#define ASM_CRT_CFGLIST_ERR 504
#define ASM_TYPE_ERR 505
#define ASM_SRCTYPE_ERR 506
#define ASM_GetSrc_FAIL 507
#define ASM_SQL_FAIL    508

/* �ӷ����������������Ϣ�Ľṹ */
typedef struct _ReqCfgNode
{
   int        iFldSeq;          /*�ӷ���������        */
   int        iFldType;         /*�ӷ�����������        */
   int        iRepFldFlag;      /*�ظ����ʶ              */
   int        iRepTmSubSeq;     /*�ظ������������ӷ������*/
   int        iRepTmFld;        /*�ظ�����ظ����������  */
   int        iRepTmType;       /*�ظ����������������    */
   int        iRepTmMsgDscrb;   /*�ظ�������������      */
   int        iFldFragCalFlag;  /*����Ƭ��������          */ 
   int        iFldAsmId;        /*������ID                */
}ReqCfgNode;

/* �ӷ�������Դ������Ϣ�Ľṹ */
typedef struct _FldSrcCfgNode
{
   int        iFldFragSeq;      /*�ӷ��������ڱ��*/
   int        iSrcInfType;      /*��Դ��Ϣ����*/
   int        iSrcSubSeq;       /*��Դ�ӷ����ڲ����*/
   int        iSrcFldSeq;       /*��Դ�ӷ���������*/
   int        iSrcRepFldFlag;   /*��Դ�ظ����ʶ*/
   int        iSrcBeginPos;     /*��ʼλ��*/
   int        iSrcLength;       /*��ʼ����*/
   char       aczSrcNote[41];   /*������Ϣ��Դ*/ 
}FldSrcCfgNode;

/*****************************************
typedef struct _ReqCfgList
{
   ReqCfgNode stReqCfgNode;
   struct _ReqCfgList * pNext;
}ReqCfgList;

typedef struct _FldSrcCfgList
{
   FldSrcCfgNode stFldSrcCfgNode;
   struct _FldSrcCfgList * pNext;
}FldSrcCfgList;
*****************************************/

/* �ӷ����������������Ϣ���� */
typedef struct _ReqCfgAry
{
   int iNum;
   ReqCfgNode * pstReqCfgNode;
}ReqCfgAry;

/* �ӷ�������Դ������Ϣ���� */
typedef struct _FldSrcCfgAry
{
   int iNum;
   FldSrcCfgNode * pstFldSrcCfgNode;
}FldSrcCfgAry;

#ifdef __cplusplus
extern "C" {
#endif

/*
**  ��������: �����ӷ����������������Ϣ����
**  ������:   ASM_CreateReqCfgAry
**  �������: 1 �����
**            2 �ӷ������
**            3 �Ƿ������ʶ
**  �������: �ӷ����������������Ϣ����ͷ
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_CreateReqCfgAry( int eiSvcId, int eiSubSeq, int eiBalFlag,
                           ReqCfgAry * opstReqCfgAry );




/*
**  ��������: ��һ������Դ������Ϣ��������
**  ������:   ASM_AddToFldSrcCfgAry
**  �������: �ӷ�������Դ������Ϣ����ͷ
**            �ӷ�������Դ������Ϣ�ڵ�
**  �������: �ӷ�������Դ������Ϣ����ͷ
**  ����ֵ: int 0 �ɹ�������ʧ��
*/
int  ASM_AddToFldSrcCfgAry( FldSrcCfgAry * eopstFldSrcCfgAry,
                            FldSrcCfgNode * epstFldSrcCfgNode );

/*
**  ��������: �����ӷ���Ŷ�λ��һ������
**  ������:   ASM_GetSrcList
**  �������: ��Դ�ӷ������
**            ������Ϣ����
**            �ӷ�����Ӧ���������־,1-����,2-��Ӧ
**  �������: ��
**  ����ֵ:   �ӷ�����Ӧ����
*/
UDP ASM_GetSrcList( int eiSrcSubSeq, SVC_Content * epstService,
                    int eiDirectFlag );


/*
**  ��������: ��ӡ�ӷ����������������Ϣ�������������ݵ���׼���
**  ������:   ASM_ReqCfgAryDisplay
**  �������: �ӷ����������������Ϣ�����׵�ַ
**  �������: ��
**  ����ֵ:   ��
*/
void ASM_ReqCfgAryDisplay( ReqCfgAry * epstReqCfgAry );


/*
**  ��������: �����ӷ�������Դ������Ϣ����
**  ������:   ASM_CreateFldSrcCfgAry
**  �������: ������Id��
**  �������: �ӷ�������Դ������Ϣ����ͷ
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_CreateFldSrcCfgAry( int eiFldAsmId, 
                             FldSrcCfgAry * opstFldSrcCfgAry );

/*
**  ��������: ȡ���ظ�����
**  ������:   ASM_GetRepTime
**  �������: 1 ������Ϣ����
**            2 ����������Ϣ����ڵ�
**  �������: �ظ�����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_GetRepTime( SVC_Content * epstService, 
                    ReqCfgNode * epstReqCfgNode,
                    int * opiRepTime );


/*
**  ��������: ������Դ��ֵ����
**  ������:   ASM_CreateFldSrcValList
**  �������: 1 ������Ϣ����
**            2 ����Դ������Ϣ����
**            3 �ظ����
**  �������: 1 ��Դ��ֵ����ڵ���
**            2 ��Դ��ֵ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_CreateFldSrcValList( SVC_Content * epstService, 
                             FldSrcCfgAry * pstFldSrcCfgAry,
                             int iRepSerial, 
                             int * opiNum,
                             UDP opcSrcList ); 

/*
**����:����EasyLink������õ����񷵻���
**����:1 ����������ڵ���
**     2 EasyLink������ֵ(���ַ�����ʽ�ṩ,�ɵ������Լ�����ת��)
**���:1 ���񷵻���
**���� 0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/
int ASM_GetCodeByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetCode );

/*
**����:����EasyLink������õ����񷵻���Ϣ
**����:1 ����������ڵ���
**     2 EasyLink������ֵ(���ַ�����ʽ�ṩ,�ɵ������Լ�����ת��)
**���:1 ���񷵻���
**���� 0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/
int ASM_GetMesgByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetMesg );

/*
**����:������������,������ȷ��SQL���
**����:1 ��������
**���:1 SQL���
**����:0-��ȷ,��������
*/
int ASM_CreateSql( UDP epcSrcList, char *opczSqlStr );

/*
**����: ȡ����������Ӧ������,����֧��FT_INT,FT_SHORT,FT_LONG,FT_STRING,FT_FLOAT
        ��FT_DOUBLE
**����: 1 ��������
**���:   ��
**����    ������
*/
int ASM_GetFieldType( char *epczFldDesc );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: asm_base.h
*/
