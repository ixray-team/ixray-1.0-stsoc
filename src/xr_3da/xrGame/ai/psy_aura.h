#pragma once
#include "../../feel_touch.h"
#include "energy_holder.h"

class CGameObject;

class CPsyAura : public Feel::Touch, public CEnergyHolder {
	
	typedef CEnergyHolder inherited;

	// владелец поля
	CGameObject	*m_object;

	// радиус поля
	float		m_radius;

public:
					CPsyAura				();
	virtual			~CPsyAura				();
	
			void	init_external			(CGameObject *obj) {m_object = obj;}
	virtual BOOL	feel_touch_contact		(CObject* O){return FALSE;}
	virtual	void	schedule_update			();
	virtual void	process_objects_in_aura	() {}

	// свойства поля
			void	set_radius				(float R) {m_radius = R;}
			float	get_radius				(){return m_radius;}

		CGameObject	*get_object				(){return m_object;}
};



