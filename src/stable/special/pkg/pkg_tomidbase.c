/*
**  file: pkg_tomidbase.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"

#include "pkg/pkg_tomidbase.h"

/** temp add from nesting **/
#include "tempadd.h"
/***************************/

#include "getAlignPos.h"
#define __MYDEBUGZ___
extern int nestedLevel;
extern int posInStruct;
extern int structStartMoveLen;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];

/*
**  函数功能: 读取BitMap的特定位,看其是否为1
**
**  函数名称: PKG_IsBitMapSet
**
**  输入参数: unsigned char * epczBitMap
**int eiNum
**
**  输出参数: 无
**
**  返回值:   0 --是, -1 -- 否, 其他错误
*/
int PKG_IsBitMapSet( unsigned char * eupczBitMap, int eiNum )
{
   int iByteNum, iBitNum, iBitMoved;
   char cTheBitChar;

   assert( eupczBitMap != NULL );

   if( eiNum < 1 || eiNum > PKG_BITMAP_MAX )
      return PKG_BITMAP_OVERFLOW;

   /* eiNum在该BitMap的第iByteNum个字节 */
   iByteNum = ( eiNum - 1 ) / 8;

   /* eiNum在所在字节的第iBitNum位 */
   iBitNum  = ( eiNum - 1 ) % 8;

   /* 在一个字节内, 0x01需要左移iBitMap位与eiNum位对齐 */
   iBitMoved = 7 - iBitNum;

   cTheBitChar = eupczBitMap[ iByteNum ];   /* eiNum位处的符号(1或0) */

   if( ((0x01 << iBitMoved) & cTheBitChar) != 0 )
      return PKG_SUCC;
   else
      return PKG_FAIL;
}


/*
**  函数功能: 计算一个重复区域的重复次数 
**
**  函数名:   PKG_GetRepTimesVal
**
**  输入参数: UDP        epfsAddr          // 中间信息报文起始地址
**            int        eiRepFldNum      // 重复区域序号(从0开始)  
**            char     * epczRepTimesSeq  // 重复域域号组成
**            char     * epczRepTimesNum  // 重复域序号组成 
** 
**  输出参数: int      * opiRepTimesVal   // 重复次数
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_GetRepTimesVal( UDP epfsAddr, int eiRepFldNum,
                         char * epczRepTimesSeq, char * epczRepTimesNum,
                         int * opiRepTimesVal )
{
   int iRet, iSeq, iNum, iDataType;
   char * pczFldVal = NULL;
   int iFldLen, iFldVal;
   short  shFldVal;
   long   lFldVal;
   double dFldVal;
   float  fFldVal;

   assert( epfsAddr != NULL );
   assert( epczRepTimesSeq != NULL );
   assert( epczRepTimesNum != NULL );

   iSeq = PKG_GetValFromFld( epczRepTimesSeq, eiRepFldNum+1 );
   iNum = PKG_GetValFromFld( epczRepTimesNum, eiRepFldNum+1 );
#ifdef __MYDEBUGZ___
   LOG4C ((LOG_DEBUG,"iSeq[%d] iNum [%d]",iSeq , iNum));
iNum =0;
#endif

   /* 取得该域的数据类型 */
   iDataType = UDO_FieldType( epfsAddr, iSeq );

   /* 从该域中取得重复次数 */
   iRet = PKG_GetFldFromLST( epfsAddr, iSeq, 1, iNum, iDataType,
                             &pczFldVal, &iFldLen, &shFldVal, &iFldVal, 
                             &lFldVal, &dFldVal, &fFldVal );
   if( iRet != PKG_SUCC )
   { 
      LOG4C ((LOG_FATAL,
                  "PKG_GetFldFromLST (in PKG_GetRepTimesVal) Has Error[%d], Seq:[%d], Num:[%d]\n",
                   iRet, iSeq, iNum));
      return iRet;
   }

   switch( iDataType )
   {
      case UDT_STRING:
      case UDT_BIN:
         *opiRepTimesVal = atoi( pczFldVal ); 
         free( pczFldVal );
         break;
      case UDT_SHORT:
         *opiRepTimesVal = shFldVal; 
         break;
      case UDT_INT:
         *opiRepTimesVal = iFldVal; 
         break;
      case UDT_LONG:
         *opiRepTimesVal = (int)lFldVal; 
         break;
      case UDT_DOUBLE:
         *opiRepTimesVal = (int)dFldVal; 
         break;
      case UDT_FLOAT:
         *opiRepTimesVal = (int)fFldVal; 
         break;
      default: 
         LOG4C ((LOG_FATAL,
                     "The iDataType:[%d] (in PKG_GetRepTimesVal) Has Error[%d], Seq:[%d], Num:[%d]\n",
                      iDataType, PKG_DATATYPE_INVALID, iSeq, iNum));
         return PKG_DATATYPE_INVALID;
   }

   return PKG_SUCC;
}

