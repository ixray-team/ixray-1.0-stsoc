#include "stdafx.h"
#include "../resourcemanager.h"
#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "level.h"
#include "../x_ray.h"
#include "../igame_persistent.h"
#include "PhysicsGamePars.h"
#include "ai_space.h"

extern	pureFrame*				g_pNetProcessor;

BOOL CLevel::net_Start_client	( LPCSTR options )
{
/*
	pApp->LoadBegin	();

	// name_of_server
	string64					name_of_server;
	strcpy						(name_of_server,options);
	if (strchr(name_of_server,'/'))	*strchr(name_of_server,'/') = 0;

	// Startup client
	string256					temp;
	sprintf						(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle				(temp);

	if (Connect2Server(options)) {
		
		LPCSTR					level_name = ai().get_alife() ? *name() : net_SessionName	();
		// Determine internal level-ID
		int						level_id = pApp->Level_ID(level_name);
		if (level_id<0)	{
			Disconnect			();
			pApp->LoadEnd		();
			return FALSE;
		}
		pApp->Level_Set			(level_id);

		// Load level
		R_ASSERT2							(Load(level_id),"Loading failed.");

		// Begin spawn
		pApp->LoadTitle						("CLIENT: Spawning...");

		// Send physics to single or multithreaded mode
		LoadPhysicsGameParams				();
		ph_world							= xr_new<CPHWorld>();
		ph_world->Create					();

		// Send network to single or multithreaded mode
		// *note: release version always has "mt_*" enabled
		Device.seqFrameMT.Remove			(g_pNetProcessor);
		Device.seqFrame.Remove				(g_pNetProcessor);
		if (psDeviceFlags.test(mtNetwork))	Device.seqFrameMT.Add	(g_pNetProcessor,REG_PRIORITY_HIGH	+ 2);
		else								Device.seqFrame.Add		(g_pNetProcessor,REG_PRIORITY_LOW	- 2);

		// Waiting for connection/configuration completition
		CTimer	timer_sync	;	timer_sync.Start	();
		while	(!net_isCompleted_Connect())	Sleep	(5);
		Msg		("* connection sync: %d ms", timer_sync.GetElapsed_ms());
		while	(!net_isCompleted_Sync())	{ ClientReceive(); Sleep(5); }
		while	(!game_configured)			{ ClientReceive(); Sleep(5); }
		
		// HUD
		pHUD->Load							();

		// Textures
		if	(!g_pGamePersistent->bDedicatedServer)
		{
			pApp->LoadTitle						("Loading textures...");
			Device.Resources->DeferredLoad		(FALSE);
			Device.Resources->DeferredUpload	();
			LL_CheckTextures					();
		}

		// Sync
		pApp->LoadTitle						("CLIENT: Synchronising...");
		Device.PreCache						(30);

		// if (strstr(Path.Current,"escape"))	Engine.Event.Signal	("level.weather.rain.start");

		//-----------------------------------------------------------------------
		if (!pStatGraph && OnServer())
		{
//			pStatGraph = xr_new<CStatGraph>();
//			pStatGraph->SetRect(0, 0, 1024, 100, 0xff000000, 0xff000000);

//			pStatGraph->SetMinMax(-0.0f, 100.0f, 300);
//			pStatGraph->SetGrid(0, 0.0f, 10, 10.0f, 0xff808080, 0xffffffff);

//			pStatGraph->SetStyle(CStatGraph::stBar);
//			pStatGraph->AppendSubGraph(CStatGraph::stBar);
//			pStatGraph->AppendSubGraph(CStatGraph::stBar);
			
		};
		//-----------------------------------------------------------------------

		// signal OK
		pApp->LoadEnd	();
		return TRUE;
	}

	pApp->LoadEnd				(); 
*/
	
	return FALSE;
}

