/*
**  file    : asm_base.c
*/

#include "asm/asm_base.h"
#include "log.h"

int ASM_SearchBySvcIdSubSeq( const void *p1, const void *p2 )
{
   SubSvcReqCfg *pstL = (SubSvcReqCfg *)p1;
   SubSvcReqCfg *pstR = (SubSvcReqCfg *)p2;

   if( pstL->iSvcId != pstR->iSvcId )
   {
      return pstL->iSvcId - pstR->iSvcId;
   }
   else
   {
      return pstL->iSubSeq - pstR->iSubSeq;
   }
}

/*
**  ��������: �����ӷ����������������Ϣ����
**  ������:   ASM_CreateReqCfgAry
**  �������: 1 �����
**            2 �ӷ������
**            3 �Ƿ������ʶ
**  �������: �ӷ����������������Ϣ����ͷ
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
**  ˵����    �����ڲ���̬����ռ䣬ʹ�����Ҫע���ͷ�
*/
int  ASM_CreateReqCfgAry( int eiSvcId, int eiSubSeq, int eiBalFlag,
                          ReqCfgAry * opstReqCfgAry )
{
   int iRow, iTemp3=0;
   SubSvcReqCfg *pstBase=NULL, *pstBegin, *pstTmp, *pstEnd, stKey;
   int iRowNum, iTemp;

   assert( opstReqCfgAry != NULL );

   iRowNum = 0;
   pstBase = (SubSvcReqCfg *)SHM_ReadData ( (void *)pcShmAddr, 
                                            SUBSVCREQCFG, &iRowNum );
   if( pstBase == NULL && iRowNum == -1 )
   {
      LOG4C ((LOG_FATAL,
             "���ӹ����ڴ���SubSvcReqCfg�ĵ�ַʧ��. "));

      return -1;
   }
   else if( pstBase == NULL && iRowNum == 0 )
   {
      LOG4C ((LOG_FATAL, "û������������Ϣ"));
      return -1;
   }

   pstEnd = pstBase + iRowNum;

   stKey.iSvcId = eiSvcId;
   stKey.iSubSeq = eiSubSeq;
   pstBegin = bsearch( &stKey, pstBase, iRowNum, sizeof(stKey),
                       ASM_SearchBySvcIdSubSeq );
   if( pstBegin == NULL )
   {
       LOG4C ((LOG_FATAL,
               "δ���ҵ�������Ϣ."
               "�����=[%d],�ӷ����ڲ����=[%d]",
               eiSvcId, eiSubSeq));

      return ASM_CRT_CFGLIST_ERR;
   }

   /**
     * �ҵ����������ĵ�һ������Ŀ������
     */
   if( pstBase->iSvcId == eiSvcId && pstBase->iSubSeq == eiSubSeq )
   {
      pstBegin = pstBase;
   }
   else
   {
      while( pstBegin != pstBase )
      {
         /* ����ǰһ������Ϣ */
         if( ((pstBegin - 1)->iSvcId == eiSvcId) &&
             ((pstBegin - 1)->iSubSeq == eiSubSeq) )
         {
            pstBegin--;
         }
         else
         {
            break;
         }
      }
   }

   iRow = 0;
   pstTmp = pstBegin;
   while( (pstTmp->iSvcId == eiSvcId) && (pstTmp->iSubSeq == eiSubSeq) &&
          pstTmp < pstEnd )
   {
      if( pstTmp->iBalFlag == eiBalFlag )
      {
         iRow++;
      }
      pstTmp++;
   }
   opstReqCfgAry->iNum = iRow;
   do
   { 
      opstReqCfgAry->pstReqCfgNode = calloc( sizeof(ReqCfgNode), (size_t)iRow );
   }while( opstReqCfgAry->pstReqCfgNode == NULL && iRow != 0 );

   iRow = 0;
   while( (pstBegin->iSvcId == eiSvcId) && (pstBegin->iSubSeq == eiSubSeq) &&
          (pstBegin < pstTmp) )
   {
      if( pstBegin->iBalFlag == eiBalFlag )
      {
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iFldSeq = pstBegin->iFldSeq;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iFldType = pstBegin->iFldType;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iRepFldFlag = 
	                                              pstBegin->iRepFldFlag;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iRepTmSubSeq = 
	                                              pstBegin->iRepTmSubSeq;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iRepTmFld = 
                                                      pstBegin->iRepTmFld;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iRepTmMsgDscrb = 
                                                      pstBegin->iBatSubSeq;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iRepTmType = 
                                                      pstBegin->iRepTmType;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iFldFragCalFlag = 
                                                      pstBegin->iFldFragCalFlag;
         (*((opstReqCfgAry->pstReqCfgNode)+iRow)).iFldAsmId = 
                                                      pstBegin->iFldAsmId;
	 iRow++;
      }
      pstBegin++;
   }

   return ASM_SUCC;
}


