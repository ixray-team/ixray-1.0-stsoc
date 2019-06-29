//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xr_input.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "std_classes.h"
#include "GameFont.h"
#include "resource.h"
#include "LightAnimLibrary.h"
#include "ispatial.h"
#include "CopyProtection.h"
#include "Text_Console.h"
#include <process.h>
//---------------------------------------------------------------------
BOOL	g_bIntroFinished			= FALSE;
extern	void	Intro				( void* fn );
extern	void	Intro_DSHOW			( void* fn );
extern	int PASCAL IntroDSHOW_wnd	(HINSTANCE hInstC, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow);
int		max_load_stage = 0;
//---------------------------------------------------------------------
// 2446363
// umbt@ukr.net
//////////////////////////////////////////////////////////////////////////
struct _SoundProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		//Msg							("------------- sound: %d [%3.2f,%3.2f,%3.2f]",u32(Device.dwFrame),VPUSH(Device.vCameraPosition));
		Device.Statistic->Sound.Begin();
		::Sound->update				(Device.vCameraPosition,Device.vCameraDirection,Device.vCameraTop);
		Device.Statistic->Sound.End	();
	}
}	SoundProcessor;

//////////////////////////////////////////////////////////////////////////
// global variables
ENGINE_API	CApplication*	pApp			= NULL;
static		HWND			logoWindow		= NULL;

			int				doLauncher		();
			void			doBenchmark		();
ENGINE_API	bool			g_bBenchmark	= false;

ENGINE_API	string512		g_sLaunchOnExit_params;
ENGINE_API	string512		g_sLaunchOnExit_app;
// -------------------------------------------
// startup point
void InitEngine		()
{
	Engine.Initialize			( );
	while (!g_bIntroFinished)	Sleep	(100);
	Device.Initialize			( );
	CheckCopyProtection			( );
}

void InitSettings	()
{
	string_path					fname; 
	FS.update_path				(fname,"$game_config$","system.ltx");
	pSettings					= xr_new<CInifile>	(fname,TRUE);
}
void InitConsole	()
{
//	if (strstr(Core.Params, "-dedicated") && !strstr(Core.Params, "-notextconsole"))
#ifdef DEDICATED_SERVER
	{
		Console						= xr_new<CTextConsole>	();		
	}
#else
	//	else
	{
		Console						= xr_new<CConsole>	();
	}
#endif
	Console->Initialize			( );

	strcpy						(Console->ConfigFile,"user.ltx");
	if (strstr(Core.Params,"-ltx ")) {
		string64				c_name;
		sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
		strcpy					(Console->ConfigFile,c_name);
	}
//	if (!FS.exist(Console->ConfigFile))
//		strcpy					(Console->ConfigFile,"user.ltx");

}

void InitInput		()
{
	BOOL bCaptureInput			= !strstr(Core.Params,"-i");

	pInput						= xr_new<CInput>		(bCaptureInput);
}
void destroyInput	()
{
	xr_delete					( pInput		);
}
void InitSound		()
{
	CSound_manager_interface::_create					(u64(Device.m_hWnd));
//	Msg				("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
//	ref_sound*	x	= 
}
void destroySound	()
{
	CSound_manager_interface::_destroy				( );
}
void destroySettings()
{
	xr_delete					( pSettings		);
}
void destroyConsole	()
{
	Console->Destroy			( );
	xr_delete					(Console);
}
void destroyEngine	()
{
	Device.Destroy				( );
	Engine.Destroy				( );
}

void execUserScript				( )
{
// Execute script
/*
	strcpy						(Console->ConfigFile,"user.ltx");
	if (strstr(Core.Params,"-ltx ")) {
		string64				c_name;
		sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
		strcpy					(Console->ConfigFile,c_name);
	}
	if (!FS.exist(Console->ConfigFile))
		strcpy					(Console->ConfigFile,"user.ltx");
*/
	Console->ExecuteScript		(Console->ConfigFile);
}
void slowdownthread	( void* )
{
//	Sleep		(30*1000);
	for (;;)	{
		if (Device.Statistic->fFPS<30) Sleep(1);
		if (Device.mt_bMustExit)	return;
		if (0==pSettings)			return;
		if (0==Console)				return;
		if (0==pInput)				return;
		if (0==pApp)				return;
	}
}
void CheckPrivilegySlowdown		( )
{
//.#ifdef DEBUG
	if	(strstr(Core.Params,"-slowdown"))	{
		thread_spawn(slowdownthread,"slowdown",0,0);
	}
	if	(strstr(Core.Params,"-slowdown2x"))	{
		thread_spawn(slowdownthread,"slowdown",0,0);
		thread_spawn(slowdownthread,"slowdown",0,0);
	}
//.#endif
}

