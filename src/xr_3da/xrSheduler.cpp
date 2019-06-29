#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"

float			psShedulerCurrent		= 10.f	;
float			psShedulerTarget		= 10.f	;
const	float	psShedulerReaction		= 0.1f	;
BOOL			g_bSheduleInProgress	= FALSE	;

//-------------------------------------------------------------------------------------
void CSheduler::Initialize		()
{
}

void CSheduler::Destroy			()
{
	internal_Registration		();

	for (u32 it=0; it<Items.size(); it++)
	{
		if (0==Items[it].Object)	
		{
			Items.erase(Items.begin()+it);
			it	--;
		}
	}
#ifdef DEBUG	
	if (!Items.empty())
	{
		string1024		_objects; _objects[0]=0;
		for (u32 it=0; it<Items.size(); it++)
			strconcat	(_objects,*Items[it].Object->shedule_Name(),",");
		Debug.fatal		(DEBUG_INFO,"Sheduler work-list is not empty\n%s",_objects);
	}
#endif
	ItemsRT.clear		();
	Items.clear			();
	ItemsProcessed.clear();
	Registration.clear	();
}

void	CSheduler::internal_Registration()
{
	for (u32 it=0; it<Registration.size(); it++)
	{
		ItemReg&	R	= Registration	[it];
		if (R.OP)	{
			// register
			// search for paired "unregister"
			BOOL	bFoundAndErased		= FALSE;
			for (u32 pair=it+1; pair<Registration.size(); pair++)
			{
				ItemReg&	R_pair	= Registration	[pair];
				if	((!R_pair.OP)&&(R_pair.Object == R.Object))	{
					bFoundAndErased		= TRUE;
					Registration.erase	(Registration.begin()+pair	);
					break				;
				}
			}

			// register if non-paired
			if (!bFoundAndErased)		internal_Register	(R.Object,R.RT);
		}
		else		{
			// unregister
			internal_Unregister	(R.Object,R.RT);
		}
	}
	Registration.clear	();
}

void CSheduler::internal_Register	(ISheduled* O, BOOL RT)
{
	VERIFY	(!O->shedule.b_locked)	;
	if (RT)
	{
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= O;
		O->shedule.b_RT				= TRUE;

		ItemsRT.push_back			(TNext);
	} else {
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= O;
		O->shedule.b_RT				= FALSE;

		// Insert into priority Queue
		Push						(TNext);
	}
}

void CSheduler::internal_Unregister	(ISheduled* O, BOOL RT)
{
	//the object may be already dead
	//VERIFY	(!O->shedule.b_locked)	;
	if (RT)
	{
		for (u32 i=0; i<ItemsRT.size(); i++)
		{
			if (ItemsRT[i].Object==O) {
				ItemsRT.erase(ItemsRT.begin()+i);
				return;
			}
		}
	} else {
		for (u32 i=0; i<Items.size(); i++)
		{
			if (Items[i].Object==O) {
				Items[i].Object	= NULL;
				return;
			}
		}
	}
}

void	CSheduler::Register		(ISheduled* A, BOOL RT				)
{
	ItemReg		R;
	R.OP		= TRUE				;
	R.RT		= RT				;
	R.Object	= A					;
	R.Object->shedule.b_RT	= RT	;

	Registration.push_back	(R);
}
void	CSheduler::Unregister	(ISheduled* A						)
{
	ItemReg		R;
	R.OP		= FALSE				;
	R.RT		= A->shedule.b_RT	;
	R.Object	= A					;
	Registration.push_back			(R);
}

void CSheduler::EnsureOrder		(ISheduled* Before, ISheduled* After)
{
	VERIFY(Before->shedule.b_RT && After->shedule.b_RT);

	for (u32 i=0; i<ItemsRT.size(); i++)
	{
		if (ItemsRT[i].Object==After) 
		{
			Item	A			= ItemsRT[i];
			ItemsRT.erase		(ItemsRT.begin()+i);
			ItemsRT.push_back	(A);
			return;
		}
	}
}

void CSheduler::Push				(Item& I)
{
	Items.push_back	(I);
	std::push_heap	(Items.begin(), Items.end());
}

void CSheduler::Pop					()
{
	std::pop_heap	(Items.begin(), Items.end());
	Items.pop_back	();
}

