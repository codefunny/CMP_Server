#include "soapStub.h"
#include "posServiceSoapBinding.nsmap"


int posRequest()
{
    struct soap posRequest_soap;
	struct ns2__posRequest request;
    struct ns2__posRequestResponse response;
	char szTmp[1024];
	
    int result = 0;
    soap_init(&posRequest_soap);
    soap_set_namespaces(&posRequest_soap, namespaces);
	memset(szTmp, 0, sizeof(szTmp));

	strcpy(szTmp, "11111");
	request.msg = szTmp;
	
    soap_call___ns1__posRequest(&posRequest_soap, NULL, NULL, &request, &response);

	printf("return=[%s]\n", response.return_);
	
    soap_end(&posRequest_soap);
    soap_done(&posRequest_soap);
    return result;
}

