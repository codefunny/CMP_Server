#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <my_getopt.h>
#include <m_string.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "unload.h"

#define atomic_inc(x) __sync_add_and_fetch((x),1)
#define atomic_dec(x) __sync_sub_and_fetch((x),1)
#define atomic_add(x,y) __sync_add_and_fetch((x),(y))
#define atomic_sub(x,y) __sync_sub_and_fetch((x),(y))

MYSQL *mysql = NULL;

int db_connect( char *host, char *db, char *user, char *pass )
{
   int   iRet;
   char  *cUnixSock = 0;

   mysql = mysql_init( NULL );
   if( mysql == NULL )
   {
      printf( "mysql_init error: %s\n", mysql_error( mysql ) );
      return -1;
   }

   mysql_options (mysql, MYSQL_READ_DEFAULT_GROUP, "socket");

   if( !mysql_real_connect(mysql, host, user, pass, db, 3306, cUnixSock, 0 ) )
   {
      mysql_close( mysql );
      printf( "mysql_real_connect error: %s\n", mysql_error( mysql ) );
      return -1;
   }

		if (!mysql_set_character_set(mysql, "latin1"))
		{
		    printf("New client character set: %s\n",
		    mysql_character_set_name(mysql));
		}
   return 0;
}

void db_close( void )
{
   mysql_close( mysql );
}

