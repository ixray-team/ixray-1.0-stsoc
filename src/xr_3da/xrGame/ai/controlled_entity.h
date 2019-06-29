#pragma once

#include "monsters/controller/controller.h"

class CEntity;

enum ETask {
	eTaskFollow		= u32(0),
	eTaskAttack,
	eTaskNone		= u32(-1)
};

struct SControlledInfo {
	ETask			m_task;
	const CEntity	*m_object;
	Fvector			m_position;
	u32				m_node;		
	float			m_radius;
};

class CControlledEntityBase {
public:
	virtual bool			is_under_control	()								= 0;

	virtual void			set_data			(const SControlledInfo &info)	= 0;
	virtual SControlledInfo &get_data			()								= 0;

	virtual void			set_task_follow		(const CEntity *e)				= 0;
	virtual void			set_task_attack		(const CEntity *e)				= 0;
	
	virtual void			set_under_control	(CController *controller)		= 0;
	virtual void			free_from_control	(CController *controller)		= 0;
};


template<typename _Object>
class CControlledEntity : public CControlledEntityBase {
	bool				m_controlled;
	SControlledInfo		m_data;
	
	struct SGroupID {
		int team_id;
		int squad_id;
		int group_id;
	} saved_id;

	_Object				*m_object;

public:
							CControlledEntity	();
	virtual					~CControlledEntity	();

	virtual bool			is_under_control	() {return m_controlled;}

	virtual void			set_data			(const SControlledInfo &info) {m_data = info;}
	virtual SControlledInfo &get_data			(){return m_data;}

	virtual void			set_task_follow		(const CEntity *e);
	virtual void			set_task_attack		(const CEntity *e);

protected:
			void			init_external		(_Object *obj) {m_object = obj;}
			void			update				();

private:
	friend	class	CController;
	
	virtual void			set_under_control	(CController *controller); 
	virtual void			free_from_control	(CController *controller);
};

#include "controlled_entity_inline.h"

