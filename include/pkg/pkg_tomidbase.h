/*
**  file: pkg_tomidbase.h
*/

#ifndef __PKG_TOMIDBASE__
#define __PKG_TOMIDBASE__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

typedef union _PKG_RawData
{
   short  shVal;
   int    iVal;
   long   lVal;
   float  fVal;
   double dVal;
   char  *pcVal;
} PKG_RawData;

typedef struct _PKG_DataValue
{
   int iType;
   int iLen;
   PKG_RawData nData;
} PKG_DataValue;

#ifdef __cplusplus
extern "C" {
#endif

/* 函数功能: 读取BitMap的特定位,看其是否为1 */
int PKG_IsBitMapSet( unsigned char * epczBitMap, int eiNum );


/* 函数功能: 计算在结构体中一个域之前的偏移量 */
void PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                             int * opiPtrMoveLen ); 

/* 函数功能: 计算一个重复区域的重复次数 */
int PKG_GetRepTimesVal( UDP epfAddr, int eiRepFldNum,
                        char * epczRepTimesSeq, char * epczRepTimesNum, 
                        int * opiRepTimesVal );


/* 函数功能: 根据数据类型从源报文中取值 */
/**********
int PKG_GetValFromBuf( char * epcBuf, int eiDataType, int eiValLen,
                       char ** opczVal, short * opshVal, int * opiVal, 
                       long * oplVal, double * opdVal, float * opfVal, 
                       int * opiValContentLen );
**********/
int PKG_GetValFromBuf( char * epcBuf, int eiDataType, int eiValLen,
                       PKG_DataValue *opstDataVal );

/* 函数功能: 根据域属性标志,取得不同域的值 */
/* 该函数中的opiSepMoveLen只有当一个域有分域时才起作用,此次开发没有用 */
/***********
int PKG_GetSepFldContentFromMsg( char * epcMsgPos, int * eopiMoveLen, 
                                 int * eopiMsgLeftLen,
                                 FldCfgValNode estFldCfgValNode,
                                 char ** opczVal, int * opiValLen, 
                                 short * opshVal, int * opiVal, long * oplVal, 
                                 double * opdVal, float * opfVal, 
                                 int * opiSepMoveLen ); 
***********/
int PKG_GetSepFldContentFromMsg( char * epcMsgPos, int * eopiMoveLen, 
                                 int * eopiMsgLeftLen,
                                 MsgFldCfg *epstMsgFldCfg,
				 PKG_DataValue *opstDataVal,
				 int * opiSepMoveLen );

/* 函数功能: 将eiSeq号域的eiSepSeq号分域的值按域类型插入链表(目前不支持分域) */
/********
int PKG_AddSepFldContentToLST( int eiSeq, int eiSepSeq, int eiDataType, 
                               char * epczVal, int eiValLen, short eshVal, 
                               int eiVal, long elVal, double edVal, 
                               float efVal, UDP opfsAddr );
********/
int PKG_AddSepFldContentToLST( int eiSeq, int eiSepSeq, int eiDataType,
                               PKG_DataValue *pstDataValue,
                               UDP opfsAddr );
/*
**  函数功能: 将eiSeq号域信息插入链表
**
**  函数名:  PKG_AddFldContentToLST
**
**  输入参数: char * epcMsgPos            // 已经指针指向eiFldSeq号域首的源报文
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int  * eopiMsgLeftLen       // 报文指针离尾部的的长度
**            int    eiSeq                // 域号
**            MsgFldCfgAry *epstMsgFldCfgAry  // 域空间起始地址
**                                          及域记录数
**
**  输出参数: UDP opfsAddr      // 中间信息链表
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int * eopiMsgLeftLen
**
**  返回值:   int 0成功, 其他失败 
*/

int  PKG_AddFldContentToLST( char * epcMsgPos, int * eopiMoveLen, 
                             int * eopiMsgLeftLen, int eiMsgAppId,
			     int eiSeq, MsgFldCfgAry *epstMsgFldCfgAry,
                             UDP opfsAddr );


#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomidbase.h
*/
