/*
**  file: pkg_struct.h
*/

#ifndef __PKG_STRUCT__
#define __PKG_STRUCT__

/* �йؽṹ�ĺ궨�� */
#define PKG_NAME_LEN 30
#define PKG_DEF_LEN 2
#define PKG_ALIGN_LEN 1
#define PKG_SEP_SYMBOL_LEN 200

#define PKG_TRAN_CODE_PARA_LEN 50
#define PKG_REP_PARA_LEN 50
#define PKG_TRAN_FLD_LEN 600

/* ���������Ϣ�Ľṹ */
typedef struct
{
   int    iMsgAppId;                            /* ����Ӧ�ú� */
   int    iFldSeq;                              /* ���� */
   int    iSepFldSeq;                           /* ������ */ 
   char   aczFldEngName[PKG_NAME_LEN+1];        /* ��Ӣ������ */ 
   char   aczFldChnName[PKG_NAME_LEN+1];        /* ���������� */
   int    iSepFldFlag;                          /* �������Ա�־ */
   int    iLenFldLen;                           /* ������ĳ��� */
   int    iFldLen;                              /* ������ĳ��� */
   int    iLenFldType;                          /* ����������� */ 
   int    iFldType;                             /* ����������� */
   char   aczLenFldDef[PKG_DEF_LEN+1];          /* �������Ĭ���ַ� */
   char   aczFldDef[PKG_DEF_LEN+1];             /* �������Ĭ���ַ� */
   char   aczLenFldAlign[PKG_ALIGN_LEN+1];      /* ������Ķ��뷽ʽ */
   char   aczFldAlign[PKG_ALIGN_LEN+1];         /* ������Ķ��뷽ʽ */
   char   aczSepSymbol[PKG_SEP_SYMBOL_LEN+1];   /* �ָ�� */
}FldCfgValNode;

/* NodeTranCfg��ṹ */
typedef struct _NodeTranCfg
{
   int  iNodeId;                     /* �ڵ�� */
   int  iMsgAppId;                   /* ����Ӧ�ú� */
   char aczMsgAppName[40+1];         /* ����Ӧ���� */
   int  iToMidTranCodeId;            /* ���ȡ���״��뺯��ID */
   int  iToMsgTranCodeId;            /* ���ȡ���״��뺯��ID */
   int  iToMidId;                    /* �������Id */
   int  iToMsgId;                    /* �������Id */
   int  iToMidIsSt;                  /* Դ�����Ƿ�Ϊ�ṹ�� */ 
   int  iToMsgIsSt;                  /* ��������Ƿ�Ϊ�ṹ�� */ 
   char aczCrtTranCodeType[PKG_TRAN_CODE_PARA_LEN+1];/* ��ȷ���״���������� */
   char aczCrtTranCodePos[PKG_TRAN_CODE_PARA_LEN+1];/* ��ȷ���״���ƫ������� */
   char aczCrtTranCodeSeq[PKG_TRAN_CODE_PARA_LEN+1];/* ��ȷ���״��������� */
   char aczCrtMsgFixFld[100+1];                       /* ��ȷ����ͷ�̶���� */
   char aczWrgTranCodeType[PKG_TRAN_CODE_PARA_LEN+1];/* �����״���������� */ 
   char aczWrgTranCodePos[PKG_TRAN_CODE_PARA_LEN+1];/* �����״���ƫ������� */
   char aczWrgTranCodeSeq[PKG_TRAN_CODE_PARA_LEN+1];/* �����״��������� */
   char aczWrgMsgFixFld[100+1];                       /* ������ͷ�̶���� */
}NodeTranCfg;

/* TranFldCfg�ṹ */ 
typedef struct _TranFldCfg
{
   int  iNodeId;                                  /* �ڵ�� */
   char aczTranCode[20+1];                        /* ���״��� */
   int  iMsgDscrb;                                /* ���������� */
   char aczRepTimesSeq[PKG_REP_PARA_LEN+1];       /* �ظ����������� */
   char aczRepTimesNum[PKG_REP_PARA_LEN+1];       /* �ظ����������� */
   char aczRepType[PKG_REP_PARA_LEN+1];           /* �ظ�����ظ�������� */
   char aczTranFld[PKG_TRAN_FLD_LEN+1];           /* ���������� */
}TranFldCfg;

#endif

/*
** end of file: pkg_struct.h
*/
