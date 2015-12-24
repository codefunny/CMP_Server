/*
**  file: pkg_base.h
*/

#ifndef __PKG_BASE_CUP20__
#define __PKG_BASE_CUP20__

#include "global.h"
#include "pub_base.h"
#include "tool_base.h"
#include "udo.h"
#include "shm_base.h"

#include "pkg_err.h"
#include "pkg_tblval.h"

#ifdef __cplusplus
extern "C" {
#endif

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

int PKG_ChangeCharToInt( char cChar );

/**
  * change bitmap to flag
  * para:
  * 1 bitmap
  * 2 compress or uncompress flag
  * 3 bitflag
  */
int PKG_ChangeBitmapToFlag( char *pcBitmap, int iCompressFlag, char *pczFlag );

MsgFldCfg * PKG_GetTheFieldConfig( MsgFldCfgAry *epstMsgFldCfgAry, int iFldId );

int PKG_CreateBitmap( MsgFldCfgAry *pstMsgFldCfgAry, char *pczBitFlags,
                      char *pczBitmap );

/*根据8583报文和域号取域的值*/
int PKG_8583GetField( int eiMsgAppId, char *epczMsgBuf, int eiMsgLen, 
                      int eiFldId, char *opczValue, int *oiFldLen );

int PKG_Get8583FieldContent( char *pczOrgMsg, int eiMsgLen, int *piCurrPos,
                             MsgFldCfg *pstMsgFldCfg, char *pczValue,
                             int *piValLen );

int PKG_CreateFieldContent( MsgFldCfgAry *pstMsgFldCfgAry, int iFldId,
                            UDP epfAddr, char *pczFldContent );
#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_base_cup20.h
*/

