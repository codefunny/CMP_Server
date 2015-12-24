#include <stdio.h>


/**********************************************
*单字节码表转换遵循IBM InforWindow 3477-S型   *
*中文工作站用户指南的码表转换原则。           *
*用户如有特殊需求,可自行对码表进行填充。      *
*双字节码表转换遵循IBM Multistation 5550      *
*汉字编码手册。                               *
*单字节码表:                                  *
*      ASCII字符转换成EBCDIC码表              *
*           ASCIItoEBCDICTable[256]           *
*      EBCDIC字符转换成ASCII码表              *
*           EBCDICtoASCIITable[256]           *
*双字节码表:                                  *
*      IBM双字节字符转换成国标GB-2313汉字     *
*      或国标GB-2313汉字转换成IBM双字节字符。 *
*      仅限于前九区的汉字码表。               *
*           IBMtoGBTable[9][188]              *
*      由于十六区至八十七区汉字转换有规律     *
*      可循,故不需要码表,直接转换即可。       *
**********************************************/

// #define _STRICT_ANSI

/*
#include "liba.h"
#include "comm.h"
#include "self_c.h"
*/
#include "transcode.h"

#define EBC_CHINESE_FLAG_START_CHAR     0x0e
#define EBC_CHINESE_FLAG_STOP_CHAR      0x0f
#define EBC_PARTITION_AREA_FLAG_CHAR    0x48
#define EBC_CHINESE_FLAG_START(x)  (x == EBC_CHINESE_FLAG_START_CHAR)?TRUE:FALSE
#define EBC_CHINESE_FLAG_STOP(x)   (x == EBC_CHINESE_FLAG_STOP_CHAR)?TRUE:FALSE
#define EBC_PARTITION_AREA_FLAG(x) (x >= EBC_PARTITION_AREA_FLAG_CHAR)?TRUE:FALSE
#define ASC_CHINESE_FLAG_CHAR     0xa0
#define ASC_PARTITION_AREA_FLAG_CHAR    0xb0
#define ASC_CHINESE_FLAG(x)        (x > ASC_CHINESE_FLAG_CHAR || x >= 0x81)?TRUE:FALSE
#define ASC_PARTITION_AREA_FLAG(x) (x >= ASC_PARTITION_AREA_FLAG_CHAR)?TRUE:FALSE


/*转换输入的EBCDIC字符串in_EBC_str,为输出的ASCII字符串out_ASC_str,
  输入的EBCDIC字符串的长度为EBC_str_len */

// unsigned char *in_EBC_str;  输入的EBCDIC码字符串
// unsigned char *out_ASC_str; 码制转换后输出的ASCII码字符串
// int  EBC_str_len;           输入的EBCDIC码字符串的长度
int EBCtoASC(unsigned char *in_EBC_str,unsigned char *out_ASC_str, \
              int EBC_str_len)
{
   int in_EBC_str_position,out_ASC_str_position=0;    
   unsigned char EBC_byte_value;
   CH_WORD temp ,temp2;

        /*   转换输入的EBCDIC字符串   */
   for (in_EBC_str_position=0;in_EBC_str_position<EBC_str_len;
                                             in_EBC_str_position++) 
     {
       EBC_byte_value=in_EBC_str[in_EBC_str_position];
        /*   转换输入的EBCDIC汉字双字节字符   */
       if ( EBC_CHINESE_FLAG_START(EBC_byte_value) )
         {
           EBC_byte_value=in_EBC_str[++in_EBC_str_position];
       while ((in_EBC_str_position<EBC_str_len) && 
                  !( EBC_CHINESE_FLAG_STOP(EBC_byte_value))) 
             {
                temp.first_byte=EBC_byte_value;
                temp.second_byte=in_EBC_str[++in_EBC_str_position];
            if ((temp.first_byte == 0xa1  && temp.second_byte >= 0x41 && temp.second_byte <= 0x81  )||
  (temp.first_byte >= 0x81 && temp.first_byte <= 0xa0 && temp.second_byte >= 0x41 && temp.second_byte <= 0xfe )) /* 判断是否属于 3 区和4 区*/
            {
                dealEBCtoASC(&temp,&temp2);
                  temp.first_byte = temp2.first_byte;
                  temp.second_byte = temp2.second_byte;

            }
            else
            if ((temp.first_byte == 0xa1  && temp.second_byte >= 0x82 && temp.second_byte <= 0xfe  )||
  (temp.first_byte >= 0xa2 && temp.first_byte <= 0xcc && temp.second_byte >= 0x41 && temp.second_byte <= 0xfe )) /* 判断是否属于 3 区和4 区*/
            {
                dealEBCtoASC4(&temp,&temp2);
                  temp.first_byte = temp2.first_byte;
                  temp.second_byte = temp2.second_byte;

            }
            else
            {
            if ( EBC_PARTITION_AREA_FLAG(temp.first_byte) )

        /*   转换输入的EBCDIC汉字双字节字符,区位从16到87   */
                  temp=DB_EBCDICtoASCII_02(temp);
            else 

        /*   转换输入的EBCDIC汉字双字节字符,区位从1到9   */
                  temp=DB_EBCDICtoASCII_01(temp);
             }
            out_ASC_str[out_ASC_str_position++]=temp.first_byte;
            out_ASC_str[out_ASC_str_position++]=temp.second_byte;
                EBC_byte_value=in_EBC_str[++in_EBC_str_position];
         }

        /*   转换输入的EBCDIC汉字双字节字符串后,转换后的ASCII汉字符串
             要增加两个空格   */
       out_ASC_str[out_ASC_str_position++]= SPACE;
       out_ASC_str[out_ASC_str_position++]= SPACE;
     }
        /*   转换输入的EBCDIC英文单字节字符   */
       else
         out_ASC_str[out_ASC_str_position++]=SB_EBCDICtoASCII(EBC_byte_value);
     }
   out_ASC_str[out_ASC_str_position]='\0';
return out_ASC_str_position;
}               



/*转换输入的ASCII字符串in_ASC_str,为输出的EBCDIC字符串out_EBC_str,
  输入的ASCII字符串的长度为ASC_str_len */

