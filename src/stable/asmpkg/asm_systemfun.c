/*
**  file  : asm_fun.c
*/
#include <stdlib.h>
#include <ctype.h>

#include "asm/asm_base.h"
#include "asm/asm_fun.h"
#include "global.h"
#include "udo.h"
#include "log.h"

/*
**  ��������: ����Դ��ֵ������Ŀ����
**  ������:   ASM_FldCopy
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_FldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
   int iRet, iFldType, iRFldType;
   int iLen;
   short   shValue;
   int     iValue;
   long    lValue;
   float   fValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ];
   int   iRowNum, iTemp, iTop, iBottom;
   int   iRetValLen = 0;
   MsgFldCfg *pstMsgFldCfg = NULL;

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   /*��������TOP��BOTTOM,Ϊ2�ֲ���׼������*/

   iRowNum = 0;
   pstMsgFldCfg = (MsgFldCfg *)SHM_ReadData ( (void *)pcShmAddr, 
                                              MSGFLDCFG, &iRowNum );
   if( pstMsgFldCfg == NULL )
   {
      LOG4C ((LOG_FATAL, "�ӹ����ڴ��ȡ����ʧ��"));
      return -1;
   }
   iTop = iRowNum - 1;
   iBottom = 0;
   iTemp = iTop/2 + iTop%2;

   while( iBottom <= iTop )
   {
      if( (*(pstMsgFldCfg + iTemp)).iMsgAppId == eiMsgAppId )
      {
         if( (*(pstMsgFldCfg + iTemp)).iFldSeq == eiFldSeq )
         {
            break;
         }
         else if( (*(pstMsgFldCfg + iTemp)).iFldSeq < eiFldSeq )
         {
            iBottom = iTemp + 1;
            iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
         }
         else
         {
            iTop = iTemp - 1;
            iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
         }
      }
      else if( (*(pstMsgFldCfg + iTemp)).iMsgAppId < eiMsgAppId )
      {
         iBottom = iTemp + 1;
         iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
      }
      else
      {
         iTop = iTemp - 1;
         iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
      }
   }
   
   if( iBottom > iTop )
   {
      LOG4C ((LOG_FATAL, "��MsgFldCfg����û���ҵ�����Ӧ�ú�%d���%d����",
              eiMsgAppId, eiFldSeq));
      return ASM_FAIL;
   }
   iRFldType = (*(pstMsgFldCfg + iTemp)).iFldType;

   memset (aczValue, 0, sizeof (aczValue));
   iRet = UDO_SafeGetField (epcSrcList, 1, 0, iRFldType, aczValue, &iLen);
   if (iRet != 0) {
       LOG4C ((LOG_FATAL, "��������ȡֵʧ�� Ret %d", iRet));
       return ASM_FAIL;
   }

   iRet = UDO_AddField (opcResultList, 1, iRFldType, aczValue, iLen);
   if (iRet != 0) {
       LOG4C ((LOG_FATAL,"�����������ӽڵ�ʧ��"));
       return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  ��������: ����Դ��ֵ�����ַ����ۼӵķ�ʽ����Ŀ����
**  ������:   ASM_AddStr
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddStr( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType, iNum;
   int iLength, iOffset=0;
   short   shValue;
   int     iValue;
   long    lValue;
   float   fValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ], aczTmpValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   memset( aczValue, 0x00, MAX_STR_NODE_LEN );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     memset( aczTmpValue, 0x00, sizeof(aczTmpValue) );
     iRet = UDO_SafeGetField (epcSrcList, i+1, 0, UDT_STRING, aczTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }
     memcpy( aczValue+iOffset, aczTmpValue, (size_t)iLength);
     iOffset += iLength;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iOffset );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  ��������: ����Դ��ֵ���ն�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddShort
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddShort( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType;
   short   shValue=0; 
   short   shTmpValue;
   int     iValue, iNum, iLength;
   long    lValue;
   float   fValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     iRet = UDO_SafeGetField(epcSrcList, i+1, 0, UDT_SHORT, (char *)&shTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }
     shValue += shTmpValue;

   }
   iRet = UDO_AddField( opcResultList, 1, UDT_SHORT,
                       (char *)&shValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  ��������: ����Դ��ֵ����������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddInt
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddInt( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                 int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType, iLength;
   short   shValue; 
   int     iValue=0;
   int     iTmpValue, iNum;
   long    lValue;
   float   fValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     iRet = UDO_SafeGetField(epcSrcList, i+1, 0, UDT_INT, (char *)&iTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }
     iValue += iTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_INT,
                       (char *)&iValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}


/*
**  ��������: ����Դ��ֵ���ճ�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddLong
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddLong( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType, iLength;
   short   shValue; 
   int     iValue, iNum;
   long    lValue=0;
   long    lTmpValue;
   float   fValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     iRet = UDO_SafeGetField(epcSrcList, i+1, 0, UDT_LONG, (char *)&lTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }
     lValue += lTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_LONG, (char *)&lValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}



/*
**  ��������: ����Դ��ֵ���ո�������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddFloat
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddFloat( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                   int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType, iLength;
   short   shValue; 
   int     iValue, iNum;
   long    lValue;
   float   fValue=0;
   float   fTmpValue;
   double  dValue;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     iRet = UDO_SafeGetField(epcSrcList, i+1, 0, UDT_FLOAT, (char *)&fTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }
    fValue += fTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_FLOAT,
                       (char *)&fValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}



/*
**  ��������: ����Դ��ֵ����˫��������ӵķ�ʽ����Ŀ����
**  ������:   ASM_AddDouble
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_AddDouble( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                    int eiFldSeq, UDP opcResultList )
{
   int iRet, i, iFldType, iLength;
   short   shValue; 
   int     iValue, iNum;
   long    lValue;
   float   fValue;
   double  dValue=0;
   double  dTmpValue;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );

   iNum = UDO_FieldCount( epcSrcList );
   for( i=0; i<iNum; i++ )
   {
     iRet = UDO_SafeGetField(epcSrcList, i+1, 0, UDT_DOUBLE, (char *)&dTmpValue,
                             &iLength);
     if (iRet != 0) {
           LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
           return ASM_FAIL;
     }

      dValue += dTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_DOUBLE, (char *)&dValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  ��������: ����Դ��ֵ����������˵ķ�ʽ����Ŀ����
**  ������:   ASM_MulData
**  �������: 1 ������Ϣ
**            2 ��Դ��ֵ��Ϣ����
**            3 ����Ӧ�ú�
**            4 ���������
**  �������: Ŀ����ֵ��Ϣ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int  ASM_MulData( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
   int iRet, iFldType;
   int iLength, iNum;
   int     iValue;
   double  dValue;
   double  dTmpValue1;
   char aczValue[ MAX_STR_NODE_LEN ];

   assert( epcSrcList != NULL );
   assert( opcResultList != NULL );
   
   iNum = UDO_FieldCount( epcSrcList );
   if( iNum != 2 )
   {
      LOG4C ((LOG_FATAL, "����Ľڵ��� %d", iNum));
      return ASM_FAIL;
   }

   iFldType = UDO_FieldType( epcSrcList, 1 );
   if( iFldType != UDT_STRING )
   {
      LOG4C ((LOG_FATAL, "����Ľڵ����� %d", iFldType));
      return ASM_FAIL;
   }
   memset( aczValue, 0x00, sizeof( aczValue ) );
   iRet = UDO_GetField( epcSrcList, 1, 0, aczValue, &iLength );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
      return ASM_FAIL;
   }
   dTmpValue1 = (double)atof( aczValue );

   iFldType = UDO_FieldType( epcSrcList, 2 );
   if( iFldType != UDT_STRING )
   {
      LOG4C ((LOG_FATAL, "����Ľڵ����� %d", iFldType));
      return ASM_FAIL;
   }
   memset( aczValue, 0x00, sizeof( aczValue ) );
   iRet = UDO_GetField( epcSrcList, 2, 0, aczValue, &iLength );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "��������ȡ�ڵ�ֵʧ��"));
      return ASM_FAIL;
   }
   iValue = (int)atoi( aczValue );
   dValue = dTmpValue1 * iValue;
   sprintf( aczValue, "%.0lf", dValue );

   iRet = UDO_AddField( opcResultList, 1, UDT_STRING,
                       aczValue, (int)strlen( aczValue ) );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}


/*
**����: ȡϵͳʱ��(HHMMSS)
**����: 1 ������Ϣ
**      2 ��Դ��ֵ��Ϣ����
**      3 ����Ӧ�ú�
**      4 ���������
**���:1 ת�����
**���� 0-�ɹ�,����-ʧ��
*/
int  ASM_SysTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
   char aczSysTime[20];

   assert( opcResultList != NULL );

   memset( aczSysTime, 0, sizeof(aczSysTime) );
   TOOL_GetSystemhhmmss( aczSysTime );
   UDO_AddField( opcResultList, 1, UDT_STRING, aczSysTime, 6 );

   return ASM_SUCC;
}

/*
**����: ȡϵͳ����(YYYYMMDD)
**����: 1 ������Ϣ
**      2 ��Դ��ֵ��Ϣ����
**      3 ����Ӧ�ú�
**      4 ���������
**���:1 ת�����
**���� 0-�ɹ�,����-ʧ��
*/
int  ASM_SysDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
   char aczSysDate[15];
   time_t tTime;
   struct tm *pstTime;

   assert( opcResultList != NULL );

   memset( aczSysDate, 0, sizeof(aczSysDate) );
   time( &tTime );
   pstTime = localtime( &tTime );
   sprintf( aczSysDate, "%04d%02d%02d",
            pstTime->tm_year + 1900,
            pstTime->tm_mon + 1,
            pstTime->tm_mday );

   UDO_AddField( opcResultList, 1, UDT_STRING, aczSysDate, 8 );

   return ASM_SUCC;
}

/*
** end of file
*/

