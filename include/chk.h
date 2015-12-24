/*
**    chk.h
**
**    ���ļ�����������·��ģ������ݽṹ�ͺ궨��
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

#define CHK_ERR_NO_COND_IS_TRUE        -651    /* û��һ���������� */
#define CHK_ERR_DATA_IS_ERR            -652    /* �����������ô�   */
#define CHK_ERR_READ_DB_ERR            -653    /* �����           */
#define CHK_ERR_COND_IS_TOO_MUCH       -654    /* ������������     */
#define CHK_ERR_NO_RECORD              -655    /* ����������͹�ϵ������û����Ӧ��¼ */
#define CHK_ERR_DFLT_COND_IS_TOO_MUCH  -656    /* �ж��Ĭ������   */

#define MAXLAYER 10

#define EQ                             0     /* ����             */
#define NE                             1     /* ������           */
#define LT                             2     /* С��             */
#define GT                             3     /* ����             */
#define LE                             4     /* С�ڵ���         */
#define GE                             5     /* ���ڵ���         */

typedef struct
{
   int iLayerNum;
   RelaCondAry stRelaCondAry[MAXLAYER];
} RelaCondStruct;

RelaCondStruct stRelaCondStruct;
#endif
