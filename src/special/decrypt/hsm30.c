#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/errno.h>
#include "log.h"
#include "easysock.h"
#include "hsm30.h"

static void ProBcdToAscii (char * ascii_buf, const unsigned char * bcd_buf, int len)
{
    int i;
    char ch;
    for (i = 0; i < len; i++)
    {
        if (i & 1)
        {
            ch = *(bcd_buf++) & 0x0f;
        }
        else
        {
            ch = *bcd_buf >> 4;
        }
        ascii_buf[i] = ch + ((ch > 9) ? 'A' - 10 : '0');
    }
}

int transHSM( char *epczAddr, int eiPort, char *epczSendBuf,
              int eiSendLen, char *opczRecvBuf, int *opiRecvLen, HeadType enumHead )
{
   int iSock;
   int iRet;
   char ascii_buf[2048];

   LOG4C(( LOG_DEBUG, "���ܻ���ַ=[%s:%d]",epczAddr, eiPort));

   iSock = tcp_connect( epczAddr, eiPort );
   if( iSock == -1 )
   {
      LOG4C(( LOG_INFO, "��ǰ��[%s:%d]����ʧ��,error=%s", epczAddr, eiPort, strerror(errno) ));
      return  -1;
   }

//    memset( ascii_buf, 0,sizeof(ascii_buf) );
//    ProBcdToAscii ( ascii_buf, epczSendBuf, eiSendLen*2);
//    LOG4C(( LOG_DEBUG, "SEND[%s]", ascii_buf ));


//   TOOL_Dump( LOG_DEBUG, "transHSM.dump", (unsigned char*) epczSendBuf,  eiSendLen);

   iRet = tcp_ctl_send( iSock, epczSendBuf, eiSendLen, 10000, enumHead );
   if( iRet == -1 )
   {
      LOG4C(( LOG_INFO, "��ǰ��[%s:%d]����ʧ��,error=%s", epczAddr, eiPort, strerror(errno) ));
      tcp_close( iSock );
      return  -1 ;
   }

   iRet = tcp_ctl_recv( iSock, opczRecvBuf, 2000, 12000, enumHead );
   if( iRet == -1 )
   {
      LOG4C(( LOG_INFO, "��ǰ��[%s:%d]����ʧ��,error=%s", epczAddr, eiPort, strerror(errno) ));
      tcp_close( iSock );
      return  -1;
   }
   *opiRecvLen = iRet;

 //   memset( ascii_buf, 0,sizeof(ascii_buf) );
//    ProBcdToAscii ( ascii_buf, opczRecvBuf, iRet*2);
//    LOG4C(( LOG_DEBUG, "RECV[%s]", ascii_buf ));

   tcp_close( iSock );

   return 0;
}

