#pragma once

#include "UILabel.h"
#include "UIOptionsItem.h"

class CUIColorAnimatorWrapper;

char* GetKey(int dik);
char* GetKeyName(int dik);

class CUIEditKeyBind : public CUILabel, public CUIOptionsItem {
public:
					CUIEditKeyBind();
	virtual			~CUIEditKeyBind();
	// options item
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual	void	OnMessage(const char* message);
	virtual bool	IsChanged();

	// CUIWindow methods
	virtual void Init(float x, float y, float width, float height);	
	virtual void Update();
	virtual bool OnMouseDown(bool left_button = true );
	virtual void OnFocusLost();
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	// IUITextControl
	virtual void SetText(const char* text);

protected:
	virtual void InitTexture(LPCSTR texture, bool horizontal = true);

	bool		m_bEditMode;
	xr_string	m_val;
	bool		m_bChanged;

	CUIColorAnimatorWrapper* m_pAnimation;
};