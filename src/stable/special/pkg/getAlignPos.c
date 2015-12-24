#include <stdio.h>
#include "getAlignPos.h"

struct CHAR_TAG
{
    char     c;
    char     x;
};
const static unsigned int Align_Char = (int)&(((struct CHAR_TAG *)0)->x);

struct SHORT_TAG
{
    char     c;
    short    x;
};
const static unsigned int Align_Short = (int)&(((struct SHORT_TAG *)0)->x);

struct INT_TAG
{
    char     c;
    int      x;
};
const static unsigned int Align_Int = (int)&(((struct INT_TAG *)0)->x);

struct LONG_TAG
{
    char     c;
    long      x;
};
const static unsigned int Align_Long = (int)&(((struct LONG_TAG *)0)->x);

struct FLOAT_TAG
{
    char     c;
    float      x;
};
const static unsigned int Align_Float = (int)&(((struct FLOAT_TAG *)0)->x);

struct DOUBLE_TAG
{
    char     c;
    double   x;
};
const static unsigned int Align_Double = (int)&(((struct DOUBLE_TAG *)0)->x);

struct POINTER_TAG
{
    char     c;
    char     * p;
};
const static unsigned int Align_Pointer = (int)&(((struct POINTER_TAG *)0)->p);

struct STRUCT_TAG_CHAR
{
    char     c;
    struct   {char c;} x;
};
const static unsigned int Align_Struct_Char = (int)&(((struct STRUCT_TAG_CHAR *)0)->x);

struct STRUCT_TAG_SHORT
{
    char     c;
    struct   {short h;} x;
};
const static unsigned int Align_Struct_Short = (int)&(((struct STRUCT_TAG_SHORT *)0)->x);

struct STRUCT_TAG_INT
{
    char     c;
    struct   {int i;} x;
};
const static unsigned int Align_Struct_Int = (int)&(((struct STRUCT_TAG_INT *)0)->x);

struct STRUCT_TAG_LONG
{
    char     c;
    struct   {long l;} x;
};
const static unsigned int Align_Struct_Long = (int)&(((struct STRUCT_TAG_LONG *)0)->x);

struct STRUCT_TAG_FLOAT
{
    char     c;
    struct   {float f;} x;
};
const static unsigned int Align_Struct_Float = (int)&(((struct STRUCT_TAG_FLOAT *)0)->x);

struct STRUCT_TAG_DOUBLE
{
    char     c;
    struct   {double d;} x;
};
const static unsigned int Align_Struct_Double = (int)&(((struct STRUCT_TAG_DOUBLE *)0)->x);

struct STRUCT_TAG_POINTER
{
    char     c;
    struct   {char *p;} x;
};
const static unsigned int Align_Struct_Pointer = (int)&(((struct STRUCT_TAG_POINTER *)0)->x);

int getAlignNum (int dataType, int * alignStructNum)
{
    int    nextPos;
    int    alignNum;
    int    alignStrNum;

    switch (dataType)
    {
    case AG_CCHARTYPE:
        alignNum = Align_Char;
	alignStrNum = Align_Struct_Char;
        break;
    case AG_CSHORTTYPE:
        alignNum = Align_Short;
	alignStrNum = Align_Struct_Short;
        break;
    case AG_CINTTYPE:
        alignNum = Align_Int;
	alignStrNum = Align_Struct_Int;
        break;
    case AG_CLONGTYPE:
        alignNum = Align_Long;
	alignStrNum = Align_Struct_Long;
        break;
    case AG_CFLOATTYPE:
        alignNum = Align_Float;
	alignStrNum = Align_Struct_Float;
        break;
    case AG_CDOUBLETYPE:
        alignNum = Align_Double;
	alignStrNum = Align_Struct_Double;
        break;
    case AG_CPOINTERTYPE:
        alignNum = Align_Pointer;
	alignStrNum = Align_Struct_Pointer;
        break;
    }

    if (alignStructNum != NULL && alignStrNum > *alignStructNum)
    {
        *alignStructNum = alignStrNum;
    }

    return alignNum;
}

int getAlignPos (int pos, int dataType, int alignStructNum)
{
    int alignNum ;
    int nextPos;

    if (dataType != AG_CSTRUCT)
    {
       alignNum = getAlignNum(dataType, NULL); 
    } else {
       alignNum = alignStructNum;
    }

    nextPos = (pos % alignNum == 0) ? pos : pos + (alignNum - pos % alignNum);

    return nextPos;
}
