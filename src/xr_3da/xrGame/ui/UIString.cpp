// UIString.cpp: объявление строки и функций работы с ней
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIString.h"

CUIString::CUIString()
{
	m_str.clear();
}

CUIString::CUIString(LPCSTR str)
{
	SetText(str);
}

CUIString::~CUIString()
{
	m_str.clear();
}
void CUIString::SetText(LPCSTR str) 
{
	m_str.clear();

	if(str == NULL) 
	{
		m_str.clear();
		return;
	}

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		m_str.push_back(str[i]);

	//символ конца строки
	m_str.push_back(0);
}


void CUIString::AppendText(LPCSTR str)
{
	//выкинуть символ конца строки
	m_str.pop_back();

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		m_str.push_back(str[i]);

	//символ конца строки
	m_str.push_back(0);
}

LPCSTR CUIString::GetBuf() const
{
	return &m_str.front();
}

CUIString::operator LPCSTR () const
{
	return (LPCSTR)GetBuf();
}
