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
**  ��������: ��ȡBitMap���ض�λ,�����Ƿ�Ϊ1
**
**  ��������: PKG_IsBitMapSet
**
**  �������: unsigned char * epczBitMap
**int eiNum
**
**  �������: ��
**
**  ����ֵ:   0 --��, -1 -- ��, ��������
*/
int PKG_IsBitMapSet( unsigned char * eupczBitMap, int eiNum )
{
   int iByteNum, iBitNum, iBitMoved;
   char cTheBitChar;

   assert( eupczBitMap != NULL );

   if( eiNum < 1 || eiNum > PKG_BITMAP_MAX )
      return PKG_BITMAP_OVERFLOW;

   /* eiNum�ڸ�BitMap�ĵ�iByteNum���ֽ� */
   iByteNum = ( eiNum - 1 ) / 8;

   /* eiNum�������ֽڵĵ�iBitNumλ */
   iBitNum  = ( eiNum - 1 ) % 8;

   /* ��һ���ֽ���, 0x01��Ҫ����iBitMapλ��eiNumλ���� */
   iBitMoved = 7 - iBitNum;

   cTheBitChar = eupczBitMap[ iByteNum ];   /* eiNumλ���ķ���(1��0) */

   if( ((0x01 << iBitMoved) & cTheBitChar) != 0 )
      return PKG_SUCC;
   else
      return PKG_FAIL;
}


