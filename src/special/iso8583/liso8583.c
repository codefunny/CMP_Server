/*
* �´�½���Թɷ����޹�˾ ��Ȩ����(c) 2006-2008
*
* POS��׼�湫����
* 8583����ģ��  --- liso8583.c
* ��    �ߣ�    ��ʫ��
* ��    �ڣ�    2006-09-10
* ����޸��ˣ�  ��ʫ��
* ����޸����ڣ�2006-09-30
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
/*�ڲ���������*/
static int ProAddPadChar(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf,
						int nFieldlen, unsigned char *poutdata, unsigned char cHead);

/****************************************************************************************
 * �������ƣ�ProAddPadChar
 * �������ܣ�Ϊһ��ISO������ݽ��в������
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 *           cHead    �D�D �Ƿ���ͷ�ַ���������͵�����C/D��־��
 * ���ڲ�����poutdata �D�D ���ڽ��ղ��������ݵĻ�����
 * �� �� ֵ���������ַ�������
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
*	�������ƣ�PubAscToHex
*	�������ܣ���AscII����ַ���ת����ѹ����HEX��ʽ
*	������������ż�����ȵ��ַ������ݶ��뷽ʽ����ȡ��0���Ҳ�F��ʽ
*	��ڲ�����pszAsciiBuf	---- ��ת����ASCII�ַ���
*			  nLen			---- �������ݳ���(ASCII�ַ����ĳ���)
*			  cType			---- ���뷽ʽ  0�������  1���Ҷ���
*	���ڲ�����pszBcdBuf		---- ת�������HEX����
*	�� �� ֵ��ISO8583_SUCC 0 �ɹ�; ISO8583_FAIL -1 ʧ��
*	�� �� �ߣ��µ�Ө
*	�޸����ڣ�2006-09-11
*******************************************************************************/
static int PubAscToHex (const unsigned char* pszAsciiBuf, int nLen, char cType, unsigned char* pszBcdBuf)
{
	int i = 0;
	char cTmp, cTmp1;

	if (pszAsciiBuf == NULL)
	{
		return ISO8583_FAIL;
	}

	if (nLen & 0x01 && cType)/*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
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
*	�������ƣ�PubHexToAsc
*	�������ܣ���HEX������ת����AscII���ַ���
*	����������
*	��ڲ�����pszBcdBuf		---- ��ת����Hex����
*			  nLen			---- ת�����ݳ���(ASCII�ַ����ĳ���)
*			  cType			---- ���뷽ʽ  1�������  0���Ҷ���
*	���ڲ�����pszAsciiBuf	---- ת�������AscII������
*	�� �� ֵ��ISO8583_SUCC 0 �ɹ�; ISO8583_FAIL -1 ʧ��
*	�� �� �ߣ��µ�Ө
*	�޸����ڣ�2006-09-11
*******************************************************************************/
static int PubHexToAsc (const unsigned char* pszBcdBuf, int nLen, char cType, unsigned char* pszAsciiBuf)
{
	int i = 0;

	if (pszBcdBuf == NULL)
	{
		return ISO8583_FAIL;
	}
	if (nLen & 0x01 && cType) /*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
	{                           /*0��1��*/
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
 * �������ƣ�PubFLBLengthSet
 * �������ܣ�����ISO������ͼ���8583�����еĳ����ֽ�(BCD��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           nLen      �D�D �����ݵĳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����BCD����ʽ���糤��104������ʾΪ 0x01, 0x04,��2���ֽ�, �����9����ʾΪ0x09,��1���ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFLBLengthGet
 * �������ܣ�����ISO������ͺ�8583�����еĳ����ֽڣ����㱨������ĳ���(BCD��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D �������ĵ�����ָ��
 * ���ڲ�����plen     �D�D ���ڽ��ռ����ĳ���
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����BCD����ʽ���糤��104������ʾΪ 0x01, 0x04,��2���ֽ�, �����9����ʾΪ0x09,��1���ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFLALengthSet
 * �������ܣ�����ISO������ͼ���8583�����еĳ����ֽ�(ASC��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           nLen      �D�D �����ݵĳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����ASC����ʽ���糤��104������ʾΪ 0x31, 0x30, 0x34,��3���ֽ�, �����9����ʾΪ0x30, 0x39�����ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFLALengthGet
 * �������ܣ�����ISO������ͺ�8583�����еĳ����ֽڣ����㱨������ĳ���(ASC��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D �������ĵ�����ָ��
 * ���ڲ�����plen     �D�D ���ڽ��ռ����ĳ���
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����ASC����ʽ���糤��104������ʾΪ 0x31, 0x30, 0x34,��3���ֽ�, �����9����ʾΪ0x30, 0x39�����ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFLHLengthSet
 * �������ܣ�����ISO������ͼ���8583�����еĳ����ֽ�(HEX��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           nLen      �D�D �����ݵĳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����HEX����ʽ���糤��104������ʾΪ 0x00, 0x68,��2���ֽ�, �����9����ʾΪ0x09,��1���ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFLHLengthGet
 * �������ܣ�����ISO������ͺ�8583�����еĳ����ֽڣ����㱨������ĳ���(HEX��ʽ)
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D �������ĵ�����ָ��
 * ���ڲ�����plen     �D�D ���ڽ��ռ����ĳ���
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * ˵    ����HEX����ʽ���糤��104������ʾΪ 0x00, 0x68,��2���ֽ�, �����9����ʾΪ0x09,��1���ֽ�
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPAMsgIDPack
 * �������ܣ�Ϊһ��ISO������ݽ��д����MSG_ID��ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 *****************************************************************************************************/
int PubFPAMsgIDPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int nFieldlen,
					unsigned char *poutdata)
{
    int nLen = pthis->nLen;

    memcpy(poutdata, pfieldbuf, nLen);
    return nLen;
}

/***************************************************************************************************************
 * �������ƣ�PubFPAMsgIDUnpack
 * �������ܣ�Ϊһ��ISO������ݽ��н����MSG_ID��ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBMsgIDpack
 * �������ܣ�Ϊһ��ISO������ݽ��д����MSG_ID��BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBMsgIDUnpack
 * �������ܣ�Ϊһ��ISO������ݽ��н����MSG_ID��BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBBitmapPack
 * �������ܣ�Ϊһ��ISO������ݽ��д����BITMAPλͼ��BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBBitmapUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н����BITMAPλͼ��BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPABitmapPack
 * �������ܣ�Ϊһ��ISO������ݽ��д����BITMAPλͼ��ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen      �D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
	PubHexToAsc(sBitmap, nBitmaplen*2, 0, poutdata);  //ԭ������/���Ū����,wwt 2011-4-11
    return nBitmaplen * 2;
}

/***************************************************************************************************************
 * �������ƣ�PubFPABitmapUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н����BITMAPλͼ��ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPACharPack
 * �������ܣ�Ϊһ��ISO������ݽ��д�����ַ�������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPACharUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н�����ַ�������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBCharPack
 * �������ܣ�Ϊһ��ISO������ݽ��д�����ַ�������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nLen      �D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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

    /* �����������ȵģ������Ҳ�������ַ�ѹ�� */
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
 * �������ƣ�PubFPBCharUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н�����ַ�������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPADigitPack
 * �������ܣ�Ϊһ��ISO������ݽ��д��������������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPADigitUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н��������������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBDigitPack
 * �������ܣ�Ϊһ��ISO������ݽ��д��������������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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

    /* �����������ȵģ������Ҳ�������ַ�ѹ�� */
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
 * �������ƣ�PubFPBDigitUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н��������������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBAmountPack
 * �������ܣ�Ϊһ��ISO������ݽ��д�������������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBAmountUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н�������������ASC����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBBinaryPack
 * �������ܣ�Ϊһ��ISO������ݽ��д����������������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           nFieldlen�D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFPBBinaryUnPack
 * �������ܣ�Ϊһ��ISO������ݽ��н����������������BCD����ʽ��
 * ����������
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           pfieldlen�D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubDelAllBit
 * �������ܣ����ISO_data�ṹ�����г�Ա����ֵ
 * ����������
 * ��ڲ�����pIsoStru   �D�D Դ���ݽṹISO_data �ṹָ��
 * ���ڲ�����pIsoStru   �D�D ��պ��ISO_data�ṹָ��
 * �� �� ֵ��ISO8583_SUCC     �D�D �ɹ�
 *           ISO8583_FAIL  �D�D ʧ�ܣ�IsoStruΪ��
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
* �������ƣ�PubDelOneBit
* �������ܣ����IsoStru�ṹ�е�nPosition���ֵ��ͬʱ���λͼ�е���Ӧ��־
* ����������
* ��ڲ�����
*          pIsoStru        �D�D Դ���ݽṹISO_data �ṹָ��
*          nPosition       �D�D ISO_data�ṹ�еĵ�nPosition����Ա
* ���ڲ�����
*          pIsoStru        �D�D ������ISO_data�ṹָ��
* �� �� ֵ��
*          ISO8583_SUCC          �D�D �ɹ�
*          ISO8583_FAIL       �D�D ʧ�ܣ�IsoStruΪNULL
*                          �D�D ��Ч�ĳ�Ա�ţ�nPosition <0 ���� >=256��
* �޸���  ����ʫ��
* �޸����ڣ�2006-09-29
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
    if (nPosition != 1) //�����޷�����128��
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
 * �������ƣ�PubStrToIso
 * �������ܣ�ת���ַ���ΪISO�ṹ
 *	����������
 * ��ڲ�����
 *          pcSourceStr     �D�D ת����Դ�ַ���
 *          nStrLen         ���� ���ݳ���
 * ���ڲ�����
 *          pTargetIsoStru  �D�D ת���Ľ��ISO�ṹ
 *          pnPosition      �D�D ����ΪNULLʱ������;
 *                              ������ת������ʱ�����ţ����Է���ERRFIELDLENʱ��Ч��
 *                              ��������¾�Ϊ-1
 * �� �� ֵ��
 *          ISO8583_SUCC          �D�D ת���ɹ�
 *          ISO8583_FAIL       �D�D ת��ʧ�ܣ�pcSourceStr����TargetIsoStruΪ��
 *                          �D�D �������
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 *****************************************************************************************************/
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition)
{
    int nI = 0;        /* ѭ����                       */
    int unpacked = 0; /* 8583�����Ѿ����⿪���ֽ���   */
    int nUsed = 0;     /* ��ǰ����8583����ռ�õ��ֽ��� */

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
 * �������ƣ�PubIsoToStr
 * �������ܣ���ISO�ṹת�����޷����ַ���
 * ����������
 * ��ڲ�����
 *          pSourceIsoStru  �D�D ת����Դ���ݽṹ
 *          pnLen           �D�D ����pcTargetStr�Ŀռ��С��������������
 * ���ڲ�����
 *          pcTargetStr     �D�D ת���Ľ���ַ���(BCD��)
 *          pnLen           �D�D ���ת���ɹ�����ַ������ȡ�
 *                               ���ʧ�ܣ�����Ϊ-1��
 * �� �� ֵ��
 *          ISO8583_SUCC          �D�D ת���ɹ�
 *          ISO8583_FAIL       �D�D ת��ʧ�ܣ�pcTargetStr����pSourceIsoStruΪ��
 *                          �D�D ���ʧ��
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 ********************************************************************************************/
int PubIsoToStr(char *pcTargetStr, STISODataStru *pSourceIsoStru, int *pnLen)
{
    int nI = 0;        /* ѭ����                       */
    int packed = 0;   /* 8583�����Ѿ����⿪���ֽ���   */
    int nUsed = 0;     /* ��ǰ����8583����ռ�õ��ֽ��� */

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
 * �������ƣ�PubGetBit
 * �������ܣ����ṹISO��ָ����Ա��ȡ���ַ���szTargetStr��
 * ����������
 * ��ڲ�����
 *          pSourceIsoStru  �D�D Դ���ݽṹISO_data �ṹָ��
 *          nPosition       �D�D ISO_data�ṹ�еĵ�N����Ա
 *          pnLen           �D�D ����Ҫת���ִ��Ŀռ��С��������β���ţ�
 * ���ڲ�����
 *          pcTargetStr     �D�D Ŀ���ַ���
 *          pnLen           �D�D ����ת���ɹ�ʱ�ַ����ĳ���
 *                              ת��ʧ����pnLen�ǿ�ʱΪ-1
 * �� �� ֵ��
 *          ISO8583_SUCC          �D�D �ɹ�
 *          ISO8583_FAIL       �D�D ʧ��,TargetIsoStru����pcSourceStr����pnLenΪ��
 *                          �D�D ����HexToAsc����
 *                          �D�D ָ���ĳ�Ա��Ч
 *                          �D�D �����pcSourceStr�ռ䲻��
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubSetBit
 * �������ܣ����ַ���pcSourceStr��ָ������nStrLen��䵽�ṹTargetIsoStru��ָ����Ա
 * ����������
 * ��ڲ�����
 *          nPosition       �D�D ָ����ISO_data�ṹ�еĵ�N����Ա
 *          pcSourceStr     �D�D �������
 *          nStrLen         �D�D pcSourceStr �ĳ���
 * ���ڲ�����
 *          pTargetIsoStru  �D�D ������Ŀ��ṹISO_data �ṹ��ָ��
 * �� �� ֵ��
 *          ISO8583_SUCC          �D�D ���óɹ�����nStrLen ����0
 *          ISO8583_FAIL       �D�D ʧ�ܣ�TargetIsostru����pcSourceStrΪNULL
 *                          �D�D ָ���ĳ�Ա��Ч
 *                          �D�D ����AscToHex����
 *                          �D�D ISO�ṹ�ۼƳ��ȳ���MAXISOLEN
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubFormatDisp8583
 * �������ܣ�������ʾ8583���ݰ�����
 * ����������
 * ��ڲ�����pIsoStru �D�D ����ʾ��ISO �ṹ
 * ���ڲ�������
 * �� �� ֵ����
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubSetDefine8583
 * �������ܣ�����8583���ݰ�����
 * ����������
 * ��ڲ�����nPosition�������
 *           nMaxLen������󳤶�
 *           LenType������������
 *           AlignType������������
 *           chPadChar���������ַ�
 *           PackFunc�����������
 *           UnpackFunc�����������
 * ���ڲ�������
 * �� �� ֵ��ISO8583_SUCC�����ɹ�
 *           ISO8583_FAIL����ʧ��
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
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
 * �������ƣ�PubImport8583
 * �������ܣ�����8583�ṹ
 * ����������
 * ��ڲ�����pNewDef ���� Ҫ�����8583�ṹ
 * ���ڲ�������
 * �� �� ֵ��ISO8583_SUCC�����ɹ�
 *           ISO8583_FAIL����ʧ��
 *	�޸���  ����ʫ��
 *	�޸����ڣ�2006-09-29
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
    stiso8583_fldcalc.pSet = PubFLBLengthSet; //�����ڴ�������ʱ���ȼ��㷽����Ĭ��BCD wwt 2011-4-11
    stiso8583_fldcalc.pGet = PubFLBLengthGet; //�����ڴ�������ʱ���ȼ��㷽����Ĭ��BCD wwt 2011-4-11
    return ISO8583_SUCC;
}

/******************************************************************************
*	�������ƣ�PubSetFieldLengthCalc
*	�������ܣ�����8583�򳤶ȼ��㺯���ṹ
*	����������
* 	��ڲ�����pNewCalc ���� Ҫ�����8583�򳤶ȼ��㺯���ṹ
* 	���ڲ�������
* 	�� �� ֵ��ISO8583_SUCC�����ɹ�
*             ISO8583_FAIL����ʧ��
*	�޸���  ����ʫ��
*	�޸����ڣ�2006-09-29
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
*	�������ƣ�PubCopyIso
*	�������ܣ�����һ��STISODataStru�ṹ
*	����������
* 	��ڲ�����
*          	  pSourceIsoStru  �D�D ԴISO_data�ṹ��ָ��
* 	���ڲ�����
*             pTargetIsoStru  �D�D ������Ŀ��ṹISO_data �ṹ��ָ��
* 	�� �� ֵ��
*          	  ISO8583_SUCC          �D�D ���óɹ�
*             ISO8583_FAIL       �D�D ʧ�ܣ�pSourceIsoStru����pTargetIsoStruΪ��
*	�޸���  ����ʫ��
*	�޸����ڣ�2006-09-29
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


