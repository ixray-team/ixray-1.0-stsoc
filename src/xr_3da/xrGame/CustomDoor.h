#ifndef __CUSTOM_DOOR_H__
#define __CUSTOM_DOOR_H__

class CCustomDoor : public CObject  
{
	enum EDoorState{
		edsOpen,
		edsClose,
		edsOpening,
		edsClosing
	};
	EDoorState		state;
private:
	typedef CObject	inherited;

	ref_sound			sndOpenClose;
public:
					CCustomDoor	();
	virtual			~CCustomDoor();

	virtual void	Load		(LPCSTR section);

	// Update
	virtual void	OnMove		();

	// Collision
	virtual void	OnNear		(CObject* near);

	void			Open		();
	void			Close		();
};

#endif //__CUSTOM_DOOR_H__
