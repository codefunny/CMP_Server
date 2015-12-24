/*
** file:   svc_basic.h
*/
#include "svc_base.h"

#ifndef __SERVICE_CONTROL_H__
#define __SERVICE_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** ���ܣ�׼��������صĲ�����ͬʱ���õ�0���ӷ���
** ���������1 ������������
**           2 �����ڵ�
**           3 ����Ӧ�ú�
**           4 ���״���
**           5 ������KEYֵ
**           6 д����KEYֵ
** ���������1 ������Ӧ����
**           2 ����ָ��
** ����ֵ��    0�ɹ�������ʧ��
*/
int SVC_Prepare( UDP epfReqList, int eiChanId, int eiPkgId,
                 char *epczTranCode, SVC_Content *opstService );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
