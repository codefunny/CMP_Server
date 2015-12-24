#ifndef __HSM_SEC_API_H__
#define __HSM_SEC_API_H__

#ifndef TRUE
#define  TRUE  0
#endif
#ifndef FALSE
#define  FALSE -1
#endif

#ifndef u8
typedef unsigned char   u8;
#endif

#ifndef u16
typedef unsigned short  u16;
#endif

#ifndef u32
typedef unsigned long   u32;
#endif

#define ESEC_CONNECT        0x91
#define ESEC_SEND           0x92
#define ESEC_RECV           0x93
#define ESEC_SEND_TIMEOUT   0x94
#define ESEC_RECV_TIMEOUT   0x95
#define ESEC_PARA				 0x96

#define SECBUF_MAX_SIZE 1500

/*#pragma pack(1)*/

typedef struct {
   unsigned char len[2];
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char PVK[32];
   unsigned char PIN[7];
   unsigned char Account[12];
   unsigned char PVKI;
} StruCmdDGIn;

typedef struct {
   unsigned char len[2];
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char PVV[4];
} StruCmdDGOut;     

typedef struct {
   unsigned char len[2];
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char TPK[16];
   unsigned char PVK[32];
   unsigned char PINBlock[16];
   unsigned char PINFormat[2];
   unsigned char Account[12];
   unsigned char PVKI;
   unsigned char PVV[4];
} StruCmdDCIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdDCOut;

typedef struct {
   unsigned char len[2];
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char ZPK[16];
   unsigned char PVK[32];
   unsigned char PINBlock[16];
   unsigned char PINFormat[2];
   unsigned char Account[12];
   unsigned char PVKI;
   unsigned char PVV[4];
} StruCmdECIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdECOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char tpk[16];
   unsigned char pvk[16];
   unsigned char maxpinlen[2];
   unsigned char pinblock[16];
   unsigned char pinformat[2];
   unsigned char checklen[2];
   unsigned char account[12];
   unsigned char dectable[16];
   unsigned char validata[12];
   unsigned char offset[12];
} StruCmdDAIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdDAOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char pvk[16];
   unsigned char offset[12];
   unsigned char pinlen[2];
   unsigned char account[12];
   unsigned char dectable[16];
   unsigned char validata[12];
} StruCmdEEIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char pin[7];
} StruCmdEEOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char pvk[16];
   unsigned char pin[7];
   unsigned char pinlen[2];
   unsigned char account[12];
   unsigned char dectable[16];
   unsigned char validata[12];
} StruCmdDEIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char offset[12];
} StruCmdDEOut;

typedef struct {
   unsigned short len;
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char zpk[16];
   unsigned char pvk[16];
   unsigned char maxpinlen[2];
   unsigned char pinblock[16];
   unsigned char pinformat[2];
   unsigned char checklen[2];
   unsigned char account[12];
   unsigned char dectable[16];
   unsigned char validata[12];
   unsigned char offset[12];
} StruCmdEAIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdEAOut;

typedef struct {
   unsigned short len;
   unsigned char head[8];
   unsigned char cmd[2];
} StruCmdASIn;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char key[16];
   unsigned char keytype[2]; /* ¡°00¡±£ºZMK¡¡¡°01¡±£ºZPK¡°02¡±£ºTMK/TPK/PVK¡°03¡±£ºTAK*/
   
} StruCmdKAIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char cvka[16];
   unsigned char cvkb[16];
   unsigned char checka[6];
   unsigned char checkb[6];
} StruCmdASOut;

typedef struct {
   unsigned short len;
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char zmk[16];
   unsigned char cvka[16];
   unsigned char cvkb[16];
   unsigned char variant;
} StruCmdAUIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char cvka[16];
   unsigned char cvkb[16];
   unsigned char checka[6];
   unsigned char checkb[6];
} StruCmdAUOut;

typedef struct {
   unsigned short len;
   unsigned char head[8];
   unsigned char cmd[2];
   unsigned char PAN[12];
   unsigned char PVK[16];
} StruCmdOAIn;

typedef struct {
   unsigned short len;
   unsigned char usr[8];
   unsigned char code[2];
   unsigned char cmd[2];
   unsigned char NaturePIN[5];
} StruCmdOAOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char PIN[7];
   unsigned char PAN[12];
   unsigned char PVK[16];
} StruCmdTMIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char Offset[5];
   unsigned char CheckVal;
} StruCmdTMOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char ZPK[16];
   unsigned char PinBlock[16];
   unsigned char PAN[12];
   unsigned char PVK[16];
} StruCmdTOIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdTOOut;
   
typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char Diff[5];
   unsigned char PIN[7];
   unsigned char PAN[12];
   unsigned char CheckVal;
} StruCmdTQIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char PIN[6];
} StruCmdTQOut;

typedef struct {
   unsigned short len;
//   unsigned char head[8];
   unsigned char head[40];
   unsigned char cmd[2];
} StruCmdNCIn;

typedef struct {
   unsigned short len;
//   unsigned char usr[8];
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char ckvl[16];
} StruCmdNCOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char Type;
   unsigned char TMK[48];
} StruCmdHCIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char key[48];
} StruCmdHCOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char Type[2];
   unsigned char Len;
   unsigned char TMK[48];
} StruCmdBUIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char Chck[16];
} StruCmdBUOut;

typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
   unsigned char Pin[9];
   unsigned char PAN[12];
} StruCmdTVIn;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
} StruCmdTVOut;

typedef struct {
   unsigned short len;
   unsigned char usr[40];
   unsigned char cmd[2];
   unsigned char code[2];
   unsigned char pinlen[2];
   unsigned char pin[16];
   unsigned char destpinformat[2];
} StruCmdCCOut;



typedef struct {
   unsigned short len;
   unsigned char head[40];
   unsigned char cmd[2];
} StruCmdCCIn;
 
/*#pragma pack()*/
#endif
