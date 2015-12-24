#include "asm/asm_interface.h"
#include "log.h"

int ASM_RunFunc( int iFldFragCalFlag, SVC_Content *epstService, UDP pcSrcList,
		 int iMsgAppId, int iFldSeq, UDP pcResult )
{
   int iMaxID, iRet;

	int i=0;
	
   iMaxID=sizeof( astAsmEntity )/sizeof( OperaFunEntity )-1;
  
   /*�ж����麯�����Ƿ�Ϸ�*/
   if( ( iFldFragCalFlag < 0 ) || ( iFldFragCalFlag > iMaxID ) )
   {
      LOG4C ((LOG_FATAL, "�������麯���� = [%d] ERROR, not exist maxId[%d]!",
                 iFldFragCalFlag, iMaxID ));
      sprintf( epstService->aczErrorMessage,
               "�ڲ��ӷ���%d���õı������麯����%d����ȷ",
               epstService->iSvcId, iFldFragCalFlag );
      epstService->iErrorType = -1;
      return( ASM_FAIL );
   }	  
//   LOG4C ((LOG_DEBUG, "ASM_RunFunc [%s]", astAsmEntity[iFldFragCalFlag].OperaName));
   
   iRet = (*astAsmEntity[iFldFragCalFlag].OperaFun)( epstService,
                                                     pcSrcList,
                                                     iMsgAppId,
                                                     iFldSeq,
                                                     pcResult );

   if( iRet != ASM_SUCC )
   {
      LOG4C ((LOG_FATAL, " ����ִ�д���"));
      if( strlen(epstService->aczErrorMessage) == 0 )
      {
         sprintf( epstService->aczErrorMessage,
                  "����%d�ڲ��ӷ���%d�������麯��ִ�д���",
                  epstService->iSvcId, epstService->pstSubSvcCurr->iSubSvcSeq );
      }

      return ASM_FAIL;
   }

   return 0;
}