void LockTables( void )
{
   char statement[10240];
   int  rc;

   printf( "正在锁表..." );
   sprintf( statement,
            "lock tables %s write, %s write, %s write, %s write, %s write,"
            "%s write, %s write, %s write, %s write, %s write, %s write,"
            "%s write, %s write",
            "msgnameid", "msgfldcfg", "svcflowcfg", "service", "nodecfg",
            "subservice", "tranfldcfg", "decision", "conditem", "subreqcfg",
            "subfldsrccfg", "logwatch", "itemkind" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "失败 : %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   printf( "成功\n" );
}

void UnLockTables( void )
{
   char statement[1024];
   int  rc;

   printf( "正在解除数据库锁..." );
   strcpy( statement, "unlock tables" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "失败 : %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   printf( "成功\n" );
}

/*DATA DECLARE */
int iRowMsgNameId;
struct msgnameid *pstMsgNameId;

int iRowMsgFldCfg;
struct msgfldcfg *pstMsgFldCfg;

int iRowSvcFlowCfg;
struct svcflowcfg *pstSvcFlowCfg;

int iRowService;
struct service *pstService;

int iRowNodeCfg;
struct nodecfg *pstNodeCfg;

int iRowSubService;
struct subservice *pstSubService;

int iRowTranFldCfg;
struct tranfldcfg *pstTranFldCfg;

int iRowDecision;
struct decision *pstDecision;

int iRowCondItem;
struct conditem *pstCondItem;

int iRowSubReqCfg;
struct subreqcfg *pstSubReqCfg;

int iRowSubFldSrcCfg;
struct subfldsrccfg *pstSubFldSrcCfg;

int iRowLogWatch;
struct logwatch *pstLogWatch;

int iRowItemKind;
struct itemkind *pstItemKind;

int readTableRow( char *table )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, rnum;

   sprintf( statement, "select count(*) from %s", table );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   row = mysql_fetch_row( result );
   if( row == NULL )
   {
      printf( "fetch row from %s error: %s\n", table, mysql_error(mysql) );
      exit( 0 );
   }

   rnum = atoi( row[0] );

   mysql_free_result( result );

   return rnum;
}

int readTranFldCfgRow( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, rnum;

   sprintf( statement, "select count(*) from tranfldcfg where kind=0" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   row = mysql_fetch_row( result );
   if( row == NULL )
   {
      printf( "fetch row from %s error: %s\n", "tranfldcfg",
              mysql_error(mysql) );
      exit( 0 );
   }

   rnum = atoi( row[0] );

   mysql_free_result( result );

   return rnum;
}
void readTablesRow( void )
{
   iRowMsgNameId    = readTableRow( "msgnameid" );
   iRowSvcFlowCfg   = readTableRow( "svcflowcfg" );
   iRowService      = readTableRow( "service" );
   iRowNodeCfg      = readTableRow( "nodecfg" );
   iRowSubService   = readTableRow( "subservice" );
   iRowTranFldCfg   = readTranFldCfgRow();
   iRowDecision     = readTableRow( "decision" );
   iRowCondItem     = readTableRow( "conditem" );
   iRowSubReqCfg    = readTableRow( "subreqcfg" );
   iRowSubFldSrcCfg = readTableRow( "subfldsrccfg" );
   iRowLogWatch     = readTableRow( "logwatch" );
   iRowItemKind     = readTableRow( "itemkind" );
}

void readMsgNameIdData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0;

   printf( "正在从数据库读 %s 表数据...", "msgnameid" );

   do
   {
      pstMsgNameId = (struct msgnameid*)malloc( sizeof(struct msgnameid)*
                                                iRowMsgNameId );
   } while( pstMsgNameId == NULL );
   
   strcpy( statement, "select * from msgnameid order by msgappid" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstMsgNameId+count)->m_Msgappid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstMsgNameId+count)->m_Parentid = atoi(row[1]);
      }
      else
      {
         (pstMsgNameId+count)->m_Parentid = 0;
      }
      if( row[2] != NULL )
      {
         strcpy( (pstMsgNameId+count)->m_Msgheaderqz, row[2] );
      }
      else
      {
         strcpy( (pstMsgNameId+count)->m_Msgheaderqz, "" );
      }
      if( row[3] != NULL )
      {
         strcpy( (pstMsgNameId+count)->m_Msgappname, row[3] );
      }
      else
      {
         strcpy( (pstMsgNameId+count)->m_Msgappname, "" );
      }
      count++;
   }
   if( !mysql_eof( result ) )
   {
      printf( "fetch row from msgnameid error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   mysql_free_result( result );
   printf( "完成\n" );
}

void readMsgFldCfgRow( void )
{
   int i;

   iRowMsgFldCfg = 0;
   for( i = 0; i < iRowMsgNameId; i++ )
   {
      MYSQL_RES *result;
      MYSQL_ROW row;
      char statement[1024];
      int  rc, rnum;

      if( (pstMsgNameId+i)->m_Parentid != 0 )
      {
         sprintf( statement,
                  "select count(*) from msgfldcfg where msgappid in (%d,%d)",
                  (pstMsgNameId+i)->m_Parentid, (pstMsgNameId+i)->m_Msgappid );
      }
      else
      {
         sprintf( statement,
                  "select count(*) from msgfldcfg where msgappid = %d",
                  (pstMsgNameId+i)->m_Msgappid );
      }
      rc = mysql_query( mysql, statement );
      if( rc != 0 )
      {
         printf( "Error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }
      result = mysql_store_result( mysql );
      if( result == NULL )
      {
         printf( "Error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }

      row = mysql_fetch_row( result );
      if( row == NULL )
      {
         printf( "fetch row from msgfldcfg error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }

      rnum = atoi( row[0] );

      mysql_free_result( result );
      iRowMsgFldCfg += rnum;
   }
}

void readMsgFldCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   readMsgFldCfgRow( );

   printf( "正在从数据库读 %s 表数据...", "msgfldcfg" );

   do
   {
      pstMsgFldCfg = (struct msgfldcfg*)malloc( sizeof(struct msgfldcfg)*
                                                iRowMsgFldCfg );
   } while( pstMsgFldCfg == NULL );

   for( i = 0; i < iRowMsgNameId; i++ )
   {
      if( (pstMsgNameId+i)->m_Parentid != 0 )
      {
         sprintf( statement,
                  "select * from msgfldcfg where msgappid in (%d,%d) "
                  "order by fldid",
                  (pstMsgNameId+i)->m_Msgappid, (pstMsgNameId+i)->m_Parentid );
      }
      else
      {
         sprintf( statement,
                  "select * from msgfldcfg where msgappid = %d "
                  "order by msgappid, fldid",
                  (pstMsgNameId+i)->m_Msgappid );
      }
      rc = mysql_query( mysql, statement );
      if( rc != 0 )
      {
         printf( "Error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }
      result = mysql_store_result( mysql );
      if( result == NULL )
      {
         printf( "Error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }

      while( (row = mysql_fetch_row( result )) != NULL )
      {
         mysql_field_seek( result, 0 );
         (pstMsgFldCfg+count)->m_Msgappid = (pstMsgNameId+i)->m_Msgappid;
         (pstMsgFldCfg+count)->m_Fldid = atoi(row[1]);
         if( row[2] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldengname, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldengname, row[2] );
         }
         if( row[3] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldchnname, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldchnname, row[3] );
         }
         if( row[4] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldmsgkind = -1;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldmsgkind = atoi(row[4]);
         }
         if( row[5] == NULL )
         {
            (pstMsgFldCfg+count)->m_Lenfldlen = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Lenfldlen = atoi(row[5]);
         }
         if( row[6] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldlen = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldlen = atoi(row[6]);
         }
         if( row[7] == NULL )
         {
            (pstMsgFldCfg+count)->m_Lenfldtype = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Lenfldtype = atoi(row[7]);
         }
         if( row[8] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldtype = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldtype = atoi(row[8]);
         }
         if( row[9] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Lenflddef, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Lenflddef, row[9] );
         }
         if( row[10] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Flddef, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Flddef, row[10] );
         }
         if( row[11] == NULL )
         {
            (pstMsgFldCfg+count)->m_Lenfldalign = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Lenfldalign = atoi(row[11]);
         }
         if( row[12] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldalign = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldalign = atoi(row[12]);
         }
         if( row[13] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Sepsymbol, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Sepsymbol, row[13] );
         }
         if( row[14] == NULL )
         {
            (pstMsgFldCfg+count)->m_Tomidid = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Tomidid = atoi(row[14]);
         }
         if( row[15] == NULL )
         {
            (pstMsgFldCfg+count)->m_Tomsgid = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Tomsgid = atoi(row[15]);
         }
         if( row[16] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldtag = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldtag = atoi(row[16]);
         }
         if( row[17] == NULL )
         {
            (pstMsgFldCfg+count)->m_Fldexprid = 0;
         }
         else
         {
            (pstMsgFldCfg+count)->m_Fldexprid = atoi(row[17]);
         }
         if( row[18] == NULL )
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldmacro, "" );
         }
         else
         {
            strcpy( (pstMsgFldCfg+count)->m_Fldmacro, row[18] );
         }
         count++;
      }
      if( !mysql_eof( result ) )
      {
         printf( "fetch row from msgnameid error: %s\n", mysql_error(mysql) );
         exit( 0 );
      }

      mysql_free_result( result );
   }
   printf( "完成\n" );
}

void readSvcFlowCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "svcflowcfg" );

   do
   {
      pstSvcFlowCfg = (struct svcflowcfg*)malloc( sizeof(struct svcflowcfg)*
                                                  iRowSvcFlowCfg );
   } while( pstSvcFlowCfg == NULL );

   strcpy( statement, "select * from svcflowcfg order by svcid, subseq" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstSvcFlowCfg+count)->m_Svcid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Seq = atoi(row[1]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Seq = 0;
      }
      if( row[2] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Subid = atoi(row[2]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Subid = 0;
      }
      if( row[3] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Subseq = atoi(row[3]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Subseq = 0;
      }
      if( row[4] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Kind = atoi(row[4]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Kind = 0;
      }
      if( row[5] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Subretimes = atoi(row[5]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Subretimes = 0;
      }
      if( row[6] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Lstid = atoi(row[6]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Lstid = 0;
      }
      if( row[7] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Crckind = atoi(row[7]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Crckind = 0;
      }
      if( row[8] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Crcsubid = atoi(row[8]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Crcsubid = 0;
      }
      if( row[9] != NULL )
      {
         (pstSvcFlowCfg+count)->m_Outtime = atoi(row[9]);
      }
      else
      {
         (pstSvcFlowCfg+count)->m_Outtime = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readServiceData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "service" );

   do
   {
      pstService = (struct service*)malloc( sizeof(struct service)*
                                            iRowService );
   } while( pstService == NULL );

   strcpy( statement,
           "select svcid,svcname,nodeid,trancode,timeout,subnodeid "
           "from service "
           "order by svcid, nodeid, trancode" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstService+count)->m_Svcid = atoi(row[0]);
      if( row[1] != NULL )
      {
         strcpy( (pstService+count)->m_Svcname, row[1] );
      }
      else
      {
         strcpy( (pstService+count)->m_Svcname, "" );
      }
      if( row[2] != NULL )
      {
         (pstService+count)->m_Nodeid = atoi(row[2]);
      }
      else
      {
         (pstService+count)->m_Nodeid = 0;
      }
      if( row[3] != NULL )
      {
         strcpy( (pstService+count)->m_Trancode, row[3] );
      }
      else
      {
         strcpy( (pstService+count)->m_Trancode, "" );
      }
      if( row[4] != NULL )
      {
         (pstService+count)->m_Timeout = atoi(row[4]);
      }
      else
      {
         (pstService+count)->m_Timeout = 30;
      }
      (pstService+count)->m_Subnodeid = atoi(row[5]);
      count++;
   }
   printf( "完成\n" );
}

void readNodeCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "nodecfg" );

   do
   {
      pstNodeCfg = (struct nodecfg*)malloc( sizeof(struct nodecfg)*
                                            iRowNodeCfg );
   } while( pstNodeCfg == NULL );

   strcpy( statement, "select * from nodecfg order by nodeid" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstNodeCfg+count)->m_Nodeid = atoi(row[0]);
      if( row[1] != NULL )
      {
         strcpy( (pstNodeCfg+count)->m_Nodename, row[1] );
      }
      else
      {
         strcpy( (pstNodeCfg+count)->m_Nodename, "" );
      }
      if( row[2] != NULL )
      {
         (pstNodeCfg+count)->m_Nodedscrb = atoi(row[2]);
      }
      else
      {
         (pstNodeCfg+count)->m_Nodedscrb = 0;
      }
      if( row[3] != NULL )
      {
         (pstNodeCfg+count)->m_Nodetype = atoi(row[3]);
      }
      else
      {
         (pstNodeCfg+count)->m_Nodetype = 0;
      }
      if( row[4] != NULL )
      {
         (pstNodeCfg+count)->m_Msgappid = atoi(row[4]);
      }
      else
      {
         (pstNodeCfg+count)->m_Msgappid = 0;
      }
      if( row[5] != NULL )
      {
         strcpy( (pstNodeCfg+count)->m_Ipaddr, row[5] );
      }
      else
      {
         strcpy( (pstNodeCfg+count)->m_Ipaddr, "" );
      }
      if( row[6] != NULL )
      {
         (pstNodeCfg+count)->m_Commport = atoi(row[6]);
      }
      else
      {
         (pstNodeCfg+count)->m_Commport = 0;
      }
      if( row[7] != NULL )
      {
         (pstNodeCfg+count)->m_Svrkeyid = atoi(row[7]);
      }
      else
      {
         (pstNodeCfg+count)->m_Svrkeyid = 0;
      }
      if( row[8] != NULL )
      {
         (pstNodeCfg+count)->m_Svwkeyid = atoi(row[8]);
      }
      else
      {
         (pstNodeCfg+count)->m_Svwkeyid = 0;
      }
      if( row[9] != NULL )
      {
         (pstNodeCfg+count)->m_Tomidid = atoi(row[9]);
      }
      else
      {
         (pstNodeCfg+count)->m_Tomidid = 0;
      }
      if( row[10] != NULL )
      {
         (pstNodeCfg+count)->m_Tomsgid = atoi(row[10]);
      }
      else
      {
         (pstNodeCfg+count)->m_Tomsgid = 0;
      }
      if( row[11] != NULL )
      {
         (pstNodeCfg+count)->m_Logflag = atoi(row[11]);
      }
      else
      {
         (pstNodeCfg+count)->m_Logflag = 0;
      }
      if( row[12] != NULL )
      {
         (pstNodeCfg+count)->m_Watchflag = atoi(row[12]);
      }
      else
      {
         (pstNodeCfg+count)->m_Watchflag = 0;
      }
      if( row[13] != NULL )
      {
         (pstNodeCfg+count)->m_Maxretry = atoi(row[13]);
      }
      else
      {
         (pstNodeCfg+count)->m_Maxretry = 0;
      }
      if( row[14] != NULL )
      {
         (pstNodeCfg+count)->m_Delay = atoi(row[14]);
      }
      else
      {
         (pstNodeCfg+count)->m_Delay = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readSubServiceData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "subservice" );

   do
   {
      pstSubService = (struct subservice*)malloc( sizeof(struct subservice)*
                                                  iRowSubService );
   } while( pstSubService == NULL );

   strcpy( statement,
           "select * from subservice order by subid, nodeid, trancode" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstSubService+count)->m_Subid = atoi(row[0]);
      if( row[1] != NULL )
      {
         strcpy( (pstSubService+count)->m_Subname, row[1] );
      }
      else
      {
         strcpy( (pstSubService+count)->m_Subname, "" );
      }
      if( row[2] != NULL )
      {
         (pstSubService+count)->m_Templetid = atoi(row[2]);
      }
      else
      {
         (pstSubService+count)->m_Templetid = 0;
      }
      if( row[3] != NULL )
      {
         (pstSubService+count)->m_Nodeid = atoi(row[3]);
      }
      else
      {
         (pstSubService+count)->m_Nodeid = 0;
      }
      if( row[4] != NULL )
      {
         strcpy( (pstSubService+count)->m_Trancode, row[4] );
      }
      else
      {
         strcpy( (pstSubService+count)->m_Trancode, "" );
      }
      if( row[5] != NULL )
      {
         (pstSubService+count)->m_Timeout = atoi( row[5] );
      }
      else
      {
         (pstSubService+count)->m_Timeout = 0;
      }
      if( row[6] != NULL )
      {
         strcpy( (pstSubService+count)->m_Remark, row[6] );
      }
      else
      {
         strcpy( (pstSubService+count)->m_Remark, "" );
      }
      (pstSubService+count)->m_Subnodeid = atoi( row[7] );
      count++;
   }
   printf( "完成\n" );
}

void readTranFldCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "tranfldcfg" );

   do
   {
      pstTranFldCfg = (struct tranfldcfg*)malloc( sizeof(struct tranfldcfg)*
                                                  iRowTranFldCfg );
   } while( pstTranFldCfg == NULL );

   strcpy( statement,
           "select * from tranfldcfg where kind = 0 "
           "order by nodeid,trancode,msgdscrb,tranfldseq" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstTranFldCfg+count)->m_Nodeid = atoi(row[0]);
      if( row[1] != NULL )
      {
         strcpy( (pstTranFldCfg+count)->m_Trancode, row[1] );
      }
      else
      {
         strcpy( (pstTranFldCfg+count)->m_Trancode, "" );
      }
      if( row[2] != NULL )
      {
         (pstTranFldCfg+count)->m_Msgdscrb = atoi(row[2]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Msgdscrb = 0;
      }
      if( row[3] != NULL )
      {
         (pstTranFldCfg+count)->m_Tranfldseq = atoi(row[3]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Tranfldseq = 0;
      }
      if( row[4] != NULL )
      {
         (pstTranFldCfg+count)->m_Tranfldid = atoi(row[4]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Tranfldid = 0;
      }
      if( row[5] != NULL )
      {
         (pstTranFldCfg+count)->m_Repflag = atoi( row[5] );
      }
      else
      {
         (pstTranFldCfg+count)->m_Repflag = 0;
      }
      if( row[6] != NULL )
      {
         (pstTranFldCfg+count)->m_Reptmfldid = atoi(row[6]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Reptmfldid = 0;
      }
      if( row[7] != NULL )
      {
         (pstTranFldCfg+count)->m_Reptype = atoi(row[7]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Reptype = 0;
      }
      if( row[8] != NULL )
      {
         (pstTranFldCfg+count)->m_Parentfldseq = atoi(row[8]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Parentfldseq = 0;
      }
      if( row[9] != NULL )
      {
         strcpy( (pstTranFldCfg+count)->m_Repfldname, row[9]);
      }
      else
      {
         strcpy( (pstTranFldCfg+count)->m_Repfldname, "" );
      }
      if( row[10] != NULL )
      {
         (pstTranFldCfg+count)->m_Fldlevel = atoi(row[10]);
      }
      else
      {
         (pstTranFldCfg+count)->m_Fldlevel = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readDecisionData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "decision" );

   do
   {
      pstDecision = (struct decision*)malloc( sizeof(struct decision)*
                                              iRowDecision );
   } while( pstDecision == NULL );

   strcpy( statement,
           "select * from decision order by svcid, cursubseq, condno" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstDecision+count)->m_Svcid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstDecision+count)->m_Cursubseq = atoi( row[1] );
      }
      else
      {
         (pstDecision+count)->m_Cursubseq = 0;
      }
      if( row[2] != NULL )
      {
         (pstDecision+count)->m_Seq = atoi(row[2]);
      }
      else
      {
         (pstDecision+count)->m_Seq = 0;
      }
      if( row[3] != NULL )
      {
         (pstDecision+count)->m_Condid = atoi(row[3]);
      }
      else
      {
         (pstDecision+count)->m_Condid = 0;
      }
      if( row[4] != NULL )
      {
         strcpy( (pstDecision+count)->m_Expression, row[4]);
      }
      else
      {
         strcpy( (pstDecision+count)->m_Expression, "" );
      }
      if( row[5] != NULL )
      {
         (pstDecision+count)->m_Nextsubseq = atoi( row[5] );
      }
      else
      {
         (pstDecision+count)->m_Nextsubseq = 0;
      }
      if( row[6] != NULL )
      {
         (pstDecision+count)->m_Funid = atoi(row[6]);
      }
      else
      {
         (pstDecision+count)->m_Funid = 0;
      }
      if( row[7] != NULL )
      {
         (pstDecision+count)->m_Dfltflag = atoi(row[7]);
      }
      else
      {
         (pstDecision+count)->m_Dfltflag = 0;
      }
      if( row[8] != NULL )
      {
         (pstDecision+count)->m_Kind = atoi(row[8]);
      }
      else
      {
         (pstDecision+count)->m_Kind = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readCondItemData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "conditem" );

   do
   {
      pstCondItem = (struct conditem*)malloc( sizeof(struct conditem)*
                                              iRowCondItem );
   } while( pstCondItem == NULL );

   strcpy( statement, "select * from conditem order by condid,exprid" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstCondItem+count)->m_Condid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstCondItem+count)->m_Exprid = atoi( row[1] );
      }
      else
      {
         (pstCondItem+count)->m_Exprid = 0;
      }
      if( row[2] != NULL )
      {
         (pstCondItem+count)->m_Lsubseq = atoi(row[2]);
      }
      else
      {
         (pstCondItem+count)->m_Lsubseq = 0;
      }
      if( row[3] != NULL )
      {
         (pstCondItem+count)->m_Lmsgdscrb = atoi(row[3]);
      }
      else
      {
         (pstCondItem+count)->m_Lmsgdscrb = 0;
      }
      if( row[4] != NULL )
      {
         (pstCondItem+count)->m_Lfldid = atoi(row[4]);
      }
      else
      {
         (pstCondItem+count)->m_Lfldid = 0;
      }
      if( row[5] != NULL )
      {
         (pstCondItem+count)->m_Lfldtype = atoi( row[5] );
      }
      else
      {
         (pstCondItem+count)->m_Lfldtype = 0;
      }
      if( row[6] != NULL )
      {
         strcpy( (pstCondItem+count)->m_Lvalue, row[6]);
      }
      else
      {
         strcpy( (pstCondItem+count)->m_Lvalue, "" );
      }
      if( row[7] != NULL )
      {
         (pstCondItem+count)->m_Lstartbit = atoi(row[7]);
      }
      else
      {
         (pstCondItem+count)->m_Lstartbit = 0;
      }
      if( row[8] != NULL )
      {
         (pstCondItem+count)->m_Lendbit = atoi(row[8]);
      }
      else
      {
         (pstCondItem+count)->m_Lendbit = 0;
      }
      if( row[9] != NULL )
      {
         (pstCondItem+count)->m_Op = atoi(row[9]);
      }
      else
      {
         (pstCondItem+count)->m_Op = 0;
      }
      if( row[10] != NULL )
      {
         (pstCondItem+count)->m_Rsubseq = atoi(row[10]);
      }
      else
      {
         (pstCondItem+count)->m_Rsubseq = 0;
      }
      if( row[11] != NULL )
      {
         (pstCondItem+count)->m_Rmsgdscrb = atoi(row[11]);
      }
      else
      {
         (pstCondItem+count)->m_Rmsgdscrb = 0;
      }
      if( row[12] != NULL )
      {
         (pstCondItem+count)->m_Rfldid = atoi(row[12]);
      }
      else
      {
         (pstCondItem+count)->m_Rfldid = 0;
      }
      if( row[13] != NULL )
      {
         (pstCondItem+count)->m_Rfldtype = atoi(row[13]);
      }
      else
      {
         (pstCondItem+count)->m_Rfldtype = 0;
      }
      if( row[14] != NULL )
      {
         strcpy( (pstCondItem+count)->m_Rvalue, row[14]);
      }
      else
      {
         strcpy( (pstCondItem+count)->m_Rvalue, "" );
      }
      if( row[15] != NULL )
      {
         (pstCondItem+count)->m_Rstartbit = atoi(row[15]);
      }
      else
      {
         (pstCondItem+count)->m_Rstartbit = 0;
      }
      if( row[16] != NULL )
      {
         (pstCondItem+count)->m_Rendbit = atoi(row[16]);
      }
      else
      {
         (pstCondItem+count)->m_Rendbit = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readSubReqCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "subreqcfg" );

   do
   {
      pstSubReqCfg = (struct subreqcfg*)malloc( sizeof(struct subreqcfg)*
                                                iRowSubReqCfg );
   } while( pstSubReqCfg == NULL );

   strcpy( statement, "select * from subreqcfg "
           "order by svcid, subseq, balflag, fldseq" );
/**           "order by svcid, subseq, balflag, fldid, fldseq" ); **/
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstSubReqCfg+count)->m_Svcid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstSubReqCfg+count)->m_Subseq = atoi( row[1] );
      }
      else
      {
         (pstSubReqCfg+count)->m_Subseq = 0;
      }
      if( row[2] != NULL )
      {
         (pstSubReqCfg+count)->m_Fldid = atoi(row[2]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Fldid = 0;
      }
      if( row[3] != NULL )
      {
         (pstSubReqCfg+count)->m_Fldseq = atoi(row[3]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Fldseq = 0;
      }
      if( row[4] != NULL )
      {
         (pstSubReqCfg+count)->m_Reptmsubseq = atoi(row[4]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Reptmsubseq = 0;
      }
      if( row[5] != NULL )
      {
         (pstSubReqCfg+count)->m_Reptmmsgdscrb = atoi( row[5] );
      }
      else
      {
         (pstSubReqCfg+count)->m_Reptmmsgdscrb = 0;
      }
      if( row[6] != NULL )
      {
         (pstSubReqCfg+count)->m_Reptmfldid = atoi(row[6]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Reptmfldid = 0;
      }
      if( row[7] != NULL )
      {
         (pstSubReqCfg+count)->m_Asmfunid = atoi(row[7]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Asmfunid = 0;
      }
      if( row[8] != NULL )
      {
         (pstSubReqCfg+count)->m_Asmid = atoi(row[8]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Asmid = 0;
      }
      if( row[9] != NULL )
      {
         (pstSubReqCfg+count)->m_Balflag = atoi(row[9]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Balflag = 0;
      }
      if( row[10] != NULL )
      {
         (pstSubReqCfg+count)->m_Kind = atoi(row[10]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Kind = 0;
      }
      if( row[11] != NULL )
      {
         (pstSubReqCfg+count)->m_Repflag = atoi(row[11]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Repflag = 0;
      }
      if( row[12] != NULL )
      {
         (pstSubReqCfg+count)->m_Fldlevel = atoi(row[12]);
      }
      else
      {
         (pstSubReqCfg+count)->m_Fldlevel = 0;
      }
      count++;
   }
   printf( "完成\n" );
}

void readSubFldSrcCfgData( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   unsigned long *lengths;
   char statement[1024];
   int  rc, count=0, i;

   printf( "正在从数据库读 %s 表数据...", "subfldsrccfg" );

   do
   {
      pstSubFldSrcCfg =(struct subfldsrccfg*)malloc(sizeof(struct subfldsrccfg)*
                                                    iRowSubFldSrcCfg );
   } while( pstSubFldSrcCfg == NULL );

   strcpy( statement, "select * from subfldsrccfg "
           "order by asmid, srcseq" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "Error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      lengths = mysql_fetch_lengths( result );
      (pstSubFldSrcCfg+count)->m_Asmid = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcseq = atoi( row[1] );
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcseq = 0;
      }
      if( row[2] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcmsgdscrb = atoi(row[2]);
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcmsgdscrb = 0;
      }
      if( row[3] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcsubseq = atoi(row[3]);
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcsubseq = 0;
      }
      if( row[4] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcfldid = atoi(row[4]);
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcfldid = 0;
      }
      if( row[5] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcbeginbit = atoi( row[5] );
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcbeginbit = 0;
      }
      if( row[6] != NULL )
      {
         (pstSubFldSrcCfg+count)->m_Srcendbit = atoi(row[6]);
      }
      else
      {
         (pstSubFldSrcCfg+count)->m_Srcendbit = 0;
      }
      if( row[7] != NULL )
      {
         strcpy( (pstSubFldSrcCfg+count)->m_Srcnote, row[7] );
         if( (pstSubFldSrcCfg+count)->m_Srcsubseq == 0 &&
             (pstSubFldSrcCfg+count)->m_Srcfldid == 0 && strlen(row[7]) == 0 )
         {
            if( (pstSubFldSrcCfg+count)->m_Srcendbit <
                sizeof((pstSubFldSrcCfg+count)->m_Srcnote) )
            {
               memset( (pstSubFldSrcCfg+count)->m_Srcnote, ' ',
                       (pstSubFldSrcCfg+count)->m_Srcendbit );
            }
            else
            {
               memset( (pstSubFldSrcCfg+count)->m_Srcnote, ' ',
                       sizeof((pstSubFldSrcCfg+count)->m_Srcnote) - 1 );
            }
         }
      }
      else
      {
         strcpy( (pstSubFldSrcCfg+count)->m_Srcnote, "" );
      }
      count++;
   }
   printf( "完成\n" );
}

void readLogWatch( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0;

   printf( "正在从数据库读 %s 表数据...", "logwatch" );

   do
   {
      pstLogWatch = (struct logwatch*)malloc( sizeof(struct logwatch)*
                                               iRowLogWatch );
   } while( pstLogWatch == NULL );
   
   strcpy( statement,
           "select * from logwatch order by tag, nodeid, dictid, fldid" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      (pstLogWatch+count)->m_Tag = atoi(row[0]);
      if( row[1] != NULL )
      {
         (pstLogWatch+count)->m_Nodeid = atoi(row[1]);
      }
      else
      {
         (pstLogWatch+count)->m_Nodeid = 0;
      }
      if( row[2] != NULL )
      {
         (pstLogWatch+count)->m_Dictid = atoi( row[2] );
      }
      else
      {
         (pstLogWatch+count)->m_Dictid = 0;
      }
      if( row[3] != NULL )
      {
         (pstLogWatch+count)->m_Fldid = atoi( row[3] );
      }
      else
      {
         (pstLogWatch+count)->m_Fldid = 0;
      }
      if( row[4] != NULL )
      {
         (pstLogWatch+count)->m_Fldtype = atoi( row[4] );
      }
      else
      {
         (pstLogWatch+count)->m_Fldtype = 0;
      }
      count++;
   }
   if( !mysql_eof( result ) )
   {
      printf( "fetch row from msgnameid error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   mysql_free_result( result );
   printf( "完成\n" );
}

void readItemKind( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   char statement[1024];
   int  rc, count=0;

   printf( "正在从数据库读 %s 表数据...", "itemkind" );

   do
   {
      pstItemKind = (struct itemkind*)malloc( sizeof(struct itemkind)*
                                               iRowItemKind );
   } while( pstItemKind == NULL );
   
   strcpy( statement, "select * from itemkind order by nodeid, subnodeid" );
   rc = mysql_query( mysql, statement );
   if( rc != 0 )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }
   result = mysql_store_result( mysql );
   if( result == NULL )
   {
      printf( "失败: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   while( (row = mysql_fetch_row( result )) != NULL )
   {
      mysql_field_seek( result, 0 );
      if( row[0] != NULL )
      {
         (pstItemKind+count)->m_NodeId = atoi(row[0]);
      }
      else
      {
         (pstItemKind+count)->m_NodeId = 0;
      }
      if( row[1] != NULL )
      {
         (pstItemKind+count)->m_SubNodeId = atoi( row[1] );
      }
      else
      {
         (pstItemKind+count)->m_SubNodeId = 0;
      }
      if( row[2] != NULL )
      {
         strcpy( (pstItemKind+count)->m_Name, row[2] );
      }
      else
      {
         strcpy( (pstItemKind+count)->m_Name, "" );
      }
      count++;
   }
   if( !mysql_eof( result ) )
   {
      printf( "fetch row from msgnameid error: %s\n", mysql_error(mysql) );
      exit( 0 );
   }

   mysql_free_result( result );
   printf( "完成\n" );
}

void readTablesData( void )
{
   readMsgNameIdData( );  /*数据字典定义数据       */
   readMsgFldCfgData( );  /*数据字典字段定义数据   */
   readSvcFlowCfgData( ); /*流程配置数据           */
   readServiceData( );    /*服务配置数据           */
   readNodeCfgData( );    /*节点配置数据           */
   readSubServiceData( ); /*子服务配置数据         */
   readTranFldCfgData( ); /*交易字段配置数据       */
   readDecisionData( );   /*条件检查数据           */
   readCondItemData( );   /*条件检查分项数据       */
   readSubReqCfgData( );  /*重组目标配置数据       */
   readSubFldSrcCfgData();/*重组源配置数据         */
   readLogWatch();        /*监控和日志定义字段     */
   readItemKind( );       /*各节点分类             */
}

void readDataFromDb( void )
{
   /*首先将数据库表加锁,保证数据开始读和结束读操作之间数据是一致的 */
   LockTables( );

   readTablesRow( );

   readTablesData( );

   UnLockTables( );
}

void createMsgFldCfg( FILE *fp )
{
   char line[2048];
   int i;

   for( i = 0; i < iRowMsgFldCfg; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstMsgFldCfg+i)->m_Msgappid );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Fldid );
      sprintf( line, "%s%d|", line, 1 );
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Fldengname );
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Fldchnname );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Fldmsgkind );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Lenfldlen );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Fldlen );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Lenfldtype );
      sprintf( line, "%s%d|", line, (pstMsgFldCfg+i)->m_Fldtype );
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Lenflddef );
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Flddef );
      if( (pstMsgFldCfg+i)->m_Lenfldalign == 0 )
      {
         sprintf( line, "%s%c|", line, 'L' );
      }
      else
      {
         sprintf( line, "%s%c|", line, 'R' );
      }
      if( (pstMsgFldCfg+i)->m_Fldalign == 0 )
      {
         sprintf( line, "%s%c|", line, 'L' );
      }
      else
      {
         sprintf( line, "%s%c|", line, 'R' );
      }
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Sepsymbol );
      sprintf( line, "%s%s|", line, (pstMsgFldCfg+i)->m_Fldmacro );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createMsgNameId( FILE *fp )
{
   char line[2048];
   int i;

   for( i = 0; i < iRowMsgNameId; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstMsgNameId+i)->m_Msgappid );
      sprintf( line, "%s%s|", line, (pstMsgNameId+i)->m_Msgappname );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createNodeTranCfg( FILE *fp )
{
   char line[2048];
   int i, j;

   for( i = 0; i < iRowNodeCfg; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstNodeCfg+i)->m_Nodeid );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Msgappid );
      /*通过数据字典编号找到数据字典名字 */
      for( j = 0; j < iRowMsgNameId; j++ )
      {
         if( (pstMsgNameId+j)->m_Msgappid == (pstNodeCfg+i)->m_Msgappid )
         {
            break;
         }
      }
      if( j == iRowMsgNameId )
      {
         printf( "NODETRANCFG: dictid %d not in msgnameid\n",
                 (pstNodeCfg+i)->m_Msgappid );
         continue;
      }
      sprintf( line, "%s%s|", line, (pstMsgNameId+j)->m_Msgappname );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Tomidid );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Tomsgid );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      /** 增加日志记录表名和字段配置        **/
      /** 如果配置的日志字段数大于1设置表名 **/
      {
         char aczFld[1201], aczTable[21];
         int  l, lNum;
         /** 查找logwatch表中tag=0 nodeid=(pstNodeCfg+i)->m_Nodeid **/
         lNum = 0;
         strcpy( aczFld, "" );
         for( l = 0; l < iRowLogWatch; l++ )
         {
            if( (pstLogWatch+l)->m_Tag == 0 &&
                (pstLogWatch+l)->m_Nodeid == (pstNodeCfg+i)->m_Nodeid )
            {
               lNum++;
               sprintf( aczFld, "%sf_%d_%d,", aczFld,
                        (pstLogWatch+l)->m_Dictid, (pstLogWatch+l)->m_Fldid );
            }
         }
         if( lNum > 0 )
         {
            /* 表名     */
            sprintf( line, "%snodelog_%d|", line, (pstNodeCfg+i)->m_Nodeid );
            sprintf( line, "%s%s|", line, aczFld );    /* 字段组成 */
         }
         else
         {
            sprintf( line, "%s%s|", line, "" );
            sprintf( line, "%s%s|", line, "" );    /* 字段组成 */
         }
      }
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Maxretry );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Delay );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

