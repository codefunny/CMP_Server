#include "pkg/pkg_base.h"
#include "getAlignPos.h"
#include "getStrAlignNums.h"
#include "tempadd.h"
#include "log.h"

int getTranFlds (int tranFldNums, char * tranFldStr, int * tranFlds)
{
    int i;

    for (i = 1; i <= tranFldNums; i++)
    {
        tranFlds[i-1] = PKG_GetValFromFld (tranFldStr, i);
    }

    return (0);
}

int getStructsAlignNums (int tranFldNums, int * tranFlds, 
                         int msgAppid,    MsgFldCfgAry * msgFldCfgAry,
                         int alignStrNums[MAX_NESTED][MAX_STRUCT_PER_NESTED])
{
    int i, ret, fldSeq, nested;
    int strNums[10];
    int fldType;
    int alignNum;

    MsgFldCfgAry msgFldCfgAryTemp;

    nested = -1;
    memset (strNums, 0, sizeof (strNums));

    for (i = 0; i < tranFldNums; i++)
    {
        alignNum = 0;
        fldSeq = tranFlds[i];
 
        if (fldSeq >= NESTED_STRUCT_BEGINTAG_S 
            && fldSeq <= NESTED_STRUCT_BEGINTAG_E)
        {
            nested += 1;
            continue;
        }
        else if (fldSeq >= NESTED_STRUCT_ENDTAG_S 
                 && fldSeq <= NESTED_STRUCT_ENDTAG_E)
        {
            
            if (nested > 0 && alignStrNums[nested][strNums[nested]] > 
                alignStrNums[nested - 1][strNums[nested-1]])
            {
                alignStrNums[nested - 1][strNums[nested-1]] = 
                             alignStrNums[nested][strNums[nested]]; 
            }
            strNums[nested]++;
            nested -= 1;
            continue;
        }

        if (nested == -1) 
        {
            continue;
        }

        memset (&msgFldCfgAryTemp, 0, sizeof (msgFldCfgAryTemp)); 
        ret = PKG_GetInfoByFldSeq (msgFldCfgAry, msgAppid,
                                   fldSeq, &msgFldCfgAryTemp);  
        if (ret !=0)
        {
            return (-1);
        }

        fldType = msgFldCfgAryTemp.pstMsgFldCfg->iFldType;
        switch (fldType)
        {
         case UDT_STRING: 
           getAlignNum (AG_CCHARTYPE, &alignNum);
           break;
         case UDT_SHORT: 
           getAlignNum (AG_CSHORTTYPE, &alignNum);
           break;
         case UDT_INT:
           getAlignNum (AG_CINTTYPE, &alignNum);
           break;
         case UDT_LONG:
           getAlignNum (AG_CLONGTYPE,&alignNum);
           break;
         case UDT_DOUBLE:
           getAlignNum (AG_CDOUBLETYPE, &alignNum);
           break;
         case UDT_FLOAT:
           getAlignNum (AG_CFLOATTYPE, &alignNum);
           break;
         default:
           LOG4C ((LOG_FATAL,"错误数据类型. 类型=[%d]", fldType));
           break;
        }

        if (alignStrNums[nested][strNums[nested]] < alignNum)
        {
            alignStrNums[nested][strNums[nested]] = alignNum;
        }
    }

    return (0);
}

