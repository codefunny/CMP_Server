
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#ifndef __HSM_FUN_H__
#define __HSM_FUN_H__

#include "log.h"
#include "tool_base.h"
#include "ap_base.h"
#include "pub_base.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_errno.h"
#include "apr_general.h"

#ifdef __cplusplus
extern "C" {
#endif


/* �궨�� */
#define         HSM_NUM         1               /* ���ܻ����� */
#define         HSM_TIMEOUT     1               /* ���ӳ�ʱʱ��(��) */

/* ������궨�� */
#define    SECU_ERR_HSM_CONNT          -9001            /*���Ӽ��ܻ�ʧ��*/
#define    SECU_ERR_HSM_SEND           -9002            /*�������ݵ����ܻ�ʧ��*/
#define    SECU_ERR_HSM_RECV           -9003            /*�������ݵ����ܻ�ʧ��*/
#define    MYPINKEY_FILE               "/.mmmppp"   /*��������������ļ�*/
#define    MYPINKEY_FILE_AS               "/.mmmpppas"  /*��400�ļ����������ļ�*/

/*
 *  �������ܣ�ͬHSM����ͨѶ,֧�ֶ��HSM��
 *            ��������:hsm1,hsm2,.....
 *
*/
int HSM_TcpTran( char *pczReq,int *piLen,char *pczRsp );

/*
 *  �������ܣ�����MAC����MACkeyΪ��Կ��data���м�������
 *
*/
int HSM_GenerateMAC( char *pczMACKEY,char *pczData,int iDataLen, char *opczMAC,
                     int iFlag );

/*
 *  �������ܣ�����PIN����PINkeyΪ��Կ��data���м�������
 *            pczAcctNo Ϊ 0x00+0x00+ ȥ���ʺ����һλ��12λ,8Ϊ������ֵ
 *
*/
int HSM_GetPIN( char *pczPINKEY,char *opczPinBlock,char *pczAcctNo );

/*
 *  �������ܣ�����PIN����PINkeyΪ��Կ��data���м�������
 *
*/
int HSM_GeneratePIN( char *pczPINKEY,char *opczPinBlock,char *pczAcctNo );

/*
 *  �������ܣ�������Կ��CHECKVALUE
 *
*/
int HSM_CheckValue( char *pczKEY, char *opczCheckValue,
                    int iFlag );


/*
 *  ��PINBlock ��ZPK1���ܱ���ת��ΪZPK2���ܱ���
 *  iFlag = 1 ��������Կ 
 *  iFlag = 2 ˫������Կ
*/
int HSM_ZPK1ToZPK2PIN( char *pczPINKEY1,                     /*ZPK1*/
                       char *pczPINKEY2,                     /*ZPk2*/
                       char *opczPinBlock_1,                 /*PinBlock(ZPK1)*/
                       char *pczSourcePINFormat,             /*ת��ǰPINBlock��ʽ*/
                       char *pczDestPINFormat,               /*ת����PINBlock��ʽ*/
                       char *pczAcctNo,                      /*���ʺ�ȥ��У��λ�����12λ*/
                       char *opczPinBlock_2,                 /*PinBlock(ZPK2)*/
                       int iFlag1,
                       int iFlag2);

/*
 *  �������ܣ� ��ZPK��Կ��MMKת����LMK
 *            
 *
*/
int HSM_GetFA( char * zpk ,char * zmk );

/*
 *  �������ܣ� ��ZAK��Կ��MMKת����LMK
 *            
 *
*/
int HSM_GetFK( char * zpk ,char * zmk );

/*
 *  �������ܣ� ��ZPK��Կ��LMKת����MMK
 *         zmk  ������Կ���� zpk lmk���ܺ�����   
 *
*/
int HSM_GetGC( char * zpk ,char * zmk );

/*
 *  �������ܣ� ��ZAK��Կ��LMKת����MMK
 *        zmk  ������Կ���� zpk lmk���ܺ�����   
 *
*/
int HSM_GetFM( char * zpk ,char * zmk );



#ifdef __cplusplus
}
#endif

#endif