int checkNeedMonitor( int nodeId )
{
   int i;
   for( i = 0; i < iRowLogWatch; i++ )
   {
      if( (pstLogWatch+i)->m_Nodeid == nodeId )
      {
         return 1;
      }
   }

   return 0;
}

void createNodeCommCfg( FILE *fp )
{
   char line[2048];
   int i, iNeedMonitor;

   for( i = 0; i < iRowNodeCfg; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstNodeCfg+i)->m_Nodeid );
      sprintf( line, "%s%s|", line, (pstNodeCfg+i)->m_Nodename );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Nodedscrb );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%s|", line, (pstNodeCfg+i)->m_Ipaddr );
      sprintf( line, "%s%s|", line, (pstNodeCfg+i)->m_Ipaddr );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Commport );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      /** 将监控端口改为是否监控 **/
      iNeedMonitor = checkNeedMonitor( (pstNodeCfg+i)->m_Nodeid );
      sprintf( line, "%s%d|", line, iNeedMonitor );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Msgappid );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 10 );
      sprintf( line, "%s%d|", line, 20 );
      sprintf( line, "%s%d|", line, 1 );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Svrkeyid );
      sprintf( line, "%s%d|", line, (pstNodeCfg+i)->m_Svwkeyid );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createService( FILE *fp )
{
   char line[2048];
   int i;

   for( i = 0; i < iRowService; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstService+i)->m_Nodeid );
      sprintf( line, "%s%s|", line, (pstService+i)->m_Trancode );
      sprintf( line, "%s%d|", line, (pstService+i)->m_Svcid );
      sprintf( line, "%s%s|", line, (pstService+i)->m_Svcname );
      /* 服务分类 */
      sprintf( line, "%s%d|", line, (pstService+i)->m_Subnodeid );
      sprintf( line, "%s%d|", line, 0 );
      sprintf( line, "%s%d|", line, 0 );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createServiceCfg( FILE *fp )
{
   char line[2048];
   int i, j, k, l;

   for( i = 0; i < iRowSvcFlowCfg; i++ )
   {
      if( (pstSvcFlowCfg+i)->m_Kind != 3 ) /* 3 表示联线 */
      {
         /*先找与decision的关系 */
         for( j = 0; j < iRowDecision; j++ )
         {
            if( (pstSvcFlowCfg+i)->m_Svcid == (pstDecision+j)->m_Svcid &&
                (pstSvcFlowCfg+i)->m_Subseq == (pstDecision+j)->m_Nextsubseq )
            {
               break;
            }
         }
         if( j == iRowDecision )
         {
            if( (pstSvcFlowCfg+i)->m_Subseq != 0 )
            {
               printf( "%d SERVICECFG: svcid %d and subseq %d not in "
                       "decision\n", __LINE__,
                       (pstSvcFlowCfg+i)->m_Svcid, (pstSvcFlowCfg+i)->m_Subseq);
            }
            continue;
         }
         /*再找与subservice的关系 */
         for( k = 0; k < iRowSubService; k++ )
         {
            if( (pstSvcFlowCfg+i)->m_Subid == (pstSubService+k)->m_Subid )
            {
               break;
            }
         }
         if( k == iRowSubService && (pstSvcFlowCfg+i)->m_Subid != -1 )
         {
            printf( "%d SERVICECFG: svcid %d and subseq %d subid %d not in "
                    "subservice\n", __LINE__,
                    (pstSvcFlowCfg+i)->m_Svcid, (pstSvcFlowCfg+i)->m_Subseq,
                    (pstSvcFlowCfg+i)->m_Subid );
            continue;
         }
         /*最后查找与service的关系 */
         for( l = 0; l < iRowService; l++ )
         {
            if( (pstSvcFlowCfg+i)->m_Svcid == (pstService+l)->m_Svcid )
            {
               break;
            }
         }
         if( l == iRowService )
         {
            printf( "%d SERVICECFG: svcid(in svcflowcfg subseq %d) %d not in "
                    "service\n", __LINE__,
                    (pstSvcFlowCfg+i)->m_Svcid, (pstSvcFlowCfg+i)->m_Subseq );
            continue;
         }
         sprintf( line, "%d|", (pstService+l)->m_Nodeid );
         sprintf( line, "%s%s|", line, (pstService+l)->m_Trancode );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Svcid );
         sprintf( line, "%s%d|", line, (pstSubService+k)->m_Nodeid );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Subseq );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Subid );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Crckind );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Crcsubid );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Subretimes );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Outtime );
         sprintf( line, "%s%d|", line, (pstDecision+j)->m_Cursubseq );
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, (pstSvcFlowCfg+i)->m_Lstid );
         fprintf( fp, "%s\n", line );
         fflush( fp );
      }
   }
}

