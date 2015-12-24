#ifndef _ASM_OPERCODE_H_
#define _ASM_OPERCODE_H_

#ifdef __cplusplus
extern "C"
{
#endif

int GetParamFromData( int nFlowNum, const char *epczFlowCode, int nDataLen, const char *epczData,
                      const char *epczCode, char *opczValue );
int PublicFun( char *opczBuf, int *opiBufLen );
int GetMenu( char *opczBuf, int *opiBufLen );
int SetMenu( const char *epczMenu , int enLen);
int GetHint( char *opczBuf, int *opiBufLen );
int SetHint(char chRefresh, char chTime, char *epczRetCode, char *epczRetMsg);
int GetParam( char *opczBuf, int *opiBufLen );
int SetParam( const char *epczBuf , int enLen);
int GetAllKey( char *opczBuf, int *opiBufLen );
int SetAllKey( const char *epczBuf , int enLen);
int GetKek( char *opczBuf, int *opiBufLen );
int SetKek( const char *epczBuf , int enLen);

int GetNormal( char *opczBuf, int *opiBufLen );
int SetNormal( const char *epczBuf , int enLen);

int GetLoginInfo( char *opczBuf, int *opiBufLen );
int SetLoginInfo( const char *epczBuf , int enLen);

int GetFileName( char *opczBuf, int *opiBufLen );
int SetFileName( const char *epczBuf , int enLen);

#ifdef __cplusplus
}
#endif

#endif
