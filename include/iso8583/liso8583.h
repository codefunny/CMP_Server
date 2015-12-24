/*
* �´�½���Թ�˾ ��Ȩ����(c) 2006-2008
*
* POS��׼�湫����
* ����ģ��ͷ�ļ�  --- liso8583.h
* ��    �ߣ�    ��ʫ�� 
* ��    �ڣ�    2006-09-11
* ����޸��ˣ�  ��ʫ��
* ����޸����ڣ�2006-09-30
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

/* ÿ�������󳤶� */
#define MAX_FIELD_LEN 1024 

/* ������󳤶� */
#define MAX_MSG_LEN 4096

/* ����뷽ʽ */
typedef enum ALIGN_TYPE 
{
    ALN_LEFT   = 0,
    ALN_RIGHT  = 1
} ALIGN_TYPE;

/* �������� */
typedef enum LENGTH_TYPE 
{
    LEN_FIX    = 0,
    LEN_LLVAR  = 1,
    LEN_LLLVAR = 2
} LENGTH_TYPE;

typedef struct STISO_FIELD STISO_FIELD;
typedef struct STISO_FIELD_DEF STISO_FIELD_DEF;

/***************************************************************************************
 * �������ƣ�*FIELD_PACKAGER_PACK
 * �������ܣ���8583����д���������ش��������ݺͳ���
 * ���������� 
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pfieldbuf�D�D ��Ҫ�����8583������ָ��
 *           len      �D�D ��Ҫ�����8583�����ݳ���
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����
 * �� �� ֵ�����������ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 ***************************************************************************************/
typedef int (*FIELD_PACKAGER_PACK) (STISO_FIELD_DEF *pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/*****************************************************************************************
 * �������ƣ�*FIELD_PACKAGER_UNPACK
 * �������ܣ�����������������ݽ����8583����
 * ���������� 
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D ���и����ĵ����ݻ�����
 * ���ڲ�����pfieldbuf�D�D ������8583�����ݻ�����
 *           plen     �D�D ���������ݳ���
 * �� �� ֵ�����ʱ��pindata��ʹ�õ����ݳ���
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 ******************************************************************************************/
typedef int (*FIELD_PACKAGER_UNPACK) (STISO_FIELD_DEF *pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/*******************************************************************************************************
 * �������ƣ�*FIELD_LENGTH_SET
 * �������ܣ�����ISO������ͼ���8583�����еĳ����ֽ�
 * ���������� 
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           len      �D�D �����ݵĳ��� 			
 * ���ڲ�����poutdata �D�D ���ڽ��ռ��������ݵĻ�����   
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 ********************************************************************************************************/
typedef void (*FIELD_LENGTH_SET) (STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/*********************************************************************************************************
 * �������ƣ�*FIELD_LENGTH_GET
 * �������ܣ�����ISO������ͺ�8583�����еĳ����ֽڣ����㱨������ĳ���
 * ���������� 
 * ��ڲ�����pthis    �D�D ISO��������ͽṹָ��
 *           pindata  �D�D �������ĵ�����ָ��
 * ���ڲ�����plen     �D�D ���ڽ��ռ����ĳ���
 *           pcount   �D�D �����ֽ���������Ϊ0��LLVAR��Ϊ1��LLLVAR��Ϊ2��
 * �� �� ֵ����
 * �޸���  ����ʫ��
 * �޸����ڣ�2006-09-29
 *********************************************************************************************************/
typedef void (*FIELD_LENGTH_GET) (STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);


/* ISO 8583 �����򳤶ȼ��㺯���ṹ */
 struct STISO_FIELD_LENGTH_CALC
{
    FIELD_LENGTH_SET	pSet;	 /* ���ó���ʱ���õĺ��� */
    FIELD_LENGTH_GET	pGet;    /* ȡ������ʱ���õĺ��� */
}; 

/* ISO 8583 ������ṹ */
struct STISO_FIELD
{
    int    nLen;
    int    nOff;  /* ָ��iso->databuf��������ƫ�ƣ������в����������͵ļ����ֽ� */
};

/* ISO 8583 �������� */
struct STISO_FIELD_DEF
{
    int                     nLen;        /* ��������󳤶� */
    LENGTH_TYPE		        lentype;    /* �����򳤶�����: 1--LLVAR�� 2--LLLVAR�� */
    ALIGN_TYPE              align;      /* �������� */
    char                    cPadchar;    /* �����ַ� */	
    FIELD_PACKAGER_PACK		pPack;	    /* �������ָ�� */
    FIELD_PACKAGER_UNPACK	pUnpack;     /* �������ָ�� */
};  

/* ISO 8583 ���Ľṹ���� */
typedef struct STISODataStru
{
    STISO_FIELD		fld[256];
    char			sBitmap[256];
    unsigned char 	szDatabuf[MAX_MSG_LEN + MAX_FIELD_LEN]; /* ����һ����ĳ��ȣ���ֹ��� */
    int        		nUsed;
} STISODataStru;

/* ����Ϊ�䳤�򳤶ȼ��㺯�� */
/* ASC����ʽ���糤��104������ʾΪ 0x31, 0x30, 0x34,��3���ֽ�, �����9����ʾΪ0x30, 0x39�����ֽ� */
void PubFLALengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLALengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* BCD����ʽ���糤��104������ʾΪ 0x01, 0x04,��2���ֽ� */
void PubFLBLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLBLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* ʮ��������ʽ���糤��104������ʾΪ 0x00, 0x68,��2���ֽ� */
void PubFLHLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void PubFLHLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/* ������������ʵ�� */
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

/* Iso8583 �������� */
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
/*�������
1.��0��128��64,˳�����
int PubSetBit(STISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);
���ַ���pcSourceStr��ָ������nStrLen��䵽�ṹTargetIsoStru��ָ����Ա
2.���ú���
int PubIsoToStr(char *pcTargetStr, STISODataStru *pSourceIsoStru, int *pnLen)
��ISO�ṹpSourceIsoStruת���ɿɷ��͵İ����޷����ַ���pcTargetStr
*/
/*�������
1.���ú���
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition)
�����յ��ı���pcSourceStrתΪISO�ṹ
2.���ú���
PubGetBit(STISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen)
��ISO�ṹpSourceIsoStru����ȡ��Ӧ��nPosition�������.
*/
#ifdef __cplusplus
	}
#endif

#endif /* End of __ISO8583_H__ */
/* End of liso8583.h */
