#include "stdafx.h"
#include "xrGameSpyServer.h"

#include "xrMessages.h"
/////////////////////// QR2 ///////////////////////////////////////
void			xrGameSpyServer::QR2_Init			()
{	
	if (!m_QR2.Init(m_iReportToMasterServer, this)) return;
	m_bQR2_Initialized = TRUE;
};

void			xrGameSpyServer::QR2_ShutDown()
{
	m_bQR2_Initialized = FALSE;
	m_QR2.ShutDown(NULL);
};

//------------------------------- CD_Key -----------------------------

void			xrGameSpyServer::CDKey_Init			()
{
	m_GCDServer.Init();	
	if (!m_GCDServer.Init()) return;
	m_bCDKey_Initialized = TRUE;
};

void			xrGameSpyServer::CDKey_ShutDown()
{
	m_GCDServer.ShutDown();
	m_bCDKey_Initialized = FALSE;
};

//generate a rand nchar challenge

void			xrGameSpyServer::SendChallengeString_2_Client (IClient* C)
{
	if (!C) return;
	xrGameSpyClientData* pClient = (xrGameSpyClientData*) C;

    m_GCDServer.CreateRandomChallenge(pClient->m_pChallengeString, 8);
	//--------- Send Respond ---------------------------------------------
	NET_Packet P;

	P.w_begin	(M_GAMESPY_CDKEY_VALIDATION_CHALLENGE);
	P.w_stringZ(pClient->m_pChallengeString);
	SendTo(pClient->ID, P);
}

void			xrGameSpyServer::OnCDKey_Validation				(int LocalID, int res, char* errormsg)
{
	ClientID ID; ID.set(u32(LocalID));
	xrGameSpyClientData* CL = (xrGameSpyClientData*)  ID_to_client(ID);
	if (0 != res)
	{
		Msg("GameSpy::CDKey: Validation successful - <%s>", errormsg);
		Check_GameSpy_CDKey_Success(CL);
	}
	else
	{
		Msg						("GameSpy::CDKey: Validation failed - <%s>", errormsg);
		SendConnectResult		(CL, u8(res), errormsg);
	}
};
