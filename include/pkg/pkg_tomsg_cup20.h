/*
**  file: pkg_tomsg.h
*/

#ifndef __PKG_TOMSG_CUP20__
#define __PKG_TOMSG_CUP20__

/*
**  该文件包含的头文件
*/

#include "pkg_base.h"
#include "pkg_base_cup20.h"

#ifdef __cplusplus
extern "C" {
#endif

int PKG_ToMsg_8583_1( char * opcOutMsg, int *opiOutMsgLen,
                      int eiNodeId, char * epczTranCode, int eiMsgDscrb,
                      MsgFldCfgAry * epstMsgFldCfgAry, UDP  epfsAddr );


/*从8583 报文中拆解出相应的域的职  
epaData 8583数据包   
eiFieldid 取数据id
opaczData 数据输出
epaczFiledLen 域的长度4个字符长度，如果第一个字符为B则为变长后面为变长字符长度  */
int special8583getfiled(char * epaData ,int eiFieldid,char * opaczData ,char * epaczFiledLen);


#ifdef __cplusplus
}
#endif

#endif

/*
** end of file: pkg_tomsg.h
*/
