/*
**  file: pkg_base.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>

#include "pkg/pkg_base.h"
#include "shm_base.h"

#include "getAlignPos.h"
#include "tempadd.h"
#include "log.h"

extern int structStartMoveLen;
extern int posInStruct;

extern int alignStructNums[MAX_NESTED][MAX_STRUCT_PER_NESTED];
extern int structNums[MAX_NESTED];

extern FILE *fpPkgLog;

/****************测试***************************/
void TPL_DisplayFldCfgValNode( FldCfgValNode estFldCfgValNode )
{
   LOG4C ((LOG_DEBUG, "iMsgAppId:[%d]\n", estFldCfgValNode.iMsgAppId ));
   LOG4C ((LOG_DEBUG, "iFldSeq:[%d]\n", estFldCfgValNode.iFldSeq ));
   LOG4C ((LOG_DEBUG, "iSepFldSeq:[%d]\n", estFldCfgValNode.iSepFldSeq ));
   LOG4C ((LOG_DEBUG, "aczFldEngName:[%s]\n", estFldCfgValNode.aczFldEngName ));
   LOG4C ((LOG_DEBUG, "aczFldChnName:[%s]\n", estFldCfgValNode.aczFldChnName ));
   LOG4C ((LOG_DEBUG, "iSepFldFlag:[%d]\n", estFldCfgValNode.iSepFldFlag ));
   LOG4C ((LOG_DEBUG, "iLenFldLen:[%d]\n", estFldCfgValNode.iLenFldLen ));
   LOG4C ((LOG_DEBUG, "iFldLen:[%d]\n", estFldCfgValNode.iFldLen ));
   LOG4C ((LOG_DEBUG, "iLenFldType:[%d]\n", estFldCfgValNode.iLenFldType ));
   LOG4C ((LOG_DEBUG, "iFldType:[%d]\n", estFldCfgValNode.iFldType ));
   LOG4C ((LOG_DEBUG, "aczLenFldDef:[%s]\n", estFldCfgValNode.aczLenFldDef ));
   LOG4C ((LOG_DEBUG, "aczFldDef:[%s]\n", estFldCfgValNode.aczFldDef ));
   LOG4C ((LOG_DEBUG, "aczLenFldAlign:[%s]\n", estFldCfgValNode.aczLenFldAlign ));
   LOG4C ((LOG_DEBUG, "aczFldAlign:[%s]\n", estFldCfgValNode.aczFldAlign ));
   LOG4C ((LOG_DEBUG, "aczSepSymbol:[%s]\n", estFldCfgValNode.aczSepSymbol ));
}

int PKG_CfgCompare( const void *pstNode1, const void *pstNode2 )
{
   if( ((MsgFldCfg *)pstNode1)->iMsgAppId ==((MsgFldCfg *)pstNode2)->iMsgAppId )
   {
      return ((MsgFldCfg *)pstNode1)->iFldSeq-((MsgFldCfg *)pstNode2)->iFldSeq;
   }
   else
   {
      return ((MsgFldCfg *)pstNode1)->iMsgAppId - ((MsgFldCfg *)pstNode2)->iMsgAppId;
   }
}


/*
**  函数功能: 查找某一域在于空间内的起始地址和分域数量
**
**  函数名:   PKG_GetInfoByFldSeq
**
**  输入参数: MsgFldCfgAry * epstMsgFldCfgAry  域空间起始地址及域空间内记录数
**            int iFldSeq   : 域编号
**
**  输出参数: MsgFldCfgAry * opstMsgFldCfgAry:域编号为 
**            iFldSeq的域空间起始地址及分域数量
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_GetInfoByFldSeq( MsgFldCfgAry * epstMsgFldCfgAry, 
                          int eiMsgAppId, int eiFldSeq,
			  MsgFldCfgAry * opstMsgFldCfgAry )
{
   int iRowNum, iTemp = 0;
   MsgFldCfg stMsgFldCfg, *pstTemp, *pstMsgFldCfgBegin;

   assert( epstMsgFldCfgAry != NULL );
   pstTemp = NULL ;
   iRowNum = 0;

   /*根据数据字典编号和字段序号进行折半查找*/
   memset( &stMsgFldCfg , 0x00 , sizeof( MsgFldCfg ));
   stMsgFldCfg.iFldSeq   = eiFldSeq;
   stMsgFldCfg.iMsgAppId   = eiMsgAppId;

   pstMsgFldCfgBegin = epstMsgFldCfgAry->pstMsgFldCfg;
   for( iTemp = 0; iTemp < epstMsgFldCfgAry->iNum; iTemp++ )
   {
      if( pstMsgFldCfgBegin->iFldSeq == eiFldSeq )
      {
         break;
      }
      pstMsgFldCfgBegin++;
   }
   if( iTemp == epstMsgFldCfgAry->iNum )
   {
      LOG4C ((LOG_FATAL, 
                  " 数据字典%d域%d在数据字典中未定义",
                  eiMsgAppId, eiFldSeq));
      return PKG_FAIL;
   }

   opstMsgFldCfgAry->iNum = 1;
   opstMsgFldCfgAry->pstMsgFldCfg = pstMsgFldCfgBegin;

#if 0
   /** 计算分域数量 **/
   iTemp = 0;
   pstTemp = pstMsgFldCfgBegin;
   do
   {
      iTemp++;
      pstTemp++;
   } while  ( pstTemp->iFldSeq == eiFldSeq ) ;
   opstMsgFldCfgAry->iNum = iTemp;
   opstMsgFldCfgAry->pstMsgFldCfg = pstMsgFldCfgBegin;

   if( ( opstMsgFldCfgAry->iNum == 0 ) || ( opstMsgFldCfgAry = NULL ) )
   {
      LOG4C ((LOG_FATAL, "There Is No Fetch Data From 'MsgFldCfg',FldSeq:"
                "[%d]\n", eiFldSeq)); 
      return PKG_MSGFLDCFG_NO_DATA;  
   }
#endif

   return PKG_SUCC;
}

int  PKG_GetInfoByFldName( MsgFldCfgAry * epstMsgFldCfgAry, 
                           char * epczFldName,
			   MsgFldCfgAry * opstMsgFldCfgAry )
{
   int iRowNum, iTemp = 0;
   
   MsgFldCfg * pstMsgFldCfgBegin;
   assert( epstMsgFldCfgAry != NULL );

   pstMsgFldCfgBegin = epstMsgFldCfgAry->pstMsgFldCfg;
   for( iTemp = 0; iTemp < epstMsgFldCfgAry->iNum; iTemp++ )
   {
      if( strcmp(pstMsgFldCfgBegin->aczFldEngName, epczFldName) == 0 )
      {
         break;
      }
      pstMsgFldCfgBegin++;
   }

   if( iTemp == epstMsgFldCfgAry->iNum )
   {
      LOG4C ((LOG_FATAL, 
                  " 数据字典域%s在数据字典中未定义",
                  epczFldName));
      return PKG_FAIL;
   }

   opstMsgFldCfgAry->iNum = 1;
   opstMsgFldCfgAry->pstMsgFldCfg = pstMsgFldCfgBegin;

   return PKG_SUCC;
}

/*
**  函数功能: 根据数据字典编号,读表msgfldcfg,得到所有域段配置信息链表
**
**  函数名:   PKG_GetInfoFromMsgFldCfg
**
**  输入参数: int eiMsgAppId
**
**  输出参数: MsgFldCfgAry * opstMsgFldCfgAry
**
**  返回值:   int  0 成功, 其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
*/

