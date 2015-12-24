/*
**  file: pkg_tomsgbase.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "pkg/pkg_tomsgbase.h"
#include "log.h"

/***************************/
#include "tempadd.h"

#include "getAlignPos.h"
extern int nestedLevel;
extern int posInStruct;
extern int structStartMoveLen;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];

extern FILE *fpPkgLog;

/*
**  函数功能: 设置BitMap的特定位,将其设为1
**
**  函数名称: PKG_SetBitMap
**
**  输入参数: unsigned char * eoupczBitMap    //BitMap
*             *int eiNum
**
**  输出参数: unsigned char * eoupczBitMap
**
**  返回值:   0--成功, 其他失败
*/
int PKG_SetBitMap( unsigned char * eoupczBitMap, int eiNum )
{
   int iByteNum, iBitNum, iBitMoved;
   char cTheBitChar;

   if( eiNum < 1 || eiNum > PKG_BITMAP_MAX )
      return PKG_BITMAP_OVERFLOW;

   iByteNum = ( eiNum - 1 ) / 8;
   iBitNum  = ( eiNum - 1 ) % 8;
   iBitMoved = 7 - iBitNum;

   cTheBitChar = eoupczBitMap[ iByteNum ];
   cTheBitChar = ( 0x01 << iBitMoved ) | cTheBitChar;
   eoupczBitMap[ iByteNum ] = cTheBitChar;

   return PKG_SUCC;
}



/*
**  函数功能: 根据数据类型, 把值写入Buf
**
**  函数名称: PKG_WriteValToBuf
**
**  输入参数: int    eiDataType           // 数据类型
**            char * epczVal              
**            int    eiValLen
**            short  eshVal
**            int    eiVal
**            long   elVal
**            double edVal
**            float  efVal
**
**  输出参数: char * opcBuf               // 输出报文(指针为该分域头) 
**            int  * opiValContentLen     // 指针从报文域头的总偏移量
**
**  返回值:   int 0 成功, 其他失败
**
*/

int  PKG_WriteValToBuf( char * opcBuf, int eiDataType,
                        char * epczVal, int eiValLen, short eshVal, int eiVal,
                        long elVal, double edVal, float efVal,
                        int * opiValContentLen )
{
   assert( opcBuf != NULL );

   switch( eiDataType )
   {
      case UDT_STRING:
         assert( epczVal != NULL );
         memcpy( opcBuf, epczVal, (size_t)eiValLen );
         *opiValContentLen = eiValLen; 
         break;
      case UDT_BIN:
         assert( epczVal != NULL );
         memcpy( opcBuf, epczVal, (size_t)eiValLen );
         *opiValContentLen = eiValLen; 
         break;
      case UDT_SHORT:
         memcpy( opcBuf, (char *)&eshVal, sizeof(short) );
         *opiValContentLen = sizeof(short);
         break;
      case UDT_INT:
         memcpy( opcBuf, (char *)&eiVal, sizeof(int) );
         *opiValContentLen = sizeof(int);
         break;
      case UDT_LONG:
         memcpy( opcBuf, (char *)&elVal, sizeof(long) );
         *opiValContentLen = sizeof(long);
         break;
      case UDT_DOUBLE:
         memcpy( opcBuf, (char *)&edVal, sizeof(double) );
         *opiValContentLen = sizeof(double);
         break;
      case UDT_FLOAT:
         memcpy( opcBuf, (char *)&efVal, sizeof(float) );
         *opiValContentLen = sizeof(float);
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The Input 'eiDataType:[%d]' (in PKG_WriteValToBuf) Has Error[%d]\n",
                      eiDataType, PKG_FLDTYPE_ERR)); 
         return PKG_FLDTYPE_ERR;
   } 

   return PKG_SUCC;
} 


