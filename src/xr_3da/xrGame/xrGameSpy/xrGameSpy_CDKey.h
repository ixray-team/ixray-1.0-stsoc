#pragma once
#include "xrGameSpy_MainDefs.h"
#include "GameSpy/QR2/qr2.h"
#include "GameSpy/CDKey/gcdkeys.h"

extern "C"
{
	EXPORT_FN_DECL(void, gcd_compute_response_newauth, (char *cdkey, char *challenge,char* response));
	EXPORT_FN_DECL(void, gcd_compute_response_reauth, (char *cdkey, char *challenge,char* response));
	EXPORT_FN_DECL(int, gcd_init_qr2, (qr2_t qrec));
	EXPORT_FN_DECL(void, gcd_shutdown, (void));
	EXPORT_FN_DECL(void, gcd_authenticate_user, (int localid, unsigned int userip, char *challenge, char *response, 
						   AuthCallBackFn authfn, RefreshAuthCallBackFn refreshfn, void *instance));
	EXPORT_FN_DECL(void, gcd_disconnect_user, (int localid));
	EXPORT_FN_DECL(void, gcd_think, (void));
}