int  PKG_GetInfoFromMsgFldCfg( int eiMsgAppId, 
                               MsgFldCfgAry * opstMsgFldCfgAry )
{
   int iRowNum;
   MsgFldCfg *pstTemp, *pstMsgFldCfg;
   int iTop, iBottom, iTemp, iActRowNum = 0, iSize ;


   /*首先设置TOP和BOTTOM,为2分查找准备数据*/

   iRowNum = 0;
   pstTemp = NULL ;
   pstMsgFldCfg = NULL ;
   pstMsgFldCfg = ( MsgFldCfg *)SHM_ReadData( (void *)pcShmAddr, 
                                              MSGFLDCFG, &iRowNum );
   if ( ( pstMsgFldCfg == NULL ) && ( iRowNum == -1 ) )
   {
      LOG4C ((LOG_FATAL,
                " SHM_ReadData(in PKG_GetInfoFromMsgFldCfg) 从共享内存读取数据失败 eiMsgAppId:[%d] pcShmAddr:[%x]\n",
                 eiMsgAppId, pcShmAddr));
      return PKG_FAIL;
   }

   if ( ( pstMsgFldCfg == NULL ) && ( iRowNum == 0 ) )
   {
      LOG4C ((LOG_FATAL,
                " SHM_ReadData(in PKG_GetInfoFromMsgFldCfg) 数据字典:[%d]在共享内存中无配置信息 pcShmAddr:[%x]\n",
               eiMsgAppId, pcShmAddr));
      return PKG_FAIL;
   }

   /*首先设置TOP和BOTTOM,为2分查找准备数据*/
   iTop = iRowNum - 1;
   iBottom = 0;
   iTemp = iTop/2 + iTop%2;

   while( iBottom <= iTop )
   {
      if( (*(pstMsgFldCfg + iTemp)).iMsgAppId == eiMsgAppId )
      {
         break;
      }
      else if( (*(pstMsgFldCfg + iTemp)).iMsgAppId < eiMsgAppId )
      {
         iBottom = iTemp + 1;
         iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
      }
      else
      {
         iTop = iTemp - 1;
         iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
      }
   }
   
   if( iBottom > iTop )
   {
      LOG4C ((LOG_FATAL, 
                  " 数据字典%d没有配置", eiMsgAppId));
      return PKG_MSGFLDCFG_NO_DATA;
   }
   while( iTemp > 0 )
   {
      if( iTemp == 0 )
      {
         break;
      }
      if( (*(pstMsgFldCfg + iTemp - 1)).iMsgAppId != eiMsgAppId )
      {
         break;
      }
      else
      {
         iTop = iTemp - 1; 
         iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
         while( (*(pstMsgFldCfg + iTemp )).iMsgAppId < eiMsgAppId )
         {
            iBottom = iTemp + 1;
            iTemp = iBottom + (iTop - iBottom)/2 + (iTop - iBottom)%2;
         }
      }
   }

   pstTemp = pstMsgFldCfg + iTemp ;
   do
   {
      /* 计算eiMsgAppId号报文的域配置信息总行数 */
      iActRowNum++;  
      iTemp++;
   }while( (*(pstMsgFldCfg + iTemp)).iMsgAppId == eiMsgAppId &&
           iTemp < iRowNum );

   iSize = iActRowNum * sizeof( MsgFldCfg );
   do
   {
      opstMsgFldCfgAry->pstMsgFldCfg = ( MsgFldCfg *)malloc( (size_t)iSize );
   } while ( opstMsgFldCfgAry->pstMsgFldCfg == NULL ) ;

   iTemp = 0;
   do
   {
      memcpy ( opstMsgFldCfgAry->pstMsgFldCfg + iTemp, pstTemp + iTemp ,
               sizeof ( MsgFldCfg ));
      iTemp++;
   } while ( iTemp < iActRowNum );

   opstMsgFldCfgAry->iNum = iActRowNum ;

   if( iActRowNum == 0 )
   {
      LOG4C ((LOG_FATAL,
                  "There Is No Fetch Data From 'MsgFldCfg', MsgAppId:[%d]\n",
                  eiMsgAppId)); 
      return PKG_MSGFLDCFG_NO_DATA;  
   }

   return PKG_SUCC;
}

int MsgSepFld_CfgCompare( const void *pstNode1, const void *pstNode2 )
{
   if( ((MsgFldCfg *)pstNode1)->iMsgAppId==((MsgFldCfg *)pstNode2)->iMsgAppId )
   {
      if( ((MsgFldCfg *)pstNode1)->iFldSeq == ((MsgFldCfg *)pstNode2)->iFldSeq )
      {
         return ((MsgFldCfg *)pstNode1)->iSepFldSeq - ((MsgFldCfg *)pstNode2)->iSepFldSeq;
      }
      else
      {
         return ((MsgFldCfg *)pstNode1)->iFldSeq - ((MsgFldCfg *)pstNode2)->iFldSeq;
      }
   }
   else
   {
      return ((MsgFldCfg*)pstNode1)->iMsgAppId - ((MsgFldCfg*)pstNode2)->iMsgAppId;
   }
}

/*
**  函数功能: 从域空间中读取第eiFldSeq号域的第eiSepFldSeq分域的配置信息
**
**  函数名: PKG_GetFldCfgValNode
**
**  输入参数: MsgFldCfgAry * epstMsgFldCfgAry 
**            int eiMsgId
**            int eiFldSeq
**            int eiSepFldSeq
**  输出参数: FldCfgValNode * opstFldCfgValNode 
**
**  返回值: 0正确 其他错误
*/
int  PKG_GetFldCfgValNode( MsgFldCfgAry * epstMsgFldCfgAry ,
                           int eiMsgAppId,
                           int eiFldSeq,
                           int eiSepFldSeq, MsgFldCfg *opstMsgFldCfg )
{
   MsgFldCfg *pstMsgFldCfg , *pstTemp, stMsgFldCfg ;

   assert( epstMsgFldCfgAry != NULL );
   assert( opstMsgFldCfg != NULL );

   memset ( opstMsgFldCfg , 0x00 , sizeof ( MsgFldCfg ) );
   pstMsgFldCfg = NULL ;

   /*根据数据字典编号和字段序号进行折半查找*/
   memset( &stMsgFldCfg , 0x00 , sizeof( MsgFldCfg ));
   stMsgFldCfg.iMsgAppId   = eiMsgAppId;
   stMsgFldCfg.iFldSeq   = eiFldSeq;
   stMsgFldCfg.iSepFldSeq = eiSepFldSeq;

   pstTemp = bsearch( (void *)&stMsgFldCfg,
                      epstMsgFldCfgAry->pstMsgFldCfg,
                      (size_t)epstMsgFldCfgAry->iNum,
                      sizeof( MsgFldCfg ),
                      MsgSepFld_CfgCompare );
   if ( opstMsgFldCfg == NULL )
   {
      LOG4C ((LOG_FATAL, "PKG_GetFldCfgValNode Error, The Input SepFldSeq "
                "Is[%d]\n",  eiSepFldSeq));
     return PKG_FAIL;
   }

   memcpy( opstMsgFldCfg, pstTemp, sizeof( MsgFldCfg ) );

   return PKG_SUCC;
}



/*
**  函数功能: 根据节点号,读表nodetrancfg, 取出一条记录
**
**  函数名称: PKG_GetInfoFromNodeTranCfg
**
**  输入参数: int eiNodeId
**
**  输出参数: NodeTranCfg * opstNodeTranCfg
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_GetInfoFromNodeTranCfg( int eiNodeId, NodeTranCfg * opstNodeTranCfg )
{
   int iTime, iRowNum;
   NodeTranCfg *pstNodeTranCfg;
   
   assert( opstNodeTranCfg != NULL );

   iRowNum=0;
   pstNodeTranCfg = NULL;
   pstNodeTranCfg = (NodeTranCfg *)SHM_ReadData ( (void *)pcShmAddr,
                                                  NODETRANCFG, &iRowNum);

   
   LOG4C ((LOG_DEBUG,"eiNodeId[%d]",eiNodeId ));
   if ( pstNodeTranCfg == NULL && iRowNum == -1 )
   {
      LOG4C ((LOG_FATAL,
                " SHM_ReadData(in PKG_GetInfoFromNodeTranCfg) 从共享内存读取数据失败 NodeId:[%d] iRowNum:[%d] pcShmAddr:[%x]\n",
                 eiNodeId,iRowNum, pcShmAddr));
      return PKG_FAIL;
   }

   if ( ( pstNodeTranCfg == NULL ) && ( iRowNum == 0 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoFromNodeTranCfg) 节点NodeId:[%d]在共享内存中无配置信息 iRowNum:[%d] pcShmAddr:[%x]\n",
                  eiNodeId,iRowNum, pcShmAddr));
      return PKG_FAIL;
   }

   for( iTime = 0; iTime < iRowNum; iTime++ )
   {   
      if ( pstNodeTranCfg->iNodeId == eiNodeId )
      {
		 *opstNodeTranCfg = *pstNodeTranCfg;

         return PKG_SUCC; 
      }
      pstNodeTranCfg++;
   }

   return PKG_NODETRANCFG_NO_DATA;
}


/*
**  函数功能: 根据节点号, 交易代码, 报文描述符, 读表tranfldcfg, 取一条记录
**
**  函数名称: PKG_GetInfoFromTranFldCfg
**
**  输入参数: int eiNodeId
**            char * epczTranCode
**            int eiMsgDscrb
**
**  输出参数: TranFldCfg * opstTranFldCfg
**
**  返回值:   int  0 成功, 其他失败
*/

