#ifndef _ADP_TO_MITENO_H_
#define _ADP_TO_MITENO_H_
#include "udo.h"
#include "svc_base.h"
#include "pkg/pkg_api.h"
#include "adp_base.h"

#define  TRANSTYPE              1        /* 交易类型                     */
#define  TXDATE                 2        /* 交易日期                     */
#define  TXTIME                 3        /* 交易时间                     */
#define  TRACENO                4        /* 流水号                       */
#define  BATCHNO                5        /* 批次号                       */
#define  VOICENO                6        /* 票据号                       */
#define  TERMINALID             7        /* 终端号                       */
#define  MERCHANTNO             8        /* 商户号                       */
#define  REFNO                  9        /* 系统参考号                   */
#define  ACCNO                  10       /* 主账号                       */
#define  TRACK2                 11       /* 二磁道                       */
#define  TRACK3                 12       /* 三磁道                       */
#define  PHONENO                13       /* 手机号                       */
#define  COUPONNO               14       /* 券码                         */
#define  SERMODE                15       /* 输入方式                     */
#define  PIN                    16       /* PIN                          */
#define  SMSCODE                17       /* 短信验证码                   */
#define  IC55                   18       /* IC55                         */
#define  IC56                   19       /* IC56                         */
#define  OLDTRACENO             20       /* 原流水号                     */
#define  OLDBATCHNO             21       /* 原批次号                     */
#define  OLDREFNO               22       /* 原系统参考号                 */
#define  OLDTXDATE              23       /* 原交易日期                   */
#define  OLDTRANSTYPE           24       /* 原交易类型                   */
#define  RESPCODE               25       /* 响应码                       */
#define  RESPINFO               26       /* 响应信息                     */
#define  TAG28                  28       /* TAG28                        */
#define  AMOUNT                 29       /* 金额                         */
#define  THIRDPANLIST           30       /* 第三方账户列表               */
#define  THIRDPAN               31       /* 第三方账户                   */
#define  COUPONLIST             32       /* 卡券列表                     */
#define  CREDITAMOUNT           33       /* 抵用金额                     */
#define  PARMVERSION            90       /* 参数版本号                   */
#define  MAINKEY                91       /* 主密钥                       */
#define  PINKEY                 92       /* PINKEY                       */
#define  MACKEY                 93       /* MACKEY                       */
#define  MERCHNAME              94       /* 商户名称                     */
#define  ADDFIELD1              95       /* 备注信息1                    */
#define  MAC                    99       /* MAC                          */

#ifdef __cplusplus
extern "C" {
#endif


int  ADP_ToMitenoPos( int eiSubID,    int eiSubSeq,
                       UDP epfReqList, UDP opfRspList,
                       SVC_Content   * epstService );

int CheckMitenoMac(UDP epfReqList,const char *pszInputMac,  const char *epczTermId, const char *epczMerchantId );

#ifdef __cplusplus
}
#endif

#endif
