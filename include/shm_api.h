/*
**  file    : shm_api.h
*/

#ifndef __SHM_API_H
#define __SHM_API_H


/*
** ���ܣ��ӹ����ڴ��ж�ȡָ��������
** ���������1 void *epShmAddr �����ڴ��ַ 
**           2 int eiTblSerialNum �����к�
** ���������1 *opiNum ��¼����
** ����ֵ��    �ɹ����ر����ݵ�ַ 
**             ʧ�ܷ��� NULL ����*opiNumΪ -1;
** ˵��������Ϊ��ʱ������ֵ��Ϊ�գ�*opiNum����0
*/
void * Shm_ReadData ( void * epShmAddr, int eiTblSerialNum, int * opiNum );

#endif

/*
** end of file: shm_api.h
*/
