#pragma once

class CBurer;

class CBurerScan : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

public:	
					CBurerScan		(CBurer *p) : pMonster(p) {}
	virtual void	Run				();
};
