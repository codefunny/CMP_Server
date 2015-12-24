/*
**  file: pkg_tomsg.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "pkg/pkg_tomsg.h"
#include "getStrAlignNums.h"
#include "log.h"

extern int nestedLevel;
extern int posInStruct;
extern int structStartMoveLen;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];


/*
**  ��������: ȡ����������״����0�ź���
**
**  ����˵��: PKG_GetToMsgTranCode0
**
*  �������: UDPnepfsAddr 
**            int eiNodeId
**            int eiMsgDscrb
**            
**  �������: char ** opczTranCode
**
**  ����ֵ:   int 0 �ɹ�, ����ʧ��
**
**  ����˵��: �����ڲ������˿ռ�, �������ע���ͷ� 
*/
int  PKG_GetToMsgTranCode0( UDP epfsAddr, 
                            int eiNodeId, int eiMsgDscrb, 
                            char ** opczTranCode )
{
   int iRet, iCrtNum, iWrgNum, iDataType, iValLen;
   int iType, iSeq;
   int iVal;
   short shVal;
   long lVal;
   double dVal;
   float fVal;
   char * pczVal = NULL;
   NodeTranCfg stNodeTranCfg;

   assert( epfsAddr != NULL );

   memset( &stNodeTranCfg, 0x00, sizeof(NodeTranCfg) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMsgTranCode0) Has Error[%d]! NodeId:[%d]\n", 
                   iRet, eiNodeId)); 
      return iRet;
   }

   iCrtNum = PKG_GetNumsOfFld( stNodeTranCfg.aczCrtTranCodeType );
   iWrgNum = PKG_GetNumsOfFld( stNodeTranCfg.aczWrgTranCodeType ); 
   if( (eiMsgDscrb > 0 && iCrtNum < eiMsgDscrb) || 
       (eiMsgDscrb < 0 && iWrgNum < eiMsgDscrb) )
   {
      LOG4C ((LOG_FATAL,
                  "The Input 'eiMsgDscrb:[%d]' (in PKG_GetToMsgTranCode0) Has OverFlowed! NodeId:[%d]\n",
                   eiMsgDscrb, eiNodeId));
      return PKG_MSGDSCRB_ERR;
   }

   if( eiMsgDscrb > 0 ) /* ��ȷ�Ľ��� */
   {
      iType = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeType, eiMsgDscrb );
   }
   else /* ����Ľ��� */
   {
      iType = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeType, 
                                 abs(eiMsgDscrb) );
   }

   if( iType == PKG_NO_TRAN_CODE ) /* û�н��״������� */
   {
      do
      {
         pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
      }while( pczVal == NULL );
      memset( pczVal, 0x00, PKG_TRAN_CODE_LEN + 1 );
      strcpy( pczVal, "-100" );
      *opczTranCode = pczVal;
   }
   else
   {
      if( eiMsgDscrb > 0 )
      {
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczCrtTranCodeSeq, 
                                   eiMsgDscrb );
      }
      else
      {
         iSeq = PKG_GetValFromFld( stNodeTranCfg.aczWrgTranCodeSeq, 
                                   abs(eiMsgDscrb) );
      }
   }

   /* �����ΪiSeq������ȡ�ý��״��� */
   iDataType = UDO_FieldType( epfsAddr, iSeq );

   iRet = PKG_GetFldFromLST( epfsAddr, iSeq, 1, 0, iDataType,
                             &pczVal, &iValLen, &shVal, &iVal, &lVal, 
                             &dVal, &fVal );
   if( iRet != PKG_SUCC )
   { 
      LOG4C ((LOG_FATAL,
                  "PKG_GetFldFromLST (in PKG_GetToMsgTranCode) Has Error[%d] When Getting TranCode From A LST! NodeId:[%d], MsgDscrb:[%d], Seq:[%d]\n",
                   iRet, eiNodeId, eiMsgDscrb, iSeq));
      return iRet;
   }

   switch( iDataType )
   {
      case UDT_STRING:
         *opczTranCode = pczVal;
         break;
      case UDT_SHORT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%d", shVal );
         *opczTranCode = pczVal;
         break;
      case UDT_INT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%d", iVal );
         *opczTranCode = pczVal;
         break;
      case UDT_LONG:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%ld", lVal );
         *opczTranCode = pczVal;
         break;
      case UDT_DOUBLE:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%.0lf", dVal );
         *opczTranCode = pczVal;
         break;
      case UDT_FLOAT:
         do
         {
            pczVal = (char *)malloc( PKG_TRAN_CODE_LEN + 1 );
         }while( pczVal == NULL );
         memset( pczVal, 0x00, PKG_TRAN_CODE_LEN+1 );
         sprintf( pczVal, "%.0f", fVal );
         *opczTranCode = pczVal;
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The 'iDataType:[%d]' Of TranCodeFld (in PKG_GetToMsgTranCode) Has Error[%d]! NodeId:[%d], MsgDscrb:[%d]\n",
                      iDataType, PKG_DATATYPE_INVALID, 
                     eiNodeId, eiMsgDscrb));
         return PKG_DATATYPE_INVALID;
   }

   return PKG_SUCC;
}

