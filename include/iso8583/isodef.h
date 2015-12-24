/*
* �´�½���Թ�˾ ��Ȩ����(c) 2006-2008
*
* POS��׼�湫����
* ����ģ��ͷ�ļ�  --- ISODef.h
* ��    �ߣ�    ��ʫ�� 
* ��    �ڣ�    2006-09-11
* ����޸��ˣ�  ��ʫ��
* ����޸����ڣ�2006-09-30
*/
#ifndef _ISODEF_H_
#define _ISODEF_H_
#include "liso8583.h"

/******************************************************************************************************** 
 * �䳤�ֶγ��ȼ��㺯����, �ڴ�������ʱ���ڼ��㡢ȡ���䳤��ĳ���
 * ���õĺ������У�(PubFLBLengthSet-PubFLBLengthGet)��(PubFLALengthSet-PubFLALengthGet)��(PubFLHLengthSet-PubFLHLengthGet)
 * �ֱ�ΪBCD����ʽ��ASC����ʽ��ʮ��������ʽ����3�ŵ���Ϣ�򳤶�104�����ֱ��ʾΪ 
 * "\x31\x30\x34" (ASC), "\x01\x04" (BCD), "\x00\x68" (HEX);
 * ����ĳЩ�����Ӧ�ã������Լ���д���������������������
 * ����������FLB** �е�FLΪField Length����д��B��ʾBCD��
 ********************************************************************************************************/ 
static struct STISO_FIELD_LENGTH_CALC stiso8583_fldcalc = {PubFLBLengthSet, PubFLBLengthGet};

/****************************************************************************************************** 
 * 8583 �����ֶζ����
 * �ֱ�Ϊ    ���ȣ��������ͣ��������ͣ������ַ�������������������
 * ���õĺ������У�(PubFPAMsgIDPack-PubFPAMsgIDUnpack)��(PubFPBMsgIDpack-PubFPBMsgIDUnpack);
 *  (PubFPABitmapPack-PubFPABitmapUnPack)��(PubFPBBitmapPack-PubFPBBitmapUnPack)��
 *  (PubFPACharPack-PubFPACharUnPack)��(PubFPBCharPack-PubFPBCharUnPack)��
 *  (PubFPADigitPack-PubFPADigitUnPack)��(PubFPBDigitPack-PubFPBDigitUnPack)��       
 *  (PubFPBAmountPack-PubFPBAmountUnPack)��(PubFPBBinaryPack-PubFPBBinaryUnPack)
 * �ֱ�ΪASC��BCD��ʽ�Ĵ����������������� FP*MsgID*, FP*Bitmap*ֻ��0��1��65��ʹ��
 * FP*Binary* һ��ֻ��52��(PINBLOCK)��64��128��(MAC)��ʹ��
 * ʹ�ý϶����FP*Char*����
 * ����������FLB** �е�FLΪField Packager����д��B��ʾBCD��
 **********************************************************************************************************/
