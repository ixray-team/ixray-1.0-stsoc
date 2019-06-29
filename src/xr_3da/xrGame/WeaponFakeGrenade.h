// WeaponFakeGrenade.h: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "gameobject.h"
#include "explosive.h"

class CWeaponMagazinedWGrenade;


class CWeaponFakeGrenade :	public CExplosive
{
private:
	//основное наследование от CGameObject
	typedef CGameObject inherited;
public:
	friend CWeaponMagazinedWGrenade;

	CWeaponFakeGrenade(void);
	virtual ~CWeaponFakeGrenade(void);


	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual BOOL UsedAI_Locations	()	{return	(FALSE);}

	virtual void Explode(const Fvector &pos, const Fvector &normal);
	virtual void Destroy();

	virtual void renderable_Render() {inherited::renderable_Render();}
	
	virtual bool	Useful	() const {return false;	}
protected:
	Fvector				m_pos, m_vel;
	CGameObject*		m_pOwner;

	enum EState{
		stInactive,
		stEngine,
		stFlying,
		stExplode
	};

	EState	m_state;
	
	int	m_dwEngineTime, m_explodeTime, m_flashTime;
	float m_mass, m_engine_f, m_engine_u;
	

	//высота подлета гранаты при взрыве
	float				m_fJumpHeight;

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);

public:
	virtual void			net_Import				(NET_Packet& P);					// import from server
	virtual void			net_Export				(NET_Packet& P);					// export to server
	virtual void			make_Interpolation		();
	virtual void			PH_B_CrPr				(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr				(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr				(); // actions & operations after phisic correction-prediction steps
	virtual void			activate_physic_shell	();
	virtual void			create_physic_shell		();
};
