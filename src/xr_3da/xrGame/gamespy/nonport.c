/******
nonport.c
GameSpy Common Code
  
Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
******/

#ifdef __cplusplus
extern "C" {
#endif

#include "nonport.h"

#if defined(_WIN32) && !defined(UNDER_CE)
#if defined(__WINSOCK_2_0__)
// Added by Saad Nader on 08-02-2004
// support for winsock2
#pragma comment(lib, "ws2_32")
#else
#pragma comment(lib, "wsock32")
#endif
// end added
#pragma comment(lib, "advapi32")
#endif

#ifdef UNDER_CE
#pragma comment(lib, "platutil")
#pragma comment(lib, "winsock")
#endif

// Disable compiler warnings for issues that are unavoidable.
/////////////////////////////////////////////////////////////
#if defined(_MSC_VER) // DevStudio
// Level4, "conditional expression is constant". 
// Occurs with use of the MS provided macro FD_SET
#pragma warning ( disable: 4127 )
#endif // _MSC_VER

#ifdef GSI_MEM_TRACK
// GSI memory tracker
size_t gsimemtrack_total;
size_t gsimemtrack_peak;
int gsimemtrack_num;

void * gsimalloctrack(size_t size)
{
	void * ptr = malloc(size + sizeof(size_t));
	if(!ptr)
		return NULL;
	memcpy(ptr, &size, sizeof(size_t));
	gsimemtrack_total += size;
	gsimemtrack_peak = max(gsimemtrack_peak, gsimemtrack_total);
	gsimemtrack_num++;
	return (void *)((char *)ptr + sizeof(size_t));
}

void gsifreetrack(void * ptr)
{
	size_t size;
	if(!ptr)
		return;
	ptr = (void *)((char *)ptr - sizeof(size_t));
	memcpy(&size, ptr, sizeof(size_t));
	gsimemtrack_total -= size;
	gsimemtrack_num--;
	free(ptr);
}

void * gsirealloctrack(void * ptr, size_t size)
{
	size_t oldSize;
	void * oldPtr;
	if(ptr && !size)
	{
		gsifreetrack(ptr);
		return NULL;
	}
	if(!ptr)
	{
		return gsimalloctrack(size);
	}
	oldPtr = (void *)((char *)ptr - sizeof(size_t));
	memcpy(&oldSize, oldPtr, sizeof(size_t));
	ptr = realloc(oldPtr, size + sizeof(size_t));
	if(!ptr)
		return NULL;
	memcpy(ptr, &size, sizeof(size_t));
	gsimemtrack_total += (size - oldSize);
	gsimemtrack_peak = max(gsimemtrack_peak, gsimemtrack_total);
	return (void *)((char *)ptr + sizeof(size_t));
}
#endif

#ifdef UNDER_CE
time_t time(time_t *timer)
{
	static time_t ret;
	SYSTEMTIME systime;
	FILETIME ftime;
	LONGLONG holder;

	GetLocalTime(&systime);
	SystemTimeToFileTime(&systime, &ftime);
	holder = (ftime.dwHighDateTime << 32) + ftime.dwLowDateTime;
	if (timer == NULL)
		timer = &ret;
	*timer = (time_t)((holder - 116444736000000000) / 10000000);
	return *timer; 
}

int isdigit( int c )
{
	return (c >= '0' && c <= '9');
}

int isxdigit( int c )
{
	return ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'));
}

int isalnum( int c )
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

int isspace(int c)
{
	return ((c >= 0x9 && c <= 0xD) || (c == 0x20)); 
}
#endif

#ifdef _PS2
extern int sceCdReadClock();

#if !defined(__MWERKS__) && !defined(_PS2)
typedef unsigned char u_char;
#endif

typedef struct {
        u_char stat;            /* status */
        u_char second;          /* second */
        u_char minute;          /* minute */
        u_char hour;            /* hour   */

        u_char pad;             /* pad    */
        u_char day;             /* day    */
        u_char month;           /* month  */
        u_char year;            /* year   */
} sceCdCLOCK;

static unsigned long GetTicks()
{
	unsigned long ticks;
	asm volatile (" mfc0 %0, $9 " : "=r" (ticks));
    return ticks;
}

#define DEC(x) (10*(x/16)+(x%16))
#define _BASE_YEAR 			 70L
#define _MAX_YEAR 			138L
#define _LEAP_YEAR_ADJUST 	 17L
int _days[] = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};

static time_t _gmtotime_t (
        int yr,     /* 0 based */
        int mo,     /* 1 based */
        int dy,     /* 1 based */
        int hr,
        int mn,
        int sc
        )
{
        int tmpdays;
        long tmptim;
        struct tm tb;

        if ( ((long)(yr -= 1900) < _BASE_YEAR) || ((long)yr > _MAX_YEAR) )
                return (time_t)(-1);

        tmpdays = dy + _days[mo - 1];
        if ( !(yr & 3) && (mo > 2) )
                tmpdays++;

        tmptim = (long)yr - _BASE_YEAR;

        tmptim = ( ( ( ( tmptim ) * 365L
                 + ((long)(yr - 1) >> 2) - (long)_LEAP_YEAR_ADJUST
                 + (long)tmpdays )
                 * 24L + (long)hr )
                 * 60L + (long)mn )
                 * 60L + (long)sc;

        tb.tm_yday = tmpdays;
        tb.tm_year = yr;
        tb.tm_mon = mo - 1;
        tb.tm_hour = hr;
        
        return (tmptim >= 0) ? (time_t)tmptim : (time_t)(-1);
}

time_t time(time_t *timer)
{
	time_t tim;
	sceCdCLOCK clocktime; /* defined in libcdvd.h */

	sceCdReadClock(&clocktime); /* libcdvd.a */

	tim =   _gmtotime_t ( DEC(clocktime.year)+2000,
							DEC(clocktime.month),
							DEC(clocktime.day),
							DEC(clocktime.hour),
							DEC(clocktime.minute),
							DEC(clocktime.second));

	if(timer)
		*timer = tim;
		
	return tim;
}

#endif /* _PS2 */

