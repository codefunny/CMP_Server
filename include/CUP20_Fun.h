
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>

#ifndef __CUP20_FUN_H__
#define __CUP20_FUN_H__

#include "udo.h"
#include "log.h"
#include "pkg_base.h"
#include "pkg_api.h"
#include "ap_global.h"
#include "adp_base.h"
#include "tool_base.h"
#include "apr_pools.h"
#include "apr_network_io.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "HSM_Fun.h"
#include "KeyPub.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	   PINMACKEY_FILE_CUP          "/.pppmmmc"      /*��Կ�ļ�*/
#define	   PINMACKEY_FILE              "/.pppmmm"       /*��Կ�ļ�*/
#define    TRANSKEY_FILE               "/.tttttt"
#define    MASTERKEY_FILE              "/.mmmmmm"
#define    CVVKEYA_FILE                "/.bbbbbb"
#define    CVVKEYB_FILE                "/.aaaaaa"
#define    PSWD_FILE                   "/.pswd"


/* ������궨�� */
#define    SECU_ERR_RWKEY_MODE         -1001            /*�����дģʽ��*/
#define    SECU_ERR_RWKEY_OPEN         -1002            /*��Կ�ļ��򿪴�*/
#define    SECU_ERR_RWKEY_READ         -1003            /*��ȡ��Կ�ļ���*/
#define    SECU_ERR_RWKEY_WRITE        -1004            /*д��Կ�ļ�����*/
#define    SECU_ERR_RWKEY_MODE2        -1005            /*����дģʽ��*/

#define    SECU_ERR_CHGKEY_SECINF      -2001            /*70�������*/
#define    SECU_ERR_CHGKEY_SOFTLOCK    -2002            /*�������*/

#define    SECU_ERR_SNDMSG_GETPKG      -3001            /*ȡ��Ӧ������Ϣ��*/
#define    SECU_ERR_SNDMSG_TOMSG       -3002            /*������Ӧ���Ĵ�*/
#define    SECU_ERR_SNDMSG_SNDBUF      -3003            /*������Ӧ���Ĵ�*/


#define    SECU_ERR_CHKPIN_ERRPIN      -4055            /*����ȷ��PIN*/
#define    SECU_ERR_CHKPIN_SYSERR      -4096            /*ϵͳ�쳣*/
#define    SECU_ERR_CHKPIN_PINFMT      -4099            /*PIN��ʽ��*/

typedef struct
{
   char cFlag;                      /* ��ǰʹ��KEY�±� */
   char aczTime[9];                 /* ����ʱ��(�����ĺ�8λ) */
   char aczKey[2][16];              /* 2��KEY */
} PMKEY;

extern int sendBuffer( char *pczBuffer, apr_size_t tSendLen );

/*
 *  �������ܣ������
 *
*/
int   CUP20_SoftLock ( int id );

/*
 *  �������ܣ�ȡ8583������ĳһ������BITMAP���Ƿ���ں�����1--���� 0--������
 *
*/
int CUP20_GetBitMap( char *pczBM1,char *pczBM2,char *pczBM3, int iFldNo );

/*
 *  �������ܣ��Ա������ļ��е�KEY���ж�д����
 *                       pczMode[0]:   'r' : read ;     'w': write
 *                               1 :   'p' : pin key;   'm': mackey
 *                               2 :   'c' : current key; 'x': other key
 *
*/
int CUP20_PinMacKey( char *pczKey,char *pczMode );

/*
 *  �������ܣ��Ƿ���������       ���أ�1---�� 0---��
 *
*/
int CUP20_IsReqMTI(char *pczMsgTypeId);

/*
 *  �������ܣ��Ƿ���Ҫ����MACУ�� ���أ�1---�� 0--��
 *
*/
int CUP20_IsVerifyMAC( UDP pUdo );

/*
 *  �������ܣ�������Ҫ����MAC����Ĵ�
 *
*/
int CUP20_BuildMACStr( UDP pUdo,char *opczMacStr );

/*
 *  �������ܣ��ڽ��б��Ĳ���iMode=0:����MAC;iMode=1:У��MAC
 *
*/
int CUP20_DesMAC( UDP pUdo,int iMode );

/*
 *  �������ܣ�У������������PIN��ת��Ϊ����PIN
 *
*/
int CUP20_ChkPin( UDP pUdo );

/*
 *  �������ܣ�����PINתDCC PIN���ܴ�����
 *
*/
int CUP20_PinToDCCPin( char *pczSource,char *opczTarg  );

/*
 *  �������ܣ�DCC PINת����PIN���ܴ�����
 *
*/
int CUP20_DCCPinToPin( char *pczSource,char *opczTarg  );

/*
 *  �������ܣ���ȡPIN��Կ
 *
*/
char * CUP20_GetPinKey ( );

/*
 *  �������ܣ�������Կ�滻�ϵ���Կ;
 *            pczNewKey  Ϊ96����
 *            pczSecuInfoΪ53����
 *
*/
int CUP20_ChgKeyFile( char *pczNewKey,char *pczSecuInfo );

/*
 *  �������ܣ�������Կ�滻�ϵ���Կ;
 *            pczNewKey  Ϊ96����
 *            pczSecuInfoΪ53����
 *            pczRetCode Ϊ39����
 *
*/
int CUP20_ChgKey( char *pczNewKey,char *pczSecuInfo,char *pczRetCode );

/*
 *  �������ܣ�ȡԭʼKEY
 *
*/
int CUP20_GetOrgKey( char mfk[] );

/*
 *  �������ܣ���дKEY����
 *
*/
int CUP20_RWDesKey(char *pczKey,char *pczMode);

/*
 *  �������ܣ�����PIN--->DCC PIN��ת���ܺ���
 *
 *  ��ڲ�����pczCupPin: ����PIN
 *            pczAcctNo: ���ʺ�
 *
 *  ���ڲ���: opczDccPin: DCC PIN
 *
*/
int CUP20_CupPinToDccPin( char *pczCupPin,char *pczAcctNo,char *opczDccPin );

/*
 *  �������ܣ�DCC PIN--->����PIN��ת���ܺ���
 *
 *
*/
int CUP20_DccPinToCupPin( char *pczSource, char *pczAcctNo, char *opczTarg  );

/*
 *  �������ܣ����������ʹ�����Ӧ
 *
 *  ��ڲ�����������UDO
 *
*/
int CUP20_SndErrMsg( UDP epfUdo );

extern int HS2B(char *hex, char *bin, int len, char fill);

#ifdef __cplusplus
}
#endif

#endif
