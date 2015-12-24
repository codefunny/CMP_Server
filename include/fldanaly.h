#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef __FLD_ANALY_H__
#define __FLD_ANALY_H__

#define offset( Struct, StructField ) (int)(&((Struct *)NULL)->StructField)

typedef struct
{
   char aczName[25];
   char aczType[25];
   char aczLen[5];
   char aczFix[5];
   char aczVar[5];
   char aczNull[3];
} Field;

typedef struct
{
   int iFieldNum;
   Field stFlds[1000];
} FieldDesc;

void SetValue( int eiFldNum, Field *opstFld, char *epcFldVal, int eiFldLen );

void FieldAnaly( char *epcFldDesc, FieldDesc *opstFldDesc );

void FieldPrint( FieldDesc *epstFieldDesc );

#endif

