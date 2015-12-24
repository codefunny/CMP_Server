/*******************************************************************\
 * �ļ�����  $Source: /api/src/include/KeyPub.h,v $
 * �ļ�����: ��Կ����������ͷ�ļ�
 * �汾��$Revision: 1.1 $
 *  ------------------------------
 *  �޸ļ�¼��
 *  $Log: KeyPub.h,v $
 *  Revision 1.1  2003/06/27 13:02:31  ty
 *  New Copy
 *
 *  Revision 1.28  2003/06/27 09:45:58  taoyuan
 *  sec_errno��Ϊsys_en
 *
 *  Revision 1.27  2003/06/26 08:38:43  taoyuan
 *  errno��Ϊsec_errno,�Ա�����߳���errno��Ϊ����
 *
 *  Revision 1.26  2003/06/20 12:04:25  taoyuan
 *  �����Զ�ʶ��CryptFlg�����ܵĺ�
 *
 *  Revision 1.25  2003/05/28 07:46:42  taoyuan
 *  *** empty log message ***
 *
 *
 *  Revision 1.23  2003/05/27 12:23:35  taoyuan
 *  ��ʱ��֧��DT_FUNC_LDAT��������
 *
 *  Revision 1.21  2003/05/27 09:00:38  taoyuan
 *  fix bug
 *
 *  Revision 1.20  2003/05/27 03:10:55  taoyuan
 *  ������ܻ�ʹ�õĺ��������ͻص���������
 *
 *  Revision 1.19  2003/05/26 05:46:34  taoyuan
 *  ����DES/3DES/AES�㷨�걸ѡ��
 *
 *  Revision 1.18  2003/05/22 06:18:19  taoyuan
 *  ����GenEn/Decrypt()����ֵ�ĺ�
 *
 *  Revision 1.17  2003/05/08 04:30:40  taoyuan
 *  ͳһ����׼C��ע��
 *
 *  Revision 1.16  2003/05/05 12:52:46  taoyuan
 *  NO_ANYKEY
 *
 *  Revision 1.15  2003/04/30 13:33:52  taoyuan
 *  ȥ���������ĺ�ͺ�������
 *
 *  Revision 1.14  2003/04/30 09:43:10  taoyuan
 *  IS_PPKEY��IS_MACKEY��ֵ�����Ժ�PP_KEY��MAC_KEYһ��
 *
 *  Revision 1.13  2003/04/30 09:18:07  taoyuan
 *  ȥ����arith.h������
 *
 *  Revision 1.12  2003/04/25 03:06:34  gouyong
 *  ������������ݣ�
 *
 *      �˺궨����Ϊ��ά�ְ�ȫ�ӿ� API ��������Сд�ϵ�ͳһ�淶��
 *  #define       GetSecurityError(clear_all_errors, sec_err_fetcher)     \
 *                getSecurityError(clear_all_errors, sec_err_fetcher)
 *
 *  Revision 1.11  2003/04/24 02:00:24  menggang
 *  ����int AttachSecurityNode_r(char* node_id);
 *      int DetachSecurityNode_r(char* node_id);
 *
 *  Revision 1.10  2003/04/18 13:16:07  gouyong
 *  ��
 *  SECERRORITEM* getSecurityError(SECERRORFETCHER  sec_err_fetcher);
 *  �ӿڸĳ�
 *  SECERRORITEM* getSecurityError(int              clear_all_errors,
 *                             SECERRORFETCHER      sec_err_fetcher)
 *
 *  Revision 1.9  2003/04/18 10:27:39  taoyuan
 *  ApplyKey()ԭ���޸�
 *
 *  Revision 1.8  2003/04/18 06:40:07  menggang
 *  *** empty log message ***
 *
 *  Revision 1.7  2003/04/17 12:55:04  taoyuan
 *  fix bug
 *
 *  Revision 1.6  2003/04/17 06:43:35  menggang
 *  ������־��ʹ��getSecurityError
 *
 *  Revision 1.5  2003/04/17 05:11:26  gouyong
 *  Ϊ getSecurityError ����ע��
 *
 *  Revision 1.3  2003/04/17 01:50:13  liubg
 *  �޸�C++ע��ΪC���
 *
 *  Revision 1.2  2003/04/15 08:32:53  menggang
 *  ������־������Ϣ�ĸ�ʽ
 *
 *  Revision 1.26  2003/04/10 10:47:17  taoyuan
 *  ȥ������
 *
 *  Revision 1.25  2003/03/27 00:46:06  taoyuan
 *  GenEnc/Decԭ����(un)signed char *��Ϊvoid *
 *
 *  Revision 1.22  2003/03/24 23:13:37  taoyuan
 *  ���亯��ԭ������
 *
 *  Revision 1.21  2003/03/21 21:45:28  taoyuan
 *  ��������������
 *
 *  Revision 1.20  2003/03/21 17:39:17  taoyuan
 *  ȥ����־�ļ���ȫ�ֱ�������־����
 *
 *  Revision 1.18  2003/03/21 04:04:06  taoyuan
 *  ������־�ļ���ȫ�ֱ���
 *
 *  Revision 1.17  2003/03/20 15:43:14  taoyuan
 *  ����DES/UNDES��C����
 *
 *  Revision 1.16  2003/03/20 03:55:31  taoyuan
 *  ��ʽ�ؼ��Ϻ�IS_COPY
 *
 *  Revision 1.15  2003/03/19 17:04:56  taoyuan
 *  ������־�������ĺ�LOGOUTPUT
 *
 *  Revision 1.14  2003/03/19 02:37:23  taoyuan
 *  ������־���õĺ�
 *
 *  Revision 1.12  2003/03/16 00:15:41  taoyuan
 *  ����ApplyKey����
 *
 *  Revision 1.11  2003/03/15 20:56:49  taoyuan
 *  �����������SHM�е�KEY�ĺ깩�򻯲�����
 *
 *  Revision 1.9  2003/03/15 18:06:02  taoyuan
 *  ���¼��Ϻ�NO_ANYKEY
 *
 *  Revision 1.8  2003/03/14 02:07:47  taoyuan
 *  ����GetFrontID������
 *
 *  Revision 1.7  2003/03/14 01:11:38  taoyuan
 *  ʹ��ͷ���ɱ��cpp��include
 *
 *  Revision 1.6  2003/03/14 01:06:40  taoyuan
 *  ����GenEnc/Dec��C��������
 *
 *  Revision 1.5  2003/03/13 17:38:30  taoyuan
 *  get_string����Ϊread_hexstr
 *
 *  Revision 1.4  2003/03/13 04:30:56  taoyuan
 *  ����get_string�Ա�������
 *
 *  Revision 1.3  2003/03/13 03:46:40  taoyuan
 *  �޸Ĺ���KEY�Ĺ��ú�
 *
 *  Revision 1.2  2003/03/12 20:41:37  zhengwj
 *  �޸��ַ������뺯��
 *
 *  Revision 1.1  2003/03/10 20:03:39  taoyuan
 *  ����������
 *
 *  Revision 1.3  2003/03/04 02:41:48  zhengwj
 *  *** empty log message ***
 *
 *  Revision 1.2  2003/03/03 21:33:50  zhengwj
 *  ����ת������
 *
 *  Revision 1.1  2003/02/27 21:19:59  zhengwj
 *  �½�
 *
 *
\*********************************************************************/

