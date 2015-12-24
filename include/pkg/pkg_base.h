/*
**  file: pkg_base.h
*/

#ifndef __PKG_BASE__
#define __PKG_BASE__

/*
**  ���ļ�������ͷ�ļ�
*/
#include "global.h"
#include "pub_base.h"
#include "tool_base.h"
#include "udo.h"
#include "shm_base.h"

#include "pkg_err.h"
#include "pkg_tblval.h"

#define PKG_HEAD_NUM       -100
#define PKG_SUCC           0
#define PKG_FAIL           -1
#define PKG_LEVEL          2
#define PKG_ERR_LOG        "pkg_err.log"
#define HP_MAX_FIELD_LEN      1024

#define PKG_CYC_REP        1
#define PKG_CYC_SINGLE     2
#define PKG_TRAN_CODE_LEN  20
#define PKG_SELECT_LEN     500

/*  for test
#define PKG_MOD            4   
*/
#define PKG_MOD            8   
#define PKG_SHORT_MOD      ( (sizeof(short)>=PKG_MOD)?PKG_MOD:sizeof(short) )  
#define PKG_INT_MOD        ( (sizeof(int)>= PKG_MOD)?PKG_MOD:sizeof(int) )
#define PKG_LONG_MOD       ( (sizeof(long)>=PKG_MOD)?PKG_MOD:sizeof(long) )
#define PKG_FLOAT_MOD      ( (sizeof(float)>=PKG_MOD)?PKG_MOD:sizeof(float) )
#define PKG_DOUBLE_MOD     ( (sizeof(double)>=PKG_MOD)?PKG_MOD:sizeof(double) )

#define PKG_IS_STRUCT      1 

#define PKG_BITMAP_MAX     128  
#define PKG_MSGTYPE_LEN    4

#define PKG_NAME_LEN 30
#define PKG_DEF_LEN 2
#define PKG_ALIGN_LEN 1
#define PKG_SEP_SYMBOL_LEN 200

#define PKG_TRAN_CODE_PARA_LEN 50
#define PKG_REP_PARA_LEN 50
#define PKG_TRAN_FLD_LEN 600

/* ����ȫ�ֱ���iToMidIsSt, iToMsgIsSt */
extern int iToMidIsSt;         /* ���뱨���Ƿ�Ϊ�ṹ�� */
extern int iToMsgIsSt;         /* ��������Ƿ�Ϊ�ṹ�� */

typedef MsgFldCfg FldCfgValNode; /* Added By Shixiaoli on 2001/05/10 */

#ifdef __cplusplus
extern "C" {
#endif

/* ��������: ���ݽڵ��,����nodetrancfg, ȡһ����¼ */
int PKG_GetInfoFromNodeTranCfg( int eiNodeId, NodeTranCfg * opstNodeTranCfg );

/* ��������: ���ݽڵ��, ���״���, ����������, ����tranfldcfg, ȡһ����¼ */
int PKG_GetInfoFromTranFldCfg( int eiNodeId, 
                               char * epczTranCode, int eiMsgDscrb, 
                               TranFldCfg * opstTranFldCfg );



/* ��������: ���㽻�����������Ϣ�еĽ�����εĸ��� */
int PKG_GetNumsOfFld( char * epczTranFld );

/* ��������: ���㽻�����������Ϣ�еĽ�����εı�� */
int PKG_GetValFromFld( char * epczTranFld, int eiNum );

/* ��������: ȡ�ظ�������ֶ� */
int PKG_GetRepFld( char * epczTranFld, int eiBeginNum, char * opczRepFld );
int PKG_GetRepFld_Nested( char * epczTranFld, int eiBeginNum, char * opczRepFld );

/* ��������: ��һ��16���Ƶ��ַ���ת��Ϊ�ַ��� */
int PKG_XToA( char * epczX, char * opczA );



/* ��������: ������֪������eiDataType,ȡ��������eiSeq�����eiSepSeq����ĵ�
             eiRepNum�ε��ظ�ֵ, �����ֱ���ڲ�ͬ������������ */ 
int PKG_GetFldFromLST( UDP epfAddr, int eiSeq, int eiSepSeq,
                       int eiRepNum, int eiDataType, 
                       char ** opczVal, int *opiValLen, short * opshVal, 
                       int *opiVal, long * oplVal, double * opdVal, 
                       float * opfVal );


/* ��������: ȡ�ñ�ʶһ�����Ľṹ�Ƿ�Ϊ�ṹ��ı���ֵ */
int PKG_MsgIsStruct( int eiNodeId, int * opiToMidIsSt, int * opiToMsgIsSt );

/* ��������: �����ڽṹ����һ����֮ǰ��ƫ���� */
void PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                             int * opiPtrMoveLen ); 


