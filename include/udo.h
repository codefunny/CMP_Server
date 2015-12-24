#include <stdio.h>
#include <stdarg.h>

#ifndef __CMP_PAGE_BLOCK_FIELD_STORAGE_STRUCT_H__
#define __CMP_PAGE_BLOCK_FIELD_STORAGE_STRUCT_H__

/*
 * �����ϰ汾UDO����ǰ�汾
 * �°汾������
 * 1 ֧��UDO_AddField�Զ������ڴ�
 * 2 UDO_Startup�����ǵ�һ�������õĺ���
 * 3 UDO_End֧����������δ���ͷŵ��ڴ� 
 */

typedef struct _UDOPage UDOPage;

/* ��һҳ�е�iFieldNum��ʾ����UDOҳ���е������, �Ժ��ҳ��ʾ��ҳ�Ƿ�ʹ�� */

typedef union _Var
{
   char   cVal;
   short  shVal;
   int    iVal;
   long   lVal;
   float  fVal;
   double dVal;
   char   *pcVal;
} Var;

typedef struct _UDOValDesc UDOValDesc;

typedef struct _UDOFieldDesc UDOFieldDesc;

#define UDO_TOO_SMALL	           -1
#define UDO_NO_FIELD	               -2
#define UDO_INVALID_OCCUR           -3
#define UDO_OVER_RANGE              -4
#define UDO_MATCH_ERROR	           -5
#define UDO_TYPE_ERR                -6
#define UDO_FIELD_LEN_TOO_LONG      -7

#define STRTYPE		0
#define SHORTTYPE	1
#define INTTYPE		2
#define LONGTYPE        3
#define FLOATTYPE	4
#define DOUBLETYPE	5
#define BINTYPE         6
#define CHARTYPE	7
#define OTHERTYPE       100

#define UDT_STRING	STRTYPE
#define UDT_SHORT	SHORTTYPE
#define UDT_INT		INTTYPE
#define UDT_LONG 	LONGTYPE
#define UDT_DOUBLE	DOUBLETYPE
#define UDT_FLOAT	FLOATTYPE
#define UDT_CHAR		CHARTYPE
#define UDT_BIN	    BINTYPE
#define UDT_OTHER    OTHERTYPE