// unsigned char *in_ASC_str;  输入的ASCII码字符串
// unsigned char *out_EBC_str; 码制转换后输出的EBCDIC码字符串
// int  ASC_str_len;           输入的ASCII码字符串的长度
int ASCtoEBC(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
             int ASC_str_len)
{
   int num_of_space_for_chinese=0;                    /*   space  number   */
   int in_ASC_str_position,out_EBC_str_position=0;  
   unsigned char ASC_byte_value;  
   CH_WORD temp ,temp2;

        /*   转换输入的ASCII字符串   */
   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
                                             in_ASC_str_position++) 
     {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
        /*   转换输入的ASCII汉字符   */
       if (  ASC_CHINESE_FLAG(ASC_byte_value) )
         {
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_START_CHAR; 
           while ((in_ASC_str_position<ASC_str_len) && 
                      (ASC_CHINESE_FLAG(ASC_byte_value))) 
             {
               temp.first_byte=ASC_byte_value;
               temp.second_byte=in_ASC_str[++in_ASC_str_position];
               //printf(" temp %02x %02x\n" ,temp.first_byte ,temp.second_byte);
               if (temp.first_byte >= 0x81 && temp.first_byte <= 0xa0 && temp.second_byte >= 0x40 && temp.second_byte <= 0xfe ) /* 判断是否属于 3 区和4 区*/
               {
                //  printf(" 22temp %02x %02x\n" ,temp.first_byte ,temp.second_byte);

                  dealASCtoEBC(&temp,&temp2);
                  temp.first_byte = temp2.first_byte;
                  temp.second_byte = temp2.second_byte;
               }
               else
               if (temp.first_byte >= 0xaa && temp.first_byte <= 0xfe && temp.second_byte >= 0x40 && temp.second_byte <= 0xa0 ) /* 判断是否属于 3 区和4 区*/
               {
                  dealASCtoEBC4(&temp,&temp2);
                  temp.first_byte = temp2.first_byte;
                  temp.second_byte = temp2.second_byte;
               }
               else
               {
               if ( ASC_PARTITION_AREA_FLAG(temp.first_byte) )

        /*   转换输入的ASCII汉字符,区位从16到87   */
                            temp=DB_ASCIItoEBCDIC_02(temp);
               else 

        /*   转换输入的ASCII汉字符,区位从1到9   */
                            temp=DB_ASCIItoEBCDIC_01(temp);
               }

               out_EBC_str[out_EBC_str_position++]=temp.first_byte;
               out_EBC_str[out_EBC_str_position++]=temp.second_byte;
               ASC_byte_value=in_ASC_str[++in_ASC_str_position];
             }
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_STOP_CHAR;
           in_ASC_str_position--;
           num_of_space_for_chinese=num_of_space_for_chinese+2; 
           /*   two  space   */
         }
        /*   转换输入的ASCII英文单字节字符   */
       else 
         {
           if (ASC_byte_value== SPACE)   
             if (num_of_space_for_chinese!=0)
               {
                 num_of_space_for_chinese--;
                 continue;
               }
           out_EBC_str[out_EBC_str_position++]=SB_ASCIItoEBCDIC(ASC_byte_value);
         }                  
     }                     
   out_EBC_str[out_EBC_str_position]='\0';
return out_EBC_str_position;
}                    



/*转换输入的ASCII字符串in_ASC_str,为输出的EBCDIC字符串out_EBC_str,
  输入的ASCII字符串的长度为ASC_str_len */

// unsigned char *in_ASC_str;  输入的ASCII码字符串
// unsigned char *out_EBC_str; 码制转换后输出的EBCDIC码字符串
// int  ASC_str_len;           输入的ASCII码字符串的长度

int ASCtoEBC1(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
              int ASC_str_len)
{
   int num_of_space_for_chinese=0;                    /*   space  number   */
   int in_ASC_str_position,out_EBC_str_position=0;  
   unsigned char ASC_byte_value;  
   CH_WORD temp;

   /*   转换输入的ASCII字符串   */
   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
                                             in_ASC_str_position++) 
   {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
        /*   转换输入的ASCII汉字符   */
       if ( ISPWD[in_ASC_str_position] == 'A' )
       {
          out_EBC_str[out_EBC_str_position++]=ASC_byte_value;
          continue;
       }

       if (  (ASC_CHINESE_FLAG(ASC_byte_value)) && 
         (ISPWD[in_ASC_str_position]!='y') )
       {
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_START_CHAR; 
           while ((in_ASC_str_position<ASC_str_len) && 
                      (ASC_CHINESE_FLAG(ASC_byte_value))) 
             {
               temp.first_byte=ASC_byte_value;
               temp.second_byte=in_ASC_str[++in_ASC_str_position];
               if ( ASC_PARTITION_AREA_FLAG(temp.first_byte) )

        /*   转换输入的ASCII汉字符,区位从16到87   */
                            temp=DB_ASCIItoEBCDIC_02(temp);
               else 

        /*   转换输入的ASCII汉字符,区位从1到9   */
                            temp=DB_ASCIItoEBCDIC_01(temp);

               out_EBC_str[out_EBC_str_position++]=temp.first_byte;
               out_EBC_str[out_EBC_str_position++]=temp.second_byte;
               ASC_byte_value=in_ASC_str[++in_ASC_str_position];
             }
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_STOP_CHAR;
           in_ASC_str_position--;
           num_of_space_for_chinese=num_of_space_for_chinese+2; 
           /*   two  space   */
       }
        /*   转换输入的ASCII英文单字节字符   */
       else {
       if (ISPWD[in_ASC_str_position]!='y') {
              if (ASC_byte_value== SPACE)   
                 if (num_of_space_for_chinese!=0) {
                     num_of_space_for_chinese--;
                     continue;
                 }
       }
           out_EBC_str[out_EBC_str_position++]=SB_ASCIItoEBCDIC(ASC_byte_value);
         }                  
   }                     

   out_EBC_str[out_EBC_str_position]='\0';

   return out_EBC_str_position;
}                    


/*转换输入的ASCII字符串in_ASC_str,为输出的EBCDIC字符串out_EBC_str,
  输入的ASCII字符串的长度为ASC_str_len */

