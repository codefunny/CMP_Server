#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>

#include "log.h"
#include "udo.h"
#include "udo_data.h"

struct _UDOPage
{
    int iSize;                        /* ��ҳ��С               */
    int iOffset;                      /* ��ʼƫ����             */
    int iFreeDescPos;                 /* ������������ʼλ��     */
    int iFreeValPos;                  /* ֵ������ʼλ��         */
    int iQueuePos;                    /* ��ҳ��UDOҳ���е�λ��  */
    UDOPage *pstNextPage;             /* ��һUDOҳ��ַ          */
    int iFieldNum;                    /* UDO���������          */
};

struct _UDOValDesc
{
   int        iLen;            /* ��ǰ��ֵ����           */
   int        iPos;            /* ��ǰ��ֵλ�ÿ���       */
   long       lNextValPos;     /* ��һֵ������λ�ÿ���   */
};
/* Ϊ��֤32λ��64Ϊ�Ŀ���ֲ�����һ���ֶζ���λlong */

struct _UDOFieldDesc
{
   int  iType;                 /*��ǰ������              */
   int  iFieldId;              /*��ǰ��ֵ���            */
   int  iRepeatTime;           /*��ǰ��ֵ����            */
   int  iValDscPos;            /*��һ��ֵ������λ��λ��  */
   long lNextField;            /*��һ��������λ��        */
};
/* Ϊ��֤32λ��64Ϊ�Ŀ���ֲ�����һ���ֶζ���λlong */

/*
 * ���ָ����ַUDOҳ�Ƿ���������, �Ƿ���>=0, �񷵻�-1 
 */
int UDO_PageIsInQueue( char *pPageAddr )
{
   short shPos;

   for( shPos = stUDOAllocQueueCtl.shUsedFirst; shPos != -1;
        shPos = astUDOAllocQueue[shPos].shNext )
   {
      if( pPageAddr == astUDOAllocQueue[shPos].pPage )
      {
         return shPos;
      }
   }

   return -1;
}

/*
 * ����: ��ʼ��UDO����ռ�
 */
void UDO_Startup()
{
   short sh, shMax;

   /* ���ÿ��ƽṹ���к��ѷ���ָ�� */
   stUDOAllocQueueCtl.shFreeFirst = 0;
   stUDOAllocQueueCtl.shUsedFirst = -1;
   stUDOAllocQueueCtl.shUsedLast  = -1;

   /* ����UDOҳ�����ʼֵ */
   shMax = sizeof(astUDOAllocQueue)/sizeof(UDOAllocQueue);
   astUDOAllocQueue[0].pPage = NULL;
   astUDOAllocQueue[0].shPre = -1;
   astUDOAllocQueue[0].shNext = 1;
   for( sh = 1; sh < shMax; sh++ )
   {
      astUDOAllocQueue[sh].pPage = NULL;
      astUDOAllocQueue[sh].shNext = sh + 1;
      astUDOAllocQueue[sh].shPre = sh - 1;
   }
   astUDOAllocQueue[sh-1].shNext = -1;
}

/*
**����:ȡUDO
**����:1 UDO
**���:
**����: >=0
*/
int UDO_PageSize( UDP epfAddr )
{
   UDOPage *pstUDOPage;

   assert( epfAddr );

   pstUDOPage = (UDOPage*)epfAddr;

   return pstUDOPage->iSize;
}

/*
**����:ȡUDO
**����:1 UDO
**���:
**����: >=0UDO
*/
int UDO_PageActualSize( UDP epfAddr )
{
   UDOPage *pstUDOPage;

   assert( epfAddr );

   pstUDOPage = (UDOPage*)epfAddr;

   return pstUDOPage->iSize - pstUDOPage->iFreeValPos + pstUDOPage->iFreeDescPos;
}

/*
**����: ��ʼ���������洢�ռ�, �����UDO�������ʼ�����е�UDO
**����: 1
**      2
**���: 
**����: �޷���ֵ 
*/
void UDO_Init( UDP epfAddr, int eiSize )
{
   UDOPage *pstUDOPage;

   assert( epfAddr != NULL );

   pstUDOPage = (UDOPage*)epfAddr;
   pstUDOPage->iFieldNum = 0;
   pstUDOPage->iFreeDescPos = sizeof(UDOPage);
   pstUDOPage->iFreeValPos = eiSize;
   pstUDOPage->iSize = eiSize;
   pstUDOPage->iOffset = 0;

   /*��һҳ��ַ�϶��Ƕ�̬�����, ����õ�ַ����UDOҳ����, ��ǰ�����Ǿ�̬����*/
   if( (pstUDOPage->pstNextPage == NULL) ||
       (UDO_PageIsInQueue( (char*)(pstUDOPage->pstNextPage) ) == -1) )
   {
      pstUDOPage->pstNextPage = NULL;
      return;
   }

   /* ��UDO����ÿҳ�����³�ʼ��������λ�ú�ֵλ�� */
   while( pstUDOPage->pstNextPage != NULL )
   {
      pstUDOPage = pstUDOPage->pstNextPage;
      pstUDOPage->iFreeDescPos = pstUDOPage->iOffset + sizeof(UDOPage);
      pstUDOPage->iFreeValPos = pstUDOPage->iOffset + pstUDOPage->iSize;
   }
}

/*
 * ָ����UDOҳ���, �ɹ�����0, ʧ�ܷ���-1 
 */ 
int UDO_AllocInQueue( char *pUDO )
{
   /* �޸�UDO������,����һ�����ӵ�*/
   if( stUDOAllocQueueCtl.shFreeFirst == -1 )
   {
      return -1;
   }

   /* �����޸��ѷ���ռ����� */
   if( stUDOAllocQueueCtl.shUsedFirst > -1 )
   {
      astUDOAllocQueue[stUDOAllocQueueCtl.shUsedLast].shNext =
                                               stUDOAllocQueueCtl.shFreeFirst;
      astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shPre =
                                               stUDOAllocQueueCtl.shUsedLast;
      stUDOAllocQueueCtl.shUsedLast = stUDOAllocQueueCtl.shFreeFirst;

      /* �޸Ŀ��пռ�����, �϶����ڿ������� */
      if( astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shNext > -1 )
      {
         stUDOAllocQueueCtl.shFreeFirst =
                       astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shNext;
         astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shPre = -1;
      }
      else   /* ��ǰ��������ҳ�����һ������ҳ */
      {
         stUDOAllocQueueCtl.shFreeFirst = -1;
      }
      astUDOAllocQueue[stUDOAllocQueueCtl.shUsedLast].shNext = -1;
   }
   else  /* �ѷ���ռ�������ǰΪ�� */
   {
      stUDOAllocQueueCtl.shUsedFirst = stUDOAllocQueueCtl.shFreeFirst;
      stUDOAllocQueueCtl.shUsedLast  = stUDOAllocQueueCtl.shFreeFirst;

      /* �޸Ŀ��пռ�����, �϶����ڿ������� */
      stUDOAllocQueueCtl.shFreeFirst =
                       astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shNext;
      astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shPre = -1;
      
      astUDOAllocQueue[stUDOAllocQueueCtl.shUsedFirst].shNext = -1;
      astUDOAllocQueue[stUDOAllocQueueCtl.shUsedFirst].shPre  = -1;
   }
   
   astUDOAllocQueue[stUDOAllocQueueCtl.shUsedLast].pPage = pUDO;

   return 0;
}

/*
**����:����UDO
**����:1
**���:
**����:NULL-ʧ��,����-
*/
UDP UDO_Alloc( int eiSize )
{
   char *pUDO;

   /* ����Ŀռ������ܱ���һ��double����ֵ */
   if( eiSize < (sizeof(UDOPage)+sizeof(UDOFieldDesc)) )
   {
      return NULL;
   }

   pUDO = (char*)calloc( 1, eiSize );
   if( pUDO == NULL )
   {
      return NULL;
   }

   if( UDO_AllocInQueue( pUDO ) == -1 )
   {
      free( pUDO );
      return NULL;
   }
   UDO_Init( pUDO, eiSize );

   return pUDO;
}

void UDO_PageDeQueue( int iPos )
{
   /* �����ͷ��Ѿ������UDOҳ���� */
   if( iPos != stUDOAllocQueueCtl.shUsedFirst )
   {
      if( iPos != stUDOAllocQueueCtl.shUsedLast )
      {
         astUDOAllocQueue[astUDOAllocQueue[iPos].shPre].shNext = astUDOAllocQueue[iPos].shNext;
         astUDOAllocQueue[astUDOAllocQueue[iPos].shNext].shPre = astUDOAllocQueue[iPos].shPre;
      }
      else
      {
         stUDOAllocQueueCtl.shUsedLast = astUDOAllocQueue[iPos].shPre;
         astUDOAllocQueue[stUDOAllocQueueCtl.shUsedLast].shNext = -1;
      }
   }
   else
   {
      /* ���ֵ������ֻ��һ��Ҫɾ����ֵ�ڵ�, ͬʱ�޸����һ���ڵ�λ�� */
      if( iPos != stUDOAllocQueueCtl.shUsedLast )
      {
         stUDOAllocQueueCtl.shUsedFirst = astUDOAllocQueue[iPos].shNext;
         astUDOAllocQueue[stUDOAllocQueueCtl.shUsedFirst].shPre = -1;
      }
      else
      {
         stUDOAllocQueueCtl.shUsedLast = -1;
         stUDOAllocQueueCtl.shUsedFirst = -1;
      }
   }

   /* ���뵽����UDOҳ����         */
   if( stUDOAllocQueueCtl.shFreeFirst > -1 )  /* ������һ��UDO����ҳ */
   {
      astUDOAllocQueue[iPos].shNext = stUDOAllocQueueCtl.shFreeFirst;
      astUDOAllocQueue[stUDOAllocQueueCtl.shFreeFirst].shPre = iPos;
   }
   else   /* û��UDO����ҳ����ָ�� */
   {
      astUDOAllocQueue[iPos].shNext = -1;
   }
   astUDOAllocQueue[iPos].shPre = -1;
   stUDOAllocQueueCtl.shFreeFirst = iPos;
   astUDOAllocQueue[iPos].pPage = NULL;
}

/*
 * ����: �ͷ�����UDOҳ��̬����Ŀռ� 
 */
void UDO_FreeAll()
{
   while( stUDOAllocQueueCtl.shUsedFirst != -1 )
   {
      free( astUDOAllocQueue[stUDOAllocQueueCtl.shUsedFirst].pPage );
      UDO_PageDeQueue( stUDOAllocQueueCtl.shUsedFirst );
   }
}

