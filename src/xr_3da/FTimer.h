#ifndef FTimerH
#define FTimerH
#pragma once

class CTimer_paused;

class XRCORE_API pauseMngr
{
	xr_vector<CTimer_paused*>	m_timers;
	BOOL						m_paused;
public:
	pauseMngr():m_paused(FALSE){}
	BOOL Paused(){return m_paused;};
	void Pause(BOOL b);
	void Register (CTimer_paused* t);
	void UnRegister (CTimer_paused* t);
};

extern XRCORE_API pauseMngr		g_pauseMngr;

class XRCORE_API CTimer {
protected:
	u64			qwStartTime;
	u64			qwPausedTime;
	u64			qwPauseAccum;
	float		fResult;
	BOOL		bPause;
public:
				CTimer			()		: qwStartTime(0),qwPausedTime(0),qwPauseAccum(0),bPause(FALSE) { }
	IC	void	Start			()		{	if(bPause) return; qwStartTime = CPU::GetCycleCount()-qwPauseAccum;	}
	IC	float	Stop			()		{	return (fResult = GetElapsed_sec());							}
	IC	float	Get				()		{	return fResult;													}
	ICF u64		GetElapsed_clk	()		{	if(bPause) return qwPausedTime; else return CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead-qwPauseAccum; }
	IC	u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed_clk())/u64(CPU::cycles_per_milisec)); }
	IC	float	GetElapsed_sec	()		{	return float(GetElapsed_clk())*CPU::cycles2seconds;				}
	IC	void	Dump			()
	{
		Msg("* Elapsed time (sec): %f",GetElapsed_sec());
	}
};

class XRCORE_API CTimer_paused  : public CTimer		{
	u64							save_clock;
public:
	CTimer_paused				()		{ g_pauseMngr.Register(this);	}
	~CTimer_paused				()		{ g_pauseMngr.UnRegister(this);	}
	IC BOOL		Paused			()		{ return bPause;				}
	IC void		Pause			(BOOL b){
		if(bPause==b)return;

		if( b ){
			save_clock			= CPU::GetCycleCount()-CPU::cycles_overhead;
			qwPausedTime		= GetElapsed_clk();
//			Msg("%X PAUSE_ON  qwPausedTime=%d time(ms)=%d qwStartTime=%d", this, qwPaused_time, GetElapsed_ms(), qwStartTime);
		}else{
			u64 cur_clock		= CPU::GetCycleCount()-CPU::cycles_overhead;
			qwPauseAccum		+=  cur_clock - save_clock;
//			Msg("%X PAUSE_OFF qwPausedTime=%d time(ms)=%d qwStartTime=%d", this, qwPaused_time, GetElapsed_ms(), qwStartTime);
		}
		bPause = b;
	}
};

class XRCORE_API CStatTimer
{
public:
	CTimer		T;
	__int64		accum;
	float		result;
	u32			count;
public:
				CStatTimer		();
	void		FrameStart		();
	void		FrameEnd		();

	ICF void	Begin			()		{	count++; T.Start(); }
	ICF void	End				()		{	accum += T.GetElapsed_clk(); }

	ICF u64		GetElapsed_clk	()		{	return accum; }
	ICF u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed_clk())/u64(CPU::cycles_per_milisec)); }
	ICF float	GetElapsed_sec	()		{	return float(GetElapsed_clk())*CPU::cycles2seconds; }
	ICF float	GetFrame_sec	()		{	return result; }
};

#endif // FTimerH
