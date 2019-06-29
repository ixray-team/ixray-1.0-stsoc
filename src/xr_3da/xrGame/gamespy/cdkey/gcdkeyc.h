/******
gcdkeyc.h
GameSpy CDKey SDK Client Header
  
Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com

******

 Please see the GameSpy CDKey SDK documentation for more 
 information

******/

#ifndef _GOACDKEYC_H_
#define _GOACDKEYC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RESPONSE_SIZE 73

void __cdecl gcd_compute_response(char *cdkey, char *challenge,/*out*/ char response[73]);


#ifdef __cplusplus
}
#endif

#endif