void Startup					( )
{
	execUserScript	();
	InitInput		();
	InitSound		();

	// ...command line for auto start
	{
		LPCSTR	pStartup			= strstr				(Core.Params,"-start ");
		if (pStartup)				Console->Execute		(pStartup+1);
	}
	{
		LPCSTR	pStartup			= strstr				(Core.Params,"-load ");
		if (pStartup)				Console->Execute		(pStartup+1);
	}

	// Initialize APP
//#ifndef DEDICATED_SERVER
	Device.Create				( );
//#endif
	LALib.OnCreate				( );
	pApp						= xr_new<CApplication>	();
	g_pGamePersistent			= (IGame_Persistent*)	NEW_INSTANCE (CLSID_GAME_PERSISTANT);
	g_SpatialSpace				= xr_new<ISpatial_DB>	();
	g_SpatialSpacePhysic		= xr_new<ISpatial_DB>	();
	
	// Destroy LOGO
	DestroyWindow				(logoWindow);
	logoWindow					= NULL;

	// Main cycle
	CheckCopyProtection			( );
	Device.Run					( );

	// Destroy APP
	xr_delete					( g_SpatialSpacePhysic	);
	xr_delete					( g_SpatialSpace		);
	DEL_INSTANCE				( g_pGamePersistent		);
	xr_delete					( pApp					);
	Engine.Event.Dump			( );

	// Destroying
	destroySound();
	destroyInput();

	if(!g_bBenchmark)
		destroySettings();

	LALib.OnDestroy				( );
	
	if(!g_bBenchmark)
		destroyConsole();
	else
		Console->Reset();

	destroyEngine();
}

static BOOL CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			DestroyWindow( hw );
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
				DestroyWindow( hw );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
/*
void	test_rtc	()
{
	CStatTimer		tMc,tM,tC,tD;
	u32				bytes=0;
	tMc.FrameStart	();
	tM.FrameStart	();
	tC.FrameStart	();
	tD.FrameStart	();
	::Random.seed	(0x12071980);
	for		(u32 test=0; test<10000; test++)
	{
		u32			in_size			= ::Random.randI(1024,256*1024);
		u32			out_size_max	= rtc_csize		(in_size);
		u8*			p_in			= xr_alloc<u8>	(in_size);
		u8*			p_in_tst		= xr_alloc<u8>	(in_size);
		u8*			p_out			= xr_alloc<u8>	(out_size_max);
		for (u32 git=0; git<in_size; git++)			p_in[git] = (u8)::Random.randI	(8);	// garbage
		bytes		+= in_size;

		tMc.Begin	();
		memcpy		(p_in_tst,p_in,in_size);
		tMc.End		();

		tM.Begin	();
		CopyMemory(p_in_tst,p_in,in_size);
		tM.End		();

		tC.Begin	();
		u32			out_size		= rtc_compress	(p_out,out_size_max,p_in,in_size);
		tC.End		();

		tD.Begin	();
		u32			in_size_tst		= rtc_decompress(p_in_tst,in_size,p_out,out_size);
		tD.End		();

		// sanity check
		R_ASSERT	(in_size == in_size_tst);
		for (u32 tit=0; tit<in_size; tit++)			R_ASSERT(p_in[tit] == p_in_tst[tit]);	// garbage

		xr_free		(p_out);
		xr_free		(p_in_tst);
		xr_free		(p_in);
	}
	tMc.FrameEnd	();	float rMc		= 1000.f*(float(bytes)/tMc.result)/(1024.f*1024.f);
	tM.FrameEnd		(); float rM		= 1000.f*(float(bytes)/tM.result)/(1024.f*1024.f);
	tC.FrameEnd		(); float rC		= 1000.f*(float(bytes)/tC.result)/(1024.f*1024.f);
	tD.FrameEnd		(); float rD		= 1000.f*(float(bytes)/tD.result)/(1024.f*1024.f);
	Msg				("* memcpy:        %5.2f M/s (%3.1f%%)",rMc,100.f*rMc/rMc);
	Msg				("* mm-memcpy:     %5.2f M/s (%3.1f%%)",rM,100.f*rM/rMc);
	Msg				("* compression:   %5.2f M/s (%3.1f%%)",rC,100.f*rC/rMc);
	Msg				("* decompression: %5.2f M/s (%3.1f%%)",rD,100.f*rD/rMc);
}
*/
extern void	testbed	(void);

