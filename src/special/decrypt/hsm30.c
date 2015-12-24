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

   LOG4C(( LOG_DEBUG, "加密机地址=[%s:%d]",epczAddr, eiPort));

   iSock = tcp_connect( epczAddr, eiPort );
   if( iSock == -1 )
   {
      LOG4C(( LOG_INFO, "与前置[%s:%d]连接失败,error=%s", epczAddr, eiPort, strerror(errno) ));
      return  -1;
   }

//    memset( ascii_buf, 0,sizeof(ascii_buf) );
//    ProBcdToAscii ( ascii_buf, epczSendBuf, eiSendLen*2);
//    LOG4C(( LOG_DEBUG, "SEND[%s]", ascii_buf ));


//   TOOL_Dump( LOG_DEBUG, "transHSM.dump", (unsigned char*) epczSendBuf,  eiSendLen);

   iRet = tcp_ctl_send( iSock, epczSendBuf, eiSendLen, 10000, enumHead );
   if( iRet == -1 )
   {
      LOG4C(( LOG_INFO, "往前置[%s:%d]发送失败,error=%s", epczAddr, eiPort, strerror(errno) ));
      tcp_close( iSock );
      return  -1 ;
   }

   iRet = tcp_ctl_recv( iSock, opczRecvBuf, 2000, 12000, enumHead );
   if( iRet == -1 )
   {
      LOG4C(( LOG_INFO, "从前置[%s:%d]接收失败,error=%s", epczAddr, eiPort, strerror(errno) ));
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
* <0xD107> 产生指定长度的随机工作密钥，并用LMK和指定ZMK加密后返回      *
* 输入参数：                                                                                                                        *
*  keylen:      随机密钥长度：长度为1字节，为8或16或24；               *
*   keytype:    密钥类型：长度为1字节                                  *
*           0x01 = 区域主密钥；                                                                                                                                                                            *
*           0x02 = 终端主密钥；                                                                                                                                                                            *
*           0x11 = PIN加密密钥；                                       *
*           0x12 = MAC计算密钥；                                       *
*           0x13 = 数据加密密钥；                                      *
*      0x21: CVV计算密钥                                               *
*   zmkindex:   通信主密钥索引：长度为2字节,值为0-999                  *
************输出参数：                                                                                                                      *
*   *keybylmk:      密文随机密钥：长度为16/32/48字节，用LMK加密；      *
*  *keybyzmk:       密文随机密钥：长度为16/32/48字节，用ZMK加密；      *
*   *checkvalue:    校验码：8字节，随机密钥对8字节0加密结果；          *
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
   *功能：导入密钥，将ZMK加密的密钥转换为LMK加密的密钥密文
   *入参：
 *   nZMKeyLen：传输密钥长度：8/16/24
 *   nWKeyLen： 工作密钥长度：8/16/24
 *   nIndex:    传输主密钥索引
 *   nKeyType:  工作密钥类型：0x01：通信主密钥;
                              0x11：PIN加密密钥；
                              0x12：MAC计算密钥；
                              0x13：数据加密密钥
                              0x21:  CVV计算密钥
 *   WKeyUnderZMK：工作密钥密文
 *   nCheckLen：   校验值长度
 *   checkValue    校验值
 *********出参：
 *   nOutKeyLen:  输出密钥密文长度
 *   nOutKey   ： 输出密钥
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
* <CmdD126> PINBLOCK解密，用输入密钥（LMK加密）对PINBLOCK解密                   *
* 输入参数：                                                                                                                        *
*  piklen : PIK字节长度,    1字节，值8或16或24                                                  *
*   *pik:           密文工作密钥：长度为8字节或16字节或24字节，为LMK变种加密结果；*
*   pinformat:PINBLOCK格式,长度为1字节
*           1表示为ANXI9.8带主帐号(ISO9564-1-0；                                                        *
*           2表示为Docutel ATM，1位长度＋n位PIN＋用户定义数据                               *
*           3表示为Diebold and IBM ATM, n位PIN＋F                                                       *
*           4表示为Plus Network, 与格式1差别在于取主帐号最左12位                        *
*           5表示为ISO9564-1-1格式，1NP..PR...R                                                         *
*           6表示为ANXIX9.8不带主帐号；                                                                         *
*   *pinblock:用PIK加密后的密文PINBLOCK，8字节                                          *
*   *account:  主帐号或补位码：12-19字节，ASCII字符，只当格式需要主帐号或补位数据时才有,字串以'\0'结尾*
************输出参数：                                                                                                                      *
*   *pinlen:        PIN长度,长度为1字节，表示明文PIN长度                                            *
*   *pin: 明文PIN：4至12字节，ASCII字符                                                                 *
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
   *功能：计算MAC
   *入参：
 *   iKeyLen：MAK长度 0x01：8字节；0x02：16字节；0x03：24字节
 *   iAlg：MAC算法：0x01：XOR；0x02：ansi9.9；0x03：ansi9.19
 *   caCipherMAKUndLMK：LMK加密的MAK
 *   iDataLen：计算MAC数据长度
 *   bData：计算MAC的数据
 ******出参：
 *   caMac：计算后的MAC
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

    /*计算块*/
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


/*区域密钥加密工作密钥处理*/

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