/*
**  ��������: ����һ���ظ�������ظ����� 
**
**  ������:   PKG_GetRepTimesVal
**
**  �������: UDP        epfsAddr          // �м���Ϣ������ʼ��ַ
**            int        eiRepFldNum      // �ظ��������(��0��ʼ)  
**            char     * epczRepTimesSeq  // �ظ���������
**            char     * epczRepTimesNum  // �ظ��������� 
** 
**  �������: int      * opiRepTimesVal   // �ظ�����
**
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
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

   /* ȡ�ø������������ */
   iDataType = UDO_FieldType( epfsAddr, iSeq );

   /* �Ӹ�����ȡ���ظ����� */
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
**  ��������: �����������ʹ��ַ�����ȡֵ 
**
**  ������:   PKG_GetValFromBuf
**
**  �������: char * epcBuf            // �ַ���
**            int  eiDataType          // ��������
**            int eiValLen      // �ַ�����Ч����
**            
**  �������: char ** opczVal          // �ַ�������
**            short * opshVal          // ������
**            int * opiVal             // ����ֵ
**            long * oplVal            // ������ֵ
**            double * opdVal          // ˫����ֵ
**            float * opfVal           // ������ֵ
**            int * opiValContentLen   // ֵ���ݵĳ���
**
**  ����ֵ: int  0 �ɹ�, ����ʧ��
**
**  ����˵��: �����ڲ������˿ռ�, �������ע���ͷ�
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
**  ��������: ���������Ա�־, ȡ�ò�ͬ�����ֵ
**
**  ������:   PKG_GetSepFldContentFromMsg
**
**  �������: char * epcMsgPos
**            int * eopiMoveLen                     // ָ����뱨��ͷ�ĳ���
**            int  * eopiMsgLeftLen                 // ָ����뱨�Ľ�β�ĳ���
**            FldCfgValNode estFldCfgValNode
**
**  �������: char ** opczVal
**            int  * opiValLen
**            short * opshVal
**            int  * opiVal
**            long * oplVal
**            double * opdVal
**            float * opfVal
**            int * opiSepMoveLen                   // ָ�����÷���ͷ�ĳ���
**            int * eopiMoveLen                     // ָ����뱨��ͷ�ĳ���
**            int  * eopiMsgLeftLen                 // ָ����뱨�Ľ�β�ĳ���
**
**  ����ֵ: int  0 �ɹ�, ����ʧ�� 
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
         /* �������ı���Ϊ�ṹ��,������������֮ǰ��ָ��ƫ���� */ 
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
         /* ������뱨��Ϊ�ṹ��, ���㳤����֮ǰ��ָ��ƫ���� */
         if( iToMidIsSt == PKG_IS_STRUCT )
         { 
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iLenFldType, 
                                     &iPtrMoveLen );
         }

         /* �ӳ�������ȡ�õ�ʵ������ */ 
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

         /* iStrMoveLenֻ�е�������Ϊ�ַ������ж���ʱ������ */
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

         /* ���㳤����ĳ��� */
         if( epstMsgFldCfg->iLenFldType == UDT_STRING ||  epstMsgFldCfg->iLenFldType == UDT_BIN )
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;
         }
         else
         {
            iLenFldLen = opstDataValue->iLen;
         }

         /* �����������ʵ�ʳ��� */
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


         /* �Գ����������Ϻ�, ����÷����ָ��ƫ�����Լ�Դ���ĵ�ָ��ƫ���� */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         epcMsgPos += *opiSepMoveLen; 

         /* ������뱨��Ϊ�ṹ��, ����������֮ǰ��ָ��ƫ���� */
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen, 
                                     epstMsgFldCfg->iFldType,
                                     &iPtrMoveLen );
         }

         /* �ӱ�����ȡ��ʵ�ʵ����� */
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
         /* �������ı���Ϊ�ṹ��,�����ڳ�����֮ǰ��ָ��ƫ���� */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen, 
                                     epstMsgFldCfg->iLenFldType, 
                                     &iPtrMoveLen );
         }

         /* �ӳ�������ȡ��ʵ�����ݵĳ��� */ 
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

         /* iStrMoveLenֻ�е�������Ϊ�ַ������ж���ʱ������ */
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

         /* ���㳤����ĳ��� */
         if( epstMsgFldCfg->iLenFldType == UDT_STRING || epstMsgFldCfg->iLenFldType == UDT_BIN)
         {
            iLenFldLen = epstMsgFldCfg->iLenFldLen;
         }
         else
         {
            iLenFldLen = opstDataValue->iLen;
         }

         /* �������ݵ�ʵ�ʳ��� */
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

         /* �Գ����������Ϻ�, ����÷����ָ��ƫ�����Լ�Դ���ĵ�ָ��ƫ���� */
         *opiSepMoveLen = iPtrMoveLen + iLenFldLen;
         epcMsgPos += *opiSepMoveLen;


         /* �������ı���Ϊ�ṹ��,������������֮ǰ��ָ��ƫ���� */ 
         if( iToMidIsSt == PKG_IS_STRUCT )
         {
            PKG_GetMoveLenForStruct( *eopiMoveLen+*opiSepMoveLen, 
                                     epstMsgFldCfg->iFldType, 
                                     &iPtrMoveLen );
         }
         
         /* �ӱ�����ȡ��ʵ������ */
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
         /* �ж��ַ����Ƿ��������� */
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

         /* ȡ������ε�ʵ������ */
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
         /* ȫ�������Ҳ�Ϊ�ָ���ķ�ʽ������ */
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


         /* �������ı���Ϊ�ṹ��,������������֮ǰ��ָ��ƫ���� */ 
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

         /* �ж��Ƿ�Ϊ���һ����� */
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
**  ��������: ��eiSeq�����eiSepSeq�ŷ����ֵ�����Ͳ�������(Ŀǰ��֧�ַ���)
**
**  ������:   PKG_AddSepFldContentToLST
**
**  �������: int  eiSeq          // ���
**            int  eiSepSeq       // �����
**            int  eiDataType     // ��������
**            char * epczVal      // �ַ���ֵ
**            int  eiValLen        // �ַ���ֵ����
**            short eshVal         // ������
**            int  eiVal           // ����ֵ
**            long elVal           // ������ֵ
**            double edVal         // ˫����ֵ 
**            float efVal          // ������ֵ
**
**  �������: UDP opfsAddr 
**
**  ����ֵ: int 0 �ɹ�,����ʧ��
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
**  ��������: ��eiSeq������Ϣ��������
**
**  ������:  PKG_AddFldContentToLST
**
**  �������: char * epcMsgPos            // �Ѿ�ָ��ָ��eiFldSeq�����׵�Դ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int  * eopiMsgLeftLen       // ����ָ����β���ĵĳ���
**            int    eiSeq                // ���
**            MsgFldCfgAry *epstMsgFldCfgAry    // ��ռ���ʼ��ַ
**                                            �����¼��
**
**  �������: UDP opfsAddr      // �м���Ϣ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int * eopiMsgLeftLen
**
**  ����ֵ:   int 0�ɹ�, ����ʧ�� 
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

      /* ��������������, ȡ���������, �����ͷֱ���� */
      /* �ú����е�iSepMoveLenֻ�е�һ�����з���ʱ��������,�˴ο���û���� */
      iRet = PKG_GetSepFldContentFromMsg( epcMsgPos+iSepMoveLen, 
                                          eopiMoveLen,
                                          eopiMsgLeftLen, &stMsgFldCfg, 
                                          &stDataValue, &iSepMoveLen );
      if( iRet == PKG_IS_BITMAP )
      {
         /* �ж�ָ��ƫ�����Ƿ񳬹������ܳ��� */
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

      /* �ж�ָ��ƫ�����Ƿ񳬹������ܳ��� */
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

      /* ����������ݰ����Ͳ������� */
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

      /* �ͷſռ� */
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
**  ��������: ��eiSeq������Ϣ��������
**
**  ������:  PKG_AddFldContentToLST
**
**  �������: char * epcMsgPos            // �Ѿ�ָ��ָ��eiFldSeq�����׵�Դ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int  * eopiMsgLeftLen       // ����ָ����β���ĵĳ���
**            int    eiSeq                // ���
**            MsgFldCfgAry *epstMsgFldCfgAry    // ��ռ���ʼ��ַ
**                                            �����¼��
**
**  �������: UDP opfsAddr      // �м���Ϣ����
**            int  * eopiMoveLen          // ָ��ӱ���ͷ����ƫ����
**            int * eopiMsgLeftLen
**
**  ����ֵ:   int 0�ɹ�, ����ʧ�� 
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

      /* ��������������, ȡ���������, �����ͷֱ���� */
      /* �ú����е�iSepMoveLenֻ�е�һ�����з���ʱ��������,�˴ο���û���� */
      iRet = PKG_GetSepFldContentFromMsg( epcMsgPos+iSepMoveLen, 
                                          eopiMoveLen,
                                          eopiMsgLeftLen, &stMsgFldCfg, 
                                          &stDataValue, &iSepMoveLen );
      if( iRet == PKG_IS_BITMAP )
      {
         /* �ж�ָ��ƫ�����Ƿ񳬹������ܳ��� */
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

      /* �ж�ָ��ƫ�����Ƿ񳬹������ܳ��� */
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

      /* ����������ݰ����Ͳ������� */
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

      /* �ͷſռ� */
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
