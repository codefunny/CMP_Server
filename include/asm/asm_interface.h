/*
**  file  : asm_fun.c
*/
#include "asm/asm_fun.h"
#include "asm/asm_fundef.h"

#ifndef __ASM_INTERFACE_H__
#define __ASM_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

int ASM_RunFunc( int iFldFragCalFlag, SVC_Content *epstService, UDP pcSrcList,
                 int iMsgAppId, int iFldSeq, UDP pcResult );


#ifdef __cplusplus
}
#endif

#endif
/*
** end of file
*/
