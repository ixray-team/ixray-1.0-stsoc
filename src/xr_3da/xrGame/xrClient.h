#pragma once

class xrClient :	public IPureClient
{
public:
	virtual		void	net_OnDisconnect ();

	xrClient();
	virtual ~xrClient();
};
