/*
**    chk.h
**
**    本文件定义了条件路由模块的数据结构和宏定义
**
*/

#ifndef __CHK__
#define __CHK__

#include "global.h"
#include "shm_base.h"
#include "udo.h"
#include "shm_base.h"

#define CHK_TRUE  1
#define CHK_FALSE 0

#define CHK_NULL  "NULL"

#define CHK_STR_MAX_LEN       100
#define CHK_COND_RECORDS_MAX  50

#define CHK_ERR_NO_COND_IS_TRUE        -651    /* 没有一个条件成立 */
#define CHK_ERR_DATA_IS_ERR            -652    /* 表中数据配置错   */
#define CHK_ERR_READ_DB_ERR            -653    /* 读表错           */
#define CHK_ERR_COND_IS_TOO_MUCH       -654    /* 条件个数超限     */
#define CHK_ERR_NO_RECORD              -655    /* 常数条件表和关系条件表没有相应记录 */
#define CHK_ERR_DFLT_COND_IS_TOO_MUCH  -656    /* 有多个默认条件   */

#define MAXLAYER 10

#define EQ                             0     /* 等于             */
#define NE                             1     /* 不等于           */
#define LT                             2     /* 小于             */
#define GT                             3     /* 大于             */
#define LE                             4     /* 小于等于         */
#define GE                             5     /* 大于等于         */

typedef struct
{
   int iLayerNum;
   RelaCondAry stRelaCondAry[MAXLAYER];
} RelaCondStruct;

RelaCondStruct stRelaCondStruct;
#endif