void createSubService( FILE *fp )
{
   char line[2048];
   int i, j;

   for( i = 0; i < iRowSubService; i++ )
   {
      memset( line, 0, sizeof(line) );
      for( j = 0; j < iRowNodeCfg; j++ )
      {
         if( (pstSubService+i)->m_Nodeid == (pstNodeCfg+j)->m_Nodeid )
         {
            break;
         }
      }
      if( j == iRowNodeCfg )
      {
         printf( "SUBSERVICE: nodeid %d not in nodecfg\n",
                 (pstSubService+i)->m_Nodeid );
         continue;
      }
      sprintf( line, "%d|", (pstSubService+i)->m_Nodeid );
      sprintf( line, "%s%s|", line, (pstSubService+i)->m_Trancode );
      sprintf( line, "%s%d|", line, (pstSubService+i)->m_Subid );
      sprintf( line, "%s%s|", line, (pstSubService+i)->m_Subname );
      sprintf( line, "%s%d|", line, (pstSubService+i)->m_Templetid );
      sprintf( line, "%s%d|", line, (pstSubService+i)->m_Timeout );
      sprintf( line, "%s%d|", line, 0 );
      /* 子服务分类 */
      sprintf( line, "%s%d|", line, (pstSubService+i)->m_Subnodeid );
      sprintf( line, "%s%d|", line, (pstNodeCfg+j)->m_Msgappid );
      sprintf( line, "%s%s|", line, (pstSubService+i)->m_Remark );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createTranFldCfg( FILE *fp )
{
   char flds[2048], reptimesseq[100], reptimesnum[100], reptype[50];
   int  i, j, k;
   int  newtranfld = 1, hasrep = 0;
   int  nodeid;
   char trancode[64];
   int  msgdscrb;
   int  repseq = -1, reptimes = -1;
   int  fldid, lastisrep, tranfldnum;
/**
   int  repflag = 0;
***/
   int  repflag[20];
/***/
   int  last_repflag = 0;
   int  fldlevel = 0, last_fldlevel = 0;
   char repfldname[20][41];

   memset (repfldname, 0, sizeof (repfldname));
   memset (repflag, 0, sizeof (repflag));

   for( i = 0; i < iRowTranFldCfg; i++ )
   {
      /** 查找该交易码和指定报文方向的所有字段组成 **/
      memset( flds, 0, sizeof(flds) );
      memset( reptimesseq, 0, sizeof(reptimesseq) );
      memset( reptimesnum, 0, sizeof(reptimesnum) );
      memset( reptype,     0, sizeof(reptype) );
      reptimes   = -1;
      tranfldnum = 0;
      nodeid = (pstTranFldCfg+i)->m_Nodeid;
      strcpy( trancode, (pstTranFldCfg+i)->m_Trancode );
      msgdscrb = (pstTranFldCfg+i)->m_Msgdscrb;

      while( i < iRowTranFldCfg &&
             nodeid == (pstTranFldCfg+i+1)->m_Nodeid &&
             strcmp( trancode, (pstTranFldCfg+i+1)->m_Trancode ) == 0 &&
             msgdscrb == (pstTranFldCfg+i+1)->m_Msgdscrb )
      {
         nodeid = (pstTranFldCfg+i)->m_Nodeid;
         strcpy( trancode, (pstTranFldCfg+i)->m_Trancode );
         msgdscrb = (pstTranFldCfg+i)->m_Msgdscrb;
         fldlevel = (pstTranFldCfg + i)->m_Fldlevel;

         fldid = (pstTranFldCfg+i)->m_Tranfldid;
         if (fldid < 0) {
             repflag[fldlevel] = (pstTranFldCfg+i)->m_Repflag;
         }

         if(fldlevel < last_fldlevel)
         {
            for (j = last_fldlevel - 1; j >= fldlevel; j--)
            {
                switch (repflag[j])
                {
                case 1:
                    sprintf( flds, "%s%d,", flds, -1 );
                    break;
                case 2:
                    sprintf( flds, "%s%d,", flds, -8001);
                    break;
                case 3:
                    sprintf( flds, "%s%d:%s,", flds, -9001,
                             repfldname[j]);
                    break;
                }
            }
         }

         if( fldid < 0)          /* 重复域标志 */
         {
            switch (repflag[fldlevel])
            {
            case 1:
                sprintf( flds, "%s%d,", flds, -1 );
                sprintf( reptimesseq, "%s%d,", reptimesseq,
                         (pstTranFldCfg+i)->m_Reptmfldid );
                sprintf( reptype, "%s%d,",
                         reptype, (pstTranFldCfg+i)->m_Reptype );
                reptimes++;
                sprintf( reptimesnum, "%s%d,",
                         reptimesnum, reptimes );
                break;
            case 2:
                sprintf( flds, "%s%d,", flds, -8000);
                break;
            case 3:
                strcpy (repfldname[fldlevel], (pstTranFldCfg+i)->m_Repfldname);
                sprintf( flds, "%s%d:%s,", flds, -9000, repfldname[fldlevel]);
                break;
            }
         } 
         else
         {
           sprintf( flds, "%s%d,", flds, fldid );
           tranfldnum++;
         }

         i++;
         last_fldlevel = fldlevel;
      }
      
      fldlevel = (pstTranFldCfg+i)->m_Fldlevel;
      fldid = (pstTranFldCfg+i)->m_Tranfldid;
      if (fldid < 0) {
          repflag[fldlevel]  = (pstTranFldCfg+i)->m_Repflag;
      }

      if (fldid >= 0)
      {
          if(fldlevel < last_fldlevel) 
          {
              for (j = last_fldlevel - 1; j >= fldlevel; j--)
              {
                switch (repflag[j])
                {
                case 1:
                    sprintf( flds, "%s%d,", flds, -1 );
                    break;
                case 2:
                    sprintf( flds, "%s%d,", flds, -8001);
                    break;
                case 3:
                    sprintf( flds, "%s%d:%s,", flds, -9001, repfldname[j]);
                    break;
                }
             } 
             sprintf( flds, "%s%d,", flds, (pstTranFldCfg+i)->m_Tranfldid );
             for (; j >= 0; j--)
             {
                switch (repflag[j])
                {
                case 1:
                    sprintf( flds, "%s%d,", flds, -1 );
                    break;
                case 2:
                    sprintf( flds, "%s%d,", flds, -8001);
                    break;
                case 3:
                    sprintf( flds, "%s%d:%s,", flds, -9001, repfldname[j]);
                    break;
                }
             } 
          }
          else                                     /* 最后一个字段非重复域 */
          {
             sprintf( flds, "%s%d,", flds, (pstTranFldCfg+i)->m_Tranfldid );
             for (j = fldlevel -1; j >= 0; j--)
             {
                 switch (repflag[j])
                 {
                 case 1:
                     sprintf( flds, "%s%d,", flds, -1 );
                     break;
                 case 2:
                     sprintf( flds, "%s%d,", flds, -8001);
                     break;
                 case 3:
                     sprintf( flds, "%s%d:%s,", flds, -9001, repfldname[j]);
                     break;
                 }
             }
          }
      }

      if( strlen( reptimesseq ) == 0 )
      {
         strcpy( reptimesseq, "-1" );
         strcpy( reptimesnum, "-1" );
         strcpy( reptype, "-1" );
      }
      fprintf( fp, "%d|%s|%d|%s|%s|%s|%s|\n",
               nodeid, trancode, msgdscrb, 
               reptimesseq, reptimesnum, reptype, flds );
      fflush( fp );
      last_fldlevel = fldlevel =0;
   }
}

void createCheckCfg( FILE *fp )
{
   char line[2048];
   int i;

   for( i = 0; i < iRowDecision; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstDecision+i)->m_Svcid );
      sprintf( line, "%s%d|", line, (pstDecision+i)->m_Cursubseq );
      sprintf( line, "%s%d|", line, (pstDecision+i)->m_Condid );
      sprintf( line, "%s%d|", line, (pstDecision+i)->m_Nextsubseq );
      sprintf( line, "%s%d|", line, (pstDecision+i)->m_Dfltflag );
      sprintf( line, "%s%s|", line, (pstDecision+i)->m_Expression );

      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createRelaCond( FILE *fp )
{
   char line[2048], *Expr;
   int  i, j, level;

   for (j = 0; j < iRowCondItem; j++)
   {
      for( i = 0; i < iRowDecision; i++ )
      {
         if( (pstCondItem+j)->m_Condid == (pstDecision+i)->m_Condid )
         {
             break;
         }
      }

      if (i < iRowDecision)
      {
            memset( line, 0, sizeof(line) );
            sprintf( line, "%d|", (pstCondItem+j)->m_Condid );
            /*** modify 
            sprintf( line, "%s%d|", line, level );
            ********/
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Exprid + 1);
            /*******/
            if( (pstCondItem+j)->m_Rmsgdscrb != 0 &&
                (pstCondItem+j)->m_Rfldid != 0 )
            {
               sprintf( line, "%s%d|", line, 0 );
            }
            else
            {
               sprintf( line, "%s%d|", line, 1 );
            }
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lsubseq );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lmsgdscrb );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lfldid );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lfldtype );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lstartbit );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lendbit );
            sprintf( line, "%s%s|", line, (pstCondItem+j)->m_Rvalue );
            sprintf( line, "%s%s|", line, (pstCondItem+j)->m_Rvalue );
            sprintf( line, "%s%d|", line, 0 );
            sprintf( line, "%s%d|", line, 1 );
            sprintf( line, "%s%d|", line, 1 );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Op );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Rsubseq );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Rmsgdscrb );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Rfldid );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Lfldtype );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Rstartbit );
            sprintf( line, "%s%d|", line, (pstCondItem+j)->m_Rendbit );

            fprintf( fp, "%s\n", line );
            fflush( fp );
         
      }
   }
}

