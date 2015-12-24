/*******************************************************************\
 * 文件名：  $Source: /api/src/include/KeyPub.h,v $
 * 文件功能: 密钥处理公共函数头文件
 * 版本：$Revision: 1.1 $
 *  ------------------------------
 *  修改纪录：
 *  $Log: KeyPub.h,v $
 *  Revision 1.1  2003/06/27 13:02:31  ty
 *  New Copy
 *
 *  Revision 1.28  2003/06/27 09:45:58  taoyuan
 *  sec_errno改为sys_en
 *
 *  Revision 1.27  2003/06/26 08:38:43  taoyuan
 *  errno改为sec_errno,以避免多线程下errno成为函数
 *
 *  Revision 1.26  2003/06/20 12:04:25  taoyuan
 *  加入自动识别CryptFlg来解密的宏
 *
 *  Revision 1.25  2003/05/28 07:46:42  taoyuan
 *  *** empty log message ***
 *
 *
 *  Revision 1.23  2003/05/27 12:23:35  taoyuan
 *  暂时不支持DT_FUNC_LDAT函数名宏
 *
 *  Revision 1.21  2003/05/27 09:00:38  taoyuan
 *  fix bug
 *
 *  Revision 1.20  2003/05/27 03:10:55  taoyuan
 *  加入加密机使用的函数声明和回调函数声明
 *
 *  Revision 1.19  2003/05/26 05:46:34  taoyuan
 *  加入DES/3DES/AES算法宏备选用
 *
 *  Revision 1.18  2003/05/22 06:18:19  taoyuan
 *  加入GenEn/Decrypt()返回值的宏
 *
 *  Revision 1.17  2003/05/08 04:30:40  taoyuan
 *  统一按标准C作注释
 *
 *  Revision 1.16  2003/05/05 12:52:46  taoyuan
 *  NO_ANYKEY
 *
 *  Revision 1.15  2003/04/30 13:33:52  taoyuan
 *  去掉不发布的宏和函数声明
 *
 *  Revision 1.14  2003/04/30 09:43:10  taoyuan
 *  IS_PPKEY和IS_MACKEY宏值互换以和PP_KEY及MAC_KEY一致
 *
 *  Revision 1.13  2003/04/30 09:18:07  taoyuan
 *  去掉对arith.h的依赖
 *
 *  Revision 1.12  2003/04/25 03:06:34  gouyong
 *  增加下面的内容：
 *
 *      此宏定义是为了维持安全接口 API 在命名大小写上的统一规范性
 *  #define       GetSecurityError(clear_all_errors, sec_err_fetcher)     \
 *                getSecurityError(clear_all_errors, sec_err_fetcher)
 *
 *  Revision 1.11  2003/04/24 02:00:24  menggang
 *  加入int AttachSecurityNode_r(char* node_id);
 *      int DetachSecurityNode_r(char* node_id);
 *
 *  Revision 1.10  2003/04/18 13:16:07  gouyong
 *  把
 *  SECERRORITEM* getSecurityError(SECERRORFETCHER  sec_err_fetcher);
 *  接口改成
 *  SECERRORITEM* getSecurityError(int              clear_all_errors,
 *                             SECERRORFETCHER      sec_err_fetcher)
 *
 *  Revision 1.9  2003/04/18 10:27:39  taoyuan
 *  ApplyKey()原型修改
 *
 *  Revision 1.8  2003/04/18 06:40:07  menggang
 *  *** empty log message ***
 *
 *  Revision 1.7  2003/04/17 12:55:04  taoyuan
 *  fix bug
 *
 *  Revision 1.6  2003/04/17 06:43:35  menggang
 *  不用日志，使用getSecurityError
 *
 *  Revision 1.5  2003/04/17 05:11:26  gouyong
 *  为 getSecurityError 增加注释
 *
 *  Revision 1.3  2003/04/17 01:50:13  liubg
 *  修改C++注释为C风格
 *
 *  Revision 1.2  2003/04/15 08:32:53  menggang
 *  调整日志调试信息的格式
 *
 *  Revision 1.26  2003/04/10 10:47:17  taoyuan
 *  去掉冗余
 *
 *  Revision 1.25  2003/03/27 00:46:06  taoyuan
 *  GenEnc/Dec原型由(un)signed char *改为void *
 *
 *  Revision 1.22  2003/03/24 23:13:37  taoyuan
 *  补充函数原型声明
 *
 *  Revision 1.21  2003/03/21 21:45:28  taoyuan
 *  初步整理函数声明
 *
 *  Revision 1.20  2003/03/21 17:39:17  taoyuan
 *  去掉日志文件名全局变量和日志级别
 *
 *  Revision 1.18  2003/03/21 04:04:06  taoyuan
 *  增加日志文件名全局变量
 *
 *  Revision 1.17  2003/03/20 15:43:14  taoyuan
 *  加入DES/UNDES的C声明
 *
 *  Revision 1.16  2003/03/20 03:55:31  taoyuan
 *  显式地加上宏IS_COPY
 *
 *  Revision 1.15  2003/03/19 17:04:56  taoyuan
 *  增加日志函数名的宏LOGOUTPUT
 *
 *  Revision 1.14  2003/03/19 02:37:23  taoyuan
 *  调整日志所用的宏
 *
 *  Revision 1.12  2003/03/16 00:15:41  taoyuan
 *  加入ApplyKey声明
 *
 *  Revision 1.11  2003/03/15 20:56:49  taoyuan
 *  加入可跳过读SHM中的KEY的宏供简化测试用
 *
 *  Revision 1.9  2003/03/15 18:06:02  taoyuan
 *  重新加上宏NO_ANYKEY
 *
 *  Revision 1.8  2003/03/14 02:07:47  taoyuan
 *  加入GetFrontID的声明
 *
 *  Revision 1.7  2003/03/14 01:11:38  taoyuan
 *  使此头可由别的cpp作include
 *
 *  Revision 1.6  2003/03/14 01:06:40  taoyuan
 *  加入GenEnc/Dec的C声明备用
 *
 *  Revision 1.5  2003/03/13 17:38:30  taoyuan
 *  get_string更名为read_hexstr
 *
 *  Revision 1.4  2003/03/13 04:30:56  taoyuan
 *  更名get_string以避免重名
 *
 *  Revision 1.3  2003/03/13 03:46:40  taoyuan
 *  修改关于KEY的公用宏
 *
 *  Revision 1.2  2003/03/12 20:41:37  zhengwj
 *  修改字符串输入函数
 *
 *  Revision 1.1  2003/03/10 20:03:39  taoyuan
 *  更名和整理
 *
 *  Revision 1.3  2003/03/04 02:41:48  zhengwj
 *  *** empty log message ***
 *
 *  Revision 1.2  2003/03/03 21:33:50  zhengwj
 *  增加转换函数
 *
 *  Revision 1.1  2003/02/27 21:19:59  zhengwj
 *  新建
 *
 *
\*********************************************************************/

