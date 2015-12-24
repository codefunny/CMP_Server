#ifndef _ADP_TO_MITENO_H_
#define _ADP_TO_MITENO_H_
#include "udo.h"
#include "svc_base.h"
#include "pkg/pkg_api.h"
#include "adp_base.h"

#define  TRANSTYPE              1        /* ��������                     */
#define  TXDATE                 2        /* ��������                     */
#define  TXTIME                 3        /* ����ʱ��                     */
#define  TRACENO                4        /* ��ˮ��                       */
#define  BATCHNO                5        /* ���κ�                       */
#define  VOICENO                6        /* Ʊ�ݺ�                       */
#define  TERMINALID             7        /* �ն˺�                       */
#define  MERCHANTNO             8        /* �̻���                       */
#define  REFNO                  9        /* ϵͳ�ο���                   */
#define  ACCNO                  10       /* ���˺�                       */
#define  TRACK2                 11       /* ���ŵ�                       */
#define  TRACK3                 12       /* ���ŵ�                       */
#define  PHONENO                13       /* �ֻ���                       */
#define  COUPONNO               14       /* ȯ��                         */
#define  SERMODE                15       /* ���뷽ʽ                     */
#define  PIN                    16       /* PIN                          */
#define  SMSCODE                17       /* ������֤��                   */
#define  IC55                   18       /* IC55                         */
#define  IC56                   19       /* IC56                         */
#define  OLDTRACENO             20       /* ԭ��ˮ��                     */
#define  OLDBATCHNO             21       /* ԭ���κ�                     */
#define  OLDREFNO               22       /* ԭϵͳ�ο���                 */
#define  OLDTXDATE              23       /* ԭ��������                   */
#define  OLDTRANSTYPE           24       /* ԭ��������                   */
#define  RESPCODE               25       /* ��Ӧ��                       */
#define  RESPINFO               26       /* ��Ӧ��Ϣ                     */
#define  TAG28                  28       /* TAG28                        */
#define  AMOUNT                 29       /* ���                         */
#define  THIRDPANLIST           30       /* �������˻��б�               */
#define  THIRDPAN               31       /* �������˻�                   */
#define  COUPONLIST             32       /* ��ȯ�б�                     */
#define  CREDITAMOUNT           33       /* ���ý��                     */
#define  PARMVERSION            90       /* �����汾��                   */
#define  MAINKEY                91       /* ����Կ                       */
#define  PINKEY                 92       /* PINKEY                       */
#define  MACKEY                 93       /* MACKEY                       */
#define  MERCHNAME              94       /* �̻�����                     */
#define  ADDFIELD1              95       /* ��ע��Ϣ1                    */
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