// unsigned char *in_ASC_str;  输入的ASCII码字符串
// unsigned char *out_EBC_str; 码制转换后输出的EBCDIC码字符串
// int  ASC_str_len;           输入的ASCII码字符串的长度
int ASCtoEBC2(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
             int ASC_str_len, int EBC_str_len)
{
   int num_of_space_for_chinese=0;                    /*   space  number   */
   int in_ASC_str_position,out_EBC_str_position=0;  
   unsigned char ASC_byte_value;  
   CH_WORD temp;
   int iZdbz = 0; /*zdbz 0-weizhongduan 1-zhongduan*/

        /*   转换输入的ASCII字符串   */
   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
                                             in_ASC_str_position++) 
     {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
        /*   转换输入的ASCII汉字符   */
       if (  ASC_CHINESE_FLAG(ASC_byte_value) )
         {
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_START_CHAR; 
           while ((in_ASC_str_position<ASC_str_len) && 
                      (ASC_CHINESE_FLAG(ASC_byte_value))) 
             {
               temp.first_byte=ASC_byte_value;
               temp.second_byte=in_ASC_str[++in_ASC_str_position];
               if ( ASC_PARTITION_AREA_FLAG(temp.first_byte) )

        /*   转换输入的ASCII汉字符,区位从16到87   */
                            temp=DB_ASCIItoEBCDIC_02(temp);
               else 

        /*   转换输入的ASCII汉字符,区位从1到9   */
                            temp=DB_ASCIItoEBCDIC_01(temp);

               if( out_EBC_str_position+1 == EBC_str_len - 0 ||
                   out_EBC_str_position+1 == EBC_str_len - 1 )
               {
                  /*out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_STOP_CHAR;*/
                  iZdbz = 1;
                  break;
               }
               out_EBC_str[out_EBC_str_position++]=temp.first_byte;
               out_EBC_str[out_EBC_str_position++]=temp.second_byte;
               ASC_byte_value=in_ASC_str[++in_ASC_str_position];
             }
           out_EBC_str[out_EBC_str_position++]= EBC_CHINESE_FLAG_STOP_CHAR;
           in_ASC_str_position--;
           num_of_space_for_chinese=num_of_space_for_chinese+2; 
           if( iZdbz == 1 )
           {
              break;
           }
        
           /*   two  space   */
         }
        /*   转换输入的ASCII英文单字节字符   */
       else 
         {
           if (ASC_byte_value== SPACE)   
             if (num_of_space_for_chinese!=0)
               {
                 num_of_space_for_chinese--;
                 continue;
               }
           out_EBC_str[out_EBC_str_position++]=SB_ASCIItoEBCDIC(ASC_byte_value);
         }                  
     }                     
   out_EBC_str[out_EBC_str_position]='\0';
return out_EBC_str_position;
}                    

int Turn_ASCtoEBC_Str(unsigned char *in_ASC_str, int ASC_str_len, 
                     int EBC_str_len)
{
   int nCh = 0;
   int num_of_space_for_chinese=0;   
   int in_ASC_str_position;
   int out_EBC_str_position=0;  
   int overlen = 0;
   unsigned char ASC_byte_value;  

   if (ASC_str_len > EBC_str_len) {
       ASC_str_len = EBC_str_len;
       in_ASC_str[ASC_str_len] = '\0';
   }

   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
        in_ASC_str_position++) 
   {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
   
       if (  ASC_CHINESE_FLAG(ASC_byte_value) )
       {
           nCh = 0;
           out_EBC_str_position++;
           while ((in_ASC_str_position<ASC_str_len) && 
                      (ASC_CHINESE_FLAG(ASC_byte_value))) 
           {
               nCh = 1;

               ASC_byte_value = in_ASC_str[++in_ASC_str_position];
               if (!ASC_CHINESE_FLAG (ASC_byte_value)) {
                   //printf ("brea  %x %d\n", ASC_byte_value, in_ASC_str_position);
                   break;
               }

               nCh = 2;

               out_EBC_str_position++;
               out_EBC_str_position++;
               ASC_byte_value=in_ASC_str[++in_ASC_str_position];
           }

           out_EBC_str_position++;
           in_ASC_str_position--;
           
           //printf ("nCh %d %d\n", nCh, out_EBC_str_position);

           if (nCh == 1) {
               in_ASC_str[in_ASC_str_position] = SPACE;
               num_of_space_for_chinese=num_of_space_for_chinese+1; 
           }
           else {
               num_of_space_for_chinese=num_of_space_for_chinese+2; 
           }
       }
       else 
       {
           nCh = 0;
           if (ASC_byte_value== SPACE) {
               if (num_of_space_for_chinese!=0) {
                 num_of_space_for_chinese--;
                 continue;
               }
           }
           out_EBC_str_position++;
           //printf ("nCh %d %d\n", nCh, out_EBC_str_position);
       }                  
   }                     
   
   //printf ("outlen %d\n", out_EBC_str_position);

   if (out_EBC_str_position <= EBC_str_len) {
       return ASC_str_len;
   }

   overlen = out_EBC_str_position - EBC_str_len;
   //printf ("over %d\n", overlen);

   for (in_ASC_str_position = ASC_str_len - 1; 
        in_ASC_str_position >=0 && overlen > 0;
        in_ASC_str_position--) 
   {
       ASC_byte_value = in_ASC_str[in_ASC_str_position];
       if (ASC_byte_value == SPACE) {
           continue;
       }

       in_ASC_str[in_ASC_str_position] = SPACE;
       overlen--;

       if (!ASC_CHINESE_FLAG (ASC_byte_value)) {
           continue;
       }

       in_ASC_str_position--; 
       in_ASC_str[in_ASC_str_position] = SPACE;
       overlen--;
   }

   return ASC_str_len;
}

// unsigned char *in_ASC_str;  输入的ASCII码字符串
// unsigned char *out_EBC_str; 码制转换后输出的EBCDIC码字符串
// int  ASC_str_len;           输入的ASCII码字符串的长度

void IC_ASCtoEBC(unsigned char *in_ASC_str,unsigned char *out_EBC_str, \
                 int ASC_str_len)
{
   int num_of_space_for_chinese=0;                    /*   space  number   */
   int in_ASC_str_position,out_EBC_str_position=0;  
   unsigned char ASC_byte_value;  
   CH_WORD temp;

        /*   转换输入的ASCII字符串   */
   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
                                             in_ASC_str_position++) 
     {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
       /*   转换输入的ASCII汉字符   */
       if (ASC_byte_value== SPACE)   
           if (num_of_space_for_chinese!=0)
           {
               num_of_space_for_chinese--;
               continue;
           }
           out_EBC_str[out_EBC_str_position++]=SB_ASCIItoEBCDIC(ASC_byte_value);
     }                     
     out_EBC_str[out_EBC_str_position]='\0';
}

/* CONVERT  ASCII CODE TO EBCDIC */

/*转换输入的ASCII字符in_ASC_byte_value,返回的为EBCDIC字符 */