/*
**  ��������: ���м���Ϣ����ת��Ϊ������ĵ�0�ź���
**
**  ����˵��: PKG_ToMsg0
**
**  �������: int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**            MsgFldCfgAry * epstMsgFldCfgAry
**            UDP  epfsAddr 
**
**  �������: char * opcOutMsg
**            int  * opiOutMsgLen
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
*/

int PKG_ToMsg0( char * opcOutMsg, int *opiOutMsgLen, 
                int eiNodeId, char * epczTranCode, int eiMsgDscrb, 
                MsgFldCfgAry *epstMsgFldCfgAry , UDP epfsAddr )
{
   int iRet;
   int a=0,i,m,n,x,y,s,t;
   int iSeq, iSeqNum = 0;
   int iMoveLen = 0; /* ����ָ����ƫ���� */
   int iRepTimesVal, iRepTimesSeqNums, iRepTimesNumNums, iRepTypeNums; 
   int iTranFldNums, iRepFldNums;
   int iRepTimesSeq[PKG_REP_PARA_LEN], iRepTimesNum[PKG_REP_PARA_LEN],
       iRepType[PKG_REP_PARA_LEN];
   int iSepSymbolLen;
   char aczRepFld[PKG_TRAN_FLD_LEN];
   TranFldCfg stTranFldCfg;
   FldCfgValNode stFldCfgValNode ;
   NodeTranCfg stNodeTranCfg;
   MsgFldCfgAry stMsgFldCfgAry;
   /** �ٶ�������ɲ�����200���� **/
   int iTranFlds[1024];

   /** �����ظ�����Ϊ0�Ĵ��� **/
   int iRepFieldId;

   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epstMsgFldCfgAry != NULL );
   assert( epfsAddr != NULL );



   nestedLevel = -1;
   iToMsgIsSt = 0;
   memset (alignStructNums, 0, sizeof (alignStructNums));
   memset (structNums,      0, sizeof (structNums));

   memset (iTranFlds,       0, sizeof (iTranFlds));

   memset( aczRepFld, 0x00, PKG_TRAN_FLD_LEN );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );
   memset( &stFldCfgValNode, 0x00, sizeof(FldCfgValNode) );
   memset( &stMsgFldCfgAry, 0x00, sizeof(MsgFldCfgAry) );
   memset( &stNodeTranCfg, 0x00, sizeof(stNodeTranCfg) );

   *opiOutMsgLen = 0;

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
       LOG4C ((LOG_FATAL,
                   "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMidTran"
                   "Code0) Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
       return iRet;
   }

   /* ���ݽڵ��,���״���,����������,����TranFldCfg,ȡ�ö�Ӧ�ļ�¼ */
   iRet = PKG_GetInfoFromTranFldCfg( eiNodeId, epczTranCode, 
                                     eiMsgDscrb, &stTranFldCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                "PKG_GetInfoFromTranFldCfg (in PKG_ToMsg0) Has "
                "Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                 iRet, eiNodeId, epczTranCode, eiMsgDscrb));
      return iRet;
   }


   {
      char aczRepTimeFld[20];
      int i, len;
      strcpy( aczRepTimeFld, stTranFldCfg.aczRepTimesSeq );
      len = strlen(aczRepTimeFld);
      for( i = 0; i < len; i++ )
      {
         if( aczRepTimeFld[i] == ',' )
         {
            aczRepTimeFld[i] = 0;
            break;
         }
      }
      iRepFieldId = atoi(aczRepTimeFld);
   }

   /* ���ظ�������α������е�ֵһһ��Ӧ������iRepTimesSeq�� */

   iRepTimesSeqNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepTimesSeq );
   for( m = 0; m < iRepTimesSeqNums; m++ )
   {
      iRepTimesSeq[m] = PKG_GetValFromFld( stTranFldCfg.aczRepTimesSeq, m+1 );
   }
   m = 0;


   /* ���ظ������������е�ֵһһ��Ӧ������iRepTimesNum�� */
   iRepTimesNumNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepTimesNum );
   for( m = 0; m < iRepTimesNumNums; m++ )
   {
      iRepTimesNum[m] = PKG_GetValFromFld( stTranFldCfg.aczRepTimesNum, m+1 );
   }
   m = 0;

   /* ���ظ�����ظ���������е�ֵһһ��Ӧ������iRepType�� */
   iRepTypeNums = PKG_GetNumsOfFld( stTranFldCfg.aczRepType );
   for( n = 0; n < iRepTypeNums; n++ )
   {
      iRepType[n] = PKG_GetValFromFld( stTranFldCfg.aczRepType, n+1 );
   }
   n = 0;

   /* ���㽻������е��������(����-1) */

   iTranFldNums = PKG_GetNumsOfFld( stTranFldCfg.aczTranFld );
   
   getTranFlds (iTranFldNums, stTranFldCfg.aczTranFld, iTranFlds);

   getStructsAlignNums (iTranFldNums, iTranFlds, 
                        stNodeTranCfg.iMsgAppId,
                        epstMsgFldCfgAry,
                        alignStructNums);

   /* ��ÿһ������в���, ����Ӧֵ�������� */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      iSeq = iTranFlds[i-1];

      if( iSeq != PKG_REP_SYMBOL )  /* ���ظ��� */ 
      {
         /* �жϸ�����Ƿ�Ϊ�ظ��򣬸����ظ�������ŵõ�iSepNum */
         if( iSeq != iRepTimesSeq[m] ) 
         {
            iSeqNum = 0;
         } 
         else
         {
            iSeqNum = 0;
            /* �ж��ڸ��ظ����֮ǰ�Ƿ���������ͬ����� */
            for( a = 0; a < m; a++ )
            {
               if( iRepTimesSeq[a] == iSeq )
                  iSeqNum++;
            }
            m++;
         }

         /* �����������д��������� */ 
         iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, &iMoveLen,
                                          stNodeTranCfg.iMsgAppId, iSeq,
                                          iSeqNum, epstMsgFldCfgAry, 
					  epfsAddr ); 
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL, "PKG_WriteFldContentToMsg (in PKG_"
                      "ToMsg0) Has Error[%d] When Getting Content From RepTim"
                      "esFld! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:"
                      "[%d], Num:[%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, iSeq, iSeqNum));
            return iRet;
         } 
      }
      else /* �ظ��� */
      {
         /* ȡ���ظ����еı�ż��� */
         iRet = PKG_GetRepFld( stTranFldCfg.aczTranFld, i, aczRepFld );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                      "PKG_GetRepFld (in PKG_ToMsg0) Has Error[%d]!"
                      " NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], The Start "
                      "Num Of TranFld Is [%d]\n",
                       iRet, eiNodeId, epczTranCode,
                      eiMsgDscrb, i+1));
            return iRet;
         }

         iRepFldNums = PKG_GetNumsOfFld( aczRepFld );
         i += iRepFldNums + 1;   /* ��iֵ��Ӧ����һ��-1���ڵ�λ�� */

         /* ������ظ�������ظ����� */
         iSeq = PKG_GetValFromFld( aczRepFld, 1 );
