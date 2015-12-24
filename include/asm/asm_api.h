/*
**  file    : asm_api.h
*/

#include "asm/asm_base.h"

#ifndef __ASM_API_H__
#define __ASM_API_H__

#define ADDASM( funname ) { "ASM_"#funname, ASM_##funname }

typedef struct
{
   char *OperaName;
   int  (*OperaFun)( SVC_Content *, UDP, int , int, UDP);
} OperaFunEntity;

#ifdef __cplusplus
extern "C" {
#endif

/*
**  ��������: �����ӷ�����������
**  ������:   CreateSubReqList
**  �������: 1 �����
**            2 �ӷ������
**            3 �Ƿ������ʶ
**            4 ������Ϣ����
**  �������: �������ӷ�����������
**            ������Ϣ������ֻ������������Ϣ����
**  ����ֵ:   �ɹ�����0������ֵʧ��
**  ˵��:     
*/
int  ASM_CreateSubReqList( int eiSvcID, int eiSubSeq, int eiBalFlag,
                           SVC_Content * epstService, UDP opstSubReqPkgList,
                           char *opcErrorMsg );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: asm_api.h
*/
