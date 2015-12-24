/*
* 新大陆电脑股份有限公司 版权所有(c) 2006-2008
*
* POS标准版公共库
* 8583函数模块  --- liso8583.c
* 作    者：    徐诗良
* 日    期：    2006-09-10
* 最后修改人：  徐诗良
* 最后修改日期：2006-09-30
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "isodef.h"

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(p)       if (!(p)) { fprintf(stderr,\
        "Assertion failed: %s, file %s, line %d\n",\
        #p, __FILE__, __LINE__);abort();}
#else
#define ASSERT(p)
#endif
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef isprint
#define isprint(c) ((char)(c) >= 0x20 && (char)(c) < 0x7f)
#endif

#ifndef isdigit
#define isdigit(c) ((char)(c) >= 0x40 && (char)(c) <= 0x49)
#endif
/*内部函数定义*/
static int ProAddPadChar(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
						int nFieldlen, unsigned char *poutdata, unsigned char cHead);

/****************************************************************************************
 * 函数名称：ProAddPadChar
 * 函数介绍：为一个ISO域的数据进行补齐操作
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要补齐的8583域数据指针
 *           nFieldlen―― 将要补齐的8583域数据长度
 *           cHead    ―― 是否有头字符（金额类型的域有C/D标志）
 * 出口参数：poutdata ―― 用于接收补齐后的数据的缓冲区
 * 返 回 值：补齐后的字符串长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ******************************************************************************************/
static int ProAddPadChar(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
						 int nFieldlen, unsigned char *poutdata, unsigned char cHead)
{
    int nLen,nCount = 0;

    if (pthis->lentype == LEN_FIX)
	{
        nLen = pthis->nLen;
	}
    else
	{
        nLen = min(pthis->nLen, nFieldlen);
	}

    nLen += cHead;
    nFieldlen = min(nLen, nFieldlen);

    /* put C/D char */
	if (cHead == 1)
	{
		poutdata[nCount++] = pfieldbuf[0];
		--nLen;
		--nFieldlen;
	}

        /* add left cPadchar */
	if ((pthis->lentype == LEN_FIX) && (pthis->align == ALN_RIGHT))
	{
		memset(poutdata + nCount, pthis->cPadchar, nLen - nFieldlen);
		nCount += nLen - nFieldlen;
	}

    memcpy(poutdata + nCount, (char *) pfieldbuf + cHead,nFieldlen);
    nCount += nFieldlen;

        /* add right cPadchar */
    if ((pthis->lentype == LEN_FIX) && (pthis->align == ALN_LEFT))
    {
        memset(poutdata + nCount, pthis->cPadchar, nLen - nFieldlen);
        nCount += nLen - nFieldlen;
    }

    ASSERT(nCount == (nLen + cHead));
    return nCount;
}

/******************************************************************************
*	函数名称：PubAscToHex
*	函数介绍：将AscII码的字符串转换成压缩的HEX格式
*	函数描述：非偶数长度的字符串根据对齐方式，采取左补0，右补F方式
*	入口参数：pszAsciiBuf	---- 被转换的ASCII字符串
*			  nLen			---- 输入数据长度(ASCII字符串的长度)
*			  cType			---- 对齐方式  0－左对齐  1－右对齐
*	出口参数：pszBcdBuf		---- 转换输出的HEX数据
*	返 回 值：ISO8583_SUCC 0 成功; ISO8583_FAIL -1 失败
*	修 改 者：陈丹莹
*	修改日期：2006-09-11
*******************************************************************************/
static int PubAscToHex (const unsigned char* pszAsciiBuf, int nLen, char cType, unsigned char* pszBcdBuf)
{
	int i = 0;
	char cTmp, cTmp1;

	if (pszAsciiBuf == NULL)
	{
		return ISO8583_FAIL;
	}

	if (nLen & 0x01 && cType)/*判别是否为奇数以及往那边对齐*/
	{
		cTmp1 = 0 ;
	}
	else
	{
		cTmp1 = 0x55 ;
	}

	for (i = 0; i < nLen; pszAsciiBuf ++, i ++)
	{
		if ( *pszAsciiBuf >= 'a' )
		{
			cTmp = *pszAsciiBuf - 'a' + 10 ;
		}
		else if ( *pszAsciiBuf >= 'A' )
		{
			cTmp = *pszAsciiBuf - 'A' + 10 ;
		}
		else if ( *pszAsciiBuf >= '0' )
		{
			cTmp = *pszAsciiBuf - '0' ;
		}
		 else
		 {
			cTmp = *pszAsciiBuf;
			cTmp&=0x0f;
		 }

		if ( cTmp1 == 0x55 )
		{
			cTmp1 = cTmp;
		}
		else
		{
			*pszBcdBuf ++ = cTmp1 << 4 | cTmp;
			cTmp1 = 0x55;
		}
	}
	if (cTmp1 != 0x55)
	{
		*pszBcdBuf = cTmp1 << 4;
	}

    return ISO8583_SUCC;
}

