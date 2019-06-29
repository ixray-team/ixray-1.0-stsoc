#ifndef FTimerH
#define FTimerH
#pragma once

class	CTimer_paused;

class XRCORE_API				pauseMngr
{
	xr_vector<CTimer_paused*>	m_timers;
	BOOL						m_paused;
public:
			pauseMngr			():m_paused(FALSE){}
	BOOL	Paused				(){return m_paused;};
	void	Pause				(BOOL b);
	void	Register			(CTimer_paused* t);
	void	UnRegister			(CTimer_paused* t);
};

extern XRCORE_API pauseMngr		g_pauseMngr;

class XRCORE_API CTimer			{
protected:
	u64			qwStartTime		;
	u64			qwPausedTime	;
	u64			qwPauseAccum	;
	BOOL		bPause			;
public:
				CTimer			()		: qwStartTime(0),qwPausedTime(0),qwPauseAccum(0),bPause(FALSE)		{ }
	ICF	void	Start			()		{	if(bPause) return;	qwStartTime = CPU::QPC()-qwPauseAccum;		}
	ICF u64		GetElapsed_ticks()const	{	if(bPause) return	qwPausedTime; else return CPU::QPC()-qwStartTime-CPU::qpc_overhead-qwPauseAccum; }
	IC	u32		GetElapsed_ms	()const	{	return u32(GetElapsed_ticks()*u64(1000)/CPU::qpc_freq );	}
	IC	float	GetElapsed_sec	()const	{
#ifndef _EDITOR
		FPU::m64r	()			;
#endif        
		float		_result		=		float(double(GetElapsed_ticks())/double(CPU::qpc_freq )	)	;
#ifndef _EDITOR
		FPU::m24r	()			;
#endif
		return		_result		;
	}
	IC	void	Dump			()
	{
		Msg("* Elapsed time (sec): %f",GetElapsed_sec());
	}
};

class XRCORE_API CTimer_paused_ex : public CTimer		{
	u64							save_clock;
public:
	CTimer_paused_ex			()		{ }
	virtual ~CTimer_paused_ex	()		{ }
	IC BOOL		Paused			()const	{ return bPause;				}
	IC void		Pause			(BOOL b){
		if(bPause==b)			return	;

		u64		_current		=		CPU::QPC()-CPU::qpc_overhead	;
		if( b )	{
			save_clock			= _current				;
			qwPausedTime		= GetElapsed_ticks()	;
		}else	{
			qwPauseAccum		+=		_current - save_clock;
		}
		bPause = b;
	}
};

class XRCORE_API CTimer_paused  : public CTimer_paused_ex		{
public:
	CTimer_paused				()		{ g_pauseMngr.Register(this);	}
	virtual ~CTimer_paused		()		{ g_pauseMngr.UnRegister(this);	}
};

extern XRCORE_API BOOL			g_bEnableStatGather;
class XRCORE_API CStatTimer
{
public:
	CTimer		T;
	u64			accum;
	float		result;
	u32			count;
public:
				CStatTimer		();
	void		FrameStart		();
	void		FrameEnd		();

	ICF void	Begin			()		{	if (!g_bEnableStatGather) return;	count++; T.Start();				}
	ICF void	End				()		{	if (!g_bEnableStatGather) return;	accum += T.GetElapsed_ticks();	}

	ICF u64		GetElapsed_ticks()const	{	return accum;					}

	IC	u32		GetElapsed_ms	()const	{	return u32(GetElapsed_ticks()*u64(1000)/CPU::qpc_freq );	}
	IC	float	GetElapsed_sec	()const	{
#ifndef _EDITOR
		FPU::m64r	()			;
#endif        
		float		_result		=		float(double(GetElapsed_ticks())/double(CPU::qpc_freq )	)	;
#ifndef _EDITOR
		FPU::m24r	()			;
#endif
		return		_result		;
	}
};

#endif // FTimerH
