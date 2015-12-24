/*
**  file: pkg_tomsgbase.h
*/

#ifndef __PKG_TOMSGBASE__
#define __PKG_TOMSGBASE__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 函数功能: 设置BitMap的特定位,将其设为1 */
int PKG_SetBitMap( unsigned char * eupczBitMap, int eiNum );

/* 函数功能: 按数据类型将值写入Buf */
int PKG_WriteValToBuf( char * opcBuf, int eiDataType, 
                       char * epczVal, int eiValLen, short eshVal, int eiVal, 
                       long elVal, double edVal, float efVal, 
                       int * opiValContentLen );


/* 函数功能:将第eiSepFldSeq个分域的信息写入输出报文 */
/* 该函数中的opiSepMoveLen只有当一个域有分域时才起作用,此次开发没有用 */
int PKG_WriteSepFldContentToMsg( char * opcMsgPos, int * eopiMoveLen,
                                 FldCfgValNode estFldCfgValNode,
                                 char * epczVal, int eiValLen, short eshVal, 
                                 int eiVal, long elVal, double edVal, 
                                 float efVal, int * opiSepMoveLen );

/* 函数功能: 将域信息写入输出报文 */
int PKG_WriteFldContentToMsg( char * opcMsgPos, int * eopiMoveLen, 
                              int eiMsgAppId, int eiFldSeq, int eiRepNum,
                              MsgFldCfgAry *epstMsgFldCfgAry,
                              UDP epfsAddr ); 

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsgbase.h
*/