/*
**  函数功能: 根据数据类型从字符串中取值 
**
**  函数名:   PKG_GetValFromBuf
**
**  输入参数: char * epcBuf            // 字符串
**            int  eiDataType          // 数据类型
**            int eiValLen      // 字符串有效长度
**            
**  输出参数: char ** opczVal          // 字符串内容
**            short * opshVal          // 短整型
**            int * opiVal             // 整型值
**            long * oplVal            // 长整型值
**            double * opdVal          // 双精度值
**            float * opfVal           // 单精度值
**            int * opiValContentLen   // 值内容的长度
**
**  返回值: int  0 成功, 其他失败
**
**  操作说明: 函数内部分配了空间, 请调用者注意释放
*/

int  PKG_GetValFromBuf( char * epcBuf, int eiDataType, int eiValLen,
                        PKG_DataValue *opstDataValue )
{
   char * pczVal = NULL;

   assert( epcBuf != NULL );

   switch( eiDataType )
   {
      case UDT_STRING:
      case UDT_BIN:
         do 
         {
            pczVal = (char *)malloc( (size_t)(eiValLen + 1) );
         }while( pczVal == NULL );
         memcpy( pczVal, epcBuf, (size_t)eiValLen );
         *( pczVal + eiValLen ) = '\0';
         opstDataValue->nData.pcVal = pczVal;
         opstDataValue->iLen = eiValLen; 
         break;
      case UDT_SHORT:
         memcpy( &opstDataValue->nData.shVal, epcBuf, sizeof(short) );
         opstDataValue->iLen = sizeof(short);
         break;
      case UDT_INT:
         memcpy( &opstDataValue->nData.iVal, epcBuf, sizeof(int) );
         opstDataValue->iLen = sizeof(int);
         break;
      case UDT_LONG:
         memcpy( &opstDataValue->nData.lVal, epcBuf, sizeof(long) );
         opstDataValue->iLen = sizeof(long);
         break;
      case UDT_DOUBLE:
         memcpy( &opstDataValue->nData.dVal, epcBuf, sizeof(double) );
         opstDataValue->iLen = sizeof(double);
         break;
      case UDT_FLOAT:
         memcpy( &opstDataValue->nData.fVal, epcBuf, sizeof(float) );
         opstDataValue->iLen = sizeof(float);
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The Input 'eiDataType:[%d]' (in PKG_GetValFromBuf) Has Error[%d]\n",
                      eiDataType, PKG_DATATYPE_INVALID));
         return PKG_DATATYPE_INVALID;
   }

   return PKG_SUCC;
}


