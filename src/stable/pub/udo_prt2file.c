#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "udo.h"
#include "tool_base.h"
#include "log.h"

/**
  * 注: 使用前需要调用LOG4C_INIT初始日志环境
  **/
int UDO_Prt2File( int eiLevel, char * epczFileName, UDP epfAddr, ... )
{
    va_list  ap;

    int    iFldNum, iValNum, iFldIdx, iValIdx;
    int    iFldId;
    int    iFldType;
    int    iFldLen;
    int    iNextOffset, iCurrOffset;
    char   cTmp;
    short  sTmp;
    int    iTmp;
    long   lTmp;
    float  fTmp;
    double dTmp; 
    char  *pcPath, *pczTmp, *pczFmt , *pcTrace, *pcLevel;
    char   aczPathFile[ 200 ];
    char   aczBakPathFile[ 200 ];
    char   aczTemp[ 200 ], aczDateTime[ 200 ];
    char   aczFile[200], aczFileSuffix[64];
    struct stat stStat;
    FILE  *fp;


    if (!LOG4C_ISLOG (eiLevel))
    {
        return -1;
    }

    memset (aczDateTime, 0, sizeof( aczDateTime ) );
    TOOL_GetSystemYYYYMMDD( aczDateTime );

    memset (aczFileSuffix, 0, sizeof (aczFileSuffix));
    strcpy (aczFileSuffix, LOG4C_SUFFIX());

    memset (aczFile, 0, sizeof (aczFile));
    if (strlen (aczFileSuffix) != 0)
    {
        sprintf (aczFile, "%s/%s.%s.%s",
                 LOG4C_DIR(), epczFileName, aczDateTime, aczFileSuffix);
    }
    else
    {
        sprintf (aczFile, "%s/%s.%s",
                 LOG4C_DIR(), epczFileName, aczDateTime);
    }

    if ((fp = fopen (aczFile, "a+")) == NULL)
    {
         return -1;
    }

    fprintf( fp, "==============================================\n\n" );

    va_start( ap, epfAddr ); 

    pczFmt = va_arg( ap, char * );

    memset( aczDateTime, 0, sizeof( aczDateTime ) );
    TOOL_GetCurrTime( aczDateTime );

    fprintf( fp, "%s  pid %d ", aczDateTime,getpid() ); fflush( fp );

    vfprintf ( fp, pczFmt, ap ); fflush( fp );

    va_end( ap );

    iFldNum = UDO_FieldCount( epfAddr );

    fprintf( fp, "Sequence Length Value\n" ); fflush( fp );
    for( iFldIdx = 0; iFldIdx < iFldNum; iFldIdx ++ ) {

       UDO_FieldId( epfAddr, iFldIdx, &iFldId );

       iFldType = UDO_FieldType( epfAddr, iFldId );

       iValNum = UDO_FieldOccur( epfAddr, iFldId );

       fprintf( fp, "Id[%-d],Type ", iFldId ); fflush( fp );
       switch( iFldType ) {
	  case UDT_CHAR:
             fprintf( fp, "[C]\n" ); fflush( fp );
	     break;
	  case UDT_SHORT:
             fprintf( fp, "[Sh]\n" ); fflush( fp );
	     break;
	  case UDT_INT:
             fprintf( fp, "[I]\n" ); fflush( fp );
	     break;
	  case UDT_LONG:
             fprintf( fp, "[L]\n" ); fflush( fp );
	     break;
	  case UDT_FLOAT:
             fprintf( fp, "[F]\n" ); fflush( fp );
	     break;
	  case UDT_DOUBLE:
             fprintf( fp, "[D]\n" ); fflush( fp );
	     break;
          case UDT_STRING:
             fprintf( fp, "[S]\n" ); fflush( fp );
	     break;
	  default:
             fprintf( fp, "[O]\n" ); fflush( fp );
	     break;
       }
       /*************fprintf( fp, "Field Id [%-d], Type ", iFldId ); fflush( fp );
       switch( iFldType ) {
	  case UDT_CHAR:
             fprintf( fp, "[Char]\n" ); fflush( fp );
	     break;
	  case UDT_SHORT:
             fprintf( fp, "[Short]\n" ); fflush( fp );
	     break;
	  case UDT_INT:
             fprintf( fp, "[Int]\n" ); fflush( fp );
	     break;
	  case UDT_LONG:
             fprintf( fp, "[Long]\n" ); fflush( fp );
	     break;
	  case UDT_FLOAT:
             fprintf( fp, "[Float]\n" ); fflush( fp );
	     break;
	  case UDT_DOUBLE:
             fprintf( fp, "[Double]\n" ); fflush( fp );
	     break;
          case UDT_STRING:
             fprintf( fp, "[String]\n" ); fflush( fp );
	     break;
	  default:
             fprintf( fp, "[Other]\n" ); fflush( fp );
	     break;
       }
       *******************/

/*********       fprintf( fp, "Sequence Length Value\n" ); fflush( fp );
************************/
       for( iValIdx = 0; iValIdx < iValNum; iValIdx ++ ) {

           switch( iFldType ) {
              case UDT_CHAR:
                 cTmp = 0;
                 if( iValIdx == 0 ) {
                    UDO_GetFirstField( epfAddr, iFldId, &cTmp, &iFldLen,
                                      &iNextOffset );
                 } else {
                    UDO_GetCurrField( epfAddr, iCurrOffset, &cTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-c\n", iValIdx, iFldLen, cTmp );
                 fflush( fp );
                 break;
              case UDT_SHORT:
                 sTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&sTmp, &iFldLen,
                                      &iNextOffset );
                 } else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&sTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-d\n", iValIdx, iFldLen, sTmp );
                 fflush( fp );
                 break;
              case UDT_INT:
                 iTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&iTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&iTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-d\n", iValIdx, iFldLen, iTmp );
                 fflush( fp );
                 break;
              case UDT_LONG:
                 lTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&lTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&lTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-ld\n", iValIdx, iFldLen, lTmp );
                 fflush( fp );
                 break;
              case UDT_FLOAT:
                 fTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&fTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&fTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-f\n", iValIdx, iFldLen, fTmp );
                 fflush( fp );
                 break;
              case UDT_DOUBLE:
                 dTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&dTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&dTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-lf\n", iValIdx, iFldLen, dTmp );
                 fflush( fp );
                 break;
              default:
                 iFldLen = UDO_FieldLen( epfAddr, iFldId, iValIdx );
                 do {
		    pczTmp = ( char * )calloc( 1, iFldLen + 1 );
                 } while ( pczTmp == NULL );
                 if( iValIdx == 0 ) {
                    UDO_GetFirstField( epfAddr, iFldId, pczTmp, &iFldLen,
                                      &iNextOffset );
                 } else {
                    UDO_GetCurrField( epfAddr, iCurrOffset, pczTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 fprintf( fp, "%-d| %-d| %-s\n", iValIdx, iFldLen, pczTmp );
                 fflush( fp );
                 free( pczTmp );
                 break;
           }
#if 0     /***********************************************************/
           switch( iFldType ) {
              case UDT_CHAR:
                 cTmp = 0;
                 if( iValIdx == 0 ) {
                    UDO_GetFirstField( epfAddr, iFldId, &cTmp, &iFldLen,
                                      &iNextOffset );
                 } else {
                    UDO_GetCurrField( epfAddr, iCurrOffset, &cTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-c\n", iValIdx, iFldLen, cTmp );
                 fflush( fp );
                 break;
              case UDT_SHORT:
                 sTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&sTmp, &iFldLen,
                                      &iNextOffset );
                 } else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&sTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-d\n", iValIdx, iFldLen, sTmp );
                 fflush( fp );
                 break;
              case UDT_INT:
                 iTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&iTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&iTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-d\n", iValIdx, iFldLen, iTmp );
                 fflush( fp );
                 break;
              case UDT_LONG:
                 lTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&lTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&lTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-ld\n", iValIdx, iFldLen, lTmp );
                 fflush( fp );
                 break;
              case UDT_FLOAT:
                 fTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&fTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&fTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-f\n", iValIdx, iFldLen, fTmp );
                 fflush( fp );
                 break;
              case UDT_DOUBLE:
                 dTmp = 0;
                 if( iValIdx == 0 )
                 {
                    UDO_GetFirstField( epfAddr, iFldId, (char*)&dTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 else
                 {
                    UDO_GetCurrField( epfAddr, iCurrOffset, (char*)&dTmp,
                                     &iFldLen, &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-lf\n", iValIdx, iFldLen, dTmp );
                 fflush( fp );
                 break;
              default:
                 iFldLen = UDO_FieldLen( epfAddr, iFldId, iValIdx );
                 do {
		    pczTmp = ( char * )calloc( 1, iFldLen + 1 );
                 } while ( pczTmp == NULL );
                 if( iValIdx == 0 ) {
                    UDO_GetFirstField( epfAddr, iFldId, pczTmp, &iFldLen,
                                      &iNextOffset );
                 } else {
                    UDO_GetCurrField( epfAddr, iCurrOffset, pczTmp, &iFldLen,
                                      &iNextOffset );
                 }
                 fprintf( fp, "%-8d %-6d %-s\n", iValIdx, iFldLen, pczTmp );
                 fflush( fp );
                 free( pczTmp );
                 break;
           }
#endif /**************/

           iCurrOffset = iNextOffset;

       }
       fprintf( fp, "\n" ); fflush( fp );
       
    }
    
    if( epczFileName != NULL )
       fclose( fp );

    return( 0 );
}
