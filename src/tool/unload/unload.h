#ifndef __MYSQL_UNLOAD_CONFIG_H__
#define __MYSQL_UNLOAD_CONFIG_H__

struct boolexpr
{
  int m_Condid;
  int m_Exprid;
  int m_Lsubseq;
  int m_Lmsgdscrb;
  int m_Lfldid;
  int m_Lfldtype;
  char m_Lvalue[41];
  int m_Lstartbit;
  int m_Lendbit;
  int m_Op;
  int m_Rsubseq;
  int m_Rmsgdscrb;
  int m_Rfldid;
  int m_Rfldtype;
  char m_Rvalue[41];
  int m_Rstartbit;
  int m_Rendbit;
};

struct chkcfg
{
  int m_Svcid;
  int m_Cursubseq;
  int m_Condid;
  int m_Nextsubseq;
  int m_Dfltflag;
};

struct chkparam
{
  int m_Svcid;
  int m_Cursubseq;
  int m_Paramid;
  int m_Subseq;
  int m_Funcid;
  int m_Msgdscrb;
  int m_Fldid;
  int m_Fldtype;
  char m_Fvalue[201];
  int m_Startbit;
  int m_Endbit;
  int m_Kind;
};

struct conditem
{
  int m_Condid;
  int m_Exprid;
  int m_Lsubseq;
  int m_Lmsgdscrb;
  int m_Lfldid;
  int m_Lfldtype;
  char m_Lvalue[41];
  int m_Lstartbit;
  int m_Lendbit;
  int m_Op;
  int m_Rsubseq;
  int m_Rmsgdscrb;
  int m_Rfldid;
  int m_Rfldtype;
  char m_Rvalue[41];
  int m_Rstartbit;
  int m_Rendbit;
};

struct decision
{
  int m_Svcid;
  int m_Cursubseq;
  int m_Seq;
  int m_Condid;
  char m_Expression[256];
  int m_Nextsubseq;
  int m_Funid;
  int m_Dfltflag;
  int m_Kind;
};

struct mapsource
{
  int m_Mapid;
  int m_Srcseq;
  int m_Srcmsgdscrb;
  int m_Srcfldid;
  int m_Srcbeginbit;
  int m_Srcendbit;
  char m_Srcconstant[41];
};

struct maptarget
{
  int m_Tag;
  int m_Svcid;
  int m_Msgdscrb;
  int m_Fldid;
  int m_Reptmmsgdscrb;
  int m_Reptmfldid;
  int m_Mapfunid;
  int m_Mapid;
};

struct msgfldcfg
{
  int m_Msgappid;
  int m_Fldid;
  char m_Fldengname[31];
  char m_Fldchnname[31];
  int m_Fldmsgkind;
  int m_Lenfldlen;
  int m_Fldlen;
  int m_Lenfldtype;
  int m_Fldtype;
  char m_Lenflddef[3];
  char m_Flddef[3];
  int m_Lenfldalign;
  int m_Fldalign;
  char m_Sepsymbol[21];
  int m_Tomidid;
  int m_Tomsgid;
  int m_Fldtag;
  int m_Fldexprid;
  char m_Fldmacro[51];
};

struct msgnameid
{
  int m_Msgappid;
  int m_Parentid;
  char m_Msgheaderqz[41];
  char m_Msgappname[41];
};

struct nodecfg
{
  int m_Nodeid;
  char m_Nodename[41];
  int m_Nodedscrb;
  int m_Nodetype;
  int m_Msgappid;
  char m_Ipaddr[16];
  int m_Commport;
  int m_Svrkeyid;
  int m_Svwkeyid;
  int m_Tomidid;
  int m_Tomsgid;
  int m_Logflag;
  int m_Watchflag;
  int m_Maxretry;
  int m_Delay;
};

struct optionreg
{
  char m_Item[21];
  char m_Itemvalue[256];
  char m_Category[21];
};

struct problems
{
  int m_Svcid;
  char m_Svcname[41];
  int m_Nodeid;
  char m_Trancode[16];
  int m_Timeout;
};
struct service
{
  int  m_Svcid;
  char m_Svcname[41];
  int  m_Nodeid;
  char m_Trancode[16];
  int  m_Timeout;
  int  m_Subnodeid;
};

