#pragma once

#include "UIStatic.h"

class CUIStatix : public CUIStatic 
{
public:
					CUIStatix			();
	virtual			~CUIStatix			();

	virtual void 	Update				();
	virtual void 	OnFocusReceive		();
	virtual void 	OnFocusLost			();
	virtual bool 	OnMouseDown			(bool left_button = true);
			void 	SetSelectedState	(bool state);
			bool 	GetSelectedState	();

private:
	bool			m_bSelected;
	void			start_anim			();
	void			stop_anim			();
};
