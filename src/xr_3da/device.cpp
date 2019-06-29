#include "stdafx.h"
#include "frustum.h"

#pragma warning(disable:4995)
// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>
// d3dx9.h
#include <d3dx9.h>
#pragma warning(default:4995)

#include "x_ray.h"

ENGINE_API CRenderDevice Device;
ENGINE_API BOOL g_bRendering = FALSE; 

BOOL g_bLoaded = FALSE;

BOOL CRenderDevice::Begin	()
{
#ifndef DEDICATED_SERVER
	HW.Validate		();
	HRESULT	_hr		= HW.pDevice->TestCooperativeLevel();
    if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if		(D3DERR_DEVICELOST==_hr)		{
			Sleep	(33);
			return	FALSE;
		}

		// Check if the device is ready to be reset
		if		(D3DERR_DEVICENOTRESET==_hr)
		{
			Reset	();
		}
	}

	CHK_DX					(HW.pDevice->BeginScene());
	RCache.OnFrameBegin		();
	RCache.set_CullMode		(CULL_CW);
	RCache.set_CullMode		(CULL_CCW);
	if (HW.Caps.SceneMode)	overdrawBegin	();
	FPU::m24r	();
	g_bRendering = 	TRUE;
#endif
	return		TRUE;
}

void CRenderDevice::Clear	()
{
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		(psDeviceFlags.test(rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,0,0),1,0
		));
}

extern void CheckPrivilegySlowdown		( );

void CRenderDevice::End		(void)
{
#ifndef DEDICATED_SERVER

	VERIFY	(HW.pDevice);

	if (HW.Caps.SceneMode)	overdrawEnd		();

	// 
	if (dwPrecacheFrame)
	{
		::Sound->set_master_volume	(0.f);
		dwPrecacheFrame	--;
//.		CHK_DX			(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1,0));
		pApp->load_draw_internal	();
		if (0==dwPrecacheFrame){
			::Sound->set_master_volume	(1.f);
//.			Log("Precache End",Device.dwFrame);
			pApp->destroy_loading_shaders();
			Memory.mem_compact		();
			Msg						("* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
			CheckPrivilegySlowdown	();
		}
	}

	g_bRendering = 	FALSE;
	// end scene
	RCache.OnFrameEnd	();
    CHK_DX(HW.pDevice->EndScene());

	HRESULT _hr		= HW.pDevice->Present( NULL, NULL, NULL, NULL );
	if				(D3DERR_DEVICELOST==_hr)	return;			// we will handle this later
	//R_ASSERT2		(SUCCEEDED(_hr),	"Presentation failed. Driver upgrade needed?");
#endif
}


volatile u32	mt_Thread_marker		= 0x12345678;
void 			mt_Thread	(void *ptr)	{
	while (true) {
		// waiting for Device permission to execute
		Device.mt_csEnter.Enter	();

		if (Device.mt_bMustExit) {
			Device.mt_bMustExit = FALSE;				// Important!!!
			Device.mt_csEnter.Leave();					// Important!!!
			return;
		}
		// we has granted permission to execute
		mt_Thread_marker			= Device.dwFrame;
		for (u32 pit=0; pit<Device.seqParallel.size(); pit++)
			Device.seqParallel[pit]	();
		Device.seqParallel.clear_not_free	();
		Device.seqFrameMT.Process	(rp_Frame);

		// now we give control to device - signals that we are ended our work
		Device.mt_csEnter.Leave	();
		// waits for device signal to continue - to start again
		Device.mt_csLeave.Enter	();
		// returns sync signal to device
		Device.mt_csLeave.Leave	();
	}
}

void CRenderDevice::PreCache	(u32 amount)
{
	if (HW.Caps.bForceGPU_REF)	amount=0;
	// Msg			("* PCACHE: start for %d...",amount);
	dwPrecacheFrame	= dwPrecacheTotal = amount;
}

ENGINE_API xr_list<LOADING_EVENT>			g_loading_events;

void CRenderDevice::Run			()
{
//	DUMP_PHASE;
	g_bLoaded		= FALSE;
	MSG				msg;
    BOOL			bGotMsg;
	Log				("Starting engine...");
	thread_name		("X-RAY Primary thread");

	// Startup timers and calculate timer delta
	dwTimeGlobal				= 0;
	Timer_MM_Delta				= 0;
	{
		u32 time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		u32 time_system		= timeGetTime	();
		u32 time_local		= TimerAsync	();
		Timer_MM_Delta		= time_system-time_local;
	}

	// Start all threads
//	InitializeCriticalSection	(&mt_csEnter);
//	InitializeCriticalSection	(&mt_csLeave);
	mt_csEnter.Enter			();
	mt_bMustExit				= FALSE;
	thread_spawn				(mt_Thread,"X-RAY Secondary thread",0,0);

	// Message cycle
    PeekMessage					( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	seqAppStart.Process			(rp_AppStart);

	CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1,0));

	while( WM_QUIT != msg.message  )
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        if( bGotMsg )
        {
              TranslateMessage	( &msg );
              DispatchMessage	( &msg );
         }
        else
        {
			if (bReady) {
				if (psDeviceFlags.test(rsStatistic))	g_bEnableStatGather	= TRUE;
				else									g_bEnableStatGather	= FALSE;

				if(g_loading_events.size()){
					if( g_loading_events.front()() )
						g_loading_events.pop_front();
					pApp->LoadDraw				();
					continue;
				}else
					FrameMove						( );

				// Precache
				if (dwPrecacheFrame)
				{
					float factor					= float(dwPrecacheFrame)/float(dwPrecacheTotal);
					float angle						= PI_MUL_2 * factor;
					vCameraDirection.set			(_sin(angle),0,_cos(angle));	vCameraDirection.normalize	();
					vCameraTop.set					(0,1,0);
					vCameraRight.crossproduct		(vCameraTop,vCameraDirection);

					mView.build_camera_dir			(vCameraPosition,vCameraDirection,vCameraTop);
				}

				// Matrices
				mFullTransform.mul			( mProject,mView	);
				RCache.set_xform_view		( mView				);
				RCache.set_xform_project	( mProject			);
				D3DXMatrixInverse			( (D3DXMATRIX*)&mInvFullTransform, 0, (D3DXMATRIX*)&mFullTransform);

				// *** Resume threads
				// Capture end point - thread must run only ONE cycle
				// Release start point - allow thread to run
				mt_csLeave.Enter			();
				mt_csEnter.Leave			();
#ifndef DEDICATED_SERVER
				Statistic->RenderTOTAL_Real.FrameStart	();
				Statistic->RenderTOTAL_Real.Begin		();
				if (bActive)							{
					if (Begin())				{
						seqRender.Process						(rp_Render);
						if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || Statistic->errors.size())	
							Statistic->Show						();
						End										();
					}
				}
				Statistic->RenderTOTAL_Real.End			();
				Statistic->RenderTOTAL_Real.FrameEnd	();
				Statistic->RenderTOTAL.accum	= Statistic->RenderTOTAL_Real.accum;
#endif
				// *** Suspend threads
				// Capture startup point
				// Release end point - allow thread to wait for startup point
				mt_csEnter.Enter						();
				mt_csLeave.Leave						();

				// Ensure, that second thread gets chance to execute anyway
				if (dwFrame!=mt_Thread_marker)			{
					for (u32 pit=0; pit<Device.seqParallel.size(); pit++)
						Device.seqParallel[pit]			();
					Device.seqParallel.clear_not_free	();
					seqFrameMT.Process					(rp_Frame);
				}
			} else {
				Sleep		(100);
			}
			if (!bActive)	Sleep	(1);
        }
    }
	seqAppEnd.Process		(rp_AppEnd);

	// Stop Balance-Thread
	mt_bMustExit			= TRUE;
	mt_csEnter.Leave		();
	while (mt_bMustExit)	Sleep(0);
