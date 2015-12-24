/*
**  file: pkg_tblval.h
*/

#ifndef __PKG_TBLVAL__
#define __PKG_TBLVAL__

/* 有关库表值的宏定义 */

/* msgfldcfg中iFldFlag字段值宏定义 */
#define PKG_BITMAP_FLD 0
#define PKG_FIXLEN_FLD 1
#define PKG_WY_FLD 2
#define PKG_UNFIXLEN_FLD 3
#define PKG_SEPSYMBOL1_FLD 4
#define PKG_SEPSYMBOL2_FLD 5
#define PKG_MUL8583_FLD 6

/* msgfldcfg中LenFldAlign(FldAlign)字段值宏定义 */
#define PKG_LEFT_ALIGN "L"
#define PKG_LEFT1_ALIGN "l"

/* nodetrancfg中ToMidTranCodeId(ToMsgTranCodeId)字段值意义 */
#define PKG_NO_FUN_ID -1

/* nodetrancfg中CrtTranCodeType(WrtTranCodeType)字段值宏定义 */
#define PKG_NO_TRAN_CODE -1
#define PKG_TRAN_CODE_POS_FIX 0
#define PKG_8583_TRAN_CODE 1
#define PKG_SEP_SYMBOL_TRAN_CODE 2

/* tranfldcfg中TranFld字段中-1的意义 */
#define PKG_REP_SYMBOL -1

#endif
/*
** end of file: pkg_tblval.h
*/
