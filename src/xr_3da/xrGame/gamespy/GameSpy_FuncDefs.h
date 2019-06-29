#pragma once
//-----------------------------------------------------------------------------------------------
//Callbacks that can occur during server browsing operations
typedef enum 
{
	sbc_serveradded, //a server was added to the list, may just have an IP & port at this point
	sbc_serverupdated, //server information has been updated - either basic or full information is now available about this server
	sbc_serverupdatefailed, //an attempt to retrieve information about this server, either directly or from the master, failed
	sbc_serverdeleted, //a server was removed from the list
	sbc_updatecomplete, //the server query engine is now idle
	sbc_queryerror,		//the master returned an error string for the provided query
	sbc_serverchallengereceived // received ip verification challenge from server
} SBCallbackReason;	

// these are possible return types for GSIAvailableCheckThink
typedef enum
{
	GSIACWaiting,                 // still waiting for a response from the backend
	GSIACAvailable,               // the game's backend services are available
	GSIACUnavailable,             // the game's backend services are unavailable
	GSIACTemporarilyUnavailable   // the game's backend services are temporarily unavailable
} GSIACResult;
//States the ServerBrowser object can be in
typedef enum 
{
	sb_disconnected, //idle and not connected to the master server
	sb_listxfer,	//downloading list of servers from the master server
	sb_querying,	//querying servers
	sb_connected	//idle but still connected to the master server
} SBState;

//Error codes that can be returned from functions
typedef enum 
{
	sbe_noerror,  //no error has occured
	sbe_socketerror, //a socket function has returned an unexpected error
	sbe_dnserror,  //DNS lookup of master address failed
	sbe_connecterror,  //connection to master server failed
	sbe_dataerror, //invalid data was returned from master server
	sbe_allocerror, //memory allocation failed
	sbe_paramerror,	//an invalid parameter was passed to a function
	sbe_duplicateupdateerror //server update requested on a server that was already being updated
} SBError;		

typedef enum {SBFalse, SBTrue} SBBool;
typedef enum {PTFalse, PTTrue} PTBool;

typedef enum {key_server, key_player, key_team} qr2_key_type;

typedef enum 
{e_qrnoerror, //no error occured
e_qrwsockerror, //a standard socket call failed (exhausted resources?)
e_qrbinderror, //the SDK was unable to find an available port to bind on
e_qrdnserror, //a DNS lookup (for the master server) failed
e_qrconnerror,  //the server is behind a nat and does not support negotiation
e_qrnochallengeerror, //no challenge was received from the master - either the master is down, or a firewall is blocking UDP
qr2_error_t_count
} qr2_error_t;

//-----------------------------------------------------------------------------------------------
typedef void (__cdecl* fnSBCallback)(void* sb, SBCallbackReason reason, void* server, void *instance);
typedef void (__cdecl* fnAuthCallBackFn)(int gameid, int localid, int authenticated, char *errmsg, void *instance);
typedef void (__cdecl* fnRefreshAuthCallBackFn)(int gameid, int localid, int hint, char *challenge, void *instance);

typedef void (__cdecl* fnqr2_natnegcallback_t)(int cookie, void *userdata);	
typedef void (__cdecl* fnqr2_clientmessagecallback_t)(char *data, int len, void *userdata);	
typedef void (__cdecl* fnqr2_publicaddresscallback_t)(unsigned int ip, unsigned short port, void *userdata);

typedef void (__cdecl* fnqr2_serverkeycallback_t)(int keyid, void* outbuf, void *userdata);
typedef void (__cdecl* fnqr2_playerteamkeycallback_t)(int keyid, int index, void* outbuf, void *userdata);	
typedef void (__cdecl* fnqr2_keylistcallback_t)(qr2_key_type keytype, void* keybuffer, void *userdata);	
typedef int  (__cdecl* fnqr2_countcallback_t)(qr2_key_type keytype, void *userdata);	
typedef void (__cdecl* fnqr2_adderrorcallback_t)(qr2_error_t error, char *errmsg, void *userdata);	

typedef void (__cdecl* ptPatchCallback) ( PTBool available, PTBool mandatory, const char * versionName, int fileID, const char * downloadURL,  void * param );

extern "C" {

#define GAMESPY_TFN_DECL(r, f, p) typedef DLL_API r __cdecl t_fn_xrGS_##f p

};

#define GAMESPY_FN_VAR_DECL(r, f, p) GAMESPY_TFN_DECL(r, f, p); t_fn_xrGS_##f* xrGS_##f;
#define GAMESPY_LOAD_FN(f)    f = (t_fn_##f*)GetProcAddress(hGameSpyDLL, #f); R_ASSERT2(f, "No such func in xrGameSpy.dll");
