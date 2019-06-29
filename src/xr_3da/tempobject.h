#ifndef _INCDEF_TempObject_H_
#define _INCDEF_TempObject_H_

class ENGINE_API IVisual;

class ENGINE_API CTempObject : public CSheduled
{
protected:
	IVisual*			m_pVisual;
	int					m_iLifeTime;
	BOOL				m_bAutoRemove;
public:
						CTempObject		();
	virtual				~CTempObject	();

	IC BOOL				Alive			(){return m_iLifeTime>0;}
	IC BOOL				IsAutomatic		(){return m_bAutoRemove;}
	IC IVisual*			Visual			(){return m_pVisual;}
	IC void				SetLifeTime		(float life_time){m_iLifeTime=iFloor(life_time*1000);}
	
	virtual void		Update			(u32 dt);
	virtual BOOL		Ready			()  { return TRUE; }
};
#endif /*_INCDEF_TempObject_H_*/

