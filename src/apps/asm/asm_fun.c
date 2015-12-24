#include "udo.h"
#include "log.h"
#include "svc_base.h"
#include "tool_base.h"
#include "log.h"
#include "global.h"
#include "transcode.h"
#include "asm_opercode.h"
#include "asm_base.h"

#include <stdlib.h>
#include <string.h>

/**
 * 这是一个字段重组的例子
 * 参数说明：
 * 1 epstService   用于访问整个服务的任意数据,一般不使用
 * 2 epcSrcList    是重组来源,将来源字段从哪来的问题屏蔽掉.域号以分域的顺序编号
 * 3 eiMsgAppId    目标域的字典编号
 * 4 eiFldSeq      目标域的字段编号
 * 5 opcResultList 重组以后的结果输出变量
 */
int ASM_Sample( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                int eiFldSeq, UDP opcResultList )
{
    char aczFld[20];
    int  iLen, i;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
    for( i = 0; i < iLen; i++ )
    {
        if( aczFld[i] >= 'a' && aczFld[i] <= 'z' )
        {
            aczFld[i] = aczFld[i] - 'a' + 'A';
        }
    }

    UDO_AddField( opcResultList, 1, UDT_STRING, aczFld, iLen );

    return 0;
}

/**
 * 用于日期转换 YYYY-MM-DD To YYYYMMDD
 * 参数说明：
 *
 * epsSrcList    输入日期 YYYY-MM-DD
 * opcResultList 输出日期 YYYYMMDD
 */
int ASM_ChangeDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                    int eiFldSeq, UDP opcResultList )
{
    char aczInDate[10+1];
    char aczOutDate[8+1];
    int  iRet, iLen, i;

    memset( aczInDate, 0, sizeof(aczInDate) );
    memset( aczOutDate, 0, sizeof(aczOutDate) );

    iRet = UDO_GetField( epcSrcList, 1, 0, aczInDate, &iLen );
    if ( iRet != 0 )
    {
        return -1;
    }
    memcpy( aczOutDate, aczInDate, 4 );
    memcpy( aczOutDate + 4, aczInDate + 5, 2 );
    memcpy( aczOutDate + 6, aczInDate + 8, 2 );

    iLen = 8;
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczOutDate, iLen );

    return 0;
}

/**
 * 用于时间转换 HH.MM.SS To HHMMSS
 * 参数说明：
 *
 * epsSrcList    输入时间 HH.MM.SS
 * opcResultList 输出时间 HHMMSS
 */
int ASM_ChangeTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                    int eiFldSeq, UDP opcResultList )
{
    char aczInTime[8+1];
    char aczOutTime[6+1];
    int  iRet, iLen;

    memset( aczInTime, 0, sizeof(aczInTime) );
    memset( aczOutTime, 0, sizeof(aczOutTime) );

    iRet = UDO_GetField( epcSrcList, 1, 0, aczInTime, &iLen );
    if ( iRet != 0 )
    {
        return -1;
    }
    memcpy( aczOutTime, aczInTime, 2 );
    memcpy( aczOutTime + 2, aczInTime + 3, 2 );
    memcpy( aczOutTime + 4, aczInTime + 6, 2 );

    iLen = 6;
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczOutTime, iLen );

    return 0;
}

/**
 * 用于日期转换 YYYYMMDD To YYYY-MM-DD
 * 参数说明：
 *
 * epsSrcList    输入日期 YYYYMMDD
 * opcResultList 输出日期 YYYY-MM-DD
 */
int ASM_ChangeDate2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                     int eiFldSeq, UDP opcResultList )
{
    char aczInDate[8+1];
    char aczOutDate[10+1];
    int  iRet, iLen, i;

    memset( aczInDate, 0, sizeof(aczInDate) );
    memset( aczOutDate, 0, sizeof(aczOutDate) );

    iRet = UDO_GetField( epcSrcList, 1, 0, aczInDate, &iLen );
    if ( iRet != 0 )
    {
        return -1;
    }
    if( strlen( aczInDate ) != 0 )
    {
        memcpy( aczOutDate, aczInDate, 4 );
        memcpy( aczOutDate + 4, "-", 1 );
        memcpy( aczOutDate + 5, aczInDate + 4, 2 );
        memcpy( aczOutDate + 7, "-", 1 );
        memcpy( aczOutDate + 8, aczInDate + 6, 2 );
    }

    iLen = 10;
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczOutDate, iLen );

    return 0;
}

int ASM_GetSerial( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList )
{
    char aczSerialNo[12+1];
    int  iRet, iLen;
    char aczSerialNoNew[ 12+1 ];

    memset( aczSerialNo, 0, sizeof(aczSerialNo) );
    memset( aczSerialNoNew, 0, sizeof(aczSerialNoNew) );

    getString( "agserial", aczSerialNo );

    sprintf( aczSerialNoNew,"%d",atol( aczSerialNo ) );

    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczSerialNoNew, strlen(aczSerialNoNew) );

    return 0;
}

/**
 * 获取中间业务平台账务日期
 * 参数说明：
 * epsSrcList    无
 * opcResultList 平台账务日期
 */
int ASM_GetAgTranDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    char aczTranDate[8+1];
    int  iRet, iLen;

    getString( "agtxdate", aczTranDate );

    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczTranDate, 8 );

    return 0;
}

/**
 * 用于时间转换 HH.MM.SS To HH:MM:SS
 * 参数说明：
 *
 * epsSrcList    输入时间 HH.MM.SS
 * opcResultList 输出时间 HH:MM:SS
 */