/*
**  函数功能: 根据域属性标志, 取得不同分域的值
**
**  函数名:   PKG_GetSepFldContentFromMsg
**
**  输入参数: char * epcMsgPos
**            int * eopiMoveLen                     // 指针距离报文头的长度
**            int  * eopiMsgLeftLen                 // 指针距离报文结尾的长度
**            FldCfgValNode estFldCfgValNode
**
**  输出参数: char ** opczVal
**            int  * opiValLen
**            short * opshVal
**            int  * opiVal
**            long * oplVal
**            double * opdVal
**            float * opfVal
**            int * opiSepMoveLen                   // 指针距离该分域头的长度
**            int * eopiMoveLen                     // 指针距离报文头的长度
**            int  * eopiMsgLeftLen                 // 指针距离报文结尾的长度
**
**  返回值: int  0 成功, 其他失败 
*/
int  PKG_GetSepFldContentFromMsg( char *epcMsgPos, int *eopiMoveLen, 
                                  int *eopiMsgLeftLen,
                                  MsgFldCfg *epstMsgFldCfg,
                                  PKG_DataValue *opstDataValue,
                                  int * opiSepMoveLen )
{
   int iRet, iPos=0, iTmp=0, iPtrMoveLen = 0, iStrMoveLen = 0;
   int iValLen, iSepSymbolLen;
   int iLenFldLen, iFldLen, iFldActLen;
   char aczLenFldDef[PKG_DEF_LEN+1], aczFldDef[PKG_DEF_LEN+1]; 
   char aczSepSymbol[PKG_SEP_SYMBOL_LEN+1];
   char * pczVal = NULL;

   assert( epcMsgPos != NULL );

   memset( aczLenFldDef, 0x00, PKG_DEF_LEN+1 );
   memset( aczFldDef, 0x00, PKG_DEF_LEN+1 );
   memset( aczSepSymbol, 0x00, PKG_SEP_SYMBOL_LEN+1 );

   if( strlen( epstMsgFldCfg->aczLenFldDef ) != 0 )
   {
      iRet = PKG_XToA( epstMsgFldCfg->aczLenFldDef, aczLenFldDef );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_GetSepFldContentFromMsg) Has "
                     "Error[%d]! The Inputed is LenFldDef:[%s]! Its iSeq:[%d],"
                     " iSepFldSeq:[%d] \n",
                      iRet, epstMsgFldCfg->aczLenFldDef, 
                     epstMsgFldCfg->iFldSeq, epstMsgFldCfg->iSepFldSeq));
         return iRet;
      }
   }

   if( strlen( epstMsgFldCfg->aczFldDef ) != 0 )
   {
      iRet = PKG_XToA( epstMsgFldCfg->aczFldDef, aczFldDef );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_GetSepFldContentFromMsg) Has Error[%d]! The Inputed is FldDef:[%s]! Its iSeq:[%d], iSepFldSeq:[%d] \n",
                      iRet, epstMsgFldCfg->aczFldDef, 
                     epstMsgFldCfg->iFldSeq, epstMsgFldCfg->iSepFldSeq));
         return iRet;
      }
   }

   if( strlen( epstMsgFldCfg->aczSepSymbol ) != 0 )
   {
      iRet = PKG_XToA( epstMsgFldCfg->aczSepSymbol, aczSepSymbol );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_XToA (in PKG_GetSepFldContentFromMsg) Has Error[%d]! The Inputed is SepSymbol:[%s]! Its iSeq:[%d], iSepFldSeq:[%d]\n",
                      iRet, epstMsgFldCfg->aczSepSymbol, 
                     epstMsgFldCfg->iFldSeq, epstMsgFldCfg->iSepFldSeq));
         return iRet;
      }
      iSepSymbolLen = strlen( aczSepSymbol );
   }

   switch( epstMsgFldCfg->iSepFldFlag )
   {
      case PKG_BITMAP_FLD: 
         *opiSepMoveLen = epstMsgFldCfg->iFldLen;
         *eopiMoveLen    += epstMsgFldCfg->iFldLen;
         *eopiMsgLeftLen -= epstMsgFldCfg->iFldLen;
         return PKG_IS_BITMAP;
      case PKG_FIXLEN_FLD:
         /* 如果输入的报文为结构体,计算在内容域之前的指针偏移量 */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iFldType, 
                                     &iPtrMoveLen );
         }
         if( epstMsgFldCfg->iFldType == UDT_STRING  || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            iFldLen = epstMsgFldCfg->iFldLen;
            switch( epstMsgFldCfg->aczFldAlign[0] )
            {
               case 'L':
               case 'l':
                  while( (epcMsgPos+iFldLen-iTmp-1)[0] == aczFldDef[0] 
                         && iTmp < iFldLen )
                  {
                     iTmp++;
                  }
                  break;
               default:
                  while( (epcMsgPos+iTmp)[0] == aczFldDef[0] 
                         && iTmp < epstMsgFldCfg->iFldLen ) 
                  {
                     iTmp++;
                  }
                  epcMsgPos += iTmp;
                  break;
            }
            iValLen = iFldLen - iTmp;
         }


         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen, 
                                   epstMsgFldCfg->iFldType, 
                                   iValLen, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                         iRet, 
                        epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                        epstMsgFldCfg->iSepFldSeq, 
                        epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         if( epstMsgFldCfg->iFldType == UDT_STRING  || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            opstDataValue->iLen = iValLen;
            *opiSepMoveLen = iFldLen;
         }
         else
         {
            *opiSepMoveLen = iPtrMoveLen + opstDataValue->iLen;
         }

         break;
      case PKG_WY_FLD:
         /* 如果输入报文为结构体, 计算长度域之前的指针偏移量 */
         if( iToMidIsSt == PKG_IS_STRUCT )
         { 
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iLenFldType, 
                                     &iPtrMoveLen );
         }

         /* 从长度域中取得的实际内容 */ 
         if( epstMsgFldCfg->iLenFldType == UDT_STRING || epstMsgFldCfg->iLenFldType == UDT_BIN)
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;
            switch( epstMsgFldCfg->aczLenFldAlign[0] )
            {
               case 'L':
               case 'l':
                  while( (epcMsgPos+iLenFldLen-iTmp-1)[0] == aczLenFldDef[0] 
                         && iTmp < iLenFldLen )
                  {
                     iTmp++;
                  }
                  break;
               default:
                  while( (epcMsgPos+iTmp)[0] == aczLenFldDef[0] 
                         && iTmp < iLenFldLen ) 
                  {
                     iTmp++;
                  }
                  iStrMoveLen = iTmp;
                  break;
            }
            iValLen = iLenFldLen - iTmp;
         }

         /* iStrMoveLen只有当长度域为字符型且有对齐时才有用 */
         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen+iStrMoveLen,
                                   epstMsgFldCfg->iLenFldType, 
                                   iValLen, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                     "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting LenFld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                      iRet, 
                     epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                     epstMsgFldCfg->iSepFldSeq, 
                     epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         /* 计算长度域的长度 */
         if( epstMsgFldCfg->iLenFldType == UDT_STRING ||  epstMsgFldCfg->iLenFldType == UDT_BIN )
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;
         }
         else
         {
            iLenFldLen = opstDataValue->iLen;
         }

         /* 计算内容域的实际长度 */
         switch( epstMsgFldCfg->iLenFldType )
         {
            case UDT_STRING: 
            case UDT_BIN: 
                  iFldActLen = atoi(opstDataValue->nData.pcVal);
                            free(opstDataValue->nData.pcVal);
                            break;
            case UDT_SHORT:  iFldActLen = opstDataValue->nData.shVal; break;
            case UDT_INT:    iFldActLen = opstDataValue->nData.iVal; break;
            case UDT_LONG:   iFldActLen = (int)opstDataValue->nData.lVal; break;
            case UDT_DOUBLE: iFldActLen = (int)opstDataValue->nData.dVal; break;
            case UDT_FLOAT:  iFldActLen = (int)opstDataValue->nData.fVal; break;
         }


         /* 对长度域操作完毕后, 计算该分域的指针偏移量以及源报文的指针偏移量 */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         epcMsgPos += *opiSepMoveLen; 

         /* 如果输入报文为结构体, 计算内容域之前的指针偏移量 */
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen, 
                                     epstMsgFldCfg->iFldType,
                                     &iPtrMoveLen );
         }

         /* 从报文中取得实际的内容 */
         if( epstMsgFldCfg->iFldType == UDT_STRING  || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            iFldLen = epstMsgFldCfg->iFldLen;

            if( epstMsgFldCfg->aczFldAlign[0] == 'R' ||
                epstMsgFldCfg->aczFldAlign[0] == 'r' )
            {
               epcMsgPos += iFldLen - iFldActLen;
            }
         }

         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen,
                                   epstMsgFldCfg->iFldType,
                                   iFldActLen, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                         iRet, 
                        epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                        epstMsgFldCfg->iSepFldSeq, 
                        epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }
          
         if( epstMsgFldCfg->iFldType == UDT_STRING || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            *opiSepMoveLen += iFldLen;
         }
         else
         {
            *opiSepMoveLen += iPtrMoveLen + opstDataValue->iLen;
         }
         break;
      case PKG_UNFIXLEN_FLD:
         /* 如果输入的报文为结构体,计算在长度域之前的指针偏移量 */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iLenFldType, 
                                     &iPtrMoveLen );
         }

         /* 从长度域中取得实际内容的长度 */ 
         if( epstMsgFldCfg->iLenFldType == UDT_STRING ||  epstMsgFldCfg->iLenFldType == UDT_BIN )
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;

            switch( epstMsgFldCfg->aczLenFldAlign[0] )
            {
               case 'L':
               case 'l':
                  while( (epcMsgPos+iLenFldLen-iTmp-1)[0] == aczLenFldDef[0] 
                         && iTmp < iLenFldLen )
                  {
                     iTmp++;
                  }
                  break;
               default:
                  while( (epcMsgPos+iTmp)[0] == aczLenFldDef[0] 
                         && iTmp < iLenFldLen ) 
                  {
                     iTmp++;
                  }
                  iStrMoveLen += iTmp;
                  break;
            }
            iValLen = iLenFldLen - iTmp;
         }

         /* iStrMoveLen只有当长度域为字符型且有对齐时才有用 */
         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen+iStrMoveLen,
                                   epstMsgFldCfg->iLenFldType, 
                                   iValLen, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                     "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting LenFld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                      iRet, 
                     epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                     epstMsgFldCfg->iSepFldSeq, 
                     epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         /* 计算长度域的长度 */
         if( epstMsgFldCfg->iLenFldType == UDT_STRING || epstMsgFldCfg->iLenFldType == UDT_BIN)
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;
         }
         else
         {
            iLenFldLen = opstDataValue->iLen;
         }

         /* 计算内容的实际长度 */
         switch( epstMsgFldCfg->iLenFldType )
         {
            case UDT_STRING:
            case UDT_BIN:
                iFldActLen = atoi(opstDataValue->nData.pcVal);
                            free(opstDataValue->nData.pcVal);
                            break;
            case UDT_SHORT:  iFldActLen = opstDataValue->nData.shVal; break;
            case UDT_INT:    iFldActLen = opstDataValue->nData.iVal; break;
            case UDT_LONG:   iFldActLen = (int)opstDataValue->nData.lVal; break;
            case UDT_DOUBLE: iFldActLen = (int)opstDataValue->nData.dVal; break;
            case UDT_FLOAT:  iFldActLen = (int)opstDataValue->nData.fVal; break;
         }

         /* 对长度域操作完毕后, 计算该分域的指针偏移量以及源报文的指针偏移量 */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         epcMsgPos += *opiSepMoveLen;


         /* 如果输入的报文为结构体,计算在内容域之前的指针偏移量 */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen, 
                                     epstMsgFldCfg->iFldType, 
                                     &iPtrMoveLen );
         }
         
         /* 从报文中取得实际内容 */
         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen,
                                   epstMsgFldCfg->iFldType,  
                                   iFldActLen, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                         iRet, 
                        epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                        epstMsgFldCfg->iSepFldSeq, 
                        epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         if( epstMsgFldCfg->iFldType == UDT_STRING || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            opstDataValue->iLen = opstDataValue->iLen;
         }

         *opiSepMoveLen += iPtrMoveLen + opstDataValue->iLen;
         break;
      case PKG_SEPSYMBOL1_FLD:
         /* 判断字符串是否在域的左侧 */
         /*******
         switch( epstMsgFldCfg->aczFldAlign[0] ) 
         {
            case 'L':
            case 'l':
               iPos = iSepSymbolLen;
               break;
         }
         *****/
         iPos = 0;

         if( epstMsgFldCfg->iFldType == UDT_STRING || epstMsgFldCfg->iFldType == UDT_BIN )
         {
            pczVal = strstr( epcMsgPos+iPos, aczSepSymbol );
         
            for( iTmp = 0; 
                 strcmp( epcMsgPos+iPos+iTmp, pczVal ) != 0 
                         && iTmp < *eopiMsgLeftLen-iPos; 
                 iTmp++ )
            {
               /*Do nothing*/
            }
         } 

         /* 取出该域段的实际内容 */
         iRet = PKG_GetValFromBuf( epcMsgPos+iPos,
                                   epstMsgFldCfg->iFldType,  
                                   iTmp, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                         iRet, 
                        epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                        epstMsgFldCfg->iSepFldSeq, 
                        epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         *opiSepMoveLen = iSepSymbolLen + opstDataValue->iLen; 
         break;
      case PKG_SEPSYMBOL2_FLD:
         /* 全部按照右侧为分割符的方式来处理 */
         if( epstMsgFldCfg->iFldType == UDT_STRING ||  epstMsgFldCfg->iFldType == UDT_BIN )
         {
            pczVal = strstr( epcMsgPos, aczSepSymbol );
            for( iTmp = 0; 
                 strcmp( epcMsgPos+iTmp, pczVal ) != 0 
                         && iTmp < *eopiMsgLeftLen; 
                 iTmp++ )
            {
               /*Do nothing*/
            }
         }


         /* 如果输入的报文为结构体,计算在内容域之前的指针偏移量 */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iFldType, 
                                     &iPtrMoveLen );
         }

         iRet = PKG_GetValFromBuf( epcMsgPos+iPtrMoveLen,
                                   epstMsgFldCfg->iFldType,  
                                   iTmp, opstDataValue );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_GetValFromBuf (in PKG_GetSepFldContentFromMsg) Has Error[%d] When Getting Fld Content! MsgAppId:[%d], Seq:[%d], SepSeq:[%d], SepFldFlag:[%d]\n",
                         iRet, 
                        epstMsgFldCfg->iMsgAppId, epstMsgFldCfg->iFldSeq, 
                        epstMsgFldCfg->iSepFldSeq, 
                        epstMsgFldCfg->iSepFldFlag)); 
            return iRet;
         }

         /* 判断是否为最后一个域段 */
         if( iTmp == *eopiMsgLeftLen )  
         {
            *opiSepMoveLen = iPtrMoveLen + opstDataValue->iLen;
         }
         else
         {
            *opiSepMoveLen = iPtrMoveLen + opstDataValue->iLen + iSepSymbolLen; 
         }

         break;
      case PKG_MUL8583_FLD:
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The iSepFldFlag:[%d] "
                     "(PKG_GetSepFldContentFromMsg) Has Error[%d], "
                     "MsgAppId:[%d], Seq:[%d], SepSeq:[%d]\n",
                      PKG_SEPFLDFLAG_ERR,
                     epstMsgFldCfg->iMsgAppId, 
                     epstMsgFldCfg->iFldSeq, epstMsgFldCfg->iSepFldSeq));
         return PKG_SEPFLDFLAG_ERR;
   }

   if (iToMidIsSt)
   {
       posInStruct += *opiSepMoveLen - iPtrMoveLen;
   }

   *eopiMoveLen    += *opiSepMoveLen;
   *eopiMsgLeftLen -= *opiSepMoveLen; 

   return PKG_SUCC;
}