bool	CLevel::net_start_client1				()
{
	pApp->LoadBegin	();

	// name_of_server
	string64					name_of_server;
	strcpy						(name_of_server,*m_caClientOptions);
	if (strchr(name_of_server,'/'))	*strchr(name_of_server,'/') = 0;

	// Startup client
	string256					temp;
	sprintf						(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle				(temp);
	return true;
}

bool	CLevel::net_start_client2				()
{
	connected_to_server = Connect2Server(*m_caClientOptions);
	return true;
}

bool	CLevel::net_start_client3				()
{
	if(connected_to_server){
		LPCSTR					level_name = ai().get_alife() ? *name() : net_SessionName	();
		// Determine internal level-ID
		int						level_id = pApp->Level_ID(level_name);
		if (level_id<0)	{
			Disconnect			();
			pApp->LoadEnd		();
			connected_to_server = FALSE;
			return false;
		}
		pApp->Level_Set			(level_id);
		m_name					= level_name;
		// Load level
		R_ASSERT2				(Load(level_id),"Loading failed.");

	}
	return true;
}

bool	CLevel::net_start_client4				()
{
	if(connected_to_server){
		// Begin spawn
		pApp->LoadTitle						("CLIENT: Spawning...");

		// Send physics to single or multithreaded mode
		LoadPhysicsGameParams				();
		ph_world							= xr_new<CPHWorld>();
		ph_world->Create					();

		// Send network to single or multithreaded mode
		// *note: release version always has "mt_*" enabled
		Device.seqFrameMT.Remove			(g_pNetProcessor);
		Device.seqFrame.Remove				(g_pNetProcessor);
		if (psDeviceFlags.test(mtNetwork))	Device.seqFrameMT.Add	(g_pNetProcessor,REG_PRIORITY_HIGH	+ 2);
		else								Device.seqFrame.Add		(g_pNetProcessor,REG_PRIORITY_LOW	- 2);

		// Waiting for connection/configuration completition
		CTimer	timer_sync	;	timer_sync.Start	();
		while	(!net_isCompleted_Connect())	Sleep	(5);
		Msg		("* connection sync: %d ms", timer_sync.GetElapsed_ms());
		while	(!net_isCompleted_Sync())	{ ClientReceive(); Sleep(5); }
		while	(!game_configured)			{ ClientReceive(); Sleep(5); }
	}
	return true;
}

bool	CLevel::net_start_client5				()
{
	if(connected_to_server){
		// HUD
		pHUD->Load							();

		// Textures
		if	(!g_pGamePersistent->bDedicatedServer)
		{
			pApp->LoadTitle						("Loading textures...");
			Device.Resources->DeferredLoad		(FALSE);
			Device.Resources->DeferredUpload	();
			LL_CheckTextures					();
		}
	}
	return true;
}

bool	CLevel::net_start_client6				()
{
	if(connected_to_server){
		// Sync
		pApp->LoadTitle				("CLIENT: Synchronising...");
		Device.PreCache				(30);
		net_start_result_total		= TRUE;
	}else{
		net_start_result_total		= FALSE;
	}

	pApp->LoadEnd					(); 
/*
	if (!pStatGraphR)
	{
		pStatGraphR = xr_new<CStatGraph>();
		pStatGraphR->SetRect(0, 0, Device.dwWidth, 100, 0xff000000, 0xff000000);

		pStatGraphR->SetMinMax(-0.0f, 100.0f, Device.dwWidth/4);
		pStatGraphR->SetGrid(0, 0.0f, 10, 10.0f, 0xff808080, 0xffffffff);

		pStatGraphR->SetStyle(CStatGraph::stBar);
		pStatGraphR->AppendSubGraph(CStatGraph::stBar);
		pStatGraphR->AppendSubGraph(CStatGraph::stBarLine);
	};

	if (!pStatGraphS)
	{
		pStatGraphS = xr_new<CStatGraph>();
		pStatGraphS->SetRect(0, 110, Device.dwWidth, 100, 0xff000000, 0xff000000);

		pStatGraphS->SetMinMax(-0.0f, 100.0f, Device.dwWidth/4);
		pStatGraphS->SetGrid(0, 0.0f, 10, 10.0f, 0xff808080, 0xffffffff);

		pStatGraphS->SetStyle(CStatGraph::stBar);
		pStatGraphS->AppendSubGraph(CStatGraph::stBar);
		pStatGraphS->AppendSubGraph(CStatGraph::stBarLine);
	};
*/
	return true;
}