// unsigned char in_ASC_byte_value;     input is ascii code
unsigned char SB_ASCIItoEBCDIC(unsigned char in_ASC_byte_value)
{
  const unsigned char ASCIItoEBCDICTable[256]={
        0x00,0x10,0x20,0x30,0x70,0x80,0x90,0x01,
        0x11,0x21,0x31,0x41,0x51,0x71,0xb1,0xe1,
        0x02,0x12,0x22,0x32,0x42,0x52,0x62,0x72,
        0xb2,0x03,0x13,0x23,0x33,0x43,0x53,0x63,
        0x40,0x5a,0x7f,0x7b,0xe0,0x6c,0x50,0x7d,
        0x4d,0x5d,0x5c,0x4e,0x6b,0x60,0x4b,0x61,
        0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,
        0xf8,0xf9,0x7a,0x5e,0x4c,0x7e,0x6e,0x6f,
        0x7c,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,
        0xc8,0xc9,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,
        0xd7,0xd8,0xd9,0xe2,0xe3,0xe4,0xe5,0xe6,
        0xe7,0xe8,0xe9,0xa0,0x5b,0xb0,0x5f,0x6d,
        0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
        0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,
        0x97,0x98,0x99,0xa2,0xa3,0xa4,0xa5,0xa6,
        0xa7,0xa8,0xa9,0xc0,0x6a,0xd0,0xa1,0x73,
        0x4a,0x4f,0xb3,0x04,0x14,0x24,0x34,0x44,
        0x54,0x64,0x74,0xb4,0x05,0x15,0x25,0x35,
        0x45,0x55,0x65,0x75,0xb5,0x06,0x16,0x26,
        0x36,0x46,0x56,0x66,0x76,0xb6,0x07,0x17,
        0x27,0x37,0x47,0x57,0x67,0x77,0xb7,0x08,
        0x18,0x28,0x38,0x48,0x58,0x68,0x78,0xb8,
        0x09,0x19,0x29,0x39,0x49,0x59,0x69,0xb9,
        0x0a,0x1a,0x2a,0x3a,0x8a,0x9a,0xaa,0xba,
        0xca,0xda,0xea,0xfa,0x0b,0x1b,0x2b,0x3b,
        0x8b,0x9b,0xab,0xbb,0xcb,0xdb,0xeb,0xfb,
        0x0c,0x1c,0x2c,0x3c,0x8c,0x9c,0xac,0xbc,
        0xcc,0xdc,0xec,0xfc,0x0d,0x1d,0x2d,0x3d,
        0x8d,0x9d,0xad,0xbd,0xcd,0xdd,0xed,0xfd,
        0x0e,0x1e,0x2e,0x3e,0x8e,0x9e,0xae,0xbe,
        0xce,0xde,0xee,0xfe,0x0f,0x1f,0x2f,0x3f,
        0x8f,0x9f,0xaf,0xbf,0xcf,0xdf,0xef,0xff};

  unsigned char out_EBC_byte_value;
  out_EBC_byte_value=ASCIItoEBCDICTable[in_ASC_byte_value];
  return(out_EBC_byte_value);
}


/*   convert  EBCDIC  code  into  ASCII    code   */

/*转换输入的EBCDIC字符in_EBC_byte_value,返回的为ASCII字符 */
// unsigned char in_EBC_byte_value;

unsigned char SB_EBCDICtoASCII(unsigned char in_EBC_byte_value)
{
  const unsigned char EBCDICtoASCIITable[256]={
        0x00,0x07,0x10,0x19,0x83,0x8c,0x95,0x9e,
        0xa7,0xb0,0xb8,0xc4,0xd0,0xdc,0xe8,0xf4,
        0x01,0x08,0x11,0x1a,0x84,0x8d,0x96,0x9f,
        0xa8,0xb1,0xb9,0xc5,0xd1,0xdd,0xe9,0xf5,
        0x02,0x09,0x12,0x1b,0x85,0x8e,0x97,0xa0,
        0xa9,0xb2,0xba,0xc6,0xd2,0xde,0xea,0xf6,
        0x03,0x0a,0x13,0x1c,0x86,0x8f,0x98,0xa1,
        0xaa,0xb3,0xbb,0xc7,0xd3,0xdf,0xeb,0xf7,
        0x20,0x0b,0x14,0x1d,0x87,0x90,0x99,0xa2,
        0xab,0xb4,0x80,0x2e,0x3c,0x28,0x2b,0x81,
        0x26,0x0c,0x15,0x1e,0x88,0x91,0x9a,0xa3,
        0xac,0xb5,0x21,0x5c,0x2a,0x29,0x3b,0x5e,
        0x2d,0x2f,0x16,0x1f,0x89,0x92,0x9b,0xa4,
        0xad,0xb6,0x7c,0x2c,0x25,0x5f,0x3e,0x3f,
        0x04,0x0d,0x17,0x7f,0x8a,0x93,0x9c,0xa5,
        0xae,0x60,0x3a,0x23,0x40,0x27,0x3d,0x22,
        0x05,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
        0x68,0x69,0xbc,0xc8,0xd4,0xe0,0xec,0xf8,
        0x06,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,
        0x71,0x72,0xbd,0xc9,0xd5,0xe1,0xed,0xf9,
        0x5b,0x7e,0x73,0x74,0x75,0x76,0x77,0x78,
        0x79,0x7a,0xbe,0xca,0xd6,0xe2,0xee,0xfa,
        0x5d,0x0e,0x18,0x82,0x8b,0x94,0x9d,0xa6,
        0xaf,0xb7,0xbf,0xcb,0xd7,0xe3,0xef,0xfb,
        0x7b,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
        0x48,0x49,0xc0,0xcc,0xd8,0xe4,0xf0,0xfc,
        0x7d,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,
        0x51,0x52,0xc1,0xcd,0xd9,0xe5,0xf1,0xfd,
        0x24,0x0f,0x53,0x54,0x55,0x56,0x57,0x58,
        0x59,0x5a,0xc2,0xce,0xda,0xe6,0xf2,0xfe,
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
        0x38,0x39,0xc3,0xcf,0xdb,0xe7,0xf3,0xff};

  unsigned char out_ASC_byte_value;
  out_ASC_byte_value=EBCDICtoASCIITable[in_EBC_byte_value];
  return(out_ASC_byte_value);
}