/*
**����:�ͷ�UDO
**����:1 UDO
**���:
**����:
*/
void UDO_Free( UDP epfAddr )
{
   UDOPage *pstPage, *pstNextPage;
   int iPos;

   if( epfAddr )
   {
      pstPage = (UDOPage*)epfAddr;
      do
      {
         pstNextPage = pstPage->pstNextPage;
         iPos = UDO_PageIsInQueue( (char*)pstPage );
         if( iPos > -1 )   /* -1��ʾUDO�����в����� */
         {
            free( pstPage );
            UDO_PageDeQueue( iPos );
         }
         pstPage = pstNextPage;
      } while( pstPage != NULL );
   }
}

/*
**����:
**����:1 UDO
**     2
**���:
**����:0-������,1-
*/
int UDO_FieldExist( UDP epfAddr, int eiFieldId )
{
   UDOFieldDesc *pstFldDesc;
   UDOPage *pstPage;

   pstPage = (UDOPage *)epfAddr;

   return (UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstFldDesc ) == NULL )? 0: 1;
}

/* @UDO_SeekFieldDesc
 * #����ָ����ŵ���������
 * %����1 ��������ҳ��ַָ��                ����������� 
 * %����2 ���                              ������� 
 * %����3 ���һ������������ַָ��          ������� NULL--��ʾUDOҳ��Ϊ�� 
 * #NULL--��ָ����ŵ��� ����--ָ����������ָ��
 * #ֻ�е�����ֵΪNULLʱ�������opstLastFldDesc�������� 
 */
UDOFieldDesc *UDO_SeekFieldDesc( UDOPage **eopstPage, int eiFieldId,
                                UDOFieldDesc **opstLastFldDesc )
{
   UDOFieldDesc *pstLastFldDesc;
   UDOPage *pstPage;
   
   pstPage = *eopstPage;
   if( pstPage->iFieldNum == 0 )  /* UDO�������Ϊ0 */
   {
      *opstLastFldDesc = NULL;
      return NULL;
   }

   /* UDO��������һ���� */
   pstLastFldDesc = (UDOFieldDesc *)((char*)pstPage + sizeof(UDOPage));
   while( pstLastFldDesc->iFieldId != eiFieldId )
   {
      while( pstLastFldDesc->lNextField > (pstPage->iOffset + pstPage->iSize) )
      {
         pstPage = pstPage->pstNextPage;
      }
      if( pstLastFldDesc->lNextField != -1 )
      {
         pstLastFldDesc = (UDOFieldDesc *)((char*)pstPage + pstLastFldDesc->lNextField - pstPage->iOffset);
      }
      else  /* ��ǰ�������һ���� */
      {
         *eopstPage = pstPage;
         *opstLastFldDesc = pstLastFldDesc;

         return NULL;
      }
   }

   *eopstPage = pstPage;

   return pstLastFldDesc;
}

/*
 * ���ܣ�����һ����������
 * �����������һ����������ΪNULL����Ҫ���ӵ����������ǵ�һ�� 
 */ 
UDOFieldDesc * UDO_AddFieldDesc( UDOPage **eopstPage, UDP epfAddr,
                                UDOFieldDesc *epstLastFldDesc,
                                int eiFieldId, int eiFieldType, int eiValLen )
{
   UDOPage *pstFirstPage, *pstPage, *pstNewPage;
   UDOFieldDesc *pstNewFldDesc, *pstLastFldDesc;
   char *pNewPage;
   int iSize;

   pstFirstPage = (UDOPage *)epfAddr;
   pstPage = *eopstPage;
   pstFirstPage = (UDOPage *)epfAddr;

   if( epstLastFldDesc == NULL )  /* ��һ���������� */
   {
      pstNewFldDesc = (UDOFieldDesc *)(epfAddr + sizeof(UDOPage));
      pstNewFldDesc->iFieldId = eiFieldId;
      pstNewFldDesc->iRepeatTime = 0;
      pstNewFldDesc->iType = eiFieldType;
      pstNewFldDesc->iValDscPos = -1;
      pstNewFldDesc->lNextField = -1;
      pstPage->iFieldNum = 1;
      pstPage->iFreeDescPos += sizeof(UDOFieldDesc);

      return pstNewFldDesc;
   }

   pstLastFldDesc = epstLastFldDesc;
   /* �����һ����������ҳ���ҵ�һ��ʣ��ռ���ڵ������������ռ��С��ҳ */
   while( (pstPage->pstNextPage != NULL) &&
          ((pstPage->iFreeValPos - pstPage->iFreeDescPos) < sizeof(UDOFieldDesc)) )
   {
      pstPage = pstPage->pstNextPage;
   }

   /* ���ҵ����һҳ���߿ռ��ѹ�, ����ռ��ѹ����÷����¿ռ� */
   if( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < sizeof(UDOFieldDesc) )
   {
   	  iSize = (pstPage->iSize > (eiValLen+100))?
              pstPage->iSize:
              eiValLen+sizeof(UDOPage)+sizeof(UDOFieldDesc);
   	  iSize = (iSize > UDO_PAGE_SIZE)? iSize: UDO_PAGE_SIZE;
      pNewPage = UDO_Alloc( iSize );
      if( pNewPage == NULL )
      {
         return NULL;
      }
      pstNewPage = (UDOPage*)pNewPage;
      pstNewPage->iOffset = pstPage->iOffset + pstPage->iSize;
      pstNewPage->iFreeDescPos += pstNewPage->iOffset;
      pstNewPage->iFreeValPos += pstNewPage->iOffset;
      pstPage->pstNextPage = pstNewPage;
      pstPage = pstNewPage;
   }

   pstNewFldDesc = (UDOFieldDesc*)((char*)pstPage + pstPage->iFreeDescPos - pstPage->iOffset);
   pstLastFldDesc->lNextField = pstPage->iFreeDescPos;
   pstPage->iFreeDescPos += sizeof(UDOFieldDesc);

   pstNewFldDesc->iFieldId = eiFieldId;
   pstNewFldDesc->iRepeatTime = 0;
   pstNewFldDesc->iType = eiFieldType;
   pstNewFldDesc->iValDscPos = -1;
   pstNewFldDesc->lNextField = -1;
   
   *eopstPage = pstPage;
   pstFirstPage->iFieldNum++;

   return pstNewFldDesc;
}

UDOValDesc *UDO_AddValueDesc( UDOPage **eopstPage, UDOFieldDesc *epstFldDesc,
                             UDOValDesc *epstLastValDesc, int eiValLen )
{
   UDOPage *pstPage, *pstNewPage;
   UDOValDesc *pstValDesc;
   char *pNewPage;
   int iSize;

   assert( eopstPage   != NULL );
   assert( epstFldDesc != NULL );

   pstPage = *eopstPage;

   /* �����ǰҳ�ռ䲻�� �����µ�ҳ�������һҳ */
   /* �����һ����������ҳ���ҵ�һ��ʣ��ռ���ڵ������������ռ��С��ҳ */
   while( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < sizeof(UDOValDesc) )
   {
      if( pstPage->pstNextPage != NULL )
      {
         pstPage = pstPage->pstNextPage;
      }
      else
      {
         break;
      }
   }
   /* ���ҵ����һҳ���߿ռ��ѹ�, ����ռ��ѹ����÷����¿ռ� */
   if( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < sizeof(UDOValDesc) )
   {
      iSize = (pstPage->iSize > (eiValLen+100))?
              pstPage->iSize:
              eiValLen+sizeof(UDOPage)+sizeof(UDOFieldDesc);
      iSize = (iSize > UDO_PAGE_SIZE)? iSize: UDO_PAGE_SIZE;
      pNewPage = UDO_Alloc( iSize );
      if( pNewPage == NULL )
      {
         return NULL;
      }
      pstNewPage = (UDOPage*)pNewPage;
      pstNewPage->iOffset = pstPage->iOffset + pstPage->iSize;
      pstNewPage->iFreeDescPos += pstNewPage->iOffset;
      pstNewPage->iFreeValPos += pstNewPage->iOffset;
      pstPage->pstNextPage = pstNewPage;
      pstPage = pstNewPage;
   }

   if( epstLastValDesc == NULL )  /* ������ĵ�һֵ������ */
   {
      epstFldDesc->iValDscPos = pstPage->iFreeDescPos;
   }
   else
   {
      epstLastValDesc->lNextValPos = pstPage->iFreeDescPos;
   }
   pstValDesc = (UDOValDesc*)((char*)pstPage + pstPage->iFreeDescPos - pstPage->iOffset);
   pstPage->iFreeDescPos += sizeof(UDOValDesc);
   pstValDesc->lNextValPos = -1;
   pstValDesc->iLen = eiValLen;
   pstValDesc->iPos = -1;
   *eopstPage = pstPage;

   return pstValDesc;
}

int UDO_SetDataLen( int eiFieldType, int eiFieldLen )
{
   switch( eiFieldType )
   {
   case UDT_SHORT:
      return SIZE_SHORT;
   case UDT_INT:
      return SIZE_INT;
   case UDT_LONG:
      return SIZE_LONG;
   case UDT_FLOAT:
      return SIZE_FLOAT;
   case UDT_DOUBLE:
      return SIZE_DOUBLE;
   case UDT_CHAR:
      return SIZE_CHAR;
   default:
      return eiFieldLen;
   }
}

UDOValDesc * UDO_SeekLastValue( UDOPage **eopstPage, UDOFieldDesc *epstFldDesc )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;
   int           iNextPos;

   assert( eopstPage   != NULL );
   assert( epstFldDesc != NULL );

   pstPage = *eopstPage;
   pstFldDesc = epstFldDesc;
   
   if( pstFldDesc->iValDscPos == -1 )
   {
      return NULL;
   }

   iNextPos = pstFldDesc->iValDscPos;
   do
   {
      /* ������һֵ����������ҳ */
      while( iNextPos > (pstPage->iOffset + pstPage->iSize) )
      {
         if( pstPage->pstNextPage )
         {
            pstPage = pstPage->pstNextPage;
         }
         else
         {
            break;
         }
      }
      pstValDesc = (UDOValDesc*)((char*)pstPage + iNextPos - pstPage->iOffset);
      iNextPos = pstValDesc->lNextValPos;
   } while( iNextPos != -1 );

   *eopstPage = pstPage;

   return pstValDesc;
}