/***********************************************************************
* <0xD107> ����ָ�����ȵ����������Կ������LMK��ָ��ZMK���ܺ󷵻�      *
* ���������                                                                                                                        *
*  keylen:      �����Կ���ȣ�����Ϊ1�ֽڣ�Ϊ8��16��24��               *
*   keytype:    ��Կ���ͣ�����Ϊ1�ֽ�                                  *
*           0x01 = ��������Կ��                                                                                                                                                                            *
*           0x02 = �ն�����Կ��                                                                                                                                                                            *
*           0x11 = PIN������Կ��                                       *
*           0x12 = MAC������Կ��                                       *
*           0x13 = ���ݼ�����Կ��                                      *
*      0x21: CVV������Կ                                               *
*   zmkindex:   ͨ������Կ����������Ϊ2�ֽ�,ֵΪ0-999                  *
************���������                                                                                                                      *
*   *keybylmk:      ���������Կ������Ϊ16/32/48�ֽڣ���LMK���ܣ�      *
*  *keybyzmk:       ���������Կ������Ϊ16/32/48�ֽڣ���ZMK���ܣ�      *
*   *checkvalue:    У���룺8�ֽڣ������Կ��8�ֽ�0���ܽ����          *
************************************************************************/
static int ProD107Command(unsigned char *puszHostIP,
    unsigned int unPort,
    unsigned char ucKeyLen,
    unsigned char ucKeyType,
    unsigned int unZmkIndex,
    unsigned char *puszKeyByLmk,
    unsigned char *puszKeyByZmk,
    unsigned char *puszCheckValue)
{
    unsigned char secbuf_in[HSM_BUF_MAX_SIZE];
    unsigned char secbuf_out[HSM_BUF_MAX_SIZE];
    int wlen;
    int recvLen=0;
    int retval;
    StruCmdD107In  *in = (StruCmdD107In *) &secbuf_in[0];
    StruCmdD107Out *out = (StruCmdD107Out *) &secbuf_out[0];

    /* command code */
    in->cmd[0] = 0xD1;
    in->cmd[1] = 0x07;

    /* work key len */
    if (ucKeyLen != 8 && ucKeyLen != 16 && ucKeyLen != 24)
    {
        return ERR_HSM_PARA;
    }
    in->KeyLen = ucKeyLen;

    /* work key type */
    in->KeyType = ucKeyType;

    /* zmk index */
    in->Index[0] = unZmkIndex / 256;
    in->Index[1] = unZmkIndex % 256;

    wlen = 2 + 1 + 1 + 2;

    retval = transHSM( puszHostIP, unPort, secbuf_in, (int)wlen, secbuf_out, &recvLen, NO_HEAD );

    if (retval < 0)
    {
        return retval;
    }

    memcpy(puszKeyByZmk, out->Key, ucKeyLen);
    memcpy(puszKeyByLmk, out->Key + ucKeyLen, ucKeyLen);
    memcpy(puszCheckValue, out->Key + ucKeyLen + ucKeyLen, 8);
    return 0;

}

int Hsm30_GerenateMacKey(ST_HSM30_GENERATEWORKKEY_IN *pstIn,
    ST_HSM30_GENERATEWORKKEY_OUT *pstOut)
{
    int nRet;
    unsigned char ucKeyType;

    /* mac key */
    ucKeyType = 0x12;
    nRet = ProD107Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        ucKeyType,
        pstIn->unZmkIndex,
        pstOut->uszMacKeyByLmk,
        pstOut->uszMacKeyByZmk,
        pstOut->uszMacKeyCheckValue);
    if (nRet)
    {
        return nRet;
    }

    return 0;
}
int Hsm30_GerenatePinKey(ST_HSM30_GENERATEWORKKEY_IN *pstIn,
    ST_HSM30_GENERATEWORKKEY_OUT *pstOut)
{
    int nRet;
    unsigned char ucKeyType;

    /* Pin Key */
    ucKeyType = 0x11;
    nRet = ProD107Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        ucKeyType,
        pstIn->unZmkIndex,
        pstOut->uszPinKeyByLmk,
        pstOut->uszPinKeyByZmk,
        pstOut->uszPinKeyCheckValue);
    if (nRet)
    {
        return nRet;
    }

    return 0;
}

int Hsm30_GerenateWorkKey(ST_HSM30_GENERATEWORKKEY_IN *pstIn,
    ST_HSM30_GENERATEWORKKEY_OUT *pstOut)
{
    int nRet;
    unsigned char ucKeyType;

    /* Pin Key */
    ucKeyType = 0x11;
    nRet = ProD107Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        ucKeyType,
        pstIn->unZmkIndex,
        pstOut->uszPinKeyByLmk,
        pstOut->uszPinKeyByZmk,
        pstOut->uszPinKeyCheckValue);
    if (nRet)
    {
        return nRet;
    }
    /* mac key */
    ucKeyType = 0x12;
    nRet = ProD107Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        ucKeyType,
        pstIn->unZmkIndex,
        pstOut->uszMacKeyByLmk,
        pstOut->uszMacKeyByZmk,
        pstOut->uszMacKeyCheckValue);
    if (nRet)
    {
        return nRet;
    }
    return 0;
}

/*********************************************************************************
   *���ܣ�������Կ����ZMK���ܵ���Կת��ΪLMK���ܵ���Կ����
   *��Σ�
 *   nZMKeyLen��������Կ���ȣ�8/16/24
 *   nWKeyLen�� ������Կ���ȣ�8/16/24
 *   nIndex:    ��������Կ����
 *   nKeyType:  ������Կ���ͣ�0x01��ͨ������Կ;
                              0x11��PIN������Կ��
                              0x12��MAC������Կ��
                              0x13�����ݼ�����Կ
                              0x21:  CVV������Կ
 *   WKeyUnderZMK��������Կ����
 *   nCheckLen��   У��ֵ����
 *   checkValue    У��ֵ
 *********���Σ�
 *   nOutKeyLen:  �����Կ���ĳ���
 *   nOutKey   �� �����Կ
 ********************************************************************************/