/******************************************************************************
*	函数名称：PubHexToAsc
*	函数介绍：将HEX码数据转换成AscII码字符串
*	函数描述：
*	入口参数：pszBcdBuf		---- 被转换的Hex数据
*			  nLen			---- 转换数据长度(ASCII字符串的长度)
*			  cType			---- 对齐方式  1－左对齐  0－右对齐
*	出口参数：pszAsciiBuf	---- 转换输出的AscII码数据
*	返 回 值：ISO8583_SUCC 0 成功; ISO8583_FAIL -1 失败
*	修 改 者：陈丹莹
*	修改日期：2006-09-11
*******************************************************************************/
static int PubHexToAsc (const unsigned char* pszBcdBuf, int nLen, char cType, unsigned char* pszAsciiBuf)
{
	int i = 0;

	if (pszBcdBuf == NULL)
	{
		return ISO8583_FAIL;
	}
	if (nLen & 0x01 && cType) /*判别是否为奇数以及往那边对齐*/
	{                           /*0左，1右*/
		i = 1;
		nLen ++;
	}
	else
	{
		i = 0;
	}
	for (; i < nLen; i ++, pszAsciiBuf ++)
	{
		if (i & 0x01)
		{
			*pszAsciiBuf = *pszBcdBuf ++ & 0x0f;
		}
		else
		{
			*pszAsciiBuf = *pszBcdBuf >> 4;
		}
		if (*pszAsciiBuf > 9)
		{
			*pszAsciiBuf += 'A' - 10;
		}
		else
		{
			*pszAsciiBuf += '0';
		}

	}
	*pszAsciiBuf = 0;
	return ISO8583_SUCC;
}


/**********************************************************************************************
 * 函数名称：PubFLBLengthSet
 * 函数介绍：根据ISO域的类型计算8583报文中的长度字节(BCD形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           nLen      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 **********************************************************************************************/
void PubFLBLengthSet(STISO_FIELD_DEF *pthis, int nLen, unsigned char *poutdata, int *pcount)
{
    unsigned char *pWt = poutdata;

    if (pthis->lentype == LEN_LLVAR)
	{
        (*pWt++) = (unsigned char)(nLen % 10) + (nLen / 10) * 16;
	}
    else
	{
		if (pthis->lentype == LEN_LLLVAR)
		{
			(*pWt++) = (unsigned char)(nLen / 100);
			(*pWt++) = (unsigned char)((nLen % 100) / 10) * 16 + (nLen % 100) % 10;
		}
	}

    *pcount = (pWt - poutdata);
    return;
}

/*************************************************************************************************
 * 函数名称：PubFLBLengthGet
 * 函数介绍：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(BCD形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ************************************************************************************************/
void PubFLBLengthGet(STISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
    unsigned char *pRt = pindata;

    if (pthis->lentype == LEN_FIX)
	{
        *plen = pthis->nLen;
	}
    else
    {
        *plen = (*pRt) - ((*pRt) >> 4) * 6;
        pRt++;
        if (pthis->lentype == LEN_LLLVAR)
        {
            *plen = (*plen) * 100 + ((*pRt) - ((*pRt) >> 4) * 6);
            pRt++;
        }
        if (*plen > pthis->nLen)
		{
            *plen = pthis->nLen;
		}
    }

    *pcount = (pRt - pindata);
    return;
}

/**********************************************************************************************************
 * 函数名称：PubFLALengthSet
 * 函数介绍：根据ISO域的类型计算8583报文中的长度字节(ASC形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           nLen      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 **********************************************************************************************************/
void PubFLALengthSet(STISO_FIELD_DEF *pthis, int nLen, unsigned char *poutdata, int *pcount)
{
    unsigned char *pWt = poutdata;
    if (pthis->lentype == LEN_LLVAR)
    {
        (*pWt++) = (unsigned char)(nLen / 10) + 0x30;
        (*pWt++) = (unsigned char)(nLen % 10) + 0x30;
    }
    else if (pthis->lentype == LEN_LLLVAR)
    {
        (*pWt++) = (unsigned char)(nLen / 100) + 0x30;
        (*pWt++) = (unsigned char)((nLen % 100) / 10)  + 0x30;
        (*pWt++) = (unsigned char)(nLen % 10)  + 0x30;
    }

    *pcount = (pWt - poutdata);
    return;
}

/********************************************************************************************************
 * 函数名称：PubFLALengthGet
 * 函数介绍：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(ASC形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ********************************************************************************************************/
void PubFLALengthGet(STISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
    unsigned char *pRt = pindata;

    if (pthis->lentype == LEN_FIX)
	{
        *plen = pthis->nLen;
	}
    else
    {
        if (pthis->lentype == LEN_LLLVAR)
		{
            *plen = ((*pRt++) - 0x30) * 100;
		}
		else
		{
			*plen = 0;
		}
        *plen += ((*pRt++) - 0x30) * 10;
        *plen += (*pRt++) - 0x30;
        if (*plen > pthis->nLen)
		{
            *plen = pthis->nLen;
		}
    }

    *pcount = (pRt - pindata);
    return;
}

/*******************************************************************************************************
 * 函数名称：PubFLHLengthSet
 * 函数介绍：根据ISO域的类型计算8583报文中的长度字节(HEX形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           nLen      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *******************************************************************************************************/
void PubFLHLengthSet(STISO_FIELD_DEF *pthis, int nLen, unsigned char *poutdata, int *pcount)
{
    unsigned char *pWt = poutdata;

    if (pthis->lentype == LEN_LLVAR)
	{
        (*pWt++) = (unsigned char) nLen;
	}
    else
	{
		if (pthis->lentype == LEN_LLLVAR)
		{
			(*pWt++) = (unsigned char) (nLen / 256);
			(*pWt++) = (unsigned char) (nLen % 256);
		}
	}

    *pcount = (pWt - poutdata);
    return;
}

