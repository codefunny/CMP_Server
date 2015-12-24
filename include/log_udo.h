/*
** 文件：log_udo.h
*/

#include <assert.h>
#include <string.h>

#include "global.h"
/* #include "ifx.h" */
#include "udo.h"
#include "pub_base.h"
#include "shm_base.h"
#include "svc_base.h"

#define LOG_SUCC                       0
#define LOG_NODE_NOT_DEF               2001
#define LOG_TABNAME_NOT_DEF            2002
#define LOG_TABFLD_NOT_DEF             2003
#define LOG_FLD_NOT_DEF                2004
#define LOG_ERR_TYPE                   2005
#define LOG_INSERT_ERR                 2006
#define LOG_TABFLD_DEF_ERR             2007

/*
** 功能：根据渠道日志表组成创建空值FSS
** 输入：1 渠道节点号
**       2 服务信息
** 输出：日志信息
** 返回：0-成功，其他-失败
*/
int LOG_CreatLogUDO( int eiChaNodeId, SVC_Content *epstService, UDP opfLog );

/*
** 功能：修改日志信息FSS
** 输入：1 渠道节点号
**       2 服务信息
** 输出：日志信息
** 返回：0-成功，其他-失败
*/
int LOG_ChgLogUDO( SUB_SvcList *epstService, UDP eopfLog, int eiFlag );


/*
** 功能：根据日志信息生成Sql语句，记日志
** 输入：1 服务信息
**       2 日志信息
** 输出：无
** 返回：0-成功，其他-失败
*/
int LOG_Write( SVC_Content * epstService, UDP epfLog, int eiSuccFlag );

/*
** end of file
*/

