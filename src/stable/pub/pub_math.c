/*
**file:pub_math.c
*/

#include "pub_base.h"

#include <stdio.h>
#include <assert.h>

/*°´ÉýÐòÅÅÐò*/
void PUB_UpSort( int eiNum, int *piArray )
{
   int iTop, iTmp, iSwap;

   assert( piArray != NULL );

   for( iTop = eiNum; iTop > 1; iTop-- )
   {
      for( iTmp = 0; iTmp < iTop; iTmp++ )
      {
         if( piArray[iTop - 1] < piArray[iTmp] )
         {
            iSwap = piArray[iTop - 1];
            piArray[iTop - 1] = piArray[iTmp];
            piArray[iTmp] = iSwap;
         }
      }
   }
}

/*
**end of file
*/
