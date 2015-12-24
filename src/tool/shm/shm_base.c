#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <assert.h>

#include "shm_base.h"
#include "pkg_base.h"
#include "shm_api.h"

/*
** ���ܣ��ӹ����ڴ��ж�ȡָ��������
** ���������1 void *epShmAddr �����ڴ��ַ 
**           2 int eiTblSerialNum �����к�
** ���������1 *opiNum ��¼����
** ����ֵ��    �ɹ����ر����ݵ�ַ 
**             ʧ�ܷ��� NULL ����*opiNumΪ -1;
** ˵��������Ϊ��ʱ������ֵ��Ϊ�գ�*opiNum����0
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