/********************************************************************************************************
 * 函数名称：PubFLHLengthGet
 * 函数介绍：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(HEX形式)
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ********************************************************************************************************/
void PubFLHLengthGet(STISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
    unsigned char *pRt = pindata;

    if (pthis->lentype == LEN_FIX)
	{
        *plen = pthis->nLen;
	}
    else
    {
        if (pthis->lentype == LEN_LLLVAR)
		{
            *plen = (*pRt++) * 256;
		}
		if (pthis->lentype == LEN_LLVAR)
		{
            *plen = 0; /*init plen*/
        }
        *plen += (*pRt++); /* */
        if (*plen > pthis->nLen)
		{
            *plen = pthis->nLen;
		}
    }

    *pcount = (pRt - pindata);
    return;
}

/*****************************************************************************************************
 * 函数名称：PubFPAMsgIDPack
 * 函数介绍：为一个ISO域的数据进行打包（MSG_ID域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *****************************************************************************************************/
int PubFPAMsgIDPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int nFieldlen,
					unsigned char *poutdata)
{
    int nLen = pthis->nLen;

    memcpy(poutdata, pfieldbuf, nLen);
    return nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPAMsgIDUnpack
 * 函数介绍：为一个ISO域的数据进行解包（MSG_ID域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPAMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen,
					  unsigned char *pindata)
{
    int nLen = pthis->nLen;

    memcpy(pfieldbuf, pindata, pthis->nLen);
    *pfieldlen = nLen;
    return nLen;
}

/**********************************************************************************************************
 * 函数名称：PubFPBMsgIDpack
 * 函数介绍：为一个ISO域的数据进行打包（MSG_ID域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***********************************************************************************************************/
int PubFPBMsgIDpack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int nFieldlen,
					unsigned char *poutdata)
{
    int nLen = pthis->nLen >> 1;

	if (PubAscToHex( pfieldbuf, pthis->nLen, 0, poutdata ) != ISO8583_SUCC)
	{
		return ISO8583_FAIL;
	}
    return nLen;
}

/**************************************************************************************************************
 * 函数名称：PubFPBMsgIDUnpack
 * 函数介绍：为一个ISO域的数据进行解包（MSG_ID域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 **************************************************************************************************************/
int PubFPBMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen,
					  unsigned char *pindata)
{
    int nLen;

    nLen = pthis->nLen >> 1;
	if (PubHexToAsc(pindata, pthis->nLen, 0, pfieldbuf) != ISO8583_SUCC)
	{
		return ISO8583_FAIL ;
	}
    *pfieldlen = pthis->nLen;
    return nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPBBitmapPack
 * 函数介绍：为一个ISO域的数据进行打包（BITMAP位图域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBBitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int nFieldlen,
					 unsigned char *poutdata)
{
    int nBitmaplen;
    int nI, nJ;
    unsigned char sBitmap[16], ucBitmask;

    nBitmaplen = nFieldlen >> 3;
    memset(sBitmap, 0, sizeof(sBitmap));
    for (nI = 0; nI < nBitmaplen; nI++)
    {
        ucBitmask = 0x80;
        for (nJ = 0; nJ < 8; nJ++, ucBitmask >>= 1)
        {
            if (pfieldbuf[nI * 8 + nJ])
			{
                sBitmap[nI] |= ucBitmask;
			}
        }
    }

    memcpy(poutdata, sBitmap, nBitmaplen);
    return nBitmaplen;
}

/***************************************************************************************************************
 * 函数名称：PubFPBBitmapUnPack
 * 函数介绍：为一个ISO域的数据进行解包（BITMAP位图域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBBitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen,
					   unsigned char *pindata)
{
    int nI,nJ,nBitmaplen;
    unsigned char ucBitmask;

    if (pindata[0] & 0x80)
	{
        nBitmaplen = 16;
	}
    else
	{
        nBitmaplen = 8;
	}

	/*memset(pfieldbuf, 0, sizeof(pfieldbuf));*/
    for (nI = 0; nI < nBitmaplen; nI++)
    {
        ucBitmask = pindata[nI];
        for (nJ = 0; nJ < 8; nJ++, ucBitmask <<= 1)
		{
			pfieldbuf[(nI << 3) + nJ] = ((ucBitmask & 0x80) ? 1 : 0);
		}
    }

    *pfieldlen = (nBitmaplen << 3);
    return nBitmaplen;
}

/***************************************************************************************************************
 * 函数名称：PubFPABitmapPack
 * 函数介绍：为一个ISO域的数据进行打包（BITMAP位图域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPABitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int nFieldlen,
					 unsigned char *poutdata)
{
    int nBitmaplen;
    unsigned char sBitmap[16];

    memset(sBitmap, 0, sizeof(sBitmap));
    nBitmaplen = PubFPBBitmapPack(pthis, pfieldbuf, nFieldlen, sBitmap);
    /*ProBcdToAsc(poutdata, sBitmap, nBitmaplen * 2, 0);*/
	//PubHexToAsc(poutdata, nBitmaplen*2, 0, sBitmap);
	PubHexToAsc(sBitmap, nBitmaplen*2, 0, poutdata);  //原来输入/输出弄反了,wwt 2011-4-11
    return nBitmaplen * 2;
}

/***************************************************************************************************************
 * 函数名称：PubFPABitmapUnPack
 * 函数介绍：为一个ISO域的数据进行解包（BITMAP位图域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPABitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen,
					   unsigned char *pindata)
{
    int nBitmaplen;
    unsigned char sBitmap[16];

    /*ProAscToBcd(sBitmap, pindata, 16, 0);*/
	PubAscToHex(pindata, 16, 0, sBitmap);
    if (sBitmap[0] & 0x80)
	{
        nBitmaplen = 16;
	}
    else
	{
        nBitmaplen = 8;
	}
    /*ProAscToBcd(sBitmap, pindata, nBitmaplen * 2, 0);*/
	PubAscToHex(pindata, nBitmaplen*2, 0, sBitmap);
    PubFPBBitmapUnPack(pthis, pfieldbuf, pfieldlen, sBitmap);
    return nBitmaplen * 2;
	//return nBitmaplen;
}