/*
**  函数功能: 将eiSeq号域的eiSepSeq号分域的值按类型插入链表(目前不支持分域)
**
**  函数名:   PKG_AddSepFldContentToLST
**
**  输入参数: int  eiSeq          // 域号
**            int  eiSepSeq       // 分域号
**            int  eiDataType     // 数据类型
**            char * epczVal      // 字符串值
**            int  eiValLen        // 字符串值长度
**            short eshVal         // 短整型
**            int  eiVal           // 整型值
**            long elVal           // 长整型值
**            double edVal         // 双精度值 
**            float efVal          // 单精度值
**
**  输出参数: UDP opfsAddr 
**
**  返回值: int 0 成功,其他失败
*/

extern FILE *fpPkgLog;

int  PKG_AddSepFldContentToLST( int eiSeq, int eiSepSeq, int eiDataType,
                                PKG_DataValue *epstDataValue,
                                UDP opfsAddr )
{
   int iRet;
   char aczTmp[4000];

   assert( opfsAddr != NULL );

   switch( eiDataType )
   {
      case UDT_STRING:
         assert( epstDataValue->nData.pcVal != NULL );
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             epstDataValue->nData.pcVal, epstDataValue->iLen );
         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%s]\n", eiSeq, epstDataValue->nData.pcVal);
         }
         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=S][LEN=%03d][%s]",
                     eiSeq, epstDataValue->iLen, epstDataValue->nData.pcVal )); 

         break;
      case UDT_BIN:
         assert( epstDataValue->nData.pcVal != NULL );
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             epstDataValue->nData.pcVal, epstDataValue->iLen );
         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, "%d=[%s]\n", eiSeq, epstDataValue->nData.pcVal);
         }

         memset( aczTmp, 0, sizeof(aczTmp) );
         TOOL_ConvertBCD2Ascii( epstDataValue->nData.pcVal, 2 * epstDataValue->iLen , aczTmp );
         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=B][LEN=%03d][%s]",
                     eiSeq, epstDataValue->iLen, aczTmp )); 

         break;
      case UDT_SHORT:
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             (char *)&epstDataValue->nData.shVal,
                             sizeof(short) );

         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%d]\n", eiSeq, epstDataValue->nData.shVal);
         }

         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=SH][LEN=%03d][%d]",
                     eiSeq, sizeof(short), epstDataValue->nData.shVal )); 
         break;
      case UDT_INT:
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             (char *)&epstDataValue->nData.iVal, sizeof(int) );

         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%d]\n", eiSeq, epstDataValue->nData.iVal);
         }

         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=I][LEN=%03d][%d]",
                     eiSeq, sizeof(int), epstDataValue->nData.iVal )); 
         break;
      case UDT_LONG:
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             (char *)&epstDataValue->nData.lVal, sizeof(long) );

         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%ld]\n", eiSeq, epstDataValue->nData.lVal);
         }

         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=L][LEN=%03d][%d]",
                     eiSeq, sizeof(long), epstDataValue->nData.lVal )); 
         break;
      case UDT_DOUBLE:
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             (char *)&epstDataValue->nData.dVal,
                             sizeof(double) );

         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%lf]\n", eiSeq, epstDataValue->nData.dVal);
         }

         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=D][LEN=%03d][%.2f]",
                     eiSeq, sizeof(double), epstDataValue->nData.dVal )); 
         break;
      case UDT_FLOAT:
         iRet = UDO_AddField( opfsAddr, eiSeq, eiDataType, 
                             (char *)&epstDataValue->nData.fVal,
                             sizeof(float) );

         if (fpPkgLog != NULL) {
             fprintf (fpPkgLog, 
                      "%d=[%f]\n", eiSeq, epstDataValue->nData.fVal);
         }

         LOG4C(( LOG_DEBUG, "[FIELD=%04d][TYPE=F][LEN=%03d][%.2f]",
                     eiSeq, sizeof(float), epstDataValue->nData.fVal )); 
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The Input 'eiDataType:[%d]' (in PKG_AddSepFldCointentToLST) Has Error! Seq:[%d], SepSeq:[%d]\n",
                      eiSeq, eiSepSeq));
         return PKG_DATATYPE_INVALID;
   }
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "LST_AddField (in PKG_AddSepFldContentToLST) Has Error[%d]! Seq:[%d], SepSeq:[%d]\n",
                   iRet));
      return iRet; 
   } 

   return PKG_SUCC;
}