const unsigned char    IBMtoGBTable[9][188] = {
        /*第一区的玖拾肆个汉字*/
    0x40,0x40,0x43,0x44,0x43,0x41,0x43,0x45,0x45,0x45,
//    0x00,0x00,0x43,0x44,0x43,0x41,0x43,0x45,0x45,0x45,
    0x45,0x46,0x44,0x60,0x44,0x5B,0x44,0x5d,0x44,0x4A,
    0x43,0xA1,0x44,0x7C,0x44,0x7F,0x44,0x61,0x44,0x71,
    0x44,0x62,0x44,0x72,0x44,0x63,0x44,0x73,0x44,0x64,
    0x44,0x74,0x44,0x65,0x44,0x75,0x43,0x42,0x43,0x43,
    0x44,0x42,0x44,0x43,0x45,0x5B,0x45,0x5C,0x44,0x66,
    0x44,0x76,0x44,0x4B,0x44,0x7A,0x44,0x7B,0x45,0x62,
    0x45,0x63,0x45,0x64,0x45,0x65,0x45,0x66,0x45,0x67,
    0x45,0x68,0x45,0x69,0x45,0x6A,0x45,0x6B,0x45,0x6C,
    0x45,0x6D,0x45,0x6E,0x45,0x6F,0x45,0x70,0x45,0x71,
    0x45,0x72,0x45,0x73,0x45,0x74,0x45,0x75,0x45,0x76,
    0x45,0x77,0x44,0x4C,0x45,0x79,0x45,0x7A,0x44,0x67,
    0x44,0x77,0x44,0x4D,0x44,0x78,0x44,0x68,0x44,0x69,
    0x44,0x79,0x44,0xED,0x44,0xEE,0x44,0xEF,0x44,0x4E,
    0x42,0xE0,0x45,0x88,0x43,0x4A,0x42,0x4A,0x45,0x8B,
    0x44,0x6A,0x44,0x6E,0x44,0xE5,0x44,0xE6,0x44,0xE0,
    0x44,0xE1,0x44,0xE4,0x44,0xE7,0x44,0xE8,0x44,0xE9,
    0x44,0xEA,0x44,0xE2,0x44,0xE3,0x44,0x6B,0x44,0xF0,
    0x44,0xF1,0x44,0xF2,0x44,0xF3,0x44,0x7D,
        /*第二区的柒拾贰个汉字*/
                /*     缺处补空       */
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x45,0xB1,0x45,0xB2,0x45,0xB3,0x45,0xB4,
    0x45,0xB5,0x45,0xB6,0x45,0xB7,0x45,0xB8,0x45,0xB9,
    0x45,0xBA,0x45,0xBB,0x45,0xAC,0x45,0xBD,0x45,0xBE,
    0x45,0xBF,0x45,0xC0,0x45,0xC1,0x45,0xC2,0x45,0xC3,
    0x45,0xC4,0x45,0xC5,0x45,0xC6,0x45,0xC7,0x45,0xC8,
    0x45,0xC9,0x45,0xCA,0x45,0xCB,0x45,0xCC,0x45,0xCD,
    0x45,0xCE,0x45,0xCF,0x45,0xD0,0x45,0xD1,0x45,0xD2,
    0x45,0xD3,0x45,0xD4,0x45,0xD5,0x45,0xD6,0x45,0xD7,
    0x45,0xD8,0x45,0xE1,0x45,0xE2,0x45,0xE3,0x45,0xE4,
    0x45,0xE5,0x45,0xE6,0x45,0xE7,0x45,0xE8,0x45,0xE9,
    0x45,0xEA,0x40,0x40,0x40,0x40,0x45,0xF1,0x45,0xF2,
    0x45,0xF3,0x45,0xF4,0x45,0xF5,0x45,0xF6,0x45,0xF7,
    0x45,0xF8,0x45,0xF9,0x45,0xFA,0x40,0x40,0x40,0x40,
    0x41,0xF1,0x41,0xF2,0x41,0xF3,0x41,0xF4,0x41,0xF5,
    0x41,0xF6,0x41,0xF7,0x41,0xF8,0x41,0xF9,0x41,0xFA,
    0x41,0xFB,0x41,0xFC,0x40,0x40,0x40,0x40,
        /*第三区的玖拾肆个汉字*/
    0x42,0x5A,0x42,0x7F,0x42,0x7B,0x42,0x5B,0x42,0x6C,
    0x42,0x50,0x44,0x50,0x42,0x4D,0x42,0x5D,0x42,0x5C,
    0x42,0x4E,0x42,0x6B,0x42,0x60,0x42,0x4B,0x42,0x61,
    0x42,0xF0,0x42,0xF1,0x42,0xF2,0x42,0xF3,0x42,0xF4,
    0x42,0xF5,0x42,0xF6,0x42,0xF7,0x42,0xF8,0x42,0xF9,
    0x42,0x7A,0x42,0x5E,0x42,0x4C,0x42,0x7E,0x42,0x6E,
    0x42,0x6F,0x42,0x7C,0x42,0xC1,0x42,0xC2,0x42,0xC3,
    0x42,0xC4,0x42,0xC5,0x42,0xC6,0x42,0xC7,0x42,0xC8,
    0x42,0xC9,0x42,0xD1,0x42,0xD2,0x42,0xD3,0x42,0xD4,
    0x42,0xD5,0x42,0xD6,0x42,0xD7,0x42,0xD8,0x42,0xD9,
    0x42,0xE2,0x42,0xE3,0x42,0xE4,0x42,0xE5,0x42,0xE6,
    0x42,0xE7,0x42,0xE8,0x42,0xE9,0x44,0x44,0x43,0xE0,
    0x44,0x45,0x44,0x70,0x42,0x6D,0x42,0x79,0x42,0x81,
    0x42,0x82,0x42,0x83,0x42,0x84,0x42,0x85,0x42,0x86,
    0x42,0x87,0x42,0x88,0x42,0x89,0x42,0x91,0x42,0x92,
    0x42,0x93,0x42,0x94,0x42,0x95,0x42,0x96,0x42,0x97,
    0x42,0x98,0x42,0x99,0x42,0xA2,0x42,0xA3,0x42,0xA4,
    0x42,0xA5,0x42,0xA6,0x42,0xA7,0x42,0xA8,0x42,0xA9,
    0x42,0xC0,0x42,0x4F,0x42,0xD0,0x42,0xA1,
        /*第四区的捌拾叁个汉字*/
                /*     缺处补空       */
    0x44,0x47,0x44,0x81,0x44,0x48,0x44,0x82,0x44,0x49,
    0x44,0x83,0x44,0x51,0x44,0x84,0x44,0x52,0x44,0x85,
    0x44,0x86,0x44,0xC0,0x44,0x87,0x44,0xC1,0x44,0x88,
    0x44,0xC2,0x44,0x89,0x44,0xC3,0x44,0x8A,0x44,0xC4,
    0x44,0x8C,0x44,0xC5,0x44,0x8D,0x44,0xC6,0x44,0x8E,
    0x44,0xC7,0x44,0x8F,0x44,0xC8,0x44,0x90,0x44,0xC9,
    0x44,0x91,0x44,0xCA,0x44,0x92,0x44,0xCB,0x44,0x56,
    0x44,0x93,0x44,0xCC,0x44,0x94,0x44,0xCD,0x44,0x95,
    0x44,0xCE,0x44,0x96,0x44,0x97,0x44,0x98,0x44,0x99,
    0x44,0x9A,0x44,0x9D,0x44,0xCF,0x44,0xD5,0x44,0x9E,
    0x44,0xD0,0x44,0xD6,0x44,0x9F,0x44,0xD1,0x44,0xD7,
    0x44,0xA2,0x44,0xD2,0x44,0xD8,0x44,0xA3,0x44,0xD3,
    0x44,0xD9,0x44,0xA4,0x44,0xA5,0x44,0xA6,0x44,0xA7,
    0x44,0xA8,0x44,0x53,0x44,0xA9,0x44,0x54,0x44,0xAA,
    0x44,0x55,0x44,0xAC,0x44,0xAD,0x44,0xAE,0x44,0xAF,
    0x44,0xBA,0x44,0xBB,0x44,0x57,0x44,0xBC,0x44,0xDA,
    0x44,0xDB,0x44,0x46,0x44,0xBD,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
        /*第伍区的捌拾陆个汉字*/
                /*     缺处补空       */
    0x43,0x47,0x43,0x81,0x43,0x48,0x43,0x82,0x43,0x49,
    0x43,0x83,0x43,0x51,0x43,0x84,0x43,0x52,0x43,0x85,
    0x43,0x86,0x43,0xC0,0x43,0x87,0x43,0xC1,0x43,0x88,
    0x43,0xC2,0x43,0x89,0x43,0xC3,0x43,0x8A,0x43,0xC4,
    0x43,0x8C,0x43,0xC5,0x43,0x8D,0x43,0xC6,0x43,0x8E,
    0x43,0xC7,0x43,0x8F,0x43,0xC8,0x43,0x90,0x43,0xC9,
    0x43,0x91,0x43,0xCA,0x43,0x92,0x43,0xCB,0x43,0x56,
    0x43,0x93,0x43,0xCC,0x43,0x94,0x43,0xCD,0x43,0x95,
    0x43,0xCE,0x43,0x96,0x43,0x97,0x43,0x98,0x43,0x99,
    0x43,0x9A,0x43,0x9D,0x43,0xCF,0x43,0xD5,0x43,0x9E,
    0x43,0xD0,0x43,0xD6,0x43,0x9F,0x43,0xD1,0x43,0xD7,
    0x43,0xA2,0x43,0xD2,0x43,0xD8,0x43,0xA3,0x43,0xD3,
    0x43,0xD9,0x43,0xA4,0x43,0xA5,0x43,0xA6,0x43,0xA7,
    0x43,0xA8,0x43,0x53,0x43,0xA9,0x43,0x54,0x43,0xAA,
    0x43,0x55,0x43,0xAC,0x43,0xAD,0x43,0xAE,0x43,0xAF,
    0x43,0xBA,0x43,0xBB,0x43,0x57,0x43,0xBC,0x43,0xDA,
    0x43,0xDB,0x43,0x46,0x43,0xBD,0x43,0xD4,0x43,0x59,
    0x43,0x5A,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
        /*第六区的肆拾捌个汉字*/
                /*     缺处补空       */
    0x41,0x61,0x41,0x62,0x41,0x63,0x41,0x64,0x41,0x65,
    0x41,0x66,0x41,0x67,0x41,0x68,0x41,0x69,0x41,0x6A,
    0x41,0x6B,0x41,0x6C,0x41,0x6D,0x41,0x6E,0x41,0x6F,
    0x41,0x70,0x41,0x71,0x41,0x72,0x41,0x73,0x41,0x74,
    0x41,0x75,0x41,0x76,0x41,0x77,0x41,0x78,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x41,0x41,0x41,0x42,0x41,0x43,
    0x41,0x44,0x41,0x45,0x41,0x46,0x41,0x47,0x41,0x48,
    0x41,0x49,0x41,0x4A,0x41,0x4B,0x41,0x4C,0x41,0x4D,
    0x41,0x4E,0x41,0x4F,0x41,0x50,0x41,0x51,0x41,0x52,
    0x41,0x53,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,
    0x41,0x58,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
        /*第七区的陆拾陆个汉字*/
                /*     缺处补空       */
    0x41,0xC0,0x41,0xC1,0x41,0xC2,0x41,0xC3,0x41,0xC4,
    0x41,0xC5,0x41,0xC6,0x41,0xC7,0x41,0xC8,0x41,0xC9,
    0x41,0xCA,0x41,0xCB,0x41,0xCC,0x41,0xCD,0x41,0xCE,
    0x41,0xCF,0x41,0xD0,0x41,0xD1,0x41,0xD2,0x41,0xD3,
    0x41,0xD4,0x41,0xD5,0x41,0xD6,0x41,0xD7,0x41,0xD8,
    0x41,0xD9,0x41,0xDA,0x41,0xDB,0x41,0xDC,0x41,0xDD,
    0x41,0xDE,0x41,0xDF,0x41,0xE0,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x41,0x80,0x41,0x81,
    0x41,0x82,0x41,0x83,0x41,0x84,0x41,0x85,0x41,0x86,
    0x41,0x87,0x41,0x88,0x41,0x89,0x41,0x8A,0x41,0x8B,
    0x41,0x8C,0x41,0x8D,0x41,0x8E,0x41,0x8F,0x41,0x90,
    0x41,0x91,0x41,0x92,0x41,0x93,0x41,0x94,0x41,0x95,
    0x41,0x96,0x41,0x97,0x41,0x98,0x41,0x99,0x41,0x9A,
    0x41,0x9B,0x41,0x9C,0x41,0x9D,0x41,0x8E,0x41,0x9F,
    0x41,0xA0,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
        /*第八区的陆拾叁个汉字*/
                /*     缺处补空       */
    0x46,0x41,0x46,0x42,0x46,0x43,0x46,0x44,0x46,0x45,
    0x46,0x46,0x46,0x47,0x46,0x48,0x46,0x49,0x46,0x4A,
    0x46,0x4B,0x46,0x4C,0x46,0x4D,0x46,0x4E,0x46,0x4F,
    0x46,0x50,0x46,0x51,0x46,0x52,0x46,0x53,0x46,0x54,
    0x46,0x55,0x46,0x56,0x46,0x57,0x46,0x58,0x46,0x59,
    0x46,0x5A,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x46,0x65,0x46,0x66,0x46,0x67,0x46,0x68,
    0x46,0x69,0x46,0x6A,0x46,0x6B,0x46,0x6C,0x46,0x6D,
    0x46,0x6E,0x46,0x6F,0x46,0x70,0x46,0x71,0x46,0x72,
    0x46,0x73,0x46,0x74,0x46,0x75,0x46,0x76,0x46,0x77,
    0x46,0x78,0x46,0x79,0x46,0x7A,0x46,0x7B,0x46,0x7C,
    0x46,0x7D,0x46,0x7E,0x46,0x7F,0x46,0x80,0x46,0x81,
    0x46,0x82,0x46,0x83,0x46,0x84,0x46,0x85,0x46,0x86,
    0x46,0x87,0x46,0x88,0x46,0x89,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
        /*第九区的柒拾陆个汉字*/
                /*     缺处补空       */
    0x40,0x40,0x40,0x40,0x40,0x40,0x46,0xA4,0x46,0xA5,
    0x46,0xA6,0x46,0xA7,0x46,0xA8,0x46,0xA9,0x46,0xAA,
    0x46,0xAB,0x46,0xAC,0x46,0xAD,0x46,0xAE,0x46,0xAF,
    0x46,0xB0,0x46,0xB1,0x46,0xB2,0x46,0xB3,0x46,0xB4,
    0x46,0xB5,0x46,0xB6,0x46,0xB7,0x46,0xB8,0x46,0xB9,
    0x46,0xBA,0x46,0xBB,0x46,0xBC,0x46,0xBD,0x46,0xBE,
    0x46,0xBF,0x46,0xC0,0x46,0xC1,0x46,0xC2,0x46,0xC3,
    0x46,0xC4,0x46,0xC5,0x46,0xC6,0x46,0xC7,0x46,0xC8,
    0x46,0xC9,0x46,0xCA,0x46,0xCB,0x46,0xCC,0x46,0xCD,
    0x46,0xCE,0x46,0xCF,0x46,0xD0,0x46,0xD1,0x46,0xD2,
    0x46,0xD3,0x46,0xD4,0x46,0xD5,0x46,0xD6,0x46,0xD7,
    0x46,0xD8,0x46,0xD9,0x46,0xDA,0x46,0xDB,0x46,0xDC,
    0x46,0xDD,0x46,0xDE,0x46,0xDF,0x46,0xE0,0x46,0xE1,
    0x46,0xE2,0x46,0xE3,0x46,0xE4,0x46,0xE5,0x46,0xE6,
    0x46,0xE7,0x46,0xE8,0x46,0xE9,0x46,0xEA,0x46,0xEB,
    0x46,0xEC,0x46,0xED,0x46,0xEE,0x46,0xEF,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40
};