/***************************************************************************************************************
 * 函数名称：PubFPACharPack
 * 函数介绍：为一个ISO域的数据进行打包（字符类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPACharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
				   int nFieldlen, unsigned char *poutdata)
{
    int nI, nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    for (nI = 0; nI < nFieldlen; nI++)
    {
        if (!isprint(pfieldbuf[nI]))
		{
            return ISO8583_FAIL;
		}
    }

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 0);
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    memcpy(poutdata + nCount, (char *) sztmp, nLen);
    return nCount + nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPACharUnPack
 * 函数介绍：为一个ISO域的数据进行解包（字符类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPACharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					 int *pfieldlen, unsigned char *pindata)
{
    int nI, nLen, nCount;

    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount);

    memcpy(pfieldbuf, pindata + nCount, nLen);
    *pfieldlen = nLen;

    for (nI = 0; nI < *pfieldlen; nI++)
    {
        if (!isprint(pfieldbuf[nI]))
        {
            *pfieldlen = 0;
            return ISO8583_FAIL;
        }
    }

    return nCount + nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPBCharPack
 * 函数介绍：为一个ISO域的数据进行打包（字符类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nLen      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBCharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
				   int nFieldlen, unsigned char *poutdata)
{
    int nI, nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    for (nI = 0; nI < nFieldlen; nI++)
    {
        if (!isprint(pfieldbuf[nI]))
		{
            return ISO8583_FAIL;
		}
    }

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 0);
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    /* 对于奇数长度的，最后半个也按补齐字符压缩 */
    if ((nLen & 1) && (pthis->align == ALN_LEFT))
	{
        sztmp[nLen++] = pthis->cPadchar;
	}

    /*ProAscToBcd(poutdata + nCount, sztmp, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));*/
	PubAscToHex( sztmp, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				poutdata + nCount );
	return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPBCharUnPack
 * 函数介绍：为一个ISO域的数据进行解包（字符类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBCharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					 int *pfieldlen, unsigned char *pindata)
{
    int nI, nLen, nCount;

    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount);

    //ProBcdToAsc(pfieldbuf, pindata + nCount, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	PubHexToAsc(pindata + nCount, nLen,
				(unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				pfieldbuf);
	*pfieldlen = nLen;

    for (nI = 0; nI < *pfieldlen; nI++)
    {
        if (!isprint(pfieldbuf[nI]))
        {
            *pfieldlen = 0;
            return ISO8583_FAIL;
        }
    }

    return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPADigitPack
 * 函数介绍：为一个ISO域的数据进行打包（数字类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPADigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
					int nFieldlen, unsigned char *poutdata)
{
    int nI, nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    for (nI = 0; nI < nFieldlen; nI++)
    {
        if (!isdigit(pfieldbuf[nI]))
		{
			return ISO8583_FAIL;
		}
    }

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 0);
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    memcpy(poutdata + nCount, (char *) sztmp, nLen);
    return nCount + nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPADigitUnPack
 * 函数介绍：为一个ISO域的数据进行解包（数字类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPADigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					  int *pfieldlen, unsigned char *pindata)
{
    int nI, nLen, nCount;

    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount);

    memcpy(pfieldbuf, pindata + nCount, nLen);
    *pfieldlen = nLen;

    for (nI = 0; nI < *pfieldlen; nI++)
    {
        if (!isdigit(pfieldbuf[nI]))
        {
            *pfieldlen = 0;
            return ISO8583_FAIL;
        }
    }

    return nCount + nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPBDigitPack
 * 函数介绍：为一个ISO域的数据进行打包（数字类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBDigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
					int nFieldlen, unsigned char *poutdata)
{
    int nI, nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    for (nI = 0; nI < nFieldlen; nI++)
    {
        if (!isdigit(pfieldbuf[nI]))
		{
            return ISO8583_FAIL;
		}
    }

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 0);
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    /* 对于奇数长度的，最后半个也按补齐字符压缩 */
    if ((nLen & 1) && (pthis->align == ALN_LEFT))
	{
        sztmp[nLen++] = pthis->cPadchar;
    }
    //ProAscToBcd(poutdata + nCount, sztmp, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
    PubAscToHex(sztmp, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				poutdata+nCount);
	return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPBDigitUnPack
 * 函数介绍：为一个ISO域的数据进行解包（数字类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBDigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					  int *pfieldlen, unsigned char *pindata)
{
    int nI, nLen, nCount;

    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount);

    //ProBcdToAsc(pfieldbuf, pindata + nCount, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	PubHexToAsc(pindata+nCount, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				pfieldbuf);
	*pfieldlen = nLen;

    for (nI = 0; nI < *pfieldlen; nI++)
    {
        if (!isdigit(pfieldbuf[nI]))
        {
            *pfieldlen = 0;
            return ISO8583_FAIL;
        }
    }

    return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPBAmountPack
 * 函数介绍：为一个ISO域的数据进行打包（金额类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBAmountPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
					 int nFieldlen, unsigned char *poutdata)
{
    int nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 1); // nLen contain the char 'C/D'
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    poutdata[nCount++] = sztmp[0];
    --nLen;
    //ProAscToBcd(poutdata + nCount, sztmp + 1, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
    PubAscToHex(sztmp + 1, nLen,
				(unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				poutdata + nCount);
	return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPBAmountUnPack
 * 函数介绍：为一个ISO域的数据进行解包（金额类型域ASC码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBAmountUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					   int *pfieldlen, unsigned char *pindata)
{
    int nLen, nCount;

    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount); // nLen NOT contain the char 'C/D'

    pfieldbuf[0] = pindata[nCount++];

    //ProBcdToAsc(pfieldbuf + 1, pindata + nCount, nLen,
	//			(unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	PubHexToAsc(pindata + nCount, nLen, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1),
				pfieldbuf + 1);
    *pfieldlen = nCount + nLen;
    return nCount + (++nLen >> 1);
}

