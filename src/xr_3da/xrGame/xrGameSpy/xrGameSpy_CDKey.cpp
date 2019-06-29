#include "stdafx.h"
#include "windows.h"
#include "xrGameSpy_MainDefs.h"

#include "xrGameSpy_CDKey.h"

#include "GameSpy/CDKey/gcdkeyc.h"
#include "GameSpy/CDKey/gcdkeys.h"

#define GAMESPY_PRODUCTID			0

XRGAMESPY_API void xrGS_gcd_compute_response_newauth(char *cdkey, char *challenge,char* response)
{
	gcd_compute_response(cdkey, challenge,response, CDResponseMethod_NEWAUTH);
};

XRGAMESPY_API void xrGS_gcd_compute_response_reauth(char *cdkey, char *challenge,char* response)
{
	gcd_compute_response(cdkey, challenge,response, CDResponseMethod_REAUTH);
};

XRGAMESPY_API int xrGS_gcd_init_qr2(qr2_t qrec)
{
	return gcd_init_qr2(qrec, GAMESPY_PRODUCTID);
};
XRGAMESPY_API void xrGS_gcd_shutdown(void)
{
	gcd_shutdown();
}
XRGAMESPY_API void xrGS_gcd_authenticate_user(int localid, unsigned int userip, char *challenge, char *response, 
						   AuthCallBackFn authfn, RefreshAuthCallBackFn refreshfn, void *instance)
{
	gcd_authenticate_user(GAMESPY_PRODUCTID, localid, userip, challenge, response, authfn, refreshfn, instance);
}
XRGAMESPY_API void xrGS_gcd_disconnect_user(int localid)
{
	gcd_disconnect_user(GAMESPY_PRODUCTID, localid);
}
XRGAMESPY_API void xrGS_gcd_think(void)
{
	gcd_think();
}