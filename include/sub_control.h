/*
** file:sub_control.h
*/

#ifndef __SUB_CONTROL_H__
#define __SUB_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** ���ܣ������ӷ�������
** ���������1 ����ָ��
** ���������1 ����ָ��
** ����ֵ��    0��������������
*/
int SUB_StartFlow( SVC_Content *eopstService );

/*
** ���ܣ������ӷ������������
** ���������1 ����ָ��
** ���������1 ����ָ��
** ����ֵ��    0������1-����(�����������鷢������,������������)
*/
int SUB_ErrorFlow( SVC_Content *eopstService );

int SUB_bsearchMsgFldCfg( const void *pLeft, const void *pRight );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file
*/