gsi_time current_time()  //returns current time in milliseconds
{ 
#ifdef _WIN32
	return (GetTickCount()); 
#endif

#ifdef _PS2
	unsigned int ticks;
	static unsigned int msec = 0;
	static unsigned int lastticks = 0;
	sceCdCLOCK lasttimecalled; /* defined in libcdvd.h */

	if(!msec)
	{
		sceCdReadClock(&lasttimecalled); /* libcdvd.a */
		msec =  (unsigned int)(DEC(lasttimecalled.day) * 86400000) +
				(unsigned int)(DEC(lasttimecalled.hour) * 3600000) +
				(unsigned int)(DEC(lasttimecalled.minute) * 60000) +
				(unsigned int)(DEC(lasttimecalled.second) * 1000);
	}

	ticks = (unsigned int)GetTicks();
	if(lastticks > ticks)
		msec += (unsigned int)((sizeof(unsigned int) - lastticks) + ticks) / 300000;
	else
		msec += (unsigned int)(ticks-lastticks) / 300000;
	lastticks = ticks;

	return msec;
#endif

#ifdef _UNIX
	struct timeval time;
	
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
}

gsi_time current_time_hires()  // returns current time in microseconds
{
#ifdef _WIN32
#if (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
	static LARGE_INTEGER counterFrequency;
	static BOOL haveCounterFrequency = FALSE;
	static BOOL haveCounter = FALSE;
	LARGE_INTEGER count;

	if(!haveCounterFrequency)
	{
		haveCounter = QueryPerformanceFrequency(&counterFrequency);
		haveCounterFrequency = TRUE;
	}

	if(haveCounter)
	{
		if(QueryPerformanceCounter(&count))
		{
			return (gsi_time)(count.QuadPart * 1000000 / counterFrequency.QuadPart);
		}
	}
#endif
	
	return (current_time() * 1000);
#endif

#ifdef _PS2
	unsigned int ticks;
	static unsigned int msec = 0;
	static unsigned int lastticks = 0;
	sceCdCLOCK lasttimecalled; /* defined in libcdvd.h */

	if(!msec)
	{
		sceCdReadClock(&lasttimecalled); /* libcdvd.a */
		msec =  (unsigned int)(DEC(lasttimecalled.day) * 86400000) +
				(unsigned int)(DEC(lasttimecalled.hour) * 3600000) +
				(unsigned int)(DEC(lasttimecalled.minute) * 60000) +
				(unsigned int)(DEC(lasttimecalled.second) * 1000);
		msec *= 1000;
	}

	ticks = (unsigned int)GetTicks();
	if(lastticks > ticks)
		msec += ((sizeof(unsigned int) - lastticks) + ticks) / 300;
	else
		msec += (unsigned int)(ticks-lastticks) / 300;
	lastticks = ticks;

	return msec;
#endif

#ifdef _UNIX
	struct timeval time;
	
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000000 + time.tv_usec);
#endif
}


void msleep(gsi_time msec)
{
#ifdef _WIN32
	Sleep(msec);
#endif

#ifdef _PS2
	#ifdef SN_SYSTEMS
		sn_delay((int)msec);
	#endif
	#ifdef EENET
		if(msec >= 1000)
		{
			sleep(msec / 1000);
			msec -= (msec / 1000);
		}
		if(msec)
			usleep(msec * 1000);
	#endif
	#ifdef INSOCK
		DelayThread(msec * 1000);
	#endif
#endif

#ifdef _UNIX
	usleep(msec * 1000);
#endif
}

void SocketStartUp()
{
#if defined(_WIN32)
	WSADATA data;
	// Added by Saad Nader on 08-02-2004
	// support for winsock2
	#ifdef __WINSOCK_2__
		WSAStartup(MAKEWORD(2,2), data);
	#else
		WSAStartup(0x0101, &data);
	#endif
	// end added
#endif
}

void SocketShutDown()
{
#if defined(_WIN32)
	WSACleanup();
#endif
}

char * goastrdup(const char *src)
{
	char *res;
	if(src == NULL)      //PANTS|02.11.00|check for NULL before strlen
		return NULL;
	res = (char *)gsimalloc(strlen(src) + 1);
	if(res != NULL)      //PANTS|02.02.00|check for NULL before strcpy
		strcpy(res, src);
	return res;
}

#if !defined(_WIN32) || defined(UNDER_CE)

char *_strlwr(char *string)
{
	char *hold = string;
	while (*string)
	{
		*string = (char)tolower(*string);
		string++;
	}

	return hold;
}

char *_strupr(char *string)
{
	char *hold = string;
	while (*string)
	{
		*string = (char)toupper(*string);
		string++;
	}

	return hold;
}
#endif

int SetSockBlocking(SOCKET sock, int isblocking)
{
	int rcode;
#ifdef _PS2
	// EENet requires int
	// SNSystems requires int
	// Insock requires int
	gsi_i32 argp;
#else
	unsigned long argp;
#endif
	
	if(isblocking)
		argp = 0;
	else
		argp = 1;

#ifdef _PS2
	#ifdef SN_SYSTEMS
		rcode = setsockopt(sock, SOL_SOCKET, (isblocking) ? SO_BIO : SO_NBIO, &argp, sizeof(argp));
	#endif

	#ifdef EENET
		rcode = setsockopt(sock, SOL_SOCKET, SO_NBIO, &argp, sizeof(argp));
	#endif

	#ifdef INSOCK
		if (isblocking)
			argp = -1;
		else
			argp = 0;
		sceInsockSetRecvTimeout(sock, argp);
		sceInsockSetSendTimeout(sock, argp);
		sceInsockSetShutdownTimeout(sock, argp);
		GSI_UNUSED(sock);
		rcode = 0;
	#endif
#else
	rcode = ioctlsocket(sock, FIONBIO, &argp);
#endif

	if(rcode == 0)
		return 1;

	return 0;
}

int DisableNagle(SOCKET sock)
{
#if defined(_WIN32) || defined(_UNIX)
	int rcode;
	int noDelay = 1;

	rcode = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&noDelay, sizeof(int));

	return (rcode != SOCKET_ERROR);	
#else
	GSI_UNUSED(sock);

	// not supported
	return 0;
#endif // moved this to here to silence VC warning
}


#ifndef INSOCK
	int SetReceiveBufferSize(SOCKET sock, int size)
	{
		int rcode;
		rcode = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));
		return (rcode != SOCKET_ERROR);
	}

	int SetSendBufferSize(SOCKET sock, int size)
	{
		int rcode;
		rcode = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));
		return (rcode != SOCKET_ERROR);
	}

	int GetReceiveBufferSize(SOCKET sock)
	{
		int rcode;
		int size;
		int len;

		len = sizeof(size);
		rcode = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len);
		if(rcode == SOCKET_ERROR)
			return -1;

		return size;
	}

	int GetSendBufferSize(SOCKET sock)
	{
		int rcode;
		int size;
		int len;

		len = sizeof(size);
		rcode = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, &len);
		if(rcode == SOCKET_ERROR)
			return -1;

		return size;
	}
	
	// Formerly known as ghiSocketSelect
