#ifndef __TRANS_CODE_H__
#define __TRANS_CODE_H__

#define SNDLEN			8192					/*	7"KMJ}>]3$6H			*/
#define RCVLEN			8192					/*	=SJUJ}>]3$6H			*/
#define SNDHDLEN		287						/*	IO4+PEO"M73$6H			*/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

  // 2?7V ASCII Bk:j6(Re

  /*    8qJ=?XVF (FE) @`WV7{    */
  /* MK8q    */
#define BS          '\010'
#define BACKSPACE   '\010'

  /* :aOr (K.F=) VF1m    */
#define HT          '\011'
#define TAB         '\011'

  /* ;;PP        */
#define LF          '\012'

  /* W]Or (49V1) VF1m    */
#define VT          '\013'

  /* ;;R3        */
#define FF          '\014'

  /* ;X35        */
#define CR          '\015'
#define RET         '\015'
#define RETURN      '\015'
  /*    Ih18?XVF (DC) @`WV7{    */

  /*    FdK|?XVF@`WV7{    */
  /* ?UWV7{    */
#define NUL         '\0'

#define BEL         '\007'
  /* 8f>/ (OlAe)    */
#define BELL        '\007'

  /* Ww7O        */
#define CAN         '\030'

  /* C=Le=aJx    */
#define EM          '\031'

  /* Lf;;        */
#define SUB         '\032'

 /* W*Re        */
#define ESC         '\033'
#define ESCAPE      '\033'

 /* <d8t (?U8q)    */
#define SP          '\040'
#define SPACE       '\040'
#define BLANK       '\040'
 /* D(5t        */
#define DEL         '\177'
#define DELETE      '\177'

 //  M<PNWV7{6(Re

#define QX          ")%"
#define WX          ")W"
#define TX          ")G"
#define UX          ")O"
#define JX          ")?"
#define KX          ")7"
#define LX          ")3"
#define XX          ")'"
#define VX          ")_"
#define NX          ")o"
#define MX          ");"


#define QY          ")$"
#define WY          ")P"
#define TY          ")@"
#define UY          ")H"
#define JY          ")<"
#define KY          ")4"
#define LY          ")0"
#define XY          ")&"
#define VY          ")X"
#define NY          ")`"
#define MY          ")8"

  // SP9X1jV>:j6(Re
#define YES         'y'
#define NO          'n'
#define ERR         (-1)
#define ARG_ERR     (-2)
#define ENGLISH     0
#define CHINESE     1
#define CONSOLE     2

#define prgname     "scus"

  /* message line */
#define MSGLINE     21
#define MSGCOLUMN   11

  /* function key line */
#define FTKEYLINE   23
  // title 1
#define HEADLINE1   0
  // title 2
#define HEADLINE2   1

#define LEFT        1
#define RIGHT       0
typedef struct
 {
    unsigned char first_byte;
    unsigned char second_byte;
 } CH_WORD;

char ISPWD[SNDLEN+1];
unsigned char g_compress_data[254];
unsigned short g_data_len;

#ifdef __cplusplus
extern "C"
{
#endif 

 extern int EBCtoASC(unsigned char *in_EBC_str,unsigned char *out_ASC_str, \
              int EBC_str_len);
 extern int ASCtoEBC(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
             int ASC_str_len);
 extern int ASCtoEBC1(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
              int ASC_str_len);
 extern int ASCtoEBC2(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
	     int ASC_str_len, int EBC_str_len);
 extern int Turn_ASCtoEBC_Str(unsigned char *in_ASC_str,int ASC_str_len,\
              int EBC_str_len);
 extern void IC_ASCtoEBC(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
                 int ASC_str_len);
 extern unsigned char SB_ASCIItoEBCDIC(unsigned char in_ASC_byte_value);
 extern unsigned char SB_EBCDICtoASCII(unsigned char in_EBC_byte_value);
 extern CH_WORD DB_ASCIItoEBCDIC_01(CH_WORD ASC_WORD);
 extern CH_WORD DB_EBCDICtoASCII_01( CH_WORD EBC_WORD);
 extern CH_WORD DB_ASCIItoEBCDIC_02(CH_WORD ASC_WORD);
 extern CH_WORD DB_EBCDICtoASCII_02( CH_WORD EBC_WORD);

extern void DspMsg(int , char *);
extern void InTrimStr(char *,char *);

#ifdef __cplusplus
}
#endif

#endif
