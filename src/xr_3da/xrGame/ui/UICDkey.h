//
#pragma once
#include "UIEditBox.h"

class CUICDkey : public CUIEditBox {
public:
	CUICDkey();
	~CUICDkey();
	// 
	virtual	void	SetText			(LPCSTR str) {}
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();
	
			void	CreateCDKeyEntry();
			void	GetCDKey(char* CDKeyStr);

	virtual void	Draw();
	virtual void	AddChar(char c);

private:
			LPCSTR	AddHyphens(LPCSTR str);
			LPCSTR	DelHyphens(LPCSTR str);

};