int  PKG_GetInfoFromTranFldCfg( int eiNodeId, 
                                char * epczTranCode, int eiMsgDscrb, 
                                TranFldCfg * opstTranFldCfg )
{
   int iTime, iRowNum;
   TranFldCfg *pstTranFldCfg;
   
   assert( epczTranCode != NULL );
   assert( opstTranFldCfg != NULL );

   iRowNum=0;
   pstTranFldCfg = NULL;
   pstTranFldCfg = (TranFldCfg *)SHM_ReadData ( (void *)pcShmAddr,
                                                TRANFLDCFG, &iRowNum);

   if ( ( pstTranFldCfg == NULL ) && ( iRowNum == -1 ) )
   {
      LOG4C ((LOG_FATAL, "SHM_ReadData (in PKG_GetInfoFromTranFldCfg) 从"
                "共享内存读取数据失败! NodeId:[%d], TranCode:[%s], MsgDscr"
                "b:[%d]\n",
                eiNodeId, epczTranCode, eiMsgDscrb));
      return PKG_FAIL;	     
   }	  

   if ( ( pstTranFldCfg == NULL ) && ( iRowNum == 0 ) )
   {
      LOG4C ((LOG_FATAL, " SHM_ReadData(in PKG_GetInfoFromTranFldCfg) 节点"
                "NodeId:[%d]在共享内存中无配置信息 iRowNum:[%d] , TRanCode:[%s]"                ", MsgDscrb:[%d] \n",
                eiNodeId,iRowNum, epczTranCode, eiMsgDscrb));
      return PKG_FAIL;
   }

   for( iTime = 0; iTime < iRowNum; iTime++ )
   {
      if( pstTranFldCfg->iNodeId == eiNodeId && 
          strcmp( pstTranFldCfg->aczTranCode, epczTranCode ) == 0 &&
          pstTranFldCfg->iMsgDscrb == eiMsgDscrb )
      {
         *opstTranFldCfg = *pstTranFldCfg;
         return PKG_SUCC; 
      }
      pstTranFldCfg++;
   }

   LOG4C ((LOG_FATAL,
               " 节点NodeId:[%d]交易[%s]在共享内存中无配置信息,MsgDscrb:[%d] \n",
               eiNodeId, epczTranCode, eiMsgDscrb));

   return PKG_TRANFLDCFG_NO_DATA;
}


/*
**  函数功能: 计算交易域段配置信息中的交易域段的个数
**
**  函数名称: PKG_GetNumsOfFld 
**
**  输入参数: char * epczTranFld     // 交易域段配置信息
**
**  输出参数: 无
** 
**  返回值:   int 交易域段的个数
*/

int PKG_GetNumsOfFld( char * epczTranFld )
{
   int i;
   int iFldNums;
   int iFldLen;

   assert( epczTranFld != NULL );

   iFldNums = 0;
   iFldLen = strlen( epczTranFld );

   for ( i = 0; i < iFldLen; i++  )
   {
      /* 取该域域号 */
      if( epczTranFld[i] == ',' )
      {
         iFldNums++;
      }
   }
   if( iFldLen > 2 )
   {
      /*如果最后一个字符不是',',增加一个域数量*/
      if( epczTranFld[iFldLen - 1] != ',' )
      {
         iFldNums++;
      }
   }

   return iFldNums;
}


/*
**  函数功能: 计算交易域段配置信息中的交易域段的编号
**
**  函数名称: PKG_GetValFromFld 
**
**  输入参数: char * epczTranFld     // 交易域段配置信息
**            int    eiNum           // 交易域段的顺序号
**
**  输出参数: 无
** 
**  返回值:   int 交易域段的编号
**                -1 错误
*/

int PKG_GetValFromFld( char *epczTranFld, int eiNum )
{
   int j;
   int iFldLen;
   int iFldNo;
   int iMovedLength;
   char aczFldNum[250];

   assert( epczTranFld != NULL );
   if( eiNum < 0 && eiNum > PKG_GetNumsOfFld( epczTranFld ) )
   {
      LOG4C ((LOG_FATAL,
                  " PKG_GetValFromFld Find not enought parameter %s in "
                  "trancodepos or trancodeseq,Please check it",
                  epczTranFld));
      return PKG_NUM_OVERFLOW;
   }

   if( eiNum <= 0 || eiNum > PKG_GetNumsOfFld( epczTranFld ) )
   {
      LOG4C ((LOG_FATAL,
                  "The Input 'eiNum:[%d]' (in PKG_GetValFromFld) Has "
                  "OverFlowed tranfld=[%s]!\n",
                  eiNum, epczTranFld));
      return PKG_NUM_OVERFLOW;
   }
 
   j = 0;
   iFldLen = 0;
   iMovedLength = 0;
   
   /* 将指针移到第eiNum-1个域号的指针结尾 */
   while( j != eiNum - 1 )
   {
      if ( epczTranFld[iMovedLength] == ',' )
      {
         j = j + 1;
      }
      iMovedLength = iMovedLength + 1;
   }
   
   iFldLen = 0;

   /* while ( iFldLen != ',' )
      iFldLen = iFldLen + 1;   */

    while ( epczTranFld[iMovedLength+iFldLen] != ',' )
    {
      iFldLen = iFldLen + 1; 
    }
   
   memcpy( aczFldNum, epczTranFld + iMovedLength, (size_t)iFldLen );
   *( aczFldNum + iFldLen ) = '\0';
   iFldNo = atoi( aczFldNum );
   
   return iFldNo;
}

int PKG_GetValFromFld_Nested(char *epczTranFld, int eiNum, char *nestedName)
{
   int j;
   int iFldLen;
   int iFldNo;
   int iMovedLength;
   char aczFldNum[250];
   char * posTemp;

   assert( epczTranFld != NULL );
   if( eiNum < 0 && eiNum > PKG_GetNumsOfFld( epczTranFld ) )
   {
      LOG4C ((LOG_FATAL,
                  " Find not enought parameter %s in "
                  "trancodepos or trancodeseq,Please check it",
                  epczTranFld));
      return PKG_NUM_OVERFLOW;
   }

   if( eiNum <= 0 || eiNum > PKG_GetNumsOfFld (epczTranFld))
   {
      LOG4C ((LOG_FATAL,
                  "The Input 'eiNum:[%d]' Has "
                  "OverFlowed tranfld=[%s]!\n",
                  eiNum, epczTranFld));
      return PKG_NUM_OVERFLOW;
   }
 
   j = 0;
   iFldLen = 0;
   iMovedLength = 0;
   
   /* 将指针移到第eiNum-1个域号的指针结尾 */
   while( j != eiNum - 1 )
   {
      if ( epczTranFld[iMovedLength] == ',' )
      {
         j = j + 1;
      }
      iMovedLength = iMovedLength + 1;
   }
   
   iFldLen = 0;

    while ( epczTranFld[iMovedLength+iFldLen] != ',' )
    {
      iFldLen = iFldLen + 1; 
    }
   
   memcpy( aczFldNum, epczTranFld + iMovedLength, (size_t)iFldLen );
   *( aczFldNum + iFldLen ) = '\0';
   iFldNo = atoi( aczFldNum );
   
   if (iFldNo <= NESTED_STRUCT_BEGINTAG_S && iFldNo >= NESTED_STRUCT_BEGINTAG_E
       || iFldNo <= NESTED_STRUCT_ENDTAG_S && iFldNo >= NESTED_STRUCT_ENDTAG_E
       || iFldNo <= NESTED_XML_BEGINTAG_S && iFldNo >= NESTED_XML_BEGINTAG_E
       || iFldNo <= NESTED_XML_ENDTAG_S && iFldNo >= NESTED_XML_ENDTAG_E)
   {
       posTemp = strchr (aczFldNum, ':');
       if (posTemp != NULL)
       {
           strcpy (nestedName, posTemp + 1);
       }
   }
   
   return iFldNo;
}