/***�滻Ϊ����һ��***
         iRepTimesVal = UDO_FieldOccur( epfsAddr, iSeq );
         if( iRepTimesVal == PKG_NO_FIELD )
         {
            LOG4C ((LOG_FATAL,
                        "UDO_FieldOccur (in PKG_ToMsg) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d]\n",
                         PKG_NO_FIELD, eiNodeId,
                        epczTranCode, eiMsgDscrb, iSeq));
            return PKG_NO_FIELD;
         }
*********************/

/**�޸�ȡ�ظ������������ ����һ��ѭ��������� **/
         {
         char aczVal[50];
         memset( aczVal, 0, sizeof(aczVal) );
         UDO_GetField( epfsAddr, iRepTimesSeq[n], 0, aczVal, 0 );
         iRepTimesVal = atoi(aczVal);
         }


         if( iRepType[n] == PKG_CYC_REP )  /* ѭ���ظ� */
         {
            for( x = 1; x <= iRepTimesVal; x++ )
            {
               for( y = 1; y <= iRepFldNums; y++ )
               {
                  iSeq = PKG_GetValFromFld( aczRepFld, y ); 

                  /* �����ظ������Ϣд��������� */
                  iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId,
                                                   iSeq, x-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr ); 
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg (in PKG_ToMsg0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, x-1));
                     return iRet;
                  } 
               }
            }
         }
         else /* ����ѭ�� */
         {
            for( s = 1; s <= iRepFldNums; s++ )
            {
               for( t = 1; t <= iRepTimesVal; t++ )
               {
                  iSeq = PKG_GetValFromFld( aczRepFld, s ); 
    
                  /* �����ظ������Ϣд��������� */
                  iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, 
                                                   &iMoveLen,
                                                   stNodeTranCfg.iMsgAppId, 
                                                   iSeq, t-1, 
                                                   epstMsgFldCfgAry ,
                                                   epfsAddr );
                  if( iRet != PKG_SUCC )
                  {
                     LOG4C ((LOG_FATAL,
                                 "PKG_WriteFldContentToMsg (in PKG_ToMsg0) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d], Num:[%d]\n",
                                  iRet, eiNodeId, 
                                 epczTranCode, eiMsgDscrb, iSeq, t-1));
                     return iRet;
                  } 
               }
            }
         }
         n++;
      }
   }


   if (iSeq < 0)
   {
       goto tomsg_end;
   }

   /* ����ñ���Ϊ2�ŷָ�����Ҳ�ı���, ���´������һ�������� */
   iRet = PKG_GetInfoByFldSeq ( epstMsgFldCfgAry, stNodeTranCfg.iMsgAppId,
                                iSeq , &stMsgFldCfgAry ); 
   if ( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_ToMsg0  Has Error[%d], There Is No FldCfgNode --- Seq:[%d]\n",
                   PKG_NO_FLDCFGNODE, iSeq));
      return PKG_NO_FLDCFGNODE;
   }


   iRet = PKG_GetFldCfgValNode ( &stMsgFldCfgAry , 
                                 stNodeTranCfg.iMsgAppId,
                                 iSeq, stMsgFldCfgAry.iNum , &stFldCfgValNode );


   if( stFldCfgValNode.iSepFldFlag == PKG_SEPSYMBOL2_FLD ) 
   {
      iSepSymbolLen = (strlen(stFldCfgValNode.aczSepSymbol))/2; 
      /* ���ĳ��ȼ�ȥ���һ���ָ���ĳ��� */
      iMoveLen = iMoveLen - iSepSymbolLen;  
      memset(opcOutMsg+iMoveLen, 0x00, (size_t)iSepSymbolLen ); 
   }


   /* �����������ǽṹ��, iMoveLenӦ����4�ı��� */
   /* not use now ******
   if( iToMsgIsSt == PKG_IS_STRUCT )
   {
      iMoveLen += ( PKG_MOD - iMoveLen % PKG_MOD ) % PKG_MOD;
   }
   *********************/

