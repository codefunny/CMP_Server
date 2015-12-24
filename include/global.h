/*
** file: global.h
*/
#include <sys/time.h>
/*
#include <rpc/rpc.h>
*/
#ifndef __GLOBAL__
#define __GLOBAL__
#define CODEING

#define DATANOTFOUND 1403 /*FOR ORACLE*/

/*�������ݿ����ӵ����Ӻ�*/
extern int iConnectId;

/*��������������������;��Ŀǰ������ʶ֤ȯ����*/
extern int iReserveVal;

/**�������õĹ����ڴ�ָ��**/
extern char * pcShmAddr;

extern char gaczTranCode[10];

/****����������ˮ��****/
extern char gaczAtmSerial[7];

/****�������������豸��ˮ��****/
extern char gaczDevSerial[11];

/****���ս��׵ڶ��ʺ�****/
extern char gaczSecAcct[29];

/* �豸��� */
extern char gaczDevNo[5];

#define DBUSER  ""
#define PASSWD  ""

#define PKG_REQ    1
#define PKG_RESP   2
#define PKG_CONF   3


extern struct timeval stBegin, stCurr;
extern struct timeval stPkgBegin, stPkgEnd;
extern long   lTotalTime;
extern int    iSubSvcTime;

/*
** ��������Aix��SCO��������ת���ı�־
** 0-����ת��, 1-��Ҫת��
** �ñ�־�ڴ����������ж�ȡ���������������ã����ǰ��Ϊ1�����������Ϊ0
*/
extern int iScoFlag;

/* ���ڶ�����Ҫת��Ϊ�����ֽ����ݵĽ��п���
** 0-����ת��, 1-��Ҫת��
** �ñ�־�ڴ����������ж�ȡ���������������ã����ǰ��Ϊ1�����������Ϊ0 
** �������ݲ�֧��
*/
extern int iNetTranFlag;
extern char  aczDwbhCheck[7];

#endif

/*
** end of file: global.h
*/