static int ProD102Command(unsigned char *puszHostIP,
    unsigned int unPort,
    unsigned int unZmkKeyLen,
    unsigned int unWorkKeyLen,
    unsigned int unKeyType,
    unsigned int unZmkIndex,
    unsigned char *puszWorkKeyUnderZmk,
    unsigned int unCheckValueLen,
    unsigned char *puszCheckValue,
    unsigned int *pnOutKeyLen,
    unsigned char *pusOutKey)
{
    unsigned char secbuf_in[HSM_BUF_MAX_SIZE];
    unsigned char secbuf_out[HSM_BUF_MAX_SIZE];
    unsigned char *in = (unsigned char *)secbuf_in;
    unsigned char *out = (unsigned char *)secbuf_out;
    int wlen, retval;
    int recvLen=0;

    unsigned char *lp;

    wlen = 0;

    in[0] = 0xD1;
    in[1] = 0x02;

    wlen += 2;

    in[2] = unWorkKeyLen & 0xFF;
    in[3] = unZmkKeyLen & 0xFF;

    wlen += 2;

    in[4] = unZmkIndex / 256;
    in[5] = unZmkIndex % 256;

    wlen += 2;

    in[6] = 0xFF & unKeyType;

    wlen += 1;

    memcpy(&in[wlen], puszWorkKeyUnderZmk, unWorkKeyLen);

    wlen += unWorkKeyLen;

    in[wlen] = unCheckValueLen & 0xFF;
    wlen += 1;
    if (unCheckValueLen != 0)
    {
        memcpy(&in[wlen], puszCheckValue, unCheckValueLen);
    }
    wlen += unCheckValueLen;

    retval = transHSM( puszHostIP, unPort, (char *)secbuf_in, (int)wlen, secbuf_out, &recvLen, NO_HEAD );

    if (retval < 0)
    {
        return retval;
    }
    if (secbuf_out[0] != 'A')
    {
        return ERR_HSM_FAIL;
    }
    *pnOutKeyLen = secbuf_out[1];
    memcpy(pusOutKey, secbuf_out + 2, *pnOutKeyLen);
    return 0;

}