/*
**  函数功能: 将分域信息写入输出报文
**
**  函数名:  PKG_WriteSepFldContentToMsg
**
**  输入参数: int  * eopiMoveLen          // 指针从报文头的总偏移量
**            FldCfgValNode estFldCfgValNode // 分域配置信息链表
**            char * epczVal
**            int    eiValLen
**            short  eshVal
**            int    eiVal
**            long   elVal
**            double edVal
**            float  efVal
**
**  输出参数: char * opcMsgPos            // 输出报文(指针为该分域头) 
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int * opiSepMoveLen         // 指针从报文域头的总偏移量
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_WriteSepFldContentToMsg( char * opcMsgPos, int * eopiMoveLen,
                                  FldCfgValNode estFldCfgValNode,
                                  char * epczVal, int eiValLen, short eshVal, 
                                  int eiVal, long elVal, double edVal, 
                                  float efVal, int * opiSepMoveLen ) 
{
   int iRet, iTmp, iValContentLen, iSepSymbolLen;
   int iStrMoveLen = 0, iPtrMoveLen = 0;
   int iLenFldLen, iFldLen;
   char aczLenFldDef[PKG_DEF_LEN+1], aczFldDef[PKG_DEF_LEN+1]; 
   char aczSepSymbol[PKG_SEP_SYMBOL_LEN+1];
   char aczFldActLen[20];
   short shValContentLen;
   long lValContentLen;
   float fValContentLen;
   double dValContentLen;

   assert( opcMsgPos != NULL );

   memset( aczLenFldDef, 0x00, PKG_DEF_LEN+1 );
   memset( aczFldDef, 0x00, PKG_DEF_LEN+1 );
   memset( aczSepSymbol, 0x00, PKG_SEP_SYMBOL_LEN+1 );
   memset( aczFldActLen, 0x00, 20 );


   if( strlen( estFldCfgValNode.aczLenFldDef ) != 0 )
   {
      iRet = PKG_XToA( estFldCfgValNode.aczLenFldDef, aczLenFldDef );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_WriteSepFldContentToMsg) Has Error[%d]! The Inputed is LenFldDef:[%s]! Its Seq:[%d], SepSeq:[%d]\n",
                      iRet, 
                     estFldCfgValNode.aczLenFldDef, estFldCfgValNode.iFldSeq,
                     estFldCfgValNode.iSepFldSeq));
         return iRet;
      }
   }

   if( strlen( estFldCfgValNode.aczFldDef ) != 0 )
   {
      iRet = PKG_XToA( estFldCfgValNode.aczFldDef, aczFldDef );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_WriteSepFldContentToMsg) Has Error[%d]! The Inputed is FldDef:[%s]! Its Seq:[%d], SepSeq:[%d]\n",
                      iRet,
                     estFldCfgValNode.aczFldDef, estFldCfgValNode.iFldSeq,
                     estFldCfgValNode.iSepFldSeq));
         return iRet;
      }
   }

   if( strlen( estFldCfgValNode.aczSepSymbol ) != 0 )
   {
      iRet = PKG_XToA( estFldCfgValNode.aczSepSymbol, aczSepSymbol );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_WriteSepFldContentToMsg) Has Error[%d]! The Inputed is SepSymbol:[%s]! Its Seq:[%d], SepSeq:[%d]\n",
                      iRet,
                     estFldCfgValNode.aczSepSymbol, estFldCfgValNode.iFldSeq,
                     estFldCfgValNode.iSepFldSeq));
         return iRet;
      }
      iSepSymbolLen = strlen( aczSepSymbol );
   }

   switch( estFldCfgValNode.iSepFldFlag )
   {
      case PKG_BITMAP_FLD: /* 暂时不处理 */
         memset( opcMsgPos, 0x20, (size_t)estFldCfgValNode.iFldLen );
         *opiSepMoveLen = estFldCfgValNode.iFldLen;
         break;
      case PKG_FIXLEN_FLD:   
         /* 如果输出的报文为结构体,计算在内容域之前的指针偏移量 */
         if( iToMsgIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen,
                                     estFldCfgValNode.iFldType,
                                     &iPtrMoveLen );
         }


         if( estFldCfgValNode.iFldType == UDT_STRING || estFldCfgValNode.iFldType == UDT_BIN )
         {
            memset( opcMsgPos, aczFldDef[0], 
                    (size_t)estFldCfgValNode.iFldLen );

            /**** 底层控制输入字符串的长度超过域长度的情况 *****/
            if( estFldCfgValNode.iFldLen < eiValLen )
               eiValLen = estFldCfgValNode.iFldLen;

            if( estFldCfgValNode.aczFldAlign[0] == 'R' ||
                estFldCfgValNode.aczFldAlign[0] == 'r' )
            {
                 opcMsgPos += estFldCfgValNode.iFldLen-eiValLen; 
            }
         }


         iRet = PKG_WriteValToBuf( opcMsgPos+iPtrMoveLen,
                                   estFldCfgValNode.iFldType,
                                   epczVal, eiValLen, eshVal, eiVal, elVal, 
                                   edVal, efVal, &iValContentLen );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_WriteValToBuf (in PKG_WriteSepFldContentToMsg) Has Error[%d] When Writing Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n", 
                         iRet, 
                        estFldCfgValNode.iMsgAppId, 
                        estFldCfgValNode.iFldSeq, 
                        estFldCfgValNode.iSepFldSeq, 
                        estFldCfgValNode.iSepFldFlag)); 
            return iRet;
         }

         if( estFldCfgValNode.iFldType == UDT_STRING ||  estFldCfgValNode.iFldType == UDT_BIN )
         {
            *opiSepMoveLen = estFldCfgValNode.iFldLen;
         } 
         else
         { 
            *opiSepMoveLen = iPtrMoveLen + iValContentLen;
         }

         break;
      case PKG_WY_FLD:
         /* 如果输出报文为结构体, 计算长度域之前的指针偏移量 */
         if( iToMsgIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen,
                                     estFldCfgValNode.iLenFldType,
                                     &iPtrMoveLen );
         }

         /* 计算长度域的长度同时赋默认值 */
         switch( estFldCfgValNode.iLenFldType )
         {
            case UDT_STRING:    
            case UDT_BIN:    
               iLenFldLen = estFldCfgValNode.iLenFldLen; 
               memset( opcMsgPos, aczLenFldDef[0], (size_t)iLenFldLen );
               break;
            case UDT_SHORT:  iLenFldLen = sizeof(short); break;
            case UDT_INT:    iLenFldLen = sizeof(int); break;
            case UDT_LONG:   iLenFldLen = sizeof(long); break;
            case UDT_DOUBLE: iLenFldLen = sizeof(double); break;
            case UDT_FLOAT:  iLenFldLen = sizeof(float); break;
         }

         /* 计算完成长度域的长度后, 计算该分域的偏移量以及报文的指针偏移量 */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         opcMsgPos += *opiSepMoveLen; 


         /* 如果输出报文为结构体, 计算内容域之前的指针偏移量 */
         if( iToMsgIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen,
                                     estFldCfgValNode.iFldType,
                                     &iPtrMoveLen );
         }
         

         /* 将内容域的内容写入输出报文 */
         if( estFldCfgValNode.iFldType == UDT_STRING || estFldCfgValNode.iFldType == UDT_BIN ) 
         {
            iFldLen = estFldCfgValNode.iFldLen;
            memset( opcMsgPos + iPtrMoveLen, aczFldDef[0], (size_t)iFldLen );

            /**** 底层控制输入字符串的长度超过域长度的情况 ******/
            if( estFldCfgValNode.iFldLen < eiValLen )
               eiValLen = estFldCfgValNode.iFldLen;

            if( estFldCfgValNode.aczFldAlign[0] == 'R' ||
                estFldCfgValNode.aczFldAlign[0] == 'r' )
            {
               iStrMoveLen = 0;
               iStrMoveLen = estFldCfgValNode.iFldLen - eiValLen; 
            }
         }


         iRet = PKG_WriteValToBuf( opcMsgPos+iPtrMoveLen+iStrMoveLen, 
                                   estFldCfgValNode.iFldType, 
                                   epczVal, eiValLen, eshVal, eiVal, elVal, 
                                   edVal, efVal, &iValContentLen );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_WriteValToBuf (in PKG_WriteSepFldContentToMsg) Has Error[%d] When Writing Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n", 
                         iRet, 
                        estFldCfgValNode.iMsgAppId, 
                        estFldCfgValNode.iFldSeq, 
                        estFldCfgValNode.iSepFldSeq, 
                        estFldCfgValNode.iSepFldFlag)); 
            return iRet;
         }

         /* 将内容的实际长度iValContentLen写入输出报文 */
         switch( estFldCfgValNode.iLenFldType )
         {
            case UDT_STRING:
            case UDT_BIN:
               sprintf( aczFldActLen, "%d", iValContentLen );
               iTmp = strlen( aczFldActLen ); 
               switch( estFldCfgValNode.aczLenFldAlign[0] )
               {
                  case 'L':
                  case 'l':
                     memcpy( opcMsgPos - iLenFldLen, aczFldActLen, (size_t)iTmp );
                     break;
                  default:
                     if( *eopiMoveLen == 0 )
                     {
                        iTmp = 0;
                     }
                     memcpy( opcMsgPos - iTmp, aczFldActLen, (size_t)iTmp );
                     break;
               }
               break;
            case UDT_SHORT:
               shValContentLen = (short)iValContentLen;
               memcpy( opcMsgPos-iLenFldLen, &shValContentLen, sizeof(short) );
               break;
            case UDT_INT:
               memcpy( opcMsgPos-iLenFldLen, &iValContentLen, sizeof(int) );
               break;
            case UDT_LONG:
               lValContentLen = (long)iValContentLen;
               memcpy( opcMsgPos-iLenFldLen, &lValContentLen, sizeof(long) );
               break;
            case UDT_DOUBLE:
               dValContentLen = (double)iValContentLen;
               memcpy( opcMsgPos-iLenFldLen, &dValContentLen, sizeof(double) );
               break;
            case UDT_FLOAT:
               fValContentLen = (float)iValContentLen;
               memcpy( opcMsgPos-iLenFldLen, &fValContentLen, sizeof(float) );
               break;
         }

         if( estFldCfgValNode.iFldType == UDT_STRING  || estFldCfgValNode.iFldType == UDT_BIN )
            *opiSepMoveLen += iPtrMoveLen + iFldLen;
         else
            *opiSepMoveLen += iPtrMoveLen + iValContentLen;
         break;
      case PKG_UNFIXLEN_FLD:
         /* 如果输出报文为结构体, 计算长度域之前的指针偏移量 */
         if( iToMsgIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen,
                                     estFldCfgValNode.iLenFldType,
                                     &iPtrMoveLen );
         }

         /* 计算长度域的长度同时赋默认值 */
         switch( estFldCfgValNode.iLenFldType )
         {
            case UDT_STRING:    
            case UDT_BIN:    
               iLenFldLen = estFldCfgValNode.iLenFldLen; 
               memset( opcMsgPos, aczLenFldDef[0], (size_t)iLenFldLen );
               break;
            case UDT_SHORT:  iLenFldLen = sizeof(short); break;
            case UDT_INT:    iLenFldLen = sizeof(int); break;
            case UDT_LONG:   iLenFldLen = sizeof(long); break;
            case UDT_DOUBLE: iLenFldLen = sizeof(double); break;
            case UDT_FLOAT:  iLenFldLen = sizeof(float); break;
         }

         /* 计算完成长度域的长度后, 计算该分域的偏移量以及报文的指针偏移量 */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         opcMsgPos += *opiSepMoveLen; 

         /* 如果输出报文为结构体, 计算内容域之前的指针偏移量 */
         if( iToMsgIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen,
                                     estFldCfgValNode.iFldType,
                                     &iPtrMoveLen );
         }
         
         /* 将内容域的内容写入输出报文 */
         if( estFldCfgValNode.iFldType == UDT_STRING ||  estFldCfgValNode.iFldType == UDT_BIN  ) 
         {
            iFldLen = estFldCfgValNode.iFldLen;
            memset( opcMsgPos + iPtrMoveLen, aczFldDef[0], (size_t)iFldLen );
         }

         iRet = PKG_WriteValToBuf( opcMsgPos+iPtrMoveLen, 
                                   estFldCfgValNode.iFldType, 
                                   epczVal, eiValLen, eshVal, eiVal, elVal, 
                                   edVal, efVal, &iValContentLen );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_WriteValToBuf (in PKG_WriteSepFldContentToMsg) Has Error[%d] When Writing Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n", 
                         iRet, 
                        estFldCfgValNode.iMsgAppId, 
                        estFldCfgValNode.iFldSeq, 
                        estFldCfgValNode.iSepFldSeq, 
                        estFldCfgValNode.iSepFldFlag)); 
            return iRet;
         }

         /* 将长度iValContentLen写入输出报文 */
         switch( estFldCfgValNode.iLenFldType )
         {
            case UDT_STRING:
            case UDT_BIN:
               
               memset(aczFldActLen,0x00,sizeof(aczFldActLen));
               sprintf( aczFldActLen, "%d", iValContentLen );
               iTmp = strlen( aczFldActLen ); 
               switch( estFldCfgValNode.aczLenFldAlign[0] )
               {
                  case 'L':
                  case 'l':
                     memcpy( opcMsgPos - iLenFldLen, aczFldActLen, (size_t)iTmp );
                     break;
                  default:
                     memcpy( opcMsgPos - iTmp, aczFldActLen, (size_t)iTmp );
                     break;
               }
               break;
            case UDT_SHORT:
               shValContentLen = (short)iValContentLen;
               memcpy( opcMsgPos - iLenFldLen, &iValContentLen, sizeof(short) );
               break;
            case UDT_INT:
               memcpy( opcMsgPos - iLenFldLen, &iValContentLen, sizeof(int) );
               break;
            case UDT_LONG:
               lValContentLen = (long)iValContentLen;
               memcpy( opcMsgPos - iLenFldLen, &lValContentLen, sizeof(long) );
               break;
            case UDT_DOUBLE:
               dValContentLen = (double)iValContentLen;
               memcpy( opcMsgPos - iLenFldLen, &dValContentLen, sizeof(double) );
               break;
            case UDT_FLOAT:
               fValContentLen = (float)iValContentLen;
               memcpy( opcMsgPos - iLenFldLen, &fValContentLen, sizeof(float) );
               break;
         }

         *opiSepMoveLen += iValContentLen;

         break;
      case PKG_SEPSYMBOL1_FLD:
         /***********
         if( estFldCfgValNode.aczFldAlign[0] == 'L' ||
             estFldCfgValNode.aczFldAlign[0] == 'l' )
         {
            memcpy( opcMsgPos, aczSepSymbol, (size_t)iSepSymbolLen );
            opcMsgPos += iSepSymbolLen; 
         }
         *****************/

         /* 将内容写入输出报文 */
         iRet = PKG_WriteValToBuf( opcMsgPos,
                                   estFldCfgValNode.iFldType,
                                   epczVal, eiValLen, eshVal, eiVal, 
                                   elVal, edVal, efVal, &iValContentLen );

         opcMsgPos += iValContentLen; 

         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_WriteValToBuf (in PKG_WriteSepFldContentToMsg) Has Error[%d] When Writing Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n", 
                         iRet, 
                        estFldCfgValNode.iMsgAppId, 
                        estFldCfgValNode.iFldSeq, 
                        estFldCfgValNode.iSepFldSeq, 
                        estFldCfgValNode.iSepFldFlag)); 
            return iRet;
         }

         /**********
         if( estFldCfgValNode.aczFldAlign[0] == 'R' ||
             estFldCfgValNode.aczFldAlign[0] == 'r' )
         {
             memcpy( opcMsgPos, aczSepSymbol, (size_t)iSepSymbolLen );
         }
         **************/
         memcpy( opcMsgPos, aczSepSymbol, (size_t)iSepSymbolLen );

         *opiSepMoveLen = iSepSymbolLen + iValContentLen; 
         break;
      case PKG_SEPSYMBOL2_FLD:
         /* 全部按照分割符在右侧来处理 */
         /* 最后分割符也加上,待PKG_WriteFldContentToMsg函数处理 */
         iRet = PKG_WriteValToBuf( opcMsgPos,
                                   estFldCfgValNode.iFldType,
                                   epczVal, eiValLen, eshVal, eiVal, elVal,
                                   edVal, efVal, &iValContentLen );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_WriteValToBuf (in PKG_WriteSepFldContentToMsg) Has Error[%d] When Writing Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n", 
                         iRet, 
                        estFldCfgValNode.iMsgAppId, 
                        estFldCfgValNode.iFldSeq, 
                        estFldCfgValNode.iSepFldSeq, 
                        estFldCfgValNode.iSepFldFlag)); 
            return iRet;
         }
         memcpy( opcMsgPos+iValContentLen, aczSepSymbol, (size_t)iSepSymbolLen );
         *opiSepMoveLen = iSepSymbolLen + iValContentLen; 

         break;
      case PKG_MUL8583_FLD:
         break;
      default: 
         LOG4C ((LOG_FATAL,
                     "The iSepFldFlag:[%d] (PKG_GetSepFldContentFromMsg) Has Error[%d], MsgAppId:[%d], Seq:[%d], SepSeq:[%d]\n",
                      PKG_SEPFLDFLAG_ERR,
                     estFldCfgValNode.iSepFldFlag, 
                     estFldCfgValNode.iFldSeq, estFldCfgValNode.iSepFldSeq));
         return PKG_SEPFLDFLAG_ERR;
   }

   if (iToMsgIsSt)
   {
       posInStruct += *opiSepMoveLen - iPtrMoveLen;
   }

   *eopiMoveLen += *opiSepMoveLen;

   return PKG_SUCC;
}

