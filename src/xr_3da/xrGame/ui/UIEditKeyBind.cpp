#include "stdafx.h"

#include "UIEditKeyBind.h"
#include "UIColorAnimatorWrapper.h"

#include "../xr_level_controller.h"
#include "../object_broker.h"

CUIEditKeyBind::CUIEditKeyBind(){
    m_bEditMode = false;

	m_pAnimation = xr_new<CUIColorAnimatorWrapper>("ui_map_area_anim");
	m_pAnimation->Cyclic(true);
	m_bChanged = false;
	m_lines.SetTextComplexMode(false);
}
CUIEditKeyBind::~CUIEditKeyBind()
{
	delete_data(m_pAnimation);
}

void CUIEditKeyBind::SetText(const char* text){
	if (!text || 0 == xr_strlen(text))
		CUILabel::SetText("<Empty>");
	else
		CUILabel::SetText(text);
}

void CUIEditKeyBind::Init(float x, float y, float width, float height){
	CUILabel::Init(x,y,width,height);
	InitTexture("ui_options_string");
}

void CUIEditKeyBind::InitTexture(LPCSTR texture, bool bHorizontal){
	CUILabel::InitTexture(texture,bHorizontal);
}

void CUIEditKeyBind::OnFocusLost(){
	CUILabel::OnFocusLost		();
	m_bEditMode					= false;
	m_lines.SetTextColor		((subst_alpha(m_lines.GetTextColor(), color_get_A(0xffffffff))));
}

extern _keybind keynames[];

bool CUIEditKeyBind::OnMouseDown(bool left_button){
	if (m_bEditMode){		
		if (left_button){
			m_val = GetKey(MOUSE_1);
			SetText(GetKeyName(MOUSE_1));
		}
		else
		{
			m_val = GetKey(MOUSE_2);
			SetText(GetKeyName(MOUSE_2));
		}
		OnFocusLost();
		m_bChanged = true;
		return true;
	}

	if (left_button)
		m_bEditMode = m_bCursorOverWindow;

	return CUILabel::OnMouseDown(left_button);
}

bool CUIEditKeyBind::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (dik == MOUSE_1 || dik == MOUSE_2 || dik == MOUSE_3)
		return false;
	if (CUILabel::OnKeyboard(dik, keyboard_action))
		return true;

	string64 message;
	if (m_bEditMode)
	{		
		m_val = GetKey(dik);
		strcpy(message,m_entry.c_str());
		const char* kn = GetKeyName(dik);
		strcat(message,"=");strcat(message,kn);		
		SetText(kn);
		OnFocusLost();
		m_bChanged = true;
		SendMessage2Group("key_binding",message);
		return true;
	}
	return false;
}

void CUIEditKeyBind::Update(){
	CUILabel::Update();

	m_bTextureAvailable = m_bCursorOverWindow;
	if (m_bEditMode)
	{
		m_pAnimation->Update();
		m_lines.SetTextColor((subst_alpha(m_lines.GetTextColor(), color_get_A(m_pAnimation->GetColor()))));
	}
	
}

void CUIEditKeyBind::SetCurrentValue(){
	string64 buff;
	ZeroMemory(buff,sizeof(buff));
	GetActionBinding(m_entry.c_str(), buff);
	SetText(buff);
	ZeroMemory(buff,sizeof(buff));
	GetActionBindingEx(m_entry.c_str(), buff);
	m_val = buff;
}

void CUIEditKeyBind::SaveValue(){
	CUIOptionsItem::SaveValue();

    BindAction2Key(m_val.c_str());
	m_bChanged = false;
}

bool CUIEditKeyBind::IsChanged(){
	return m_bChanged;
}

void CUIEditKeyBind::OnMessage(const char* message){
	// message = "command=key"
	int eq = (int)strcspn(message,"=");
	if (0 != xr_strcmp(m_lines.GetText(),message + eq + 1))
		return;

	string64 command;
	strcpy(command,message);
	command[eq] = 0;

    if (0 == xr_strcmp(m_entry.c_str(),command))
		return;

	SetText("");
	m_val = "";
}

string64 get_key_str;

char* GetKey(int dik){
	int i = 0;
	while (keynames[i].name)
	{
		if (keynames[i].DIK == dik)            
			return keynames[i].name;
		i++;
	}

	ZeroMemory(get_key_str,sizeof(get_key_str));
	return get_key_str;
}

char* GetKeyName(int dik){
	ZeroMemory(get_key_str,sizeof(get_key_str));
	int i = 0;
	while (keynames[i].name)
	{
		if (keynames[i].DIK == dik)
		{
			strcpy(get_key_str, (keynames[i].name[0]=='k')? keynames[i].name+1 : keynames[i].name);
			return get_key_str;
		}
		i++;
	}
	return get_key_str;
}