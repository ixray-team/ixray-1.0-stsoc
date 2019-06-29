#pragma once
#include "UICharacterInfo.h"

class CUICharacterInfo;
class CInventoryOwner;
class CUIStatic;

class UIFollowerIcon: public CUICharacterInfo
{
	typedef CUICharacterInfo		inherited;
	enum{
		efiUICurrentActionIcon	=	0,
		efiUIHealthIcon,
		efiMax,
	};
	CUIStatic*	m_icons[efiMax];
public:
				UIFollowerIcon	();
	virtual		~UIFollowerIcon	();
			void Init			(Frect rect, const char* xml_name);
			void InitCharacter	(u16 id);
	virtual void Draw			();
	virtual void Update			();
};