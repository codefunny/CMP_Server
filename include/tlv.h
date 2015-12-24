#ifndef _TLV_H_
#define _TLV_H_

#ifdef __cplusplus
extern "C" {
#endif

int TlvGetValue(int nTag, char *psValue, int *pnValueLen, char *pszSrc, int nSrcLen);
int TlvSetValue(int nTag, char *pszSrc, int *pnSrcLen, char *psValue, int nValueLen, int *pnTagNum);
int TlvGetCurrentTag(int *pnTag, char *psValue, int *pnValueLen, char *pszSrc, int *pnOffet);


#ifdef __cplusplus
}
#endif

#endif