#define SIZE_CHAR	sizeof(char)
#define SIZE_SHORT	sizeof(short)
#define SIZE_INT	sizeof(int)
#define SIZE_LONG	sizeof(long)
#define SIZE_FLOAT	sizeof(float)
#define SIZE_DOUBLE	sizeof(double)
#ifdef __cplusplus
extern "C" {
#endif
typedef char * UDP;

/*
**����:
**����:1
**     2
**���:
**����:
*/
void  UDO_Init( UDP epfAddr, int eiSize );

/*
**����:����UDP
**����:1
**���:
**����:NULL-ʧ��,����-
*/
UDP UDO_Alloc( int eiSize );

/*
**����:��UDP���Ϳռ�����һ����ֵ,����򲻴���,���Ӹ��򲢱���ֵ,
**����:1 UDP
**     2
**     3
**     4
**     5
**���:
**����:0-�ɹ�,����-
*/
int UDO_AddField( UDP epfAddr, int eiFieldId, int eiFieldType,
                 char * epValue, int eiValLen );

/*
**����:
**����:1 UDP
**     2
**     3
**���:1
**     2
**����:0-�ɹ�,����-
*/
int UDO_GetField( UDP epfAddr, int eiFieldId, int eiFieldOccu,
                 char * opValue, int *opValLen );

/*
**����:��ӡUDP
**����:1 UDP
**���:
**����:
*/
void UDO_Print( UDP epfAddr );

/*
**����:
**����:1 UDP
**     2
**���:
**����: >0�ظ�����, <0
*/
int UDO_FieldOccur( UDP epfAddr, int eiFieldId );

/*
**����:
**����:1 UDP
**���:
**����: >=0
*/
int UDO_FieldCount( UDP epfAddr );

/*
**����:ȡUDO
**����:1 UDP
**���:
**����: >=0
*/
int UDO_PageSize( UDP epfAddr );

/*
**����:ȡUDO
**����:1 UDP
**���:
**����: >=0UDO
*/
int UDO_PageActualSize( UDP epfAddr );

/*
**����:ȡUDO���и�ҳʵ�ʴ�С֮�ͣ����ܻ���ΪUDO_ChgField�����ֵ��С֮�Ͳ��� 
**����:1 UDP
**���:
**����: >=0UDO
*/
int UDO_ValueSize( UDP epfAddr );

/*
**����:ȡUDO������ҳ��С֮�� 
**����:1 UDP
**���:
**����: >=0
*/
int UDO_Size( UDP epfAddr );

/*
**����:�ͷ�UDP
**����:1 UDP
**���:
**����:
*/
void UDO_Free( UDP epfAddr );

/*
 * ����: �ͷ�����UDOҳ��̬����Ŀռ� 
 */
void UDO_FreeAll();

/*
**����:
**����:1 FS
**     2
**���:

**����:0-������,1-
*/
int UDO_FieldExist( UDP epfAddr, int eiFieldId );

/*
**����:
**����:1 UDP
**     2
**���:
**����:>0,�����ͣ�<0
*/
int UDO_FieldType( UDP epfAddr, int iFieldId );

/*
**����:
**����:1 UDP
**     2
**���:1
**����:0-�ɹ�������ʧ�ܣ�����UDP������ţ���Ŵ�0
*/
int UDO_FieldId( UDP epfAddr, int eiFldNo, int *opiFldId );

/*
**����:��һ��UDP�ռ丽��������һ��UDP
**����:1 Ŀ��UDP
**     2 ԴUDP
**���:1 Ŀ��UDP
**����:0-�ɹ�������ʧ�ܣ�Ŀ��UDP
*/
int UDO_Append( UDP eopfTargetAddr, UDP epfSourceAddr );

/*
**����:UDP
**����:1 ԴUDP
**���:1 Ŀ��UDP
**����:0-�ɹ�,����ʧ��(Ŀ��ռ�̫С)
*/
int UDO_Copy( UDP opfTargetAddr, UDP epfSourceAddr );

/*
**����:
**����:1 UDP
**     2
**     3
**���:
**����:>=0��ȷ,��ֵ����,��������,
*/
int UDO_FieldLen( UDP epfAddr, int eiFieldId, int eiOccur );

/*
**����:
**����:1 UDP
**     2
**     3
**     4
**���:1
**     2
**����:0-�ɹ�,����-
*/
int UDO_SafeGetField( UDP epfAddr, int eiFieldId, int eiFieldOccu,
                     int eiType, char * opValue, int *opiValLen );


/*
**����:�ı�ָ����ָ���ظ�������ֵ,���û�и��򷵻ش�,
**
**����:1 UDP
**     2 ��ID
**     3
**     4
**     5
**���:
**����:  0-�ɹ�,����ʧ��(�ռ��С���������޴���)
*/
int UDO_ChgField( UDP epfAddr, int eiFieldId, int eiFieldOccu,
                 char *epcValue, int eiFieldLen );

/*
**funtion:get the first field by the fieldid and return the next field address
**input:1 UDP address
**      2 field id
**output:1 field value
**       2 field len
**       3 second field address offset
**return:0-success,other-error,see also UDO_GetField
*/
int UDO_GetFirstField( UDP epfAddr, int eiFieldId, char *opcValue,
                      int *opiLen, int *opiSeconddOffset );

void UDO_FilePrint( UDP epfAddr, FILE *fp );

/*
**function:get the current field value and next field address
**input: 1 pfs address
**       2 current field offset
**output:1 current field value
**       2 current field len
**       3 next field address
**return:0-correct,other-error,see also UDO_GetField
*/
int UDO_GetCurrField( UDP epfAddr, int eiCurrFielOffset, char *opcValue,
                     int *opiValLen, int *opiNextFieldOffset );

void UDO_FPrint( char * epczFileName, UDP epfAddr );

/*
 * ����: �ͷ�����UDOҳ��̬����Ŀռ� 
 */
void UDO_FreeAll();

/* @UDO_SeekFieldDesc
 * #����ָ����ŵ���������
 * %����1 ��������ҳ��ַָ��                ����������� 
 * %����2 ���                              ������� 
 * %����3 ���һ������������ַָ��          ������� NULL--��ʾUDOҳ��Ϊ�� 
 * #NULL--��ָ����ŵ��� ����--ָ����������ָ��
 * #ֻ�е�����ֵΪNULLʱ�������opstLastFldDesc�������� 
 */
UDOFieldDesc *UDO_SeekFieldDesc( UDOPage **eopstPage, int eiFieldId, UDOFieldDesc **opstLastFldDesc );

UDP UDO_Merge( UDP *opfDest, UDP epfSrc );

/*
 * ��UDOת��Ϊ�ַ�����ʽ
 */
int UDO_sprintf (UDP epfAddr, char * opcBuf, int *opiLen );

/* ���ַ����лָ�UDO,�벻Ҫʹ��������UDOת�����ַ��� */
int UDO_sscanf (char * epcBuf, int eiLen, UDP opfAddr);

int UDO_ACField( UDP epfSrc, int eiFldId, int eiType, char *epValue,
                 int eiLen );

/**
  * ע: ʹ��ǰ��Ҫ����LOG4C_INIT��ʼ��־����
  **/
int UDO_Prt2File( int eiLevel, char * epczFileName, UDP epfAddr, ... );

int UDO_GetTypePri (int eiType);

#define UDO_PAGE_SIZE 307200

#ifdef __cplusplus
};
#endif

#endif