int ASM_ChangeTime2( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                     int eiFldSeq, UDP opcResultList )
{
    char aczInTime[8+1];
    char aczOutTime[8+1];
    int  iRet, iLen;

    memset( aczInTime, 0, sizeof(aczInTime) );
    memset( aczOutTime, 0, sizeof(aczOutTime) );

    iRet = UDO_GetField( epcSrcList, 1, 0, aczInTime, &iLen );
    if ( iRet != 0 )
    {
        return -1;
    }
    memcpy( aczOutTime, aczInTime, 2 );
    memcpy( aczOutTime + 2, ":",1 );
    memcpy( aczOutTime + 3, aczInTime + 3, 2 );
    memcpy( aczOutTime + 5, ":",1 );
    memcpy( aczOutTime + 6, aczInTime + 6, 2 );

    iLen = 8;
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczOutTime, iLen );

    return 0;
}


int ASM_AscToBcd( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[2048];
    char aczBcd[1024];
    int  iLen, i, j;
    char cTmp;

    memset( aczFld, 0, sizeof(aczFld) );
    memset( aczBcd, 0, sizeof(aczBcd) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );

    for (i=0,j=0,cTmp=0; i<iLen; i++)
    {
        if( aczFld[i] >= 'a' && aczFld[i] <= 'f' )
        {
            cTmp = aczFld[i] - 'a' + 10;
        }
        else if( aczFld[i] >= 'A' && aczFld[i] <='F' )
        {
            cTmp = aczFld[i] - 'A' + 10;
        }
        else if( aczFld[i] >= '0' && aczFld[i] <='9' )
        {
            cTmp = aczFld[i] - '0';
        }
        else
        {
            cTmp = aczFld[i] & 0x0F;
        }

        if (i & 1)
        {
            aczBcd[j++] |= cTmp & 0x0f;
        }
        else
        {
            aczBcd[j] = cTmp << 4;
        }
    }

    iLen = (iLen+1) / 2;

    UDO_AddField( opcResultList, 1, UDT_BIN, aczBcd, iLen );

    return 0;
}

int ASM_BcdToAsc( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[1024];
    char aczAsc[2048];
    int  iLen, i;

    memset( aczFld, 0, sizeof(aczFld) );
    memset( aczAsc, 0, sizeof(aczAsc) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );

    for(i=0; i<iLen; i++)
    {
        sprintf(aczAsc+i*2, "%02X", aczFld[i]&0xff);
    }

    iLen = iLen * 2;

    UDO_AddField( opcResultList, 1, UDT_STRING, aczAsc, iLen );

    return 0;
}

int ASM_AscToHex( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[2048];
    int  iLen;
    int  iFld;
    char aczHex[5];

    memset( aczFld, 0, sizeof(aczFld) );
    memset( aczHex, 0, sizeof(aczHex) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );

    iFld = atoi( aczFld );
    memcpy( aczHex, (char *)&iFld, 4 );
    /*
    aczHex[0] = iFld %256;
    aczHex[1] = (iFld/256)%256;
    aczHex[2] = (iFld/256/256)%256;
    aczHex[3] = (iFld/256/256/256)%256;
    */
    /*
    LOG4C(( LOG_DEBUG, "iFld[%d],[%02x][%02x][%02x][%02x]", iFld, aczHex[0]&0xff, aczHex[1]&0xff,aczHex[2]&0xff, aczHex[3]&0xff ));
    */

    UDO_AddField( opcResultList, 1, UDT_BIN, aczHex, 4 );

    return 0;
}

int ASM_HexToAsc( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[20];
    int  iLen, i;
    int nFld=0;
    char aczAsc[20];

    memset( aczFld, 0, sizeof(aczFld) );
    memset( aczAsc, 0, sizeof(aczAsc) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
    memcpy( &nFld, aczFld, sizeof(int) );
    sprintf( aczAsc, "%d", nFld );
    UDO_AddField( opcResultList, 1, UDT_STRING, aczAsc, strlen(aczAsc) );
    return 0;
}

int ASM_GetTime2BCD( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                     int eiFldSeq, UDP opcResultList )
{
    char aczTime[10];
    char aczTmp[4];

    memset( aczTime, 0, sizeof(aczTime) );
    memset( aczTmp, 0, sizeof(aczTmp) );
    TOOL_GetSystemhhmmss(aczTime);
    aczTmp[0] = aczTime[0] << 4;
    aczTmp[0] |= aczTime[1]&0x0f;
    aczTmp[1] = aczTime[2] << 4;
    aczTmp[1] |= aczTime[3]&0x0f;
    aczTmp[2] = aczTime[4] << 4;
    aczTmp[2] |= aczTime[5]&0x0f;
    UDO_AddField( opcResultList, 1, UDT_BIN, aczTmp, 3 );
    return 0;
}

int ASM_GetPtDateTime( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    char aczTranDate[8+1];
    char aczTime[10];
    char aczFld[20];
    int  iRet;

    getString( "agtxdate", aczTranDate );
    memset( aczTime, 0, sizeof(aczTime) );
    TOOL_GetSystemhhmmss(aczTime);

    memset( aczFld, 0, sizeof(aczFld) );
    memcpy( aczFld, aczTranDate, 8 );
    memcpy( aczFld+8, aczTime, 6 );

    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczFld, 14 );
    return 0;
}

