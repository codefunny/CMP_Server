
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


/* 宏定义 */
#define         HSM_NUM         1               /* 加密机数量 */
#define         HSM_TIMEOUT     1               /* 连接超时时间(秒) */

/* 错误码宏定义 */
#define    SECU_ERR_HSM_CONNT          -9001            /*连接加密机失败*/
#define    SECU_ERR_HSM_SEND           -9002            /*发送数据到加密机失败*/
#define    SECU_ERR_HSM_RECV           -9003            /*接收数据到加密机失败*/
#define    MYPINKEY_FILE               "/.mmmppp"   /*网间加密密码存放文件*/
#define    MYPINKEY_FILE_AS               "/.mmmpppas"  /*到400的加密密码存放文件*/

/*
 *  函数功能：同HSM进行通讯,支持多个HSM，
 *            主机名称:hsm1,hsm2,.....
 *
*/
int HSM_TcpTran( char *pczReq,int *piLen,char *pczRsp );

/*
 *  函数功能：产生MAC，以MACkey为密钥对data进行加密运算
 *
*/
int HSM_GenerateMAC( char *pczMACKEY,char *pczData,int iDataLen, char *opczMAC,
                     int iFlag );

/*
 *  函数功能：解密PIN，以PINkey为密钥对data进行加密运算
 *            pczAcctNo 为 0x00+0x00+ 去掉帐号最后一位的12位,8为二进制值
 *
*/
int HSM_GetPIN( char *pczPINKEY,char *opczPinBlock,char *pczAcctNo );

/*
 *  函数功能：产生PIN，以PINkey为密钥对data进行加密运算
 *
*/
int HSM_GeneratePIN( char *pczPINKEY,char *opczPinBlock,char *pczAcctNo );

/*
 *  函数功能：计算密钥的CHECKVALUE
 *
*/
int HSM_CheckValue( char *pczKEY, char *opczCheckValue,
                    int iFlag );


/*
 *  将PINBlock 由ZPK1加密保护转化为ZPK2加密保护
 *  iFlag = 1 单倍长密钥 
 *  iFlag = 2 双倍长密钥
*/
int HSM_ZPK1ToZPK2PIN( char *pczPINKEY1,                     /*ZPK1*/
                       char *pczPINKEY2,                     /*ZPk2*/
                       char *opczPinBlock_1,                 /*PinBlock(ZPK1)*/
                       char *pczSourcePINFormat,             /*转换前PINBlock格式*/
                       char *pczDestPINFormat,               /*转换后PINBlock格式*/
                       char *pczAcctNo,                      /*主帐号去掉校验位的最后12位*/
                       char *opczPinBlock_2,                 /*PinBlock(ZPK2)*/
                       int iFlag1,
                       int iFlag2);

/*
 *  函数功能： 把ZPK密钥从MMK转换到LMK
 *            
 *
*/
int HSM_GetFA( char * zpk ,char * zmk );

/*
 *  函数功能： 把ZAK密钥从MMK转换到LMK
 *            
 *
*/
int HSM_GetFK( char * zpk ,char * zmk );

/*
 *  函数功能： 把ZPK密钥从LMK转换到MMK
 *         zmk  网间密钥加密 zpk lmk加密后数据   
 *
*/
int HSM_GetGC( char * zpk ,char * zmk );

/*
 *  函数功能： 把ZAK密钥从LMK转换到MMK
 *        zmk  网间密钥加密 zpk lmk加密后数据   
 *
*/
int HSM_GetFM( char * zpk ,char * zmk );



#ifdef __cplusplus
}
#endif

#endif