/***************************************************************************************************************
 * 函数名称：PubFPBBinaryPack
 * 函数介绍：为一个ISO域的数据进行打包（二进制类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           nFieldlen―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBBinaryPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
					 int nFieldlen, unsigned char *poutdata)
{
    int nLen, nCount;
    unsigned char sztmp[MAX_FIELD_LEN];

    nLen = ProAddPadChar(pthis, pfieldbuf, nFieldlen, sztmp, 0);
    stiso8583_fldcalc.pSet(pthis, nLen, poutdata, &nCount);

    memcpy(poutdata + nCount, (char *) sztmp, nLen);
    return nCount + nLen;
}

/***************************************************************************************************************
 * 函数名称：PubFPBBinaryUnPack
 * 函数介绍：为一个ISO域的数据进行解包（二进制类型域BCD码形式）
 * 函数描述：
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           pfieldlen―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ***************************************************************************************************************/
int PubFPBBinaryUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf,
					   int *pfieldlen, unsigned char *pindata)
{
    int nLen, nCount;
    stiso8583_fldcalc.pGet(pthis, pindata, &nLen, &nCount);

    memcpy(pfieldbuf, pindata + nCount, nLen);
    *pfieldlen = nLen;
    return nCount + nLen;
}

/**********************************************************************
 * 函数名称：PubDelAllBit
 * 函数介绍：清空ISO_data结构中所有成员变量值
 * 函数描述：
 * 入口参数：pIsoStru   ―― 源数据结构ISO_data 结构指针
 * 出口参数：pIsoStru   ―― 清空后的ISO_data结构指针
 * 返 回 值：ISO8583_SUCC     ―― 成功
 *           ISO8583_FAIL  ―― 失败，IsoStru为空
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 **********************************************************************/
int PubDelAllBit(STISODataStru *pIsoStru)
{
    int nI;

    if(NULL == pIsoStru)
    {
        return ISO8583_FAIL;
    }

    /*Initialize the iso fields*/
    for (nI = 0; nI < 256; nI++)
    {
        pIsoStru->sBitmap[nI] = 0;
        pIsoStru->fld[nI].nOff = 0;
        pIsoStru->fld[nI].nLen = 0;
    }
    memset(pIsoStru->szDatabuf,0,MAX_MSG_LEN + MAX_FIELD_LEN);
    pIsoStru->nUsed = 0;
    return ISO8583_SUCC;
}

/************************************************************************
* 函数名称：PubDelOneBit
* 函数介绍：清空IsoStru结构中第nPosition域的值，同时清除位图中的相应标志
* 函数描述：
* 入口参数：
*          pIsoStru        ―― 源数据结构ISO_data 结构指针
*          nPosition       ―― ISO_data结构中的第nPosition个成员
* 出口参数：
*          pIsoStru        ―― 清除后的ISO_data结构指针
* 返 回 值：
*          ISO8583_SUCC          ―― 成功
*          ISO8583_FAIL       ―― 失败，IsoStru为NULL
*                          ―― 无效的成员号（nPosition <0 或者 >=256）
* 修改者  ：徐诗良
* 修改日期：2006-09-29
**************************************************************************/
int PubDelOneBit(STISODataStru *pIsoStru, int nPosition)
{
    int nI;
	//int nBitmap = 0;

    if(NULL == pIsoStru)
    {
        return ISO8583_FAIL;
    }

    if (nPosition < 0 || nPosition >= 256)
    {
        return ISO8583_FAIL;
    }
    if (nPosition != 1) //避免无法设置128域
	{
		pIsoStru->sBitmap[nPosition-1] = 0;
	}
    if (pIsoStru->fld[nPosition].nOff != 0 && pIsoStru->fld[nPosition].nLen != 0)
    {
        /* move  forward all the field data in iso->szDatabuf*/
        for (nI = 0; nI < 256; nI++)
        {
            if ((pIsoStru->fld[nI].nLen > 0)
				&& (pIsoStru->fld[nI].nOff > pIsoStru->fld[nPosition].nOff))
			{
                pIsoStru->fld[nI].nOff -= pIsoStru->fld[nPosition].nLen;
			}
        }
        pIsoStru->nUsed -= pIsoStru->fld[nPosition].nLen;
        for (nI = pIsoStru->fld[nPosition].nOff; nI < pIsoStru->nUsed; nI++)
		{
            pIsoStru->szDatabuf[nI] = pIsoStru->szDatabuf[nI + pIsoStru->fld[nPosition].nLen];
		}
    }
    pIsoStru->fld[nPosition].nOff = 0;
    pIsoStru->fld[nPosition].nLen = 0;

    return ISO8583_SUCC;
}