int ASM_RetMsgid( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[20];
    int  iLen;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
    aczFld[1] |= 0x10;
    UDO_AddField( opcResultList, 1, UDT_BIN, aczFld, iLen );

    return 0;
}


int ASM_TrimBlank( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList )
{
    char aczFld[2000];
    int  iLen;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
    PUB_StrTrim( aczFld, ' ' );
    UDO_AddField( opcResultList, 1, UDT_STRING, aczFld, strlen(aczFld) );

    return 0;
}

int ASM_GetChRetCode( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                      int eiFldSeq, UDP opcResultList )
{
    char aczHostId[100];
    char aczChanId[100];
    char aczHostRetCode[100];
    int  iLen;
    char aczChanRetCode[100];
    char aczChanRetMsg[100];
    int  nRet=0;

    memset( aczHostId, 0, sizeof(aczHostId) );
    memset( aczChanId, 0, sizeof(aczChanId) );
    memset( aczHostRetCode, 0, sizeof(aczHostRetCode) );
    memset( aczChanRetCode, 0, sizeof(aczChanRetCode) );
    memset( aczChanRetMsg, 0, sizeof(aczChanRetMsg) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczHostId, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取主机节点号出错" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 2, 0, aczHostRetCode, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取主机响应码出错" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 3, 0, aczChanId, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取渠道节点号出错" ));
        return -1;
    }
    nRet = ADP_GetChanRet( atoi(aczHostId), atoi(aczChanId), aczHostRetCode, NULL, aczChanRetCode, aczChanRetMsg );
    if( nRet != 0 )
    {
        strcpy( aczChanRetCode, "96" );
        strcpy( aczChanRetMsg, "系统故障" );
    }
    UDO_AddField( opcResultList, 1, UDT_STRING, aczChanRetCode, strlen(aczChanRetCode) );

    return 0;
}

int ASM_GetChRetMsg( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                     int eiFldSeq, UDP opcResultList )
{
    char aczHostId[100];
    char aczChanId[100];
    char aczHostRetCode[100];
    int  iLen;
    char aczChanRetCode[100];
    char aczChanRetMsg[100];
    int  nRet=0;

    memset( aczHostId, 0, sizeof(aczHostId) );
    memset( aczChanId, 0, sizeof(aczChanId) );
    memset( aczHostRetCode, 0, sizeof(aczHostRetCode) );
    memset( aczChanRetCode, 0, sizeof(aczChanRetCode) );
    memset( aczChanRetMsg, 0, sizeof(aczChanRetMsg) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczHostId, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取主机节点号出错" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 2, 0, aczHostRetCode, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取主机响应码出错" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 3, 0, aczChanId, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "获取渠道节点号出错" ));
        return -1;
    }
    nRet = ADP_GetChanRet( atoi(aczHostId), atoi(aczChanId), aczHostRetCode, NULL, aczChanRetCode, aczChanRetMsg );
    if( nRet != 0 )
    {
        strcpy( aczChanRetCode, "96" );
        strcpy( aczChanRetMsg, "系统故障" );
    }
    UDO_AddField( opcResultList, 1, UDT_STRING, aczChanRetMsg, strlen(aczChanRetMsg) );

    return 0;
}

int ASM_ChgTpdu( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                 int eiFldSeq, UDP opcResultList )
{
    int  iLen;
    char aczBuf[20];
    int  nRet=0;
    char aczTpdu[6];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczTpdu, 0, sizeof(aczTpdu) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取TPDU失败" ));
        return -1;
    }
    aczTpdu[0] = aczBuf[0];
    aczTpdu[1] = aczBuf[3];
    aczTpdu[2] = aczBuf[4];
    aczTpdu[3] = aczBuf[1];
    aczTpdu[4] = aczBuf[2];

    UDO_AddField( opcResultList, 1, UDT_BIN, aczTpdu, 5 );

    return 0;
}


int ASM_GetBatch( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczBatch[7];
    char aczOffset[10];
    int  iNum=0;
    int  iOffset=0;

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczBatch, 0, sizeof(aczBatch) );
    memset( aczOffset, 0, sizeof(aczOffset) );

    iNum = UDO_FieldCount( epcSrcList );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取60域失败" ));
        return -1;
    }

    if( iNum > 1 )
    {
        nRet = UDO_GetField( epcSrcList, 2, 0, aczOffset, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取批次号偏移位失败" ));
            return -1;
        }
        iOffset = atoi( aczOffset );
    }
    else
    {
        iOffset = 2;
    }

    TOOL_ConvertBCD2Ascii( aczBuf+iOffset, 6, aczBatch );

    UDO_AddField( opcResultList, 1, UDT_STRING, aczBatch, 6 );

    return 0;
}

int ASM_GetOldSerial( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                      int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczOldSerial[7];
    int  iNum=0;
    char aczOffset[10];
    int  iOffset=0;

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczOffset, 0, sizeof(aczOffset) );
    memset( aczOldSerial, 0, sizeof(aczOldSerial) );

    iNum = UDO_FieldCount( epcSrcList );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取61域失败" ));
        return -1;
    }

    if( iNum > 1 )
    {
        nRet = UDO_GetField( epcSrcList, 2, 0, aczOffset, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取原流水偏移位失败" ));
            return -1;
        }
        iOffset = atoi( aczOffset );
    }
    else
    {
        iOffset = 2;
    }

    TOOL_ConvertBCD2Ascii( aczBuf+iOffset, 6, aczOldSerial );

    UDO_AddField( opcResultList, 1, UDT_STRING, aczOldSerial, 6 );

    return 0;
}