/*
**  函数功能: 取重复区域的字段 
**
**  函数名称: PKG_GetRepFld 
**
**  输入参数: char * epczTranFld     // 交易域段配置信息
**            int    eiBeginNum      // 重复区域开始时-1的顺序号 
**
**  输出参数: char * opczRepFld      // 重复区域的字段 
** 
**  返回值:   int 0 成功 
*/
int PKG_GetRepFld( char * epczTranFld, int eiBeginNum, char * opczRepFld )
{
   int m, iMoveLen = 0, iNum = 0;

   assert( epczTranFld != NULL );
		
   for( m = eiBeginNum+1; ; m++ )
   {
      iNum = PKG_GetValFromFld( epczTranFld, m );
      if( iNum == -1 )
      {
         break;
      }
      sprintf( opczRepFld+iMoveLen, "%d,", iNum );
      iMoveLen += strlen( opczRepFld+iMoveLen );
   }

   return PKG_SUCC;
}


int PKG_GetRepFld_Nested(char * epczTranFld, int eiBeginNum, char * opczRepFld )
{
   int m, iMoveLen = 0, iNum = 0;
   char aczNestedName[50];

   assert( epczTranFld != NULL );
		
   for( m = eiBeginNum+1; ; m++ )
   {
      memset (aczNestedName, 0, sizeof (aczNestedName));
      iNum = PKG_GetValFromFld_Nested( epczTranFld, m, aczNestedName );
      if( iNum == -1 )
      {
         break;
      }

      if (iNum >= 0)
      {
          sprintf( opczRepFld+iMoveLen, "%d,", iNum );
      }
      else
      {
          sprintf( opczRepFld+iMoveLen, "%d:%s,", iNum, aczNestedName );
      }

      iMoveLen += strlen( opczRepFld+iMoveLen );
   }

   return PKG_SUCC;
}

/*
** 函数功能: 将一个用16进制数字表示的字符串转换成字符串
**
** 函数名称: PKG_XToA
**
** 输入参数: char * epczX
**           
** 输出参数: char * opczA
**
** 返回值: 0成功,其他失败 
*/
int PKG_XToA( char * epczX, char * opczA )
{
   int iALen, iXLen, iPos;
   int iVal = 0, iDVal[2];

   assert( epczX != NULL );

   iXLen = strlen( epczX );

   if( iXLen % 2 != 0 )
   {
      LOG4C ((LOG_FATAL, "Length Of The Input 16 jinzhi Buf:[%s] (in PKG_"
                "XToA) Has Error\n", epczX));
      return PKG_16_LENGTH_ERR;
   }

   for( iALen = 1; iALen <= iXLen/2; iALen++, epczX+=2, opczA++ )
   {
      for( iPos = 0, iVal = 0; iPos < 2; iPos++ )
      {
         switch( *(epczX + iPos) )
         {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': 
               iDVal[iPos] = *( epczX + iPos ) - '0'; 
               break;
            case 'A':
            case 'a':
               iDVal[iPos] = 10; 
               break;
            case 'B':
            case 'b':
               iDVal[iPos] = 11; 
               break;
            case 'C':
            case 'c':
               iDVal[iPos] = 12; 
               break;
            case 'D':
            case 'd':
               iDVal[iPos] = 13; 
               break;
            case 'E':
            case 'e':
               iDVal[iPos] = 14; 
               break;
            case 'F':
            case 'f':
               iDVal[iPos] = 15; 
               break;
            default:
               LOG4C ((LOG_FATAL, 
                           "Val Of The Input 16 jinzhi Buf in PKG_XToA Has Out Of '1--F'\n"));
               return PKG_16_INPUT_ERR;
         }
         if( iPos == 0 )
         {
            iVal = iDVal[iPos]*16;
         }
         else
         {
            iVal += iDVal[iPos]; 
         }
      }

      *opczA = iVal;
   }

   return PKG_SUCC;
}


/*
**  函数功能: 根据已知域类型eiDataType, 取得链表中eiSeq号域的eiSepSeq分域的
**            第eiNum次的重复值, 其结果分别放在不同的数据类型中
**
**  函数说明: PKG_GetFldFromLST
**
**  输入参数: UDP    epfsAddr               // 中间信息报文 
**            int    eiSeq                 // 域号 
**            int    eiSepSeq              // 分域号 
**            int    eiRepNum              // 重复顺序号 
**            int    eiDataType            // 域类型
**            
**  输出参数: char   ** opczVal 
**            int    * opiValLen
**            short  * opshVal
**            int    * opiVal
**            long   * oplVal  
**            double * opdVal
**            float  * opfVal
**
**  返回值:   int  0 成功, 其他失败
**
**  操作说明: 函数内部分配了空间,请调用者释放
*/
int  PKG_GetFldFromLST( UDP epfsAddr, int eiSeq, int eiSepSeq,
                        int eiRepNum, int eiDataType, 
                        char ** opczVal, int * opiValLen, short * opshVal, 
                        int * opiVal, long * oplVal, double * opdVal, 
                        float * opfVal  )
{
   int iRet;
   char * pczVal = NULL;

   assert( epfsAddr != NULL );

   switch( eiDataType )
   {
   case UDT_STRING:
   case UDT_BIN:
      *opiValLen = UDO_FieldLen( epfsAddr, eiSeq, eiRepNum );
      if( *opiValLen < 0 )
      {
         LOG4C ((LOG_FATAL, 
                   "UDO_FieldLen (in PKG_GetFldFromLST) Has Error"
                   "[%d], Seq:[%d], RepNum:[%d]\n",
                   *opiValLen, eiSeq, eiRepNum));
         return *opiValLen;
      }

      do
      {
         pczVal = (char *)malloc( (size_t)(*opiValLen + 1) );
      }while( pczVal == NULL );
      *( pczVal + *opiValLen ) = '\0';

      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          pczVal, opiValLen );
      *opczVal = pczVal;

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%s]\n", eiSeq, pczVal);
       }

      break;
   case UDT_SHORT:
      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          (char *)opshVal, opiValLen );

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%d]\n", eiSeq, *opshVal);
       }

      break;
   case UDT_INT:
      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          (char *)opiVal, opiValLen );

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%d]\n", eiSeq, *opiVal);
       }

      break;
   case UDT_LONG:
      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          (char *)oplVal, opiValLen );

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%ld]\n", eiSeq, *oplVal);
       }

      break;
   case UDT_DOUBLE:
      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          (char *)opdVal, opiValLen );

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%lf]\n", eiSeq, *opdVal);
       }

      break;
   case UDT_FLOAT:
      iRet = UDO_GetField( epfsAddr, eiSeq, eiRepNum, 
                          (char *)opfVal, opiValLen );

       if (fpPkgLog != NULL) {
           fprintf (fpPkgLog, "%d=[%f\n", eiSeq, *opfVal);
       }

      break;




   default:
      LOG4C ((LOG_FATAL,
                  "The Input 'iDataType:[%d]' Of Seq:[%d], SepSeq[%d], RepNum:[%d] (in PKG_GetFldFromLST) Has Error[%d]\n",
                  eiDataType, eiSeq, eiSepSeq, eiRepNum, 
                  PKG_DATATYPE_INVALID));
      return PKG_DATATYPE_INVALID;
   }

   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "UDO_GetField (in PKG_GetFldFromLST) Has Error[%d], Seq:[%d], RepNum:[%d]\n",
                  eiSeq, eiRepNum));
      return iRet;
   }

   return PKG_SUCC;
}


/* 
**  函数功能: 取得一个报文是否为机构体的变量值
**
**  函数名称: PKG_MsgIsStruct
**
**  输入参数: int eiNodeId
**
**  输出参数: int * opiToMidIsSt
**            int * opiToMsgIsSt
**
**  返回值:   int   0 成功, 其他失败
*/

int  PKG_MsgIsStruct( int eiNodeId, int * opiToMidIsSt, int * opiToMsgIsSt )
{
   int iRet;
   NodeTranCfg stNodeTranCfg;

   memset( &stNodeTranCfg, 0x00, sizeof(NodeTranCfg) );

   iRet = PKG_GetInfoFromNodeTranCfg( eiNodeId, &stNodeTranCfg );
   if( iRet != PKG_SUCC )
   {
      LOG4C ((LOG_FATAL,
                  "PKG_GetInfoFromNodeTranCfg(in PKG_MsgIsStruct) Has Error[%d], NodeId:[%d]\n",
                  iRet, eiNodeId));
      return iRet;
   }

   *opiToMidIsSt = stNodeTranCfg.iToMidIsSt;
   *opiToMsgIsSt = stNodeTranCfg.iToMsgIsSt;

   return PKG_SUCC;
}

