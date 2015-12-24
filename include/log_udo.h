/*
** �ļ���log_udo.h
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
** ���ܣ�����������־����ɴ�����ֵFSS
** ���룺1 �����ڵ��
**       2 ������Ϣ
** �������־��Ϣ
** ���أ�0-�ɹ�������-ʧ��
*/
int LOG_CreatLogUDO( int eiChaNodeId, SVC_Content *epstService, UDP opfLog );

/*
** ���ܣ��޸���־��ϢFSS
** ���룺1 �����ڵ��
**       2 ������Ϣ
** �������־��Ϣ
** ���أ�0-�ɹ�������-ʧ��
*/
int LOG_ChgLogUDO( SUB_SvcList *epstService, UDP eopfLog, int eiFlag );


/*
** ���ܣ�������־��Ϣ����Sql��䣬����־
** ���룺1 ������Ϣ
**       2 ��־��Ϣ
** �������
** ���أ�0-�ɹ�������-ʧ��
*/
int LOG_Write( SVC_Content * epstService, UDP epfLog, int eiSuccFlag );

/*
** end of file
*/