#ifdef SN_SYSTEMS
	#undef FD_SET
	#define FD_SET(s,p)   ((p)->array[((s) - 1) >> SN_FD_SHR] |= \
                       (unsigned int)(1 << (((s) - 1) & SN_FD_BITS)) )

#endif
	int GSISocketSelect(SOCKET theSocket, int* theReadFlag, int* theWriteFlag, int* theExceptFlag)
	{
		fd_set aReadSet;
		fd_set aWriteSet;
		fd_set aExceptSet;
		fd_set * aReadFds   = NULL;
		fd_set * aWriteFds  = NULL;
		fd_set * aExceptFds = NULL;
		int aResult;
		struct timeval aTimeout = { 0, 0 };
		char	TmpTest[1024];

		assert(theSocket != INVALID_SOCKET);

		// Setup the parameters.
		////////////////////////
		if(theReadFlag != NULL)
		{
			FD_ZERO(&aReadSet);
			FD_SET(theSocket,&aReadSet);
			aReadFds = &aReadSet;
		}
		if(theWriteFlag != NULL)
		{
			FD_ZERO(&aWriteSet);
			FD_SET(theSocket, &aWriteSet);
			aWriteFds = &aWriteSet;
		}
		if(theExceptFlag != NULL)
		{
			FD_ZERO(&aExceptSet);
			FD_SET(theSocket, &aExceptSet);
			aExceptFds = &aExceptSet;
		}
		
		// Perform the select
		aResult = select(FD_SETSIZE, aReadFds, aWriteFds, aExceptFds, &aTimeout);
		if(aResult == SOCKET_ERROR)
		{
			OutputDebugString("SOCKET_ERROR");
			return SOCKET_ERROR;
		}

		// Check results.
		/////////////////
		if(theReadFlag != NULL)
		{
//			OutputDebugString("Read Result - ");
//			sprintf(TmpTest, "%d\n", aResult);
//			OutputDebugString(TmpTest);

			if((aResult > 0) && FD_ISSET(theSocket, aReadFds))
				*theReadFlag = 1;
			else
				*theReadFlag = 0;
		}
		if(theWriteFlag != NULL)
		{
			OutputDebugString("Write \n");
			if((aResult > 0) && FD_ISSET(theSocket, aWriteFds))
				*theWriteFlag = 1;
			else
				*theWriteFlag = 0;
		}
		if(theExceptFlag != NULL)
		{
			OutputDebugString("Exept \n");
			if((aResult > 0) && FD_ISSET(theSocket, aExceptFds))
				*theExceptFlag = 1;
			else
				*theExceptFlag = 0;
		}
		return aResult; // 0 or 1 at this point
	}
	
#else // INSOCK BELOW
	#define INSOCK_MAX_UDP_BUFSIZE 8000000  // default max 
	#define INSOCK_MAX_TCP_BUFSIZE 32000

	extern sceSifMClientData gGSIInsockClientData;
	extern u_int             gGSIInsockSocketBuffer[NETBUFSIZE] __attribute__((aligned(64)));

	// NOT FULLY IMPLEMENTED
	int SetReceiveBufferSize(SOCKET sock, int size)
	{return -1; GSI_UNUSED(sock); GSI_UNUSED(size); }

	// NOT FULLY IMPLEMENTED
	int SetSendBufferSize(SOCKET sock, int size)
	{return -1; GSI_UNUSED(sock); GSI_UNUSED(size); }
	
	int GetReceiveBufferSize(SOCKET sock)
	{return NETBUFSIZE; GSI_UNUSED(sock); }
		
	int GetSendBufferSize(SOCKET sock)
	{return NETBUFSIZE; GSI_UNUSED(sock); }

	// Poll socket for Send, Recv and Except
	int GSISocketSelect(SOCKET theSocket, int* theReadFlag, int* theWriteFlag, int* theExceptFlag)
	{
		int result = 0;
		sceInetPollFd_t aPollFdSet;

		// Init the flags to 0
		if ((theReadFlag   != NULL))
			*theReadFlag = 0;
		if ((theWriteFlag  != NULL))
			*theWriteFlag = 0;
		if ((theExceptFlag != NULL))
			*theExceptFlag = 0;

		// Setup the fd set
		aPollFdSet.cid     = theSocket;  // the socket
		aPollFdSet.events  = 0;          // events in
		aPollFdSet.revents = 0;          // events out

		if (theReadFlag   != NULL) aPollFdSet.events |= sceINET_POLLIN;
		if (theWriteFlag  != NULL) aPollFdSet.events |= sceINET_POLLOUT;
		if (theExceptFlag != NULL) aPollFdSet.events |= sceINET_POLLERR;

		// Poll the fds
		//    1 fds, 0 ms timeout
		result = sceInsockPoll(&aPollFdSet, 1, 0);
		if (result > 0)
		{
			// If the Flag is valid, set the return value
			if ((theReadFlag   != NULL))
				*theReadFlag    = (aPollFdSet.revents & sceINET_POLLIN)  ? 1:0;
			if ((theWriteFlag  != NULL))
				*theWriteFlag   = (aPollFdSet.revents & sceINET_POLLOUT) ? 1:0;
			if ((theExceptFlag != NULL))
				*theExceptFlag  = (aPollFdSet.revents & sceINET_POLLERR) ? 1:0;
		}
		return result;	
	}

#endif // END INSOCK socket platform switch


// Return 1 for immediate recv, otherwise 0
int CanReceiveOnSocket(SOCKET sock)
{
	int aReadFlag = 0;
	if (1 == GSISocketSelect(sock, &aReadFlag, NULL, NULL))
		return aReadFlag;

	// SDKs expect 0 on SOCKET_ERROR
	return 0;
}

// Return 1 for immediate send, otherwise 0
int CanSendOnSocket(SOCKET sock)
{
	int aWriteFlag = 0;
	if (1 == GSISocketSelect(sock, NULL, &aWriteFlag, NULL))
		return aWriteFlag;

	// SDKs expect 0 on SOCKET_ERROR
	return 0;
}

