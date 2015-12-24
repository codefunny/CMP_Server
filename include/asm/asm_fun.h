/*
**  file  : asm_fun.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "global.h"
#include "tool_base.h"
#include "svc_base.h"
#include "pub_base.h"
#include "tool_datetime.h"
#include "udo.h"
#include "shm_base.h"

#define ASM_DATESIZE 14
#define ASM_SUCC 0

#ifndef __ASM_FUN_H__
#define __ASM_FUN_H__

#ifdef __cplusplus
extern "C" {
#endif

int  ASM_Sample( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );
/*
**  函数功能: 将来源域值拷贝到目标域
**  函数名:   ASM_FldCopy
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_FldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照字符串累加的方式生成目标域
**  函数名:   ASM_AddStr
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddStr( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照短整形相加的方式生成目标域
**  函数名:   ASM_AddShort
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddShort( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照整形相加的方式生成目标域
**  函数名:   ASM_AddInt
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddInt( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照长整形相加的方式生成目标域
**  函数名:   ASM_AddLong
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddLong( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照浮点形相加的方式生成目标域
**  函数名:   ASM_AddFloat
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddFloat( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值按照双精度型相加的方式生成目标域
**  函数名:   ASM_AddDouble
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_AddDouble( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                    int eiFldSeq, UDP opcResultList );

/*
**功能: 取系统时间(HHMMSS)
**输入: 1 服务信息
**      2 来源域值信息链表
**      3 报文应用号
**      4 报文域序号
**输出:1 转换结果
**返回 0-成功,其他-失败
*/
int  ASM_SysTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );


/*
**功能: 取系统日期(YYYYMMDD)
**输入: 1 服务信息
**      2 来源域值信息链表
**      3 报文应用号
**      4 报文域序号
**输出:1 转换结果
**返回 0-成功,其他-失败
*/
int  ASM_SysDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
		  int eiFldSeq, UDP opcResultList );

/**
 * 用于日期转换 YYYY-MM-DD To YYYYMMDD
 * 参数说明：
 * epsSrcList    输入日期 YYYY-MM-DD
 * opcResultList 输出日期 YYYYMMDD
 */
int  ASM_ChangeDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
 * 用于时间转换 HH.MM.SS To HHMMSS
 * 参数说明：
 * epsSrcList    输入时间 HH.MM.SS
 * opcResultList 输出时间 HHMMSS
 */
int  ASM_ChangeTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
 * 用于日期转换 YYYYMMDD To YYYY-MM-DD
 * 参数说明：
 * epsSrcList    输入日期 YYYYMMDD
 * opcResultList 输出日期 YYYY-MM-DD
 */
int  ASM_ChangeDate2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/**
 * 获取中间业务平台流水号(左补空格)
 * 参数说明：
 * epsSrcList    无
 * opcResultList 平台流水号
 */
int ASM_GetSerial( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList );

/**
 * 获取中间业务平台账务日期
 * 参数说明：
 * epsSrcList    无
 * opcResultList 平台日期
 */
int ASM_GetAgTranDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList );


int  ASM_ChangeTime2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
  * 将字符ASC转换为BCD 支持0-F的转换
  */
int ASM_AscToBcd( SVC_Content *epstService, UDP epcSrcList,
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 将字符BCD转换为ASC
  */
int ASM_BcdToAsc( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 将字符转化为HEX格式
  */
int ASM_AscToHex( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 将HEX格式转换为ASC串
  */
int ASM_HexToAsc( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 取系统日期并转换为BCD码
  */
int ASM_GetTime2BCD( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 取平台date+time
  */
int ASM_GetPtDateTime( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 应答消息类型转换
  */
int ASM_RetMsgid( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 去除左右空格
  */
int ASM_TrimBlank( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 获取渠道应答码 
  */
int ASM_GetChRetCode( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 获取渠道应答信息 
  */
int ASM_GetChRetMsg( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );


/**
  * 转换TPDU 
  */
int ASM_ChgTpdu( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
                    
/**
  * 获取终端批次号
  */
int ASM_GetBatch( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 获取原交易流水号
  */
int ASM_GetOldSerial( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
                 
/**
  * 获取终端批次号
  */
int ASM_GetOldDate( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 获取终端批次号
  */
int ASM_GetOldBatch( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
/**
  * 根据长度将BIN转换为ASCII
  */
int ASM_LEN_BIN2ASCII( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 字符串格式化累积
  */
int ASM_AddStr_FORMAT( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 获取系统日期的BCD
  */
int ASM_GetSysDate2BCD( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 打包60域
  */
int ASM_PackField60Ums( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 打包余额域
  */
int ASM_PackBalanceUms( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 打包结算信息域
  */
int ASM_PackSettleData( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 打包结算结果信息域
  */
int ASM_PackSettleResult( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 设置代理机构标识
  */
int ASM_SetAgenId( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * 根据长度将ASCII转换为ASCII
  */
int ASM_LEN_ASCII2ASCII( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
* BCD格式数据左移4个BIT
**/
int ASM_AlignLeft( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
* 打包交行余额返回
**/
int ASM_PackBalanceBankComm( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
*通用拆包函数
**/
int ASM_GetNormalString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
*雅酷余额打包函数
**/
int ASM_PackBalanceYacol( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );


/**
  * 字符串格式化累积扩展
  */

int ASM_AddStr_FORMAT_Ext( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/*通用打包解包函数*/
int ASM_PackString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/*打包MAC 数据*/
int ASM_PackMacBuffer( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList );


/*获取系统变量*/
int ASM_GetEvn( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList );

/**
  * ASC 应答消息类型转换
  */

int ASM_RetAscMsgid( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList );


/*通联余额打包*/
int ASM_PackBalanceAllinpay( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList );



/*打包函数*/
int ASM_PackToAsc( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                         int eiFldSeq, UDP opcResultList );

/*
**  函数功能: 将来源域值拷贝到目标域
**  函数名:   ASM_FldCopy
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
*/
int  ASM_UncharFldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/**
  * ASC 应答交易类型转换
  */

int ASM_RetTranstype( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList );


/**
  * 字符串格式化累积前补'0'
  */

int ASM_AddStr_Amount( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList );


#ifdef __cplusplus
}
#endif

#endif
/*
** end of file
*/
