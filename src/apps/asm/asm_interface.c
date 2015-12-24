#include "asm/asm_interface.h"
#include "log.h"

int ASM_RunFunc( int iFldFragCalFlag, SVC_Content *epstService, UDP pcSrcList,
		 int iMsgAppId, int iFldSeq, UDP pcResult )
{
   int iMaxID, iRet;

	int i=0;
	
   iMaxID=sizeof( astAsmEntity )/sizeof( OperaFunEntity )-1;
  
   /*判断重组函数号是否合法*/
   if( ( iFldFragCalFlag < 0 ) || ( iFldFragCalFlag > iMaxID ) )
   {
      LOG4C ((LOG_FATAL, "报文重组函数号 = [%d] ERROR, not exist maxId[%d]!",
                 iFldFragCalFlag, iMaxID ));
      sprintf( epstService->aczErrorMessage,
               "内部子服务%d配置的报文重组函数号%d不正确",
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
      LOG4C ((LOG_FATAL, " 函数执行错误"));
      if( strlen(epstService->aczErrorMessage) == 0 )
      {
         sprintf( epstService->aczErrorMessage,
                  "服务%d内部子服务%d报文重组函数执行错误",
                  epstService->iSvcId, epstService->pstSubSvcCurr->iSubSvcSeq );
      }

      return ASM_FAIL;
   }

   return 0;
}