HOSTENT * getlocalhost(void)
{
#ifdef EENET
	#define MAX_IPS  5

	static HOSTENT localhost;
	static char * aliases = NULL;
	static char * ipPtrs[MAX_IPS + 1];
	static unsigned int ips[MAX_IPS];

	struct sceEENetIfname * interfaces;
	struct sceEENetIfname * interface;
	int num;
	int i;
	int count;
	int len;
	u_short flags;
	IN_ADDR address;

	// initialize the host
	localhost.h_name = "localhost";
	localhost.h_aliases = &aliases;
	localhost.h_addrtype = AF_INET;
	localhost.h_length = 0;
	localhost.h_addr_list = ipPtrs;

	// get the local interfaces
	sceEENetGetIfnames(NULL, &num);
	interfaces = (struct sceEENetIfname *)gsimalloc(num * sizeof(struct sceEENetIfname));
	if(!interfaces)
		return NULL;
	sceEENetGetIfnames(interfaces, &num);

	// loop through the interfaces
	count = 0;
	for(i = 0 ; i < num ; i++)
	{
		// the next interface
		interface = &interfaces[i];
		//printf("eenet%d: %s\n", i, interface->ifn_name);

		// get the flags
		len = sizeof(flags);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_IFFLAGS, &flags, &len) != 0)
			continue;
		//printf("eenet%d flags: 0x%X\n", i, flags);

		// check for up, running, and non-loopback
		if(!(flags & (IFF_UP|IFF_RUNNING)) || (flags & IFF_LOOPBACK))
			continue;
		//printf("eenet%d: up and running, non-loopback\n", i);

		// get the address
		len = sizeof(address);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_ADDR, &address, &len) != 0)
			continue;
		//printf("eenet%d: %s\n", i, inet_ntoa(address));

		// add this address
		ips[count] = address.s_addr;
		ipPtrs[count] = (char *)&ips[count];
		count++;
	}

	// free the interfaces
	gsifree(interfaces);

	// check that we got at least one IP
	if(!count)
		return NULL;

	// finish filling in the host struct
	localhost.h_length = (4 * count);
	ipPtrs[count] = NULL;

	return &localhost;

	////////////////////
	// INSOCK
#elif defined(INSOCK)
	// Global storage
	#define MAX_IPS  sceLIBNET_MAX_INTERFACE
	static HOSTENT   localhost;
	static char    * aliases = NULL;
	static char    * ipPtrs[MAX_IPS + 1];
	static unsigned int ips[MAX_IPS];

	// Temp storage
	int aInterfaceIdArray[MAX_IPS];
	int aNumInterfaces = 0;
	int aInterfaceNum = 0;
	int aCount = 0;
	
	// Get the list of interfaces
	aNumInterfaces = sceInetGetInterfaceList(&gGSIInsockClientData, 
		                 &gGSIInsockSocketBuffer, aInterfaceIdArray, MAX_IPS);
	if (aNumInterfaces < 1)
		return NULL;

	// initialize the HOSTENT
	localhost.h_name      = "localhost";
	localhost.h_aliases   = &aliases;
	localhost.h_addrtype  = AF_INET;
	localhost.h_addr_list = ipPtrs;

	// Look up each address and copy into the HOSTENT structure
	aCount = 0; // count of valid interfaces
	for (aInterfaceNum = 0; aInterfaceNum < aNumInterfaces; aInterfaceNum++)
	{
		sceInetAddress_t anAddr;
		int result = sceInetInterfaceControl(&gGSIInsockClientData, &gGSIInsockSocketBuffer,
			                    aInterfaceIdArray[aInterfaceNum], sceInetCC_GetAddress,
								&anAddr, sizeof(anAddr));
		if (result == 0)
		{
			// Add this interface to the array
			localhost.h_addr_list[aCount] = anAddr.data;

			memcpy(&ips[aCount], anAddr.data, sizeof(ips[aCount]));
			ipPtrs[aCount] = (char*)&ips[aCount];
			aCount++;
		}
	}

	// Set the final hostent data, then return
	localhost.h_length = (int)(sizeof(char*) * aNumInterfaces);
	ipPtrs[aCount]     = NULL;
	return &localhost;
	
#else
	char hostname[256] = "";

	// get the local host's name
	gethostname(hostname, sizeof(hostname));

	// return the host for that name
	return gethostbyname(hostname);
#endif
}

int IsPrivateIP(IN_ADDR * addr)
{
	int b1;
	int b2;
	unsigned int ip;

	// get the first 2 bytes
	ip = ntohl(addr->s_addr);
	b1 = (int)((ip >> 24) & 0xFF);
	b2 = (int)((ip >> 16) & 0xFF);

	// 10.X.X.X
	if(b1 == 10)
		return 1;

	// 172.16-31.X.X
	if((b1 == 172) && ((b2 >= 16) && (b2 <= 31)))
		return 1;

	// 192.168.X.X
	if((b1 == 192) && (b2 == 168))
		return 1;

	return 0;
}

#if defined(_WIN32)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
					/////////////  THREAD  /////////////  

void gsiInitializeCriticalSection(GSICriticalSection *theCrit) { InitializeCriticalSection(theCrit); }
void gsiEnterCriticalSection     (GSICriticalSection *theCrit) { EnterCriticalSection(theCrit);      }
void gsiLeaveCriticalSection     (GSICriticalSection *theCrit) { LeaveCriticalSection(theCrit);      }
void gsiDeleteCriticalSection    (GSICriticalSection *theCrit) { DeleteCriticalSection(theCrit);     }

gsi_u32 gsiHasThreadShutdown(GSIThreadID theThreadID) 
{ 
	DWORD result = WaitForSingleObject(theThreadID, 0); 
	if (result == WAIT_ABANDONED || result == WAIT_OBJECT_0)
		return 1; // thread is dead
	else
		return 0; // keep waiting
}

GSISemaphoreID gsiCreateSemaphore(gsi_i32 theInitialCount, gsi_i32 theMaxCount, char* theName)
{
	GSISemaphoreID aSemaphore = CreateSemaphore(NULL, theInitialCount, theMaxCount, theName);
	if (aSemaphore == NULL)
	{
		gsDebugFormat(GSIDebugCat_Common, GSIDebugType_Misc, GSIDebugLevel_WarmError,
			"Failed to create semaphore\r\n");
	}
	return aSemaphore;
}

// Waits for -- and signals -- the semaphore
gsi_u32 gsiWaitForSemaphore(GSISemaphoreID theSemaphore, gsi_u32 theTimeoutMs)
{
	DWORD result = WaitForSingleObject((HANDLE)theSemaphore, (DWORD)theTimeoutMs);
	return (gsi_u32)result;
}

// Allow other objects to access the semaphore
void gsiReleaseSemaphore(GSISemaphoreID theSemaphore, gsi_i32 theReleaseCount)
{
	ReleaseSemaphore(theSemaphore, theReleaseCount, NULL);
}

void gsiCloseSemaphore(GSISemaphoreID theSemaphore)
{
	CloseHandle(theSemaphore);
}


