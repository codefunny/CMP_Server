/*
 * crypt.h ---- Key management and data encryption/decryption.
 */

#ifndef _CRYPT_H_
#define _CRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

void  mydes(unsigned char *output, const unsigned char *input, const unsigned char *key, int direction);
void  EncryptPIN (unsigned char cipher[], const char plain[], const char pan[], const unsigned char pin_key[]);
void  DecryptPIN (char plain[], const unsigned char cipher[], const char pan[], const unsigned char pin_key[]);
void  CalcMAC (char mac[], const char data[], const unsigned char mac_key[]);
void  CalcMAC1 (unsigned char mac[], const unsigned char data[], int len, const unsigned char mac_key[]);
void  CalcMAC_X99 (char mac[], const char data[], int len, const unsigned char mac_key[]);
void  CalcMAC_X919 (char mac[], const char data[], int len, const unsigned char mac_key[]);
void  EncryptKey (char cipher[], const unsigned char plain[]);
void  DecryptKey (unsigned char plain[], const char cipher[]);
void  mydes3(unsigned char *plain, unsigned char *crypt, const unsigned char *key, char flag);

#ifdef __cplusplus
}
#endif

#endif
