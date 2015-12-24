#ifndef __HSM30_H__
#define __HSM30_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HSM_BUF_MAX_SIZE 2048
#define MAX_DATA_LEN (HSM_BUF_MAX_SIZE-50)


/* ������궨�� */
#define    ERR_HSM_CONNT            -9001            /*���Ӽ��ܻ�ʧ��*/
#define    ERR_HSM_SEND             -9002            /*�������ݵ����ܻ�ʧ��*/
#define    ERR_HSM_RECV             -9003            /*�������ݵ����ܻ�ʧ��*/
#define    ERR_HSM_PARA             -9004           /*��������*/
#define    ERR_HSM_FAIL             -9005           /*���ü��ܻ�ʧ��*/



/*��ȡ������Կ*/
typedef struct
{
    unsigned char uszHostIP[15+1];                  /*���ܻ���ַ*/
    unsigned int unPort;                            /*���ܻ��˿�*/
    unsigned char ucKeyLen;                         /*��Կ����*/
    unsigned int unZmkIndex;                        /*��������Կ����*/
}ST_HSM30_GENERATEWORKKEY_IN;


typedef struct
{
    unsigned char uszPinKeyByZmk[16+1];             /*PIN key ��������Կ��������*/
    unsigned char uszMacKeyByZmk[16+1];             /*MAC key ��������Կ��������*/
    unsigned char uszPinKeyByLmk[16+1];             /*PIN key ��������Կ��������*/
    unsigned char uszMacKeyByLmk[16+1];             /*MAC key ��������Կ��������*/
    unsigned char uszPinKeyCheckValue[8+1];         /*PIN KEY У��ֵ*/
    unsigned char uszMacKeyCheckValue[8+1];         /*MAC KEY У��ֵ*/
}ST_HSM30_GENERATEWORKKEY_OUT;

/*����PIN*/
typedef struct
{
    unsigned char uszHostIP[15+1];                  /*���ܻ���ַ*/
    unsigned int unPort;                            /*���ܻ��˿�*/
    unsigned char ucKeyLen;                         /*��Կ����*/
    unsigned int unZmkIndex;                        /*��������Կ����*/
    unsigned char uszPinKeyByZmk[16+1];             /*PIN key ��������Կ��������*/
    unsigned char uszAcctNo[19+1];                  /*���˺�*/
    unsigned char uszPinBlock[8+1];                 /*PIN BLOCK*/
}ST_HSM30_DECRYPT_PIN_IN;

typedef struct
{
    unsigned char ucPinPlainLen;                    /*��Կ���ĳ���*/
    unsigned char uszPinPlain[13+1];                    /*��Կ����*/
}ST_HSM30_DECRYPT_PIN_Out;


/*����MAC*/
typedef struct
{
    unsigned char uszHostIP[15+1];                  /*���ܻ���ַ*/
    unsigned int unPort;                            /*���ܻ��˿�*/
    unsigned char ucMacKeyLen;                      /*mac ��Կ����*/
    unsigned int unZmkIndex;                        /*��������Կ����*/
    unsigned char uszMacKey[16+1];                  /*MAC key ��������Կ��������*/
    unsigned int unDataLen;                         /*���ݳ���*/    
    unsigned char usData[HSM_BUF_MAX_SIZE];         /*����*/
}ST_HSM30_GENERATE_MAC_IN;

typedef struct
{
    unsigned char uszMac[8+1];                      /*mac */
}ST_HSM30_GENERATE_MAC_OUT;



/*����MAC*/
typedef struct
{
    unsigned char uszHostIP[15+1];                  /*���ܻ���ַ*/
    unsigned int unPort;                            /*���ܻ��˿�*/
    unsigned char ucMacKeyLen;                      /*mac ��Կ����*/
    unsigned int unZmkIndex;                        /*��������Կ����*/
    unsigned char uszMacKey[16+1];                  /*MAC key ��������Կ��������*/
	unsigned char ucMode;							/*�㷨��ʶ0x01:XOR 0x02: ANSI9.9 0x03:ANSI9.19*/
    unsigned int unDataLen;                         /*���ݳ���*/    
    unsigned char usData[HSM_BUF_MAX_SIZE];         /*����*/
}ST_HSM30_GENERATE_MAC_EXT_IN;

typedef struct
{
    unsigned char uszMac[8+1];                      /*mac */
}ST_HSM30_GENERATE_MAC_EXT_OUT;


/*�������������Կ*/
typedef struct
{
    unsigned char cmd[2];
    unsigned char KeyLen;
    unsigned char KeyType;
    unsigned char Index[2];
    unsigned char TMKLen; /*�������Index == 0xFFFFʱ����*/
    unsigned char TMK[1]; /*�������Index == 0xFFFFʱ����*/
} StruCmdD107In;

typedef struct
{
    unsigned char Code;
    unsigned char KeyLen;
    unsigned char Key[1]; /*CipherKeyUnderLMK + CipherKeyUnderTMK + KeyCheckValue*/
} StruCmdD107Out;


typedef struct
  {
      unsigned char cmd[2];
      unsigned char keylen;
      unsigned char key[1]; /* cipher Key(by lmk) + pinfomat + cpin + pan */
  }               StruCmdD126In;

  typedef struct
  {
      unsigned char code;
      unsigned char pinlen;
      unsigned char pin[13]; /* plain pin */
  }               StruCmdD126Out;


/*����MAC*/
typedef struct
{
    unsigned char cmd[2];
    unsigned char flag;
    unsigned char keylen;
    unsigned char key[1]; /* cipher Key1(by lmk) + IV + datalen(int) +
                * data */
}StruCmdD132In;

typedef struct
{
    unsigned char code;
    unsigned char mac[8];
}StruCmdD132Out;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HSM30_H__ */

