/*
* 新大陆电脑公司 版权所有(c) 2006-2008
*
* POS标准版公共库
* 工具模块头文件  --- liso8583.h
* 作    者：    徐诗良 
* 日    期：    2006-09-11
* 最后修改人：  徐诗良
* 最后修改日期：2006-09-30
*/


#ifndef __LISO8583_H__
#define __LISO8583_H__

#ifdef __cplusplus
extern "C" {
#endif


#define ISO8583_SUCC	0
#define ISO8583_FAIL	(-1)
#define ISO8583_QUIT	(-2)


#ifndef BOOL
typedef unsigned char BOOL;
#endif

/* 每个域的最大长度 */
#define MAX_FIELD_LEN 1024 

/* 报文最大长度 */
#define MAX_MSG_LEN 4096

/* 域对齐方式 */
typedef enum ALIGN_TYPE 
{
    ALN_LEFT   = 0,
    ALN_RIGHT  = 1
} ALIGN_TYPE;

/* 长度类型 */
typedef enum LENGTH_TYPE 
{
    LEN_FIX    = 0,
    LEN_LLVAR  = 1,
    LEN_LLLVAR = 2
} LENGTH_TYPE;

typedef struct STISO_FIELD STISO_FIELD;
typedef struct STISO_FIELD_DEF STISO_FIELD_DEF;

/***************************************************************************************
 * 函数名称：*FIELD_PACKAGER_PACK
 * 函数介绍：对8583域进行打包，并返回打包后的数据和长度
 * 函数描述： 
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************/
typedef int (*FIELD_PACKAGER_PACK) (STISO_FIELD_DEF *pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/*****************************************************************************************
 * 函数名称：*FIELD_PACKAGER_UNPACK
 * 函数介绍：将缓冲区里面的数据解包到8583域中
 * 函数描述： 
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ******************************************************************************************/
typedef int (*FIELD_PACKAGER_UNPACK) (STISO_FIELD_DEF *pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/*******************************************************************************************************
 * 函数名称：*FIELD_LENGTH_SET
 * 函数介绍：根据ISO域的类型计算8583报文中的长度字节
 * 函数描述： 
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           len      ―― 域数据的长度 			
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区   
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ********************************************************************************************************/
typedef void (*FIELD_LENGTH_SET) (STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/*********************************************************************************************************
 * 函数名称：*FIELD_LENGTH_GET
 * 函数介绍：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度
 * 函数描述： 
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *********************************************************************************************************/
typedef void (*FIELD_LENGTH_GET) (STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);


/* ISO 8583 数据域长度计算函数结构 */
 struct STISO_FIELD_LENGTH_CALC
{
    FIELD_LENGTH_SET	pSet;	 /* 设置长度时调用的函数 */
    FIELD_LENGTH_GET	pGet;    /* 取出长度时调用的函数 */
}; 

/* ISO 8583 数据域结构 */
struct STISO_FIELD
{
    int    nLen;
    int    nOff;  /* 指向iso->databuf缓冲区的偏移，数据中不含长度类型的几个字节 */
};

/* ISO 8583 数据域定义 */
struct STISO_FIELD_DEF
{
    int                     nLen;        /* 数据域最大长度 */
    LENGTH_TYPE		        lentype;    /* 数据域长度类型: 1--LLVAR型 2--LLLVAR型 */
    ALIGN_TYPE              align;      /* 对齐类型 */
    char                    cPadchar;    /* 补齐字符 */	
    FIELD_PACKAGER_PACK		pPack;	    /* 打包函数指针 */
    FIELD_PACKAGER_UNPACK	pUnpack;     /* 解包函数指针 */
};  

/* ISO 8583 报文结构定义 */
typedef struct STISODataStru
{
    STISO_FIELD		fld[256];
    char			sBitmap[256];
    unsigned char 	szDatabuf[MAX_MSG_LEN + MAX_FIELD_LEN]; /* 多留一个域的长度，防止溢出 */
    int        		nUsed;
} STISODataStru;

/* 以下为变长域长度计算函数 */
/* ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节 */
void PubFLALengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLALengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节 */
void PubFLBLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLBLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* 十六进制形式，如长度104，将表示为 0x00, 0x68,共2个字节 */
void PubFLHLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLHLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* 打包、解包函数实现 */
int PubFPAMsgIDPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPAMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int  *pfieldlen, unsigned char *pindata);
int PubFPBMsgIDpack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPABitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPABitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPBBitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBBitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPACharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPACharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPBCharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBCharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPADigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPADigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPBDigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBDigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPBAmountPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBAmountUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);
int PubFPBBinaryPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);
int PubFPBBinaryUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/* Iso8583 操作函数 */
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition);
int PubIsoToStr(char *pcTargetStr, STISODataStru *SourceIsoStru, int *pnLen);
int PubGetBit(STISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen);
int PubSetBit(STISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);
int PubDelAllBit(STISODataStru *pIsoStru);
int PubDelOneBit(STISODataStru *pIsoStru, int nPosition);
int PubSetDefine8583(int nPosition, int nMaxLen, LENGTH_TYPE	LenType, ALIGN_TYPE AlignType, char chPadChar, FIELD_PACKAGER_PACK PackFunc, FIELD_PACKAGER_UNPACK UnpackFunc);
int PubImport8583(const struct STISO_FIELD_DEF *pNewDef);
void PubFormatDisp8583(STISODataStru *pIsoStru);
int PubSetFieldLengthCalc(const struct STISO_FIELD_LENGTH_CALC *pNewCalc);
int PubCopyIso(STISODataStru *pSourceIsoStru, STISODataStru *pTargetIsoStru);
/*打包流程
1.从0到128或64,顺序调用
int PubSetBit(STISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);
将字符串pcSourceStr按指定长度nStrLen填充到结构TargetIsoStru中指定成员
2.调用函数
int PubIsoToStr(char *pcTargetStr, STISODataStru *pSourceIsoStru, int *pnLen)
将ISO结构pSourceIsoStru转换成可发送的包文无符号字符串pcTargetStr
*/
/*解包流程
1.调用函数
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition)
将接收到的报文pcSourceStr转为ISO结构
2.调用函数
PubGetBit(STISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen)
从ISO结构pSourceIsoStru中提取相应的nPosition域的内容.
*/
#ifdef __cplusplus
	}
#endif

#endif /* End of __ISO8583_H__ */
/* End of liso8583.h */
