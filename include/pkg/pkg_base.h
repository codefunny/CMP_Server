/*
**  file: pkg_base.h
*/

#ifndef __PKG_BASE__
#define __PKG_BASE__

/*
**  该文件包含的头文件
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

/* 定义全局变量iToMidIsSt, iToMsgIsSt */
extern int iToMidIsSt;         /* 输入报文是否为结构体 */
extern int iToMsgIsSt;         /* 输出报文是否为结构体 */

typedef MsgFldCfg FldCfgValNode; /* Added By Shixiaoli on 2001/05/10 */

#ifdef __cplusplus
extern "C" {
#endif

/* 函数功能: 根据节点号,读表nodetrancfg, 取一条记录 */
int PKG_GetInfoFromNodeTranCfg( int eiNodeId, NodeTranCfg * opstNodeTranCfg );

/* 函数功能: 根据节点号, 交易代码, 报文描述符, 读表tranfldcfg, 取一条记录 */
int PKG_GetInfoFromTranFldCfg( int eiNodeId, 
                               char * epczTranCode, int eiMsgDscrb, 
                               TranFldCfg * opstTranFldCfg );



/* 函数功能: 计算交易域段配置信息中的交易域段的个数 */
int PKG_GetNumsOfFld( char * epczTranFld );

/* 函数功能: 计算交易域段配置信息中的交易域段的编号 */
int PKG_GetValFromFld( char * epczTranFld, int eiNum );

/* 函数功能: 取重复区域的字段 */
int PKG_GetRepFld( char * epczTranFld, int eiBeginNum, char * opczRepFld );
int PKG_GetRepFld_Nested( char * epczTranFld, int eiBeginNum, char * opczRepFld );

/* 函数功能: 将一个16进制的字符串转化为字符串 */
int PKG_XToA( char * epczX, char * opczA );



/* 函数功能: 根据已知域类型eiDataType,取得链表中eiSeq号域的eiSepSeq分域的第
             eiRepNum次的重复值, 其结果分别放在不同的数据类型中 */ 
int PKG_GetFldFromLST( UDP epfAddr, int eiSeq, int eiSepSeq,
                       int eiRepNum, int eiDataType, 
                       char ** opczVal, int *opiValLen, short * opshVal, 
                       int *opiVal, long * oplVal, double * opdVal, 
                       float * opfVal );


/* 函数功能: 取得标识一个报文结构是否为结构体的变量值 */
int PKG_MsgIsStruct( int eiNodeId, int * opiToMidIsSt, int * opiToMsgIsSt );

/* 函数功能: 计算在结构体中一个域之前的偏移量 */
void PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                             int * opiPtrMoveLen ); 


/* 函数功能: 显示结构NodeTranCfg */
void PKG_DisplayNodeTranCfg( NodeTranCfg estNodeTranCfg );

/* 函数功能: 显示结构TranFldCfg */
void PKG_DisplayTranFldCfg( TranFldCfg estTranFldCfg );
                           
/* 函数功能: 显示结构FldCfgValNode */
void PKG_DisplayFldCfgValNode( FldCfgValNode estFldCfgValNode );

/*
**  函数功能: 根据报文应用号,交易代码和请求响应标志，读表msgfldcfg,
**            得到所有域段配置信息放入以pstFldCfgHead开始的域空间内
**  输入参数: 1 节点号
**            2 交易代码
**            3 请求响应标志，1-请求，2-响应
**  输出参数: MsgFldCfgAry * opstmsgFldCfgAry
**  返回值:   int  0 成功, 其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
*/
int  PKG_GetInfoByTranCode( int eiNodeId, char *epczTranCode,
                            int eiFlag,
                            MsgFldCfgAry *opstMsgFldCfgAry );


/*
**  函数功能: 根据报文应用号,读表msgfldcfg,得到所有域段配置信息链表
**
**  函数名:   PKG_GetInfoFromMsgFldCfg
**
**  输入参数: int eiMsgAppId
**
**  输出参数: MsgFldcfgAry * opstMsgFldCfgAry // 域空间
**                                               域数量
**
**  返回值:   int  0 成功, 其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
*/
int  PKG_GetInfoFromMsgFldCfg( int eiMsgAppId, 
                               MsgFldCfgAry *epstMsgFldCfgAry );

/*
**  函数功能: 查找某一域在于空间内的起始地址和分域数量
**
**  函数名:   PKG_GetInfoByFldSeq
**
**  输入参数: MsgFldCfgAry * epstMsgFldCfgAry  域空间起始地址 
**                                      域空间内记录数
**            int iFldSeq   : 域编号
**
**  输出参数: MsgFldCfgAry * opstmsgFldCfgAry:域编号为 
**                         iFldSeq的域空间起始地址及分域数量
**
**  返回值:   int  0 成功, 其他失败
*/
int  PKG_GetInfoByFldSeq( MsgFldCfgAry * epstMsgFldCfgAry, 
                          int eiMsgAppId, int iFldSeq,
			  MsgFldCfgAry * opstMsgFldCfgAry ); 

int  PKG_GetInfoByFldName( MsgFldCfgAry * epstMsgFldCfgAry,
                           char * epczFldName,
                           MsgFldCfgAry * opstMsgFldCfgAry );


/*
**  函数功能: 从域空间中读取第eiFldSeq号域的第eiSepFldSeq分域的配置信息
**
**  函数名: PKG_GetFldCfgValNode
**
**  输入参数: MsgFldCfgAry * opstMsgFldCfgAry  // 分域eiFldSeq在于
**                                      空间内的起始地址及分域数量
**            int eiMsgId   // 报文应用号
**            int eiFldSeq  // 域编号
**            int eiSepFldSeq // 分域编号
**  输出参数: FldCfgValNode * opstFldCfgValNode 
**
**  返回值: 0正确 其他错误
*/
int  PKG_GetFldCfgValNode( MsgFldCfgAry * opstMsgFldCfgAry  ,
                           int eiMsgId,
                           int eiFldSeq,
                           int eiSepFldSeq, MsgFldCfg * opstMsgFldCfg );

void PKG_FreeMsgFldCfgAry( MsgFldCfgAry *epstMsgFldCfgAry );


/*
** 短整型转换, 处理aix与sco之间的转换
*/
short chg_short( short s );

/*
** 整型转换, 处理aix与sco之间的转换（包括整型之间以及32位机长整型之间）
*/
int chg_int( int t );

/*
** 浮点型转换, 处理aix与sco之间的转换
*/
float chg_float( float f );

/*
** 双精度型转换, 处理aix与sco之间的转换
*/
double chg_double( double d );





#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_base.h
*/