int gsiStartThread(GSThreadFunc func, gsi_u32 theStackSize, void *arg, GSIThreadID * id)
{
	HANDLE handle;
	DWORD threadID;

	// create the thread
	handle = CreateThread(NULL, theStackSize, func, arg, 0, &threadID);
	if(handle == NULL)
		return -1;

	// store the id
	*id = handle;

	return 0;
}

void gsiCancelThread(GSIThreadID id)
{
	//TODO: is TerminateThread causing lost resources?
	TerminateThread(id, 0);
}

void gsiCleanupThread(GSIThreadID id)
{
	GSI_UNUSED(id);
}
#elif defined(_PS2)


void gsiInitializeCriticalSection(GSICriticalSection *theCrit) 
{
	theCrit->mSemaphore = gsiCreateSemaphore(1, 1, NULL); 
	theCrit->mOwnerThread = 0;
	theCrit->mEntryCount = 0;
}
void gsiEnterCriticalSection(GSICriticalSection *theCrit) 
{ 
	// If we're not already in it, wait for it
	if (GetThreadId() != theCrit->mOwnerThread)
	{
		gsiWaitForSemaphore(theCrit->mSemaphore, 0);
		theCrit->mOwnerThread = GetThreadId();
	}

	// Increment entry count
	theCrit->mEntryCount++;
}
void gsiLeaveCriticalSection(GSICriticalSection *theCrit)
{ 
	// We must be the owner? (assert?)
	if (GetThreadId() != theCrit->mOwnerThread)
	{
		assert(GetThreadId() == theCrit->mOwnerThread);
		return;
	}

	// Release semaphore
	theCrit->mEntryCount--;
	if (theCrit->mEntryCount == 0)
	{
		theCrit->mOwnerThread = 0;
		gsiReleaseSemaphore(theCrit->mSemaphore, 1);     
	}
}

void gsiDeleteCriticalSection(GSICriticalSection *theCrit) 
{ 
	gsiCloseSemaphore(theCrit->mSemaphore);       
}

gsi_u32 gsiHasThreadShutdown(GSIThreadID theThreadID) 
{ 
	struct ThreadParam aStatus;
	ReferThreadStatus(theThreadID, &aStatus);
	if (aStatus.status == THS_DORMANT)
		return 1; // dead
	else
		return 0; // still kicking;
}

GSISemaphoreID gsiCreateSemaphore(gsi_i32 theInitialCount, gsi_i32 theMaxCount, char* theName)
{
	struct SemaParam aParam;
	int aSemaphore = 0;

	aParam.initCount = theInitialCount;
	aParam.maxCount = theMaxCount;
	
	aSemaphore = CreateSema(&aParam);
	if (aSemaphore < 0)
	{
		gsDebugFormat(GSIDebugCat_Common, GSIDebugType_Misc, GSIDebugLevel_WarmError,
			"Failed to create semaphore\r\n");
	}
	return aSemaphore;
}

gsi_u32 gsiWaitForSemaphore(GSISemaphoreID theSemaphore, gsi_u32 theTimeoutMs)
{
	int result = WaitSema(theSemaphore);
	return (gsi_u32)result;

	GSI_UNUSED(theTimeoutMs);
}

void gsiReleaseSemaphore(GSISemaphoreID theSemaphore, gsi_i32 theReleaseCount)
{
	while (theReleaseCount-- > 0)
		SignalSema(theSemaphore);
	//ReleaseSemaphore(theSemaphore, theReleaseCount, NULL);
}

void gsiCloseSemaphore(GSISemaphoreID theSemaphore)
{
	DeleteSema(theSemaphore);
}

int gsiStartThread(GSThreadFunc func,  gsi_u32 theStackSize, void *arg, GSIThreadID *id)
{
	const int stackSize = theStackSize;
	const int threadPriority = 3;
	struct ThreadParam param;
	void * stack;
	int threadID;

	// allocate a stack
	stack = gsimemalign(16, stackSize);
	if(!stack)
		return -1;

	// setup the thread parameters
	param.entry = func;
	param.stack = stack;
	param.stackSize = stackSize;
	param.gpReg = &_gp;
	param.initPriority = threadPriority;

	// create the thread
	threadID = CreateThread(&param);
	if(threadID == -1)
	{
		gsifree(stack);
		return -1;
	}

	// start the thread
	if(StartThread(threadID, arg) == -1)
	{
		DeleteThread(threadID);
		gsifree(stack);
		return -1;
	}

	// store the id
	*id = threadID;

	// Note:  This was added to prevent PS2 lockups when starting multiple threads
	//        The PS2 would block for approx 5 seconds
	msleep(1);

	return 0;
}


void gsiCancelThread(GSIThreadID id)
{
	void* aStack = NULL;

	// get the stack ptr
	struct ThreadParam aThreadParam;
	ReferThreadStatus(id, &aThreadParam);
	aStack = (void*)aThreadParam.stack;

	// terminate the thread
	TerminateThread(id);

	// delete the thread
	DeleteThread(id);

	//free the stack
	gsifree(aStack);
}

void gsiCleanupThread(GSIThreadID id)
{
	// same as cancel (terminates just to be sure)
	gsiCancelThread(id);
}
#endif

typedef struct GSIResolveHostnameInfo
{
	char * hostname;
	unsigned int ip;
#if defined(_WIN32) || defined(_PS2)
	int finishedResolving;
	GSIThreadID threadID;
#endif
} GSIResolveHostnameInfo;

#if defined(_WIN32) || defined(_PS2)
	#if defined(_WIN32)
DWORD WINAPI gsiResolveHostnameThread(void * arg)
	#elif defined(_PS2)
static void gsiResolveHostnameThread(void * arg)
	#endif
{
	HOSTENT * hostent;
	GSIResolveHostnameHandle handle = (GSIResolveHostnameHandle)arg;

	SocketStartUp();

	// do the gethostbyname
	hostent = gethostbyname(handle->hostname);
	if(hostent)
	{
		// got the ip
		handle->ip = *(unsigned int *)hostent->h_addr_list[0];
	}
	else
	{
		// didn't resolve
		handle->ip = GSI_ERROR_RESOLVING_HOSTNAME;
	}

	SocketShutDown();

	// finished resolving
	handle->finishedResolving = 1;

	#if defined(_WIN32)
	return 0;
	#endif
}

int gsiStartResolvingHostname(const char * hostname, GSIResolveHostnameHandle * handle)
{
	GSIResolveHostnameInfo * info;

	// allocate a handle
	info = (GSIResolveHostnameInfo *)gsimalloc(sizeof(GSIResolveHostnameInfo));
	if(!info)
		return -1;

	// make a copy of the hostname so the thread has access to it
	info->hostname = goastrdup(hostname);
	if(!info->hostname)
	{
		gsifree(info);
		return -1;
	}

	// not resolved yet
	info->finishedResolving = 0;

	// start the thread
	if(gsiStartThread(gsiResolveHostnameThread, (0x1000), info, &info->threadID) == -1)
	{
		gsifree(info->hostname);
		gsifree(info);
		return -1;
	}

	// set the handle to the info
	*handle = info;

	return 0;
}

