/*
** file:   pkg_tuxapi.h
*/
#include "fml32.h"
#include "pkg_tuxbase.h"

#ifndef __PKG_TUX_API_H__
#define __PKG_TUX_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** function:translate tuxedo's fml into plat middle infolist format
** input:   1 交易报文需要的各个FML字段定义
**          2 tuxedo's fml buffer
** output:  1 udo list
** return:  0-success,other failed
*/
int PKG_Fml2Udo( MsgFldCfgAry *epstFmlMsgFldCfgAry,
                 FBFR32 *epstFml, UDP opfUdo );

/*
** function:translate plat middle infolist into tuxedo's fml format
** input:   1 交易报文需要的各个FML字段定义
**          2 udo list
** output:  1 tuxedo's fml buffer
** return:  0-success,other failed
*/
int PKG_Udo2Fml( MsgFldCfgAry *epstMsgFldCfgAry,
                 UDP epfUdo, FBFR32 *opstFml );

#ifdef __cplusplus
}
#endif

/*
** end of file
*/
#endif
/*
** end of file
*/
