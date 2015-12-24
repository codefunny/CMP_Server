#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <assert.h>

#include "shm_base.h"
#include "pkg_base.h"
#include "shm_api.h"

/*
** 功能：从共享内存中读取指定表数据
** 输入参数：1 void *epShmAddr 共享内存地址 
**           2 int eiTblSerialNum 表序列号
** 输出参数：1 *opiNum 记录条数
** 返回值：    成功返回表数据地址 
**             失败返回 NULL 并置*opiNum为 -1;
** 说明：当表为空时，返回值不为空，*opiNum等于0
*/
void * SHM_ReadData ( char * epShmAddr, int eiTblSerialNum, int * opiNum )
{
   int iOffset;

   assert( epShmAddr != NULL );
   assert( opiNum != NULL );

   *opiNum = -1;

   if( eiTblSerialNum < 0 ||
       (size_t)eiTblSerialNum > sizeof(aiTableSize)/sizeof(int) )
   {
      return NULL;
   }

   /**/
   memcpy( &iOffset, epShmAddr + sizeof(ShmCa) + sizeof(int)*eiTblSerialNum,
           sizeof(int) );
   memcpy( opiNum, epShmAddr + iOffset + sizeof(ShmCa), sizeof(int) );
   iOffset += sizeof(int) + sizeof(ShmCa);
   /**
   memcpy( &iOffset, epShmAddr + sizeof(int)*eiTblSerialNum,
           sizeof(int) );
   memcpy( opiNum, epShmAddr + iOffset, sizeof(int) );
   iOffset += sizeof(int);
   ***/

   return (void*)(epShmAddr+iOffset);
}

/*
** end of file shm_base.c
*/

