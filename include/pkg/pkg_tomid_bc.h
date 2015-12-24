/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMID_BC__
#define __PKG_TOMID_BC__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"
#include "pkg_base_cup20.h"

#ifdef __cplusplus
extern "C" {
#endif

int  PKG_ToMid_8583_2 (char * epcOrgMsg, int eiOrgMsgLen, 
                    int eiNodeId, char *epczTranCode, int eiMsgDscrb, 
   		    MsgFldCfgAry *epstMsgFldCfgAry ,
                    UDP opfsAddr);

#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