void gsiCancelResolvingHostname(GSIResolveHostnameHandle handle)
{
	// cancel the thread
	gsiCancelThread(handle->threadID);

	gsifree(handle->hostname);
	gsifree(handle);
}

unsigned int gsiGetResolvedIP(GSIResolveHostnameHandle handle)
{
	unsigned int ip;

	// check if we haven't finished
	if(!handle->finishedResolving)
		return GSI_STILL_RESOLVING_HOSTNAME;

	// save the ip
	ip = handle->ip;

	// free resources
	gsiCleanupThread(handle->threadID);
	gsifree(handle->hostname);
	gsifree(handle);

	return ip;
}
#else
int gsiStartResolvingHostname(const char * hostname, GSIResolveHostnameHandle * handle)
{
	GSIResolveHostnameInfo * info;
	HOSTENT * hostent;

	// do the lookup now
	hostent = gethostbyname(hostname);
	if(hostent == NULL)
		return -1;

	// allocate info to store the result
	info = (GSIResolveHostnameHandle)gsimalloc(sizeof(GSIResolveHostnameInfo));
	if(!info)
		return -1;

	// we already have the ip
	info->ip = *(unsigned int *)hostent->h_addr_list[0];

	// set the handle to the info
	*handle = info;

	return 0;
}

void gsiCancelResolvingHostname(GSIResolveHostnameHandle handle)
{
	gsifree(handle);
}

unsigned int gsiGetResolvedIP(GSIResolveHostnameHandle handle)
{
	// we always to the resolve in the initial call for systems without
	// an async version, so we'll always have the IP at this point
	unsigned int ip = handle->ip;
	gsifree(handle);
	return ip;
}
#endif

#if defined(UNDER_CE)
int strcasecmp(const char *string1, const char *string2)
{
	while (tolower(*string1) == tolower(*string2) && *string1 != 0 && *string2 != 0)
	{
		*string1++; *string2++;
	}
	return tolower(*string1) - tolower(*string2);
}

int strncasecmp(const char *string1, const char *string2, size_t count)
{
	while (--count > 0 && tolower(*string1) == tolower(*string2) && *string1 != 0 && *string2 != 0)
	{
		*string1++; *string2++;
	}
	return tolower(*string1) - tolower(*string2);
}
#endif

#ifdef SN_SYSTEMS
int GOAGetLastError(SOCKET s)
{
	int val = 0;
	int soval = sizeof(val);
	if (0 != getsockopt(s,SOL_SOCKET,SO_ERROR,&val,&soval))
		return 0; // getsockopt failed
	else
		return val;
}
#endif

#ifdef UNDER_CE
const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[17];
	unsigned char buff[8];
	DWORD size;

	size = 0;
	for (size = 0 ; size < sizeof(buff) ; size++)
		buff[size] = 0;
	size = sizeof(buff);
	FirmwareGetSerialNumber(buff, &size);
	for (size = 0 ; size < sizeof(buff) ; size++)
	{
		sprintf(keyval + (size * 2),"%02x",buff[size]);
	}
	return keyval;
}
#endif //UNDER_CE


#ifdef _PS2
#ifdef UNIQUEID

#if defined(EENET)

#include <net/if_dl.h>
// Removed due to updated sony libraries,  Saad Nader
//#include <net/if_types.h>
#include <net/if_ether.h>

static const char * GetMAC(void)
{
	static struct sceEENetEtherAddr linkAddress;
	struct sceEENetIfname * interfaces;
	struct sceEENetIfname * interface;
	int num;
	int type;
	int len;
	int i;
	const unsigned char * MAC = NULL;

	// get the local interfaces
	sceEENetGetIfnames(NULL, &num);
	interfaces = (struct sceEENetIfname *)gsimalloc(num * sizeof(struct sceEENetIfname));
	if(!interfaces)
		return NULL;
	sceEENetGetIfnames(interfaces, &num);

	// loop through the interfaces
	for(i = 0 ; i < num ; i++)
	{
		// the next interface
		interface = &interfaces[i];
		//printf("eenet%d: %s\n", i, interface->ifn_name);

		// get the type
		len = sizeof(type);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_IFTYPE, &type, &len) != 0)
			continue;
		//printf("eenet%d type: %d\n", i, type);

		// check for ethernet
		if(type != sceEENET_IFTYPE_ETHER)
			continue;
		//printf("eenet%d: ethernet\n", i);

		// get the address
		len = sizeof(linkAddress);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_MACADDR, &linkAddress, &len) != 0)
			continue;
		MAC = linkAddress.ether_addr_octet;
		//printf("eenet%d: MAC: %02X-%02X-%02X-%02X-%02X-%02X\n", i, MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);

		break;
	}

	// free the interfaces
	gsifree(interfaces);

	return MAC;
}

#elif defined(SN_SYSTEMS)

	static const char * GetMAC(void)
	{
		static char MAC[6];
		int len = sizeof(MAC);
		int rcode;

		// get the MAC
		rcode = sndev_get_status(0, SN_DEV_STAT_MAC, MAC, &len);
		if((rcode != 0) || (len != 6))
			return NULL;

		return MAC;
	}

#elif defined(INSOCK)

	static const char * GetMAC(void)
	{
		//sceInetInterfaceControl(sceInetCC_GetHWaddr)
		static char MAC[36];
		int result = sceInetInterfaceControl(&gGSIInsockClientData, &gGSIInsockSocketBuffer,
			                                 1, sceInetCC_GetHWaddr, MAC, sizeof(MAC));
		if (result == sceINETE_OK)
			return MAC;

		// error
		return NULL;
	}

#endif

static const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[17];
	const char * MAC;

	// check if we already have the Unique ID
	if(keyval[0])
		return keyval;

	// get the MAC
	MAC = GetMAC();
	if(!MAC)
	{
		// error getting the MAC
		static char errorMAC[6] = { 1, 2, 3, 4, 5, 6 };
		MAC = errorMAC;
	}

	// format it
	sprintf(keyval, "%02X%02X%02X%02X%02X%02X0000",
		MAC[0] & 0xFF,
		MAC[1] & 0xFF,
		MAC[2] & 0xFF,
		MAC[3] & 0xFF,
		MAC[4] & 0xFF,
		MAC[5] & 0xFF);

	return keyval;
}