int ASM_GetOldDate( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                    int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczOldDate[9];
    int iNum=0;
    int iOffset;
    char aczOffset[10];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczOldDate, 0, sizeof(aczOldDate) );
    memset( aczOffset, 0, sizeof(aczOffset) );

    iNum = UDO_FieldCount( epcSrcList );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取原交易日期失败" ));
        return -1;
    }
    if( iNum > 1 )
    {
        nRet = UDO_GetField( epcSrcList, 2, 0, aczOffset, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取原交易日期偏移位失败" ));
            return -1;
        }
        iOffset = atoi( aczOffset );
    }
    else
    {
        iOffset = 2;
    }

    TOOL_ConvertBCD2Ascii( aczBuf+iOffset, 4, aczOldDate );

    UDO_AddField( opcResultList, 1, UDT_STRING, aczOldDate, 4 );

    return 0;
}

int ASM_GetOldBatch( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                     int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczBatch[7];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczBatch, 0, sizeof(aczBatch) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取原批次号失败" ));
        return -1;
    }

    TOOL_ConvertBCD2Ascii( aczBuf+2, 6, aczBatch );

    UDO_AddField( opcResultList, 1, UDT_STRING, aczBatch, 6 );

    return 0;
}

int ASM_LEN_BIN2ASCII( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  iLenLen=0;
    int  iBufLen=0;
    unsigned char aczBuf[1024];
    int  nRet = 0;
    unsigned char aczLen[6];
    char aczValue[1024];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iBufLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：源数据失败" ));
        return -1;
    }

    nRet = UDO_GetField( epcSrcList, 2, 0, aczLen, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：数据长度失败" ));
        return -1;
    }

    iLenLen = atoi( aczLen );
    if( iLenLen== 1 )
    {
        iLen = (aczBuf[0] >> 4) * 10 + (aczBuf[0] & 0x0f);
    }
    else
    {
        iLen = (aczBuf[0] >> 4) * 10 + (aczBuf[0] & 0x0f);
        iLen = iLen*100 + (aczBuf[1] >> 4) * 10 + (aczBuf[1] & 0x0f);
    }

    TOOL_ConvertBCD2Ascii( aczBuf+iLenLen, iLen, aczValue );

    UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iLen );

    return 0;
}

