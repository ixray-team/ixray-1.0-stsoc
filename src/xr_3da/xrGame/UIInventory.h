#pragma once


#include "ui\UIInventoryWnd.h"
#include "UICursor.h"
#include "UI.h"


class CUIInventory
{
public:
	CUIInventory(void);
	virtual ~CUIInventory(void);

	//by Dandy
	void Init();

	void Show();
	void Hide();
	bool Visible() { return m_active; }

	virtual void Render();
	virtual void OnFrame();
	virtual bool IR_OnKeyboardPress(int dik);
	virtual bool IR_OnKeyboardRelease(int dik);
	
	//by Dandy
	virtual bool IR_OnMouseMove(int dx, int dy);
	//end

	bool m_active, m_shift, m_deleteNext;
	f32 m_rowOffs, m_rowSkip, m_colOffs[3];


protected:
	CUIInventoryWnd UIInventoryWnd;
	CUICursor* m_pUICursor;
	CUI* m_pUI;
	bool m_bCrosshair;
};