/************************************************************************
* <CmdD126> PINBLOCK���ܣ���������Կ��LMK���ܣ���PINBLOCK����                   *
* ���������                                                                                                                        *
*  piklen : PIK�ֽڳ���,    1�ֽڣ�ֵ8��16��24                                                  *
*   *pik:           ���Ĺ�����Կ������Ϊ8�ֽڻ�16�ֽڻ�24�ֽڣ�ΪLMK���ּ��ܽ����*
*   pinformat:PINBLOCK��ʽ,����Ϊ1�ֽ�
*           1��ʾΪANXI9.8�����ʺ�(ISO9564-1-0��                                                        *
*           2��ʾΪDocutel ATM��1λ���ȣ�nλPIN���û���������                               *
*           3��ʾΪDiebold and IBM ATM, nλPIN��F                                                       *
*           4��ʾΪPlus Network, ���ʽ1�������ȡ���ʺ�����12λ                        *
*           5��ʾΪISO9564-1-1��ʽ��1NP..PR...R                                                         *
*           6��ʾΪANXIX9.8�������ʺţ�                                                                         *
*   *pinblock:��PIK���ܺ������PINBLOCK��8�ֽ�                                          *
*   *account:  ���ʺŻ�λ�룺12-19�ֽڣ�ASCII�ַ���ֻ����ʽ��Ҫ���ʺŻ�λ����ʱ����,�ִ���'\0'��β*
************���������                                                                                                                      *
*   *pinlen:        PIN����,����Ϊ1�ֽڣ���ʾ����PIN����                                            *
*   *pin: ����PIN��4��12�ֽڣ�ASCII�ַ�                                                                 *
************************************************************************/
static int ProD126Command(unsigned char *puszHostIP,
    unsigned int unPort,
    unsigned int unPinKeyLen,
    const unsigned char *pusPik,
    unsigned char ucPinFormat,
    const unsigned char *pusPinBlock,
    const unsigned char *pusAccount,
    unsigned char *pucPinLen,
    unsigned char *puszPin)
{
    unsigned char secbuf_in[HSM_BUF_MAX_SIZE];
    unsigned char secbuf_out[HSM_BUF_MAX_SIZE];

    int wlen;
    unsigned char acclen;
    int retval;
    int recvLen=0;

    StruCmdD126In  *in = (StruCmdD126In *) &secbuf_in[0];
    StruCmdD126Out *out = (StruCmdD126Out *) &secbuf_out[0];

    /* Command Code */
    in->cmd[0] = 0xD1;
    in->cmd[1] = 0x26;

    /* Pik Length */
    if (unPinKeyLen != 8 && unPinKeyLen != 16 && unPinKeyLen != 24)
    {
        return ERR_HSM_PARA;
    }
    in->keylen = unPinKeyLen;
    /* PIK */
    memcpy(&in->key[0], pusPik, unPinKeyLen);
    /* Pin Format */
    if (ucPinFormat < 1 || ucPinFormat > 7)
    {
        return ERR_HSM_PARA;
    }
    in->key[unPinKeyLen] = ucPinFormat;

    /* Pin block */
    memcpy(&in->key[unPinKeyLen + 1], pusPinBlock, 8);

    /* Account */
    if (ucPinFormat == 1 || ucPinFormat == 4)
    {
        acclen = strlen(pusAccount);
        if (acclen < 12 || acclen > 19)
        {
            return ERR_HSM_PARA;
        }
        strcpy(&in->key[unPinKeyLen + 1 + 8], pusAccount);
    }
    else
    {
        acclen = 0;
    }

    wlen = 2 + 1 + unPinKeyLen + 1 + 8 + acclen;

    retval = transHSM( puszHostIP, unPort, secbuf_in, (int)wlen, secbuf_out, &recvLen, NO_HEAD );


    if (retval < 0)
    {
        return retval;
    }

    if (secbuf_out[0] != 'A')
    {
        return ERR_HSM_FAIL;
    }

    *pucPinLen = out->pinlen;
    memcpy(puszPin, out->pin, *pucPinLen);
    return 0;
}

int Hsm30_DecryptPin(ST_HSM30_DECRYPT_PIN_IN *pstIn,
    ST_HSM30_DECRYPT_PIN_Out *pstOut)
{

    unsigned char uszPinKeyByLmk[16 + 1];
    unsigned int unLen = 0;

    int nRet = -1;

    memset(uszPinKeyByLmk, 0, sizeof(uszPinKeyByLmk));

    /*
    nRet = ProD102Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        pstIn->ucKeyLen,
        0x11,
        pstIn->unZmkIndex,
        pstIn->uszPinKeyByZmk,
        0,
        NULL,
        &unLen,
        uszPinKeyByLmk);
    if (nRet)
    {
        return nRet;
    }
    */
    memcpy( uszPinKeyByLmk, pstIn->uszPinKeyByZmk, 16 );

    nRet = ProD126Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        uszPinKeyByLmk,
        0x01,
        pstIn->uszPinBlock,
        pstIn->uszAcctNo,
        &pstOut->ucPinPlainLen,
        pstOut->uszPinPlain);
    if (nRet)
    {
        return nRet;
    }
    return 0;
}

/***************************************************************************
   *���ܣ�����MAC
   *��Σ�
 *   iKeyLen��MAK���� 0x01��8�ֽڣ�0x02��16�ֽڣ�0x03��24�ֽ�
 *   iAlg��MAC�㷨��0x01��XOR��0x02��ansi9.9��0x03��ansi9.19
 *   caCipherMAKUndLMK��LMK���ܵ�MAK
 *   iDataLen������MAC���ݳ���
 *   bData������MAC������
 ******���Σ�
 *   caMac��������MAC
 **************************************************************************/