int ASM_AddStr_FORMAT( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    int iRet, i, iFldType, iNum;
    int iLength, iOffset=0;
    short   shValue;
    int     iValue;
    long    lValue;
    float   fValue;
    double  dValue;
    char aczValue[ 2000 ], aczTmpValue[ 2000 ];
    char aczTmpLen[100], aczBuf[2000];

    memset( aczValue, 0x00, sizeof(aczValue) );

    iNum = UDO_FieldCount( epcSrcList );
    if( iNum%2 != 0 )
    {
        LOG4C ((LOG_FATAL, "参数个数必须是偶数"));
        return -1;
    }
    for( i=0; i<iNum; i+=2 )
    {
        memset( aczTmpValue, 0x00, sizeof(aczTmpValue) );
        memset( aczTmpLen, 0x00, sizeof(aczTmpLen) );
        iRet = UDO_GetField (epcSrcList, i+1, 0, aczTmpValue, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        iRet = UDO_GetField (epcSrcList, i+1+1, 0, aczTmpLen, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        iLength = atoi( aczTmpLen );
        memset( aczBuf, 0, sizeof(aczBuf) );
        sprintf( aczBuf, "%*.*s", iLength, iLength, aczTmpValue );
        memcpy( aczValue+iOffset, aczBuf, (size_t)iLength);
        iOffset += iLength;
    }
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iOffset );
    if( iRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_GetSysDate2BCD( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczDate[20];
    char aczLen[10];
    char aczBcd[10];

    memset( aczDate, 0, sizeof(aczDate) );
    memset( aczLen, 0, sizeof(aczLen) );
    memset( aczBcd, 0, sizeof(aczBcd) );

    TOOL_GetSystemYYYYMMDD( aczDate );
    TOOL_ConvertAscii2BCD( aczDate, aczBcd, 8 );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczLen, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：日期格式出错" ));
        return -1;
    }


    if( aczLen[0] == '4' )
    {
        nRet = UDO_AddField( opcResultList, 1, UDT_BIN, aczBcd+2, 2 );
    }
    else
    {
        nRet = UDO_AddField( opcResultList, 1, UDT_BIN, aczBcd, 4 );
    }

    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;
}

int ASM_PackField60Ums( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[100];

    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取60域批次号错误" ));
        return -1;
    }

    TOOL_ConvertBCD2Ascii( aczIn, iLen*2, aczBuf );

    memset( aczIn, 0, sizeof(aczIn) );
    sprintf( aczIn, "00%6.6s001", aczBuf+4 );
    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczIn, strlen(aczIn) );

    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_PackBalanceUms( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[100];
    struct
    {
        char sAmount_Type[2];
        char sAccount_Type[2];
        char sCurrency_Code[3];
        char cAmount_Sign;
        char sAmount[12];
    } stBal;

    memset( &stBal, 0, sizeof(stBal) );
    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取54余额错误" ));
        return -1;
    }

    memcpy( stBal.sAmount_Type, "02", 2 );
    memcpy( stBal.sAccount_Type, "10", 2 );
    memcpy( stBal.sCurrency_Code, "156", 3 );
    stBal.cAmount_Sign = 'C';
    sprintf( aczBuf, "%012d", atoi( aczIn+12 ) );
    memcpy( stBal.sAmount, aczBuf, 12 );

    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)&stBal, 20 );

    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_PackSettleData( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[200];
    char aczField[200];
    typedef struct
    {
        /**<
         *内卡结算内容：
         */
        char sDebitAmt_N[12+1];           /**<借记总金额          N12*/
        char sDebitNum_N[3+1];            /**<借记总笔数          N3*/
        char sCreditAmt_N[12+1];          /**<贷记总金额          N12*/
        char sCreditNum_N[3+1];           /**<贷记总笔数          N3*/
        char cSettleCode_N;             /**<对账应答代码       N1*/
        /**<
         *外卡结算内容：
         */
        char sDebitAmt_W[12+1];           /**<借记总金额          N12*/
        char sDebitNum_W[3+1];            /**<借记总笔数          N3*/
        char sCreditAmt_W[12+1];          /**<贷记总金额          N12*/
        char sCreditNum_W[3+1];           /**<贷记总笔数          N3*/
        char cSettleCode_W;             /**<对账应答代码       N1*/
    } STAMT_NUM_SETTLE;
    STAMT_NUM_SETTLE stSettle;

    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczField, 0, sizeof(aczField) );
    memset( &stSettle, 0, sizeof(STAMT_NUM_SETTLE) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取结算数据错误" ));
        return -1;
    }

    TOOL_ConvertBCD2Ascii( aczIn, iLen*2, aczBuf );

    memcpy( stSettle.sDebitAmt_N, aczBuf,              12 );
    memcpy( stSettle.sDebitNum_N, aczBuf+12,           3 );
    memcpy( stSettle.sCreditAmt_N,aczBuf+12+3,         12 );
    memcpy( stSettle.sCreditNum_N,aczBuf+12+3+12,      3 );

    sprintf( aczField,"%06d%012d%06d%012d%06d%012d", atoi(stSettle.sDebitNum_N), atoi( stSettle.sDebitAmt_N ),
             atoi(stSettle.sCreditNum_N), atoi( stSettle.sCreditAmt_N ), 0, 0 );

    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczField, strlen(aczField) );

    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_PackSettleResult( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                          int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[200];
    char aczResult[20];
    typedef struct
    {
        /**<
         *内卡结算内容：
         */
        char sDebitAmt_N[12];           /**<借记总金额          N12*/
        char sDebitNum_N[3];            /**<借记总笔数          N3*/
        char sCreditAmt_N[12];          /**<贷记总金额          N12*/
        char sCreditNum_N[3];           /**<贷记总笔数          N3*/
        char cSettleCode_N;             /**<对账应答代码       N1*/
        /**<
         *外卡结算内容：
         */
        char sDebitAmt_W[12];           /**<借记总金额          N12*/
        char sDebitNum_W[3];            /**<借记总笔数          N3*/
        char sCreditAmt_W[12];          /**<贷记总金额          N12*/
        char sCreditNum_W[3];           /**<贷记总笔数          N3*/
        char cSettleCode_W;             /**<对账应答代码       N1*/
    } STAMT_NUM_SETTLE;
    STAMT_NUM_SETTLE stSettle;
    int  iResultLen=0;

    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczResult, 0, sizeof(aczResult) );
    memset( &stSettle, 0, sizeof(STAMT_NUM_SETTLE) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取结算数据错误" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 1, 0, aczResult, &iResultLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取结算结果错误" ));
        return -1;
    }

    TOOL_ConvertBCD2Ascii( aczIn, iLen*2, aczBuf );
    memcpy( &stSettle, aczBuf, strlen(aczBuf) );
    stSettle.cSettleCode_N = aczResult[iResultLen-1];
    stSettle.cSettleCode_W = '1';

    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)&stSettle, sizeof(STAMT_NUM_SETTLE) );
    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_SetAgenId( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList )
{
    char *p=NULL;
    char aczAgenId[20];
    int  nRet=0;

    memset( aczAgenId, 0, sizeof(aczAgenId) );
    p = getenv("AGENID");

    if( p!=NULL)
    {
        strcpy( aczAgenId, p );
    }
    else
    {
        strcpy( aczAgenId, "00000000" );
    }

    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)aczAgenId, strlen(aczAgenId) );
    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;
}


int ASM_LEN_ASCII2ASCII( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                         int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  iLenLen=0;
    int  iBufLen=0;
    unsigned char aczBuf[1024];
    int  nRet = 0;
    unsigned char aczLen[6];
    char aczValue[1024];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );
    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iBufLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：源数据失败" ));
        return -1;
    }
    nRet = UDO_GetField( epcSrcList, 2, 0, aczLen, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：数据长度失败" ));
        return -1;
    }
    iLenLen = atoi( aczLen );
    if( iLenLen== 1 )
    {
        iLen = (aczBuf[0] >> 4) * 10 + (aczBuf[0] & 0x0f);
    }
    else
    {
        iLen = (aczBuf[0] >> 4) * 10 + (aczBuf[0] & 0x0f);
        iLen = iLen*100 + (aczBuf[1] >> 4) * 10 + (aczBuf[1] & 0x0f);
    }

    memset(aczValue, 0, sizeof(aczValue));
    memcpy(aczValue, aczBuf+iLenLen, iLen);
    UDO_AddField( opcResultList, 1, UDT_STRING,  aczValue, iLen );
    return 0;
}