// video
/*
static	HINSTANCE	g_hInstance		;
static	HINSTANCE	g_hPrevInstance	;
static	int			g_nCmdShow		;
void	__cdecl		intro_dshow_x	(void*)
{
	IntroDSHOW_wnd		(g_hInstance,g_hPrevInstance,"GameData\\Stalker_Intro.avi",g_nCmdShow);
	g_bIntroFinished	= TRUE	;
}
*/
#include "xr_ioc_cmd.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       nCmdShow)
{
	SetThreadAffinityMask		(GetCurrentThread(),1);

	// Title window
	logoWindow = CreateDialog	(GetModuleHandle(NULL),	MAKEINTRESOURCE(IDD_STARTUP), 0, logDlgProc );

	// AVI
	g_bIntroFinished		= TRUE;
	/*
	if (!IsDebuggerPresent())
	{
		g_hInstance				= hInstance;
		g_hPrevInstance			= hPrevInstance;
		g_nCmdShow				= nCmdShow;
		thread_spawn			(Intro_DSHOW,"intro",0,"GameData\\Stalker_Intro.avi");
		Sleep					(100);
	}
	*/
	g_sLaunchOnExit_app[0]		= NULL;
	g_sLaunchOnExit_params[0]	= NULL;
	// Core
	Core._initialize		("xray",NULL);
//	DUMP_PHASE;
	
	FPU::m24r				();

	// auth
	{
#pragma todo("container is created in stack!")
		xr_vector<xr_string>	ignore, test	;
		ignore.push_back		(xr_string("user"));
		ignore.push_back		(xr_string("Stalker_net.exe"));
		ignore.push_back		(xr_string("map_list.ltx"));
		ignore.push_back		(xr_string("banned.ltx"));
		ignore.push_back		(xr_string("maprot_list.ltx"));


		test.push_back			(xr_string(".ltx"));
		test.push_back			(xr_string(".script"));
		test.push_back			(xr_string(".exe"));
		test.push_back			(xr_string(".dll"));
		FS.auth_generate		(ignore,test)	;
	}
	// auth end 

//#ifdef DEBUG
//	testbed	();
//#endif

	InitEngine				();
	InitSettings			();
	InitConsole				();

	if(strstr(lpCmdLine, "-batch_benchmark")){
		doBenchmark();
		return 0;
	}

	if (strstr(lpCmdLine,"-launcher")) 
	{
		int l_res = doLauncher();
		if (l_res != 0)
			return 0;
	};
	

	if(strstr(Core.Params,"-r2a"))	
		Console->Execute			("renderer renderer_r2a");
	else
	if(strstr(Core.Params,"-r2"))	
		Console->Execute			("renderer renderer_r2");
	else
	{
		CCC_LoadCFG_custom*	pTmp = xr_new<CCC_LoadCFG_custom>("renderer ");
		pTmp->Execute				(Console->ConfigFile);
		xr_delete					(pTmp);
	}


	Engine.External.Initialize	( );
//	CheckPrivilegySlowdown		( );
	Console->Execute			("stat_memory");
	Startup	 					( );
	Core._destroy				( );

	char* _args[3];
	// check for need to execute something external
	if (xr_strlen(g_sLaunchOnExit_params) && xr_strlen(g_sLaunchOnExit_app) ) 
	{
		_args[0] = g_sLaunchOnExit_app;
		_args[1] = g_sLaunchOnExit_params;
		_args[2] = NULL;

		_spawnv(_P_NOWAIT, _args[0], _args);
	}

	return						0;
}