int srchFld( const void *l, const void *r )
{
   struct msgfldcfg *pstL = (struct msgfldcfg *)l;
   struct msgfldcfg *pstR = (struct msgfldcfg *)r;

   if( pstL->m_Msgappid != pstR->m_Msgappid )
   {
      return pstL->m_Msgappid - pstR->m_Msgappid;
   }
   else
   {
      return pstL->m_Fldid - pstR->m_Fldid;
   }
}

int searchSrcFld( const void *l, const void *r )
{
   struct subfldsrccfg *pl = (struct subfldsrccfg *)l;
   struct subfldsrccfg *pr = (struct subfldsrccfg *)r;

   return pl->m_Asmid - pr->m_Asmid;
}

void createSubReqCfg( FILE *fp )
{
   char line[2048];
   struct msgfldcfg *pstFldCfg, stFldCfgKey;
   struct subfldsrccfg stFldSrcKey, *pstFldSrc;
   int i, s, n, f;
   int nodeid;
   int reptmsubseq = -1, reptmfldid = -1, repfldtype = -1, repfldmsgdscrb;

   for( i = 0; i < iRowSubReqCfg; i++ )
   {
      memset( line, 0, sizeof(line) );

      /** 首先判断服务流程中该子服务请求字段要重组 **/
      for( f = 0; f < iRowSvcFlowCfg; f++ )
      {
         if( (pstSubReqCfg+i)->m_Svcid == (pstSvcFlowCfg+f)->m_Svcid &&
             (pstSubReqCfg+i)->m_Subseq == (pstSvcFlowCfg+f)->m_Subseq &&
             (pstSvcFlowCfg+f)->m_Kind != 3 )
         {
            break;
         }
      }
      if( f == iRowSvcFlowCfg )
      {
         printf( "SUBREQCFG: svcid %d and subseq %d not in svcflowcfg\n",
                 (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq );
         continue;
      }

      if( (pstSvcFlowCfg+f)->m_Subseq < 0 &&
          (pstSvcFlowCfg+f)->m_Crckind == 4 )  /** 出错返回不需要重组 **/
      {
         continue;
      }

      /** 为防止记录中重复字段来源重组信息不在来源中引起错误   **/
      /** 跳过部分判断                                         **/
      if( (pstSubReqCfg+i)->m_Fldid < 0 &&
          (pstSubReqCfg+i)->m_Repflag == 1)
      {
         goto RepeatFldTimeHandle;
      }

      stFldSrcKey.m_Asmid = (pstSubReqCfg+i)->m_Asmid;
      pstFldSrc = bsearch( (const void *)&stFldSrcKey, pstSubFldSrcCfg,
                           iRowSubFldSrcCfg, sizeof(stFldSrcKey),
                           searchSrcFld );
      if( pstFldSrc == NULL && (pstSubReqCfg+i)->m_Fldid >= 0 )
      {
         printf( "SUBREQCFG: svcid %d subseq %d fldid %d asmid %d not "
                 "in subfldsrccfg\n",
                 (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                 (pstSubReqCfg+i)->m_Fldid, (pstSubReqCfg+i)->m_Asmid );
         continue;
      }

      if( (pstSubReqCfg+i)->m_Subseq > 0 )  /** 正常子服务 **/
      {
         for( s = 0; s < iRowSubService; s++ )
         {
            if( (pstSvcFlowCfg+f)->m_Subid == (pstSubService+s)->m_Subid )
            {
               break;
            }
         }
         if( s == iRowSubService )
         {
            printf( "SUBREQCFG: Invalid svcid %d subseq %d subid %d\n",
                    (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                    (pstSvcFlowCfg+f)->m_Subid );
            continue;
         }
         nodeid = (pstSubService+s)->m_Nodeid;
      }
      else  /** 服务返回 **/
      {
         for( s = 0; s < iRowService; s++ )
         {
            if( (pstService+s)->m_Svcid == (pstSubReqCfg+i)->m_Svcid )
            {
               break;
            }
         }
         if( s == iRowService )
         {
            printf( "SUBREQCFG: Invalid svcid %d subseq %d\n",
                    (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq );
            continue;
         }
         nodeid = (pstService+s)->m_Nodeid;
      }
      for( n = 0; n < iRowNodeCfg; n++ )
      {
         if( (pstNodeCfg+n)->m_Nodeid == nodeid )
         {
            break;
         }
      }
      if( n == iRowNodeCfg )
      {
         printf( "SUBSEQCFG: svcid %d subseq %d node id %d not in nodecfg\n",
                 (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                 nodeid );
         continue;
      }
      stFldCfgKey.m_Msgappid = (pstNodeCfg+n)->m_Msgappid;
      stFldCfgKey.m_Fldid    = (pstSubReqCfg+i)->m_Fldid;
      if( (pstSubReqCfg+i)->m_Fldid >= 0 )
      {
         pstFldCfg = bsearch( (void*)&stFldCfgKey, (void*)pstMsgFldCfg,
                              iRowMsgFldCfg, sizeof(stFldCfgKey), srchFld );
         if( pstFldCfg == NULL )
         {
            printf( "SUBREQCFG: svcid %d subseq %d nodeid %d dictid %d"
                    " fldid %d not in msgfldcfg\n",
                    (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                    nodeid,
                    (pstNodeCfg+n)->m_Msgappid, (pstSubReqCfg+i)->m_Fldid );
            continue;
         }
         sprintf( line, "%d|", (pstSubReqCfg+i)->m_Svcid );
         sprintf( line, "%s%d|", line, (pstSubReqCfg+i)->m_Subseq );
         sprintf( line, "%s%d|", line, (pstSubReqCfg+i)->m_Fldid );
         if( (pstSubReqCfg+i)->m_Repflag == 0 ) /* 非重复域 */
         {
            sprintf( line, "%s%d|", line, 0 );
         }
         else                       /* 重复域   */
         {
            if( reptmsubseq < 0 ) /* 重复次数是常数 */
            {
               sprintf( line, "%s%d|", line, 0 );
            }
            else                    /* 重复次数非常数 */
            {
               sprintf( line, "%s%d|", line, repfldmsgdscrb );
            }
         }
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, pstFldCfg->m_Fldtype );
         if( (pstSubReqCfg+i)->m_Repflag != 1 ) /* 非重复域 */
         {
            sprintf( line, "%s%d|", line, 0 );
            sprintf( line, "%s%d|", line, 0 );
            sprintf( line, "%s%d|", line, 0 );
            sprintf( line, "%s%d|", line, 0 );
         }
         else
         {
            sprintf( line, "%s%d|", line, 1 );
            sprintf( line, "%s%d|", line, reptmsubseq );
            sprintf( line, "%s%d|", line, reptmfldid );
            sprintf( line, "%s%d|", line, repfldtype );
         }
         sprintf( line, "%s%d|", line, (pstSubReqCfg+i)->m_Asmfunid );
         sprintf( line, "%s%d|", line, (pstSubReqCfg+i)->m_Asmid );
         sprintf( line, "%s%d|", line, (pstSubReqCfg+i)->m_Balflag );
         fprintf( fp, "%s\n", line );
         fflush( fp );
      }
      else if ((pstSubReqCfg+i)->m_Repflag == 1)
      {
RepeatFldTimeHandle:
         reptmsubseq = (pstSubReqCfg+i)->m_Reptmsubseq;
         repfldmsgdscrb = (pstSubReqCfg+i)->m_Reptmmsgdscrb;
         reptmfldid =  (pstSubReqCfg+i)->m_Reptmfldid;
         repfldtype = 0;
         if( reptmsubseq != 0 ) /* 重复次数来源不是服务请求 */
         {
            for( f = 0; f < iRowSvcFlowCfg; f++ )
            {
               if( (pstSubReqCfg+i)->m_Svcid == (pstSvcFlowCfg+f)->m_Svcid &&
                   reptmsubseq == (pstSvcFlowCfg+f)->m_Subseq &&
                   (pstSvcFlowCfg+f)->m_Kind != 3 )
               {
                  break;
               }
            }
            if( f == iRowSvcFlowCfg )
            {
               printf( "%d SUBREQCFG: svcid %d and subseq %d fldid %d "
                       "rep from subseq %d not in svcflowcfg\n", __LINE__,
                       (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                       (pstSubReqCfg+i)->m_Fldid, reptmsubseq );
               continue;
            }
            for( s = 0; s < iRowSubService; s++ )
            {
               if( (pstSvcFlowCfg+f)->m_Subid == (pstSubService+s)->m_Subid )
               {
                  break;
               }
            }
            if( s == iRowSubService )
            {
               printf( "SUBREQCFG: svcid %d subid %d not in subsvc\n",
                       (pstSvcFlowCfg+f)->m_Subid );
               continue;
            }
            nodeid = (pstSubService+s)->m_Nodeid;
         }
         else /** 重复次数来源于服务请求 */
         {
            for( s = 0; s < iRowService; s++ )
            {
               if( (pstService+s)->m_Svcid == (pstSubReqCfg+i)->m_Svcid )
               {
                  break;
               }
            }
            if( s == iRowService )
            {
               printf( "SUBREQCFG: svcid %d\n", (pstSubReqCfg+i)->m_Svcid );
               continue;
            }
            nodeid = (pstService+s)->m_Nodeid;
         }
         for( n = 0; n < iRowNodeCfg; n++ )
         {
            if( (pstNodeCfg+n)->m_Nodeid == nodeid )
            {
               break;
            }
         }
         if( n == iRowNodeCfg )
         {
            printf( "SUBREQCFG: svcid %d node id %d\n",
                    (pstSubReqCfg+i)->m_Svcid, nodeid );
            continue;
         }
         stFldCfgKey.m_Msgappid = (pstNodeCfg+n)->m_Msgappid;
         stFldCfgKey.m_Fldid    = reptmfldid;
         pstFldCfg = bsearch( (void*)&stFldCfgKey, (void*)pstMsgFldCfg,
                              iRowMsgFldCfg, sizeof(stFldCfgKey),
                              srchFld );
         if( pstFldCfg == NULL )
         {
            printf( "%d SUBREQCFG: svcid %d subseq %d "
                    "dict id %d and rep fldid %d not in msgfldcfg\n",
                    __LINE__,
                    (pstSubReqCfg+i)->m_Svcid, (pstSubReqCfg+i)->m_Subseq,
                    (pstNodeCfg+n)->m_Msgappid, reptmfldid );
            continue;
         }
         repfldtype = pstFldCfg->m_Fldtype;
      }
   }
}

int sortSubReqCfg( const void *l, const void *r )
{
   struct subreqcfg *pl = (struct subreqcfg *)l;
   struct subreqcfg *pr = (struct subreqcfg *)r;

   return pl->m_Asmid - pr->m_Asmid;
}

void sortByAsmId( void )
{
   qsort( (void*)pstSubReqCfg, iRowSubReqCfg, sizeof(struct subreqcfg),
          sortSubReqCfg );
}

int searchReqCfg( const void *l, const void *r )
{
   struct subreqcfg *pl = (struct subreqcfg *)l;
   struct subreqcfg *pr = (struct subreqcfg *)r;

   return pl->m_Asmid - pr->m_Asmid;
}

void createSubFldSrcCfg( FILE *fp )
{
   char line[2048];
   struct msgfldcfg *pstFldCfg, stFldCfgKey;
   struct subreqcfg stReqKey, *pstReqCfg;
   int i, s, n, f, t;
   int fldtype, nodeid, isRep;
   char trancode[60];

   /* 先将 subreqcfg 按asmid 排序 */
   sortByAsmId();

   for( i = 0; i < iRowSubFldSrcCfg; i++ )
   {
      memset( line, 0, sizeof(line) );
      stReqKey.m_Asmid = (pstSubFldSrcCfg+i)->m_Asmid;
      pstReqCfg = bsearch( (const void *)&stReqKey, pstSubReqCfg,
                           iRowSubReqCfg, sizeof(struct subreqcfg), 
                           searchReqCfg );
      if( pstReqCfg == NULL )
      {
         printf( "SUBFLDSRC: asmid %d not in subreqcfg\n",
                 (pstSubFldSrcCfg+i)->m_Asmid );
         continue;
      }
      switch( (pstSubFldSrcCfg+i)->m_Srcmsgdscrb )
      {
      case 0:
         fldtype = 1; break;
      case 1:
         fldtype = 3; break;
      case 2:
         fldtype = 0; break;
      default:
         fldtype = (pstSubFldSrcCfg+i)->m_Srcmsgdscrb;
      }
      isRep = 0;
      if( fldtype == 1 )  /** 目标域是常数 **/
      {
         sprintf( line, "%d|", (pstSubFldSrcCfg+i)->m_Asmid );
         sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcseq );
         sprintf( line, "%s%d|", line, fldtype );
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, 0 );
         sprintf( line, "%s%d|", line, isRep );
         sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcbeginbit );
         sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcendbit );
         sprintf( line, "%s%s|", line, (pstSubFldSrcCfg+i)->m_Srcnote );
         fprintf( fp, "%s\n", line );
         fflush( fp );
         continue;
      }
      if( pstReqCfg->m_Repflag == 1 ) /* 目标域是重复域 */
      {
         for( f = 0; f < iRowSvcFlowCfg; f++ )
         {
            if( pstReqCfg->m_Svcid == (pstSvcFlowCfg+f)->m_Svcid &&
                (pstSubFldSrcCfg+i)->m_Srcsubseq == (pstSvcFlowCfg+f)->m_Subseq
                && (pstSvcFlowCfg+f)->m_Kind != 3 )
            {
               break;
            }
         }
         if( f == iRowSvcFlowCfg )
         {
            printf( "SUBFLDSRC: asmid %d svcid %d srcsubseq %d not in "
                    "svcflowcfg\n",
                    (pstSubFldSrcCfg+i)->m_Asmid,
                    pstReqCfg->m_Svcid, pstReqCfg->m_Subseq );
            continue;
         }
         if( (pstSvcFlowCfg+f)->m_Kind == 4 ) /** 来源是服务字段 **/
         {
            /** 取service中的nodeid和trancode **/
            for( s = 0; s < iRowService; s++ )
            {
               if( (pstService+s)->m_Svcid == pstReqCfg->m_Svcid )
               {
                  break;
               }
            }
            if( s == iRowService )
            {
               printf( "SUBFLDSRC: asmid %d svcid %d not in service\n",
                       (pstSubFldSrcCfg+i)->m_Asmid, pstReqCfg->m_Svcid );
               continue;
            }
            nodeid = (pstService+s)->m_Nodeid;
            strcpy( trancode, (pstService+s)->m_Trancode );
         }
         else if( (pstSvcFlowCfg+f)->m_Kind == 1 )
         { /** 来源是子服务字段 m_kind 应该是1 **/
            /** 取subservice中的nodeid和trancode **/
            for( s = 0; s < iRowSubService; s++ )
            {
               if( (pstSubService+s)->m_Subid == (pstSvcFlowCfg+f)->m_Subid )
               {
                  break;
               }
            }
            if( s == iRowSubService )
            {
               printf( "SUBFLDSRC: asmid %d svcid %d subid %d not in "
                       "subservice\n",
                       (pstSubFldSrcCfg+i)->m_Asmid, pstReqCfg->m_Svcid,
                       (pstSvcFlowCfg+f)->m_Subid );
               continue;
            }
            nodeid = (pstSubService+s)->m_Nodeid;
            strcpy( trancode, (pstSubService+s)->m_Trancode );
         }
         else
         {
            printf( "SUBFLDSRC: asmid %d svcid %d srcsubseq %d kind %d "
                    "invalid\n",
                    (pstSubFldSrcCfg+i)->m_Asmid,
                    pstReqCfg->m_Svcid, pstReqCfg->m_Subseq,
                    (pstSvcFlowCfg+f)->m_Kind );
            continue;
         }
         /** 有节点号和交易代码后从tranfldcfg取字段类型 **/
         for( t = 0; t < iRowTranFldCfg; t++ )
         {
            if( nodeid == (pstTranFldCfg+t)->m_Nodeid &&
                strcmp( trancode, (pstTranFldCfg+t)->m_Trancode ) == 0 &&
                (pstSubFldSrcCfg+i)->m_Srcmsgdscrb == (pstTranFldCfg+t)->m_Msgdscrb &&
                (pstSubFldSrcCfg+i)->m_Srcfldid == (pstTranFldCfg+t)->m_Tranfldid )
            {
               break;
            }
         }
         if( t == iRowTranFldCfg )
         {
            printf( "SUBFLDSRC: svcid %d subseq %d from subseq %d nodeid %d "
                    "trancode %s msgdesc %d fldid %d not in tranfldcfg\n",
                    pstReqCfg->m_Svcid, pstReqCfg->m_Subseq,
                    (pstSubFldSrcCfg+i)->m_Srcsubseq,
                    nodeid, trancode, (pstSubFldSrcCfg+i)->m_Srcmsgdscrb,
                    (pstSubFldSrcCfg+i)->m_Srcfldid );
            continue;
         }
         isRep = (pstTranFldCfg+t)->m_Repflag;
      }
      sprintf( line, "%d|", (pstSubFldSrcCfg+i)->m_Asmid );
      sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcseq );
      sprintf( line, "%s%d|", line, fldtype );
      sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcsubseq );
      sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcfldid );
      sprintf( line, "%s%d|", line, isRep );
      sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcbeginbit );
      sprintf( line, "%s%d|", line, (pstSubFldSrcCfg+i)->m_Srcendbit );
      sprintf( line, "%s%s|", line, (pstSubFldSrcCfg+i)->m_Srcnote );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createLogWatch( FILE *fp )
{
   char line[2048];
   int  i, j, k, iPreNodeId = -1, num;

   for( i = 0; i < iRowLogWatch; i++ )
   {
      memset( line, 0, sizeof(line) );
      if( (pstLogWatch+i)->m_Nodeid != iPreNodeId )
      {
         iPreNodeId = (pstLogWatch+i)->m_Nodeid;
         sprintf( line, "%d|", (pstLogWatch+i)->m_Tag );
         sprintf( line, "%s%d|", line, (pstLogWatch+i)->m_Nodeid );
         sprintf( line, "%s%d|", line, -1 );
         num = 0;
         for( k = i; k < iRowLogWatch; k++ )
         {
            if( iPreNodeId != (pstLogWatch+k)->m_Nodeid )
            {
               break;
            }
            num++;
         }
         sprintf( line, "%s%d|", line, num );
         sprintf( line, "%s%d|", line, 0 );
         fprintf( fp, "%s\n", line );
         fflush( fp );
      }
      sprintf( line, "%d|", (pstLogWatch+i)->m_Tag );
      sprintf( line, "%s%d|", line, (pstLogWatch+i)->m_Nodeid );
      sprintf( line, "%s%d|", line, (pstLogWatch+i)->m_Dictid );
      sprintf( line, "%s%d|", line, (pstLogWatch+i)->m_Fldid );
      for( j = 0; j < iRowMsgFldCfg; j++ )
      {
         if( (pstMsgFldCfg+j)->m_Msgappid == (pstLogWatch+i)->m_Dictid &&
             (pstMsgFldCfg+j)->m_Fldid == (pstLogWatch+i)->m_Fldid )
         {
            sprintf( line, "%s%d|", line, (pstMsgFldCfg+j)->m_Fldtype );
            break;
         }
      }
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createItemKind( FILE *fp )
{
   char line[2048];
   int  i, j, k, iPreNodeId = -1, num;

   for( i = 0; i < iRowItemKind; i++ )
   {
      memset( line, 0, sizeof(line) );
      sprintf( line, "%d|", (pstItemKind+i)->m_NodeId );
      sprintf( line, "%s%d|", line, (pstItemKind+i)->m_SubNodeId );
      sprintf( line, "%s%s|", line, (pstItemKind+i)->m_Name );
      fprintf( fp, "%s\n", line );
      fflush( fp );
   }
}

void createFile( char *path )
{
   char filename[128];
   FILE *fp;

   sprintf( filename, "%s/%s", path, "msgfldcfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createMsgFldCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "msgnameid.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createMsgNameId( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "nodetrancfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createNodeTranCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "nodecommcfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createNodeCommCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "service.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createService( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "servicecfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createServiceCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "subservice.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createSubService( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "tranfldcfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createTranFldCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "retcodemap.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   /* createTranFldCfg( fp ); */
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "chkcfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createCheckCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "constcond.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "relacond.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createRelaCond( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "subsvcreqcfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createSubReqCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "subsvcfldsrccfg.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createSubFldSrcCfg( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "logwatch.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createLogWatch( fp );
   fclose( fp );
   /* printf( "完成\n" ); */

   sprintf( filename, "%s/%s", path, "itemkind.txt" );
   printf( "创建文件 %s \n", filename );
   fp = fopen( filename, "w" );
   if( fp == NULL )
   {
      printf( "open file %s error: %s\n", filename, strerror(errno) );
   }
   createItemKind( fp );
   fclose( fp );
   /* printf( "完成\n" ); */
}

void Usage( char *pro )
{
   printf( "%s [-h host][-d database][-u user][-p password][-D datadir]i[-H]\n",
           pro );
   printf( "-h host     default is \"localhost\"\n" );
   printf( "-d database default is \"nlcmp\"\n" );
   printf( "-u user     default is \"root\"\n" );
   printf( "-p password default is \"root\"\n" );
   printf( "-D datadir  default is \"%s/data\"\n", getenv("MYROOT") );
   printf( "-H print this help message\n" );
}

int main( int argc, char *argv[] )
{
   char host[40], db[64], user[64], pass[64], datadir[128];
   char opt, *home;
   struct stat stStat;

   home = getenv( "MYROOT" );
   /*设置缺省值*/
   strcpy( host, "localhost" );
   strcpy( db,   "nlcmp" );
   strcpy( user, "root" );
   strcpy( pass, "" );
   sprintf( datadir, "%s/data", home );
   while( (opt=getopt( argc, argv, "h:d:u:p:D:H" )) != -1 )
   {
      switch( opt )
      {
      case 'h':
         strcpy( host, optarg );
         break;
      case 'd':
         strcpy( db, optarg );
         break;
      case 'u':
         strcpy( user, optarg );
         break;
      case 'p':
         strcpy( pass, optarg );
         break;
      case 'D':
         strcpy( datadir, optarg );
         break;
      case 'H':
         Usage( argv[0] );
         exit( 0 );
      default:
         printf( "Invalid argument [%c][%d]\n", opt, opt );
         Usage( argv[0] );
         exit( 0 );
      }
   }

   printf( "正在连接数据库..." );
   if( db_connect( host, db, user, pass ) == -1 )
   {
      printf( "失败\n" );
      return -1;
   }

   printf( "成功\n" );

   readDataFromDb( );

   db_close( );

   if( stat( datadir, &stStat ) == -1 )
   {
      printf( "data dir %s error: %s\n", strerror(errno) );
      exit(0);
   }

   if( !S_ISDIR( stStat.st_mode ) )
   {
      printf( "%s 不是一个目录\n", datadir );
   }

   createFile( datadir );

   return 0;
}
