#pragma once

enum EEventType {
	eventAnimationStart = u32(0),
	eventAnimationEnd,
	eventSoundStart,
	eventSoundEnd,
	eventParticlesStart,
	eventParticlesEnd,
	eventStep,
};

class CEvent {
	EEventType type;		
public:
				CEvent	(EEventType _type) : type(_type) {}
	EEventType	get_type() {return type;}
};

class IEventListener {
public:
	virtual bool	proper_event		(const CEvent *event)	= 0;
	virtual void	process_event		(const CEvent *event)	= 0;
};


class CListenerHolder {
	xr_vector<IEventListener *> m_listeners;
public:
	void	register_listener	(IEventListener *listener);
	void	remove_listener		(IEventListener *listener);

	void	process_event		(const CEvent *event);
};



