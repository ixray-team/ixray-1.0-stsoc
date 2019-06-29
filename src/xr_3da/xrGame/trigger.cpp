#include "stdafx.h"
/*
#include "trigger.h"
#include "net_utils.h"
#include "xrServer_Objects_Alife.h"
#include "../skeletonanimated.h"

CTrigger::CTrigger()
{
}

u32 CTrigger::GetState			( ) const
{
	return m_state;
}
void CTrigger::SetState			( u32 state )
{
	if(m_state==state)
		return;
	u32 old_state = m_state;
	m_state=state;
	OnStateChanged(old_state, m_state);
}
void CTrigger::OnStateChanged		( u32 prev_state, u32 new_state )
{
}
void CTrigger::PlayAnim			( LPCSTR anim_name )
{
	CSkeletonAnimated	*A	= smart_cast<CSkeletonAnimated*>(Visual());
	if (A) {
		A->PlayCycle		(anim_name);
		A->CalculateBones	();
	}
}

void CTrigger::Load				( LPCSTR section )
{
	inherited::Load			( section );
}
BOOL CTrigger::net_Spawn			( CSE_Abstract* DC )
{
	inherited::net_Spawn			( DC );
	CSE_Abstract		*abstract	=(CSE_Abstract*)(DC);
	CSE_Trigger	*cse_trigger	= smart_cast<CSE_Trigger*>(abstract);
	m_state = cse_trigger->m_state;


	PlayAnim(*cse_trigger->startup_animation);

	setVisible			(true);
	setEnabled			(true);
	return TRUE;
}

void CTrigger::net_Destroy			()
{
	inherited::net_Destroy			();
}
void CTrigger::net_Export			( NET_Packet& P )
{
	inherited::net_Export			( P );
//	P.w_u32(m_state);
}
void CTrigger::net_Import			( NET_Packet& P )
{
	inherited::net_Import			( P );
//	m_state = P.r_u32();
}


*/