//	DeleteCriticalSection	(&mt_csEnter);
//	DeleteCriticalSection	(&mt_csLeave);
}

void ProcessLoading(RP_FUNC *f);
void CRenderDevice::FrameMove()
{
	dwFrame			++;

	dwTimeContinual	= TimerMM.GetElapsed_ms	();
	if (psDeviceFlags.test(rsConstantFPS))	{
		// 20ms = 50fps
		fTimeDelta		=	0.020f;			
		fTimeGlobal		+=	0.020f;
		dwTimeDelta		=	20;
		dwTimeGlobal	+=	20;
	} else {
		// Timer
		float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
		fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;			// smooth random system activity - worst case ~7% error
		if (fTimeDelta>.1f) fTimeDelta=.1f;									// limit to 15fps minimum

		if(Pause())		fTimeDelta = 0.0f;

//		u64	qTime		= TimerGlobal.GetElapsed_clk();
		fTimeGlobal		= TimerGlobal.GetElapsed_sec(); //float(qTime)*CPU::cycles2seconds;
		u32	_old_global	= dwTimeGlobal;
		dwTimeGlobal	= TimerGlobal.GetElapsed_ms	();	//u32((qTime*u64(1000))/CPU::cycles_per_second);
		dwTimeDelta		= dwTimeGlobal-_old_global;
	}

	// Frame move
	Statistic->EngineTOTAL.Begin	();
	if(!g_bLoaded) 
		ProcessLoading				(rp_Frame);
	else
		seqFrame.Process			(rp_Frame);
	Statistic->EngineTOTAL.End	();
}

void ProcessLoading				(RP_FUNC *f)
{
	Device.seqFrame.Process				(rp_Frame);
	g_bLoaded							= TRUE;
/*
	static u32 processed_idx			= 0;
	u32 tm = Device.TimerAsync_MMT		();

	xr_vector<_REG_INFO>& R		= Device.seqFrame.R;
    if (R.empty())				return;

	for (; processed_idx<R.size(); processed_idx++)	{
		f(R[processed_idx].Object);
		if	(Device.TimerAsync_MMT() > tm+50) return;
	}

	g_bLoaded				= TRUE;
	processed_idx			= 0;
*/
}

ENGINE_API BOOL bShowPauseString			= TRUE;
void	CRenderDevice::Pause				(BOOL bOn)
{
	if (!g_bBenchmark){
		if(!Pause())						bShowPauseString = TRUE;
		g_pauseMngr.Pause					(bOn);
	}
}

void	CRenderDevice::PauseSound			(BOOL val)
{
	::Sound->pause_emitters					(!!val);
}
