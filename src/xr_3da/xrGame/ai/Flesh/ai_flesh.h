#pragma once
#include "../biting/ai_biting.h"
#include "../controlled_entity.h"

class CAI_Flesh : public CAI_Biting,
				  public CControlledEntity<CAI_Flesh> {

	typedef		CAI_Biting						inherited;
	typedef		CControlledEntity<CAI_Flesh>	controlled;

public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
	virtual	void	Load					(LPCSTR section);
	virtual	BOOL	net_Spawn				(LPVOID DC);
	virtual void    StateSelector			();

	virtual	void	CheckSpecParams			(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}
	
	CBitingSearchEnemy	*stateSearchEnemy;

private:
	bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
									Fvector SphereCenter, float SphereRadius);
};