/*   unix  chinese partition 01-09 is changed as/400 chinese   */
        /*   转换输入的ASCII汉字符,区位从1到9   */

CH_WORD DB_ASCIItoEBCDIC_01(CH_WORD ASC_WORD)
{
  unsigned char m_index,s_index;
  CH_WORD temp;
  m_index=ASC_WORD.first_byte-0xa1;
  s_index=ASC_WORD.second_byte-0xa1;
  temp.first_byte=IBMtoGBTable[m_index][s_index*2];
  temp.second_byte=IBMtoGBTable[m_index][s_index*2+1];
  return(temp);
}

/*   as/400  chinese partition 01-09 is changed unix chinese   */

        /*   转换输入的EBCDIC汉字双字节字符,区位从1到9   */
CH_WORD DB_EBCDICtoASCII_01( CH_WORD EBC_WORD)
{
  unsigned char index1,index2;
  CH_WORD temp;
  for(index1=0;index1<9;index1++)
     for(index2=0;index2<94;index2++)
     if(IBMtoGBTable[index1][index2*2]==EBC_WORD.first_byte &&
        IBMtoGBTable[index1][index2*2+1]==EBC_WORD.second_byte )
     {
          temp.first_byte=index1+0xa1;
          temp.second_byte=index2+0xa1;
      return(temp);
     }
  return(temp);
}