int ASM_AlignLeft( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList )
{
    int  iBufLen=0;
    unsigned char aczBuf[1024];
    unsigned char aczValue[1024];
    int  nRet = 0;
    memset( aczBuf, 0, sizeof(aczBuf) );
    memset(aczValue, 0, sizeof(aczValue));

    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iBufLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：源数据失败" ));
        return -1;
    }
    TOOL_ConvertBCD2Ascii(aczBuf, iBufLen*2, aczValue);
    memset(aczBuf, 0, sizeof(aczBuf));
    aczValue[iBufLen*2] = '0';
    TOOL_ConvertAscii2BCD(aczValue+1,aczBuf, iBufLen*2);
    UDO_AddField( opcResultList, 1, UDT_STRING,  aczBuf, iBufLen );
    return 0;
}



int ASM_PackBalanceBankComm( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                             int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[100];

    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取54余额错误" ));
        return -1;
    }
    sprintf( aczBuf, "%012d", atoi( aczIn+12+2 ) );
    memset(aczIn, 0, sizeof(aczIn));
    TOOL_ConvertAscii2BCD(aczBuf, aczIn, 12);
    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)&aczIn, 6 );
    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

/*通用拆包函数*/
int ASM_GetNormalString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                         int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczOffset[10];
    char aczLen[10];
    int  iNum=0;
    int  iOffset=0;
    int iDataLen=0;
    char aczDataMode[10];
    char cDataMode='A';
    char aczData[100];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );
    memset( aczOffset, 0, sizeof(aczOffset) );
    memset( aczDataMode, 0, sizeof(aczDataMode) );
    memset( aczData, 0, sizeof(aczData) );

    iNum = UDO_FieldCount( epcSrcList );

    /*读取字符串*/
    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取数据域失败" ));
        return -1;
    }
    /*读取偏移地址*/
    if( iNum > 1 )
    {
        nRet = UDO_GetField( epcSrcList, 2, 0, aczOffset, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        iOffset = atoi( aczOffset );
    }
    else
    {
        iOffset = 2;
    }

    /*读取字符串长度*/
    if( iNum > 2 )
    {
        nRet = UDO_GetField( epcSrcList, 3, 0, aczLen, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        iDataLen = atoi( aczLen );
    }
    else
    {
        iDataLen = 6;
    }
    /*读取数据格式*/
    if( iNum > 3 )
    {
        nRet = UDO_GetField( epcSrcList, 4, 0, aczDataMode, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        cDataMode = aczDataMode[0];
    }
    else
    {
        cDataMode = 'A';
    }


    /*处理数据*/
    if(cDataMode == 'A')
    {
        memcpy(aczData, aczBuf+iOffset, iDataLen);
    }
    else
    {
        TOOL_ConvertBCD2Ascii( aczBuf+iOffset, iDataLen, aczData );
    }

    UDO_AddField( opcResultList, 1, UDT_STRING, aczData, iDataLen);

    return 0;
}




/*雅酷余额域打包*/
int ASM_PackBalanceYacol( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                          int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[100];

    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取54余额错误" ));
        return -1;
    }
    sprintf( aczBuf, "C%012d", atoi( aczIn+12+2 ) );
    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)&aczBuf, 13 );
    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}