tomsg_end:

   *opiOutMsgLen = iMoveLen;


   return PKG_SUCC;
}

/*
**  ��������: ���м���Ϣ����ת��Ϊ������ĵ�1�ź���(��Ա�׼8583)
**
**  ����˵��: PKG_ToMsg1
**
**  �������: int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**            MsgFldCfg * epstMsgFldCfg
**            int eiRowNum
**            UDP  epfsAddr
**
**  �������: char * opcOutMsg
**            int  * opiOutMsgLen
**
**  ����ֵ:   int  0�ɹ�, ����ʧ��
*/

int PKG_ToMsg1( char * opcOutMsg, int *opiOutMsgLen,
                int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr )
{
   int i, iRet, iSeq, iLen, iBitMapLen=0;
   int iMoveLen = 0; /* ����ָ����ƫ���� */
   int iTranFldNums;
   char aczHead[PKG_MSGTYPE_LEN + 1];
   char aczBitMap[PKG_BITMAP_MAX + 1];
   TranFldCfg stTranFldCfg;
   NodeTranCfg stNodeTranCfg;

   assert( epstMsgFldCfgAry != NULL );
   assert( opcOutMsg != NULL );
   assert( epczTranCode != NULL );
   assert( epfsAddr != NULL );

   memset( aczHead, 0x00, PKG_MSGTYPE_LEN+1 );
   memset( aczBitMap, 0x00, PKG_BITMAP_MAX+1 );
   memset( &stTranFldCfg, 0x00, sizeof(TranFldCfg) );

   *opiOutMsgLen = 0;

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromNodeTranCfg (in PKG_GetToMidTranCode0) Has Error[%d]! NodeId:[%d]\n",
                    iRet, eiNodeId));
      return iRet;
   }

   /* ���ݽڵ��,���״���,����������,����TranFldCfg,ȡ�ö�Ӧ�ļ�¼ */
   iRet = PKG_GetInfoFromTranFldCfg( eiNodeId, epczTranCode,
                                     eiMsgDscrb, &stTranFldCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromTranFldCfg(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                   iRet, eiNodeId, epczTranCode, 
                  eiMsgDscrb)); 
      return iRet;
   }

   /* ������������д��������� */
   iRet = UDO_GetField( epfsAddr, 0, 0, aczHead, &iLen );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,  
                  "UDO_GetField(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n", 
                   iRet, eiNodeId, epczTranCode, 
                  eiMsgDscrb));
      return iRet;
   }
   
   /**
   {
      char c[2];
      c[0] = aczHead[2];
      c[1] = '\0';
 
      if (atoi (c) % 2 == 0)
          aczHead[2] += 1;
   }
   */

   memcpy( opcOutMsg, aczHead, PKG_MSGTYPE_LEN );
   iMoveLen += PKG_MSGTYPE_LEN;

   /* ���㽻������е��������(����-1) */
   iTranFldNums = PKG_GetNumsOfFld( stTranFldCfg.aczTranFld );

   /* �����������BitMap�ĳ��� */
   for( i = 1; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );
      if( iSeq > 64 )
      {
         iRet = PKG_SetBitMap( (unsigned char *)aczBitMap, 1 );
         if( iRet != PKG_SUCC )
         {
            LOG4C ((LOG_FATAL,
                        "PKG_SetBitMap(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",
                         iRet, 
                        eiNodeId, epczTranCode, eiMsgDscrb));
            return iRet;
         }
         iBitMapLen = 16;
         break;
      }
   }
   iBitMapLen = (iBitMapLen == 16)?16:8;

   /* ����������ĵ�BitMap */
   for( i = 3; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );
      iRet = PKG_SetBitMap( (unsigned char *)aczBitMap, iSeq );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_SetBitMap(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d]\n",  iRet, 
                      eiNodeId, epczTranCode, eiMsgDscrb));

         return iRet;
      }
   }

   memcpy( opcOutMsg+iMoveLen, aczBitMap, (size_t)iBitMapLen );
   iMoveLen += iBitMapLen;


   /* ��ÿһ������в���, ����Ӧֵ�������� */
   for( i = 3; i <= iTranFldNums; i++ )
   {
      iSeq = PKG_GetValFromFld( stTranFldCfg.aczTranFld, i );

      /* �����������д��������� */
      iRet = PKG_WriteFldContentToMsg( opcOutMsg+iMoveLen, &iMoveLen,
                                       stNodeTranCfg.iMsgAppId, iSeq, 0,
                                       epstMsgFldCfgAry, 
				       epfsAddr );
      if( iRet != PKG_SUCC )
      {
         LOG4C ((LOG_FATAL,
                     "PKG_WriteFldContentToMsg(in PKG_ToMsg1) Has Error[%d]! NodeId:[%d], TranCode:[%s], MsgDscrb:[%d], Seq:[%d]\n", 
                     iRet, eiNodeId, epczTranCode,
                    eiMsgDscrb, iSeq));

         return iRet;
      }
   }

   *opiOutMsgLen = iMoveLen;

   return PKG_SUCC;
}

/*
** end of file: pkg_tomsg.c
*/
