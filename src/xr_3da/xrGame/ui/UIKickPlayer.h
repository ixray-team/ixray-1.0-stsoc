#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIFrameWindow;
class CUIListBox;
class CUIXml;

class CUIKickPlayer : public CUIDialogWnd {
public:
	using CUIDialogWnd::Init;

	CUIKickPlayer();

			void InitKick(CUIXml& xml_doc);
			void InitBan(CUIXml& xml_doc);

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);
	virtual void Update();

	void OnBtnOk();
	void OnBtnCancel();

protected:
	typedef enum{
		MODE_KICK,
		MODE_BAN
	} E_MODE;

	E_MODE			mode;
	void Init(CUIXml& xml_doc);

	CUIStatic*		bkgrnd;
	CUIStatic*		header;
	CUIFrameWindow* lst_back;
	CUIListBox*		lst;

	CUI3tButton*	btn_ok;
	CUI3tButton*	btn_cancel;

	u32				m_prev_upd_time;

	u32				selected_item_id;
	shared_str		selected_item_text;
};