/*
**  file: pkg_tomsg_tms.h
*/

#ifndef __PKG_TOMSG_TMS_H__
#define __PKG_TOMSG_TMS_H__

/*
**  ���ļ�������ͷ�ļ�
*/

#include "pkg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_Tms( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg_tms.h
*/