int ASM_AddStr_FORMAT_Ext( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                           int eiFldSeq, UDP opcResultList )
{
    int iRet, i, iFldType, iNum;
    int iLength, iOffset=0;
    short   shValue;
    int     iValue;
    long    lValue;
    float   fValue;
    double  dValue;
    char aczValue[ 2000 ], aczTmpValue[ 2000 ];
    char aczTmpLen[100], aczBuf[2000];
    char aczOffset[100];
    int nBufOffset=0;

    memset( aczValue, 0x00, sizeof(aczValue) );

    iNum = UDO_FieldCount( epcSrcList );
    if( iNum%3 != 0 )
    {
        LOG4C ((LOG_FATAL, "参数个数必须是3的倍数"));
        return -1;
    }
    for( i=0; i<iNum; i+=3 )
    {
        memset( aczTmpValue, 0x00, sizeof(aczTmpValue) );
        memset( aczTmpLen, 0x00, sizeof(aczTmpLen) );
        memset( aczOffset, 0x00, sizeof(aczOffset) );

        /*字符串*/
        iRet = UDO_GetField (epcSrcList, i+1, 0, aczTmpValue, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        /*偏移地址*/
        iRet = UDO_GetField (epcSrcList, i+2, 0, aczOffset, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        nBufOffset = atoi(aczOffset);
        /*长度*/
        iRet = UDO_GetField (epcSrcList, i+3, 0, aczTmpLen, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        iLength = atoi( aczTmpLen );
        memset( aczBuf, 0, sizeof(aczBuf) );

        sprintf( aczBuf, "%*.*s", iLength, iLength, aczTmpValue+nBufOffset);
        memcpy( aczValue+iOffset, aczBuf, (size_t)iLength);
        iOffset += iLength;
    }
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iOffset );
    if( iRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}


/*通用拆包打包函数*/
int ASM_PackString( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                    int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    char aczBuf[1024];
    int  nRet = 0;
    char aczOffset[10];
    char aczLen[10];
    int  iNum=0;
    int  iOffset=0;
    int iDataLen=0;
    char aczDataMode[10];
    char cDataMode='A';
    char aczData[100];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );
    memset( aczOffset, 0, sizeof(aczOffset) );
    memset( aczDataMode, 0, sizeof(aczDataMode) );
    memset( aczData, 0, sizeof(aczData) );

    iNum = UDO_FieldCount( epcSrcList );

    /*读取字符串*/
    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );

    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取数据域失败" ));
        return -1;
    }
    /*读取偏移地址*/
    if( iNum > 1 )
    {
        nRet = UDO_GetField( epcSrcList, 2, 0, aczOffset, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        iOffset = atoi( aczOffset );
    }
    else
    {
        iOffset = 2;
    }

    /*读取字符串长度*/
    if( iNum > 2 )
    {
        nRet = UDO_GetField( epcSrcList, 3, 0, aczLen, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        iDataLen = atoi( aczLen );
    }
    else
    {
        iDataLen = 6;
    }
    /*读取数据格式*/
    if( iNum > 3 )
    {
        nRet = UDO_GetField( epcSrcList, 4, 0, aczDataMode, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        cDataMode = aczDataMode[0];
    }
    else
    {
        cDataMode = 'A';
    }


    /*处理数据*/
    if(cDataMode == 'A')
    {
        memcpy(aczData, aczBuf+iOffset, iDataLen);
    }
    else
    {
        TOOL_ConvertBCD2Ascii( aczBuf+iOffset, iDataLen, aczData );
    }


    /*打包格式*/
    if( iNum > 4 )
    {
        memset(aczDataMode, 0, sizeof(aczDataMode));
        nRet = UDO_GetField( epcSrcList, 5, 0, aczDataMode, &iLen );
        if( nRet != 0 )
        {
            LOG4C(( LOG_DEBUG, "重组函数：获取数据域偏移位失败" ));
            return -1;
        }
        cDataMode = aczDataMode[0];
    }
    else
    {
        cDataMode = 'A';
    }

    /*处理数据*/
    memset(aczBuf, 0, sizeof(aczBuf));
    if(cDataMode == 'A')
    {
        memcpy(aczBuf, aczData, iDataLen);
    }
    else
    {
        TOOL_ConvertAscii2BCD( aczData, aczBuf, iDataLen );
        iDataLen = (iDataLen+1)/2;
    }

    UDO_AddField( opcResultList, 1, UDT_STRING, aczBuf, iDataLen);
    return 0;
}


int ASM_PackMacBuffer( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    int i=0;
    int iNum=0;
    int iRet;
    char aczBuf[2048];
    int  iLen = 0;
    char aczTmp[500];
    int nBufLen=0;

    assert( epcSrcList != NULL );
    assert( opcResultList != NULL );

    memset(aczBuf, 0, sizeof(aczBuf));
    iNum = UDO_FieldCount( epcSrcList );

    
    for( i=0; i<iNum; i++ )
    {
        memset(aczTmp, 0, sizeof(aczTmp));
        
        iRet = UDO_GetField( epcSrcList, i+1, 0, aczTmp, &iLen );
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return ASM_FAIL;
        }
        
        if(iLen > 0)
        {
            memcpy(aczBuf+nBufLen, aczTmp, iLen);
            nBufLen += iLen;
            memcpy(aczBuf+nBufLen, " ", 1);
            nBufLen += 1;
       //     TOOL_Dump( LOG_DEBUG, "mac.dump", (unsigned char*)aczBuf, nBufLen);
            
        }

    }
    if(nBufLen <= 1)
    {

        LOG4C ((LOG_FATAL, "ASM_PackMacBuffer 域数据错误"));
        return ASM_FAIL;

    }
    nBufLen --;
    aczBuf[nBufLen] = 0;

   // LOG4C ((LOG_FATAL, "aczBuf = [%d][%s]", nBufLen, aczBuf));

  //  TOOL_Dump( LOG_DEBUG, "aczBuf.dump", (unsigned char*)aczBuf, nBufLen);

    UDO_AddField( opcResultList, 1, UDT_STRING, aczBuf, nBufLen);
    return 0;
}


/*获取系统变量*/
int ASM_GetEvn( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                   int eiFldSeq, UDP opcResultList )
{

    char *p=NULL;
    char aczFld[100];
    int  iLen;
    int nRet;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );    
    p = getenv(aczFld);
    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)p, strlen(p) );
    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;
}


/*返回MSG ID*/
int ASM_RetAscMsgid( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[20];
    int  iLen;
    int nValue=0;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
	
    nValue = atoi(aczFld);
    nValue += 10;
    memset(aczFld, 0, sizeof(aczFld));
    sprintf(aczFld, "%04d", nValue);
    UDO_AddField( opcResultList, 1, UDT_STRING, aczFld, iLen );
    return 0;
}

