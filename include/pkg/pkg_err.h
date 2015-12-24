/*
**  file: pkg_err.h
*/

#ifndef __PKG_ERR__
#define __PKG_ERR__

/* 有关错误代码的宏定义 */
#define PKG_BASE                   200

#define PKG_NO_TOMID_TRANCODE_ID   PKG_BASE + 1
#define PKG_NO_TOMSG_TRANCODE_ID   PKG_BASE + 2
#define PKG_TOMID_TRANCODE_ID_ERR  PKG_BASE + 3
#define PKG_TOMSG_TRANCODE_ID_ERR  PKG_BASE + 4
#define PKG_TOMID_ID_ERR           PKG_BASE + 5
#define PKG_TOMSG_ID_ERR           PKG_BASE + 6
#define PKG_LENGTH_SHORT           PKG_BASE + 7
#define PKG_LENGTH_OVER            PKG_BASE + 8

#define PKG_NUM_OVERFLOW           PKG_BASE + 11

#define PKG_SEPFLDSEQ_OVERFLOW     PKG_BASE + 21
#define PKG_NO_FLDCFGNODE          PKG_BASE + 22 

#define PKG_16_LENGTH_ERR          PKG_BASE + 31
#define PKG_16_INPUT_ERR           PKG_BASE + 32

#define PKG_SEPFLDFLAG_ERR         PKG_BASE + 41
#define PKG_LENFLDTYPE_ERR         PKG_BASE + 42
#define PKG_FLDTYPE_ERR            PKG_BASE + 43

#define PKG_MSGDSCRB_ERR           PKG_BASE + 52
#define PKG_POS_OVERFLOW           PKG_BASE + 53

#define PKG_IS_BITMAP              PKG_BASE + 61
#define PKG_BITMAP_OVERFLOW        PKG_BASE + 62

#define PKG_MSGFLDCFG_NO_DATA      PKG_BASE + 91
#define PKG_NODETRANCFG_NO_DATA    PKG_BASE + 92
#define PKG_TRANFLDCFG_NO_DATA     PKG_BASE + 93

#define PKG_DATATYPE_INVALID       UDO_MATCH_ERROR
#define PKG_NO_FIELD               UDO_NO_FIELD 

#endif

/*
** end of file: pkg_err.h
*/