#endif // UNIQUEID
#endif // _PS2

#if (defined(_WIN32) || defined(_UNIX)) && !defined(UNDER_CE)

static void GenerateID(char *keyval)
{
	int i;
	const char crypttab[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
#ifdef _WIN32
	LARGE_INTEGER l1;
	UINT seed;
	if (QueryPerformanceCounter(&l1))
		seed = (l1.LowPart ^ l1.HighPart);
	else
		seed = 0;
	Util_RandSeed(seed ^ GetTickCount() ^ time(NULL) ^ clock());
#else
	Util_RandSeed(time(NULL) ^ clock());
#endif
	for (i = 0; i < 19; i++)
		if (i == 4 || i == 9 || i == 14)
			keyval[i] = '-';
	else
		keyval[i] = crypttab[Util_RandInt(0, 62)];
	keyval[19] = 0;
}

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifdef _WIN32
#define REG_KEY	  "Software\\GameSpy\\GameSpy 3D\\Registration"
#endif

const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[PATH_MAX] = "";
	unsigned int ret;

#ifdef _WIN32
	int docreate;
	HKEY thekey;
	DWORD thetype = REG_SZ;
	DWORD len = MAX_PATH;
	DWORD disp;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY, 0, KEY_ALL_ACCESS, &thekey) != ERROR_SUCCESS)
		docreate = 1;
	else
		docreate = 0;
	ret = RegQueryValueExA(thekey, (LPCSTR)"Crypt", 0, &thetype, (LPBYTE)keyval, &len);
#else
	FILE *f;
	f = fopen("id.bin","r");
	if (!f)
		ret = 0;
	else
	{
		ret = fread(keyval,1,19,f);
		keyval[ret] = 0;
		fclose(f);
	}
#endif

	if (ret != 0 || strlen(keyval) != 19)//need to generate a new key
	{
		GenerateID(keyval);
#ifdef _WIN32
		if (docreate)
		{
			ret = RegCreateKeyExA(HKEY_CURRENT_USER, REG_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &thekey, &disp);
		}
		RegSetValueExA(thekey, (LPCSTR)"Crypt", 0, REG_SZ, (const LPBYTE)keyval, strlen(keyval)+1);
#else
		f = fopen("id.bin","w");
		if (f)
		{
			fwrite(keyval,1,19,f);
			fclose(f);
		} else
			keyval[0] = 0; //don't generate one each time!!
#endif
	}

#ifdef _WIN32
	RegCloseKey(thekey);
#endif

	// Strip out the -'s.
	/////////////////////
	memmove(keyval + 4, keyval + 5, 4);
	memmove(keyval + 8, keyval + 10, 4);
	memmove(keyval + 12, keyval + 15, 4);
	keyval[16] = '\0';
	
	return keyval;
}

#endif

#if !defined(_PS2) || defined(UNIQUEID)
GetUniqueIDFunction GOAGetUniqueID = GOAGetUniqueID_Internal;
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Cross platform random number generator
#define RANa 16807                 // multiplier
#define LONGRAND_MAX 2147483647L   // 2**31 - 1

static long randomnum = 1;

static long nextlongrand(long seed)
{
	unsigned

	long lo, hi;
	lo = RANa *(unsigned long)(seed & 0xFFFF);
	hi = RANa *((unsigned long)seed >> 16);
	lo += (hi & 0x7FFF) << 16;

	if (lo > LONGRAND_MAX)
	{
		lo &= LONGRAND_MAX;
		++lo;
	}
	lo += hi >> 15;

	if (lo > LONGRAND_MAX)
	{
		lo &= LONGRAND_MAX;
		++lo;
	}

	return(long)lo;
}

// return next random long
static long longrand(void)
{
	randomnum = nextlongrand(randomnum);
	return randomnum;
}

// to seed it
void Util_RandSeed(unsigned long seed)
{
	// nonzero seed
	randomnum = seed ? (long)(seed & LONGRAND_MAX) : 1;
}

int Util_RandInt(int low, int high)
{
	int range = high-low;
	int num;
	
	if (range == 0)
		return (low); // Prevent divide by zero

	num = (int)(longrand() % range);

	return(num + low);
}


/*****************************
UNICODE ENCODING
******************************/

static void QuartToTrip(char *quart, char *trip, int inlen)
{
	if (inlen >= 2)
		trip[0] = (char)(quart[0] << 2 | quart[1] >> 4);
	if (inlen >= 3)
		trip[1] = (char)((quart[1] & 0x0F) << 4 | quart[2] >> 2);
	if (inlen >= 4)
		trip[2] = (char)((quart[2] & 0x3) << 6 | quart[3]);
}

static void TripToQuart(const char *trip, char *quart, int inlen)
{
	unsigned char triptemp[3];
	int i;
	for (i = 0; i < inlen ; i++)
	{
		triptemp[i] = (unsigned char)trip[i];
	}
	while (i < 3) //fill the rest with 0
	{
		triptemp[i] = 0;
		i++;
	}
	quart[0] = (char)(triptemp[0] >> 2);
	quart[1] = (char)(((triptemp[0] & 3) << 4) | (triptemp[1] >> 4));
	quart[2] = (char)((triptemp[1] & 0x0F) << 2 | (triptemp[2] >> 6));
	quart[3] = (char)(triptemp[2] & 0x3F);

}

const char defaultEncoding[] = {'+','/','='};
const char alternateEncoding[] = {'[',']','_'};
const char urlSafeEncodeing[] = {'-','_','='};

void B64Decode(char *input, char *output, int *len, int encodingType)
{
	const char *encoding;
	char *holdin = input;
	int outlen = -1;
	int inlen = (int)strlen(input);

	// 10-31-2004 : Added by Saad Nader
	// now supports URL safe encoding
	////////////////////////////////////////////////
	switch(encodingType)
	{	
		case 1: 
			encoding = alternateEncoding;
			break;
		case 2:
			encoding = urlSafeEncodeing;
			break;
		default: encoding = defaultEncoding;
	}
	
	while (*input)
	{
		if (*input == encoding[2])
		{
			*input = 0;
			if (outlen == -1)
				outlen = (input - holdin) / 4 * 3 + (input - holdin - 1) % 4;
		}
		else if (*input >= '0' && *input <= '9')
			*input = (char)(*input - 48 + 52);
		else if (*input >= 'a' && *input <= 'z')
			*input = (char)(*input - 71);
		else if (*input >= 'A' && *input <= 'Z')
			*input = (char)(*input - 65);
		else if (*input == encoding[0])
			*input = 62;
		else if (*input == encoding[1])
			*input = 63;
		else 
		{
		//	(assert(0)); //bad input data
			if (len)
				*len = 0;
			return; //invaid data

		}
		input++;
	}
	if (outlen == -1)
		outlen = (input - holdin) / 4 * 3;
	input = holdin;
	while (inlen - (input - holdin) > 0)
	{
		QuartToTrip(input, output, inlen - (input - holdin));
		input += 4;
		output += 3;
	}
	if (len)
		*len = outlen;

}