#ifndef _H_KEYPUB
#define _H_KEYPUB

/* Key ���� �ͼӽ��ܲ����ĺ� */
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

#define _USING_AES	0  /**������ϵͳ �ӽ���ʹ�õ�ȱʡ�㷨 **/
#define _USING_DES	1<<8
#define _USING_3DES     1<<9

#ifdef __cplusplus
extern "C" {
#endif
/* ���ӻ�Ͽ�һ�����ؽڵ�(�����ڴ���Դ)���� */
int AttachSecurityNode(char* node_id);
int AttachSecurityNode_r(char* node_id);
int DetachSecurityNode(void);
int DetachSecurityNode_r(char* node_id);

/*ͨ�üӽ��ܺ�У��API*/
int GenEncrypt( char *TargetID, void *SourStr, int *length,
        			void *DestStr, int CryptFlg );

int GenDecrypt( char *SourceID, void *SourStr, int *length,
        			void *DestStr, int CryptFlg );

/*DES�ӽ���*/
int DES( unsigned char *buf,unsigned int buflen,unsigned char key[] );

int UNDES( unsigned char *buf,unsigned int  buflen,unsigned char key[] );

/**���ܻ�ת�ӽ���**/
typedef	int	(*DATA_TRANSFORM_FUNC)(char* data, char* transformed_data, int* data_len);

int HSMEncrypt( char* 	dest_node_id,
		char* 	encrypted_ppkey,	/*8�ֽڱ���׼des���ܹ��� ppkey��
					������ԿΪ�Ե�ͨѶ�ڵ�� basekey */
		DATA_TRANSFORM_FUNC	data_transform_func,
		char* 	in_data,
		char* 	out_data,
		int*  	len,
		int	flag ) ;

int HSMDecrypt( char* 	src_node_id,
		char* 	encrypted_ppkey,	/*8�ֽڱ���׼des���ܹ��� ppkey��
					������ԿΪ�Ե�ͨѶ�ڵ�� basekey */
		DATA_TRANSFORM_FUNC	data_transform_func,
		char* 	in_data,
		char* 	out_data,
		int*  	len,
		int	flag ) ;

/**	Ϊ���ܻ�ת�ӽ��ܿ�����Ҫ����ʽת�� Ԥ����ĺ꺯�� **/

/**	���µ͵�ַ���ᱻ�������Ͳ���ϵͳ����ʵ�ʵĺ�����ַ**/

/**	����Ҫ������ת�� **/
#define	DT_FUNC_NULL		((DATA_TRANSFORM_FUNC)0)
/*	��ASCII��ѹ���� BCD�� */
#define	DT_FUNC_CBCD		((DATA_TRANSFORM_FUNC)1)

/*	2B���ȣ����ݸ�ʽ */
#define	DT_FUNC_LDAT		((DATA_TRANSFORM_FUNC)2)	

/**	��ȡ ���ܹ��� ppkey �ĺ���,�����ܻ�Ӧ�� **/
/**	����ֵ��	0���ɹ��� < 0��ʧ��  **/
int getSecurityNodePPkey(char* peer_node_id, char* ppkey) ;


/**�ӽ��ܸ�������**/
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



/**	level Ϊ�����������еĲ�Σ����ϲ�Ϊ 1��������£���������
	sec_err_item Ϊ getSecurityError �����ص������İ�ȫģ��ĳ�����Ϣ
**/
typedef	void (*SECERRORFETCHER)(int level, SECERRORITEM* sec_err_item);

/**clear_all_errors Ϊ��0ֵ����ʾ�� getSecurityError ���غ�Ҫ���
	����ջ�ϵ����д���Ϊ0����ʾ��getSecurityError ���غ���Ȼ����
	����ջ�ϵ����д���
**/	
/**	sec_err_fetcher Ϊ�ص�������ַ�������ֵ��Ϊ NULL
	���������Ѵ���ջ�м�¼�Ĵ��󰴴����ϲ㵽���²�Ĵ�����һ����
	�� sec_err_fetcher����ʹ����Ի�ȡ��ȫģ�����꾡�Ĵ�����Ϣ
**/
SECERRORITEM* getSecurityError(int clear_all_errors, SECERRORFETCHER sec_err_fetcher);

/**	�˺궨����Ϊ��ά�ְ�ȫ�ӿ� API ��������Сд�ϵ�ͳһ�淶��
**/
#define	GetSecurityError(clear_all_errors, sec_err_fetcher)	\
		getSecurityError(clear_all_errors, sec_err_fetcher)





#ifdef __cplusplus
}
#endif

