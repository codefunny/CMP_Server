/*
**file:asm_fundef.h
*/

#include "asm_api.h"
#include "asm_fun.h"

#ifndef __ASM_FUN_DEF_H__
#define __ASM_FUN_DEF_H__
OperaFunEntity astAsmEntity[] =
{
    ADDASM( FldCopy ),               /* 0域拷贝  */
    ADDASM( AddStr ),                /* 1字符串累加 */
    ADDASM( AddShort ),              /* 2短整数累加  */
    ADDASM( AddInt ),                /* 3整数累加  */
    ADDASM( AddLong ),               /* 4长整数累加 */
    ADDASM( AddFloat ),              /* 5浮点数累加  */
    ADDASM( AddDouble ),             /* 6双精度数累加  */
    ADDASM( SysTime ),               /* 7取系统time HHMMSS*/
    ADDASM( SysDate ),               /* 8取系统日期：YYYYMMDD  */
    ADDASM( ChangeDate ),            /* 9日期转换YYYY-MM-DD到YYYYMMDD*/
    ADDASM( ChangeTime ),            /* 10时间转换HH.MM.SS到HHMMSS*/
    ADDASM( ChangeDate2 ),           /* 11日期转换YYYYMMDD到YYYY-MM-DD*/
    ADDASM( GetSerial ),             /* 12获取当前中间业务平台流水号(左补0)*/
    ADDASM( GetAgTranDate ),         /* 13获取当前中间业务平台账务日期 */
    ADDASM( ChangeTime2 ),           /* 14时间转换HHMMSS到HH:MM:SS*/
    ADDASM( AscToBcd ),              /* 15ASCII转换为BCD*/
    ADDASM( BcdToAsc ),              /* 16BCD转换为ASCII*/
    ADDASM( AscToHex ),              /* 17ASCII转换为HEX*/
    ADDASM( HexToAsc ),              /* 18HEX转换为ASCII*/
    ADDASM( GetTime2BCD ),           /* 19取系统时间转换为BCD*/
    ADDASM( GetPtDateTime ),         /* 20取平台DATE+TIME yyyymmddhhmmss */
    ADDASM( RetMsgid ),              /* 21应答的消息类型转变 */
    ADDASM( TrimBlank ),             /* 22去左右空格 */
    ADDASM( GetChRetCode ),          /* 23获取渠道应答码 */
    ADDASM( GetChRetMsg  ),          /* 24获取渠道应答信息 */
    ADDASM( ChgTpdu  ),              /* 25转换TPDU */
    ADDASM( GetBatch  ),             /* 26获取终端批次号 */
    ADDASM( GetOldSerial  ),         /* 27获取原交易流水号 */
    ADDASM( GetOldDate  ),           /* 28获取原交易日期 */
    ADDASM( GetOldBatch  ),          /* 29获取原交易日期 */
    ADDASM( LEN_BIN2ASCII  ),        /* 30根据长度将BIN转换为ASCII */
    ADDASM( AddStr_FORMAT  ),        /* 31字符串格式化累积 */
    ADDASM( GetSysDate2BCD  ),       /* 32获取系统日期BCD */
    ADDASM( PackField60Ums  ),       /* 33打包60域 */
    ADDASM( PackBalanceUms  ),       /* 34打包余额域 */
    ADDASM( PackSettleData  ),       /* 35打包结算数据域 */
    ADDASM( PackSettleResult  ),     /* 36打包结算结果域 */
    ADDASM( SetAgenId  ),            /* 37设置代理机构标识 */
    ADDASM( LEN_ASCII2ASCII  ) ,     /* 38根据长度将ASCII转换为ASCII */
    ADDASM( AlignLeft),              /*39 BCD格式字符串左移4个BIT*/
    ADDASM( PackBalanceBankComm),    /*40打包交行余额域*/
    ADDASM( GetNormalString),        /*41通用拆包函数*/
    ADDASM( PackBalanceYacol),       /*42雅酷卡余额打包函数*/
    ADDASM( AddStr_FORMAT_Ext),      /*43字符串格式化累积 扩展*/
    ADDASM( PackString),             /*44通用重打包函数*/
    ADDASM(PackMacBuffer),           /*45打包MAC 数据*/
    ADDASM( GetEvn),                 /*46获取系统变量*/
    ADDASM( RetAscMsgid),            /*47ASC应答的消息类型转变 */
    ADDASM( PackBalanceAllinpay),    /*48打包通联余额域*/
    ADDASM( PackToAsc),			     /*49根据长度将asc转换为ASCII */
    ADDASM( UncharFldCopy),			 /* 50BIN2CHAR域拷贝  */
	ADDASM( RetTranstype), 		     /* 51ASC应答交易类型转换*/
	ADDASM( AddStr_Amount),			 /* 52字符串格式化累积 前补'0'*/
};	
#endif



/*
**end of file
*/
