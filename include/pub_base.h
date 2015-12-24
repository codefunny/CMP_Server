/*
**  file: pub_base.h
*/
#include <sys/time.h>
#include <unistd.h>

#ifndef __PUB_BASE__
#define __PUB_BASE__

#ifdef __cplusplus
extern "C" {
#endif

/* ȥ�ַ������ָ���ַ���pczSrc����'\0'�������ַ��� */
void PUB_StrLeftTrim( char * epczStr, char ecCh );

/* ȥ�ַ����ұ�ָ���ַ���pczSrc����'\0'�������ַ��� */
void PUB_StrRightTrim( char * epczStr, char ecCh );

/* ȥ�ַ�����������ָ���ַ���pczSrc����'\0'�������ַ��� */
void PUB_StrTrim( char * epczStr, char ecCh );

/*
** ���ܣ��ַ���ȫ�����д
** ���������1 ������������ַ���
** ���������1 �����д�ַ���
** ����ֵ��    ��
*/
void PUB_StrUpper( char *eopczStr );

/*
** ���ܣ��ַ���ȫ����Сд
** ���������1 ������������ַ���
** ���������1 ���Сд�ַ���
** ����ֵ��    ��
*/
void PUB_StrLower( char *eopczStr );

void PUB_UpSort( int eiNum, int *piArray );

long PUB_CalcTimeVal( struct timeval stTimeBegin, struct timeval stTimeEnd );

/**
 * ����: �ַ������ָ����ֽ���ַ���ʸ��
 * ����: char * epczStr  - ���ֽ��ַ���
 *char   chSep    - �ָ���
 *char **oppczRet - �ַ���ָ������,ָ���ʸ���ַ���
 *u_int  max_flds - ���ֽ�ʸ����,Ϊ0��ȫ�ֽ������ַ���
 * ����: ʸ������
 * ��ע: ע��,�˺����Ὣԭ�ַ������л�max_flds���ָ����滻���ַ�'\0'
**/
int PUB_ParseStr(char *epczStr, char chSep, char *oppczRet[],
                 unsigned int max_flds);


#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pub_base.h
*/