/*
**  函数功能: 计算在结构体中一个域之前的偏移量 
**
**  函数名称: PKG_GetMoveLenForStruct 
**
**  输入参数: int eiMoveLen
**            int eiDataType
**
**  输出参数: int * opiPtrMoveLen
**
**  返回值:   无
*/
void  PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                               int * opiPtrMoveLen )
{
   switch( eiDataType )
   {
      case UDT_STRING: 
           posInStruct = getAlignPos (posInStruct, AG_CCHARTYPE, 0);
           break;
      case UDT_SHORT: 
           posInStruct = getAlignPos (posInStruct, AG_CSHORTTYPE, 0);
         break;
      case UDT_INT:
           posInStruct = getAlignPos (posInStruct, AG_CINTTYPE, 0);
         break;
      case UDT_LONG:
           posInStruct = getAlignPos (posInStruct, AG_CLONGTYPE, 0);
         break;
      case UDT_DOUBLE:
           posInStruct = getAlignPos (posInStruct, AG_CDOUBLETYPE, 0);
         break;
      case UDT_FLOAT:
           posInStruct = getAlignPos (posInStruct, AG_CFLOATTYPE, 0);
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The Input 'eiDataType:[%d] (in PKG_GetMoveLenInStruct) Has Error\n", 
                     eiDataType));
         break;
   }

   *opiPtrMoveLen = structStartMoveLen + posInStruct - eiMoveLen; 
}

/*********
void  PKG_GetMoveLenForStruct( int eiMoveLen, int eiDataType, 
                               int * opiPtrMoveLen )
{
   int iRemainder;

   switch( eiDataType )
   {
      case UDT_STRING: *opiPtrMoveLen = 0; break;
      case UDT_SHORT: 
         iRemainder = eiMoveLen % PKG_SHORT_MOD; 
         *opiPtrMoveLen = ( PKG_SHORT_MOD - iRemainder ) % PKG_SHORT_MOD;
         break;
      case UDT_INT:
         iRemainder = eiMoveLen % PKG_INT_MOD; 
         *opiPtrMoveLen = ( PKG_INT_MOD - iRemainder ) % PKG_INT_MOD;
         break;
      case UDT_LONG:
         iRemainder = eiMoveLen % PKG_LONG_MOD; 
         *opiPtrMoveLen = ( PKG_LONG_MOD - iRemainder ) % PKG_LONG_MOD;
         break;
      case UDT_DOUBLE:
         iRemainder = eiMoveLen % PKG_DOUBLE_MOD; 
         *opiPtrMoveLen = ( PKG_DOUBLE_MOD - iRemainder ) % PKG_DOUBLE_MOD;
         break;
      case UDT_FLOAT:
         iRemainder = eiMoveLen % PKG_FLOAT_MOD; 
         *opiPtrMoveLen = ( PKG_FLOAT_MOD - iRemainder ) % PKG_FLOAT_MOD;
         break;
      default:
         LOG4C ((LOG_FATAL,
                     "The Input 'eiDataType:[%d] (in PKG_GetMoveLenInStruct) Has Error\n", 
                     eiDataType));
         break;
   }
}
******/



/* 显示NodeTranCfg */
void PKG_DisplayNodeTranCfg( NodeTranCfg estNodeTranCfg )
{
fprintf( stderr, "iNodeId:[%d]\n", estNodeTranCfg.iNodeId );
fprintf( stderr, "iMsgAppId:[%d]\n", estNodeTranCfg.iMsgAppId );
fprintf( stderr, "aczMsgAppName:[%s]\n", estNodeTranCfg.aczMsgAppName );
fprintf( stderr, "iToMidId:[%d]\n", estNodeTranCfg.iToMidId );
fprintf( stderr, "iToMsgId:[%d]\n", estNodeTranCfg.iToMsgId );
fprintf( stderr, "iToMidIsSt:[%d]\n", estNodeTranCfg.iToMidIsSt );
fprintf( stderr, "iToMsgIsSt:[%d]\n", estNodeTranCfg.iToMsgIsSt );
fprintf( stderr, "aczCrtTranCodeType:[%s]\n", estNodeTranCfg.aczCrtTranCodeType );
fprintf( stderr, "aczCrtTranCodePos:[%s]\n", estNodeTranCfg.aczCrtTranCodePos );
fprintf( stderr, "aczCrtTranCodeSeq:[%s]\n", estNodeTranCfg.aczCrtTranCodeSeq );
fprintf( stderr, "aczCrtMsgFixFld:[%s]\n", estNodeTranCfg.aczCrtMsgFixFld );
fprintf( stderr, "aczWrgTranCodeType:[%s]\n", estNodeTranCfg.aczWrgTranCodeType );
fprintf( stderr, "aczWrgTranCodePos:[%s]\n", estNodeTranCfg.aczWrgTranCodePos );
fprintf( stderr, "aczWrgTranCodeSeq:[%s]\n", estNodeTranCfg.aczWrgTranCodeSeq );
fprintf( stderr, "aczWrgMsgFixFld:[%s]\n", estNodeTranCfg.aczWrgMsgFixFld );
}

void PKG_DisplayTranFldCfg( TranFldCfg estTranFldCfg )
{
   fprintf( stderr, "iNodeId:[%d]\n", estTranFldCfg.iNodeId );
   fprintf( stderr, "TranCode:[%s]\n", estTranFldCfg.aczTranCode );
   fprintf( stderr, "iMsgDscrb:[%d]\n", estTranFldCfg.iMsgDscrb );
   fprintf( stderr, "RepTimesSeq:[%s]\n", estTranFldCfg.aczRepTimesSeq );
   fprintf( stderr, "RepTimesNum:[%s]\n", estTranFldCfg.aczRepTimesNum );
   fprintf( stderr, "RepType:[%s]\n", estTranFldCfg.aczRepType );
   fprintf( stderr, "TranFld:[%s]\n", estTranFldCfg.aczTranFld );
}

void PKG_DisplayFldCfgValNode( FldCfgValNode estFldCfgValNode )
{
   fprintf( stderr, "iMsgAppId:[%d]\n", estFldCfgValNode.iMsgAppId );
   fprintf( stderr, "iFldSeq:[%d]\n", estFldCfgValNode.iFldSeq );
   fprintf( stderr, "iSepFldSeq:[%d]\n", estFldCfgValNode.iSepFldSeq );
   fprintf( stderr, "aczFldEngName:[%s]\n", estFldCfgValNode.aczFldEngName );
   fprintf( stderr, "aczFldChnName:[%s]\n", estFldCfgValNode.aczFldChnName );
   fprintf( stderr, "iSepFldFlag:[%d]\n", estFldCfgValNode.iSepFldFlag );
   fprintf( stderr, "iLenFldLen:[%d]\n", estFldCfgValNode.iLenFldLen );
   fprintf( stderr, "iFldLen:[%d]\n", estFldCfgValNode.iFldLen );
   fprintf( stderr, "iLenFldType:[%d]\n", estFldCfgValNode.iLenFldType );
   fprintf( stderr, "iFldType:[%d]\n", estFldCfgValNode.iFldType );
   fprintf( stderr, "aczLenFldDef:[%s]\n", estFldCfgValNode.aczLenFldDef );
   fprintf( stderr, "aczFldDef:[%s]\n", estFldCfgValNode.aczFldDef );
   fprintf( stderr, "aczLenFldAlign:[%s]\n", estFldCfgValNode.aczLenFldAlign );
   fprintf( stderr, "aczFldAlign:[%s]\n", estFldCfgValNode.aczFldAlign );
   fprintf( stderr, "aczSepSymbol:[%s]\n", estFldCfgValNode.aczSepSymbol );
}

int PKG_SrchTranFld( const void *pstNode1, const void *pstNode2 )
{
   int iCompResult;

   if( ((TranFldCfg*)pstNode1)->iNodeId == ((TranFldCfg*)pstNode2)->iNodeId )
   {
      iCompResult = strcmp( ((TranFldCfg*)pstNode1)->aczTranCode,
                            ((TranFldCfg*)pstNode2)->aczTranCode );
      if( iCompResult == 0 )
      {
         return ((TranFldCfg*)pstNode1)->iMsgDscrb - ((TranFldCfg*)pstNode2)->iMsgDscrb;
      }
      else
      {
         return iCompResult;
      }
   }
   else
   {
      return ((TranFldCfg*)pstNode1)->iNodeId -((TranFldCfg*)pstNode2)->iNodeId;
   }
}

