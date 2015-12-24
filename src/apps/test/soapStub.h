/* soapStub.h
   Generated by gSOAP 2.7.7 from posService.h
   Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#ifndef SOAP_TYPE_ns2__posRequest
#define SOAP_TYPE_ns2__posRequest (6)
/* ns2:posRequest */
struct ns2__posRequest
{
	char *msg;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_ns2__posRequestResponse
#define SOAP_TYPE_ns2__posRequestResponse (7)
/* ns2:posRequestResponse */
struct ns2__posRequestResponse
{
	char *return_;	/* RPC return element */	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_ns2__posUserRequest
#define SOAP_TYPE_ns2__posUserRequest (8)
/* ns2:posUserRequest */
struct ns2__posUserRequest
{
	struct ns2__userEntity *user;	/* optional element of type ns2:userEntity */
};
#endif

#ifndef SOAP_TYPE_ns2__userEntity
#define SOAP_TYPE_ns2__userEntity (9)
/* ns2:userEntity */
struct ns2__userEntity
{
	char *userName;	/* optional element of type xsd:string */
	char *userPwd;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_ns2__posUserRequestResponse
#define SOAP_TYPE_ns2__posUserRequestResponse (11)
/* ns2:posUserRequestResponse */
struct ns2__posUserRequestResponse
{
	struct ns2__userEntity *return_;	/* RPC return element */	/* optional element of type ns2:userEntity */
};
#endif

#ifndef SOAP_TYPE___ns1__posRequest
#define SOAP_TYPE___ns1__posRequest (15)
/* Operation wrapper: */
struct __ns1__posRequest
{
	struct ns2__posRequest *ns2__posRequest;	/* optional element of type ns2:posRequest */
};
#endif

#ifndef SOAP_TYPE___ns1__posUserRequest
#define SOAP_TYPE___ns1__posUserRequest (19)
/* Operation wrapper: */
struct __ns1__posUserRequest
{
	struct ns2__posUserRequest *ns2__posUserRequest;	/* optional element of type ns2:posUserRequest */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (22)
/* SOAP Header: */
struct SOAP_ENV__Header
{
	void *dummy;	/* transient */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (23)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
	char *SOAP_ENV__Value;	/* optional element of type QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (25)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
	char *__any;
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (26)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (27)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
	char *faultcode;	/* optional element of type QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (4)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif


/******************************************************************************\
 *                                                                            *
 * Typedef Synonyms                                                           *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Stubs                                                                      *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__posRequest(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns2__posRequest *ns2__posRequest, struct ns2__posRequestResponse *ns2__posRequestResponse);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__posUserRequest(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns2__posUserRequest *ns2__posUserRequest, struct ns2__posUserRequestResponse *ns2__posUserRequestResponse);

#ifdef __cplusplus
}
#endif

#endif

/* End of soapStub.h */