void CSheduler::ProcessStep			()
{
	// Normal priority
	u32		dwTime					= Device.dwTimeGlobal;
	CTimer							eTimer;
	for (int i=0;!Items.empty() && Top().dwTimeForExecute < dwTime; ++i) {
		u32		delta_ms			= dwTime - Top().dwTimeForExecute;

		// Update
		Item	T					= Top	();
		u32		Elapsed				= dwTime-T.dwTimeOfLastExecute;
		if	(NULL==T.Object || !T.Object->shedule_Needed())		{
			// Erase element
			Pop						();
			continue;
		}

		// Insert into priority Queue
		Pop							();

		// Real update call
		// Msg						("------- %d:",Device.dwFrame);
#ifdef DEBUG
		T.Object->dbg_startframe	= Device.dwFrame;
		eTimer.Start				();
		LPCSTR		_obj_name		= T.Object->shedule_Name().c_str();
#endif

		// Calc next update interval
		u32		dwMin				= _max(u32(30),T.Object->shedule.t_min);
		u32		dwMax				= (1000+T.Object->shedule.t_max)/2;
		float	scale				= T.Object->shedule_Scale	(); 
		u32		dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,u32(_max(dwMin,u32(20))),dwMax);


		try {
			T.Object->shedule_Update	(clampr(Elapsed,u32(1),u32(_max(u32(T.Object->shedule.t_max),u32(1000)))) );
		} catch (...) {
#ifdef DEBUG
			Msg		("! xrSheduler: object '%s' raised an exception", _obj_name);
			throw	;
#endif
		}
#ifdef DEBUG
		u32	execTime				= eTimer.GetElapsed_ms		();
#endif

		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;

		ItemsProcessed.push_back	(TNext);

#ifdef DEBUG
//		u32	execTime				= eTimer.GetElapsed_ms		();
		// VERIFY3					(T.Object->dbg_update_shedule == T.Object->dbg_startframe, "Broken sequence of calls to 'shedule_Update'", _obj_name );
		if (delta_ms> 3*dwUpdate)	{
			//Msg	("! xrSheduler: failed to shedule object [%s] (%dms)",	_obj_name, delta_ms	);
		}
		if (execTime> 15)			{
			Msg	("* xrSheduler: too much time consumed by object [%s] (%dms)",	_obj_name, execTime	);
		}
#endif

		// 
		if ((i % 3) != (3 - 1))
			continue;

		if (CPU::QPC() > cycles_limit)		{
			// we have maxed out the load - increase heap
			psShedulerTarget		+= (psShedulerReaction * 3);
			break;
		}
	}

	// Push "processed" back
	while (ItemsProcessed.size())	{
		Push	(ItemsProcessed.back())	;
		ItemsProcessed.pop_back		()	;
	}

	// always try to decrease target
	psShedulerTarget	-= psShedulerReaction;
}
/*
void CSheduler::Switch				()
{
	if (fibered)	
	{
		fibered						= FALSE;
		SwitchToFiber				(fiber_main);
	}
}
*/
void CSheduler::Update				()
{
	// Initialize
	Device.Statistic->Sheduler.Begin	();
	cycles_start					= CPU::QPC			();
	cycles_limit					= CPU::qpc_freq * u64 (iCeil(psShedulerCurrent)) / 1000i64 + cycles_start;
	internal_Registration			();
	g_bSheduleInProgress			= TRUE;

	// Realtime priority
	u32	dwTime						= Device.dwTimeGlobal;
	for (u32 it=0; it<ItemsRT.size(); it++)
	{
		Item&	T						= ItemsRT[it];
		if(!T.Object->shedule_Needed()){
			T.dwTimeOfLastExecute		= dwTime;
			continue;
		}

		u32	Elapsed						= dwTime-T.dwTimeOfLastExecute;
#ifdef DEBUG
		VERIFY							(T.Object->dbg_startframe != Device.dwFrame);
		T.Object->dbg_startframe		= Device.dwFrame;
#endif
		T.Object->shedule_Update	(Elapsed);
		T.dwTimeOfLastExecute			= dwTime;
	}

	// Normal (sheduled)
	ProcessStep						();
	clamp							(psShedulerTarget,3.f,66.f);
	psShedulerCurrent				= 0.9f*psShedulerCurrent + 0.1f*psShedulerTarget;
	Device.Statistic->fShedulerLoad	= psShedulerCurrent;

	// Finalize
	g_bSheduleInProgress			= FALSE;
	internal_Registration			();
	Device.Statistic->Sheduler.End	();
}
