#include "shm_base.h"
#include "tempadd.h"

#ifndef __STR_AG_NUMS_H__
#define __STR_AG_NUMS_H__

#ifdef __cplusplus
extern "C" {
#endif

int getTranFlds (int tranFldNums, char * tranFldStr, int * tranFlds);

int getStructsAlignNums (int tranFldNums, int * iTranFlds, 
                         int msgAppid,     MsgFldCfgAry * msgFldCfgAry,
                         int alignStrNums[MAX_NESTED][MAX_STRUCT_PER_NESTED]);

#ifdef __cplusplus
}
#endif

#endif
