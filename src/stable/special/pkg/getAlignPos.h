
#ifndef __GETAGLINPOS_H__
#define __GETAGLINPOS_H__

#define AG_CCHARTYPE       100
#define AG_CSHORTTYPE      101
#define AG_CINTTYPE        102
#define AG_CLONGTYPE       103
#define AG_CFLOATTYPE      104
#define AG_CDOUBLETYPE     105
#define AG_CPOINTERTYPE    106
#define AG_CSTRUCT	   107

#ifdef __cplusplus
extern "C" {
#endif

int getAlignNum (int dataType, int * align_struct_num);
int getAlignPos (int pos, int dataType, int align_struct_num);

#ifdef __cplusplus
}
#endif

#endif
