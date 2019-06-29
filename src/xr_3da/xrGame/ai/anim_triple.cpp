#include "stdafx.h"
#include "anim_triple.h"

char *dbg_states[] = {
	"eStatePrepare", 
	"eStateExecute", 
	"eStateFinalize", 
	"eStateNone"
};


CAnimTriple::CAnimTriple()
{
	cur_state	= eStateNone;
	m_active	= false;
}
CAnimTriple::~CAnimTriple()
{

}

void CAnimTriple::init_external(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	pool[0] = m_def1;
	pool[1]	= m_def2;
	pool[2]	= m_def3;
}

void CAnimTriple::activate()
{
	if (is_active()) deactivate();

	cur_state	= eStatePrepare;
	m_active	= true;
}

void CAnimTriple::deactivate()
{
	cur_state	= eStateNone;
	m_active	= false;
}

bool CAnimTriple::prepare_animation(CMotionDef **m)
{
	if (cur_state == eStateNone) {
		deactivate();
		return false;
	}
	
	if (is_active()) *m = pool[cur_state];
	if (cur_state != eStateExecute) cur_state = EStateAnimTriple(cur_state + 1);
	return true;
}


void CAnimTriple::pointbreak()
{
	cur_state = eStateFinalize;
}