/*    TURN CHINESE CODE FROM UNIX TO AS/400  */

        /*   转换输入的ASCII汉字符,区位从16到87   */
CH_WORD DB_ASCIItoEBCDIC_02(CH_WORD ASC_WORD)
{
  unsigned char firstbyte;
  CH_WORD temp;

     firstbyte=ASC_WORD.first_byte - ASC_PARTITION_AREA_FLAG_CHAR;

     if (firstbyte%2!=0)
         firstbyte++;
     firstbyte/=2;
     temp.first_byte=firstbyte+0x48;

     if (ASC_WORD.first_byte%2!=0)  
     {
         if (ASC_WORD.second_byte<0xe0)
              temp.second_byte=ASC_WORD.second_byte-0x60;
         else
              temp.second_byte=ASC_WORD.second_byte-0x5f;
     }
     else 
         temp.second_byte=ASC_WORD.second_byte-1;

     return(temp);
}

/* TURN CHINESE CODE FROM AS/400  TO  UNIX  */

        /*   转换输入的EBCDIC汉字双字节字符,区位从16到87   */
CH_WORD DB_EBCDICtoASCII_02( CH_WORD EBC_WORD)
{
  unsigned char firstbyte;
  CH_WORD temp;

      if (EBC_WORD.first_byte!=0x48) 
      {
          firstbyte=EBC_WORD.first_byte-0x48;
          firstbyte*=2;
          if (EBC_WORD.second_byte<=0x9f)
               firstbyte--;
          temp.first_byte=firstbyte+0xb0;
          if (EBC_WORD.second_byte<=0x9f)  
          {
              if (EBC_WORD.second_byte<0x81)
                   temp.second_byte=EBC_WORD.second_byte+0x60;
              else
                   temp.second_byte=EBC_WORD.second_byte+0x5f;
          }
          else 
              temp.second_byte=EBC_WORD.second_byte+1;
      }
      else 
      {
          temp.first_byte=0xb0;
          temp.second_byte=EBC_WORD.second_byte+1;
      };

      return(temp);
}


int Turn_ASCtoEBC_Str2(unsigned char *in_ASC_str, int ASC_str_len, 
                     int EBC_str_len)
{
   int nCh = 0;
   int num_of_space_for_chinese=0;   
   int in_ASC_str_position;
   int out_EBC_str_position=0;  
   int overlen = 0;
   unsigned char ASC_byte_value;  

   if (ASC_str_len > EBC_str_len) {
       ASC_str_len = EBC_str_len;
       in_ASC_str[ASC_str_len] = '\0';
   }

   for (in_ASC_str_position=0;in_ASC_str_position<ASC_str_len;
        in_ASC_str_position++) 
   {
       ASC_byte_value=in_ASC_str[in_ASC_str_position];
   
       if (  ASC_CHINESE_FLAG(ASC_byte_value) )
       {
           nCh = 0;
           out_EBC_str_position++;
           while ((in_ASC_str_position<ASC_str_len) && 
                      (ASC_CHINESE_FLAG(ASC_byte_value))) 
           {
               nCh = 1;

               ASC_byte_value = in_ASC_str[++in_ASC_str_position];
               if (!ASC_CHINESE_FLAG (ASC_byte_value)) {
                   //printf ("brea  %x %d\n", ASC_byte_value, in_ASC_str_position);
                   break;
               }

               nCh = 2;

               out_EBC_str_position++;
               out_EBC_str_position++;
               ASC_byte_value=in_ASC_str[++in_ASC_str_position];
           }

           in_ASC_str_position--;
           
           //printf ("nCh %d %d\n", nCh, out_EBC_str_position);

           if (nCh == 1) {
               in_ASC_str[in_ASC_str_position] = SPACE;
               num_of_space_for_chinese=num_of_space_for_chinese+1; 
           }
           else {
               out_EBC_str_position++;
               num_of_space_for_chinese=num_of_space_for_chinese+2; 
           }
       }
       else 
       {
           nCh = 0;
           /*if (ASC_byte_value== SPACE) {
               if (num_of_space_for_chinese!=0) {
                 num_of_space_for_chinese--;
                 continue;
               }
           }*/
           out_EBC_str_position++;
           //printf ("nCh %d %d\n", nCh, out_EBC_str_position);
       }                  
   }                     
   
   //printf ("outlen %d\n", out_EBC_str_position);

   if (out_EBC_str_position <= EBC_str_len) {
       return out_EBC_str_position;
   }

   

   return out_EBC_str_position;
}