/*
**  函数功能: 将eiFldSeq号域第eiRepNum次重复信息写入输出报文
**
**  函数名:  PKG_WriteFldContentToMsg
**
**  输入参数: int    eiSeq                // 域号
**            int    eiRepNum             // 重复次数顺序号
**            MsgFldCfgAry *epstMsgFldCfgAry   
**            UDP    epfsAddr             // 中间信息报文
**            int * eopiMoveLen           // 指针从报文头的总偏移量
**
**  输出参数: char * opcMsgPos            // 输出报文 
**            int * eopiMoveLen           // 指针从报文头的总偏移量
**
**  返回值:   int 0成功, 其他失败 
*/

int  PKG_WriteFldContentToMsg( char * opcMsgPos, int * eopiMoveLen, 
                               int eiMsgAppId, int eiSeq, int eiRepNum,
                               MsgFldCfgAry *epstMsgFldCfgAry,
                               UDP epfsAddr ) 
{
   int  i, iRet, iDataType;
   int  iSepMoveLen = 0;    /* 分域偏移量 */
   char * pczVal = NULL;
   int  iVal, iValLen;
   short shVal;
   long lVal;
   float fVal;
   double dVal;
   FldCfgValNode stFldCfgValNode ;
   MsgFldCfgAry stMsgFldCfgAry;
   char aczTmp[3000];

   assert( opcMsgPos != NULL );
   assert( epfsAddr != NULL );
   assert( epstMsgFldCfgAry != NULL );

   /**** temp add for nesting ****/
   if (eiSeq >= NESTED_STRUCT_BEGINTAG_S && eiSeq <= NESTED_STRUCT_BEGINTAG_E)
   {
       nestedLevel += 1;
       if (nestedLevel == 0) 
       {
           iToMsgIsSt = 1;
           structStartMoveLen = *eopiMoveLen;
           posInStruct = 0;
       }
       else
       {
           posInStruct = getAlignPos (posInStruct, AG_CSTRUCT, 
                         alignStructNums[nestedLevel][structNums[nestedLevel]]);
           *eopiMoveLen = structStartMoveLen + posInStruct;
       }

       return PKG_SUCC;
   }


   if (eiSeq >= NESTED_STRUCT_ENDTAG_S && eiSeq <= NESTED_STRUCT_ENDTAG_E)
   {
       posInStruct = getAlignPos (posInStruct, AG_CSTRUCT, 
                       alignStructNums[nestedLevel][structNums[nestedLevel]]);
       structNums[nestedLevel]++;
       nestedLevel -= 1;

       *eopiMoveLen = structStartMoveLen + posInStruct;

       if (nestedLevel == -1)
       {
           iToMsgIsSt = 0;
           structStartMoveLen = 0;
           posInStruct = 0;
       }

       return PKG_SUCC;
   }

   /*****************************/

   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );

   iRet = PKG_GetInfoByFldSeq ( epstMsgFldCfgAry, eiMsgAppId,
                                eiSeq, &stMsgFldCfgAry );  
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_WriteFldContentToMsg Has Error[%d], There Is No FldCfgNode---Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, eiSeq));
      return PKG_NO_FLDCFGNODE;
   }


   for( i = 1; i <= stMsgFldCfgAry.iNum; i++ )
   {
      iRet = PKG_GetFldCfgValNode( &stMsgFldCfgAry,
                                   eiMsgAppId, eiSeq, i, &stFldCfgValNode ); 
      if ( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                    "PKG_GetFldCfgValNode Has Error[%d], There Is No FldCfgNode---Seq:[%d], SepFldSeq [%d] \n",
                      PKG_NO_FLDCFGNODE, eiSeq, i));
         return PKG_NO_FLDCFGNODE;
      }

      /* 判断eiSeq域中的第i个分域的类型 */
      iDataType = stFldCfgValNode.iFldType;

      /* 从链表中取得eiSeq域中的第i个分域的第eiRepNum次重复内容 */
      if( stFldCfgValNode.iSepFldFlag != PKG_BITMAP_FLD )
      {
         iRet = PKG_GetFldFromLST( epfsAddr, eiSeq, i, eiRepNum, iDataType,
                                   &pczVal, &iValLen, &shVal, &iVal, &lVal, 
                                   &dVal, &fVal );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetFldFromLST (in PKG_WriteFldContentToMsg) Has Error[%d]! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], Num:[%d], FldType:[%d]\n", 
                         iRet, stFldCfgValNode.iMsgAppId,
                        eiSeq, i, eiRepNum, iDataType ));
            return iRet;
         }

         /**********add 2011-09-20**********/
         switch( iDataType )
         {
         case UDT_STRING:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=S ][LEN=%03d][%s]", eiSeq, i, iValLen, pczVal ));
             break;
         case UDT_BIN:
             memset( aczTmp, 0, sizeof(aczTmp) );
             TOOL_ConvertBCD2Ascii( pczVal, iValLen*2, aczTmp );
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=B ][LEN=%03d][%s]", eiSeq, i, iValLen, aczTmp ));
             break;
         case UDT_SHORT:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=SH][LEN=%03d][%d]", eiSeq, i, sizeof(short), shVal ));
             break;
         case UDT_INT:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=I ][LEN=%03d][%d]", eiSeq, i, sizeof(int), iVal ));
             break;
         case UDT_LONG:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=L ][LEN=%03d][%d]", eiSeq, i, sizeof(long), lVal ));
             break;
         case UDT_FLOAT:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=F ][LEN=%03d][%.2f]", eiSeq, i, sizeof(float), fVal ));
             break;
         case UDT_DOUBLE:
             LOG4C(( LOG_DEBUG, "[FIELD=%04d,%d][TYPE=D ][LEN=%03d][%.2f]", eiSeq, i, sizeof(float), dVal ));
             break;
         }
         /**********************************/
      }

      /* 将分域的内容写入输出报文 */ 
      iRet = PKG_WriteSepFldContentToMsg( opcMsgPos+iSepMoveLen, eopiMoveLen,
                                          stFldCfgValNode,
                                          pczVal, iValLen, shVal, iVal, lVal, 
                                          dVal, fVal, &iSepMoveLen );

      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_WriteSepFldContentToMsg (in PKG_WriteFldContentToMsg) Has Error[%d]! At MsgAppId:[%d], Seq:[%d], SepSeq:[%d], Num:[%d]\n", 
                      iRet, stFldCfgValNode.iMsgAppId,
                     eiSeq, i, eiRepNum));
         return iRet;
      }

      if( iDataType == UDT_STRING || iDataType == UDT_BIN )
      {
         free( pczVal );
      }

   }

   return PKG_SUCC;
}

/*
** end of file: pkg_tomidbase.c
*/