/*****************************************************************************************************
 * 函数名称：PubStrToIso
 * 函数介绍：转换字符串为ISO结构
 *	函数描述：
 * 入口参数：
 *          pcSourceStr     ―― 转换的源字符串
 *          nStrLen         －－ 数据长度
 * 出口参数：
 *          pTargetIsoStru  ―― 转换的结果ISO结构
 *          pnPosition      ―― 传入为NULL时，忽略;
 *                              否则存放转换出错时的域编号（仅对返回ERRFIELDLEN时有效）
 *                              其余情况下均为-1
 * 返 回 值：
 *          ISO8583_SUCC          ―― 转换成功
 *          ISO8583_FAIL       ―― 转换失败，pcSourceStr或者TargetIsoStru为空
 *                          ―― 解包出错
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *****************************************************************************************************/
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition)
{
    int nI = 0;        /* 循环子                       */
    int unpacked = 0; /* 8583报文已经被解开的字节数   */
    int nUsed = 0;     /* 当前域在8583报文占用的字节数 */

    if (NULL == pcSourceStr || NULL == pTargetIsoStru)
    {
        return ISO8583_FAIL;
    }

    /* Initialize the pTargetIsoStru fields */
    PubDelAllBit(pTargetIsoStru);

    /* Get 128 + 64 elements */
    for (nI = 0; nI <= ((pTargetIsoStru->sBitmap[65 - 1]) ? 192 : 128); nI++)
    {
        /* field 0 (MSG_ID) and field 1 (BITMAP) need not check */
        if (nI >= 2 && 0 == pTargetIsoStru->sBitmap[nI - 1])
		{
			continue;
		}

        pTargetIsoStru->fld[nI].nOff = pTargetIsoStru->nUsed;
        nUsed = stiso8583_def[nI].pUnpack(&stiso8583_def[nI],
										  pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nI].nOff,
										  &pTargetIsoStru->fld[nI].nLen,
										  (unsigned char*) pcSourceStr + unpacked);
        if ((nUsed <= 0)
			|| ((unpacked + nUsed) > nStrLen)
			|| ((pTargetIsoStru->nUsed + pTargetIsoStru->fld[nI].nLen) > MAX_MSG_LEN))
        {
            if (NULL != pnPosition)
			{
                *pnPosition = nI;
			}
            pTargetIsoStru->fld[nI].nOff = 0;

            return ISO8583_QUIT;
        }
        unpacked += nUsed;
        pTargetIsoStru->nUsed += pTargetIsoStru->fld[nI].nLen;

        if (nI == 1) /* Bitmap for 2 - 128 */
        {
            memset( pTargetIsoStru->sBitmap, 0,
					sizeof(pTargetIsoStru->sBitmap) / sizeof(pTargetIsoStru->sBitmap[0])) ;
            memcpy( pTargetIsoStru->sBitmap,
					pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nI].nOff,
					pTargetIsoStru->fld[nI].nLen);
        }
        if (nI == 65) /* Bitmap for 129-192 */
		{
            memcpy( pTargetIsoStru->sBitmap + 128,
					pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nI].nOff,
					pTargetIsoStru->fld[nI].nLen);
		}
	}
    return ISO8583_SUCC;
}

/*******************************************************************************************
 * 函数名称：PubIsoToStr
 * 函数介绍：将ISO结构转换成无符号字符串
 * 函数描述：
 * 入口参数：
 *          pSourceIsoStru  ―― 转换的源数据结构
 *          pnLen           ―― 传入pcTargetStr的空间大小（包含结束符）
 * 出口参数：
 *          pcTargetStr     ―― 转换的结果字符串(BCD码)
 *          pnLen           ―― 存放转换成功后的字符串长度。
 *                               如果失败，长度为-1。
 * 返 回 值：
 *          ISO8583_SUCC          ―― 转换成功
 *          ISO8583_FAIL       ―― 转换失败，pcTargetStr或者pSourceIsoStru为空
 *                          ―― 打包失败
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ********************************************************************************************/
int PubIsoToStr(char *pcTargetStr, STISODataStru *pSourceIsoStru, int *pnLen)
{
    int nI = 0;        /* 循环子                       */
    int packed = 0;   /* 8583报文已经被解开的字节数   */
    int nUsed = 0;     /* 当前域在8583报文占用的字节数 */

    if (NULL == pcTargetStr || NULL == pSourceIsoStru)
    {
        return ISO8583_FAIL;
    }

    PubSetBit(pSourceIsoStru, 1, pSourceIsoStru->sBitmap, (pSourceIsoStru->sBitmap[0] ? 128 : 64));
    if (pSourceIsoStru->sBitmap[65-1])
	{
        PubSetBit(pSourceIsoStru, 65, pSourceIsoStru->sBitmap + 128, 64);
	}

    for (nI = 0; nI <= 128 + pSourceIsoStru->fld[65].nLen; nI++)
    {
        if (nI == 0 || nI == 1 || pSourceIsoStru->sBitmap[nI - 1])
        {
            nUsed = stiso8583_def[nI].pPack(&stiso8583_def[nI],
											pSourceIsoStru->szDatabuf + pSourceIsoStru->fld[nI].nOff,
											pSourceIsoStru->fld[nI].nLen,
											(unsigned char*) pcTargetStr + packed);
			if (nUsed < 0)
			{
				if (NULL != pnLen)
				{
					*pnLen = -1;
				}
				return ISO8583_FAIL;
			}
            packed += nUsed;
        }
    }

    if (NULL != pnLen)
	{
        *pnLen = packed;
	}
	return ISO8583_SUCC;
}

