#include "stdafx.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../../xr_input.h"
#include "../xr_level_controller.h"

extern ENGINE_API BOOL bShowPauseString;

//-----------------------------------------------------------------------------
// Tutorial Item
//-----------------------------------------------------------------------------
CTutorialItem::~CTutorialItem()
{
	SubItemVecIt _I			= m_subitems.begin();
	SubItemVecIt _E			= m_subitems.end();
	for(;_I!=_E;++_I){
		SSubItem*& s		= *_I;
		s->Stop				();
		xr_delete			(s);
	}
	m_subitems.clear		();
	m_sound.stop			();
	delete_data				(m_UIWindow);
}

CUIWindow* find_child_window(CUIWindow* parent, const shared_str& _name)
{
	CUIWindow::WINDOW_LIST& wl		= parent->GetChildWndList();
	CUIWindow::WINDOW_LIST_it _I	= wl.begin();
	CUIWindow::WINDOW_LIST_it _E	= wl.end();
	for(;_I!=_E;++_I) if((*_I)->WindowName()==_name) return (*_I);
	return NULL;
}

void CTutorialItem::Load(CUIXml* xml, int idx)
{
	XML_NODE* _stored_root	= xml->GetLocalRoot();
	xml->SetLocalRoot		(xml->NavigateToNode("item",idx));
	
	LPCSTR m_snd_name		= xml->Read				("sound",0,""			);
	if (m_snd_name&&m_snd_name[0]){
		m_sound.create		(TRUE,m_snd_name,0);	
		VERIFY				(m_sound._handle());
	}
	m_time_length			= xml->ReadFlt			("length_sec",0,0		);
	m_desired_cursor_pos.x	= xml->ReadAttribFlt	("cursor_pos",0,"x",1024);
	m_desired_cursor_pos.y	= xml->ReadAttribFlt	("cursor_pos",0,"y",768	);
	strcpy					(m_pda_section, xml->Read("pda_section",0,"")	);

	LPCSTR str				= xml->Read				("pause_state",0,"ignore");

	m_flags.set										(etiNeedPauseOn, 0==_stricmp(str, "on"));
	m_flags.set										(etiNeedPauseOff, 0==_stricmp(str, "off"));

	str						= xml->Read				("guard_key",0,NULL		);
	m_continue_dik_guard	= -1;
	if (str && !_stricmp(str,"any")){
		m_continue_dik_guard = 9999;
		str					= NULL;
	}
	if(str){
		for(int i=0; keybind[i].name; ++i) {
			if(0==_stricmp(keybind[i].name,str)){
				m_continue_dik_guard = keybind[i].DIK;
				break;
			}
		}
	}

	m_flags.set						(etiCanBeStopped,	(m_continue_dik_guard==-1));
	m_flags.set						(etiGrabInput,		1==xml->ReadInt("grab_input",0,1));

	//ui-components
	m_UIWindow						= xr_new<CUIWindow>();
	m_UIWindow->SetAutoDelete		(false);
	XML_NODE* _lsr					= xml->GetLocalRoot();
	CUIXmlInit xml_init;
	xml_init.InitWindow				(*xml, "main_wnd", 0,	m_UIWindow);
	xml_init.InitAutoStaticGroup	(*xml, "main_wnd",		m_UIWindow);
	xml->SetLocalRoot				(_lsr);

	// initialize auto_static
	int cnt							= xml->GetNodesNum	("main_wnd",0,"auto_static");
	m_subitems.resize				(cnt,0);
	for(int i=0;i<cnt;++i){
		XML_NODE* _sr				= xml->GetLocalRoot();
		xml->SetLocalRoot			(xml->NavigateToNode("main_wnd",0));

		XML_NODE* _wn				= xml->NavigateToNode("auto_static",i);
		LPCSTR wnd_name				= xml->Read			(_wn,"window_name",0,"not_cpecified");
		LPCSTR	_tp					= xml->Read			(_wn,"type",0,"");
		SSubItem*& _si				= m_subitems[i];
		if (_tp&&(0==stricmp(_tp,"video"))){
			SVideoSubItem* _vsi		= xr_new<SVideoSubItem>();
			LPCSTR m_snd_name		= xml->Read			(_wn,"sound",0,"");
			if (m_snd_name&&m_snd_name[0]){
				string_path			_l, _r;
				strconcat			(_l, m_snd_name, "_l");
				strconcat			(_r, m_snd_name, "_r");
				_vsi->m_sound[0].create	(TRUE,_l,0);	VERIFY(_vsi->m_sound[0]._handle());
				_vsi->m_sound[1].create	(TRUE,_r,0);	VERIFY(_vsi->m_sound[1]._handle());
			}
			_si						= _vsi;
		}else{
			_si						= xr_new<SSubItem>();
		}
		_si->m_start				= xml->ReadAttribFlt("auto_static",i,"start_time",0);
		_si->m_length				= xml->ReadAttribFlt("auto_static",i,"length_sec",0);
		_si->m_visible				= false;
		_si->m_wnd					= smart_cast<CUIStatic*>(find_child_window(m_UIWindow, wnd_name)); VERIFY(_si->m_wnd);
		_si->m_wnd->Show			(false);

		xml->SetLocalRoot			(_sr);
	}
	xml->SetLocalRoot				(_stored_root);
}