/*
**  函数功能: 将eiSeq号域信息插入链表
**
**  函数名:  PKG_AddFldContentToLST
**
**  输入参数: char * epcMsgPos            // 已经指针指向eiFldSeq号域首的源报文
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int  * eopiMsgLeftLen       // 报文指针离尾部的的长度
**            int    eiSeq                // 域号
**            MsgFldCfgAry *epstMsgFldCfgAry    // 域空间起始地址
**                                            及域记录数
**
**  输出参数: UDP opfsAddr      // 中间信息链表
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int * eopiMsgLeftLen
**
**  返回值:   int 0成功, 其他失败 
*/

int  PKG_AddFldContentToLST( char *epcMsgPos, int *eopiMoveLen, 
                             int *eopiMsgLeftLen, int eiMsgAppId,
                             int eiSeq, MsgFldCfgAry *epstMsgFldCfgAry,
                             UDP opfsAddr )
{
   int  i, iRet;
   PKG_DataValue stDataValue;
   int  iSepMoveLen = 0;
   MsgFldCfg stMsgFldCfg;
   MsgFldCfgAry stMsgFldCfgAry;

   assert( epstMsgFldCfgAry != NULL );
   assert( epcMsgPos != NULL );
   assert( opfsAddr != NULL );

   memset( &stMsgFldCfg, 0x00, sizeof(MsgFldCfg) );
   memset( &stDataValue, 0x00, sizeof(PKG_DataValue) );

   /**** temp add for nesting ****/
   if (eiSeq >= NESTED_STRUCT_BEGINTAG_S && eiSeq <= NESTED_STRUCT_BEGINTAG_E)
   {
       nestedLevel += 1;
       if (nestedLevel == 0) 
       {
           iToMidIsSt = 1;
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
           iToMidIsSt = 0;
           structStartMoveLen = 0;
           posInStruct = 0;
       }
       return PKG_SUCC;
   }

#ifdef DEBUG
printf ("%d nested %d %d structStartMoveLen %d posInStruct %d eiMove %d\n", 
        eiSeq, nestedLevel, structNums[nestedLevel], structStartMoveLen, posInStruct,
        *eopiMoveLen);
#endif

   iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry, eiMsgAppId,
                               eiSeq , &stMsgFldCfgAry ); 
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_AddFldContentToLST Has Error[%d], There Is "
                  "No FldCfgNode---MsgAppId:[%d] Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, eiMsgAppId, eiSeq));
      return PKG_NO_FLDCFGNODE;
   }

   for( i = 1; i <= stMsgFldCfgAry.iNum; i++ )
   {
      stMsgFldCfg = stMsgFldCfgAry.pstMsgFldCfg[i-1];

      /* 根据域属性类型, 取得域的内容, 按类型分别放置 */
      /* 该函数中的iSepMoveLen只有当一个域有分域时才起作用,此次开发没有用 */
      iRet = PKG_GetSepFldContentFromMsg( epcMsgPos+iSepMoveLen, 
                                          eopiMoveLen,
                                          eopiMsgLeftLen, &stMsgFldCfg, 
                                          &stDataValue, &iSepMoveLen );
      if( iRet == PKG_IS_BITMAP )
      {
         /* 判断指针偏移量是否超过报文总长度 */
         if( *eopiMsgLeftLen < 0 )
         {
            LOG4C ((LOG_WARN,
                        "The Org Msg Length (MsgAppId:[%d]) Is Shorter Than It Should Be! After Seq:[%d], SepSeq:[%d], The Msg Pos Has Out Of OrgMsg,eopiMsgLeftLen[%d]\n",
                         stMsgFldCfg.iMsgAppId, 
                        stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq,
                        *eopiMsgLeftLen));
            return PKG_LENGTH_SHORT;
         }

         return PKG_IS_BITMAP;
      }
      if( iRet != PKG_SUCC && iRet != PKG_IS_BITMAP )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_GetSepFldContentFromMsg (in PKG_AddFldCon"
                     "tentToLST) Has Error[%d]! At MsgAppId:[%d], Seq:[%d], S"
                     "epSeq:[%d] \n",  
                      iRet, stMsgFldCfg.iMsgAppId,
                     stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq));
         return iRet;
      }

      /* 判断指针偏移量是否超过报文总长度 */
      if( *eopiMsgLeftLen < 0 )
      {
         LOG4C ((LOG_WARN,
                     "The Org Msg Length (MsgAppId:[%d]) Is Shorter "
                     "Than It Should Be! After Seq:[%d], SepSeq:[%d], The Msg "
                     "Pos Has Out Of OrgMsg,eopiMsgLeftLen[%d]\n",
                      stMsgFldCfg.iMsgAppId, 
                     stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq, *eopiMsgLeftLen));
         return PKG_LENGTH_SHORT;
      }

      /* 将分域的内容按类型插入链表 */
      iRet = PKG_AddSepFldContentToLST( eiSeq, i, stMsgFldCfg.iFldType, 
                                        &stDataValue, opfsAddr ); 
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_AddSepFldContentToLST (in PKG_AddFldContentToLST) Has Error[%d], MsgAppId:[%d], Seq:[%d], SepSeq:[%d], FldType:[%d]\n",
                      iRet, stMsgFldCfg.iMsgAppId, 
                     eiSeq, i, stMsgFldCfg.iFldType));
         return iRet;
      }

      /* 释放空间 */
      if( stMsgFldCfg.iFldType == UDT_STRING  ||  stMsgFldCfg.iFldType == UDT_BIN )
      {
         free( stDataValue.nData.pcVal );
      }
   }

   return PKG_SUCC;
}