int UDO_AddValue( UDOPage *epstPage, UDOFieldDesc *epstFldDesc,
                 UDOValDesc *epstValDesc, char *epValue,
                 int eiFieldType, int eiValLen )
{
   UDOPage *pstPage, *pstNewPage;
   int iSize;
   char *pNewPage, *pAddr;

   pstPage = epstPage;
   assert( epstPage    != NULL );
   assert( epstFldDesc != NULL );
   assert( epstValDesc != NULL );
   assert( epValue     != NULL );

   while( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < eiValLen )
   {
      if( pstPage->pstNextPage != NULL )
      {
         pstPage = pstPage->pstNextPage;
      }
      else
      {
         break;
      }
   }
   
   /* ���ҵ����һҳ���߿ռ��ѹ�, ����ռ��ѹ����÷����¿ռ� */
   if( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < eiValLen )
   {
      iSize = (pstPage->iSize > (eiValLen+100))?
              pstPage->iSize:
              eiValLen+sizeof(UDOPage)+sizeof(UDOFieldDesc);
      iSize = (iSize > UDO_PAGE_SIZE)? iSize: UDO_PAGE_SIZE;
      pNewPage = UDO_Alloc( iSize );
      if( pNewPage == NULL )
      {
         return UDO_TOO_SMALL;
      }
      pstNewPage = (UDOPage*)pNewPage;
      pstNewPage->iOffset = pstPage->iOffset + pstPage->iSize;
      pstPage->pstNextPage = pstNewPage;
      pstNewPage->iFreeDescPos += pstNewPage->iOffset;
      pstNewPage->iFreeValPos += pstNewPage->iOffset;
      pstPage = pstNewPage;
   }

   /* �޸����ظ�ֵ������ֵλ�� */
   pstPage->iFreeValPos -= eiValLen;
   epstFldDesc->iRepeatTime++;
   epstValDesc->iPos = pstPage->iFreeValPos;

   pAddr = (char*)pstPage + pstPage->iFreeValPos - pstPage->iOffset;
   memcpy( pAddr, epValue, eiValLen );

   return 0;
}

/*
**����:��UDO���Ϳռ�����һ����ֵ,����򲻴���,���Ӹ��򲢱���ֵ,
**
**����:1 UDO
**     2
**     3
**     4
**     5
**���:
**����:0-�ɹ�,����-
*/
int UDO_AddField( UDP epfAddr, int eiFieldId, int eiFieldType,
                 char * epValue, int eiValLen )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc, *pstLastValDesc;

   assert( epfAddr != NULL );
   assert( epValue != NULL );

   pstPage = (UDOPage*)epfAddr;
   /* ��������������ȷ�ĳ��� */
   eiValLen = UDO_SetDataLen( eiFieldType, eiValLen );

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );

   /* ��������������������� */
   if( pstFldDesc == NULL )
   {
      /* ��������������ʧ�ܷ���NULL */
      pstFldDesc = UDO_AddFieldDesc( &pstPage, epfAddr, pstLastFldDesc,
                                    eiFieldId, eiFieldType, eiValLen );
      if( pstFldDesc == NULL )
      {
         return UDO_TOO_SMALL;
      }
   }

   /* ���Ҹ�ֵ���һ��ֵ�����������ڷ������һ��ֵ������ָ�� */
   pstLastValDesc = UDO_SeekLastValue( &pstPage, pstFldDesc );

   pstValDesc = UDO_AddValueDesc( &pstPage, pstFldDesc, pstLastValDesc, eiValLen );
   if( pstValDesc == NULL )
   {
      return UDO_TOO_SMALL;
   }

   return UDO_AddValue( pstPage, pstFldDesc, pstValDesc, epValue,
                       eiFieldType, eiValLen );
}

/*
**����:
**����:1 UDO
**     2
**     3
**���:1
**     2
**����:0-�ɹ�,����-
*/
int UDO_GetField( UDP epfAddr, int eiFieldId, int eiFieldOccur,
                 char * opValue, int *opValLen )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc;
   int           iTemp, iPos;

   assert( epfAddr != NULL );
   assert( opValue != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }
   
   if( (eiFieldOccur < 0) || eiFieldOccur > (pstFldDesc->iRepeatTime-1) )
   {
      return UDO_INVALID_OCCUR;
   }

   /* �Ѿ��ҵ��������ظ�������ȷ */
   iTemp = 0;
   iPos = pstFldDesc->iValDscPos;
   do
   {
      /* ���ҵ�ǰֵ����������ҳ */
      while( (pstPage->iOffset+pstPage->iSize) < iPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstValDesc = (UDOValDesc*)((char*)pstPage + iPos - pstPage->iOffset);
      if( iTemp == eiFieldOccur )
      {
         break;
      }
      iPos = pstValDesc->lNextValPos;
      iTemp++;
   } while( iPos != -1 );

   /* ����ֵλ������ҳ */
   iPos = pstValDesc->iPos;
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }

   memcpy( opValue, (char*)pstPage + iPos - pstPage->iOffset, pstValDesc->iLen );

   if( opValLen )
   {
      *opValLen = pstValDesc->iLen;
   }

   return 0;
}

/*
**����:
**����:1 UDO
**     2
**���:
**����: >0�ظ�����, <0
*/
int UDO_FieldOccur( UDP epfAddr, int eiFieldId )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;

   assert( epfAddr != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }

   return pstFldDesc->iRepeatTime;
}

/* ����ָ���ظ���������ָ��ֵ������                                       */
/* ʹ�ñ�����ǰ�Ѿ�ȷ��ֵ�����Ҳ��ᳬ���ظ�������Χ����˲����ǳ�������� */
UDOValDesc * UDO_SeekValueByOccur( UDOPage **eopstPage,
                                  UDOFieldDesc *epstFldDesc,
                                  int eiOccur )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;
   int           iNextPos, iTmp;

   assert( eopstPage   != NULL );
   assert( epstFldDesc != NULL );

   pstPage = *eopstPage;
   pstFldDesc = epstFldDesc;

   if( pstFldDesc->iValDscPos == -1 )
   {
      return NULL;
   }

   iNextPos = pstFldDesc->iValDscPos;
   iTmp = 0;
   do
   {
      /* ������һֵ����������ҳ */
      while( iNextPos > (pstPage->iOffset + pstPage->iSize) )
      {
         if( pstPage->pstNextPage )
         {
            pstPage = pstPage->pstNextPage;
         }
         else
         {
            break;
         }
      }
      pstValDesc = (UDOValDesc*)((char*)pstPage + iNextPos - pstPage->iOffset);
      iNextPos = pstValDesc->lNextValPos;
      if( iTmp == eiOccur )
      {
         break;
      }
      iTmp++;
   } while( iNextPos != -1 );

   *eopstPage = pstPage;

   return pstValDesc;
}

/* ����λ�ô����ֵ, ���Ϳ϶�Ϊ�ַ������߶����� */
int UDO_AddValueAtNew( UDOPage *epstPage, UDOValDesc *epstValDesc,
                      char *epValue, int eiValLen)
{
   UDOPage *pstPage, *pstNewPage;
   int iSize;
   char *pNewPage, *pAddr;

   assert( epstPage    != NULL );
   assert( epstValDesc != NULL );
   assert( epValue     != NULL );

   pstPage = epstPage;
   
   while( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < eiValLen )
   {
      if( pstPage->pstNextPage != NULL )
      {
         pstPage = pstPage->pstNextPage;
      }
      else
      {
         break;
      }
   }
   
   /* ���ҵ����һҳ���߿ռ��ѹ�, ����ռ��ѹ����÷����¿ռ� */
   if( (pstPage->iFreeValPos - pstPage->iFreeDescPos) < eiValLen )
   {
      iSize = (pstPage->iSize > (eiValLen+100))?
              pstPage->iSize:
              eiValLen+sizeof(UDOPage)+sizeof(UDOFieldDesc);
      iSize = (iSize > UDO_PAGE_SIZE)? iSize: UDO_PAGE_SIZE;
      pNewPage = UDO_Alloc( iSize );
      if( pNewPage == NULL )
      {
         return UDO_TOO_SMALL;
      }
      pstNewPage = (UDOPage*)pNewPage;
      pstNewPage->iOffset = pstPage->iOffset + pstPage->iSize;
      pstPage->pstNextPage = pstNewPage;
      pstNewPage->iFreeDescPos += pstNewPage->iOffset;
      pstNewPage->iFreeValPos += pstNewPage->iOffset;
      pstPage = pstNewPage;
   }

   /* �޸����ظ�ֵ������ֵλ�� */
   pstPage->iFreeValPos -= eiValLen;
   epstValDesc->iPos = pstPage->iFreeValPos;
   epstValDesc->iLen = eiValLen;

   pAddr = (char*)pstPage + pstPage->iFreeValPos - pstPage->iOffset;
   memcpy( pAddr, epValue, eiValLen );

   return 0;
}

/* ����ָ��ֵ��������ֵ */
int UDO_UpdateValue( UDOPage *epstPage, UDOValDesc *epstValDesc,
                    int eiType, char *epValue, int eiFieldLen )
{
   UDOPage *pstPage;
   int iPos;
   
   assert( epstPage    != NULL );
   assert( epstValDesc != NULL );
   
   pstPage = epstPage;
   /* ����ֵλ������ҳ */
   iPos = epstValDesc->iPos;
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }
   /* �ж��������ͣ�����������ֱ���޸�ֵ�����ַ����Ͷ��������͸���ֵ */
   if( (eiType == UDT_STRING) || (eiType == UDT_BIN) )
   {
      if( epstValDesc->iLen >= eiFieldLen ) /* ��ǰֵ�ռ��ܱ�����ֵ */
      {
         memcpy( (char*)pstPage + iPos - pstPage->iOffset,
                 epValue, eiFieldLen );
         epstValDesc->iLen = eiFieldLen;
      }
      else  /* ��ǰֵ�ռ��С����ʱ�����ҿ��ÿռ���������µ�ҳ */
      {
         return UDO_AddValueAtNew( pstPage, epstValDesc, epValue, eiFieldLen);
      }
   }
   else
   {
      memcpy( (char*)pstPage + iPos - pstPage->iOffset,
              epValue, eiFieldLen );
   }

   return 0;
}