/*
**  函数功能: 根据数据字典编号,交易代码和请求响应标志，读表msgfldcfg,
**            得到所有域段配置信息放入以pstFldCfgHead开始的域空间内
**  输入参数: 1 节点号
**            2 交易代码
**            3 请求响应标志，1-请求，2-响应
**  输出参数: MsgFldCfgAry * opstMsgFldCfgAry
**  返回值:   int  0 成功, 其他失败
**
** 操作说明: 函数内部分配了空间, 请调用者注意释放 
*/

int  PKG_GetInfoByTranCode( int eiNodeId, char *epczTranCode,
                            int eiFlag,
                            MsgFldCfgAry *opstMsgFldCfgAry)
{
   int iNum1, iRowNum1, iRowNum2, iTemp, iTemp1;
   TranFldCfg *pstSrchTranFldCfg,*pstTranFldCfg, stTranFldCfg;
   NodeTranCfg stNodeTranCfg,*pstNodeTranCfg;
   MsgFldCfg *pstMsgFldCfg, *pstMsgFldCfgBegin;
   char *pTmp1, *pTmp2;
   int  iMsgAppId, iFldSeq, iFldNum, aiFlds[1024];
   int iSize, iTop, iBottom;
   char aczStr[1024];

   assert( epczTranCode       != NULL );
   PUB_StrRightTrim( epczTranCode, (char)' ' );

   iRowNum1 = 0;
   pstTranFldCfg = NULL;
   pstTranFldCfg = (TranFldCfg *)SHM_ReadData ( (void *)pcShmAddr,
                                                TRANFLDCFG, &iRowNum1);
   if ( ( pstTranFldCfg == NULL ) && ( iRowNum1 == -1 ) )
   {
         LOG4C ((LOG_FATAL,
                     "SHM_ReadData (in PKG_GetInfoByTranCode) 从共享"
                     "内存读取数据失败!NodeId:[%d],TranCode:[%s],eiFlag:[%d]\n",
                     eiNodeId, epczTranCode, eiFlag));
	  return PKG_FAIL;	     
   }	  
   if ( ( pstTranFldCfg == NULL ) && ( iRowNum1 == 0 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoByTranCode)节点NodeId:"
                  "[%d]在共享内存中无配置信息 iRowNum:[%d] , TRanCode:[%s], ei"
                  "Flag:[%d] \n",
                  eiNodeId,iRowNum1, epczTranCode, eiFlag));
      return PKG_FAIL;
   }
    
   memset( &stTranFldCfg, 0x00, sizeof( TranFldCfg ) );
   stTranFldCfg.iNodeId = eiNodeId;
   stTranFldCfg.iMsgDscrb = eiFlag;
   strcpy( stTranFldCfg.aczTranCode, epczTranCode );
   pstSrchTranFldCfg = bsearch( (void *)&stTranFldCfg,
                                pstTranFldCfg,
                                (size_t)iRowNum1,
                                sizeof( TranFldCfg ),
                                PKG_SrchTranFld ); 
   if( pstSrchTranFldCfg == NULL )
   {
      LOG4C ((LOG_FATAL, 
                  " 节点%d,trancode[%s]报文方向%d未定义",
                  eiNodeId, epczTranCode, eiFlag));
      return PKG_FAIL;
   }

   stTranFldCfg = *pstSrchTranFldCfg;
   PUB_StrRightTrim( stTranFldCfg.aczTranFld, ' ' );
   if( !strcmp( stTranFldCfg.aczTranFld, "" ) )
   {
      LOG4C ((LOG_FATAL, 
                  " 节点%d,trancode[%s], 交易域段未定义",
                  eiNodeId, epczTranCode));
      return PKG_FAIL;
   }

   iRowNum1=0;
   pstNodeTranCfg = NULL ;
   pstNodeTranCfg = (NodeTranCfg *)SHM_ReadData ( (void *)pcShmAddr,
       	                                          NODETRANCFG, &iRowNum1);
   if ( ( pstNodeTranCfg == NULL ) && ( iRowNum1 == -1 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoByTranCode) 从共享"
                  "内存读取数据失败 NodeId:[%d] pcShmAddr:[%x]\n",
                  eiNodeId, pcShmAddr));
      return PKG_FAIL;
   }

   if ( ( pstNodeTranCfg == NULL ) && ( iRowNum1 == 0 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoByTranCode)节点NodeId:"
                  "[%d]在共享内存中无配置信息 iRowNum:[%d] pcShmAddr:[%x]\n",
                  eiNodeId,iRowNum1, pcShmAddr));
      return PKG_FAIL;
   }

   for( iNum1 = 0; iNum1 < iRowNum1; iNum1++ )
   {
      if ( ( *(pstNodeTranCfg + iNum1 )).iNodeId == eiNodeId )
      {
	 memcpy( &stNodeTranCfg , pstNodeTranCfg + iNum1 ,
                 sizeof( NodeTranCfg ) );
         break;
      }
   }

   if( iNum1 == iRowNum1 )
   {
      LOG4C ((LOG_FATAL, 
                  " 节点%d,在nodetrancfg中未定义", eiNodeId));
      return PKG_FAIL;
   }

   iMsgAppId = stNodeTranCfg.iMsgAppId;

   pTmp1 = stTranFldCfg.aczTranFld;
   iFldNum = 0;
   /*下面首先根据域段组成生成一个数组*/
   do
   {
      pTmp2 = strchr( pTmp1, ',' );
      if( pTmp2 != NULL )
      {
         memset( aczStr, 0x00, sizeof( aczStr ) );
         memcpy( aczStr, pTmp1, (size_t)(pTmp2 - pTmp1) );
         iFldSeq = atoi( aczStr );
         pTmp1 = pTmp2 + 1;
         if( iFldSeq != -1 && iFldSeq != -8000 && iFldSeq != -8001
             && iFldSeq != -9000 && iFldSeq != -9001)
         {
            aiFlds[iFldNum] = iFldSeq;
            iFldNum++;
         }
      }
   }while( pTmp2 != NULL );
   
   PUB_UpSort( iFldNum, aiFlds );

   pstMsgFldCfg = ( MsgFldCfg *)SHM_ReadData( (void *)pcShmAddr, 
                                              MSGFLDCFG, &iRowNum2 );

   if ( ( pstMsgFldCfg == NULL ) && ( iRowNum2 == -1 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoByTranCode) 从共享"
                  "内存读取数据失败 iMsgAppId:[%d] pcShmAddr:[%x]\n",
                  iMsgAppId, pcShmAddr));
      return PKG_FAIL;
   }
   if ( ( pstMsgFldCfg == NULL ) && ( iRowNum2 == 0 ) )
   {
      LOG4C ((LOG_FATAL,
                  " SHM_ReadData(in PKG_GetInfoByTranCode) 数据字典"
                  ":[%d]在共享内存中无配置信息 pcShmAddr:[%x]\n",
                  iMsgAppId, pcShmAddr));
      return PKG_FAIL;
   }

   /*设置已经查找的域数量初始值为0*/
   iTemp1 = 0;
   iSize = iFldNum * sizeof ( MsgFldCfg );
   do 
   {
      opstMsgFldCfgAry->pstMsgFldCfg = (void *)malloc( (size_t)iSize + 1);
   }  while( opstMsgFldCfgAry->pstMsgFldCfg == NULL );

   for (iTemp = 0; iTemp < iFldNum; iTemp++)
   {
      MsgFldCfg stMsgFldCfg;

      memset (&stMsgFldCfg, 0x00, sizeof (MsgFldCfg));
      stMsgFldCfg.iFldSeq   = aiFlds[iTemp];
      stMsgFldCfg.iMsgAppId = iMsgAppId;

      pstMsgFldCfgBegin = bsearch ( (void *)&stMsgFldCfg,
                                    pstMsgFldCfg,
                                    iRowNum2,
                                    sizeof (MsgFldCfg),
                                    PKG_CfgCompare);
      if (pstMsgFldCfgBegin == NULL)
      {
         LOG4C ((LOG_FATAL, 
                     " 数据字典%d域%d在数据字典中未定义",
                     iMsgAppId, stMsgFldCfg.iFldSeq));
         return PKG_FAIL;
      }
      *(opstMsgFldCfgAry->pstMsgFldCfg+iTemp) = *pstMsgFldCfgBegin;
   }

   opstMsgFldCfgAry->iNum = iFldNum ;

   return PKG_SUCC;
}