int dealASCtoEBC(char * insrc ,char * outsrc )
{
   int sum1;
   unsigned char f1,s1,f2,s2;

   f1 = insrc[0];
   s1 = insrc[1];

   //printf("%02x %02x \n" ,f1,s1);
   sum1 = (f1-0x81) * 0xbe + s1 - 0x3f;
   if (s1>=0x7f)
      sum1--;

   //printf("sum1 = %d \n" ,sum1);
   f2 = sum1 / 0xbe +0x81;
   s2 = sum1 % 0xbd;


   if(f2>0x81)
      s2+=sum1/0xbd;

   s2 += 0x40;

   if(s2 >= 0x80)
     s2++;

   if(s2 >= 0xfe)

   {

      s2 = s2 - 0xfe + 0x41;

      f2++;

   }

   outsrc[0] = f2;
   outsrc[1] = s2;
   return 0;
}



int dealASCtoEBC4(char * insrc ,char * outsrc )
{
   int sum1;
   unsigned char f1,s1,f2,s2;

   f1 = insrc[0];
   s1 = insrc[1];
    //printf("22 %02x %02x \n" ,f1,s1);
   if ( !((f1 == 0xFD && s1 > 0x9b) || f1 > 0xFD ))
   {
         //printf("2222 %02x %02x \n" ,f1,s1);
         sum1 = (f1-0xAA) * 0x60 + s1 - 0x3f;
         if (s1>0x7f)
             sum1--;
         //printf("sum1 = %d \n" ,sum1);
         if( sum1 <= 125  )
         {
            f2 = sum1 / 0x7D + 0xA1;
            s2 = sum1 % 0x7D;
            //printf("f2 = %02x \n" , sum1 / 0x7D);

            s2 += 0x81;
            //printf("s2 = %02x \n" ,s2);
            if(sum1 == 125)

            {

               s2 =  0x41;
               
            }
         }
         else
         {
            sum1 = sum1 - 124;

            f2 = sum1 / 0xBC + 0xA2;
            s2 = sum1 % 0xBC;
            //printf("f2 = %d \n" , sum1 / 0xBC);

            s2 += 0x40;
            //printf("s2 = %02x \n" ,s2);
            if(s2 >= 0x80)
              s2 ++;

            if(s2 >= 0xfe)
            {

               s2 = s2 - 0xfe + 0x42;

               f2++;

            }
         }
   }
   else
   {
      if (f1 == 0xfd )
      {
         sum1 =  s1 - 0x9b;
      }
      else 
      {
         //printf("%02x %02x \n" ,f1,s1);
         sum1 =  s1 - 0x3f + 5;
         if (s1>=0x7f)
            sum1--;
      }
      //printf("sum11 = %d \n" ,sum1);
      f2 = 0xce;
      s2 = sum1 ;


      s2 += 0x40;

      if(s2 >= 0x80)
        s2++;

      if(s2 >= 0xfe)

      {

         s2 = s2 - 0xfe + 0x41;

         f2++;

      }
   }

   outsrc[0] = f2;
   outsrc[1] = s2;
   return 0;
}


int dealEBCtoASC(char * insrc ,char * outsrc )
{
   int sum1;
   unsigned char f1,s1,f2,s2;

   f1 = insrc[0];
   s1 = insrc[1];

   sum1 = (f1-0x81) * 0xbc + s1 - 0x40;
   if (s1>0x80)
       sum1--;

   //printf("sum1 = %d \n" ,sum1);
   f2 = sum1 / 0xbe +0x81;
   s2 = sum1 % 0xbe;


   s2 += 0x3f;

   if(s2 >= 0x7f)
     s2++;

   if(s2 >= 0xff)

   {

      s2 = s2 - 0xff + 0x40;

      f2++;

   }

   outsrc[0] = f2;
   outsrc[1] = s2;
   return 0;
}


int dealEBCtoASC4(char * insrc ,char * outsrc )
{
   int sum1;
   unsigned char f1,s1,f2,s2;

   f1 = insrc[0];
   s1 = insrc[1];

   if ( !(f1 == 0xce && s1 >= 0x41))
   {
         //printf("%02x %02x \n" ,f1,s1);
         
         if( f1 == 0xA1  )
         {
            sum1 = s1 - 0x81 ;
         }
         else
         {
            sum1 = (f1 - 0xa2 ) * 0xBc + 124 + s1 - 0x40;  
            if (s1>0x80)
                sum1--;
         }

      //printf("sum1 = %d \n" ,sum1);
      f2 = sum1 / 0x60 +0xaa;
      s2 = sum1 % 0x60;


      s2 += 0x3f;

      if(s2 >= 0x7f)
        s2++;

      if(s2 >= 0xff)

      {

         s2 = s2 - 0xff + 0x40;

         f2++;

      }

   }
   else
   {
      //printf("%02x %02x \n" ,f1,s1);
      if(s1 <= 0x45 )
      { 
          s2 = 0x9b + s1 - 0x40;
          f2 = 0xfd;
      }
      else
      { 
          s2 =  s1  - 0x45 +  0x3f;
          f2 = 0xfe;
         if (s1>0x80)
            s2--;
         if(s2 >= 0x7f)
            s2++;
      }

   }

   outsrc[0] = f2;
   outsrc[1] = s2;
   return 0;
}

#if 0
int main ()
{
    unsigned char str[100];
    unsigned char ostr[100];
    int asc_len;
    int ebc_len;

    //strcpy (str, "测试g汉字  ");
    memset(ostr,0x00,sizeof(ostr));
    memset(str,0x00,sizeof(str));
    strcpy (str , "冚");
    //str[0] = 0xa6;
    //str[1] = 0x73;
    asc_len = strlen (str);
    ebc_len = asc_len;
   
    //printf ("%d [%s]\n", strlen (str), str);

   

    ASCtoEBC (str,ostr, 2 );
    //dealASCtoEBC(str,ostr );
    //printf ("[%02x][%02x][%02x][%02x]\n",ostr[0],ostr[1],ostr[2],ostr[3]);
    memset(str,0x00,sizeof(str));
    EBCtoASC( ostr,str,4);
    //dealEBCtoASC(ostr,str );
    //printf ("[%02x][%02x][%02x][%02x][%s]\n",str[0],str[1],str[2],str[3],str);
    return 0;
}
#endif