/*
**����:�ı�ָ����ָ���ظ�������ֵ,���û�и��򷵻ش�,
**
**����:1 UDO
**     2 ��ID
**     3
**     4
**     5
**���:
**����:  0-�ɹ�,����ʧ��(�ռ��С���������޴���)
*/
int UDO_ChgField( UDP epfAddr, int eiFieldId, int eiFieldOccu,
                 char *epValue, int eiFieldLen )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc, *pstLastValDesc;

   assert( epfAddr != NULL );
   assert( epValue != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );

   /* �����������������, ʧ�� */
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }

   /* ��������������ȷ�ĳ��� */
   eiFieldLen = UDO_SetDataLen( pstFldDesc->iType, eiFieldLen );

   if( (eiFieldOccu >= 0) && (eiFieldOccu < pstFldDesc->iRepeatTime) )
   {
      /* �Ȳ���ָ���ظ�����ֵ���ڵ�ֵ��������Ȼ���޸��Ѿ����ڵ�ֵ */
      pstValDesc = UDO_SeekValueByOccur( &pstPage, pstFldDesc, eiFieldOccu );
      return UDO_UpdateValue( pstPage, pstValDesc, pstFldDesc->iType, epValue,
                             eiFieldLen );
   }
   else   /* ���ָ��������ֵ������, UDO_AddField����һ��ֵ */
   {
      /* ���Ҹ�ֵ���һ��ֵ�����������ڷ������һ��ֵ������ָ�� */ 
      pstLastValDesc = UDO_SeekLastValue( &pstPage, pstFldDesc );

      pstValDesc = UDO_AddValueDesc( &pstPage, pstFldDesc,
                                    pstLastValDesc, eiFieldLen );
      if( pstValDesc == NULL )
      {
         return UDO_TOO_SMALL;
      }

      return UDO_AddValue( pstPage, pstFldDesc, pstValDesc, epValue,
                          pstFldDesc->iType, eiFieldLen );
   }
}

/*
**����:
**����:1 UDO
**���:
**����: >=0
*/
int UDO_FieldCount( UDP epfAddr )
{
   UDOPage      *pstPage;

   assert( epfAddr != NULL );
   
   pstPage = (UDOPage *)epfAddr;

   return pstPage->iFieldNum;
}

/*
**����:ȡUDO������ҳ��С֮�� 
**����:1 UDO
**���:
**����: >=0
*/
int UDO_Size( UDP epfAddr )
{
   UDOPage      *pstPage;

   assert( epfAddr != NULL );
   
   pstPage = (UDOPage *)epfAddr;

   while( pstPage->pstNextPage != NULL )
   {
      pstPage = pstPage->pstNextPage;
   }

   return pstPage->iOffset + pstPage->iSize;
}

/*
**����:ȡUDO���и�ҳʵ�ʴ�С֮�ͣ����ܻ���ΪUDO_ChgField�����ֵ��С֮�Ͳ��� 
**����:1 UDO
**���:
**����: >=0UDO
*/
int UDO_ValueSize( UDP epfAddr )
{
   UDOPage *pstPage;
   int iSize;

   assert( epfAddr != NULL );
   
   pstPage = (UDOPage *)epfAddr;

   iSize = 0;
   do
   {
      iSize += pstPage->iSize - pstPage->iFreeValPos +
               pstPage->iFreeDescPos - sizeof(UDOPage);
   } while( pstPage->pstNextPage && (pstPage = pstPage->pstNextPage) );

   return iSize + sizeof(UDOPage);
}

/*
**����:
**����:1 UDO
**     2
**���:
**����:>0,�����ͣ�<0
*/
int UDO_FieldType( UDP epfAddr, int eiFieldId )
{
   UDOFieldDesc *pstFldDesc;
   UDOPage *pstPage;

   pstPage = (UDOPage *)epfAddr;

   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }
   
   return pstFldDesc->iType;
}

/*
**����:
**����:1 UDO
**     2
**���:1
**����:0-�ɹ�������ʧ��(����UDO������ţ���Ŵ�0��ʼ) 
*/
int UDO_FieldId( UDP epfAddr, int eiFldSeq, int *opiFldId )
{
   UDOFieldDesc *pstFldDesc;
   UDOPage *pstPage;
   int iTmp;
   
   pstPage = (UDOPage*)epfAddr;
   if( (pstPage->iFieldNum == 0) || (eiFldSeq < 0) )  /* UDO�������Ϊ0 */
   {
      return UDO_OVER_RANGE;
   }

   iTmp = 0;
   /* UDO��������һ���� */
   pstFldDesc = (UDOFieldDesc *)((char*)pstPage + sizeof(UDOPage));
   while( iTmp != eiFldSeq )
   {
      while( pstFldDesc->lNextField > (pstPage->iOffset + pstPage->iSize) )
      {
         pstPage = pstPage->pstNextPage;
      }
      if( pstFldDesc->lNextField != -1 )
      {
         pstFldDesc = (UDOFieldDesc *)((char*)pstPage + pstFldDesc->lNextField - pstPage->iOffset);
      }
      else  /* ��ǰ�������һ���� */
      {
         break;
      }
      iTmp++;
   }
   
   if( iTmp == eiFldSeq )
   {
      *opiFldId = pstFldDesc->iFieldId;
      return 0;
   }
   else
   {
      return UDO_OVER_RANGE;
   }
}

/* ȡ��ǰϵͳʱ�� YYYYMMDD:hhmmss */
void UDO_GetTime( char *opczTime )
{
   time_t tTime;
   struct tm *pstLocalTime;

   assert( opczTime != NULL );

   time( &tTime );
   pstLocalTime = localtime( &tTime );
   sprintf( opczTime, "Date: %04d%02d%02d Time: %02d%02d%02d",
            pstLocalTime->tm_year + 1900,
            pstLocalTime->tm_mon + 1,
            pstLocalTime->tm_mday,
            pstLocalTime->tm_hour,
            pstLocalTime->tm_min,
            pstLocalTime->tm_sec );
}
#if 0
void UDO_PrintValue( FILE *fp, char *epVal, int eiSeq, int eiType, int eiLen )
{
   Var uVar;
   int i;
   char c;

   if( (eiType != UDT_STRING) && (eiType != UDT_BIN) )
   {
      memcpy( &uVar, epVal, eiLen );
   }
   else
   {
      uVar.pcVal = epVal;
   }

   switch( eiType )
   {
   case UDT_CHAR:
      fprintf( fp, "%-4d %-7d %c\n", eiSeq, eiLen, uVar.cVal );
      break;
   case UDT_SHORT:
      fprintf( fp, "%-4d %-7d %d\n", eiSeq, eiLen, uVar.shVal );
      break;
   case UDT_INT:
      fprintf( fp, "%-4d %-7d %d\n", eiSeq, eiLen, uVar.iVal );
      break;
   case UDT_LONG:
      fprintf( fp, "%-4d %-7d %ld\n", eiSeq, eiLen, uVar.lVal );
      break;
   case UDT_FLOAT:
      fprintf( fp, "%-4d %-7d %f\n", eiSeq, eiLen, uVar.fVal );
      break;
   case UDT_DOUBLE:
      fprintf( fp, "%-4d %-7d %lf\n", eiSeq, eiLen, uVar.dVal );
      break;
   case UDT_STRING:
      fprintf( fp, "%-4d %-7d ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%c", c );
      }
      fprintf( fp, "\n" );
      break;
   case UDT_BIN:
      fprintf( fp, "%-4d %-7d ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%02X", c&0xFF );
      }
      fprintf( fp, "\n" );
      break;
   default:
      fprintf( fp, "%-4d %-7d ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%02X", c&0xFF );
      }
      fprintf( fp, "\n" );
      break;
      return;
   }
}
#endif /****************************************/
void UDO_PrintValue( FILE *fp, char *epVal, int eiSeq, int eiType, int eiLen )
{
   Var uVar;
   int i;
   char c;

   if( (eiType != UDT_STRING) && (eiType != UDT_BIN) )
   {
      memcpy( &uVar, epVal, eiLen );
   }
   else
   {
      uVar.pcVal = epVal;
   }

   switch( eiType )
   {
   case UDT_CHAR:
      fprintf( fp, "%-d| %-d| %c\n", eiSeq, eiLen, uVar.cVal );
      break;
   case UDT_SHORT:
      fprintf( fp, "%-d| %-d| %d\n", eiSeq, eiLen, uVar.shVal );
      break;
   case UDT_INT:
      fprintf( fp, "%-d| %-d| %d\n", eiSeq, eiLen, uVar.iVal );
      break;
   case UDT_LONG:
      fprintf( fp, "%-d| %-d| %ld\n", eiSeq, eiLen, uVar.lVal );
      break;
   case UDT_FLOAT:
      fprintf( fp, "%-d| %-d| %f\n", eiSeq, eiLen, uVar.fVal );
      break;
   case UDT_DOUBLE:
      fprintf( fp, "%-d| %-d| %lf\n", eiSeq, eiLen, uVar.dVal );
      break;
   case UDT_STRING:
      fprintf( fp, "%-d| %-d| ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%c", c );
      }
      fprintf( fp, "\n" );
      break;
   case UDT_BIN:
      fprintf( fp, "%-d| %-d| ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%02X", c&0xFF );
      }
      fprintf( fp, "\n" );
      break;
   default:
      fprintf( fp, "%-d| %-d| ", eiSeq, eiLen );
      for( i = 0; i < eiLen; i++ )
      {
         c = *(uVar.pcVal+i);
         fprintf( fp, "%02X", c&0xFF );
      }
      fprintf( fp, "\n" );
      break;
      return;
   }
}
void UDO_FilePrint( UDP epfAddr, FILE *fp )
{
   UDOPage      *pstPage, *pstTmpPage, *pstPrePage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;
   int           iFldCount, iValCount, i, j, iPos;
   char         *pVal, aczPrintTime[46];
   char         *pczTypeInfo[] = {
                      "string",
                      "short integer",
                      "integer",
                      "long integer",
                      "float number",
                      "double number",
                      "binary data",
                      "single charecter" };
   char         *pczTypeInfo2[] = {
                      "S",
                      "Sh",
                      "I",
                      "L",
                      "F",
                      "D",
                      "B",
                      "C" };

   assert( epfAddr != NULL );
   assert( fp      != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����ж������ */
   iFldCount = pstPage->iFieldNum;

   memset( aczPrintTime, 0, sizeof(aczPrintTime) );
   UDO_GetTime( aczPrintTime );
   fprintf( fp, "%s\nField Storage Struct Field Content List\n", aczPrintTime );
   for( i = 0; i < iFldCount; i++ )
   {
      if( i > 0 )  /* ���ڷǵ�һ���������һ���������� */
      {
         iPos = pstFldDesc->lNextField;
         while( (pstPage->iOffset + pstPage->iSize) < iPos )
         {
            if( pstPage->pstNextPage )
            {
               pstPage = pstPage->pstNextPage;
            }
            else
            {
               fprintf( fp, "Invalid field %d next field descriptor position %d",
                        pstFldDesc->iFieldId, iPos );
               return;
            }
         }
         pstFldDesc = (UDOFieldDesc*)((char*)pstPage + iPos - pstPage->iOffset);
      }
      else
      {
         pstFldDesc = (UDOFieldDesc*)((char*)pstPage + sizeof(UDOPage));
      }
      iPos = pstFldDesc->iValDscPos;
      pstPrePage = pstPage;
      /* ���ҵ�һֵλ�� */
      while( (pstPrePage->iOffset + pstPrePage->iSize) < iPos )
      {
         if( pstPrePage->pstNextPage )
         {
            pstPrePage = pstPrePage->pstNextPage;
         }
         else
         {
            fprintf( fp, "Invalid field %d first value description position %d",
                     pstFldDesc->iFieldId, iPos );
            return;
         }
      }
      pstTmpPage = pstPrePage;
      pstValDesc = (UDOValDesc*)((char*)pstTmpPage + iPos - pstTmpPage->iOffset);
      iValCount = pstFldDesc->iRepeatTime;
      /***************************fprintf( fp, "field id %d, %d values, %s\n",
      ************************/
      fprintf( fp, "id %d, %d , %s\n",
               pstFldDesc->iFieldId, pstFldDesc->iRepeatTime,
               pczTypeInfo2[pstFldDesc->iType] );
      iPos = pstValDesc->iPos;
      j = 0;
      do
      {
         /* ����ֵλ�� */
         while( (pstTmpPage->iOffset + pstTmpPage->iSize) < iPos )
         {
            if( pstTmpPage->pstNextPage )
            {
               pstTmpPage = pstTmpPage->pstNextPage;
            }
            else
            {
               fprintf( fp, "Invalid field %d field %d value position %d",
                        pstFldDesc->iFieldId, j, iPos );
               return;
            }
         }
         /* ��ӡֵ */
         pVal = (char*)pstTmpPage + iPos - pstTmpPage->iOffset;
         UDO_PrintValue( fp, pVal, j, pstFldDesc->iType, pstValDesc->iLen );
         iPos = pstValDesc->lNextValPos;
         while( (pstTmpPage->iOffset + pstTmpPage->iSize) < iPos )
         {
            if( pstTmpPage->pstNextPage )
            {
               pstTmpPage = pstTmpPage->pstNextPage;
            }
            else
            {
               fprintf( fp, "Invalid field %d field %d value description position %d",
                        pstFldDesc->iFieldId, j+1, iPos );
               return;
            }
         }
         pstValDesc = (UDOValDesc*)((char*)pstTmpPage + iPos - pstTmpPage->iOffset);
         if( iPos != -1 )
         {
            iPos = pstValDesc->iPos;
         }
      } while( (++j < iValCount) && (iPos != -1));
   }
}

/*
**����:��ӡUDO
**����:1 UDO
**���:
**����:
*/
void UDO_Print( UDP epfAddr )
{
   UDO_FilePrint( epfAddr, stdout );
   fflush( stdout );
}

void UDO_FPrint( char * epczFileName, UDP epfAddr )
{
   FILE *fp;

   fp = fopen( epczFileName, "a" );
   if( fp == NULL )
   {
      return;
   }
   
   UDO_FilePrint( epfAddr, fp );
   fprintf( fp, "\n\n" );
   fclose( fp );
}

/*
**����:
**����:1 UDO
**     2
**     3
**���:
**����:>=0��ȷ,��ֵ����,��������,
*/
int UDO_FieldLen( UDP epfAddr, int eiFieldId, int eiOccur )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc;
   int           iTemp, iPos;

   assert( epfAddr != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }
   
   if( (eiOccur < 0) || eiOccur > (pstFldDesc->iRepeatTime-1) )
   {
      return UDO_INVALID_OCCUR;
   }

   /* �Ѿ��ҵ��������ظ�������ȷ */
   iTemp = 0;
   iPos = pstFldDesc->iValDscPos;
   do
   {
      /* ���ҵ�ǰֵ����������ҳ */
      while( (pstPage->iOffset+pstPage->iSize) < iPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstValDesc = (UDOValDesc*)((char*)pstPage + iPos - pstPage->iOffset);
      if( iTemp == eiOccur )
      {
         break;
      }
      iPos = pstValDesc->lNextValPos;
      iTemp++;
   } while( iPos != -1 );

   /* ����ֵλ������ҳ */
   iPos = pstValDesc->iPos;
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }

   return pstValDesc->iLen;
}