int  PKG_GetInfoByTranCode_2(int eiNodeId, char *epczTranCode,
                           int eiFlag,
                           MsgFldCfgAry1 *opstMsgFldCfgAry1)
{
   int  iNum1, iRowNum1, iRowNum2, iTemp, iTemp1;
   int  iMsgAppId, iFldSeq, iFldNum, aiFlds[1024];
   int  iRepTmFldNum, aiRepTmFlds[100], iIsRepFld = 0, iRepTmFldIdx =0;
   int  aiRepTmFldIdx[1024];
   int  iSize;
/*
 iTop, iBottom;
*/
   char *pTmp1, *pTmp2;
   char aczStr[1024];
   TranFldCfg  *pstSrchTranFldCfg,*pstTranFldCfg, stTranFldCfg;
   NodeTranCfg stNodeTranCfg,*pstNodeTranCfg;
   MsgFldCfg   stMsgFldCfg, *pstMsgFldCfg, *pstMsgFldCfgTemp;

   assert( epczTranCode != NULL );

   PUB_StrRightTrim (epczTranCode, ' ');

   iRowNum1 = 0;
   pstTranFldCfg = NULL;
   pstTranFldCfg = (TranFldCfg *)SHM_ReadData ((void *)pcShmAddr,
                                               TRANFLDCFG, &iRowNum1);
   if ((pstTranFldCfg == NULL) && (iRowNum1 == -1)) {
       LOG4C ((LOG_FATAL,
                   "从共享内存读取数据失败!"
                   " NodeId:[%d], TranCode:[%s], eiFlag:[%d]\n",
                   eiNodeId, epczTranCode, eiFlag));
       return PKG_FAIL;	     
   }

   if ((pstTranFldCfg == NULL) && (iRowNum1 == 0 )) {
       LOG4C ((LOG_FATAL,
                   " 节点NodeId:[%d]在共享内存中无配置信息"
                   " iRowNum:[%d] , TRanCode:[%s], eiFlag:[%d] \n",
                   eiNodeId,iRowNum1, epczTranCode, 
	           eiFlag));
       return PKG_FAIL;
   }
    
   memset (&stTranFldCfg, 0x00, sizeof (TranFldCfg));
   stTranFldCfg.iNodeId = eiNodeId;
   stTranFldCfg.iMsgDscrb = eiFlag;
   strcpy (stTranFldCfg.aczTranCode, epczTranCode);
   pstSrchTranFldCfg = bsearch ((void *)&stTranFldCfg,
                                pstTranFldCfg,
                                (size_t)iRowNum1,
                                sizeof (TranFldCfg),
                                PKG_SrchTranFld); 
   if (pstSrchTranFldCfg == NULL) {
       LOG4C ((LOG_FATAL, 
                   " 节点%d,trancode[%s]报文方向%d未定义",
                  eiNodeId, epczTranCode, eiFlag));
      return PKG_FAIL;
   }

   stTranFldCfg = *pstSrchTranFldCfg;
   PUB_StrRightTrim (stTranFldCfg.aczTranFld, ' ');
   if (!strcmp( stTranFldCfg.aczTranFld, "")) {
       LOG4C ((LOG_FATAL, 
                  " 节点%d,trancode[%s], 交易域段未定义",
                  eiNodeId, epczTranCode));
       return PKG_FAIL;
   }

   iRowNum1=0;
   pstNodeTranCfg = NULL ;
   pstNodeTranCfg = (NodeTranCfg *)SHM_ReadData ( (void *)pcShmAddr,
       	                                          NODETRANCFG, &iRowNum1);
   if ((pstNodeTranCfg == NULL) && (iRowNum1 == -1)) {
       LOG4C ((LOG_FATAL,
                "从共享内存读取数据失败 NodeId:[%d] pcShmAddr:[%x]\n",
                 eiNodeId, pcShmAddr));
       return PKG_FAIL;
   }

   if ((pstNodeTranCfg == NULL) && (iRowNum1 == 0)) {
       LOG4C ((LOG_FATAL,
                " 节点NodeId:[%d]在共享内存中无配置信息"
                " iRowNum:[%d] pcShmAddr:[%x]\n",
               eiNodeId,iRowNum1, pcShmAddr));
       return PKG_FAIL;
   }

   for (iNum1 = 0; iNum1 < iRowNum1; iNum1++) {
       if ((*(pstNodeTranCfg + iNum1 )).iNodeId == eiNodeId) {
 	   memcpy (&stNodeTranCfg, pstNodeTranCfg + iNum1,
                   sizeof (NodeTranCfg));
           break;
       }
   }

   if (iNum1 == iRowNum1) {
       LOG4C ((LOG_FATAL, 
                   " 节点%d,在nodetrancfg中未定义", eiNodeId));
       return PKG_FAIL;
   }

   iMsgAppId = stNodeTranCfg.iMsgAppId;

   pstMsgFldCfg = (MsgFldCfg *)SHM_ReadData ((void *)pcShmAddr, 
                                             MSGFLDCFG, &iRowNum2);

   if ((pstMsgFldCfg == NULL) && (iRowNum2 == -1)) {
       LOG4C ((LOG_FATAL,
                   " 从共享内存读取数据失败"
                   " iMsgAppId:[%d] pcShmAddr:[%x]\n",
                   iMsgAppId, pcShmAddr));
       return PKG_FAIL;
   }
   if ((pstMsgFldCfg == NULL) && (iRowNum2 == 0)) {
       LOG4C ((LOG_FATAL,
                   " 数据字典:[%d]在共享内存中无配置信息"
                   " pcShmAddr:[%x]\n",
                   iMsgAppId, pcShmAddr));
       return PKG_FAIL;
   }

   PUB_StrRightTrim (stTranFldCfg.aczRepTimesSeq, ' ');
   iRepTmFldNum = 0;
   if (strlen (stTranFldCfg.aczRepTimesSeq) != 0) {
       pTmp1 = stTranFldCfg.aczRepTimesSeq;
       do {
           pTmp2 = strchr (pTmp1, ',');
           if (pTmp2 != NULL) {
              memset (aczStr, 0x00, sizeof (aczStr));
              memcpy (aczStr, pTmp1, (size_t)(pTmp2 - pTmp1));
              aiRepTmFlds[iRepTmFldNum] = atoi(aczStr);
              iRepTmFldNum++;
              pTmp1 = pTmp2 + 1;
           }
       } while (pTmp2!= NULL);
   }

   pTmp1 = stTranFldCfg.aczTranFld;
   iFldNum = iIsRepFld = 0;
   iRepTmFldIdx = -1;
   do {
       pTmp2 = strchr (pTmp1, ',');
       if (pTmp2 != NULL ) 
       {
           memset (aczStr, 0x00, sizeof (aczStr));
           memcpy (aczStr, pTmp1, (size_t)(pTmp2 - pTmp1));
           iFldSeq = atoi( aczStr );
           pTmp1 = pTmp2 + 1;
           if (iFldSeq == -8000 || iFldSeq == -8001 || iFldSeq == -9000 
               || iFldSeq == -9001)
           {

           }
           else if( iFldSeq != -1)
           {
               aiFlds[iFldNum] = iFldSeq;
               if (iIsRepFld) {
                   aiRepTmFldIdx[iFldNum] = iRepTmFldIdx;
               } else {
                   aiRepTmFldIdx[iFldNum] = -1;
               }
               iFldNum++;
           }
           else 
           {
               if (iIsRepFld == 0) {
                   iIsRepFld = 1;
                   iRepTmFldIdx += 1 ;
               } else {
                   iIsRepFld = 0; 
               }
           }
       }
   } while( pTmp2 != NULL );

   for (iTemp = 0; iTemp < iRepTmFldNum; iTemp++) {
       for (iTemp1 = 0; iTemp1 < iFldNum; iTemp1++) {
           if (aiFlds[iTemp1] == aiRepTmFlds[iTemp]) {
               aiRepTmFlds[iTemp] = iTemp1;
               aiRepTmFldIdx[iTemp1] = -2;
               break;
           }
       }
   }

   if (iRepTmFldNum != 0) {
       for (iTemp = 0; iTemp < iFldNum; iTemp++) {
            if (aiRepTmFldIdx[iTemp] >= 0 ) {
                aiRepTmFldIdx[iTemp] = aiRepTmFlds[aiRepTmFldIdx[iTemp]];
            }
       }
   }

   iSize = iFldNum * sizeof (MsgFldCfg);
   do {
      opstMsgFldCfgAry1->pstMsgFldCfg = (void *)malloc ((size_t)iSize);
   }  while (opstMsgFldCfgAry1->pstMsgFldCfg == NULL);

   iSize = iFldNum * sizeof (int);
   do {
      opstMsgFldCfgAry1->piRepTmFldIdx = (void *)malloc ((size_t)iSize);
   }  while (opstMsgFldCfgAry1->piRepTmFldIdx == NULL);

   for (iTemp = 0; iTemp < iFldNum; iTemp++)
   {
      memset (&stMsgFldCfg, 0x00, sizeof (MsgFldCfg));
      stMsgFldCfg.iFldSeq   = aiFlds[iTemp];
      stMsgFldCfg.iMsgAppId = iMsgAppId;

      pstMsgFldCfgTemp = bsearch ((void *)&stMsgFldCfg,
                                  pstMsgFldCfg,
                                  iRowNum2,
                                  sizeof (MsgFldCfg),
                                  PKG_CfgCompare);
      if (pstMsgFldCfgTemp == NULL)
      {
         LOG4C ((LOG_FATAL,
                     " 数据字典%d域%d在数据字典中未定义",
                     iMsgAppId, stMsgFldCfg.iFldSeq));
         return PKG_FAIL;
      }
      *(opstMsgFldCfgAry1->pstMsgFldCfg+iTemp) = *pstMsgFldCfgTemp;
   }

   memcpy(opstMsgFldCfgAry1->piRepTmFldIdx,aiRepTmFldIdx,iFldNum * sizeof(int));

   opstMsgFldCfgAry1->iNum = iFldNum ;

   return PKG_SUCC;
}

