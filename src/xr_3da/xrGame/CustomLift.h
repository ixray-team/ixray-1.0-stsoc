#ifndef __CUSTOM_LIFT_H__
#define __CUSTOM_LIFT_H__

class CCustomLift : public CObject  
{
	enum ELiftState{
		elsStop,
		elsMove
	};
	ELiftState		state;
	float			cur_floor;
	int				tgt_floor;
	int				floor_count;
private:
	typedef CObject	inherited;
public:
					CCustomLift	();
	virtual			~CCustomLift();

	virtual void	Load		(LPCSTR section);

	// Update
	virtual void	OnMove		();

	// Collision
	virtual void	OnNear		(CObject* near);

	void			Move		(int tgt_floor);
	void			Stop		();
};

#endif //__CUSTOM_LIFT_H__
