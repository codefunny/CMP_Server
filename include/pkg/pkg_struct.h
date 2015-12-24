/*
**  file: pkg_struct.h
*/

#ifndef __PKG_STRUCT__
#define __PKG_STRUCT__

/* 有关结构的宏定义 */
#define PKG_NAME_LEN 30
#define PKG_DEF_LEN 2
#define PKG_ALIGN_LEN 1
#define PKG_SEP_SYMBOL_LEN 200

#define PKG_TRAN_CODE_PARA_LEN 50
#define PKG_REP_PARA_LEN 50
#define PKG_TRAN_FLD_LEN 600

/* 域段配置信息的结构 */
typedef struct
{
   int    iMsgAppId;                            /* 报文应用号 */
   int    iFldSeq;                              /* 域编号 */
   int    iSepFldSeq;                           /* 分域编号 */ 
   char   aczFldEngName[PKG_NAME_LEN+1];        /* 域英文名称 */ 
   char   aczFldChnName[PKG_NAME_LEN+1];        /* 域中文名称 */
   int    iSepFldFlag;                          /* 分域属性标志 */
   int    iLenFldLen;                           /* 长度域的长度 */
   int    iFldLen;                              /* 内容域的长度 */
   int    iLenFldType;                          /* 长度域的类型 */ 
   int    iFldType;                             /* 内容域的类型 */
   char   aczLenFldDef[PKG_DEF_LEN+1];          /* 长度域的默认字符 */
   char   aczFldDef[PKG_DEF_LEN+1];             /* 内容域的默认字符 */
   char   aczLenFldAlign[PKG_ALIGN_LEN+1];      /* 长度域的对齐方式 */
   char   aczFldAlign[PKG_ALIGN_LEN+1];         /* 内容域的对齐方式 */
   char   aczSepSymbol[PKG_SEP_SYMBOL_LEN+1];   /* 分割符 */
}FldCfgValNode;

/* NodeTranCfg表结构 */
typedef struct _NodeTranCfg
{
   int  iNodeId;                     /* 节点号 */
   int  iMsgAppId;                   /* 报文应用号 */
   char aczMsgAppName[40+1];         /* 报文应用名 */
   int  iToMidTranCodeId;            /* 拆包取交易代码函数ID */
   int  iToMsgTranCodeId;            /* 拆包取交易代码函数ID */
   int  iToMidId;                    /* 拆包函数Id */
   int  iToMsgId;                    /* 打包函数Id */
   int  iToMidIsSt;                  /* 源报文是否为结构体 */ 
   int  iToMsgIsSt;                  /* 输出报文是否为结构体 */ 
   char aczCrtTranCodeType[PKG_TRAN_CODE_PARA_LEN+1];/* 正确交易代码类型组成 */
   char aczCrtTranCodePos[PKG_TRAN_CODE_PARA_LEN+1];/* 正确交易代码偏移量组成 */
   char aczCrtTranCodeSeq[PKG_TRAN_CODE_PARA_LEN+1];/* 正确交易代码域号组成 */
   char aczCrtMsgFixFld[100+1];                       /* 正确报文头固定域段 */
   char aczWrgTranCodeType[PKG_TRAN_CODE_PARA_LEN+1];/* 错误交易代码类型组成 */ 
   char aczWrgTranCodePos[PKG_TRAN_CODE_PARA_LEN+1];/* 错误交易代码偏移量组成 */
   char aczWrgTranCodeSeq[PKG_TRAN_CODE_PARA_LEN+1];/* 错误交易代码域号组成 */
   char aczWrgMsgFixFld[100+1];                       /* 错误报文头固定域段 */
}NodeTranCfg;

/* TranFldCfg结构 */ 
typedef struct _TranFldCfg
{
   int  iNodeId;                                  /* 节点号 */
   char aczTranCode[20+1];                        /* 交易代码 */
   int  iMsgDscrb;                                /* 交易描述符 */
   char aczRepTimesSeq[PKG_REP_PARA_LEN+1];       /* 重复次数域号组成 */
   char aczRepTimesNum[PKG_REP_PARA_LEN+1];       /* 重复次数域号组成 */
   char aczRepType[PKG_REP_PARA_LEN+1];           /* 重复域的重复类型组成 */
   char aczTranFld[PKG_TRAN_FLD_LEN+1];           /* 交易域段组成 */
}TranFldCfg;

#endif

/*
** end of file: pkg_struct.h
*/