void PKG_FreeMsgFldCfgAry( MsgFldCfgAry *epstMsgFldCfgAry )
{
   free( epstMsgFldCfgAry->pstMsgFldCfg );
}


/*
** 根据报文定义的重复域配置产生RECORD_INFO数组结构
*/
/********
int PKG_GetGRecordInfo( MsgFldCfgAry1 *epstMsgFldCfgAry1, RECORD_INFO *epstRecordInfo, int *opiNum )
{
   int iRet, i, j, k;
   MsgFldCfg *pstMsgFldCfg;
   int *piRepTmFldIdx;
   int iNum = 0;

   RECORD_INFO *pstRecordInfo;
   char *pTmp1, *pTmp2;
   char aczTmp[100], aczTmp2[100], aczTmp3[100];
   
   assert( epstMsgFldCfgAry1 != NULL );
   assert( epstRecordInfo != NULL );
   assert( opiNum != NULL );

   pstMsgFldCfg = epstMsgFldCfgAry1->pstMsgFldCfg;
   pstRecordInfo = epstRecordInfo;
   piRepTmFldIdx = epstMsgFldCfgAry1->piRepTmFldIdx;

   piRepTmFldIdx = epstMsgFldCfgAry1->piRepTmFldIdx;
   for( i=0; i<epstMsgFldCfgAry1->iNum; i++ )
   {
      if( *piRepTmFldIdx > 0 )
      {
         pstRecordInfo->fieldname = (char *)malloc(100);
         if( pstRecordInfo->fieldname == NULL )
         {
            LOG4C ((LOG_FATAL, 
                        " malloc error"));
            return PKG_FAIL;
         }
         strcpy( pstRecordInfo->fieldname, pstMsgFldCfg->aczFldEngName );
         
         memset (aczTmp, 0, sizeof (aczTmp));
         memset (aczTmp2, 0, sizeof (aczTmp2));
         memset (aczTmp3, 0, sizeof (aczTmp3));

         switch (pstMsgFldCfg->iFldType)
         {
         case UDT_STRING :
         case UDT_BIN:
            pstRecordInfo->fieldtype = FLDTYPE_STR;
            pstRecordInfo->fieldlen = pstMsgFldCfg->iFldLen;
            break;
         case UDT_SHORT:
         case UDT_INT:
         case UDT_LONG:
            pstRecordInfo->fieldtype = FLDTYPE_INT;
            strcpy( aczTmp, pstMsgFldCfg->aczMacro );
            if( aczTmp[0] != '_' )
            {
               LOG4C ((LOG_FATAL, 
                           " 数据字典%d域%d宏定义[%s]格式错误",
                           pstMsgFldCfg->iMsgAppId,
                           pstMsgFldCfg->iFldSeq, pstMsgFldCfg->aczMacro));
               return PKG_FAIL;
            }
            j = 1;
            while( aczTmp[j] != 0 )
            {
               if( aczTmp[j]<'0' || aczTmp[j]>'9' )
               {
                  LOG4C ((LOG_FATAL, 
                              " 数据字典%d域%d宏定义[%s]格式错误",
                              pstMsgFldCfg->iMsgAppId,
                              pstMsgFldCfg->iFldSeq, pstMsgFldCfg->aczMacro));
                  return PKG_FAIL;
               } 
               j++;
            }
            pstRecordInfo->fieldlen = 1 + atoi(pstMsgFldCfg->aczMacro+1);
            break;
         case UDT_FLOAT:
         case UDT_DOUBLE:
            pstRecordInfo->fieldtype = FLDTYPE_DOUBLE;
            strcpy( aczTmp, pstMsgFldCfg->aczMacro );
            if( aczTmp[0] != '_' )
            {
               LOG4C ((LOG_FATAL, 
                           " 数据字典%d域%d宏定义[%s]格式错误",
                           pstMsgFldCfg->iMsgAppId,
                           pstMsgFldCfg->iFldSeq, pstMsgFldCfg->aczMacro));
               return PKG_FAIL;
            }
            j = 1;
            while( aczTmp[j] != '_' )
            {
               if( aczTmp[j]<'0' || aczTmp[j]>'9' )
               {
                  LOG4C ((LOG_FATAL, 
                              " 数据字典%d域%d宏定义[%s]格式错误",
                              pstMsgFldCfg->iMsgAppId,
                              pstMsgFldCfg->iFldSeq, pstMsgFldCfg->aczMacro));
                  return PKG_FAIL;
               } 
               j++;
            }
            strncpy( aczTmp2, aczTmp+1, j-1 );
            j++;
            k = j;
            while( aczTmp[j] != '\0' )
            {
               if( aczTmp[j]<'0' || aczTmp[j]>'9' )
               {
                  LOG4C ((LOG_FATAL,
                              " 数据字典%d域%d宏定义[%s]格式错误, aczTmp[j]=%c, j=%d",
                              pstMsgFldCfg->iMsgAppId,
                              pstMsgFldCfg->iFldSeq, pstMsgFldCfg->aczMacro, aczTmp[j], j));
                  return PKG_FAIL;
               }
               j++;
            }
            strncpy( aczTmp3, aczTmp+k, j-k );
            pstRecordInfo->fieldlen = 1+atoi(aczTmp2)+1+atoi(aczTmp3); 
            pstRecordInfo->fieldprec = atoi(aczTmp3); 
            break;
         default :
            LOG4C ((LOG_FATAL,
                        " 不能处理的域类型[%d], fldseq[%d], msgappid[%d]",
                        pstMsgFldCfg->iFldType,
                        pstMsgFldCfg->iFldSeq, pstMsgFldCfg->iMsgAppId));
            return PKG_FAIL;
            break;
         }
         pstRecordInfo++;
         iNum++;
      }
      pstMsgFldCfg++;
      piRepTmFldIdx++;

   }
   *opiNum = iNum;
   return 0;
}
********/


/*
** 释放RecordInfo结构数组
*/
/******
void FreeGRecordInfo( RECORD_INFO *epstRecordInfo, int iNum )
{
   int i;
   RECORD_INFO *pstRecordInfo;

   assert( epstRecordInfo != NULL );

   pstRecordInfo = epstRecordInfo;

   for( i=0; i<iNum; i++ )
   {
      if( pstRecordInfo->fieldname != NULL )
      {
         free( pstRecordInfo->fieldname );
      }
      pstRecordInfo++;
   }
   
}
*****/

/* 
** end of file: pkg_base.c 
*/
