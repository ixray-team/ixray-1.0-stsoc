//-----------------------------------------------------------------------------/
//  Коммуникационный поддиалог PDA
//-----------------------------------------------------------------------------/

#include "stdafx.h"
#include "UIPdaCommunication.h"
#include "UIPdaContactsWnd.h"

#include "../Pda.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../HUDManager.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"

#include "../PhraseDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaCommunication::CUIPdaCommunication()
{
//	Show(false);

	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().Font().pFontMedium);
	UIPdaContactsWnd	= NULL;
//	m_pOurDialogManager = m_pOthersDialogManager = NULL;

//	ToTopicMode();

//	m_bNeedToUpdateQuestions = false;
}

CUIPdaCommunication::~CUIPdaCommunication()
{
}

void CUIPdaCommunication::Init()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH,PDA_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, float(Device.dwWidth), float(Device.dwHeight));

	//окно разговора по PDA
//	AttachChild(&UIPdaDialogWnd);
//	UIPdaDialogWnd.Init(0, 0, GetWidth(), GetHeight());
//	UIPdaDialogWnd.Show(false);

	//список контактов
	UIPdaContactsWnd = xr_new<CUIPdaContactsWnd>(); UIPdaContactsWnd->SetAutoDelete(true);
	AttachChild(UIPdaContactsWnd);
	UIPdaContactsWnd->Init(0,0, GetWidth(), GetHeight());
	UIPdaContactsWnd->Show(false);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUIPdaCommunication::InitPDACommunication()
{
	m_pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	m_pPda = m_pInvOwner->GetPDA();

	if(!m_pPda)	return;

	InitPdaContacts();

//	UIPdaDialogWnd.Show(false);
	UIPdaContactsWnd->Show(true);
}


void CUIPdaCommunication::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIPdaCommunication::Update()
{
	if(!m_pPda) return;

	//обновлять список контактов вне зависимости
	//от того показано окно контактов или нет
	UpdatePdaContacts();


	inherited::Update();
}

void CUIPdaCommunication::Draw()
{
	inherited::Draw();
}

void CUIPdaCommunication::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		InitPDACommunication();
	}
}


/////////////////////////////////////////
// Функции работы с котактами
////////////////////////////////////////
void CUIPdaCommunication::InitPdaContacts()
{
	UIPdaContactsWnd->RemoveAll();

	xr_vector<CPda*> pda_list;
	m_pPda->ActiveContacts(pda_list);

	xr_vector<CPda*>::iterator it = pda_list.begin();

	for(; it!=pda_list.end();++it){
		UIPdaContactsWnd->AddContact(*it);
	}
}

//обновление списка активных контактов PDA
void CUIPdaCommunication::UpdatePdaContacts()
{
	xr_vector<CPda*> pda_list;
	m_pPda->ActiveContacts(pda_list);

	xr_vector<CPda*>::iterator it = pda_list.begin();

	for(; it!=pda_list.end();++it){
		CPda* pPda = (*it);
		if(!UIPdaContactsWnd->IsInList(pPda))
			UIPdaContactsWnd->RemoveContact(pPda);
	}
}
