/******************************************************************************

                  版权所有 (C), 2014-2016, 北京永超科技有限公司

 ******************************************************************************
  文 件 名   : pkg_tomsg_allinpay.h
  版 本 号   : 初稿
  作    者   : 任辉
  生成日期   : 2015年2月26日
  最近修改   :
  功能描述   : pkg_tomsg_allinpay.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年2月26日
    作    者   : 任辉
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#ifndef __PKG_TOMSG_TLV_H__
#define __PKG_TOMSG_TLV_H__

#include "pkg_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int PKG_ToMsg_tlv( char * opcOutMsg, int *opiOutMsgLen,
                               int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                               MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PKG_TOMSG_ALLINPAY_H__ */
