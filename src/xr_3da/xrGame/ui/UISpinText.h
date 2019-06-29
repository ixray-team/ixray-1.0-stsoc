// file:		UISpinNum.h
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinText : public CUICustomSpin{
public:
	CUISpinText();
	~CUISpinText();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();	

	// own
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();

			void	AddItem(const char* item);

protected:
	virtual bool	CanPressUp();
	virtual bool	CanPressDown();
	void	SetItem();
	typedef xr_vector<xr_string>			Items;
	typedef xr_vector<xr_string>::iterator	Items_it;

    Items	m_list;
	int		m_curItem;
};