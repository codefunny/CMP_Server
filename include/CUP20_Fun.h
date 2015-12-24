
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

#define	   PINMACKEY_FILE_CUP          "/.pppmmmc"      /*密钥文件*/
#define	   PINMACKEY_FILE              "/.pppmmm"       /*密钥文件*/
#define    TRANSKEY_FILE               "/.tttttt"
#define    MASTERKEY_FILE              "/.mmmmmm"
#define    CVVKEYA_FILE                "/.bbbbbb"
#define    CVVKEYB_FILE                "/.aaaaaa"
#define    PSWD_FILE                   "/.pswd"


/* 错误码宏定义 */
#define    SECU_ERR_RWKEY_MODE         -1001            /*输入读写模式错*/
#define    SECU_ERR_RWKEY_OPEN         -1002            /*密钥文件打开错*/
#define    SECU_ERR_RWKEY_READ         -1003            /*读取密钥文件错*/
#define    SECU_ERR_RWKEY_WRITE        -1004            /*写密钥文件出错*/
#define    SECU_ERR_RWKEY_MODE2        -1005            /*输入写模式错*/

#define    SECU_ERR_CHGKEY_SECINF      -2001            /*70号域错误*/
#define    SECU_ERR_CHGKEY_SOFTLOCK    -2002            /*软加锁错*/

#define    SECU_ERR_SNDMSG_GETPKG      -3001            /*取响应配置信息错*/
#define    SECU_ERR_SNDMSG_TOMSG       -3002            /*生成响应报文错*/
#define    SECU_ERR_SNDMSG_SNDBUF      -3003            /*发送响应报文错*/


#define    SECU_ERR_CHKPIN_ERRPIN      -4055            /*不正确的PIN*/
#define    SECU_ERR_CHKPIN_SYSERR      -4096            /*系统异常*/
#define    SECU_ERR_CHKPIN_PINFMT      -4099            /*PIN格式错*/

typedef struct
{
   char cFlag;                      /* 当前使用KEY下标 */
   char aczTime[9];                 /* 更换时间(秒数的后8位) */
   char aczKey[2][16];              /* 2个KEY */
} PMKEY;

extern int sendBuffer( char *pczBuffer, apr_size_t tSendLen );

/*
 *  函数功能：软件锁
 *
*/
int   CUP20_SoftLock ( int id );

/*
 *  函数功能：取8583报文中某一号域在BITMAP中是否存在函数，1--存在 0--不存在
 *
*/
int CUP20_GetBitMap( char *pczBM1,char *pczBM2,char *pczBM3, int iFldNo );

/*
 *  函数功能：对保存在文件中的KEY进行读写处理
 *                       pczMode[0]:   'r' : read ;     'w': write
 *                               1 :   'p' : pin key;   'm': mackey
 *                               2 :   'c' : current key; 'x': other key
 *
*/
int CUP20_PinMacKey( char *pczKey,char *pczMode );

/*
 *  函数功能：是否是请求报文       返回：1---是 0---否
 *
*/
int CUP20_IsReqMTI(char *pczMsgTypeId);

/*
 *  函数功能：是否需要进行MAC校验 返回：1---是 0--否
 *
*/
int CUP20_IsVerifyMAC( UDP pUdo );

/*
 *  函数功能：构造需要进行MAC运算的串
 *
*/
int CUP20_BuildMACStr( UDP pUdo,char *opczMacStr );

/*
 *  函数功能：在进行报文拆解后，iMode=0:生成MAC;iMode=1:校验MAC
 *
*/
int CUP20_DesMAC( UDP pUdo,int iMode );

/*
 *  函数功能：校验银联过来的PIN并转化为本机PIN
 *
*/
int CUP20_ChkPin( UDP pUdo );

/*
 *  函数功能：银联PIN转DCC PIN加密处理函数
 *
*/
int CUP20_PinToDCCPin( char *pczSource,char *opczTarg  );

/*
 *  函数功能：DCC PIN转银联PIN加密处理函数
 *
*/
int CUP20_DCCPinToPin( char *pczSource,char *opczTarg  );

/*
 *  函数功能：获取PIN密钥
 *
*/
char * CUP20_GetPinKey ( );

/*
 *  函数功能：用新密钥替换老的密钥;
 *            pczNewKey  为96号域
 *            pczSecuInfo为53号域
 *
*/
int CUP20_ChgKeyFile( char *pczNewKey,char *pczSecuInfo );

/*
 *  函数功能：用新密钥替换老的密钥;
 *            pczNewKey  为96号域
 *            pczSecuInfo为53号域
 *            pczRetCode 为39号域
 *
*/
int CUP20_ChgKey( char *pczNewKey,char *pczSecuInfo,char *pczRetCode );

/*
 *  函数功能：取原始KEY
 *
*/
int CUP20_GetOrgKey( char mfk[] );

/*
 *  函数功能：读写KEY函数
 *
*/
int CUP20_RWDesKey(char *pczKey,char *pczMode);

/*
 *  函数功能：银联PIN--->DCC PIN的转加密函数
 *
 *  入口参数：pczCupPin: 银联PIN
 *            pczAcctNo: 主帐号
 *
 *  出口参数: opczDccPin: DCC PIN
 *
*/
int CUP20_CupPinToDccPin( char *pczCupPin,char *pczAcctNo,char *opczDccPin );

/*
 *  函数功能：DCC PIN--->银联PIN的转加密函数
 *
 *
*/
int CUP20_DccPinToCupPin( char *pczSource, char *pczAcctNo, char *opczTarg  );

/*
 *  函数功能：向银联发送错误响应
 *
 *  入口参数：请求报文UDO
 *
*/
int CUP20_SndErrMsg( UDP epfUdo );

extern int HS2B(char *hex, char *bin, int len, char fill);

#ifdef __cplusplus
}
#endif

#endif