#ifndef _H_KEYPUB
#define _H_KEYPUB

/* Key 类型 和加解密参数的宏 */
#ifndef NO_ANYKEY
#define NO_ANYKEY       0
#endif

#define IS_PPKEY        1
#ifndef IS_DEFTKEY
#define IS_DEFTKEY      2
#define IS_PKGKEY       4
#define IS_COPY		10
#endif

#ifndef DEFTMAC
#define DEFTMAC         16
#endif

#ifndef DEFT_DEFTMAC
#define DEFT_DEFTMAC    21
#endif

#ifndef COPY_DEFTMAC
#define COPY_DEFTMAC    23
#endif

#define AUTO_DECODEMAC  -1

#define _USING_AES	0  /**非主机系统 加解密使用的缺省算法 **/
#define _USING_DES	1<<8
#define _USING_3DES     1<<9

#ifdef __cplusplus
extern "C" {
#endif
/* 附加或断开一个本地节点(共享内存资源)连接 */
int AttachSecurityNode(char* node_id);
int AttachSecurityNode_r(char* node_id);
int DetachSecurityNode(void);
int DetachSecurityNode_r(char* node_id);

/*通用加解密和校验API*/
int GenEncrypt( char *TargetID, void *SourStr, int *length,
        			void *DestStr, int CryptFlg );

int GenDecrypt( char *SourceID, void *SourStr, int *length,
        			void *DestStr, int CryptFlg );

/*DES加解密*/
int DES( unsigned char *buf,unsigned int buflen,unsigned char key[] );

int UNDES( unsigned char *buf,unsigned int  buflen,unsigned char key[] );

/**加密机转加解密**/
typedef	int	(*DATA_TRANSFORM_FUNC)(char* data, char* transformed_data, int* data_len);

int HSMEncrypt( char* 	dest_node_id,
		char* 	encrypted_ppkey,	/*8字节被标准des加密过的 ppkey，
					加密密钥为对等通讯节点的 basekey */
		DATA_TRANSFORM_FUNC	data_transform_func,
		char* 	in_data,
		char* 	out_data,
		int*  	len,
		int	flag ) ;

int HSMDecrypt( char* 	src_node_id,
		char* 	encrypted_ppkey,	/*8字节被标准des加密过的 ppkey，
					加密密钥为对等通讯节点的 basekey */
		DATA_TRANSFORM_FUNC	data_transform_func,
		char* 	in_data,
		char* 	out_data,
		int*  	len,
		int	flag ) ;

/**	为加密机转加解密可能需要做格式转换 预定义的宏函数 **/

/**	以下低地址不会被编译器和操作系统用作实际的函数地址**/

/**	不需要做数据转换 **/
#define	DT_FUNC_NULL		((DATA_TRANSFORM_FUNC)0)
/*	把ASCII串压缩成 BCD码 */
#define	DT_FUNC_CBCD		((DATA_TRANSFORM_FUNC)1)

/*	2B长度＋数据格式 */
#define	DT_FUNC_LDAT		((DATA_TRANSFORM_FUNC)2)	

/**	获取 加密过的 ppkey 的函数,给加密机应用 **/
/**	返回值：	0：成功， < 0：失败  **/
int getSecurityNodePPkey(char* peer_node_id, char* ppkey) ;


/**加解密辅助函数**/
int GetMaxDestAddLen( int CryptFlg ) ;

int ApplyKey( char *SelfID, char *SuperID ) ;

int getSecurityNodePPkey(char* peer_node_id, char* ppkey) ;

typedef	struct {
	int		sec_ec;		/*	security module error code */
	int		sys_en;		/*	errno */
	char	*file;		/*	source file name */
	int		line;		/*	source file line number */
	char	*sec_em;	/*	error message */
} SECERRORITEM;



/**	level 为函数调用序列的层次，最上层为 1，层次向下，数量递增
	sec_err_item 为 getSecurityError 传给回调函数的安全模块的出错信息
**/
typedef	void (*SECERRORFETCHER)(int level, SECERRORITEM* sec_err_item);

/**clear_all_errors 为非0值，表示在 getSecurityError 返回后，要清除
	错误栈上的所有错误，为0，表示在getSecurityError 返回后，仍然保留
	错误栈上的所有错误。
**/	
/**	sec_err_fetcher 为回调函数地址，如果其值不为 NULL
	本函数将把错误栈中记录的错误按从最上层到最下层的次序逐一传递
	给 sec_err_fetcher，以使其可以获取安全模块中详尽的错误信息
**/
SECERRORITEM* getSecurityError(int clear_all_errors, SECERRORFETCHER sec_err_fetcher);

/**	此宏定义是为了维持安全接口 API 在命名大小写上的统一规范性
**/
#define	GetSecurityError(clear_all_errors, sec_err_fetcher)	\
		getSecurityError(clear_all_errors, sec_err_fetcher)





#ifdef __cplusplus
}
#endif