int ASM_PackBalanceAllinpay( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                        int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  nRet = 0;
    char aczIn[100];
    char aczBuf[100];
    char szTmp[100+1];
    char aczOut[100+1];
    struct
    {
        char sAmount_Type[2];
        char sAccount_Type[2];
        char sCurrency_Code[3];
        char cAmount_Sign;
        char sAmount[12];
    } stBal;

    memset( &stBal, 0, sizeof(stBal) );
    memset( aczIn, 0, sizeof(aczIn) );
    memset( aczBuf, 0, sizeof(aczBuf) );

    nRet = UDO_GetField( epcSrcList, 1, 0, aczIn, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：获取54余额错误" ));
        return -1;
    }
    memcpy( stBal.sAmount_Type, "90", 2 );
    memcpy( stBal.sAccount_Type, "01", 2 );
    memcpy( stBal.sCurrency_Code, "156", 3 );
    stBal.cAmount_Sign = 'C';
    memset(szTmp, 0, sizeof(szTmp));
    memcpy(szTmp, aczIn+2, 12);
    sprintf( aczBuf, "%012d", atoi( szTmp) );
    memcpy( stBal.sAmount, aczBuf, 12 );
    

    memset(aczOut, 0, sizeof(aczOut));
    memcpy(aczOut, (char *)&stBal, 20);

    memcpy( stBal.sAmount_Type, "90", 2 );
    memcpy( stBal.sAccount_Type, "02", 2 );
    memcpy( stBal.sCurrency_Code, "156", 3 );
    stBal.cAmount_Sign = 'C';
    memset(szTmp, 0, sizeof(szTmp));
    memcpy(szTmp, aczIn+2+12, 12);
    sprintf( aczBuf, "%012d", atoi( szTmp) );
    memcpy( stBal.sAmount, aczBuf, 12 );
    memcpy(aczOut+20, (char *)&stBal, 20);

    nRet = UDO_AddField( opcResultList, 1, UDT_STRING, (char *)aczOut, 40 );

    if( nRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}


int ASM_PackToAsc( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                         int eiFldSeq, UDP opcResultList )
{
    int  iLen = 0;
    int  iLenLen=0;
    int  iBufLen=0;
    unsigned char aczBuf[1024];
    int  nRet = 0;
    unsigned char aczLen[100];
    char aczValue[1024];

    memset( aczBuf, 0, sizeof(aczBuf) );
    memset( aczLen, 0, sizeof(aczLen) );
    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iBufLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：源数据失败" ));
        return -1;
    }
	
    nRet = UDO_GetField( epcSrcList, 2, 0, aczLen, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：数据长度失败" ));
        return -1;
    }
	
    iLenLen = atoi( aczLen );
   	memset(aczLen, 0, sizeof(aczLen));
    memcpy(aczLen, aczBuf, iLenLen);

	iLen = atoi(aczLen);

    memset(aczValue, 0, sizeof(aczValue));
    memcpy(aczValue, aczBuf+iLenLen, iLen);
	
    UDO_AddField( opcResultList, 1, UDT_STRING,  aczValue, iLen );
    return 0;
}


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
int  ASM_UncharFldCopy( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId, 
                  int eiFldSeq, UDP opcResultList )
{
	
    int  iLen = 0;
    unsigned char aczBuf[1024];
	int nRet;

    memset( aczBuf, 0, sizeof(aczBuf) );
    nRet = UDO_GetField( epcSrcList, 1, 0, aczBuf, &iLen );
    if( nRet != 0 )
    {
        LOG4C(( LOG_DEBUG, "重组函数：源数据失败" ));
        return -1;
    }
  

    UDO_AddField( opcResultList, 1, UDT_STRING,  aczBuf, iLen );
    return 0;
}


/*返回MSG ID*/
int ASM_RetTranstype( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                  int eiFldSeq, UDP opcResultList )
{
    char aczFld[20];
    int  iLen;
    int nValue=0;

    memset( aczFld, 0, sizeof(aczFld) );
    UDO_GetField( epcSrcList, 1, 0, aczFld, &iLen );
	
    nValue = atoi(aczFld);
    nValue += 10000;
    memset(aczFld, 0, sizeof(aczFld));
    sprintf(aczFld, "%06d", nValue);
    UDO_AddField( opcResultList, 1, UDT_STRING, aczFld, 6 );
    return 0;
}


int ASM_AddStr_Amount( SVC_Content *epstService, UDP epcSrcList, int eiMsgAppId,
                       int eiFldSeq, UDP opcResultList )
{
    int iRet, i, iFldType, iNum;
    int iLength, iOffset=0;
    short   shValue;
    int     iValue;
    long    lValue;
    float   fValue;
    double  dValue;
    char aczValue[ 2000 ], aczTmpValue[ 2000 ];
    char aczTmpLen[100], aczBuf[2000];

    memset( aczValue, 0x00, sizeof(aczValue) );

    iNum = UDO_FieldCount( epcSrcList );
    if( iNum%2 != 0 )
    {
        LOG4C ((LOG_FATAL, "参数个数必须是偶数"));
        return -1;
    }
    for( i=0; i<iNum; i+=2 )
    {
        memset( aczTmpValue, 0x00, sizeof(aczTmpValue) );
        memset( aczTmpLen, 0x00, sizeof(aczTmpLen) );
        iRet = UDO_GetField (epcSrcList, i+1, 0, aczTmpValue, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        iRet = UDO_GetField (epcSrcList, i+1+1, 0, aczTmpLen, &iLength);
        if (iRet != 0)
        {
            LOG4C ((LOG_FATAL, "从链表中取节点值失败"));
            return -1;
        }
        iLength = atoi( aczTmpLen );
        memset( aczBuf, 0, sizeof(aczBuf) );
        sprintf( aczBuf, "%0*.*s", iLength, iLength, aczTmpValue );
        memcpy( aczValue+iOffset, aczBuf, (size_t)iLength);
        iOffset += iLength;
    }
    iRet = UDO_AddField( opcResultList, 1, UDT_STRING, aczValue, iOffset );
    if( iRet != 0 )
    {
        LOG4C ((LOG_FATAL, "向链表中增加节点失败"));
        return -1;
    }
    return 0;

}