void B64Encode(const char *input, char *output, int inlen, int encodingType)
{
	const char *encoding;
	char *holdout = output;
	char *lastchar;
	int todo = inlen;
	
	// 10-31-2004 : Added by Saad Nader
	// now supports URL safe encoding
	////////////////////////////////////////////////
	switch(encodingType)
	{	
		case 1: 
			encoding = alternateEncoding;
			break;
		case 2:
			encoding = urlSafeEncodeing;
			break;
		default: encoding = defaultEncoding;
	}
	
//assume interval of 3
	while (todo > 0)
	{
		TripToQuart(input, output, inlen >= 3 ? 3 : inlen);
		output += 4;
		input += 3;
		todo -= 3;
	}
	lastchar = output;
	if (inlen % 3 == 1)
		lastchar -= 2;
	else if (inlen % 3 == 2)
		lastchar -= 1;
	*output = 0; //null terminate!
	while (output > holdout)
	{
		output--;
		if (output >= lastchar) //pad the end
			*output = encoding[2];
		else if (*output <= 25)
			*output = (char)(*output + 65);
		else if (*output <= 51)
			*output = (char)(*output + 71);
		else if (*output <= 61)
			*output = (char)(*output + 48 - 52);
		else if (*output == 62)
			*output = encoding[0];
		else if (*output == 63)
			*output = encoding[1];
	} 
}

// Re-enable previously disabled compiler warnings
///////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning ( default: 4127 )
#endif // _MSC_VER

void gsiPadRight(char *cArray, char padChar, int cLength);
char * gsiXxteaAlg(const char *sIn, int nIn, char key[XXTEA_KEY_SIZE], int bEnc, int *nOut);

void gsiPadRight(char *cArray, char padChar, int cLength)
{
	int diff;
	int length = strlen(cArray);
	
	diff = cLength - length;
	memset(&cArray[length], padChar, diff);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// The heart of the XXTEA encryption/decryption algorithm.
//
// sIn:  Input stream.
// nIn:  Input length (bytes).
// key:  Key (only first 128 bits are significant).
// bEnc: Encrypt (else decrypt)?
char * gsiXxteaAlg(const char *sIn, int nIn, char key[XXTEA_KEY_SIZE], int bEnc, int *nOut)
{
	int	i, p, n1;
	unsigned int *k, *v, z, y;
	char *oStr = NULL, *pStr = NULL;
	char *sIn2 = NULL;
	/////////////////////////////////
	// ERROR CHECK!
	if (!sIn || !key[0] || nIn == 0)
		return NULL;
	
	// Convert stream length to a round number of 32-bit words
	// Convert byte	count to 32-bit	word count
	nIn	= (nIn + 3)/4;
	if ( nIn <=	1 )		// XXTEA requires at least 64 bits
		nIn	= 2;

	// Load	and	zero-pad first 16 characters (128 bits)	of key
	gsiPadRight( key , '\0', XXTEA_KEY_SIZE);
	k = (unsigned int *)key;

	// Load and zero-pad entire input stream as 32-bit words
	sIn2 = gsimalloc(4 * nIn);
	strcpy(sIn2, sIn);
	gsiPadRight( sIn2, '\0', 4*nIn);
	v = (unsigned int *)sIn2;

	// Prepare to encrypt or decrypt
	n1 = nIn - 1;
	z = v[ n1 ];
	y = v[ 0 ];
	i = ( int )( 6 + 52/nIn );

	if (bEnc == 1)		// Encrypt
	{
		unsigned int sum = 0;
		while ( i-- != 0 ) 
		{
			int	e;
			sum += 0x9E3779B9;
			e = ( int )( sum >> 2 );
			for ( p = -1; ++p < nIn; ) 
			{
				y = v[( p < n1 ) ? p + 1 : 0 ];
				z = ( v[ p ] +=
					(	 (( z >> 5 ) ^ ( y << 2 ))
					+ (( y >> 3 ) ^ ( z << 4 )))
					^ (	 ( sum ^ y )
					+ ( k[( p ^ e ) & 3 ] ^ z )));
			}
		}
	}
	else if (bEnc == 0)			// Decrypt
	{
		unsigned int sum = ( unsigned int ) i * 0x9E3779B9;
		while ( sum != 0 ) 
		{
			int	e = ( int )( sum >> 2 );
			for ( p = nIn; p-- != 0; )
			{
				z = v[( p != 0 ) ? p - 1 : n1 ];
				y = ( v[ p ] -=
					(	 (( z >> 5 ) ^ ( y << 2 ))
					+ (( y >> 3 ) ^ ( z << 4 )))
					^ (	 ( sum ^ y )
					+ ( k[( p ^ e ) & 3 ] ^ z )));
			}
			sum -= 0x9E3779B9;
		}
	}
	else return NULL;
	// Convert result from 32-bit words to a byte stream
	
	
	oStr = gsimalloc(4 * nIn + 1);
	pStr = oStr;
	*nOut = 4 *nIn;
	for ( i = -1; ++i < nIn; ) 
	{
		unsigned int q = v[ i ];
		
		*pStr++ = (char)(q & 0xFF);
		*pStr++ = (char)(( q >>  8 ) & 0xFF);
		*pStr++ = (char)(( q >> 16 ) & 0xFF);
		*pStr++ = (char)(( q >> 24 ) & 0xFF);
	}
	*pStr = '\0';
	return oStr;
	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// XXTEA Encrpyt
// params
// iStr    : the input string to be encrypted
// iLength : the length of the input string
// key     : the key used to encrypt
char * gsXxteaEncrypt(const char * iStr, int iLength, char key[XXTEA_KEY_SIZE], int *oLength)
{
	return gsiXxteaAlg( iStr, iLength, key, 1, oLength );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// XXTEA Decrypt
// params
// iStr    : the input string to be decrypted
// iLength : the length of the input string
// key     : the key used to decrypt
char * gsXxteaDecrypt(const char * iStr, int iLength, char key[XXTEA_KEY_SIZE], int *oLength)
{
	return gsiXxteaAlg( iStr, iLength, key, 0, oLength);
}

#ifdef __cplusplus
}
#endif