/*
**funtion:get the first field by the fieldid and return the next field address
**input:1 UDO address
**      2 field id
**output:1 field value
**       2 field len
**       3 second field address offset
**return:0-success,other-error,see also UDO_GetField
*/
int UDO_GetFirstField( UDP epfAddr, int eiFieldId, char *opValue,
                      int *opiValLen, int *opiSecondOffset )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc;
   int           iPos;

   assert( epfAddr != NULL );
   assert( opValue != NULL );
   assert( opiSecondOffset != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }
   
   /* �Ѿ��ҵ��������ظ�������ȷ */
   iPos = pstFldDesc->iValDscPos;
   /* ���ҵ�ǰֵ����������ҳ */
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }
   pstValDesc = (UDOValDesc*)((char*)pstPage + iPos - pstPage->iOffset);
   iPos = pstValDesc->lNextValPos;

   /* ����ֵλ������ҳ */
   iPos = pstValDesc->iPos;
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }

   memcpy( opValue,
           (char*)pstPage + iPos - pstPage->iOffset,
           pstValDesc->iLen );

   if( opiValLen )
   {
      *opiValLen = pstValDesc->iLen;
   }

   *opiSecondOffset = pstValDesc->lNextValPos;

   return 0;
}

/*
**function:get the current field value and next field address
**input: 1 pfs address
**       2 current field offset
**output:1 current field value
**       2 current field len
**       3 next field address
**return:0-correct,other-error,see also UDO_GetField
*/
int UDO_GetCurrField( UDP epfAddr, int eiCurrFielOffset, char *opValue,
                     int *opiValLen, int *opiNextFieldOffset )
{
   UDOPage      *pstPage;
   UDOValDesc   *pstValDesc;
   int           iPos;

   /* ���ݵ�ǰֵλ��ȡ��ǰֵ����ֵ��һ��ַλ�� */
   assert( epfAddr            != NULL );
   assert( opValue            != NULL );
   assert( opiNextFieldOffset != NULL );

   pstPage = (UDOPage*)epfAddr;

   /* ���ҵ�ǰֵ���������ڵ�ҳ */
   while( (pstPage->iOffset+pstPage->iSize) < eiCurrFielOffset )
   {
      pstPage = pstPage->pstNextPage;
   }

   pstValDesc = (UDOValDesc*)((char*)pstPage + eiCurrFielOffset - pstPage->iOffset);
   iPos = pstValDesc->iPos;
   
   /* ����ֵ���ڵ�ҳ */
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }

   memcpy( opValue,
           (char*)pstPage + iPos - pstPage->iOffset,
           pstValDesc->iLen );

   if( opiValLen )
   {
      *opiValLen = pstValDesc->iLen;
   }

   *opiNextFieldOffset = pstValDesc->lNextValPos;

   return 0;
}