void CTutorialItem::SSubItem::Start	()
{
	m_wnd->Show						(true);
	m_wnd->ResetAnimation			();
	m_visible						= true;
}

void CTutorialItem::SSubItem::Stop	()
{
	m_wnd->Show						(false);
	m_visible						= false;
}

void CTutorialItem::SVideoSubItem::Start()
{
	CTutorialItem::SSubItem::Start	();
	m_played						= false;
}

void CTutorialItem::SVideoSubItem::Stop()
{
	CTutorialItem::SSubItem::Stop	();
	m_sound[0].stop					();
	m_sound[1].stop					();
	m_texture						= 0;
}

void CTutorialItem::SVideoSubItem::Update()
{
	if (m_texture){
		if (m_texture->video_IsPlaying()){
			m_texture->video_Sync		(Device.dwTimeContinual);
		}else{
			// sync start
			if (false==m_played){
				m_sound[0].play_at_pos	(NULL, Fvector().set(-0.5f,0.f,0.3f), sm_2D);
				m_sound[1].play_at_pos	(NULL, Fvector().set(+0.5f,0.f,0.3f), sm_2D);
				m_texture->video_Play	(FALSE,Device.dwTimeContinual);
				m_played				= true;
			}
		}
	}
}

void CTutorialItem::SVideoSubItem::OnRender()
{
	VERIFY							(m_visible);
	if (NULL==m_texture && m_wnd->GetShader()){
		RCache.set_Shader					(m_wnd->GetShader());
		m_texture = RCache.get_ActiveTexture(0);
		m_texture->video_Stop				();
	}
}

void CTutorialItem::OnRender		()
{
	SubItemVecIt _I					= m_subitems.begin();
	SubItemVecIt _E					= m_subitems.end();
	for(;_I!=_E;++_I){
		SSubItem& s					= **_I;
		if (s.m_visible)			s.OnRender();
	}
}
void CTutorialItem::Update			()
{
	float gt						= float(Device.dwTimeContinual)/1000.0f;
	SubItemVecIt _I					= m_subitems.begin();
	SubItemVecIt _E					= m_subitems.end();
	for(;_I!=_E;++_I){
		SSubItem& s					= **_I;
		bool bPlaying				= (gt>(m_time_start+s.m_start))&&(gt<(m_time_start+s.m_start+s.m_length));
		if (true==bPlaying&&(false==s.m_visible))			s.Start	();
		else if ((false==bPlaying)&&(true==s.m_visible))	s.Stop	();
		if (s.m_visible)			s.Update();
	}
}

#include "../UIGameSp.h"
#include "../HUDManager.h"
#include "../level.h"
#include "UIPdaWnd.h"
void CTutorialItem::Start		(CUIGameTutorial* t)
{
	m_flags.set					(etiStoredPauseState, Device.Pause());
	
	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState)){
		Device.Pause			(TRUE);
		bShowPauseString		= FALSE;
	}

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	GetUICursor()->SetPos		(m_desired_cursor_pos.x, m_desired_cursor_pos.y);
	m_time_start				= float(Device.dwTimeContinual)/1000.0f;
	t->MainWnd()->AttachChild	(m_UIWindow);

	if (m_sound._handle())		m_sound.play(NULL, sm_2D);

	if (g_pGameLevel){
		bool bShowPda			= false;
		CUIGameSP* ui_game_sp	= smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(!stricmp(m_pda_section,"pda_contacts")){
			ui_game_sp->PdaMenu->SetActiveSubdialog(eptContacts);
			bShowPda = true;
		}else{
			if(!stricmp(m_pda_section,"pda_map")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptMap);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_quests")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptQuests);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_diary")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptDiary);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_ranking")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptRanking);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_statistics")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptActorStatistic);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_encyclopedia")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptEncyclopedia);
				bShowPda = true;
			}
		}
		if( (!ui_game_sp->PdaMenu->IsShown() && bShowPda) || 
			(ui_game_sp->PdaMenu->IsShown() && !bShowPda))
			HUD().GetUI()->StartStopMenu			(ui_game_sp->PdaMenu,true);
	}
}

bool CTutorialItem::Stop			(CUIGameTutorial* t, bool bForce)
{
	if(!m_flags.test(etiCanBeStopped)&&!bForce) 
		return false;

	t->MainWnd()->DetachChild	(m_UIWindow);
	m_sound.stop				();

	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(TRUE);

	return true;
}

void CTutorialItem::OnKeyboardPress	(int dik)
{
	if	(!m_flags.test(etiCanBeStopped) )
	{
		VERIFY		(m_continue_dik_guard!=-1);
		if(m_continue_dik_guard==-1)m_flags.set(etiCanBeStopped, TRUE); //not binded action :(

		if(m_continue_dik_guard==9999 || key_binding[dik] == m_continue_dik_guard)
			m_flags.set(etiCanBeStopped, TRUE); //match key

	}
}