#ifndef PACK_MAXLEN 
#define PACK_MAXLEN  25600
#endif

/** GenEncrypt( ) �� GenDecrypt( ), HSMEncrypt( ), HSMDecrypt( ) ���غ�ֵ�� ����ʹ�ú����Ҫֱ��ʹ�� ��ֵ����**/
#define _DC_SEC_BASE     200

#define _DC_SEC_ENOMEM     1   /** ��⵽�ڴ��ϵͳ�ں�������� **/
#define _DC_SEC_EOTHER     2   /** �ӽ�������ʧ�� **/
#define _DC_SEC_EINVAL     3   /** �ӽ������볤��Ϊ��������ȷ���зǷ�ָ�룻
				����GenDecrypt�Ĳ�������ȷ���Ķ�ʧ��������ָ���İ�ȫ���� **/
#define _DC_SEC_EMSGCHANGE 4   /** ���ؽڵ�ͶԵȽڵ�[%10.10s]ͨѶʱ�ܵ�����,���ı��ı�! **/
#define _DC_SEC_ECOMM 	   4   /** ���ؽڵ�ͶԵȽڵ�[%10.10s]ͨѶʱ�����д�! **/
#define _DC_SEC_EVERRIFY   5   /** ��Կ��ͬ��, ���Ľ���λ��ƫ�ƴ������ܲ������Ե� **/
#define _DC_SEC_EACCES     6   /** ������Կ��д����ʧ�ܣ�����Կ�ڴ治�ܷ��ʵ���Ȩ�޲�������
		û���ҵ�ָ���ڵ����Կ����ĳЩ��������¿�������������֮һ��**/
				
#define _DC_SEC_ENOENT	   7   /** ������(��Կ)�ڴ���ļ� �� �����ڸýڵ���Կ **/
#define _DC_SEC_EACCESKEY  8   /** Ȩ�޲��������ɷ��ʸ�(��Կ) **/
#define _DC_SEC_ENXIO      9   /** û���ҵ�ָ���ڵ���Կ **/

#define _DC_SEC_EMSGSIZE   10  /** ���ĳ��ȳ����˰�ȫ�汾��һ�δ������󳤶� PACK_MAXLEN-32 **/


#define _DC_SEC_ENOTRUST   _DC_SEC_EVERRIFY     /* not a trusted program */
#define _DC_SEC_ETIMEDOUT       78      /* Connection timed out */
#define _DC_SEC_ECONNREFUSED    79      /* Connection refused */
#define _DC_SEC_EHOSTDOWN       80      /* Host is down */
#define _DC_SEC_EHOSTUNREACH    81      /* No route to host */
#define _DC_SEC_ENOTCONN        235     /* Socket is not connected */

#define _DC_SEC_ESAD            113     /* security authentication denied */


#endif