/*
** end of file: pkg_tomidbase.c
*/



/*
**  函数功能: 将eiSeq号域信息插入链表
**
**  函数名:  PKG_AddFldContentToLST
**
**  输入参数: char * epcMsgPos            // 已经指针指向eiFldSeq号域首的源报文
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int  * eopiMsgLeftLen       // 报文指针离尾部的的长度
**            int    eiSeq                // 域号
**            MsgFldCfgAry *epstMsgFldCfgAry    // 域空间起始地址
**                                            及域记录数
**
**  输出参数: UDP opfsAddr      // 中间信息链表
**            int  * eopiMoveLen          // 指针从报文头的总偏移量
**            int * eopiMsgLeftLen
**
**  返回值:   int 0成功, 其他失败 
**  Modify By Houyq:20070515
*/

int  PKG_AddFldContentToLST_Trim( char *epcMsgPos, int *eopiMoveLen, 
                             int *eopiMsgLeftLen, int eiMsgAppId,
                             int eiSeq, MsgFldCfgAry *epstMsgFldCfgAry,
                             UDP opfsAddr )
{
   int  i, iRet;
   PKG_DataValue stDataValue;
   int  iSepMoveLen = 0;
   MsgFldCfg stMsgFldCfg;
   MsgFldCfgAry stMsgFldCfgAry;

   assert( epstMsgFldCfgAry != NULL );
   assert( epcMsgPos != NULL );
   assert( opfsAddr != NULL );

   memset( &stMsgFldCfg, 0x00, sizeof(MsgFldCfg) );
   memset( &stDataValue, 0x00, sizeof(PKG_DataValue) );

   /**** temp add for nesting ****/
   if (eiSeq >= NESTED_STRUCT_BEGINTAG_S && eiSeq <= NESTED_STRUCT_BEGINTAG_E)
   {
       nestedLevel += 1;
       if (nestedLevel == 0) 
       {
           iToMidIsSt = 1;
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
           iToMidIsSt = 0;
           structStartMoveLen = 0;
           posInStruct = 0;
       }
       return PKG_SUCC;
   }

#ifdef DEBUG
printf ("%d nested %d %d structStartMoveLen %d posInStruct %d eiMove %d\n", 
        eiSeq, nestedLevel, structNums[nestedLevel], structStartMoveLen, posInStruct,
        *eopiMoveLen);
#endif

   iRet = PKG_GetInfoByFldSeq( epstMsgFldCfgAry, eiMsgAppId,
                               eiSeq , &stMsgFldCfgAry ); 
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_AddFldContentToLST Has Error[%d], There Is "
                  "No FldCfgNode---MsgAppId:[%d] Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, eiMsgAppId, eiSeq));
      return PKG_NO_FLDCFGNODE;
   }

   for( i = 1; i <= stMsgFldCfgAry.iNum; i++ )
   {
      stMsgFldCfg = stMsgFldCfgAry.pstMsgFldCfg[i-1];

      /* 根据域属性类型, 取得域的内容, 按类型分别放置 */
      /* 该函数中的iSepMoveLen只有当一个域有分域时才起作用,此次开发没有用 */
      iRet = PKG_GetSepFldContentFromMsg( epcMsgPos+iSepMoveLen, 
                                          eopiMoveLen,
                                          eopiMsgLeftLen, &stMsgFldCfg, 
                                          &stDataValue, &iSepMoveLen );
      if( iRet == PKG_IS_BITMAP )
      {
         /* 判断指针偏移量是否超过报文总长度 */
         if( *eopiMsgLeftLen < 0 )
         {
            LOG4C ((LOG_WARN,
                        "The Org Msg Length (MsgAppId:[%d]) Is Shorter Than It Should Be! After Seq:[%d], SepSeq:[%d], The Msg Pos Has Out Of OrgMsg,eopiMsgLeftLen[%d]\n",
                         stMsgFldCfg.iMsgAppId, 
                        stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq,
                        *eopiMsgLeftLen));
            return PKG_LENGTH_SHORT;
         }

         return PKG_IS_BITMAP;
      }
      if( iRet != PKG_SUCC && iRet != PKG_IS_BITMAP )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_GetSepFldContentFromMsg (in PKG_AddFldCon"
                     "tentToLST) Has Error[%d]! At MsgAppId:[%d], Seq:[%d], S"
                     "epSeq:[%d] \n",  
                      iRet, stMsgFldCfg.iMsgAppId,
                     stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq));
         return iRet;
      }

      /* 判断指针偏移量是否超过报文总长度 */
      if( *eopiMsgLeftLen < 0 )
      {
         LOG4C ((LOG_WARN,
                     "The Org Msg Length (MsgAppId:[%d]) Is Shorter "
                     "Than It Should Be! After Seq:[%d], SepSeq:[%d], The Msg "
                     "Pos Has Out Of OrgMsg,eopiMsgLeftLen[%d]\n",
                      stMsgFldCfg.iMsgAppId, 
                     stMsgFldCfg.iFldSeq, stMsgFldCfg.iSepFldSeq, *eopiMsgLeftLen));
         return PKG_LENGTH_SHORT;
      }

      /* 将分域的内容按类型插入链表 */
      /* Add By Houyq 20070515 == begin == */
      if( UDT_STRING == stMsgFldCfg.iFldType )
      {
      	 PUB_StrTrim((char *)&stDataValue.nData,' ');
      }
      /* Add By Houyq 20070515 == end == */
      
      iRet = PKG_AddSepFldContentToLST( eiSeq, i, stMsgFldCfg.iFldType, 
                                        &stDataValue, opfsAddr ); 
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_AddSepFldContentToLST (in PKG_AddFldContentToLST) Has Error[%d], MsgAppId:[%d], Seq:[%d], SepSeq:[%d], FldType:[%d]\n",
                      iRet, stMsgFldCfg.iMsgAppId, 
                     eiSeq, i, stMsgFldCfg.iFldType));
         return iRet;
      }

      /* 释放空间 */
      if( stMsgFldCfg.iFldType == UDT_STRING  ||  stMsgFldCfg.iFldType == UDT_BIN )
      {
         free( stDataValue.nData.pcVal );
      }
   }

   return PKG_SUCC;
}

/*
** end of file: pkg_tomidbase.c
*/
