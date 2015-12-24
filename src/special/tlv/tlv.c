#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include "tlv.h"
#include "log.h"


/*根据标签读取TLV值*/
int TlvGetValue(int nTag, char *psValue, int *pnValueLen, char *pszSrc, int nSrcLen)
{
	char *p = NULL;
	int nTlvLen=0;
	int nTlvTag=0;
	int nOffset=0;
	char szTmp[1024];

	nOffset =0;
	p = pszSrc;

	while(nOffset < nSrcLen)
	{
		//TAG
		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, p+nOffset, 4);
		nTlvTag = atoi(szTmp);
		nOffset += 4;
			
		//LEN
		
		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, p+nOffset, 4);
		nTlvLen = atoi(szTmp);
		nOffset+=4;

		if(nTlvLen > 512)
		{
			return -1;
		}
		if(nTlvTag == nTag)
		{
			memcpy(psValue, p+nOffset, nTlvLen);
			*pnValueLen = nTlvLen;
			return 0;
		}
		nOffset += nTlvLen;
	}
}



int TlvSetValue(int nTag, char *pszSrc, int *pnSrcLen, char *psValue, int nValueLen, int *pnTagNum)
{
    char  aczTag[4+1];
	char  aczTagLen[4+1];
	int nOffset=0;
	char *p = pszSrc;


	/*判断TLV 值的长度*/
	if(nValueLen <= 0)
	{
		return 0;
    }

	nOffset = *pnSrcLen;

	memset(aczTag, 0, sizeof(aczTag));
	sprintf(aczTag, "%03d", nTag%999);

	memset(aczTagLen, 0, sizeof(aczTagLen));
	sprintf(aczTagLen, "%04d", nValueLen%9999);

	memcpy(p+nOffset, aczTag, 4);
	nOffset += 4;

	
	memcpy(p+nOffset, aczTagLen, 4);
	nOffset += 4;

	memcpy(p+nOffset, psValue, nValueLen);
	nOffset += nValueLen;

	*pnSrcLen = nOffset;

	*pnTagNum += 1;

	return 0;
	
}


int TlvGetCurrentTag(int *pnTag, char *psValue, int *pnValueLen, char *pszSrc, int *pnOffet)
{
	char *p=NULL;
	int nTlvLen=0;
	int nTlvTag=0;
	char szTmp[1024];

	p = pszSrc + *pnOffet;
	
	//TAG
	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, p, 4);
	nTlvTag = atoi(szTmp);
	p += 4;
	
	*pnOffet += 4;
	
	//LOG4C ((LOG_FATAL, "tag[%d]", nTlvTag));
	//LEN
	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, p, 4);
	nTlvLen = atoi(szTmp);
	p+=4;
	*pnOffet += 4;
//	LOG4C ((LOG_FATAL, "nTlvLen[%d]", nTlvLen));

	if(nTlvLen > 512)
	{
		return -1;
	}
	*pnTag = nTlvTag;
	memcpy(psValue, p, nTlvLen);
	*pnValueLen = nTlvLen;
	
	*pnOffet += nTlvLen;
	return 0;
}






