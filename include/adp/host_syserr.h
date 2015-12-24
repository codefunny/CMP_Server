#ifndef __HOST_SYSTEM_ERROR_H__
#define __HOST_SYSTEM_ERROR_H__
/**
  * ���ݿ����
  */
static const int DB_SELECT_NOT_FOUND = 201;
static const int DB_SELECT_ERROR     = 202;
static const int DB_INSERT_ERROR     = 203;
static const int DB_UPDATE_ERROR     = 204;
static const int DB_DELETE_ERROR     = 205;
static const int DB_OPEN_CUR_ERROR   = 206;

/**
  * ϵͳ����
  */
static const int READ_OPEN_FILE_ERROR  = 101;
static const int WRITE_OPEN_FILE_ERROR = 102;
static const int CREAT_FILE_ERROR      = 103;
static const int NO_REQUIRED_FIELD     = 104;

static const int HOST_GETMSGINFO_ERR   = 150;
static const int HOST_PACKAGE_ERR      = 151;
static const int HOST_UNPACKAGE_ERR    = 152;


/*
** Ӧ���๫�������붨�壬һ��Ӧ����ǩԼ����ҳʽ��ѯ��
*/
/* �������׷��ʹ���ʧ�� */
static const int HOST_SEND_FAIL = 301;

/* �������׽��մ���ʧ�� */
static const int HOST_RECV_FAIL = 302;

/* �������״���ʧ�� */
static const int HOST_DEAL_FAIL = 303;

#endif