LPCSTR _GetFontTexName (LPCSTR section)
{
	u32 w = Device.dwWidth;

	static char* tex_names[]={"texture800","texture","texture1600"};
	int def_idx		= 1;//default 1024x768
	int idx			= def_idx;

	if(w<=800)		idx = 0;
	else if(w<=1080)idx = 1;
	else 			idx = 2;

	while(idx>=0){
		if( pSettings->line_exist(section,tex_names[idx]) )
			return pSettings->r_string(section,tex_names[idx]);
		--idx;
	}
	return pSettings->r_string(section,tex_names[def_idx]);
}

void _InitializeFont(CGameFont*& F, LPCSTR section, u32 flags)
{
	LPCSTR font_tex_name = _GetFontTexName(section);
	R_ASSERT(font_tex_name);

	if(!F){
		F = xr_new<CGameFont> ("font", font_tex_name, flags);
		Device.seqRender.Add( F, REG_PRIORITY_LOW-1000 );
	}else
		F->Initialize("font",font_tex_name);

	if (pSettings->line_exist(section,"size")){
		float sz = pSettings->r_float(section,"size");
		if (flags&CGameFont::fsDeviceIndependent)	F->SetSizeI(sz);
		else										F->SetSize(sz);
	}
	if (pSettings->line_exist(section,"interval"))
		F->SetInterval(pSettings->r_fvector2(section,"interval"));

}

CApplication::CApplication()
{
	ll_dwReference	= 0;

	// events
	eQuit						= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStart						= Engine.Event.Handler_Attach("KERNEL:start",this);
	eStartLoad					= Engine.Event.Handler_Attach("KERNEL:load",this);
	eDisconnect					= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
//	pFontSystem					= xr_new<CGameFont>	("startup_font",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	pFontSystem					= NULL;


	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	if (psDeviceFlags.test(mtSound))	Device.seqFrameMT.Add		(&SoundProcessor);
	else								Device.seqFrame.Add			(&SoundProcessor);
	Console->Show				( );
}

CApplication::~CApplication()
{
	Console->Hide				( );

	// font
	Device.seqRender.Remove		( pFontSystem		);
	xr_delete					( pFontSystem		);

	Device.seqFrameMT.Remove	(&SoundProcessor);
	Device.seqFrame.Remove		(&SoundProcessor);
	Device.seqFrame.Remove		(this);


	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartLoad,this);
	Engine.Event.Handler_Detach	(eStart,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E==eQuit)	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			xr_free(Levels[i].folder	);
			xr_free(Levels[i].name	);
		}
	} else if (E==eStart) {
		Console->Execute("main_menu off");
		Console->Hide();
		LPSTR		op_server		= LPSTR	(P1);
		LPSTR		op_client		= LPSTR	(P2);
		R_ASSERT	(0==g_pGameLevel);
		R_ASSERT	(0!=g_pGamePersistent);
		//-----------------------------------------------------------
		g_pGamePersistent->PreStart		(op_server);
		//-----------------------------------------------------------
		g_pGameLevel					= (IGame_Level*)NEW_INSTANCE(CLSID_GAME_LEVEL);
		pApp->LoadBegin					(); 
		g_pGamePersistent->Start		(op_server);
		g_pGameLevel->net_Start			(op_server,op_client);
		pApp->LoadEnd					(); 
		xr_free							(op_server);
		xr_free							(op_client);
	} else if (E==eDisconnect) {
		if (g_pGameLevel) {
			Console->Hide			();
			g_pGameLevel->net_Stop	();
			DEL_INSTANCE			(g_pGameLevel);
			Console->Show			();
			
			if( (FALSE == Engine.Event.Peek("KERNEL:quit")) &&(FALSE == Engine.Event.Peek("KERNEL:start")) )
			{
				Console->Execute("main_menu off");
				Console->Execute("main_menu on");
			}
		}
		R_ASSERT			(0!=g_pGamePersistent);
		g_pGamePersistent->Disconnect();
	}
}

static	CTimer	phase_timer		;
extern	ENGINE_API BOOL			g_appLoaded = FALSE;