/*
**����:��һ��UDO�ռ丽��������һ��UDP
**����:1 Ŀ��UDO
**     2 ԴUDO
**���:1 Ŀ��UDO
**����:0-�ɹ�������ʧ��
*/
int UDO_Append( UDP eopfTargetAddr, UDP epfSourceAddr )
{
   int iFldDescPos, iValDescPos, iPos, iRet;
   UDOPage *pstPage, *pstTmpPage, *pstPrePage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;

   assert( eopfTargetAddr != NULL );
   assert( epfSourceAddr  != NULL );

   pstPage = (UDOPage*)epfSourceAddr;
   /* ����Դ UDO ÿ�����ÿ��ֵ */

   iFldDescPos = sizeof(UDOPage);
   do
   {
      /* ����������������ҳ */
      while( (pstPage->iOffset + pstPage->iSize) < iFldDescPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstFldDesc = (UDOFieldDesc*)((char*)pstPage + iFldDescPos - pstPage->iOffset);
      iFldDescPos = (int)pstFldDesc->lNextField;

      iValDescPos = pstFldDesc->iValDscPos;
      pstPrePage = pstPage;
      do
      {
         /* ����ֵ����������ҳ */
         while( (pstPrePage->iOffset + pstPrePage->iSize) < iValDescPos )
         {
            pstPrePage = pstPrePage->pstNextPage;
         }
         pstTmpPage = pstPrePage;
         pstValDesc = (UDOValDesc*)((char*)pstTmpPage + iValDescPos - pstTmpPage->iOffset);
         iValDescPos = pstValDesc->lNextValPos;
         iPos = pstValDesc->iPos;

         pstPrePage = pstTmpPage;
         /* ����ֵ����ҳ */
         while( (pstTmpPage->iOffset + pstTmpPage->iSize) < iPos )
         {
            pstTmpPage = pstTmpPage->pstNextPage;
         }

         iRet = UDO_AddField( eopfTargetAddr, pstFldDesc->iFieldId,
                             pstFldDesc->iType,
                             (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                             pstValDesc->iLen );
         if( iRet != 0 )
         {
            return iRet;
         }
      } while( iValDescPos != -1 );
   } while( iFldDescPos != -1 );

   return 0;
}

/*
**����:UDO
**����:1 ԴUDO
**���:1 Ŀ��UDO
**����:0-�ɹ�,����ʧ��(Ŀ��ռ�̫С)
*/
int UDO_Copy( UDP opfTargetAddr, UDP epfSourceAddr )
{
   assert( opfTargetAddr != NULL );
   assert( epfSourceAddr != NULL );
   
   UDO_Init( opfTargetAddr, UDO_PageSize(opfTargetAddr) );
   return UDO_Append( opfTargetAddr, epfSourceAddr );
}

/*
** ��ĳ����ֵת��Ϊָ�����͵�ֵ 
** �����Ʋ�֧��ת�� 
*/
int UDO_ChgType( char *epVal, int iSourceType, int iNewType,
                char *opValue, int eiValLen, int *opiValLen )
{
   Var unVar, unNewVar;
   int iLen;
   char acValue[1024], *pVal;

   /***
   if( iNewType == UDT_BIN || iSourceType == UDT_BIN )
   {
      return UDO_TYPE_ERR;
   }
   ***/

   pVal = NULL;
   unVar.pcVal = acValue;
   if( eiValLen <= sizeof(acValue) )
   {
      memset( acValue, 0, sizeof(acValue) );
   }
   else
   {
      do
      {
         pVal = (char*)calloc(eiValLen+1,1);
      } while( pVal == NULL );
      unVar.pcVal = pVal;
   }

   if( iSourceType != UDT_STRING )
   {
      memcpy( &unVar, epVal, eiValLen );
   }
   else
   {
      memcpy( unVar.pcVal, epVal, eiValLen );
   }
   switch( iSourceType )
   {
   case UDT_SHORT:
      switch( iNewType )
      {
      case UDT_INT:
         unNewVar.iVal = (int)unVar.shVal;
         memcpy( opValue, &(unNewVar.iVal), SIZE_INT );
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         unNewVar.lVal = (long)unVar.shVal;
         memcpy( opValue, &(unNewVar.lVal), SIZE_LONG );
         iLen = SIZE_LONG;
         break;
      case UDT_FLOAT:
         unNewVar.fVal = (float)unVar.shVal;
         memcpy( opValue, &(unNewVar.fVal), SIZE_FLOAT );
         iLen = SIZE_FLOAT;
         break;
      case UDT_DOUBLE:
         unNewVar.dVal = (double)unVar.shVal;
         memcpy( opValue, &(unNewVar.dVal), SIZE_DOUBLE );
         iLen = SIZE_DOUBLE;
         break;
      case UDT_STRING:
         sprintf( opValue, "%d", unVar.shVal );
         iLen = strlen( opValue );
         break;
      default:
         memcpy (opValue, &(unVar.shVal), SIZE_SHORT);
         iLen = SIZE_SHORT;
         break;
      }
      break;
   case UDT_INT:
      switch( iNewType )
      {
      case UDT_SHORT:
         unNewVar.shVal = (short)unVar.iVal;
         memcpy( opValue, &(unNewVar.shVal), SIZE_SHORT );
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         unNewVar.lVal = (long)unVar.iVal;
         memcpy( opValue, &(unNewVar.lVal), SIZE_LONG );
         iLen = SIZE_LONG;
         break;
      case UDT_FLOAT:
         unNewVar.fVal = (float)unVar.iVal;
         memcpy( opValue, &(unNewVar.fVal), SIZE_FLOAT );
         iLen = SIZE_FLOAT;
         break;
      case UDT_DOUBLE:
         unNewVar.dVal = (double)unVar.iVal;
         memcpy( opValue, &(unNewVar.dVal), SIZE_DOUBLE );
         iLen = SIZE_DOUBLE;
         break;
      case UDT_STRING:
         sprintf( opValue, "%d", unVar.iVal );
         iLen = strlen( opValue );
         break;
      default:
         memcpy (opValue, &(unVar.iVal), SIZE_INT);
         iLen = SIZE_INT;
         break;
      }
      break;
   case UDT_LONG:
      switch( iNewType )
      {
      case UDT_SHORT:
         unNewVar.shVal = (short)unVar.lVal;
         memcpy( opValue, &(unNewVar.lVal), SIZE_SHORT );
         iLen = SIZE_INT;
         break;
      case UDT_INT:
         unNewVar.iVal = (int)unVar.lVal;
         memcpy( opValue, &(unNewVar.iVal), SIZE_INT );
         iLen = SIZE_INT;
         break;
      case UDT_FLOAT:
         unNewVar.fVal = (float)unVar.lVal;
         memcpy( opValue, &(unNewVar.fVal), SIZE_FLOAT );
         iLen = SIZE_FLOAT;
         break;
      case UDT_DOUBLE:
         unNewVar.dVal = (double)unVar.lVal;
         memcpy( opValue, &(unNewVar.dVal), SIZE_DOUBLE );
         iLen = SIZE_DOUBLE;
         break;
      case UDT_STRING:
         sprintf( opValue, "%ld", unVar.lVal );
         iLen = strlen( opValue );
         break;
      default:
         memcpy (opValue, &(unVar.lVal), SIZE_LONG);
         iLen = SIZE_LONG;
         break;
      }
      break;
   case UDT_FLOAT:
      switch( iNewType )
      {
      case UDT_INT:
         unNewVar.iVal = (int)unVar.fVal;
         memcpy( opValue, &(unNewVar.iVal), SIZE_INT );
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         unNewVar.lVal = (long)unVar.fVal;
         memcpy( opValue, &(unNewVar.lVal), SIZE_LONG );
         iLen = SIZE_LONG;
         break;
      case UDT_SHORT:
         unNewVar.shVal = (short)unVar.fVal;
         memcpy( opValue, &(unNewVar.shVal), SIZE_SHORT );
         iLen = SIZE_SHORT;
         break;
      case UDT_DOUBLE:
         unNewVar.dVal = (double)unVar.fVal;
         memcpy( opValue, &(unNewVar.dVal), SIZE_DOUBLE );
         iLen = SIZE_DOUBLE;
         break;
      case UDT_STRING:
         sprintf( opValue, "%f", unVar.fVal );
         iLen = strlen( opValue );
         break;
      default:
         memcpy (opValue, &(unVar.fVal), SIZE_FLOAT);
         iLen = SIZE_FLOAT;
         break;
      }
      break;
   case UDT_DOUBLE:
      switch( iNewType )
      {
      case UDT_INT:
         unNewVar.iVal = (int)unVar.dVal;
         memcpy( opValue, &(unNewVar.iVal), SIZE_INT );
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         unNewVar.lVal = (long)unVar.dVal;
         memcpy( opValue, &(unNewVar.lVal), SIZE_LONG );
         iLen = SIZE_LONG;
         break;
      case UDT_FLOAT:
         unNewVar.fVal = (float)unVar.dVal;
         memcpy( opValue, &(unNewVar.fVal), SIZE_FLOAT );
         iLen = SIZE_FLOAT;
         break;
      case UDT_SHORT:
         unNewVar.shVal = (short)unVar.dVal;
         memcpy( opValue, &(unNewVar.shVal), SIZE_SHORT );
         iLen = SIZE_SHORT;
         break;
      case UDT_STRING:
         sprintf( opValue, "%lf", unVar.dVal );
         iLen = strlen( opValue );
         break;
      default:
         memcpy (opValue, &(unVar.dVal), SIZE_DOUBLE);
         iLen = SIZE_DOUBLE;
         break;
      }
      break;
   case UDT_STRING:
      switch( iNewType )
      {
      case UDT_INT:
         unNewVar.iVal = atoi( unVar.pcVal );
         memcpy( opValue, &(unNewVar.iVal), SIZE_INT );
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         unNewVar.lVal = atol( unVar.pcVal);
         memcpy( opValue, &(unNewVar.lVal), SIZE_LONG );
         iLen = SIZE_LONG;
         break;
      case UDT_FLOAT:
         unNewVar.fVal = atof( unVar.pcVal );
         memcpy( opValue, &(unNewVar.fVal), SIZE_FLOAT );
         iLen = SIZE_FLOAT;
         break;
      case UDT_DOUBLE:
         unNewVar.dVal = atof( unVar.pcVal );
         memcpy( opValue, &(unNewVar.dVal), SIZE_DOUBLE );
         iLen = SIZE_DOUBLE;
         break;
      case UDT_SHORT:
         unNewVar.shVal = atoi( unVar.pcVal );
         memcpy( opValue, &(unNewVar.shVal), SIZE_SHORT );
         iLen = SIZE_SHORT;
         break;
      default:
         memcpy (opValue, unVar.pcVal, eiValLen);
         iLen = eiValLen;
         break;
      }
      break;
   default:
      switch( iNewType )
      {
      case UDT_INT:
         iLen = SIZE_INT;
         break;
      case UDT_LONG:
         iLen = SIZE_LONG;
         break;
      case UDT_FLOAT:
         iLen = SIZE_FLOAT;
         break;
      case UDT_DOUBLE:
         iLen = SIZE_DOUBLE;
         break;
      case UDT_SHORT:
         iLen = SIZE_SHORT;
         break;
      default:
         iLen = eiValLen;
         break;
      }
      memcpy (opValue, unVar.pcVal, iLen);
      break;
   }

   if( pVal != NULL )
   {
      free( pVal );
   }

   if( opiValLen )
   {
      *opiValLen = iLen;
   }
   
   return 0;
}

/*
**����:ȡһ����ָ���ظ�������ֵ
**����:1 UDO�ռ�
**     2 ����
**     3 ���ظ�����
**     4 Ŀ������������
**���:1 ��ֵ
**     2 ֵ����
**����:0-�ɹ�,����-ʧ��
*/
int UDO_SafeGetField( UDP epfAddr, int eiFieldId, int eiFieldOccur,
                     int eiType, char * opValue, int *opiValLen )
{
   UDOPage      *pstPage;
   UDOFieldDesc *pstFldDesc, *pstLastFldDesc;
   UDOValDesc   *pstValDesc;
   int           iTemp, iPos ,i;

   FILE *fp;

   assert( epfAddr != NULL );
   assert( opValue != NULL );

   pstPage = (UDOPage*)epfAddr;
#if 0
   /* һ���ǵ���UDOSafegetfile ʱ��ʹ�õ� */
   fp = fopen("/home/cmp/log/UDOSafegetfile.log","a");

   fprintf(fp,"%d in  %s ���� [%d] \n",__LINE__,__FILE__,  eiFieldId );
   fflush(fp);

  fprintf(fp ,"%d in  %s  iSize[%d], iOffset[%d], iFreeDescPos[%d], iFreeValPos[%d], iQueuePos[%d],  pstNextPage[%s], iFieldNum[%d]\n" ,
  __LINE__,__FILE__,          
  pstPage->iSize ,                     /* ��ҳ��С               */
  pstPage->iOffset,                     /* ��ʼƫ����             */
  pstPage->iFreeDescPos,                 /* ������������ʼλ��     */
  pstPage->iFreeValPos,                  /* ֵ������ʼλ��         */
  pstPage->iQueuePos,                    /* ��ҳ��UDOҳ���е�λ��  */
  pstPage->pstNextPage,             /* ��һUDOҳ��ַ          */
  pstPage->iFieldNum);                    /* UDO���������          */

   pstLastFldDesc = (UDOFieldDesc *)((char*)pstPage + sizeof(UDOPage));
   while( pstLastFldDesc->iFieldId != 10000 )
   {
      fprintf(fp ,"%d in  %s  iType;[%d]iFieldId[%d]iRepeatTime[%d]iValDscPos[%d]lNextField[%d]\n",
                  __LINE__,__FILE__,          
                  pstLastFldDesc->iType,                 /*��ǰ������              */
                  pstLastFldDesc->iFieldId,             /*��ǰ��ֵ���            */
                  pstLastFldDesc->iRepeatTime,           /*��ǰ��ֵ����            */
                  pstLastFldDesc->iValDscPos,           /*��һ��ֵ������λ��λ��  */ 
                  pstLastFldDesc->lNextField            /*��һ��������λ��        */ 
             );
      fflush(fp);

      while( pstLastFldDesc->lNextField > (pstPage->iOffset + pstPage->iSize) )
      {
              fprintf(fp ,"%d in  %s  iSize[%d], iOffset[%d], iFreeDescPos[%d], iFreeValPos[%d], iQueuePos[%d],  pstNextPage[%s], iFieldNum[%d]\n" ,
              __LINE__,__FILE__,          
              pstPage->iSize ,                     /* ��ҳ��С               */
              pstPage->iOffset,                     /* ��ʼƫ����             */
              pstPage->iFreeDescPos,                 /* ������������ʼλ��     */
              pstPage->iFreeValPos,                  /* ֵ������ʼλ��         */
              pstPage->iQueuePos,                    /* ��ҳ��UDOҳ���е�λ��  */
              pstPage->pstNextPage,             /* ��һUDOҳ��ַ          */
              pstPage->iFieldNum);                    /* UDO���������          */

         pstPage = pstPage->pstNextPage;
      }

      if( pstLastFldDesc->lNextField != -1 )
      {
         pstLastFldDesc = (UDOFieldDesc *)((char*)pstPage + pstLastFldDesc->lNextField - pstPage->iOffset);
      }
      else  /* ��ǰ�������һ���� */
      {
         fprintf(fp ,"%d in  %s  iType;[%d]iFieldId[%d]iRepeatTime[%d]iValDscPos[%d]lNextField[%d]\n",
                     __LINE__,__FILE__,          
                     pstLastFldDesc->iType,                 /*��ǰ������              */
                     pstLastFldDesc->iFieldId,             /*��ǰ��ֵ���            */
                     pstLastFldDesc->iRepeatTime,           /*��ǰ��ֵ����            */
                     pstLastFldDesc->iValDscPos,           /*��һ��ֵ������λ��λ��  */ 
                     pstLastFldDesc->lNextField            /*��һ��������λ��        */ 
                );
         fflush(fp);


         break;
      }
   }
   fflush(fp);
   fclose(fp);
#endif

   /* �����жϸ�ֵ�������Ƿ���ڣ����ڷ�����ͷָ�룬���򷵻�NULL        */
   /* ͬʱpstLastFldDescָ�����һ����ָ�룬�����ǰUDO�������򷵻�NULL */
   pstFldDesc = UDO_SeekFieldDesc( &pstPage, eiFieldId, &pstLastFldDesc );
   if( pstFldDesc == NULL )
   {
      return UDO_NO_FIELD;
   }
   
   if( (eiFieldOccur < 0) || eiFieldOccur > (pstFldDesc->iRepeatTime-1) )
   {
      return UDO_INVALID_OCCUR;
   }
   /* �Ѿ��ҵ��������ظ�������ȷ */
   iTemp = 0;
   iPos = pstFldDesc->iValDscPos;
   do
   {
      /* ���ҵ�ǰֵ����������ҳ */
      while( (pstPage->iOffset+pstPage->iSize) < iPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstValDesc = (UDOValDesc*)((char*)pstPage + iPos - pstPage->iOffset);
      if( iTemp == eiFieldOccur )
      {
         break;
      }
      iPos = pstValDesc->lNextValPos;
      iTemp++;
   } while( iPos != -1 );
   /* ����ֵλ������ҳ */
   iPos = pstValDesc->iPos;
   while( (pstPage->iOffset+pstPage->iSize) < iPos )
   {
      pstPage = pstPage->pstNextPage;
   }

   if( opiValLen )
   {
      *opiValLen = pstValDesc->iLen;
   }

   if( pstFldDesc->iType == eiType )
   {
      memcpy( opValue, (char*)pstPage + iPos - pstPage->iOffset, pstValDesc->iLen );
      return 0;
   }

   /* ���洦��Դ���ͺ�Ŀ�����Ͳ�һ�µ���� */
   return UDO_ChgType( (char*)pstPage + iPos - pstPage->iOffset,
                      pstFldDesc->iType, eiType, opValue,
                      pstValDesc->iLen, opiValLen );
}

/*
 * ��UDOת��Ϊ�ַ�����ʽ
 */
int UDO_sprintf (UDP epfAddr, char * opcBuf, int *opiLen )
{
   Var          unVar;
   int          iMalloced, iTemp, iBufPos = 0;
   char         aczDigValue[60], aczStrValue[10000], *pVal;
   int          iFldDescPos, iValDescPos, iPos;
   UDOPage      *pstPage, *pstTmpPage, *pstPrePage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;

   assert( epfAddr != NULL );
   assert( opcBuf  != NULL );

   pstPage = (UDOPage*)epfAddr;
   /* ����Դ UDO ÿ�����ÿ��ֵ */
   sprintf( opcBuf, "%d ", pstPage->iFieldNum );  
   iBufPos = strlen( opcBuf );

   if( UDO_FieldCount( epfAddr ) == 0 )
   {
      *opiLen = iBufPos;
      return 0;
   }

   iFldDescPos = sizeof(UDOPage);
   do
   {
      /* ����������������ҳ */
      while( (pstPage->iOffset + pstPage->iSize) < iFldDescPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstFldDesc = (UDOFieldDesc*)( (char*)pstPage +
                                    iFldDescPos - pstPage->iOffset);
      sprintf( aczStrValue, "%d %d %d ",
               pstFldDesc->iType, pstFldDesc->iFieldId,
               pstFldDesc->iRepeatTime );
      sprintf( opcBuf+iBufPos, "%s", aczStrValue );
      iBufPos += strlen( aczStrValue );
      iFldDescPos = (int)pstFldDesc->lNextField;

      iValDescPos = pstFldDesc->iValDscPos;
      pstPrePage = pstPage;
      do
      {
         /* ����ֵ����������ҳ */
         while( (pstPrePage->iOffset + pstPrePage->iSize) < iValDescPos )
         {
            pstPrePage = pstPrePage->pstNextPage;
         }
         pstTmpPage = pstPrePage;
         pstValDesc = (UDOValDesc*)((char*)pstTmpPage +
                                    iValDescPos - pstTmpPage->iOffset);
         iValDescPos = pstValDesc->lNextValPos;
         iPos = pstValDesc->iPos;

         pstPrePage = pstTmpPage;
         /* ����ֵ����ҳ */
         while( (pstTmpPage->iOffset + pstTmpPage->iSize) < iPos )
         {
            pstTmpPage = pstTmpPage->pstNextPage;
         }

         switch( pstFldDesc->iType )
         {
         case UDT_CHAR:
            unVar.cVal = *((char*)pstTmpPage + iPos - pstTmpPage->iOffset);
            sprintf( aczStrValue, "%d %c", 1, unVar.cVal );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_SHORT:
            memcpy( &(unVar.shVal),
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    SIZE_SHORT );
            sprintf( aczDigValue, "%d", unVar.shVal );
            sprintf( aczStrValue, "%d %s ",strlen(aczDigValue), aczDigValue );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_INT:
            memcpy( &(unVar.iVal),
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    SIZE_INT );
            sprintf( aczDigValue, "%d", unVar.iVal );
            sprintf( aczStrValue, "%d %s ",strlen(aczDigValue), aczDigValue );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_LONG:
            memcpy( &(unVar.lVal),
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    SIZE_LONG );
            sprintf( aczDigValue, "%ld", unVar.lVal );
            sprintf( aczStrValue, "%d %s ",strlen(aczDigValue), aczDigValue );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_FLOAT:
            memcpy( &(unVar.fVal),
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    SIZE_FLOAT );
            sprintf( aczDigValue, "%f", unVar.fVal );
            sprintf( aczStrValue, "%d %s ",strlen(aczDigValue), aczDigValue );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_DOUBLE:
            memcpy( &(unVar.dVal),
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    SIZE_DOUBLE );
            sprintf( aczDigValue, "%lf", unVar.dVal );
            sprintf( aczStrValue, "%d %s ",strlen(aczDigValue), aczDigValue );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            break;
         case UDT_STRING:
            iMalloced = 0;
            sprintf( aczStrValue, "%d ", pstValDesc->iLen );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            memcpy( opcBuf+iBufPos,
                    (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                    pstValDesc->iLen );
            memcpy( opcBuf+iBufPos+pstValDesc->iLen, " ", 1 );
            iBufPos += pstValDesc->iLen + 1;
            break;
         case UDT_BIN:
            sprintf( aczStrValue,"%d ",pstValDesc->iLen*2 );
            sprintf( opcBuf+iBufPos, "%s", aczStrValue );
            iBufPos += strlen(aczStrValue);
            memset( aczStrValue, 0, sizeof(aczStrValue) );
            for( iTemp = 0; iTemp < pstValDesc->iLen; iTemp++ )
            {
               sprintf( aczStrValue, "%s%02X", aczStrValue,
                        *((char*)pstTmpPage + iPos - pstTmpPage->iOffset+iTemp)&0xFF );
            }
            sprintf( opcBuf+iBufPos, "%s ", aczStrValue );
            iBufPos += strlen(aczStrValue) + 1;
            break;
         }
      } while( iValDescPos != -1 );
   } while( iFldDescPos != -1 );
   
   *opiLen = iBufPos;

   return 0;
}

/* ���ַ����лָ�UDO,�벻Ҫʹ��������UDOת�����ַ��� */
int UDO_sscanf (char * epcBuf, int eiLen, UDP opfAddr)
{
   int i, j, k, iFldCnt, iFldOccur, iType, iFieldId, iLen, iMalloced;
   char *pEnd, *pBegin, aczFldVal[10000], *pVal=NULL;
   char aczValBin[10000];
   Var unVar;
   assert( epcBuf  != NULL );
   assert( opfAddr != NULL );
   
   /* ���Ȳ�������� */
   pBegin = epcBuf;
   pEnd = strchr( pBegin, ' ' );
   memcpy( aczFldVal, pBegin, pEnd-pBegin );
   aczFldVal[pEnd-pBegin] = 0;
   iFldCnt = atoi( aczFldVal );

   for( j = 0; j < iFldCnt; j++ )
   {
      pBegin = pEnd + 1;
      /* ����������, id, ��ֵ���� */
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iType = atoi( aczFldVal );

      pBegin = pEnd + 1;
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iFieldId = atoi( aczFldVal );

      pBegin = pEnd + 1;
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iFldOccur = atoi( aczFldVal );

      pBegin = pEnd;
      for( i = 0; i < iFldOccur; i++ )
      {
         pBegin = pEnd + 1;
         memset( aczFldVal, 0x00, sizeof(aczFldVal) );
         /* ��һ��ֵ�ǳ��� */
         pEnd = strchr( pBegin, ' ' );
         memcpy( aczFldVal, pBegin, pEnd-pBegin );
         aczFldVal[pEnd-pBegin] = 0;
         iLen = atoi( aczFldVal );

         pBegin = pEnd + 1;
         switch( iType )
         {
         case UDT_CHAR:
            UDO_AddField( opfAddr, iFieldId, iType, pBegin, 1 );
            pEnd = pBegin + iLen;
            break;
         case UDT_SHORT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.shVal = atoi( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.shVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_INT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.iVal = atoi( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.iVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_LONG:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.lVal = atol( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.lVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_FLOAT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.fVal = atof( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.fVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_DOUBLE:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.dVal = atof( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.dVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_STRING:
            UDO_AddField( opfAddr, iFieldId, iType, pBegin, iLen );
            pEnd = pBegin + iLen;
            break;
         case UDT_BIN:
            iMalloced = 0;
            if( iLen > sizeof(aczFldVal) )
            {
               pVal = (char*)malloc( iLen*2 + 1 );
               iMalloced = 1;
               memset( pVal, 0x00, iLen*2 + 1 );
            }
            else
            {
               pVal = aczFldVal;
               memset( pVal, 0x00, iLen );
            }

            memset( aczValBin,0x00,sizeof(aczValBin) );

            for( k = 0; k < iLen; k++ )
            {
               if( *(pBegin + k) >= '0' && *(pBegin + k)<='9' )
               {
                  *(pVal+k) = *(pBegin + k)-'0';
               }
               else if( *(pBegin + k) >= 'a' && *(pBegin + k)<='f' )
               {
                  *(pVal+k) = *(pBegin + k)-'a'+0x0a;
               }
               else if( *(pBegin + k) >= 'A' && *(pBegin + k)<='F' )
               {
                  *(pVal+k) = *(pBegin + k)-'A'+0x0a;
               }
            }
            for (k=0;k<iLen/2;k++ )
            {
               aczValBin[k] =  (*(pVal+k*2+1)) | ((*(pVal+k*2 ))<<4 );
            }

            UDO_AddField( opfAddr, iFieldId, iType, aczValBin, iLen/2 );
            if( iMalloced == 1 )
            {
               free( pVal );
               pVal = NULL;
            }
            pEnd = pBegin + iLen;
            break;
         }
      }
   }
   
   return 0;
}

#if 0
/* ���ַ����лָ�UDO,�벻Ҫʹ��������UDOת�����ַ��� */
int UDO_sscanf (char * epcBuf, int eiLen, UDP opfAddr)
{
   int i, j, k, iFldCnt, iFldOccur, iType, iFieldId, iLen, iMalloced;
   char *pEnd, *pBegin, aczFldVal[3000], *pVal=NULL;
   char *pValBin=NULL;
   Var unVar;

   assert( epcBuf  != NULL );
   assert( opfAddr != NULL );
   
   /* ���Ȳ�������� */
   pBegin = epcBuf;
   pEnd = strchr( pBegin, ' ' );
   memcpy( aczFldVal, pBegin, pEnd-pBegin );
   aczFldVal[pEnd-pBegin] = 0;
   iFldCnt = atoi( aczFldVal );

   for( j = 0; j < iFldCnt; j++ )
   {
      pBegin = pEnd + 1;
      /* ����������, id, ��ֵ���� */
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iType = atoi( aczFldVal );

      pBegin = pEnd + 1;
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iFieldId = atoi( aczFldVal );

      pBegin = pEnd + 1;
      pEnd = strchr( pBegin, ' ' );
      memcpy( aczFldVal, pBegin, pEnd-pBegin );
      aczFldVal[pEnd-pBegin] = 0;
      iFldOccur = atoi( aczFldVal );

      pBegin = pEnd;
      for( i = 0; i < iFldOccur; i++ )
      {
         pBegin = pEnd + 1;
         memset( aczFldVal, 0x00, sizeof(aczFldVal) );
         /* ��һ��ֵ�ǳ��� */
         pEnd = strchr( pBegin, ' ' );
         memcpy( aczFldVal, pBegin, pEnd-pBegin );
         aczFldVal[pEnd-pBegin] = 0;
         iLen = atoi( aczFldVal );

         pBegin = pEnd + 1;
         switch( iType )
         {
         case UDT_CHAR:
            UDO_AddField( opfAddr, iFieldId, iType, pBegin, 1 );
            pEnd = pBegin + iLen;
            break;
         case UDT_SHORT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.shVal = atoi( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.shVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_INT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.iVal = atoi( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.iVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_LONG:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.lVal = atol( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.lVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_FLOAT:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.fVal = atof( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.fVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_DOUBLE:
            memcpy( aczFldVal, pBegin, iLen );
            unVar.dVal = atof( aczFldVal );
            UDO_AddField( opfAddr, iFieldId, iType, (char*)&(unVar.dVal), 0 );
            pEnd = pBegin + iLen;
            break;
         case UDT_STRING:
            UDO_AddField( opfAddr, iFieldId, iType, pBegin, iLen );
            pEnd = pBegin + iLen;
            break;
         case UDT_BIN:
            iMalloced = 0;
            if( iLen > sizeof(aczFldVal) )
            {
               pVal = (char*)malloc( iLen*2 + 1 );
               iMalloced = 1;
            }
            else
            {
               pVal = aczFldVal;
            }
            pValBin = (char*)malloc( iLen + 1 );

            memset( pValBin,0x00,iLen );

            memset( pVal, 0x00, iLen );
            for( k = 0; k < iLen; k++ )
            {
               if( *(pBegin + k) >= '0' && *(pBegin + k)<='9' )
               {
                  *(pVal+k) = *(pBegin + k)-'0';
               }
               else if( *(pBegin + k) >= 'a' && *(pBegin + k)<='f' )
               {
                  *(pVal+k) = *(pBegin + k)-'a'+0x0a;
               }
               else if( *(pBegin + k) >= 'A' && *(pBegin + k)<='F' )
               {
                  *(pVal+k) = *(pBegin + k)-'A'+0x0a;
               }
            }
            for (k=0;k<iLen/2;k++ )
            {
               *(pValBin+k) =  (*(pVal+k*2+1)) | ((*(pVal+k*2 ))<<4 );
            }

            UDO_AddField( opfAddr, iFieldId, iType, pValBin, iLen/2 );
            free( pValBin );
            pValBin = NULL;
            if( iMalloced == 1 )
            {
               free( pVal );
               pVal = NULL;
            }
            pEnd = pBegin + iLen;
            break;
         }
      }
   }
   
   return 0;
}
#endif


/*
 * ���ܣ� 
 */
UDP UDO_Merge( UDP *opfDest, UDP epfSrc )
{
   int          iFldDescPos, iValDescPos, iPos;
   UDOPage      *pstPage, *pstTmpPage, *pstPrePage, *pstDestPage;
   UDOFieldDesc *pstFldDesc;
   UDOValDesc   *pstValDesc;
   UDP           pfDest;

   assert( opfDest != NULL );
   assert( epfSrc  != NULL );

   /* �����ж�Ŀ�� UDO ��һҳ�ռ��Ƿ񹻴���������������µĿռ� */
   pstDestPage = (UDOPage*)*opfDest;
   if( pstDestPage->iSize < UDO_ValueSize( epfSrc ) )
   {
       iPos = UDO_PageIsInQueue( *opfDest );
       if( iPos > -1 )
       {
          UDO_PageDeQueue( iPos );
          free( (void*)pstDestPage );
       }
       pfDest = UDO_Alloc( UDO_ValueSize( epfSrc ) );
       if( pfDest == NULL )
       {
          return NULL;
       }
       *opfDest = pfDest;
   }
   else
   {
      pfDest = *opfDest;
      UDO_Init( *opfDest, pstDestPage->iSize );
   }

   pstPage = (UDOPage*)epfSrc;
   
   /* ����Դ UDO ÿ�����ÿ��ֵ */  
   iFldDescPos = sizeof(UDOPage);
   do
   {
      /* ����������������ҳ */
      while( (pstPage->iOffset + pstPage->iSize) < iFldDescPos )
      {
         pstPage = pstPage->pstNextPage;
      }
      pstFldDesc = (UDOFieldDesc*)((char*)pstPage + iFldDescPos - pstPage->iOffset);

      iFldDescPos = (int)pstFldDesc->lNextField;

      iValDescPos = pstFldDesc->iValDscPos;
      pstPrePage = pstPage;
      do
      {
         /* ����ֵ����������ҳ */
         while( (pstPrePage->iOffset + pstPrePage->iSize) < iValDescPos )
         {
            pstPrePage = pstPrePage->pstNextPage;
         }
         pstTmpPage = pstPrePage;
         pstValDesc = (UDOValDesc*)((char*)pstTmpPage + iValDescPos - pstTmpPage->iOffset);
         iValDescPos = pstValDesc->lNextValPos;
         iPos = pstValDesc->iPos;

         pstPrePage = pstTmpPage;
         /* ����ֵ����ҳ */
         while( (pstTmpPage->iOffset + pstTmpPage->iSize) < iPos )
         {
            pstTmpPage = pstTmpPage->pstNextPage;
         }
         
         UDO_AddField( pfDest, pstFldDesc->iFieldId, pstFldDesc->iType,
                      (char*)pstTmpPage + iPos - pstTmpPage->iOffset,
                      pstValDesc->iLen );
      } while( iValDescPos != -1 );
   } while( iFldDescPos != -1 );

   return pfDest;
}

int UDO_ACField( UDP epfSrc, int eiFldId, int eiType, char *epValue, int eiLen )
{
   if( UDO_FieldExist( epfSrc, eiFldId ) == 0 )
   {
      return UDO_AddField( epfSrc, eiFldId, eiType, epValue, eiLen );
   }
   else
   {
      return UDO_ChgField( epfSrc, eiFldId, 0, epValue, eiLen );
   }
}

typedef struct {
    int iType;
    int iPri;
} TypePriElt;

static TypePriElt astTypePri[] = {
                              { UDT_OTHER,     0, },
                              { UDT_BIN,       0, },
                              { UDT_STRING,    0, },
                              { UDT_CHAR,      1, },
                              { UDT_SHORT,     2, },
                              { UDT_INT,       3, },
                              { UDT_LONG,      4, },
                              { UDT_FLOAT,     5, },
                              { UDT_DOUBLE,    6, }
                         };

static int iTypePriNum = sizeof (astTypePri) / sizeof (TypePriElt);

int UDO_GetTypePri (int eiType)
{
    int i;
    
    for (i = 0; i < iTypePriNum; i++) {
        if (astTypePri[i].iType == eiType) {
            return (astTypePri[i].iPri);
        }
    }

    return (0);
}