#ifndef PACK_MAXLEN 
#define PACK_MAXLEN  25600
#endif

/** GenEncrypt( ) 和 GenDecrypt( ), HSMEncrypt( ), HSMDecrypt( ) 返回宏值， 建议使用宏而不要直接使用 数值常数**/
#define _DC_SEC_BASE     200

#define _DC_SEC_ENOMEM     1   /** 检测到内存等系统内核溢出问题 **/
#define _DC_SEC_EOTHER     2   /** 加解密其他失败 **/
#define _DC_SEC_EINVAL     3   /** 加解密输入长度为负数不正确或有非法指针；
				传入GenDecrypt的参数不正确或报文丢失，不能做指定的安全操作 **/
#define _DC_SEC_EMSGCHANGE 4   /** 本地节点和对等节点[%10.10s]通讯时受到干扰,报文被改变! **/
#define _DC_SEC_ECOMM 	   4   /** 本地节点和对等节点[%10.10s]通讯时发送有错! **/
#define _DC_SEC_EVERRIFY   5   /** 密钥不同步, 或报文解密位置偏移错误或解密参数不对等 **/
#define _DC_SEC_EACCES     6   /** 调用密钥读写例程失败，如密钥内存不能访问到（权限不够）、
		没有找到指定节点的密钥，在某些具体情况下可以是如下三种之一：**/
				
#define _DC_SEC_ENOENT	   7   /** 不存在(密钥)内存或文件 或 不存在该节点密钥 **/
#define _DC_SEC_EACCESKEY  8   /** 权限不够，不可访问该(密钥) **/
#define _DC_SEC_ENXIO      9   /** 没有找到指定节点密钥 **/

#define _DC_SEC_EMSGSIZE   10  /** 报文长度超过此安全版本能一次处理的最大长度 PACK_MAXLEN-32 **/


#define _DC_SEC_ENOTRUST   _DC_SEC_EVERRIFY     /* not a trusted program */
#define _DC_SEC_ETIMEDOUT       78      /* Connection timed out */
#define _DC_SEC_ECONNREFUSED    79      /* Connection refused */
#define _DC_SEC_EHOSTDOWN       80      /* Host is down */
#define _DC_SEC_EHOSTUNREACH    81      /* No route to host */
#define _DC_SEC_ENOTCONN        235     /* Socket is not connected */

#define _DC_SEC_ESAD            113     /* security authentication denied */


#endif
