/* soapLicWebServiceSoapProxy.cpp
   Generated by gSOAP 2.8.22 from LicWebService.h

Copyright(C) 2000-2015, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "soapLicWebServiceSoapProxy.h"

LicWebServiceSoapProxy::LicWebServiceSoapProxy()
{	LicWebServiceSoapProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

LicWebServiceSoapProxy::LicWebServiceSoapProxy(const struct soap &_soap) : soap(_soap)
{ }

LicWebServiceSoapProxy::LicWebServiceSoapProxy(const char *url)
{	LicWebServiceSoapProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = url;
}

LicWebServiceSoapProxy::LicWebServiceSoapProxy(soap_mode iomode)
{	LicWebServiceSoapProxy_init(iomode, iomode);
}

LicWebServiceSoapProxy::LicWebServiceSoapProxy(const char *url, soap_mode iomode)
{	LicWebServiceSoapProxy_init(iomode, iomode);
	soap_endpoint = url;
}

LicWebServiceSoapProxy::LicWebServiceSoapProxy(soap_mode imode, soap_mode omode)
{	LicWebServiceSoapProxy_init(imode, omode);
}

LicWebServiceSoapProxy::~LicWebServiceSoapProxy()
{ }

void LicWebServiceSoapProxy::LicWebServiceSoapProxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "http://www.humkyung.co.kr/LicWebService/", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void LicWebServiceSoapProxy::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void LicWebServiceSoapProxy::reset()
{	destroy();
	soap_done(this);
	soap_initialize(this);
	LicWebServiceSoapProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void LicWebServiceSoapProxy::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *LicWebServiceSoapProxy::soap_header()
{	return this->header;
}

const SOAP_ENV__Fault *LicWebServiceSoapProxy::soap_fault()
{	return this->fault;
}

const char *LicWebServiceSoapProxy::soap_fault_string()
{	return *soap_faultstring(this);
}

const char *LicWebServiceSoapProxy::soap_fault_detail()
{	return *soap_faultdetail(this);
}

int LicWebServiceSoapProxy::soap_close_socket()
{	return soap_closesock(this);
}

int LicWebServiceSoapProxy::soap_force_close_socket()
{	return soap_force_closesock(this);
}

void LicWebServiceSoapProxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void LicWebServiceSoapProxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *LicWebServiceSoapProxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

int LicWebServiceSoapProxy::Login(const char *endpoint, const char *soap_action, _ns1__Login *ns1__Login, _ns1__LoginResponse &ns1__LoginResponse)
{	struct soap *soap = this;
	struct __ns1__Login soap_tmp___ns1__Login;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://localhost:49227/LicWebService.asmx?op=Login";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Login.ns1__Login = ns1__Login;
	soap_serializeheader(soap);
	soap_serialize___ns1__Login(soap, &soap_tmp___ns1__Login);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Login(soap, &soap_tmp___ns1__Login, "-ns1:Login", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Login(soap, &soap_tmp___ns1__Login, "-ns1:Login", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__LoginResponse)
		return soap_closesock(soap);
	ns1__LoginResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__LoginResponse.soap_get(soap, "ns1:LoginResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::IsValidToken(const char *endpoint, const char *soap_action, _ns1__IsValidToken *ns1__IsValidToken, _ns1__IsValidTokenResponse &ns1__IsValidTokenResponse)
{	struct soap *soap = this;
	struct __ns1__IsValidToken soap_tmp___ns1__IsValidToken;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/IsValidToken";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__IsValidToken.ns1__IsValidToken = ns1__IsValidToken;
	soap_serializeheader(soap);
	soap_serialize___ns1__IsValidToken(soap, &soap_tmp___ns1__IsValidToken);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__IsValidToken(soap, &soap_tmp___ns1__IsValidToken, "-ns1:IsValidToken", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__IsValidToken(soap, &soap_tmp___ns1__IsValidToken, "-ns1:IsValidToken", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__IsValidTokenResponse)
		return soap_closesock(soap);
	ns1__IsValidTokenResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__IsValidTokenResponse.soap_get(soap, "ns1:IsValidTokenResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::Logout(const char *endpoint, const char *soap_action, _ns1__Logout *ns1__Logout, _ns1__LogoutResponse &ns1__LogoutResponse)
{	struct soap *soap = this;
	struct __ns1__Logout soap_tmp___ns1__Logout;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/Logout";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Logout.ns1__Logout = ns1__Logout;
	soap_serializeheader(soap);
	soap_serialize___ns1__Logout(soap, &soap_tmp___ns1__Logout);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Logout(soap, &soap_tmp___ns1__Logout, "-ns1:Logout", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Logout(soap, &soap_tmp___ns1__Logout, "-ns1:Logout", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__LogoutResponse)
		return soap_closesock(soap);
	ns1__LogoutResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__LogoutResponse.soap_get(soap, "ns1:LogoutResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::Reset(const char *endpoint, const char *soap_action, _ns1__Reset *ns1__Reset, _ns1__ResetResponse &ns1__ResetResponse)
{	struct soap *soap = this;
	struct __ns1__Reset soap_tmp___ns1__Reset;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/Reset";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Reset.ns1__Reset = ns1__Reset;
	soap_serializeheader(soap);
	soap_serialize___ns1__Reset(soap, &soap_tmp___ns1__Reset);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Reset(soap, &soap_tmp___ns1__Reset, "-ns1:Reset", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Reset(soap, &soap_tmp___ns1__Reset, "-ns1:Reset", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__ResetResponse)
		return soap_closesock(soap);
	ns1__ResetResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__ResetResponse.soap_get(soap, "ns1:ResetResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::Login_(const char *endpoint, const char *soap_action, _ns1__Login *ns1__Login, _ns1__LoginResponse &ns1__LoginResponse)
{	struct soap *soap = this;
	struct __ns1__Login_ soap_tmp___ns1__Login_;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/Login";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Login_.ns1__Login = ns1__Login;
	soap_serializeheader(soap);
	soap_serialize___ns1__Login_(soap, &soap_tmp___ns1__Login_);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Login_(soap, &soap_tmp___ns1__Login_, "-ns1:Login", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Login_(soap, &soap_tmp___ns1__Login_, "-ns1:Login", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__LoginResponse)
		return soap_closesock(soap);
	ns1__LoginResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__LoginResponse.soap_get(soap, "ns1:LoginResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::IsValidToken_(const char *endpoint, const char *soap_action, _ns1__IsValidToken *ns1__IsValidToken, _ns1__IsValidTokenResponse &ns1__IsValidTokenResponse)
{	struct soap *soap = this;
	struct __ns1__IsValidToken_ soap_tmp___ns1__IsValidToken_;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/IsValidToken";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__IsValidToken_.ns1__IsValidToken = ns1__IsValidToken;
	soap_serializeheader(soap);
	soap_serialize___ns1__IsValidToken_(soap, &soap_tmp___ns1__IsValidToken_);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__IsValidToken_(soap, &soap_tmp___ns1__IsValidToken_, "-ns1:IsValidToken", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__IsValidToken_(soap, &soap_tmp___ns1__IsValidToken_, "-ns1:IsValidToken", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__IsValidTokenResponse)
		return soap_closesock(soap);
	ns1__IsValidTokenResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__IsValidTokenResponse.soap_get(soap, "ns1:IsValidTokenResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::Logout_(const char *endpoint, const char *soap_action, _ns1__Logout *ns1__Logout, _ns1__LogoutResponse &ns1__LogoutResponse)
{	struct soap *soap = this;
	struct __ns1__Logout_ soap_tmp___ns1__Logout_;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/Logout";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Logout_.ns1__Logout = ns1__Logout;
	soap_serializeheader(soap);
	soap_serialize___ns1__Logout_(soap, &soap_tmp___ns1__Logout_);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Logout_(soap, &soap_tmp___ns1__Logout_, "-ns1:Logout", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Logout_(soap, &soap_tmp___ns1__Logout_, "-ns1:Logout", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__LogoutResponse)
		return soap_closesock(soap);
	ns1__LogoutResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__LogoutResponse.soap_get(soap, "ns1:LogoutResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int LicWebServiceSoapProxy::Reset_(const char *endpoint, const char *soap_action, _ns1__Reset *ns1__Reset, _ns1__ResetResponse &ns1__ResetResponse)
{	struct soap *soap = this;
	struct __ns1__Reset_ soap_tmp___ns1__Reset_;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost:49227/LicWebService.asmx";
	if (soap_action == NULL)
		soap_action = "http://www.humkyung.co.kr/LicWebService/Reset";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__Reset_.ns1__Reset = ns1__Reset;
	soap_serializeheader(soap);
	soap_serialize___ns1__Reset_(soap, &soap_tmp___ns1__Reset_);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__Reset_(soap, &soap_tmp___ns1__Reset_, "-ns1:Reset", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__Reset_(soap, &soap_tmp___ns1__Reset_, "-ns1:Reset", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&ns1__ResetResponse)
		return soap_closesock(soap);
	ns1__ResetResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__ResetResponse.soap_get(soap, "ns1:ResetResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}
/* End of client proxy code */