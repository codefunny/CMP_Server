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
**  ��������: ����Դ��ֵ������Ŀ����
**  ������:   ASM_FldCopy
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_FldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ�����ַ����ۼӵķ�ʽ����Ŀ����
**  ������:   ASM_AddStr
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddStr( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ���ն�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddShort
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddShort( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ����������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddInt
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddInt( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ���ճ�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddLong
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddLong( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ���ո�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddFloat
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddFloat( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ����˫��������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddDouble
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddDouble( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                    int eiFldSeq, UDP opcResultList );

/*
**����: ȡϵͳʱ��(HHMMSS)
**����: 1 ������Ϣ
**      2 ��Դ��ֵ��Ϣ����
**      3 ����Ӧ�ú�
**      4 ���������
**���:1 ת�����
**���� 0-�ɹ�,����-ʧ��
*/
int  ASM_SysTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );


/*
**����: ȡϵͳ����(YYYYMMDD)
**����: 1 ������Ϣ
**      2 ��Դ��ֵ��Ϣ����
**      3 ����Ӧ�ú�
**      4 ���������
**���:1 ת�����
**���� 0-�ɹ�,����-ʧ��
*/
int  ASM_SysDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
		  int eiFldSeq, UDP opcResultList );

/**
 * ��������ת�� YYYY-MM-DD To YYYYMMDD
 * ����˵����
 * epsSrcList    �������� YYYY-MM-DD
 * opcResultList ������� YYYYMMDD
 */
int  ASM_ChangeDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
 * ����ʱ��ת�� HH.MM.SS To HHMMSS
 * ����˵����
 * epsSrcList    ����ʱ�� HH.MM.SS
 * opcResultList ���ʱ�� HHMMSS
 */
int  ASM_ChangeTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
 * ��������ת�� YYYYMMDD To YYYY-MM-DD
 * ����˵����
 * epsSrcList    �������� YYYYMMDD
 * opcResultList ������� YYYY-MM-DD
 */
int  ASM_ChangeDate2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList );

/**
 * ��ȡ�м�ҵ��ƽ̨��ˮ��(�󲹿ո�)
 * ����˵����
 * epsSrcList    ��
 * opcResultList ƽ̨��ˮ��
 */
int ASM_GetSerial( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList );

/**
 * ��ȡ�м�ҵ��ƽ̨��������
 * ����˵����
 * epsSrcList    ��
 * opcResultList ƽ̨����
 */
int ASM_GetAgTranDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList );


int  ASM_ChangeTime2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList );

/**
  * ���ַ�ASCת��ΪBCD ֧��0-F��ת��
  */
int ASM_AscToBcd( SVC_Content *epstService, UDP epcSrcList,
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���ַ�BCDת��ΪASC
  */
int ASM_BcdToAsc( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���ַ�ת��ΪHEX��ʽ
  */
int ASM_AscToHex( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��HEX��ʽת��ΪASC��
  */
int ASM_HexToAsc( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ȡϵͳ���ڲ�ת��ΪBCD��
  */
int ASM_GetTime2BCD( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ȡƽ̨date+time
  */
int ASM_GetPtDateTime( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * Ӧ����Ϣ����ת��
  */
int ASM_RetMsgid( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ȥ�����ҿո�
  */
int ASM_TrimBlank( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��ȡ����Ӧ���� 
  */
int ASM_GetChRetCode( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��ȡ����Ӧ����Ϣ 
  */
int ASM_GetChRetMsg( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );


/**
  * ת��TPDU 
  */
int ASM_ChgTpdu( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
                    
/**
  * ��ȡ�ն����κ�
  */
int ASM_GetBatch( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��ȡԭ������ˮ��
  */
int ASM_GetOldSerial( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
                 
/**
  * ��ȡ�ն����κ�
  */
int ASM_GetOldDate( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��ȡ�ն����κ�
  */
int ASM_GetOldBatch( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );
/**
  * ���ݳ��Ƚ�BINת��ΪASCII
  */
int ASM_LEN_BIN2ASCII( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * �ַ�����ʽ���ۻ�
  */
int ASM_AddStr_FORMAT( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��ȡϵͳ���ڵ�BCD
  */
int ASM_GetSysDate2BCD( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���60��
  */
int ASM_PackField60Ums( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ��������
  */
int ASM_PackBalanceUms( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���������Ϣ��
  */
int ASM_PackSettleData( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * �����������Ϣ��
  */
int ASM_PackSettleResult( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���ô��������ʶ
  */
int ASM_SetAgenId( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
  * ���ݳ��Ƚ�ASCIIת��ΪASCII
  */
int ASM_LEN_ASCII2ASCII( SVC_Content *epstService, UDP epcSrcList,  
                    int eiMsgAppId, int eiFldSeq, UDP opcResultList );

/**
* BCD��ʽ��������4��BIT
**/
int ASM_AlignLeft( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
* �����������
**/
int ASM_PackBalanceBankComm( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
*ͨ�ò������
**/
int ASM_GetNormalString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/**
*�ſ����������
**/
int ASM_PackBalanceYacol( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );


/**
  * �ַ�����ʽ���ۻ���չ
  */

int ASM_AddStr_FORMAT_Ext( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/*ͨ�ô���������*/
int ASM_PackString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList );

/*���MAC ����*/
int ASM_PackMacBuffer( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList );


/*��ȡϵͳ����*/
int ASM_GetEvn( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList );

/**
  * ASC Ӧ����Ϣ����ת��
  */

int ASM_RetAscMsgid( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList );


/*ͨ�������*/
int ASM_PackBalanceAllinpay( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList );



/*�������*/
int ASM_PackToAsc( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                         int eiFldSeq, UDP opcResultList );

/*
**  ��������: ����Դ��ֵ������Ŀ����
**  ������:   ASM_FldCopy
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_UncharFldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList );

/**
  * ASC Ӧ��������ת��
  */

int ASM_RetTranstype( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList );


/**
  * �ַ�����ʽ���ۻ�ǰ��'0'
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
