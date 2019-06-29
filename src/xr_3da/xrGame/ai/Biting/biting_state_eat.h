#pragma once

#include "../../state_manager_state.h"
#include "../../state_base.h"

class CAI_Biting;

class CStateBitingEat : public CStateManagerState<CAI_Biting> {
protected:
	typedef CStateManagerState<CAI_Biting> inherited;

	enum EEatStates {
		eES_ApproachCorpse	= u32(0),
		eES_Eat,
		eES_PrepareDrag,
		eES_Drag
	};
public:
						CStateBitingEat		(LPCSTR state_name);
	virtual				~CStateBitingEat	();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Biting *object);
	
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

//////////////////////////////////////////////////////////////////////////

class CStateBitingEatCheckCorpse : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingEatCheckCorpse	(LPCSTR state_name) : inherited(state_name) {}

	virtual	void		initialize					();
	virtual	void		execute						();
	virtual	void		finalize					();

	virtual	bool		completed					() const;
};