int ProD132Command(unsigned char *puszHostIP,
    unsigned int unPort,
    unsigned int unKeyLen,
    unsigned char *pusMacKeyByLmk,
    unsigned int unDataLen,
    unsigned char *pusData,
    unsigned char *pusMac)
{
    unsigned char secbuf_in[HSM_BUF_MAX_SIZE];
    unsigned char secbuf_out[HSM_BUF_MAX_SIZE];

    int wlen, tmplen;
    int retval;
    int recvLen=0;
    unsigned char vector[9] = {"\x00\x00\x00\x00\x00\x00\x00\x00\x00"};
    unsigned char             *lp;

    memset(secbuf_in, 0, sizeof(secbuf_in));
    memset(secbuf_out, 0, sizeof(secbuf_out));
	
	
    StruCmdD132In  *in = (StruCmdD132In *) &secbuf_in[0];
    StruCmdD132Out *out = (StruCmdD132Out *) &secbuf_out[0];

    /* Command Code */
    in->cmd[0] = 0xD1;
    in->cmd[1] = 0x32;

    /* Flag */
    in->flag = 0x02;

    /* MAK Length */
    if (unKeyLen != 8 && unKeyLen != 16 && unKeyLen != 24)
    {
        return ERR_HSM_PARA;
    }
    in->keylen = unKeyLen;
    /* MAK */
    memcpy(&in->key[0], pusMacKeyByLmk, unKeyLen);
    /* Initial vector */
    memcpy(&in->key[unKeyLen], vector, 8);

    /* Data Length */
    if (unDataLen < 1 || unDataLen > MAX_DATA_LEN)
    {
        return ERR_HSM_PARA;
    }
    tmplen = htons(unDataLen);
    memcpy(&in->key[unKeyLen + 8], (unsigned char *) &tmplen, 2);

    /* Data */
    memcpy(&in->key[unKeyLen + 8 + 2], pusData, unDataLen);
    wlen = 2 + 1 + 1 + unKeyLen + 8 + 2 + unDataLen;

    retval = transHSM( puszHostIP, unPort, secbuf_in, (int)wlen, secbuf_out, &recvLen, NO_HEAD );

    if (retval < 0)
    {
        return retval;
    }

    if (secbuf_out[0] != 'A')
    {
        return ERR_HSM_FAIL;
    }

    lp = pusMac;
    memcpy(lp, out->mac, 8);
    return 0;
}


int Hsm30_GenerateMAC(ST_HSM30_GENERATE_MAC_IN *pstIn,
    ST_HSM30_GENERATE_MAC_OUT *pstOut )
{
    unsigned char uszMacKeyByLmk[16 + 1];
    unsigned int unLen = 0;
    int nRet = -1;
    unsigned char uszMacBlock[16 + 1];
    unsigned char uszTmp[16 + 1];
    unsigned char uszMac[8 + 1];
    int i = 0;
    int j = 0;

    memset(uszMacKeyByLmk, 0, sizeof(uszMacKeyByLmk));

    memcpy( uszMacKeyByLmk, pstIn->uszMacKey, 16 );

    /*�����*/
    memset(uszMacBlock, 0, sizeof(uszMacBlock));
    for (i = 0; i < pstIn->unDataLen; i++)
    {
        uszMacBlock[i & 0x07] ^= pstIn->usData[i];
    }
    memset(uszTmp, 0, sizeof(uszTmp));
    ProBcdToAscii(uszTmp, uszMacBlock, 16);

    nRet = ProD132Command(pstIn->uszHostIP,
        pstIn->unPort, pstIn->ucMacKeyLen,
        uszMacKeyByLmk, 8, uszTmp, uszMac);

    if (nRet)
    {
        return nRet;
    }
    memset(uszMacBlock, 0, sizeof(uszMacBlock));
    memcpy(uszMacBlock, uszMac, 8);

    for (j = 0; j < 8; j++)
    {
        uszMacBlock[j] ^= uszTmp[8 + j];
    }

    nRet = ProD132Command(pstIn->uszHostIP,
        pstIn->unPort, pstIn->ucMacKeyLen,
        uszMacKeyByLmk, 8, uszMacBlock, pstOut->uszMac);
    if (nRet)
    {
        return nRet;
    }

    return 0;
}