/************************************************************************************************
 * 函数名称：PubGetBit
 * 函数介绍：将结构ISO中指定成员提取到字符串szTargetStr中
 * 函数描述：
 * 入口参数：
 *          pSourceIsoStru  ―― 源数据结构ISO_data 结构指针
 *          nPosition       ―― ISO_data结构中的第N个成员
 *          pnLen           ―― 传入要转换字串的空间大小（包含结尾符号）
 * 出口参数：
 *          pcTargetStr     ―― 目标字符串
 *          pnLen           ―― 返回转换成功时字符串的长度
 *                              转换失败且pnLen非空时为-1
 * 返 回 值：
 *          ISO8583_SUCC          ―― 成功
 *          ISO8583_FAIL       ―― 失败,TargetIsoStru或者pcSourceStr或者pnLen为空
 *                          ―― 调用HexToAsc出错
 *                          ―― 指定的成员无效
 *                          ―― 传入的pcSourceStr空间不够
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *************************************************************************************************/
int PubGetBit(STISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen)
{
    if (NULL == pSourceIsoStru || NULL == pcTargetStr || NULL == pnLen)
    {
        return ISO8583_FAIL;
    }

    if (nPosition > 0 && 0 == pSourceIsoStru->sBitmap[nPosition - 1])
    {
        pcTargetStr[0] = '\0';
        *pnLen = 0;
        return ISO8583_SUCC;
    }

    if (*pnLen >= pSourceIsoStru->fld[nPosition].nLen)
    {
        memcpy( pcTargetStr,
				pSourceIsoStru->szDatabuf + pSourceIsoStru->fld[nPosition].nOff,
				pSourceIsoStru->fld[nPosition].nLen);

        if (*pnLen > pSourceIsoStru->fld[nPosition].nLen)
		{
            pcTargetStr[pSourceIsoStru->fld[nPosition].nLen] = '\0';
		}
        *pnLen = pSourceIsoStru->fld[nPosition].nLen;
        return ISO8583_SUCC;
    }
    else
    {
        *pnLen = -1;
        return ISO8583_FAIL;
    }
	return ISO8583_SUCC ;
}

/*****************************************************************************************
 * 函数名称：PubSetBit
 * 函数介绍：将字符串pcSourceStr按指定长度nStrLen填充到结构TargetIsoStru中指定成员
 * 函数描述：
 * 入口参数：
 *          nPosition       ―― 指定的ISO_data结构中的第N个成员
 *          pcSourceStr     ―― 打包数据
 *          nStrLen         ―― pcSourceStr 的长度
 * 出口参数：
 *          pTargetIsoStru  ―― 待填充的目标结构ISO_data 结构的指针
 * 返 回 值：
 *          ISO8583_SUCC          ―― 设置成功或者nStrLen 等于0
 *          ISO8583_FAIL       ―― 失败，TargetIsostru或者pcSourceStr为NULL
 *                          ―― 指定的成员无效
 *                          ―― 调用AscToHex出错
 *                          ―― ISO结构累计长度超过MAXISOLEN
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ********************************************************************************************/
int PubSetBit(STISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen)
{
    unsigned char szBuf[MAX_FIELD_LEN];

    PubDelOneBit(pTargetIsoStru, nPosition);

    if (nPosition > 1 && nPosition <= 196)
	{
        pTargetIsoStru->sBitmap[nPosition - 1] = 1;
	}

    if (nPosition >= 65 && nPosition <= 128)
	{
        pTargetIsoStru->sBitmap[0] = 1;
	}
    else
	{
		if (nPosition > 128)
		{
			pTargetIsoStru->sBitmap[64] = 1;
		}
	}

    if (nStrLen > MAX_FIELD_LEN)
	{
        nStrLen = MAX_FIELD_LEN;
	}

       /* because the input maybe too long, we call stiso8583_def.pack
          to valid the input data. and pTargetIsoStru->fld[nPosition].nLen
          will be updated in function unpack();*/
    pTargetIsoStru->fld[nPosition].nOff = pTargetIsoStru->nUsed;
    memcpy(pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nPosition].nOff, pcSourceStr, nStrLen);
    pTargetIsoStru->fld[nPosition].nLen = nStrLen;

    if ( (0 >= stiso8583_def[nPosition].pPack( &stiso8583_def[nPosition],
												pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nPosition].nOff,
												pTargetIsoStru->fld[nPosition].nLen, szBuf))
		|| (0 >= stiso8583_def[nPosition].pUnpack( &stiso8583_def[nPosition],
													pTargetIsoStru->szDatabuf + pTargetIsoStru->fld[nPosition].nOff,
													&pTargetIsoStru->fld[nPosition].nLen, szBuf)) )
	{
			pTargetIsoStru->fld[nPosition].nLen = 0;
			pTargetIsoStru->fld[nPosition].nOff = 0;
			return ISO8583_FAIL;
	}
    pTargetIsoStru->nUsed += pTargetIsoStru->fld[nPosition].nLen;
    return ISO8583_SUCC;
}


