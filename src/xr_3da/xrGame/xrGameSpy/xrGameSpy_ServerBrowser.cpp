#include "stdafx.h"
#include "windows.h"
#include "xrGameSpy_MainDefs.h"

#include "xrGameSpy_ServerBrowser.h"


XRGAMESPY_API int xrGS_GetQueryVersion ()
{
	return QVERSION_QR2;
}

XRGAMESPY_API ServerBrowser xrGS_ServerBrowserNew(const gsi_char *queryForGamename, const gsi_char *queryFromGamename, const gsi_char *queryFromKey, int queryFromVersion, int maxConcUpdates, int queryVersion, SBBool bLAN_Only, ServerBrowserCallback callback, void *instance)
{
	return ServerBrowserNew(queryForGamename, queryFromGamename, queryFromKey, queryFromVersion, maxConcUpdates, queryVersion, bLAN_Only, callback, instance);
}

XRGAMESPY_API void xrGS_ServerBrowserFree (ServerBrowser sb)
{
	ServerBrowserFree (sb);
};

XRGAMESPY_API void xrGS_ServerBrowserClear(ServerBrowser sb)
{
	ServerBrowserClear(sb);
};

XRGAMESPY_API SBError xrGS_ServerBrowserThink(ServerBrowser sb)
{
	return ServerBrowserThink(sb);
};

XRGAMESPY_API SBState xrGS_ServerBrowserState(ServerBrowser sb)
{
	return ServerBrowserState(sb);
};

XRGAMESPY_API void xrGS_ServerBrowserHalt(ServerBrowser sb)
{
	ServerBrowserHalt(sb);
};

XRGAMESPY_API SBError xrGS_ServerBrowserUpdate (ServerBrowser sb, SBBool async, SBBool disconnectOnComplete, const unsigned char *basicFields, int numBasicFields, const gsi_char *serverFilter)
{
	return ServerBrowserUpdate(sb, async, disconnectOnComplete, basicFields, numBasicFields, serverFilter);
};

XRGAMESPY_API SBError xrGS_ServerBrowserLANUpdate(ServerBrowser sb, SBBool async, unsigned short startSearchPort, unsigned short endSearchPort)
{
	return ServerBrowserLANUpdate(sb, async,  startSearchPort,  endSearchPort);
}

XRGAMESPY_API int xrGS_ServerBrowserCount (ServerBrowser sb)
{
	return ServerBrowserCount (sb);
};

XRGAMESPY_API SBServer xrGS_ServerBrowserGetServer (ServerBrowser sb, int index)
{
	return ServerBrowserGetServer(sb, index);
}
XRGAMESPY_API SBServer xrGS_ServerBrowserGetServerByIP(ServerBrowser sb, const gsi_char* ip, unsigned short port)
{
	return ServerBrowserGetServerByIP(sb, ip, port);
};

XRGAMESPY_API char * xrGS_SBServerGetPublicAddress(SBServer server)
{
	return SBServerGetPublicAddress(server);
};
XRGAMESPY_API unsigned short xrGS_SBServerGetPublicQueryPort(SBServer server)
{
	return SBServerGetPublicQueryPort(server);
};
XRGAMESPY_API const gsi_char * xrGS_SBServerGetStringValue(SBServer server, const gsi_char *keyname, const gsi_char *def)
{
	return SBServerGetStringValue(server, keyname, def);
};
XRGAMESPY_API int xrGS_SBServerGetIntValue(SBServer server, const gsi_char *key, int idefault)
{
	return SBServerGetIntValue(server, key, idefault);
};
XRGAMESPY_API double xrGS_SBServerGetFloatValue(SBServer server, const gsi_char *key, double fdefault)
{
	return SBServerGetFloatValue(server, key, fdefault);
};
XRGAMESPY_API SBBool xrGS_SBServerGetBoolValue(SBServer server, const gsi_char *key, SBBool bdefault)
{
	return SBServerGetBoolValue(server, key, bdefault);
};
XRGAMESPY_API int xrGS_SBServerGetPing(SBServer server)
{
	return SBServerGetPing(server);
};

XRGAMESPY_API SBError xrGS_ServerBrowserAuxUpdateServer(ServerBrowser sb, SBServer server, SBBool async, SBBool fullUpdate)
{
	return ServerBrowserAuxUpdateServer(sb, server, async, fullUpdate);
}

XRGAMESPY_API SBError xrGS_ServerBrowserAuxUpdateIP(ServerBrowser sb, const gsi_char *ip, unsigned short port, SBBool viaMaster, SBBool async, SBBool fullUpdate)
{
	return ServerBrowserAuxUpdateIP(sb, ip, port, viaMaster, async, fullUpdate);
}

XRGAMESPY_API const gsi_char * xrGS_SBServerGetPlayerStringValue(SBServer server, int playernum, const gsi_char *key, const gsi_char *sdefault)
{
	return SBServerGetPlayerStringValue(server, playernum, key, sdefault);
}
XRGAMESPY_API int xrGS_SBServerGetPlayerIntValue(SBServer server, int playernum, const gsi_char *key, int idefault)
{
	return SBServerGetPlayerIntValue(server, playernum, key, idefault);
}
XRGAMESPY_API double xrGS_SBServerGetPlayerFloatValue(SBServer server, int playernum, const gsi_char *key, double fdefault)
{
	return SBServerGetPlayerFloatValue(server, playernum, key, fdefault);
}

XRGAMESPY_API const gsi_char *xrGS_SBServerGetTeamStringValue(SBServer server, int teamnum, const gsi_char *key, const gsi_char *sdefault)
{
	return SBServerGetTeamStringValue(server, teamnum, key, sdefault);
}
XRGAMESPY_API int xrGS_SBServerGetTeamIntValue(SBServer server, int teamnum, const gsi_char *key, int idefault)
{
	return SBServerGetTeamIntValue(server, teamnum, key, idefault);
}
XRGAMESPY_API double xrGS_SBServerGetTeamFloatValue(SBServer server, int teamnum, const gsi_char *key, double fdefault)
{
	return SBServerGetTeamFloatValue(server, teamnum, key, fdefault);
}

XRGAMESPY_API void xrGS_ServerBrowserRemoveIP(ServerBrowser sb, const gsi_char *ip, unsigned short port)
{
	ServerBrowserRemoveIP(sb, ip, port);
}
XRGAMESPY_API void xrGS_ServerBrowserRemoveServer(ServerBrowser sb, SBServer server)
{
	ServerBrowserRemoveServer(sb, server);
}