/*������Կ���ܹ�����Կ����*/

int Hsm30_DecryptPin_ZMK(ST_HSM30_DECRYPT_PIN_IN *pstIn,
    ST_HSM30_DECRYPT_PIN_Out *pstOut)
{

    unsigned char uszPinKeyByLmk[16 + 1];
    unsigned int unLen = 0;

    int nRet = -1;

    memset(uszPinKeyByLmk, 0, sizeof(uszPinKeyByLmk));

    nRet = ProD102Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        pstIn->ucKeyLen,
        0x11,
        pstIn->unZmkIndex,
        pstIn->uszPinKeyByZmk,
        0,
        NULL,
        &unLen,
        uszPinKeyByLmk);
    if (nRet)
    {
        return nRet;
    }

    nRet = ProD126Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucKeyLen,
        uszPinKeyByLmk,
        0x01,
        pstIn->uszPinBlock,
        pstIn->uszAcctNo,
        &pstOut->ucPinPlainLen,
        pstOut->uszPinPlain);
    if (nRet)
    {
        return nRet;
    }
    return 0;
}




int Hsm30_GenerateMAC_ZMK(ST_HSM30_GENERATE_MAC_EXT_IN *pstIn,
    ST_HSM30_GENERATE_MAC_EXT_OUT *pstOut)
{

	unsigned char uszMacKeyByLmk[16 + 1];
    	unsigned int unLen = 0;
       int nRet = -1;

       memset(uszMacKeyByLmk, 0, sizeof(uszMacKeyByLmk));
       nRet = ProD102Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucMacKeyLen,
        pstIn->ucMacKeyLen,
        0x12,
        pstIn->unZmkIndex,
        pstIn->uszMacKey,
        0,
        NULL,
        &unLen,
        uszMacKeyByLmk);
	 if (nRet)
	 {
	        return nRet;
	 }


	
//   TOOL_Dump( LOG_DEBUG, "mac3.dump", (unsigned char*) pstIn->usData,  pstIn->unDataLen);

 
        nRet = ProD132Command(pstIn->uszHostIP,
         	pstIn->unPort, pstIn->ucMacKeyLen,
        	uszMacKeyByLmk, pstIn->unDataLen, pstIn->usData, pstOut->uszMac);
    	if (nRet)
    	{
       	return nRet;
    	}
       return 0;
}




#if 0

int Hsm30_GenerateMAC_Ext(ST_HSM30_GENERATE_MAC_EXT_IN *pstIn,
    ST_HSM30_GENERATE_MAC_EXT_OUT *pstOut )
{
    unsigned char uszMacKeyByLmk[16 + 1];
    int nRet = -1;

    memset(uszMacKeyByLmk, 0, sizeof(uszMacKeyByLmk));

    memcpy( uszMacKeyByLmk, pstIn->uszMacKey, pstIn->ucMacKeyLen);
    nRet = ProD132Command(pstIn->uszHostIP,
        pstIn->unPort, pstIn->ucMacKeyLen,
        uszMacKeyByLmk, pstIn->unDataLen, pstIn->usData, pstOut->uszMac);

    if (nRet)
    {
        return nRet;
    }
    return 0;
}

int Hsm30_GenerateMAC_ByZmk(ST_HSM30_GENERATE_MAC_EXT_IN * pstIn, 
	ST_HSM30_GENERATE_MAC_EXT_OUT * pstOut)
{
	unsigned char uszMacKeyByLmk[16 + 1];
    	unsigned int unLen = 0;

    int nRet = -1;

    memset(uszMacKeyByLmk, 0, sizeof(uszMacKeyByLmk));

    nRet = ProD102Command(pstIn->uszHostIP,
        pstIn->unPort,
        pstIn->ucMacKeyLen,
        pstIn->ucMacKeyLen,
        0x12,
        pstIn->unZmkIndex,
        pstIn->uszMacKey,
        0,
        NULL,
        &unLen,
        uszMacKeyByLmk);
    if (nRet)
    {
        return nRet;
    }

    memcpy(pstIn->uszMacKey, uszMacKeyByLmk, pstIn->ucMacKeyLen);
    return Hsm30_GenerateMAC_Ext(pstIn, pstOut);

}
#endif


