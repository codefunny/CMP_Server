/*
**file:asm_fundef.h
*/

#include "asm_api.h"
#include "asm_fun.h"

#ifndef __ASM_FUN_DEF_H__
#define __ASM_FUN_DEF_H__
OperaFunEntity astAsmEntity[] =
{
    ADDASM( FldCopy ),               /* 0�򿽱�  */
    ADDASM( AddStr ),                /* 1�ַ����ۼ� */
    ADDASM( AddShort ),              /* 2�������ۼ�  */
    ADDASM( AddInt ),                /* 3�����ۼ�  */
    ADDASM( AddLong ),               /* 4�������ۼ� */
    ADDASM( AddFloat ),              /* 5�������ۼ�  */
    ADDASM( AddDouble ),             /* 6˫�������ۼ�  */
    ADDASM( SysTime ),               /* 7ȡϵͳtime HHMMSS*/
    ADDASM( SysDate ),               /* 8ȡϵͳ���ڣ�YYYYMMDD  */
    ADDASM( ChangeDate ),            /* 9����ת��YYYY-MM-DD��YYYYMMDD*/
    ADDASM( ChangeTime ),            /* 10ʱ��ת��HH.MM.SS��HHMMSS*/
    ADDASM( ChangeDate2 ),           /* 11����ת��YYYYMMDD��YYYY-MM-DD*/
    ADDASM( GetSerial ),             /* 12��ȡ��ǰ�м�ҵ��ƽ̨��ˮ��(��0)*/
    ADDASM( GetAgTranDate ),         /* 13��ȡ��ǰ�м�ҵ��ƽ̨�������� */
    ADDASM( ChangeTime2 ),           /* 14ʱ��ת��HHMMSS��HH:MM:SS*/
    ADDASM( AscToBcd ),              /* 15ASCIIת��ΪBCD*/
    ADDASM( BcdToAsc ),              /* 16BCDת��ΪASCII*/
    ADDASM( AscToHex ),              /* 17ASCIIת��ΪHEX*/
    ADDASM( HexToAsc ),              /* 18HEXת��ΪASCII*/
    ADDASM( GetTime2BCD ),           /* 19ȡϵͳʱ��ת��ΪBCD*/
    ADDASM( GetPtDateTime ),         /* 20ȡƽ̨DATE+TIME yyyymmddhhmmss */
    ADDASM( RetMsgid ),              /* 21Ӧ�����Ϣ����ת�� */
    ADDASM( TrimBlank ),             /* 22ȥ���ҿո� */
    ADDASM( GetChRetCode ),          /* 23��ȡ����Ӧ���� */
    ADDASM( GetChRetMsg  ),          /* 24��ȡ����Ӧ����Ϣ */
    ADDASM( ChgTpdu  ),              /* 25ת��TPDU */
    ADDASM( GetBatch  ),             /* 26��ȡ�ն����κ� */
    ADDASM( GetOldSerial  ),         /* 27��ȡԭ������ˮ�� */
    ADDASM( GetOldDate  ),           /* 28��ȡԭ�������� */
    ADDASM( GetOldBatch  ),          /* 29��ȡԭ�������� */
    ADDASM( LEN_BIN2ASCII  ),        /* 30���ݳ��Ƚ�BINת��ΪASCII */
    ADDASM( AddStr_FORMAT  ),        /* 31�ַ�����ʽ���ۻ� */
    ADDASM( GetSysDate2BCD  ),       /* 32��ȡϵͳ����BCD */
    ADDASM( PackField60Ums  ),       /* 33���60�� */
    ADDASM( PackBalanceUms  ),       /* 34�������� */
    ADDASM( PackSettleData  ),       /* 35������������� */
    ADDASM( PackSettleResult  ),     /* 36����������� */
    ADDASM( SetAgenId  ),            /* 37���ô��������ʶ */
    ADDASM( LEN_ASCII2ASCII  ) ,     /* 38���ݳ��Ƚ�ASCIIת��ΪASCII */
    ADDASM( AlignLeft),              /*39 BCD��ʽ�ַ�������4��BIT*/
    ADDASM( PackBalanceBankComm),    /*40������������*/
    ADDASM( GetNormalString),        /*41ͨ�ò������*/
    ADDASM( PackBalanceYacol),       /*42�ſῨ���������*/
    ADDASM( AddStr_FORMAT_Ext),      /*43�ַ�����ʽ���ۻ� ��չ*/
    ADDASM( PackString),             /*44ͨ���ش������*/
    ADDASM(PackMacBuffer),           /*45���MAC ����*/
    ADDASM( GetEvn),                 /*46��ȡϵͳ����*/
    ADDASM( RetAscMsgid),            /*47ASCӦ�����Ϣ����ת�� */
    ADDASM( PackBalanceAllinpay),    /*48���ͨ�������*/
    ADDASM( PackToAsc),			     /*49���ݳ��Ƚ�ascת��ΪASCII */
    ADDASM( UncharFldCopy),			 /* 50BIN2CHAR�򿽱�  */
	ADDASM( RetTranstype), 		     /* 51ASCӦ��������ת��*/
	ADDASM( AddStr_Amount),			 /* 52�ַ�����ʽ���ۻ� ǰ��'0'*/
};	
#endif



/*
**end of file
*/