struct servicecfg
{
  int m_Chnnodeid;
  char m_Svctrancode[21];
  int m_Svcid;
  int m_Hostnodeid;
  char m_Subtrancode[21];
  int m_Subid;
  int m_Subseq;
  int m_Retrytime;
  int m_Outtime;
  int m_Parentsubseq;
  int m_Sublayer;
  int m_Batflag;
  int m_Batmode;
  int m_Lstid;
  int m_Crckind;
  int m_Crcsubid;
};

struct subfldsrccfg
{
  int m_Asmid;
  int m_Srcseq;
  int m_Srcmsgdscrb;
  int m_Srcsubseq;
  int m_Srcfldid;
  int m_Srcbeginbit;
  int m_Srcendbit;
  char m_Srcnote[41];
};

struct subflowcfg
{
  int m_Subsvcid;
  int m_Seq;
  int m_Subid;
  int m_Subseq;
  int m_Kind;
  int m_Subretimes;
  int m_Lstid;
  int m_Outtime;
  int m_Crckind;
  int m_Crcsubid;
};

struct subreqcfg
{
  int m_Svcid;
  int m_Subseq;
  int m_Fldid;
  int m_Fldseq;
  int m_Reptmsubseq;
  int m_Reptmmsgdscrb;
  int m_Reptmfldid;
  int m_Asmfunid;
  int m_Asmid;
  int m_Balflag;
  int m_Kind;
  int m_Repflag;
  int m_Fldlevel;
};

struct subservice
{
  int  m_Subid;
  char m_Subname[41];
  int  m_Templetid;
  int  m_Nodeid;
  char m_Trancode[16];
  int  m_Timeout;
  char m_Remark[41];
  int  m_Subnodeid;
};

struct svcflowcfg
{
  int m_Svcid;
  int m_Seq;
  int m_Subid;
  int m_Subseq;
  int m_Kind;
  int m_Subretimes;
  int m_Lstid;
  int m_Crckind;
  int m_Crcsubid;
  int m_Outtime;
};

struct svcflowcfg1
{
  int m_Svcid;
  int m_Subid;
  int m_Subseq;
  int m_Subretimes;
  int m_Parentseq;
  int m_Sublayer;
  int m_Lstid;
  int m_Crckind;
  int m_Crcsubid;
  int m_OLeft;
  int m_OTop;
  int m_OWidth;
  int m_OHeight;
};

struct sys_seq
{
  char m_CityCode[6];
  char m_SeqType[21];
  char m_SeqName[41];
  int m_SeqNo;
  char m_State[2];
};

struct trancode
{
  char m_CityCode[6];
  char m_TTxKind[3];
  char m_TTxCode[7];
  char m_TTxName[31];
  char m_VTxCode[7];
  char m_VReqCode[7];
  char m_VTxName[31];
  char m_CashFlag[2];
  char m_KTxCode[7];
  char m_KTxName[31];
  char m_OTxCode[7];
  char m_OTxName[31];
  char m_BusiCode[5];
  char m_AgUnitNo[6];
  char m_State[2];
};

struct tranfldcfg
{
  int m_Nodeid;
  char m_Trancode[16];
  int m_Msgdscrb;
  int m_Tranfldseq;
  int m_Tranfldid;
  int m_Repflag;
  int m_Reptmfldid;
  int m_Reptype;
  int m_Parentfldseq;
  char m_Repfldname[41];
  int m_Fldlevel;
};

struct tranmsgkindcfg
{
  int m_Nodeid;
  char m_Trancode[16];
  int m_Msgdscrb;
  char m_Msgname[31];
};

struct trans_log
{
  int m_X16;
  int m_X19;
  int m_X111;
  char m_X113[5];
  double m_X114;
  int m_X110;
};

struct logwatch
{
   int m_Tag;     /* 标志 */
   int m_Nodeid;  /* 节点编号 */
   int m_Dictid;
   int m_Fldid;   /* 字段编号 */
   int m_Fldtype; /* 字段类型 */
};

struct itemkind
{
   int  m_NodeId;
   int  m_SubNodeId;
   char m_Name[41];
};

#endif
