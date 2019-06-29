#pragma once
#pragma pack(push,1)

IC u32	net_flags	(BOOL bReliable=FALSE, BOOL bSequental=TRUE, BOOL bHighPriority=FALSE)
{
	return 
		(bReliable?DPNSEND_GUARANTEED:DPNSEND_NOCOMPLETE) | 
		(bSequental?0:DPNSEND_NONSEQUENTIAL) | 
		(bHighPriority?DPNSEND_PRIORITY_HIGH:0)
		;
}

struct	MSYS_CONFIG	// server 2 client - first packet ever, uncompressed
{
	u32			sign1;	// 0x12071980;
	u32			sign2;	// 0x26111975;
	u16			send	[256];
	u16			receive	[256];
};
struct	MSYS_PING
{
	u32			sign1;	// 0x12071980;
	u32			sign2;	// 0x26111975;
	u32			dwTime_ClientSend;
	u32			dwTime_Server;
	u32			dwTime_ClientReceive;
};

/*
CL_PING
{
	u32	dwTimeLocal;
}
*/

#pragma pack(pop)
