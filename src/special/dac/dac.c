#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

S_DAC(str,mac)
char str[];
char mac[];
{

int len , i , j ;
char string[4][17];
char mac_bcd[65];
char obj[65],obj1[65];
char pinkey[17];
char pinkey_bcd[65];

	len = ( int ) strlen ( str );
	if ( len > 64 ) {
		str[64] = 0;
	} else if ( len < 64 ) {
		for( i = len ; i < 64 ; i++ ) str[i] = '0';
		str[i] = 0;
	}
	
	for ( i = 0 ; i < 4 ; i++ ) {
		memcpy ( string[i] , &(str[i*16]),16 );
		string[i][16] = 0;
	}
	for ( i = 0 ; i < 16 ; i++ ) {
		len=0;
		for ( j = 0 ; j < 4 ; j++ ) {
			len += chtonum( string[j][i] ); 
		}
		numtoch(&mac[i],len);
	}
	mac[i]=0;

	if ( dis_pinkey(pinkey) !=0 ) return ( -1 );

	card_change(pinkey,pinkey_bcd);
	card_change(mac,mac_bcd);
	for(i=0;i<64;i++) {
		mac_bcd[i] -='0';
		pinkey_bcd[i]-='0';
	}

	CDTDES(pinkey_bcd,mac_bcd,obj1);

	for ( i =0 ; i < 64 ; i++) {
		obj[i]=obj1[i]+'0';
	}
	obj[64]=0;

	T_bcd(obj,mac);
	return ( 0 );
}

dis_pinkey(pinkey)
char pinkey[];
{
char masterkey[17];
char enmasterkey[17];
char enpinkey[17];
char pinkey_bcd[65];
char masterkey_bcd[65];
char obj[65];
char obj1[65];
int i;

	if ( read_en_master_key(enmasterkey,enpinkey) !=0 ) return ( -1 );

	en_masterkey(enmasterkey,masterkey);

	card_change(enpinkey,pinkey_bcd);

	card_change(masterkey,masterkey_bcd);

	for(i=0;i<64;i++) {
		masterkey_bcd[i]-='0';
		pinkey_bcd[i]-='0';
	}

	CDT_DES(masterkey_bcd,pinkey_bcd,obj);

	for(i=0;i<64;i++) {
		obj1[i]=obj[i]+'0';
	}
	obj1[i]=0;
	T_bcd(obj1,pinkey);

	return ( 0 );
}

en_masterkey(sou_buf,obj_buf)
char sou_buf[];
char *obj_buf;
{
char HEX_const[17];
int num;
int i,d1,d2;
int Hex;
	strcpy(HEX_const,"1234567890ABCDEF");
	memset(obj_buf,0,sizeof(obj_buf));
	for(i=0;i<16;i++) {
		if(HEX_const[i]>='0' && HEX_const[i]<='9')
			d1=HEX_const[i]-'0';
		else 
			d1=HEX_const[i]-'A'+10;
		if(sou_buf[i]>='0' && sou_buf[i]<='9')
			d2=sou_buf[i]-'0';
		else 
			d2=sou_buf[i]-'A'+10;
		num= d1 ^ d2;
		if(num<10) 
			obj_buf[i]=num+'0';
		else 
			obj_buf[i]=num-10+'A';
	}
	obj_buf[16]=0;
}

card_change(str1,obj1) 
char *str1;
char *obj1;
{
int i;
char ch1;
char bcdm[5];
int len;

	obj1[0]=0;
	bcdm[0]=0;
	len=(int)strlen(str1);
/*
	for(i=0;i<(int)strlen(str1);i++) {
*/
	for(i=0;i<len;i++) {
		ch1=str1[i];
		card_bcd(ch1,bcdm);
		strcat(obj1,bcdm);
	}
	obj1[4*len]=0;
}

card_bcd(ch,bcdm1)
char ch;
char *bcdm1;
{
int num;
int i;
int j;
	switch(ch) {
		case '0': case '1': case '2':
		case '3': case '4': case '5':
		case '6': case '7': case '8':
		case '9':
			num=ch-'0';
			break;
		case 'A': case 'B': case 'C':
		case 'D': case 'E': case 'F':
			num=10+ch-'A';
			break;
		case 'a': case 'b': case 'c':
		case 'd': case 'e': case 'f':
			num=10+ch-'a';
			break;
	}
	for(i=0;i<4;i++) {
		if(num<2) {
			bcdm1[3-i]=num+'0';
			i++;
			break;
		} else {
			bcdm1[3-i]=num%2+'0';
			num=num/2;
		}
	}
	if(i!=4) {
		for(j=0;j<4-i;j++) bcdm1[j]='0';
	}
	bcdm1[4]='\0';
}

T_bcd(bcdm,buf)
char *bcdm;
char *buf;
{
int i,j,len,sum=0;
	
	len = ( int ) strlen ( bcdm );
	for(i = 0 ; i < len / 4;i++) {
		sum=bcdm[i*4]-'0';
		for(j=1;j<4;j++) {
			sum=sum*2+(bcdm[i*4+j]-'0');
		}
		if(sum>=0 && sum<=9)
			buf[i]=sum+'0';
		else 
			buf[i]=sum-10+'A';
	}
	buf[len/4]=0;
}

numtoch(ch,num)
char *ch;
int num;
{
	num  = num % 64;
	if ( num >= 10 && num <= 35) {
		*ch='a'+(num-10);
	} else if ( num>=36 && num <= 61) {
		*ch='A'+(num-36);
	} else if ( num == 62 ) {
		*ch = 'C';
	} else if ( num == 63 ) {
		*ch = 'D';
	} else {
		*ch = num+'0';
	}
	if ( *ch>'f' && *ch <='z') {
		*ch = (*ch-'g')%6+'a';
	} else if ( *ch>'F' && *ch <='Z') {
		*ch = (*ch-'G')%6+'A';
	}
}

chtonum(ch)
char ch;
{

int num;
	if(ch == '.')  {
		num=62;
	} else if(ch == '-')  {
		num=63;
	} else if ( ch >= 'a' && ch <='z' ) {
		num=ch-'a'+10;
	} else if ( ch >= 'A' && ch <='Z' ) {
		num=ch-'A'+36;
	} else if (ch >= '0' && ch <= '9' ) {
		num=ch-'0';
	} else {
		num=0;
	}
	return ( num );
}