static struct	STISO_FIELD_DEF stiso8583_def[] = {
	{  4, LEN_FIX,   ALN_LEFT,  '\0', PubFPBMsgIDpack,  PubFPBMsgIDUnpack},  /* 0   "MESSAGE TYPE INDICATOR" */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBitmapPack, PubFPBBitmapUnPack}, /* 1   "BIT MAP" */
	{ 19, LEN_LLVAR, ALN_LEFT,  '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 2   "PAN - PRIMARY ACCOUNT NUMBER" */
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 3   "PROCESSING CODE" */
	{ 12, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 4   "AMOUNT, TRANSACTION" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 5   "AMOUNT, SETTLEMENT" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 6   "AMOUNT, CARDHOLDER BILLING" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 7   "TRANSMISSION DATE AND TIME" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 8   "AMOUNT, CARDHOLDER BILLING FEE" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 9   "CONVERSION RATE, SETTLEMENT" */
	{  2, LEN_FIX,   ALN_RIGHT, ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 10  "CONVERSION RATE, CARDHOLDER BILLING" */
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 11  "SYSTEM TRACE AUDIT NUMBER" */
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 12  "TIME, LOCAL TRANSACTION" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 13  "DATE, LOCAL TRANSACTION" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 14  "DATE, EXPIRATION" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 15  "DATE, SETTLEMENT" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 16  "DATE, CONVERSION" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 17  "DATE, CAPTURE" */
	{  3, LEN_FIX,   ALN_RIGHT, ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 18  "MERCHANTS TYPE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 19  "ACQUIRING INSTITUTION COUNTRY CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 20  "PAN EXTENDED COUNTRY CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 21  "FORWARDING INSTITUTION COUNTRY CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 22  "POINT OF SERVICE ENTRY MODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 23  "CARD SEQUENCE NUMBER" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 24  "NETWORK INTERNATIONAL IDENTIFIEER" */
	{  2, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 25  "POINT OF SERVICE CONDITION CODE" */
	{  2, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 26  "POINT OF SERVICE PIN CAPTURE CODE" */
	{  2, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 27  "AUTHORIZATION IDENTIFICATION RESP LEN" */
	{ 28, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 28  "AMOUNT, TRANSACTION FEE" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBAmountPack, PubFPBAmountUnPack}, /* 29  "AMOUNT, SETTLEMENT FEE" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBAmountPack, PubFPBAmountUnPack}, /* 30  "AMOUNT, TRANSACTION PROCESSING FEE" */
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBAmountPack, PubFPBAmountUnPack}, /* 31  "AMOUNT, SETTLEMENT PROCESSING FEE" */
	{ 11, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 32  "ACQUIRING INSTITUTION IDENT CODE" */
	{ 11, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 33  "FORWARDING INSTITUTION IDENT CODE" */
	{ 28, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 34  "PAN EXTENDED" */
	{ 37, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 35  "TRACK 2 DATA" */
	{104, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 36  "TRACK 3 DATA" */
	{  6, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 37  "RETRIEVAL REFERENCE NUMBER" */
	{  8, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 38  "AUTHORIZATION IDENTIFICATION RESPONSE" */
	{  4, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 39  "RESPONSE CODE" */
	{  3, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 40  "SERVICE RESTRICTION CODE" */
	{  9, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 41  "CARD ACCEPTOR TERMINAL IDENTIFICACION" */
	{  8, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 42  "CARD ACCEPTOR IDENTIFICATION CODE"  */
	{ 40, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 43  "CARD ACCEPTOR NAME/LOCATION" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 44  "ADITIONAL RESPONSE DATA" */
	{ 76, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 45  "TRACK 1 DATA" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 46  "ADITIONAL DATA - ISO" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 47  "ADITIONAL DATA - NATIONAL" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 48  "ADITIONAL DATA - PRIVATE" */
	{  3, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 49  "CURRENCY CODE, TRANSACTION" */
	{  3, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 50  "CURRENCY CODE, SETTLEMENT" */
	{  3, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 51  "CURRENCY CODE, CARDHOLDER BILLING"    */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 52  "PIN DATA"    */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 53  "SECURITY RELATED CONTROL INFORMATION" */
	{120, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 54  "ADDITIONAL AMOUNTS" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 55  "RESERVED ISO" */
	{ 99, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 56  "RESERVED ISO" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 57  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 58  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 59  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 60  "RESERVED PRIVATE" */
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPACharPack,   PubFPACharUnPack},   /* 61  "RESERVED PRIVATE" */
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPACharPack,   PubFPACharUnPack},   /* 62  "RESERVED PRIVATE" */
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPACharPack,   PubFPACharUnPack},   /* 63  "RESERVED PRIVATE" */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 64  "MESSAGE AUTHENTICATION CODE FIELD" */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBitmapPack, PubFPBBitmapUnPack}, /* 65  "BITMAP, EXTENDED" */
	{  1, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 66  "SETTLEMENT CODE" */
	{  2, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 67  "EXTENDED PAYMENT CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 68  "RECEIVING INSTITUTION COUNTRY CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 69  "SETTLEMENT INSTITUTION COUNTRY CODE" */
	{  3, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 70  "NETWORK MANAGEMENT INFORMATION CODE" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 71  "MESSAGE NUMBER" */
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 72  "MESSAGE NUMBER LAST" */
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 73  "DATE ACTION" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 74  "CREDITS NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 75  "CREDITS REVERSAL NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 76  "DEBITS NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 77  "DEBITS REVERSAL NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 78  "TRANSFER NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 79  "TRANSFER REVERSAL NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 80  "INQUIRIES NUMBER" */
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 81  "AUTHORIZATION NUMBER" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 82  "CREDITS, PROCESSING FEE AMOUNT" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 83  "CREDITS, TRANSACTION FEE AMOUNT" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 84  "DEBITS, PROCESSING FEE AMOUNT" */
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 85  "DEBITS, TRANSACTION FEE AMOUNT" */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 86  "CREDITS, AMOUNT" */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 87  "CREDITS, REVERSAL AMOUNT" */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 88  "DEBITS, AMOUNT" */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 89  "DEBITS, REVERSAL AMOUNT" */
	{ 42, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 90  "ORIGINAL DATA ELEMENTS" */
	{  1, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 91  "FILE UPDATE CODE" */
	{  2, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 92  "FILE SECURITY CODE" */
	{  5, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 93  "RESPONSE INDICATOR" */
	{  7, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 94  "SERVICE INDICATOR" */
	{ 42, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 95  "REPLACEMENT AMOUNTS" */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 96  "MESSAGE SECURITY CODE" */
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBAmountPack, PubFPBAmountUnPack}, /* 97  "AMOUNT, NET SETTLEMENT" */
	{ 25, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 98  "PAYEE" */
	{ 11, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 99  "SETTLEMENT INSTITUTION IDENT CODE" */
	{ 11, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 100 "RECEIVING INSTITUTION IDENT CODE" */
	{ 17, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 101 "FILE NAME" */
	{ 28, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 102 "ACCOUNT IDENTIFICATION 1" */
	{ 28, LEN_LLVAR, ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 103 "ACCOUNT IDENTIFICATION 2" */
	{100, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 104 "TRANSACTION DESCRIPTION" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 105 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 106 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 107 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 108 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 109 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 110 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 111 "RESERVED ISO USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 112 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 113 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 114 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 115 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 116 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 117 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 118 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 119 "RESERVED NATIONAL USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 120 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 121 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 122 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 123 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 124 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 125 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 126 "RESERVED PRIVATE USE" */
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 127 "RESERVED PRIVATE USE" */
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 128 "MAC 2" */
	{  8, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 129 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 130 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 131 "RESERVED PRIVATE USE" */ 
	{ 19, LEN_LLVAR, ALN_LEFT,  '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 132 "RESERVED PRIVATE USE" */ 
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 133 "RESERVED PRIVATE USE" */ 
	{ 12, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 134 "RESERVED PRIVATE USE" */ 
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 135 "RESERVED PRIVATE USE" */ 
	{ 12, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 136 "RESERVED PRIVATE USE" */ 
	{ 10, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 137 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 138 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 139 "RESERVED PRIVATE USE" */ 
	{  2, LEN_FIX,   ALN_RIGHT, ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 140 "RESERVED PRIVATE USE" */ 
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 141 "RESERVED PRIVATE USE" */ 
	{  6, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 142 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 143 "RESERVED PRIVATE USE" */ 
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 144 "RESERVED PRIVATE USE" */ 
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 145 "RESERVED PRIVATE USE" */ 
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 146 "RESERVED PRIVATE USE" */ 
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 147 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 148 "RESERVED PRIVATE USE" */ 
	{  4, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 149 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 150 "RESERVED PRIVATE USE" */ 
	{  3, LEN_FIX,   ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 151 "RESERVED PRIVATE USE" */ 
	{  8, LEN_FIX,   ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 152 "RESERVED PRIVATE USE" */ 
	{ 16, LEN_FIX,   ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 153 "RESERVED PRIVATE USE" */ 
	{120, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 154 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 155 "RESERVED PRIVATE USE" */ 
	{ 99, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack,   PubFPBCharUnPack},   /* 156 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 157 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  ' ',  PubFPACharPack,   PubFPACharUnPack},   /* 158 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 159 "RESERVED PRIVATE USE" */ 
	{999, LEN_LLLVAR,ALN_LEFT,  '\0', PubFPBBinaryPack, PubFPBBinaryUnPack}, /* 160 "RESERVED PRIVATE USE" */ 
};
#endif
/* End of isodef.h */