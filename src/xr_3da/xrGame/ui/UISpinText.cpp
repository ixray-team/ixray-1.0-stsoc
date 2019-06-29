// file:		UISpinNum.cpp
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinText.h"
#include "UILines.h"

CUISpinText::CUISpinText(){
    m_curItem = -1;
}

CUISpinText::~CUISpinText(){

}

void CUISpinText::AddItem(const char* item){
	m_list.push_back(item);
	if (-1 == m_curItem)
	{
		m_curItem = 0;
		SetItem();
	}
}

void CUISpinText::SetItem(){
	R_ASSERT(m_curItem != -1);
	m_pLines->SetText(m_list[m_curItem].c_str());
}

void CUISpinText::SetCurrentValue(){
	xr_token* tok = GetOptToken();

	while (tok->name){
		AddItem(tok->name);
		tok++;
	}
	xr_string val = GetOptTokenValue();

	for (u32 i = 0; i < m_list.size(); i++)
		if (val == m_list[i])
		{
			m_curItem = i;
			break;
		}

	SetItem();
}

void CUISpinText::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptTokenValue(m_pLines->GetText());
}

bool CUISpinText::IsChanged(){
	return 0 != xr_strcmp(GetOptTokenValue(), m_pLines->GetText());
}

void CUISpinText::OnBtnUpClick(){	
	if (CanPressUp())
	{
		m_curItem ++;
		SetItem();
	}

	CUICustomSpin::OnBtnUpClick();
}

void CUISpinText::OnBtnDownClick(){	
	if (CanPressDown())
	{
		m_curItem--;
		SetItem();
	}

	CUICustomSpin::OnBtnDownClick();
}

bool CUISpinText::CanPressUp(){
	return m_curItem < (int)m_list.size() - 1;

}
bool CUISpinText::CanPressDown(){
	return m_curItem > 0;
}