/*
** file:   pkg_tuxbase.h
*/

#include "global.h"
#include "udo.h"
#include "shm_base.h"


#ifndef __PKG_TUX_BASE_H__
#define __PKG_TUX_BASE_H__

#define MAX_FLD_LEN     2048

#define TUX_SUCC        0
#define TUX_FAIL        -1
#define TUX_ERR_BASE    1000
#define TUX_FLD_DEF_ERR TUX_ERR_BASE + 1
#define TUX_FETCH_ERROR TUX_ERR_BASE + 2
#define TUX_MATCH_ERROR TUX_ERR_BASE + 3
#define TUX_FLD_NOT_DEF TUX_ERR_BASE + 4
#define TUX_NODE_NO_PKG TUX_ERR_BASE + 5
#define FML_NOT_FIND    TUX_ERR_BASE + 6
#define FML_DEST_SMALL  TUX_ERR_BASE + 7
#define LST_NODE_NOT_FIND  TUX_ERR_BASE + 8
#define LST_OCC_NOT_FIND  TUX_ERR_BASE + 9
#define PKG_GET_APPID_ERR TUX_ERR_BASE + 10
#define TUX_NOTFOUND TUX_ERR_BASE + 11 

/*
**the struct's fields map the fields of table msgfldcfg:
**iPkgAppId    -- msgappid
**iFldSeq      -- fldseq
**aczMacroName -- fldengname
**iMacroValue  -- fldlen
**iFldType     -- fldtype
**field type details information
**0----string
**1----int
**2----long
**3----double
**4----float
**5----carray
*/
#define FMLSTRING 0
#define FMLINT    1
#define FMLLONG   2
#define FMLDOUBLE 3
#define FMLFLOAT  4
#define FMLCARRAY 5

/*I assume there are 1000 fields in one tuxedo domain*/
#define TUX_MAX_FLD 1000

#endif
/*
** end of file
*/