/* ��������: ��ʾ�ṹNodeTranCfg */
void PKG_DisplayNodeTranCfg( NodeTranCfg estNodeTranCfg );

/* ��������: ��ʾ�ṹTranFldCfg */
void PKG_DisplayTranFldCfg( TranFldCfg estTranFldCfg );
                           
/* ��������: ��ʾ�ṹFldCfgValNode */
void PKG_DisplayFldCfgValNode( FldCfgValNode estFldCfgValNode );

/*
**  ��������: ���ݱ���Ӧ�ú�,���״����������Ӧ��־������msgfldcfg,
**            �õ��������������Ϣ������pstFldCfgHead��ʼ����ռ���
**  �������: 1 �ڵ��
**            2 ���״���
**            3 ������Ӧ��־��1-����2-��Ӧ
**  �������: MsgFldCfgAry * opstmsgFldCfgAry
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
**
** ����˵��: �����ڲ������˿ռ�, �������ע���ͷ� 
*/
int  PKG_GetInfoByTranCode( int eiNodeId, char *epczTranCode,
                            int eiFlag,
                            MsgFldCfgAry *opstMsgFldCfgAry );


/*
**  ��������: ���ݱ���Ӧ�ú�,����msgfldcfg,�õ��������������Ϣ����
**
**  ������:   PKG_GetInfoFromMsgFldCfg
**
**  �������: int eiMsgAppId
**
**  �������: MsgFldcfgAry * opstMsgFldCfgAry // ��ռ�
**                                               ������
**
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
**
** ����˵��: �����ڲ������˿ռ�, �������ע���ͷ� 
*/
int  PKG_GetInfoFromMsgFldCfg( int eiMsgAppId, 
                               MsgFldCfgAry *epstMsgFldCfgAry );

/*
**  ��������: ����ĳһ�����ڿռ��ڵ���ʼ��ַ�ͷ�������
**
**  ������:   PKG_GetInfoByFldSeq
**
**  �������: MsgFldCfgAry * epstMsgFldCfgAry  ��ռ���ʼ��ַ 
**                                      ��ռ��ڼ�¼��
**            int iFldSeq   : ����
**
**  �������: MsgFldCfgAry * opstmsgFldCfgAry:����Ϊ 
**                         iFldSeq����ռ���ʼ��ַ����������
**
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  PKG_GetInfoByFldSeq( MsgFldCfgAry * epstMsgFldCfgAry, 
                          int eiMsgAppId, int iFldSeq,
			  MsgFldCfgAry * opstMsgFldCfgAry ); 

int  PKG_GetInfoByFldName( MsgFldCfgAry * epstMsgFldCfgAry,
                           char * epczFldName,
                           MsgFldCfgAry * opstMsgFldCfgAry );


/*
**  ��������: ����ռ��ж�ȡ��eiFldSeq����ĵ�eiSepFldSeq�����������Ϣ
**
**  ������: PKG_GetFldCfgValNode
**
**  �������: MsgFldCfgAry * opstMsgFldCfgAry  // ����eiFldSeq����
**                                      �ռ��ڵ���ʼ��ַ����������
**            int eiMsgId   // ����Ӧ�ú�
**            int eiFldSeq  // ����
**            int eiSepFldSeq // ������
**  �������: FldCfgValNode * opstFldCfgValNode 
**
**  ����ֵ: 0��ȷ ��������
*/
int  PKG_GetFldCfgValNode( MsgFldCfgAry * opstMsgFldCfgAry  ,
                           int eiMsgId,
                           int eiFldSeq,
                           int eiSepFldSeq, MsgFldCfg * opstMsgFldCfg );

void PKG_FreeMsgFldCfgAry( MsgFldCfgAry *epstMsgFldCfgAry );


/*
** ������ת��, ����aix��sco֮���ת��
*/
short chg_short( short s );

/*
** ����ת��, ����aix��sco֮���ת������������֮���Լ�32λ��������֮�䣩
*/
int chg_int( int t );

/*
** ������ת��, ����aix��sco֮���ת��
*/
float chg_float( float f );

/*
** ˫������ת��, ����aix��sco֮���ת��
*/
double chg_double( double d );





#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_base.h
*/

