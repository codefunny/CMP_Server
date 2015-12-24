/*
**file:adp_fundef.h
*/
#include "adp_control.h"
#include "adp/adp_calladp.h"
#include "adp/adp_reverse.h"
#include "adp_tochannel.h"
#include "adp_topos.h"
#include "adp_tounion.h"
#include "adp_toterm.h"
#include "adp_toallinpay.h"
#include "adp_tomiteno.h"

#ifndef __ADP_FUN_DEF_H__
#define __ADP_FUN_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

ADP_TempletEntity stTempletEntity[] = {
      { "SysError",            ADP_SysError    	    },   /*00*/
      { "ADP_StdAdp_Comm",     ADP_StdAdp_Comm      },   /*01*/
      { "ADP_ToChannel_GT",    ADP_ToChannel_GT     },   /*02*/
      { "ADP_ToChanErr_GT",    ADP_ToChanErr_GT     },   /*03*/
      { "ADP_ChanStdFixup",    ADP_ChanStdFixup     },   /*04*/
      { "ADP_WriteBalLog",     ADP_WriteBalLog      },   /*05*/
      { "ADP_ToUnion",         ADP_ToUnion          },   /*06*/
      { "ADP_ToTerm",          ADP_ToTerm           },   /*07*/
      { "ADP_ToOther",         ADP_ToOther          },   /*08*/
      { "ADP_ToPos",           ADP_ToPos            },   /*09*/    
      { "ADP_ToBankcomm",      ADP_ToBankcomm  		},   /*10*/
      { "ADP_ToYacol",         ADP_ToYacol  		},   /*11*/
      { "ADP_ToAllinpay",      ADP_ToAllinpay       },   /*12*/
	  { "ADP_ToMitenoPos",	   ADP_ToMitenoPos      },	 /*13*/

};

#ifdef __cplusplus
}
#endif

#endif

/*
**end of file
*/
