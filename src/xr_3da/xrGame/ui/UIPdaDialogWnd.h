////////////////////////////////////////////////
//	UIPdaDialogWnd.h
// меню диалога со сталкером в PDA
////////////////////////////////////////////////


#pragma once

#include "UIFrameWindow.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"
#include "UIAnimatedStatic.h"

class CUIPdaDialogWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaDialogWnd();
	virtual ~CUIPdaDialogWnd();

	virtual void Init(float x, float y, float width, float height);

//	typedef enum{BACK_BUTTON_CLICKED,
//				 MESSAGE_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void Show(bool status);

	virtual void Update();

	void ContactLoss();
	void ContactRestore();
	void ContactWaitForReply();

	u32			GetHeaderColor()		{ return m_iNameTextColor; }
	CGameFont *	GetHeaderFont()			{ return m_pNameTextFont; }
	u32			GetOurReplicsColor()	{ return m_uOurReplicsColor; }

private:
	// Ўрифт и цвет текста с именем персонажа
	CGameFont			*m_pNameTextFont;
	u32					m_iNameTextColor;
	// ÷вет тeкста и шрифт наших реплик
	u32					m_uOurReplicsColor;

public:
	//номер выбранного вопроса
	int m_iClickedQuestion;

	//номер отправленного сообщени€
	EPdaMsg m_iMsgNum;

	bool m_bContactActive;
	
	//лог сообщений
	CUIListWnd UILogListWnd;
	//список фраз которые может говорить игрок
	CUIListWnd UIPhrasesListWnd;

	// ‘реймы дикоративного оформлени€
	CUIFrameWindow		UIMsglogFrame;
	CUIFrameWindow		UICharIconFrame;
	CUIFrameWindow		UIPhrasesFrame;
	CUIFrameLineWnd		UIMsglogHeader;
	CUIFrameLineWnd		UICharIconHeader;
	CUIFrameLineWnd		UIPhrasesHeader;
	CUIAnimatedStatic	UIAnimatedIcon;

	//информаци€ о персонаже
	CUIWindow			UICharacterWindow;
	CUICharacterInfo	UICharacterInfo;
	CUIFrameWindow		UIMask;
};