/*
**  ��������: �����ӷ���Ŷ�λ��һ������
**  ������:   ASM_GetSrcList
**  �������: ��Դ�ӷ������
**            ������Ϣ����
**            �ӷ�����Ӧ���������־,1-����,2-��Ӧ
**  �������: ��
**  ����ֵ:   �ӷ�����Ϣ����, ʧ�ܷ��ؿ�
*/
UDP ASM_GetSrcList( int eiSrcSubSeq, SVC_Content * epstService,
                    int eiDirectFlag )
{
   SUB_SvcList * pstSubSvcList;

   assert( epstService != NULL );

   for( pstSubSvcList = &(epstService->stSubSvcHead);
        (pstSubSvcList != NULL) && (eiSrcSubSeq != pstSubSvcList->iSubSvcSeq);
        pstSubSvcList = pstSubSvcList->pstNextSubSvc );

   if( pstSubSvcList == NULL )
   {
      return NULL;
   }
   if( eiDirectFlag == 1 )
   {
      if( eiSrcSubSeq != 0 )
      {
         return pstSubSvcList->pfReqList;
      }
      else
      {
         return pstSubSvcList->pfAnswList;
      }
   }
   else
   {
      return pstSubSvcList->pfAnswList;
   }
}


/*
**  ��������: ��ӡ�ӷ����������������Ϣ�������������ݵ���׼���
**  ������:   ASM_ReqCfgAryDisplay
**  �������: �ӷ����������������Ϣ�����׵�ַ
**  �������: ��
**  ����ֵ:   ��
*/
void ASM_ReqCfgAryDisplay( ReqCfgAry *epstReqCfgAry )
{
   int iTemp;
   assert( epstReqCfgAry != NULL );

   fprintf( stdout, "ReqCfgAry_Display: " );
   fprintf( stdout, "\n" );
   for( iTemp=0; iTemp<epstReqCfgAry->iNum; iTemp++ )
   {
      fprintf( stdout, "************************************************\n" );
      fprintf( stdout, "fldseq: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iFldSeq );
      fprintf( stdout, "fldtype: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iFldType );
      fprintf( stdout, "repfldflag: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iRepFldFlag );
      fprintf( stdout, "reptmsubseq: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iRepTmSubSeq );
      fprintf( stdout, "reptmfld: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iRepTmFld );
      fprintf( stdout, "reptmtype: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iRepTmType );
      fprintf( stdout, "fldfragcalflag: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iFldFragCalFlag );
      fprintf( stdout, "fldasmid: %d\n", (*((epstReqCfgAry->pstReqCfgNode)+iTemp)).iFldAsmId );
   }
   fprintf( stdout, "************************************************\n" );
}

int ASM_SearchByAsmId( const void *p1, const void *p2 )
{
   SubSvcFldSrcCfg *pstL = (SubSvcFldSrcCfg *)p1;
   SubSvcFldSrcCfg *pstR = (SubSvcFldSrcCfg *)p2;

   if( pstL->iFldAsmId != pstR->iFldAsmId )
   {
      return pstL->iFldAsmId - pstR->iFldAsmId;
   }
   return pstL->iFldFragSeq - pstR->iFldFragSeq;
}

/*
**  ��������: �����ӷ�������Դ������Ϣ����
**  ������:   ASM_CreateFldSrcCfgAry
**  �������: ������Id��
**  �������: �ӷ�������Դ������Ϣ����ͷ
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_CreateFldSrcCfgAry( int eiFldAsmId, 
                            FldSrcCfgAry * opstFldSrcCfgAry )
{
   SubSvcFldSrcCfg *pstSubSvcFldSrcCfg = NULL, *pstBegin;
   SubSvcFldSrcCfg stKey;
   int iRow, iRowNum, iTemp;

   assert( opstFldSrcCfgAry != NULL );

   /* ���ùؼ�ֵ */
   stKey.iFldAsmId   = eiFldAsmId;
   stKey.iFldFragSeq = 0;

   iRowNum = 0;
   pstSubSvcFldSrcCfg = (SubSvcFldSrcCfg *)SHM_ReadData ( (void *)pcShmAddr,
                                                          SUBSVCFLDSRCCFG,
                                                          &iRowNum );
   if( pstSubSvcFldSrcCfg == NULL )
   {
      LOG4C ((LOG_FATAL, "%s:%d ���ӹ����ڴ���SubSvcFldSrcCfg�ĵ�ַʧ��."));
      return -1;
   }

   pstBegin = (SubSvcFldSrcCfg *)bsearch( &stKey, pstSubSvcFldSrcCfg,
                                          iRowNum, sizeof(stKey),
                                          ASM_SearchByAsmId );
   if( pstBegin == NULL )
   {
       LOG4C ((LOG_FATAL,
               "������Դ����û����subsvcsrcfldcfg�ҵ�.�����=[%d]",
               eiFldAsmId));

      return ASM_CRT_CFGLIST_ERR;
   }

   iRow = 0;
   while( (*(pstBegin + iRow)).iFldAsmId == eiFldAsmId &&
          (pstBegin + iRow) != (pstSubSvcFldSrcCfg + iRowNum ) )
   {
      iRow++;
   }
   opstFldSrcCfgAry->iNum = iRow;
   do
   { 
      opstFldSrcCfgAry->pstFldSrcCfgNode = 
                     calloc( sizeof(FldSrcCfgNode), (size_t)iRow );
   }while( opstFldSrcCfgAry->pstFldSrcCfgNode == NULL );
   /*����������Դ���ü��뵽����������*/
   iTemp = 0;
   while( pstBegin->iFldAsmId == eiFldAsmId && iTemp < iRow )
   {
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iFldFragSeq =
                                                     pstBegin->iFldFragSeq;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcInfType =
                                                     pstBegin->iSrcInfType;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcSubSeq =
                                                     pstBegin->iSrcSubSeq;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcFldSeq =
                                                     pstBegin->iSrcFldSeq;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcRepFldFlag =
                                                     pstBegin->iSrcRepFldFlag;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcBeginPos =
                                                     pstBegin->iSrcBeginPos;
      (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcLength =
                                                     pstBegin->iSrcLength;
      strcpy( (*((opstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).aczSrcNote,
              pstBegin->aczSrcNote );

      iTemp++;
      pstBegin++;
   }

   return ASM_SUCC;
}

/*
**  ��������: ȡ���ظ�����
**  ������:   ASM_GetRepTime
**  �������: 1 ������Ϣ����
**            2 �������������Ϣ����ڵ�
**  �������: �ظ�����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_GetRepTime( SVC_Content * epstService,
                    ReqCfgNode * epstReqCfgNode,
                    int * opiRepTime )
{
   UDP pcInfoList = NULL;
   int        iLength;
   int        iRepTmSubSeq;
   int        iRepTmMsgDscrb;
   int        iRepTmFld;
   int        iRepTmType;
   int        iRet;
   int        iValue;
   long       lValue;
   float      fValue;
   double     dValue;
   short      sValue;
   char       aczValue[MAX_STR_NODE_LEN];
   
   assert( epstService != NULL );
   assert( epstReqCfgNode != NULL );
   assert( opiRepTime != NULL );

   iRepTmSubSeq   = epstReqCfgNode->iRepTmSubSeq;
   iRepTmFld      = epstReqCfgNode->iRepTmFld;
   iRepTmType     = epstReqCfgNode->iRepTmType;
   iRepTmMsgDscrb = epstReqCfgNode->iRepTmMsgDscrb;
   /* �����ֶ���Ϊ�ظ���Դ�������͡�*/

   if( iRepTmSubSeq == -1 )
   {
      *opiRepTime = iRepTmFld;
      return ASM_SUCC;
   }

   /*�ظ�����Ĭ����Դ�������ӷ�����Ӧ��*/
   pcInfoList = ASM_GetSrcList( iRepTmSubSeq, epstService, iRepTmMsgDscrb );
   if( pcInfoList == NULL )
   {
      LOG4C ((LOG_FATAL,
             "��λ�ظ���������Դ�������."
             "�����=[%d] �ڲ��ӷ������=[%d] ���ķ���=[%d]",
             epstService->iSvcId, iRepTmSubSeq, iRepTmMsgDscrb));

      return ASM_GetSrc_FAIL;
   }

   iRet = UDO_SafeGetField (pcInfoList, iRepTmFld, 0, UDT_INT, 
                            (char *)&iValue, &iLength); 
   if( iRet != ASM_SUCC )
   {
       LOG4C ((LOG_FATAL,
             "ȡ�ظ�������ֵ����."
             "�����=[%d] �ڲ��ӷ������=[%d] ���ķ���=[%d] ���=[%d]",
              epstService->iSvcId, iRepTmSubSeq, iRepTmMsgDscrb, iRepTmFld));

       return ASM_FAIL;
   }
   *opiRepTime = iValue;

   return ASM_SUCC;
}

/*
**  ��������: ������Դ��ֵ����
**  ������:   ASM_CreateFldSrcValList
**  �������: 1 ������Ϣ����
**            2 ����Դ������Ϣ����
**            3 �ظ����
**  �������: 1 ��Դ��ֵ����ڵ���
**            2 ��Դ��ֵ����
**  ����ֵ:   int  0 �ɹ�, ����ʧ��
*/
int ASM_CreateFldSrcValList( SVC_Content * epstService,
                             FldSrcCfgAry * epstFldSrcCfgAry,
                             int eiRepSerial,
                             int * opiNum,
                             UDP opcSrcList )
{
   UDP pcInfoList = NULL;

   int        iRet, i, iCount, iDirectFlag, iRow, iTemp;
   int        iFldType;
   int        iValue;
   long       lValue;
   float      fValue;
   double     dValue;
   short      shValue;
   char       aczValue[MAX_STR_NODE_LEN], aczTmpValue[MAX_STR_NODE_LEN];
   int        iLength;

   int        iSrcInfType;      /*��Դ��Ϣ����*/
   int        iSrcSubSeq;       /*��Դ�ӷ����ڲ����*/
   int        iSrcFldSeq;       /*��Դ�ӷ���������*/
   int        iSrcRepFldFlag;   /*��Դ�ظ����ʶ*/
   int        iSrcBeginPos;     /*��ʼλ��*/
   int        iSrcLength;       /*��ʼ����*/
   char       aczSrcNote[41];   /*������Ϣ��Դ*/

   assert( epstService != NULL );
   assert( epstFldSrcCfgAry != NULL );
   assert( opcSrcList != NULL );
   assert( opiNum != NULL );

   iCount = 0;
   *opiNum = 0;
   iRow = epstFldSrcCfgAry->iNum;
   for( iTemp=0; iTemp<iRow; iTemp++ )
   {
      iSrcInfType = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcInfType;
      iSrcSubSeq = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcSubSeq;
      iSrcFldSeq = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcFldSeq;
      iSrcRepFldFlag = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcRepFldFlag;
      iSrcBeginPos = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcBeginPos;
      iSrcLength = (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).iSrcLength;
      strcpy( aczSrcNote,
              (*((epstFldSrcCfgAry->pstFldSrcCfgNode)+iTemp)).aczSrcNote );
      iCount++;
      memset( aczValue, 0x00, MAX_STR_NODE_LEN );
      memset( aczTmpValue, 0x00, MAX_STR_NODE_LEN );
      switch( iSrcInfType )
      {
      case 0:    /*��Դ�������ӷ�����Ӧ*/
      case -1:   /*��Դ�������ӷ���Ĵ�����Ӧ*/
      case -2:
      case -3:
      case -4:
      case -5:
      case -6:
      case -7:
      case -8:
      case -9:
      case -10:
      case -11:
      case -12:
      case -13:
      case -14:
      case 3:    /*��Դ�������ӷ�������*/
         if( iSrcInfType == 0 )
         {
            iDirectFlag = 2; /*��Ӧ*/
         }
         else if( iSrcInfType < 0 )
         {
            iDirectFlag = iSrcInfType; /*������Ӧ*/
         }
         else
         {
            iDirectFlag = 1; /*����*/
         }
         pcInfoList = ASM_GetSrcList( iSrcSubSeq, epstService, iDirectFlag );
         if( pcInfoList == NULL )
         {
            LOG4C ((LOG_FATAL,
                    "��λ����Դ���������."
                    "�����=[%d] �ڲ��ӷ������=[%d] ���ķ���=[%d]",
                    epstService->iSvcId, iSrcSubSeq, iDirectFlag ));

            return ASM_FAIL;
         }
         /* �����Դ�����ظ�����ʹ���ظ����� */
         i = (iSrcRepFldFlag==1)?eiRepSerial:0;
         iFldType = UDO_FieldType( pcInfoList, iSrcFldSeq ); 
         /*LOG4C ((LOG_DEBUG,"ȡ��Դ��[%d]iFldType ",iFldType));*/
         if(iFldType < 0) iFldType = 0; /*Ϊ����Ӧ8583���ݰ���ѡ���ݴ��� */

         memset( aczTmpValue, 0x00, sizeof(aczTmpValue) );
         iRet = UDO_SafeGetField( pcInfoList, iSrcFldSeq, i,
                                 iFldType, (char*)aczTmpValue, &iLength );
         if( iRet != ASM_SUCC )
         {
             LOG4C ((LOG_FATAL,
                    "ȡ��Դ��ֵ����."
                    "�����=[%d] �ڲ��ӷ������=[%d] ���ķ���=[%d] ���=[%d]"
                    " �ظ�����=[%d]",
                 epstService->iSvcId, iSrcSubSeq, iDirectFlag, iSrcFldSeq ,i));
             iLength = 0;
             return ASM_FAIL;
         }

         if (iFldType == UDT_STRING)
         {
            if (iLength <= iSrcBeginPos)
            {
                iLength = 0;
            }
            else
            {
                if (iLength < iSrcBeginPos + iSrcLength)
                {
                    iLength = iLength - iSrcBeginPos;
                }
                else
                {
                    iLength = iSrcLength;
                }
            }
            memset( aczValue, 0x00, sizeof(aczValue) );
            memcpy( aczValue, aczTmpValue+iSrcBeginPos, iLength );
         }
         else
         {
            memcpy( aczValue, aczTmpValue, iLength);
         }
         iRet = UDO_AddField( opcSrcList, iCount, iFldType, 
                              aczValue, iLength );
         if( iRet != ASM_SUCC )
         {
             LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
             return ASM_FAIL;
         }
         break;
      case 1:    /*��Դ�ڳ���*/
         memset( aczValue, 0x00, sizeof(aczValue) );
         memcpy( aczValue, aczSrcNote+iSrcBeginPos, (size_t)iSrcLength );
         iRet = UDO_AddField( opcSrcList, iCount, UDT_STRING, 
                             aczValue, (int)strlen( aczValue ) );
         if( iRet != ASM_SUCC )
         {
            LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
            return ASM_FAIL;
         }
         break;
      case 2:    /*��Դ��ϵͳʱ��*/
         TOOL_GetSystemYYYYMMDDhhmmss( aczValue );
         iRet = UDO_AddField( opcSrcList, iCount, UDT_STRING, 
                             aczValue, (int)strlen( aczValue ) );
         if( iRet != ASM_SUCC )
         {
            LOG4C ((LOG_FATAL, "�����������ӽڵ�ʧ��"));
            return ASM_FAIL;
         }
         break;
      default:
         LOG4C ((LOG_FATAL,
                "�������Դ������."
                "�����=[%d] �ڲ��ӷ������=[%d] ���ķ���=[%d] ���=[%d]"
                " ����=[%d]" ,
                 epstService->iSvcId, iSrcSubSeq, iDirectFlag, iSrcFldSeq ,
                 iSrcInfType));

         return ASM_FAIL;
      }
   }
   *opiNum = iCount;
   return ASM_SUCC;
}

/*�ú����ɽ����ṩ,��ȷ����ȷ��*/
int KhZh(char *zqwdh,char *ykh,char *xkh,char *type,int mode)
{
   FILE *fp;
   char FileName[128],tmpbuf[128],tmpkh[24];
   char JGBM[10],CTL[30],khgjz[10];
   int offset=0,ret=0;;
   
   strcpy(type,"HS");
   strcpy(khgjz,"436742");
   memset(FileName,0,sizeof(FileName));
   memset(tmpbuf,0,sizeof(tmpbuf));
   sprintf(tmpkh,"%23s"," ");

   sprintf(FileName,"%s/etc/ZQWDTYPE",getenv("MYROOT"));
   fp=fopen(FileName,"r");
   while(fp!=NULL && fgets(tmpbuf,sizeof(tmpbuf),fp)!=NULL)
   {
      if(tmpbuf[0]!='#' && tmpbuf[0]!='\n')
      {
         sscanf(tmpbuf,"%s%s%d", JGBM,CTL,&offset);
         if(!strcmp(zqwdh,JGBM))
         {
            strcpy(type,CTL);
            fclose(fp);
            if(offset!=0)
            {
               ret=1;
            }
            goto   out;
         }
      }
   }
   fclose(fp);

out:
   strcpy(tmpkh,ykh);

   if( mode == 0 )
   {       /*** YHkh(436742) to ZQkh(36742) ****/
      if( !strncmp(ykh,"436742" , 6 )  )
      {
         strcpy(tmpkh,ykh+offset);
      }
   }
   else
   {            /*** ZQkh(36742) to YHkh(436742) ***/
      if(!strncmp(ykh,khgjz+offset,(size_t)(6-offset)) )
      {
         strncpy(tmpkh,"436742", (size_t)offset);
         tmpkh[offset]=0;
         strcat(tmpkh,ykh);
      }
   }
   strcpy(xkh,tmpkh);

   return(ret) ;
}

/*
**����:���ݷ�����õ����񷵻���
**����:1 ����������ڵ���
**     2 ������ֵ(���ַ�����ʽ�ṩ,�ɵ������Լ�����ת��)
**���:1 ���񷵻���
**����   0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/
int ASM_GetCodeByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetCode )
{
	#if 0
   int iTemp, iRowNum;
   RetCodeMap *pstRetCodeMap = NULL;

   assert( opczChanRetCode != NULL );

   iRowNum = 0;
   pstRetCodeMap = (RetCodeMap *)SHM_ReadData ( (void *)pcShmAddr, 
                                                RETCODEMAP, &iRowNum );
   if( (pstRetCodeMap == NULL)&&(iRowNum == -1) )
   {
      LOG4C ((LOG_FATAL, "���ӹ����ڴ���RetCodeMap�ĵ�ַʧ��. "));
      return -1;
   }

   for( iTemp = 0; iTemp < iRowNum; iTemp++ )
   {
      PUB_StrTrim( (*(pstRetCodeMap+iTemp)).aczChanRetCode, ' ' );
      if( ( (*(pstRetCodeMap+iTemp)).iChanNodeId == eiChanNodeId ) &&
          ( (*(pstRetCodeMap+iTemp)).iELRetCode  == eiSubRet ) )
      {
         PUB_StrTrim( (*(pstRetCodeMap+iTemp)).aczChanRetCode, ' ' );
         strcpy( opczChanRetCode, (*(pstRetCodeMap+iTemp)).aczChanRetCode );
         return 0;
      }
   } 

   strcpy( opczChanRetCode, "UDF" );
#endif

   return 1;
}

/*
**����:����CMP������õ����񷵻���Ϣ
**����:1 ����������ڵ���
**     2 CMP��������Ϣ(���ַ�����ʽ�ṩ,�ɵ������Լ�����ת��)
**���:1 ���񷵻���
**����   0-�ɹ�,1-û�ж��������ڵ�������ڵ㷵�����Ӧ
*/
int ASM_GetMesgByRet( int eiChanNodeId, int eiSubRet, char *opczChanRetMesg )
{
	#if 0
   int iRowNum, iTemp;
   RetCodeMap *pstRetCodeMap = NULL;

   assert( opczChanRetMesg != NULL );

   iRowNum = 0;
   pstRetCodeMap = (RetCodeMap *)SHM_ReadData ( (void *)pcShmAddr, 
                                                RETCODEMAP, &iRowNum );
   if( (pstRetCodeMap == NULL)&&(iRowNum == -1) )
   {
      LOG4C ((LOG_FATAL, "���ӹ����ڴ���RetCodeMap�ĵ�ַʧ��."));
      return -1;
   }
   for( iTemp = 0; iTemp < iRowNum; iTemp++ )
   {
      PUB_StrTrim( (*(pstRetCodeMap+iTemp)).aczChanRetCode, ' ' );
      if( ( (*(pstRetCodeMap+iTemp)).iChanNodeId == eiChanNodeId ) &&
          ( (*(pstRetCodeMap+iTemp)).iELRetCode  == eiSubRet ) )
      {
         PUB_StrTrim( (*(pstRetCodeMap+iTemp)).aczErrorMessage, ' ' );
         strcpy( opczChanRetMesg, (*(pstRetCodeMap+iTemp)).aczErrorMessage );
         return 0;
      }
   } 

   strcpy( opczChanRetMesg, "UDF" );

#endif
   return 1;
}

/*
**����:������������,������ȷ��SQL���
**����:1 ��������
**���:1 SQL���
**����:0-��ȷ,��������
*/
int ASM_CreateSql( UDP epcSrcList, char *opczSqlStr )
{
   int  iVal, iFldNum, iCondNum, iRet;
   short shVal;
   float fVal;
   long lVal;
   double dVal;
   char *pc1, *pc2, aczFldVal[200], aczFldCond[100];
   char aczCondSeg[100];

   assert( epcSrcList    != NULL );

   iFldNum = UDO_FieldCount( epcSrcList );
   if( iFldNum < 3 )
   {
      LOG4C ((LOG_FATAL, "�������õ���Ϣ����"));
      return ASM_FAIL;
   }

   memset( aczFldVal, 0, 200 );
   UDO_GetField( epcSrcList, 2, 0, aczFldVal, 0 );
   sprintf( opczSqlStr, "select %s from ", aczFldVal );
   memset( aczFldVal, 0, 200 );
   UDO_GetField( epcSrcList, 1, 0, aczFldVal, 0 );
   sprintf( opczSqlStr, "%s%s where ", opczSqlStr, aczFldVal );
   memset( aczFldVal, 0, 200 );
   UDO_GetField( epcSrcList, 3, 0, aczFldVal, 0 );
   pc1 = aczFldVal;
   iCondNum = 0;
   do
   {
      pc2 = strchr( pc1, '%' );
      if( pc2 != NULL )
      {
         memset( aczCondSeg, 0, 100 );
         memcpy( aczCondSeg, pc1, (size_t)(pc2 - pc1) );
         iCondNum++;
         iRet = UDO_FieldType( epcSrcList, 3 + iCondNum );
         if( iRet < 0 )
         {
            LOG4C ((LOG_FATAL, "��������ָ������������"));
            return ASM_SQL_FAIL;
         }
         pc2++;
         switch( iRet )
         {
         case UDT_SHORT:
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, (char*)&shVal, 0 );
            switch( *pc2 )
            {
            case 's':   /*�ַ�������*/
            case 'S':
            case 'f':   /*�����ȷ���������*/
            case 'F':
            case 'd':   /*��������,����short,int��long*/
            case 'D':
            case 'l':   /*˫��������*/
            case 'L':
               sprintf( opczSqlStr, "%s%s%d", opczSqlStr, aczCondSeg, shVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
            break;
         case UDT_INT:
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, (char*)&iVal, 0 );
            switch( *pc2 )
            {
            case 's':   /*�ַ�������*/
            case 'S':
            case 'f':   /*�����ȷ���������*/
            case 'F':
            case 'd':   /*��������,����short,int��long*/
            case 'D':
            case 'l':   /*˫��������*/
            case 'L':
               sprintf( opczSqlStr, "%s%s%d", opczSqlStr, aczCondSeg, iVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
            break;
         case UDT_LONG:
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, (char*)&lVal, 0 );
            switch( *pc2 )
            {
            case 's':   /*�ַ�������*/
            case 'S':
            case 'f':   /*�����ȷ���������*/
            case 'F':
            case 'd':   /*��������,����short,int��long*/
            case 'D':
            case 'l':   /*˫��������*/
            case 'L':
               sprintf( opczSqlStr, "%s%s%ld", opczSqlStr, aczCondSeg, lVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
            break;
         case UDT_DOUBLE:
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, (char*)&dVal, 0 );
            switch( *pc2 )
            {
            case 'f':   /*�����ȷ���������*/
            case 'F':
               sprintf( opczSqlStr, "%s%s%f", opczSqlStr,aczCondSeg,(float)dVal);
               break;
            case 'd':   /*��������,����short,int��long*/
            case 'D':
               sprintf( opczSqlStr, "%s%s%d", opczSqlStr, aczCondSeg, (int)dVal);
               break;
            case 'l':   /*˫��������*/
            case 'L':
            case 's':   /*�ַ�������*/
            case 'S':
               sprintf( opczSqlStr, "%s%s%lf", opczSqlStr, aczCondSeg, dVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
            break;
         case UDT_FLOAT:
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, (char*)&fVal, 0 );
            switch( *pc2 )
            {
            case 'd':   /*��������,����short,int��long*/
            case 'D':
               sprintf( opczSqlStr, "%s%s%d", opczSqlStr, aczCondSeg, (int)fVal);
               break;
            case 's':   /*�ַ�������*/
            case 'S':
            case 'f':   /*�����ȷ���������*/
            case 'F':
            case 'l':   /*˫��������*/
            case 'L':
               sprintf( opczSqlStr, "%s%s%f", opczSqlStr, aczCondSeg, fVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
            break;
         default:
            memset( aczFldCond, 0, 100 );
            UDO_GetField( epcSrcList, 3 + iCondNum, 0, aczFldCond, 0 );
            switch( *pc2 )
            {
            case 's':   /*�ַ�������*/
            case 'S':
               sprintf( opczSqlStr, "%s%s%s",opczSqlStr,aczCondSeg, aczFldCond );
               break;
            case 'd':   /*��������,����short,int��long*/
            case 'D':
               iVal = atoi( aczFldCond );
               sprintf( opczSqlStr, "%s%s%d", opczSqlStr, aczCondSeg, iVal );
               break;
            case 'f':   /*�����ȷ���������*/
            case 'F':
               fVal = (float)atof( aczFldCond );
               sprintf( opczSqlStr, "%s%s%f", opczSqlStr, aczCondSeg, fVal );
               break;
            case 'l':   /*˫��������*/
            case 'L':
               dVal = atof( aczFldCond );
               sprintf( opczSqlStr, "%s%s%lf", opczSqlStr, aczCondSeg, dVal );
               break;
            default:
               LOG4C ((LOG_FATAL, "��������ָ��������ָʾ������"));
               return ASM_SQL_FAIL;
            } 
         }
         pc1 = pc2 + 1;
      }
      else
      {
         sprintf( opczSqlStr, "%s%s", opczSqlStr, pc1 );
      }
   }while( pc2 != NULL );
   
   return ASM_SUCC;
}

/*
**����: ȡ����������Ӧ������,����֧��UDT_INT,UDT_SHORT,UDT_LONG,UDT_STRING,UDT_FLOAT         ��UDT_DOUBLE
**����: 1 ��������
**���:   ��
**����    ������
*/
int ASM_GetFieldType( char *epczFldDesc )
{
   char *pc1, *pc2;
   char aczType[10];

   pc1 = epczFldDesc;
   pc2 = strstr( pc1, "SQL" );
   pc1 = strchr( pc2, ';' );
   memset( aczType, 0, 10 );
   memcpy( aczType, pc2 + 3, (size_t)(pc1 - pc2) - 3 );
   if( strcmp( aczType, "INT" ) == 0 )
   {
      return UDT_INT;
   }
   if( strcmp( aczType, "SMALLINT" ) == 0 )
   {
      return UDT_SHORT;
   }
   if( strcmp( aczType, "FLOAT" ) == 0 )
   {
      return UDT_DOUBLE;
   }
   if( strcmp( aczType, "SMALLFLOAT" ) == 0 )
   {
      return UDT_FLOAT;
   }
   else
   {
      return UDT_STRING;
   }
}

/* 
** end of file: asm_base.c 
*/

