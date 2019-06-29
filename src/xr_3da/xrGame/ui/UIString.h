// UIString.h: объявление строки и функций работы с ней
//
//////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_VECTOR	(char, STRING, STRING_IT);

class CUIString
{
public:
	CUIString();
	CUIString(LPCSTR str);
	virtual ~CUIString();
	
	CUIString& operator = (LPCSTR str) {*this = CUIString(str); return *this;}
	void SetText(LPCSTR str);
	void AppendText(LPCSTR str);
	
	LPCSTR GetBuf() const;
	operator LPCSTR () const;

	STRING m_str;
};