#if 0
/****************************************************************************************************
 * 函数名称：PubFormatDisp8583
 * 函数介绍：按域显示8583数据包内容
 * 函数描述：
 * 入口参数：pIsoStru ―― 待显示的ISO 结构
 * 出口参数：无
 * 返 回 值：无
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 ****************************************************************************************************/
void PubFormatDisp8583(STISODataStru *pIsoStru)
{
    int nI, nStrLen;
    char szDispStr[MAX_FIELD_LEN],szPosition[6];
    unsigned char cRow, cLine;
    unsigned char uiKey;

    nStrLen = sizeof(szDispStr);
    if (PubGetBit(pIsoStru, 0,szDispStr, &nStrLen) != ISO8583_SUCC)
	{
        return;
	}
    clrscr();
    //ProDisp8x8str(1,1,"[MSG_ID]",8);
    //ProDisp8x8str(1,2, szDispStr,16);
	PubDisplayStr( 1, 1, 1, "%s", "[MSG_ID]" );
	PubDisplayStr( 1, 2, 1, "%s", szDispStr ) ;
    cRow = 3;
    for (nI = 2; nI <= 128; nI++)
    {
        nStrLen = sizeof(szDispStr);
        if (PubGetBit(pIsoStru, nI,szDispStr, &nStrLen) != ISO8583_SUCC)
		{
            continue;
		}
        if (cRow >= 9)
        {
            uiKey = getkeycode(0);
            if (uiKey == ESC)
			{
                return;
			}
            clrscr();
            cRow = 1;
        }
        sprintf(szPosition,"[%03d]",nI);
        ProDisp8x8str(1,cRow, szPosition,5);
        cLine = cRow + 1;
        ProDisp8x8str(1,cLine, szDispStr,16);
        cRow = cLine + 1;
    }
    getkeycode(0);
    return;
}
#endif
/**************************************************************************************
 * 函数名称：PubSetDefine8583
 * 函数介绍：设置8583数据包定义
 * 函数描述：
 * 入口参数：nPosition－－域号
 *           nMaxLen－－最大长度
 *           LenType－－长度类型
 *           AlignType－－对齐类型
 *           chPadChar－－补齐字符
 *           PackFunc－－打包函数
 *           UnpackFunc－－解包函数
 * 出口参数：无
 * 返 回 值：ISO8583_SUCC－－成功
 *           ISO8583_FAIL－－失败
 * 修改者  ：徐诗良
 * 修改日期：2006-09-29
 *****************************************************************************************/
int PubSetDefine8583(int nPosition, int nMaxLen, LENGTH_TYPE  LenType,
					 ALIGN_TYPE AlignType, char chPadChar, FIELD_PACKAGER_PACK PackFunc,
					 FIELD_PACKAGER_UNPACK UnpackFunc)
{
    if (nPosition < 1 || nPosition > 256 || nMaxLen > 999)
    {
        return ISO8583_FAIL;
    }
    stiso8583_def[nPosition - 1].nLen = nMaxLen;
    stiso8583_def[nPosition - 1].lentype = LenType;
    stiso8583_def[nPosition - 1].align = AlignType;
    stiso8583_def[nPosition - 1].cPadchar = chPadChar;
    stiso8583_def[nPosition - 1].pPack = PackFunc;
    stiso8583_def[nPosition - 1].pUnpack = UnpackFunc;

    return ISO8583_SUCC;
}

/******************************************************************************************
 * 函数名称：PubImport8583
 * 函数介绍：导入8583结构
 * 函数描述：
 * 入口参数：pNewDef －－ 要导入的8583结构
 * 出口参数：无
 * 返 回 值：ISO8583_SUCC－－成功
 *           ISO8583_FAIL－－失败
 *	修改者  ：徐诗良
 *	修改日期：2006-09-29
 *******************************************************************************************/
int PubImport8583(const struct STISO_FIELD_DEF *pNewDef)
{
    int nBitNum;
    int nRet;

    if (pNewDef == NULL)
	{
        return ISO8583_FAIL;
	}
    for (nBitNum = 0; nBitNum <= 128; nBitNum++, pNewDef++)
    {
        nRet = PubSetDefine8583(nBitNum + 1, pNewDef->nLen, pNewDef->lentype,
								pNewDef->align, pNewDef->cPadchar, pNewDef->pPack,
								pNewDef->pUnpack);
        if (nRet == ISO8583_FAIL)
		{
            return ISO8583_FAIL;
		}
    }
    stiso8583_fldcalc.pSet = PubFLBLengthSet; //重置在打包、解包时长度计算方法，默认BCD wwt 2011-4-11
    stiso8583_fldcalc.pGet = PubFLBLengthGet; //重置在打包、解包时长度计算方法，默认BCD wwt 2011-4-11
    return ISO8583_SUCC;
}

/******************************************************************************
*	函数名称：PubSetFieldLengthCalc
*	函数介绍：导入8583域长度计算函数结构
*	函数描述：
* 	入口参数：pNewCalc －－ 要导入的8583域长度计算函数结构
* 	出口参数：无
* 	返 回 值：ISO8583_SUCC－－成功
*             ISO8583_FAIL－－失败
*	修改者  ：徐诗良
*	修改日期：2006-09-29
*******************************************************************************/

int PubSetFieldLengthCalc(const struct STISO_FIELD_LENGTH_CALC *pNewCalc)
{
	if (pNewCalc == NULL)
	{
        return ISO8583_FAIL;
	}
	stiso8583_fldcalc.pSet = pNewCalc->pSet;
    stiso8583_fldcalc.pGet = pNewCalc->pGet;
    return ISO8583_SUCC;
}

/******************************************************************************
*	函数名称：PubCopyIso
*	函数介绍：复制一个STISODataStru结构
*	函数描述：
* 	入口参数：
*          	  pSourceIsoStru  ―― 源ISO_data结构的指针
* 	出口参数：
*             pTargetIsoStru  ―― 待填充的目标结构ISO_data 结构的指针
* 	返 回 值：
*          	  ISO8583_SUCC          ―― 设置成功
*             ISO8583_FAIL       ―― 失败，pSourceIsoStru或者pTargetIsoStru为空
*	修改者  ：徐诗良
*	修改日期：2006-09-29
*******************************************************************************/

int PubCopyIso(STISODataStru *pSourceIsoStru, STISODataStru *pTargetIsoStru)
{
    if (pSourceIsoStru == NULL || pTargetIsoStru == NULL)
	{
        return ISO8583_FAIL;
	}
    memcpy(pTargetIsoStru, pSourceIsoStru, sizeof(STISODataStru));
    return ISO8583_SUCC;
}


