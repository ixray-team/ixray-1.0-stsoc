#include "stdafx.h"
#include "UITalkWnd.h"

#include "UITradeWnd.h"
#include "UITalkDialogWnd.h"

#include "../actor.h"
#include "../HUDManager.h"
#include "../UIGameSP.h"
#include "../PDA.h"
#include "../character_info.h"
#include "../level.h"

#include "../PhraseDialog.h"
#include "../PhraseDialogManager.h"

#include "../game_cl_base.h"
#include "../string_table.h"
#include "../xr_level_controller.h"
#include "../../cameraBase.h"
#include "UIXmlInit.h"

CUITalkWnd::CUITalkWnd()
{
	m_pActor				= NULL;

	m_pOurInvOwner			= NULL;
	m_pOthersInvOwner		= NULL;

	m_pOurDialogManager		= NULL;
	m_pOthersDialogManager	= NULL;

	ToTopicMode				();

	Init					();
	Hide					();
//.	SetFont					(HUD().Font().pFontHeaderRussian);

	m_bNeedToUpdateQuestions = false;
}

//////////////////////////////////////////////////////////////////////////

CUITalkWnd::~CUITalkWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Init()
{
	inherited::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	/////////////////////////
	//Меню разговора
	UITalkDialogWnd = xr_new<CUITalkDialogWnd>();UITalkDialogWnd->SetAutoDelete(true);
	AttachChild(UITalkDialogWnd);
	UITalkDialogWnd->Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	/////////////////////////
	//Меню торговли
	UITradeWnd = xr_new<CUITradeWnd>();UITradeWnd->SetAutoDelete(true);
	AttachChild(UITradeWnd);
	UITradeWnd->Hide();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::InitTalkDialog()
{
	m_pActor = smart_cast<CActor *>(Level().CurrentEntity());
	if (m_pActor && !m_pActor->IsTalking()) return;

	m_pOurInvOwner = smart_cast<CInventoryOwner*>(m_pActor);
	m_pOthersInvOwner = m_pActor->GetTalkPartner();

	m_pOurDialogManager = smart_cast<CPhraseDialogManager*>(m_pOurInvOwner);
	m_pOthersDialogManager = smart_cast<CPhraseDialogManager*>(m_pOthersInvOwner);

	//имена собеседников
	UITalkDialogWnd->UICharacterInfoLeft.InitCharacter(m_pOurInvOwner->object_id());
	UITalkDialogWnd->UICharacterInfoRight.InitCharacter(m_pOthersInvOwner->object_id());
	UITalkDialogWnd->UIDialogFrame.UITitleText.SetText(m_pOthersInvOwner->Name());
	UITalkDialogWnd->UIOurPhrasesFrame.UITitleText.SetText(m_pOurInvOwner->Name());
	
	//очистить лог сообщений
	UITalkDialogWnd->ClearAll();

	InitOthersStartDialog					();
	NeedUpdateQuestions						();
	Update									();

	UITalkDialogWnd->Show					();
	UITradeWnd->Hide							();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::InitOthersStartDialog()
{
	m_pOthersDialogManager->UpdateAvailableDialogs(m_pOurDialogManager);
	if(!m_pOthersDialogManager->AvailableDialogs().empty())
	{
		m_pCurrentDialog = m_pOthersDialogManager->AvailableDialogs().front();
		m_pOthersDialogManager->InitDialog(m_pOurDialogManager, m_pCurrentDialog);
		
		//сказать фразу
		CStringTable stbl;
		AddAnswer(m_pCurrentDialog->GetPhraseText(0), m_pOthersInvOwner->Name());
		m_pOthersDialogManager->SayPhrase(m_pCurrentDialog, 0);

		//если диалог завершился, перейти в режим выбора темы
		if(!m_pCurrentDialog || m_pCurrentDialog->IsFinished()) ToTopicMode();
	}
}

//////////////////////////////////////////////////////////////////////////
void CUITalkWnd::NeedUpdateQuestions()
{
	m_bNeedToUpdateQuestions = true;
}

void CUITalkWnd::UpdateQuestions()
{
	UITalkDialogWnd->ClearQuestions();
	R_ASSERT2(m_pOurInvOwner->GetPDA(), "PDA for character does not init yet");

	//если нет активного диалога, то
	//режима выбора темы
	if(!m_pCurrentDialog)
	{
		m_pOurDialogManager->UpdateAvailableDialogs(m_pOthersDialogManager);
		for(u32 i=0; i< m_pOurDialogManager->AvailableDialogs().size(); i++)
		{
			const DIALOG_SHARED_PTR& phrase_dialog = m_pOurDialogManager->AvailableDialogs()[i];
			AddQuestion(phrase_dialog->DialogCaption(), (int)i);
		}
	}
	else
	{
		if(m_pCurrentDialog->IsWeSpeaking(m_pOurDialogManager))
		{
			//если в списке допустимых фраз только одна фраза пустышка, то просто
			//сказать (игрок сам не производит никаких действий)
			if( !m_pCurrentDialog->PhraseList().empty() && m_pCurrentDialog->allIsDummy() ){
				CPhrase* phrase = m_pCurrentDialog->PhraseList()[Random.randI(m_pCurrentDialog->PhraseList().size())];
				SayPhrase(phrase->GetIndex());
			};

			//выбор доступных фраз из активного диалога
			if( m_pCurrentDialog && !m_pCurrentDialog->allIsDummy() )
			{			
				for(PHRASE_VECTOR::const_iterator   it = m_pCurrentDialog->PhraseList().begin();
					it != m_pCurrentDialog->PhraseList().end();
					it++)
				{
					CPhrase* phrase = *it;
					AddQuestion(phrase->GetText(), (int)phrase->GetIndex());
				}
			}
			else
				UpdateQuestions();
		}
	}
	m_bNeedToUpdateQuestions = false;

}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == UITalkDialogWnd && msg == TALK_DIALOG_TRADE_BUTTON_CLICKED)
	{
		SwitchToTrade();
	}
	else if(pWnd == UITalkDialogWnd && msg == TALK_DIALOG_QUESTION_CLICKED)
	{
		AskQuestion();
	}
	else if(pWnd == UITradeWnd && msg == TRADE_WND_CLOSED)
	{
		UITalkDialogWnd->Show();
		UITradeWnd->Hide();
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////
void UpdateCameraDirection(CGameObject* pTo)
{
	CCameraBase* cam = Actor()->cam_Active();

	Fvector des_dir; 
	Fvector des_pt;
	pTo->Center(des_pt);
	des_pt.y+=pTo->Radius()*0.5f;

	des_dir.sub(des_pt,cam->vPosition);

	float p,h;
	des_dir.getHP(h,p);


	if(angle_difference(cam->yaw,-h)>0.2)
		cam->yaw		= angle_inertion_var(cam->yaw,		-h,	0.15f,	0.2f,	PI_DIV_6,	Device.fTimeDelta);

	if(angle_difference(cam->pitch,-p)>0.2)
		cam->pitch		= angle_inertion_var(cam->pitch,	-p,	0.15f,	0.2f,	PI_DIV_6,	Device.fTimeDelta);

}

void CUITalkWnd::Update()
{
	//остановить разговор, если нужно
	if (m_pActor && !m_pActor->IsTalking() )
	{
		Game().StartStopMenu(this,true);
	}else{
		CGameObject* pOurGO = smart_cast<CGameObject*>(m_pOurInvOwner);
		CGameObject* pOtherGO = smart_cast<CGameObject*>(m_pOthersInvOwner);
		
		if(NULL==pOurGO || NULL==pOtherGO || pOurGO->Position().distance_to(pOtherGO->Position())>3.0f )
			Game().StartStopMenu(this,true);
	}

	if(m_bNeedToUpdateQuestions)
	{
		UpdateQuestions			();
	}
	inherited::Update			();
	UpdateCameraDirection		(smart_cast<CGameObject*>(m_pOthersInvOwner));

}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Draw()
{
	inherited::Draw				();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Show()
{
	InitTalkDialog				();
	inherited::Show				();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Hide()
{
	StopSnd						();

	inherited::Hide				();
	UITradeWnd->Hide				();
	if(!m_pActor)				return;
	
	ToTopicMode					();

	if (m_pActor->IsTalking()) m_pActor->StopTalk();
	m_pActor = NULL;
}

//////////////////////////////////////////////////////////////////////////

bool  CUITalkWnd::TopicMode			() 
{
	return NULL == m_pCurrentDialog.get();
}

//////////////////////////////////////////////////////////////////////////

void  CUITalkWnd::ToTopicMode		() 
{
	m_pCurrentDialog = DIALOG_SHARED_PTR((CPhraseDialog*)NULL);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AskQuestion()
{
	if(m_bNeedToUpdateQuestions) return;//quick dblclick:(
	int phrase_id;

	//игрок выбрал тему разговора
	if(TopicMode())
	{
		if ( (UITalkDialogWnd->m_iClickedQuestion < 0) ||
			(UITalkDialogWnd->m_iClickedQuestion >= (int)m_pOurDialogManager->AvailableDialogs().size()) ) {

			string128	s;
			sprintf		(s,"ID = [%i] of selected question is out of range of available dialogs ",UITalkDialogWnd->m_iClickedQuestion);
			VERIFY2(FALSE, s);
		}

		m_pCurrentDialog = m_pOurDialogManager->AvailableDialogs()[UITalkDialogWnd->m_iClickedQuestion];
		
		m_pOurDialogManager->InitDialog(m_pOthersDialogManager, m_pCurrentDialog);
		phrase_id = 0;
	}
	else
	{
		phrase_id = (int)UITalkDialogWnd->m_iClickedQuestion;
	}

	SayPhrase(phrase_id);
	NeedUpdateQuestions();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::SayPhrase(int phrase_id)
{

	AddAnswer(m_pCurrentDialog->GetPhraseText(phrase_id), m_pOurInvOwner->Name());
	m_pOurDialogManager->SayPhrase(m_pCurrentDialog, phrase_id);
/*
	//добавить ответ собеседника в список, если он что-то сказал
	if(m_pCurrentDialog->GetLastPhraseID() !=  phrase_id)
		AddAnswer(m_pCurrentDialog->GetLastPhraseText(), m_pOthersInvOwner->Name());
*/
	//если диалог завершился, перейти в режим выбора темы
	if(m_pCurrentDialog->IsFinished()) ToTopicMode();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AddQuestion(LPCSTR text, int value)
{
	if(xr_strlen(text) == 0) return;
	UITalkDialogWnd->AddQuestion(*CStringTable().translate(text),value);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AddAnswer(LPCSTR text, const char* SpeakerName)
{
	//для пустой фразы вообще ничего не выводим
	if(xr_strlen(text) == 0) return;
	PlaySnd			(text);

	bool i_am = (0 == xr_strcmp(SpeakerName, m_pOurInvOwner->Name()));
	UITalkDialogWnd->AddAnswer(SpeakerName,*CStringTable().translate(text),i_am);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::SwitchToTrade()
{
	if(m_pOurInvOwner->IsTradeEnabled() && m_pOthersInvOwner->IsTradeEnabled() ){

		UITalkDialogWnd->Hide		();

		UITradeWnd->InitTrade		(m_pOurInvOwner, m_pOthersInvOwner);
		UITradeWnd->Show				();
		UITradeWnd->StartTrade		();
		UITradeWnd->BringAllToTop	();
		StopSnd						();
	}
}

bool CUITalkWnd::IR_OnKeyboardPress(int dik)
{
//.	StopSnd						();
	int cmd = key_binding[dik];
	if(cmd==kUSE)
	{
		GetHolder()->StartStopMenu(this, true);
		return true;
	}
	return inherited::IR_OnKeyboardPress(dik);
}

void CUITalkWnd::PlaySnd(LPCSTR text)
{
	if(xr_strlen(text) == 0) return;
	StopSnd						();
	
	string_path	fn;
	strconcat(fn, "characters_voice\\dialogs\\", text, ".ogg");
	if(FS.exist("$game_sounds$",fn)){
		VERIFY(m_pActor);
		if (!m_pActor->OnDialogSoundHandlerStart(m_pOthersInvOwner,fn)) {
			m_sound.create(fn,st_Effect,sg_SourceType);
			m_sound.play(0,sm_2D);
		}
	}
}

void CUITalkWnd::StopSnd()
{
	if (m_pActor && m_pActor->OnDialogSoundHandlerStop(m_pOthersInvOwner)) return;

	if(m_sound._feedback()) 
		m_sound.stop	();
}

void CUITalkWnd::AddIconedMessage(LPCSTR text, LPCSTR texture_name, Frect texture_rect, LPCSTR templ_name)
{
	UITalkDialogWnd->AddIconedAnswer(text, texture_name, texture_rect, templ_name);
}

void CUITalkWnd::Stop()
{
}