void CApplication::LoadBegin	()
{
	ll_dwReference++;
	if (1==ll_dwReference)	{
		_InitializeFont		(pFontSystem,"ui_font_graffiti19_russian",0);

		g_appLoaded			= FALSE;
		phase_timer.Start	();
//.		::Sound->mute		(true);
		ll_hGeom.create		(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
		sh_progress.create	("hud\\default","ui\\ui_load");
		ll_hGeom2.create		(FVF::F_TL, RCache.Vertex.Buffer(),NULL);

		load_stage			= 0;

		CheckCopyProtection	();
	}
}

void CApplication::LoadEnd		()
{
	ll_dwReference--;
	if (0==ll_dwReference)		{
		Msg						("* phase time: %d ms",phase_timer.GetElapsed_ms());
		Msg						("* phase cmem: %d K", Memory.mem_usage()/1024);
		Console->Execute		("stat_memory");
		g_appLoaded				= TRUE;
//		DUMP_PHASE;
	}
}

void CApplication::destroy_loading_shaders()
{
	hLevelLogo.destroy		();
	sh_progress.destroy		();
//.	::Sound->mute			(false);
}

u32 calc_progress_color(u32, u32, int, int);

void CApplication::LoadDraw		()
{
	if(g_appLoaded)				return;
	Device.dwFrame				+= 1;


	if(!Device.Begin () )		return;

	if	(g_pGamePersistent->bDedicatedServer)
		Console->OnRender			();
	else
		load_draw_internal			();

	Device.End					();
	CheckCopyProtection			();
}

void CApplication::LoadTitle	(char *S, char *S2)
{
	load_stage++;
	VERIFY						(ll_dwReference);
	sprintf						(app_title,S,S2);
	Msg							("* phase time: %d ms",phase_timer.GetElapsed_ms());	phase_timer.Start();
	Msg							("* phase cmem: %d K", Memory.mem_usage()/1024);
	Console->Execute			("stat_memory");
//	DUMP_PHASE;
	Log							(app_title);
	
	if (g_pGamePersistent->GameType()==1 && strstr(Core.Params,"alife"))
		max_load_stage			= 17;
	else
		max_load_stage			= 14;

	LoadDraw					();
}

void CApplication::LoadSwitch	()
{
//	if (ll_hLogo == ll_hLogo1)	ll_hLogo = ll_hLogo2;
//	else						ll_hLogo = ll_hLogo1;
}

void CApplication::SetLoadLogo			(ref_shader NewLoadLogo)
{
//	hLevelLogo = NewLoadLogo;
//	R_ASSERT(0);
};

// Sequential
void CApplication::OnFrame	( )
{
	Engine.Event.OnFrame			();
	g_SpatialSpace->update			();
	g_SpatialSpacePhysic->update	();
	if (g_pGameLevel)				g_pGameLevel->SoundEvent_Dispatch	( );
}

void CApplication::Level_Append		(LPCSTR folder)
{
	string256	N1,N2,N3,N4;
	strconcat	(N1,folder,"level");
	strconcat	(N2,folder,"level.ltx");
	strconcat	(N3,folder,"level.geom");
	strconcat	(N4,folder,"level.cform");
	if	(
		FS.exist("$game_levels$",N1)		&&
		FS.exist("$game_levels$",N2)		&&
		FS.exist("$game_levels$",N3)		&&
		FS.exist("$game_levels$",N4)	
		)
	{
		sLevelInfo			LI;
		LI.folder			= xr_strdup(folder);
		LI.name				= 0;
		Levels.push_back	(LI);
	}
}

void CApplication::Level_Scan()
{
#pragma todo("container is created in stack!")
	xr_vector<char*>*		folder			= FS.file_list_open		("$game_levels$",FS_ListFolders|FS_RootOnly);
	R_ASSERT				(folder&&folder->size());
	for (u32 i=0; i<folder->size(); i++)	Level_Append((*folder)[i]);
	FS.file_list_close		(folder);
#ifdef DEBUG
	folder									= FS.file_list_open		("$game_levels$","$debug$\\",FS_ListFolders|FS_RootOnly);
	if (folder){
		string_path	tmp_path;
		for (u32 i=0; i<folder->size(); i++)Level_Append(strconcat(tmp_path,"$debug$\\",(*folder)[i]));
		FS.file_list_close	(folder);
	}
#endif
}

void CApplication::Level_Set(u32 L)
{
	if (L>=Levels.size())	return;
	Level_Current = L;
	FS.get_path	("$level$")->_set	(Levels[L].folder);


	string_path					temp;
	string_path					temp2;
	strconcat					(temp,"intro\\intro_",Levels[L].folder);
	temp[xr_strlen(temp)-1] = 0;
	if (FS.exist(temp2, "$game_textures$", temp, ".dds"))
		hLevelLogo.create	("font", temp);
	else
		hLevelLogo.create	("font", "intro\\intro_no_start_picture");
		

	CheckCopyProtection		();
}

int CApplication::Level_ID(LPCSTR name)
{
	char buffer	[256];
	strconcat	(buffer,name,"\\");
	for (u32 I=0; I<Levels.size(); I++)
	{
		if (0==stricmp(buffer,Levels[I].folder))	return int(I);
	}
	return -1;
}


//launcher stuff----------------------------
extern "C"{
	typedef int	 __cdecl LauncherFunc	(int);
}
HMODULE			hLauncher		= NULL;
LauncherFunc*	pLauncher		= NULL;

void InitLauncher(){
	if(hLauncher)
		return;
	hLauncher	= LoadLibrary	("xrLauncher.dll");
	if (0==hLauncher)	R_CHK	(GetLastError());
	R_ASSERT2		(hLauncher,"xrLauncher DLL raised exception during loading or there is no xrLauncher.dll at all");

	pLauncher = (LauncherFunc*)GetProcAddress(hLauncher,"RunXRLauncher");
	R_ASSERT2		(pLauncher,"Cannot obtain RunXRLauncher function from xrLauncher.dll");
};

void FreeLauncher(){
	if (hLauncher)	{ 
		FreeLibrary(hLauncher); 
		hLauncher = NULL; pLauncher = NULL; };
}

int doLauncher()
{
	execUserScript();
	InitLauncher();
	int res = pLauncher(0);
	FreeLauncher();
	if(res == 1) // do benchmark
		g_bBenchmark = true;

	if(g_bBenchmark){ //perform benchmark cycle
		doBenchmark();
	
		// InitLauncher	();
		// pLauncher	(2);	//show results
		// FreeLauncher	();

		Core._destroy			();
		return					(1);

	};
	if(res==8){//Quit
		Core._destroy			();
		return					(1);
	}
	return 0;

}

void doBenchmark()
{
	g_bBenchmark = true;
	string_path in_file;
	FS.update_path(in_file,"$server_root$","tmp_benchmark.ini");
	CInifile ini(in_file);
	int test_count = ini.line_count("benchmark");
	LPCSTR test_name,t;
	shared_str test_command;
	for(int i=0;i<test_count;++i){
		ini.r_line( "benchmark", i, &test_name, &t);
		
		test_command = ini.r_string_wb("benchmark",test_name);
		strlwr				(strcpy(Core.Params,*test_command));
		
		if(i){
			ZeroMemory(&HW,sizeof(CHW));
			InitEngine();
		}

		Engine.External.Initialize	( );
		execUserScript();
		Startup	 				();
	}
}
#pragma optimize("g", off)
void CApplication::load_draw_internal()
{
	if(!sh_progress){
		CHK_DX			(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1,0));
		return;
	}
		// Draw logo
		u32	Offset;
		u32	C						= 0xffffffff;
		u32	_w						= Device.dwWidth;
		u32	_h						= Device.dwHeight;
		FVF::TL* pv					= NULL;

//progress
		float bw					= 1024.0f;
		float bh					= 768.0f;
		Fvector2					k; k.set(float(_w)/bw, float(_h)/bh);

		RCache.set_Shader			(sh_progress);
		CTexture*	T				= RCache.get_ActiveTexture(0);
		Fvector2					tsz;
		tsz.set						((float)T->get_Width(),(float)T->get_Height());
		Frect						back_text_coords;
		Frect						back_coords;
		Fvector2					back_size;

//progress background
		static float offs			= -0.5f;

		back_size.set				(1024,768);
		back_text_coords.lt.set		(0,0);back_text_coords.rb.add(back_text_coords.lt,back_size);
		back_coords.lt.set			(offs, offs); back_coords.rb.add(back_coords.lt,back_size);

		back_coords.lt.mul			(k);back_coords.rb.mul(k);

		back_text_coords.lt.x/=tsz.x; back_text_coords.lt.y/=tsz.y; back_text_coords.rb.x/=tsz.x; back_text_coords.rb.y/=tsz.y;
		pv							= (FVF::TL*) RCache.Vertex.Lock(4,ll_hGeom.stride(),Offset);
		pv->set						(back_coords.lt.x,	back_coords.rb.y,	C,back_text_coords.lt.x,	back_text_coords.rb.y);	pv++;
		pv->set						(back_coords.lt.x,	back_coords.lt.y,	C,back_text_coords.lt.x,	back_text_coords.lt.y);	pv++;
		pv->set						(back_coords.rb.x,	back_coords.rb.y,	C,back_text_coords.rb.x,	back_text_coords.rb.y);	pv++;
		pv->set						(back_coords.rb.x,	back_coords.lt.y,	C,back_text_coords.rb.x,	back_text_coords.lt.y);	pv++;
		RCache.Vertex.Unlock		(4,ll_hGeom.stride());

		RCache.set_Geometry			(ll_hGeom);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

//progress bar
		back_size.set				(268,37);
		back_text_coords.lt.set		(0,768);back_text_coords.rb.add(back_text_coords.lt,back_size);
		back_coords.lt.set			(379 ,726);back_coords.rb.add(back_coords.lt,back_size);

		back_coords.lt.mul			(k);back_coords.rb.mul(k);

		back_text_coords.lt.x/=tsz.x; back_text_coords.lt.y/=tsz.y; back_text_coords.rb.x/=tsz.x; back_text_coords.rb.y/=tsz.y;



		u32 v_cnt					= 40;
		pv							= (FVF::TL*)RCache.Vertex.Lock	(2*(v_cnt+1),ll_hGeom2.stride(),Offset);
		FVF::TL* _pv				= pv;
		float pos_delta				= back_coords.width()/v_cnt;
		float tc_delta				= back_text_coords.width()/v_cnt;
		u32 clr = C;

		for(u32 idx=0; idx<v_cnt+1; ++idx){
			clr =					calc_progress_color(idx,v_cnt,load_stage,max_load_stage);
			pv->set					(back_coords.lt.x+pos_delta*idx+offs,	back_coords.rb.y+offs,	0+EPS_S, 1, clr, back_text_coords.lt.x+tc_delta*idx,	back_text_coords.rb.y);	pv++;
			pv->set					(back_coords.lt.x+pos_delta*idx+offs,	back_coords.lt.y+offs,	0+EPS_S, 1, clr, back_text_coords.lt.x+tc_delta*idx,	back_text_coords.lt.y);	pv++;
		}
		VERIFY						(u32(pv-_pv)==2*(v_cnt+1));
		RCache.Vertex.Unlock		(2*(v_cnt+1),ll_hGeom2.stride());

		RCache.set_Geometry			(ll_hGeom2);
		RCache.Render				(D3DPT_TRIANGLESTRIP, Offset, 2*v_cnt);


		// Draw title
		VERIFY						(pFontSystem);
		pFontSystem->Clear			();
		pFontSystem->SetColor		(color_rgba(157,140,120,255));
		pFontSystem->SetAligment	(CGameFont::alCenter);
		pFontSystem->OutI			(0.f,0.815f,app_title);
		pFontSystem->OnRender		();


//draw level-specific screenshot
		if(hLevelLogo){
			Frect						r;
			r.lt.set					(257,369);
			r.lt.x						+= offs;
			r.lt.y						+= offs;
			r.rb.add					(r.lt,Fvector2().set(512,256));
			r.lt.mul					(k);						
			r.rb.mul					(k);						
			pv							= (FVF::TL*) RCache.Vertex.Lock(4,ll_hGeom.stride(),Offset);
			pv->set						(r.lt.x,				r.rb.y,		C, 0, 1);	pv++;
			pv->set						(r.lt.x,				r.lt.y,		C, 0, 0);	pv++;
			pv->set						(r.rb.x,				r.rb.y,		C, 1, 1);	pv++;
			pv->set						(r.rb.x,				r.lt.y,		C, 1, 0);	pv++;
			RCache.Vertex.Unlock		(4,ll_hGeom.stride());

			RCache.set_Shader			(hLevelLogo);
			RCache.set_Geometry			(ll_hGeom);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}

}

u32 calc_progress_color(u32 idx, u32 total, int stage, int max_stage)
{
	if(idx>(total/2)) 
		idx	= total-idx;


	float kk			= (float(stage+1)/float(max_stage))*(total/2.0f);
	float f				= 1/(exp((float(idx)-kk)*0.5f)+1.0f);

	return color_argb_f		(f,1.0f,1.0f,1.0f);
}
