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
**  函数功能: 将来源域值拷贝到目标域
**  函数名:   ASM_FldCopy
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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

   /*首先设置TOP和BOTTOM,为2分查找准备数据*/

   iRowNum = 0;
   pstMsgFldCfg = (MsgFldCfg *)SHM_ReadData ( (void *)pcShmAddr, 
                                              MSGFLDCFG, &iRowNum );
   if( pstMsgFldCfg == NULL )
   {
      LOG4C ((LOG_FATAL, "从共享内存读取数据失败"));
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
      LOG4C ((LOG_FATAL, "在MsgFldCfg表中没有找到报文应用号%d域号%d的域",
              eiMsgAppId, eiFldSeq));
      return ASM_FAIL;
   }
   iRFldType = (*(pstMsgFldCfg + iTemp)).iFldType;

   memset (aczValue, 0, sizeof (aczValue));
   iRet = UDO_SafeGetField (epcSrcList, 1, 0, iRFldType, aczValue, &iLen);
   if (iRet != 0) {
       LOG4C ((LOG_FATAL, "从链表中取值失败 Ret %d", iRet));
       return ASM_FAIL;
   }

   iRet = UDO_AddField (opcResultList, 1, iRFldType, aczValue, iLen);
   if (iRet != 0) {
       LOG4C ((LOG_FATAL,"向链表中增加节点失败"));
       return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  函数功能: 将来源域值按照字符串累加的方式生成目标域
**  函数名:   ASM_AddStr
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }
     memcpy( aczValue+iOffset, aczTmpValue, (size_t)iLength);
     iOffset += iLength;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iOffset );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  函数功能: 将来源域值按照短整形相加的方式生成目标域
**  函数名:   ASM_AddShort
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }
     shValue += shTmpValue;

   }
   iRet = UDO_AddField( opcResultList, 1, UDT_SHORT,
                       (char *)&shValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  函数功能: 将来源域值按照整形相加的方式生成目标域
**  函数名:   ASM_AddInt
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }
     iValue += iTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_INT,
                       (char *)&iValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}


/*
**  函数功能: 将来源域值按照长整形相加的方式生成目标域
**  函数名:   ASM_AddLong
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }
     lValue += lTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_LONG, (char *)&lValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}



/*
**  函数功能: 将来源域值按照浮点形相加的方式生成目标域
**  函数名:   ASM_AddFloat
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }
    fValue += fTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_FLOAT,
                       (char *)&fValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}



/*
**  函数功能: 将来源域值按照双精度型相加的方式生成目标域
**  函数名:   ASM_AddDouble
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
           LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
           return ASM_FAIL;
     }

      dValue += dTmpValue;
   }
   iRet = UDO_AddField( opcResultList, 1, UDT_DOUBLE, (char *)&dValue, 0 );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}

/*
**  函数功能: 将来源域值按照整型相乘的方式生成目标域
**  函数名:   ASM_MulData
**  输入参数: 1 服务信息
**            2 来源域值信息链表
**            3 报文应用号
**            4 报文域序号
**  输出参数: 目标域值信息链表
**  返回值:   int  0 成功, 其他失败
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
      LOG4C ((LOG_FATAL, "错误的节点数 %d", iNum));
      return ASM_FAIL;
   }

   iFldType = UDO_FieldType( epcSrcList, 1 );
   if( iFldType != UDT_STRING )
   {
      LOG4C ((LOG_FATAL, "错误的节点类型 %d", iFldType));
      return ASM_FAIL;
   }
   memset( aczValue, 0x00, sizeof( aczValue ) );
   iRet = UDO_GetField( epcSrcList, 1, 0, aczValue, &iLength );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
      return ASM_FAIL;
   }
   dTmpValue1 = (double)atof( aczValue );

   iFldType = UDO_FieldType( epcSrcList, 2 );
   if( iFldType != UDT_STRING )
   {
      LOG4C ((LOG_FATAL, "错误的节点类型 %d", iFldType));
      return ASM_FAIL;
   }
   memset( aczValue, 0x00, sizeof( aczValue ) );
   iRet = UDO_GetField( epcSrcList, 2, 0, aczValue, &iLength );
   if( iRet != 0 )
   {
      LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
      return ASM_FAIL;
   }
   iValue = (int)atoi( aczValue );
   dValue = dTmpValue1 * iValue;
   sprintf( aczValue, "%.0lf", dValue );

   iRet = UDO_AddField( opcResultList, 1, UDT_STRING,
                       aczValue, (int)strlen( aczValue ) );
   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
      return ASM_FAIL;
   }
   return ASM_SUCC;
}


/*
**功能: 取系统时间(HHMMSS)
**输入: 1 服务信息
**      2 来源域值信息链表
**      3 报文应用号
**      4 报文域序号
**输出:1 转换结果
**返回 0-成功,其他-失败
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
**功能: 取系统日期(YYYYMMDD)
**输入: 1 服务信息
**      2 来源域值信息链表
**      3 报文应用号
**      4 报文域序号
**输出:1 转换结果
**返回 0-成功,其他-失败
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

