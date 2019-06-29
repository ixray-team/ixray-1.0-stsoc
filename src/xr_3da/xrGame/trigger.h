/*
#pragma once
#include "gameObject.h"
#include "script_export_space.h"

class CTrigger :public CGameObject
{
	typedef CGameObject inherited ;
protected:
	u32		m_state;
public :

	CTrigger();

			u32				GetState			( ) const;
			void			SetState			( u32 state );
	virtual void			OnStateChanged		( u32 prev_state, u32 new_state );
			void			PlayAnim			( LPCSTR anim_name );
	// Methods
	virtual void			Load				( LPCSTR section );

	virtual BOOL			net_Spawn			( CSE_Abstract* DC );
	virtual void			net_Destroy			();
	virtual void			net_Export			( NET_Packet& P );
	virtual void			net_Import			( NET_Packet& P );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CTrigger)
#undef script_type_list
#define script_type_list save_type_list(CTrigger)
*/