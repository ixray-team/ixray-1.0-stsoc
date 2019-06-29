//-----------------------------------------------------------------------------/
//  Коммуникационный поддиалог PDA
//-----------------------------------------------------------------------------/

#ifndef UI_PDA_COMMUNICATION_H_
#define UI_PDA_COMMUNICATION_H_

/*
#include "UIWindow.h"


class CInventoryOwner;
class CActor;
class CPhraseDialogManager;
class CUIPdaContactsWnd;
class CPda;
///////////////////////////////////////
// Подложка и основные кнопки PDA
///////////////////////////////////////

class CUIPdaCommunication: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaCommunication();
	virtual ~CUIPdaCommunication();

			void Init();
			void InitPDACommunication();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	virtual void Show(bool status);

protected:
	//контакты
			void InitPdaContacts();
			void UpdatePdaContacts();

	//диалог
//	virtual void InitPdaDialog();
//	virtual void UpdateMessageLog();
//	virtual void UpdateMsgButtons();
public:
//	virtual void UpdateDisplay();
protected:

//	CUIFrameWindow		UIMainPdaFrame;
	CUIPdaContactsWnd*	UIPdaContactsWnd;
	//указатель на владельца инвентаря вызвавшего менюшку
	CInventoryOwner*	m_pInvOwner;
	CPda*				m_pPda;

//	CUIPdaDialogWnd		UIPdaDialogWnd;


	//указатели на текущий контакт
//	u32					m_idContact;
//	CObject*			m_pContactObject;
//	CInventoryOwner*	m_pContactInvOwner;
//	CPda*				m_pContactPda;

	//потеря контакта во время диалога
//	bool m_bContactLoss;


	//указатель на владельца инвентаря вызвавшего менюшку
	//и его собеседника
	CActor*			 m_pActor;
	CInventoryOwner* m_pOurInvOwner;
//	CInventoryOwner* m_pOthersInvOwner;

//	CPhraseDialogManager* m_pOurDialogManager;
//	CPhraseDialogManager* m_pOthersDialogManager;

	//текущий диалог, если NULL, то переходим в режим выбора темы
//	DIALOG_SHARED_PTR m_pCurrentDialog;
//	PHRASE_ID		  m_iLastPhraseID;

//	bool TopicMode	();
//	void ToTopicMode();

	//диалог
//	void InitTalkDialog();
//	void UpdateQuestions();
//	void AskQuestion();

//	void SayPhrase(PHRASE_ID phrase_id);

	// Функции добавления строк в листы вопросов и ответов
//	void AddQuestion(CUIString str, void* pData, int value = 0);
//	void AddAnswer(CUIString str, const CUIString &SpeakerName);
//public:
//	void NeedUpdateQuestions();

//protected:
//	bool m_bNeedToUpdateQuestions;
};
*/
#endif