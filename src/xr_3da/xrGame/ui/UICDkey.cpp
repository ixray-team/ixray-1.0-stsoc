
#include "stdafx.h"
#include "UICDkey.h"
#include "UIColorAnimatorWrapper.h"

extern string64	gsCDKey;

CUICDkey::CUICDkey(){
	CreateCDKeyEntry();
	SetCurrentValue();
}

CUICDkey::~CUICDkey(){	


}

void CUICDkey::Draw(){
	Frect			rect;
	GetAbsoluteRect	(rect);
	float outX, outY;

	outY = (m_wndSize.y - m_lines.m_pFont->CurrentHeight_/*Rel*/())/2;
	outX = 0;
	m_lines.m_pFont->SetColor(m_lines.GetTextColor());

	Fvector2 pos;
	pos.set						(rect.left+outX, rect.top+outY);
	UI()->ClientToScreenScaled	(pos);
	if(m_bInputFocus)
	{		
		m_lines.m_pFont->Out( pos.x, pos.y, "%s" ,AddHyphens(m_lines.m_text.c_str()) );
	
		outY = (m_wndSize.y - m_lines.m_pFont->CurrentHeight_/*Rel*/())/2;
		outX = m_lines.GetDrawCursorPos();
		outX += m_lines.m_pFont->SizeOf_/*Rel*/("-")*(_min(iFloor(m_lines.m_iCursorPos/4.0f),3));

		CUILine::DrawCursor(m_lines.m_pFont, rect.left+outX, rect.top+outY, m_lines.GetTextColor());
	}
	else
	{
		string64 tmp = "xxxxxxxxxxxxxxxx";
		tmp[m_lines.m_text.size()] = 0;

		m_lines.m_pFont->Out(pos.x, pos.y, "%s" ,AddHyphens(tmp) );
	}
}

LPCSTR CUICDkey::AddHyphens(LPCSTR c){
	static string32 buf;

	int sz = xr_strlen(c);
	int j = 0; 

	for (int i = 1; i<=3; i++)
		buf[i*5 - 1]='-';

	for (int i = 0; i<sz; i++)
	{
		j = i + iFloor(i/4.0f);
		buf[j] = c[i];		
	}
	buf[sz + iFloor(sz/4.0f)] = 0;

	return buf;
}

LPCSTR CUICDkey::DelHyphens(LPCSTR c){
	static string32 buf;

	int sz = xr_strlen(c);
	int j = 0; 

	for (int i = 0; i<sz - _min(iFloor(sz/4.0f),3); i++)
	{
		j = i + iFloor(i/4.0f);
		buf[i] = c[j];		
	}
	buf[sz - _min(iFloor(sz/4.0f),3)] = 0;

	return buf;
}

void CUICDkey::AddChar(char c){
	if (m_lines.m_text.length() < 16)
	{
		CUIEditBox::AddChar(c);
		SaveValue();
	}
}

void CUICDkey::SetCurrentValue(){
	char CDKeyStr[64];
	GetCDKey(CDKeyStr);
	m_lines.SetText(DelHyphens(CDKeyStr));

}
extern string64	gsCDKey;

void CUICDkey::SaveValue(){
	CUIOptionsItem::SaveValue();

//	char NewCDKey[32];
//	HKEY KeyCDKey = 0;

	sprintf(gsCDKey,"%s",AddHyphens(m_lines.GetText()));
/*
	long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
		REGISTRY_CDKEY_STR, 0, KEY_ALL_ACCESS, &KeyCDKey);

	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		DWORD KeyValueSize = (DWORD) xr_strlen(NewCDKey);
		DWORD KeyValueType = REG_SZ;
		res = RegSetValueEx(KeyCDKey, "value", NULL, KeyValueType, (LPBYTE)NewCDKey, KeyValueSize);
	}

	if (KeyCDKey) RegCloseKey(KeyCDKey);
*/
}

bool CUICDkey::IsChanged(){
	string64	tmpCDKeyStr;
	GetCDKey	(tmpCDKeyStr);
	return 0 != xr_strcmp(tmpCDKeyStr, m_lines.GetText());
}

void CUICDkey::CreateCDKeyEntry(){
/*
	HKEY KeyCDKey;

	long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
		REGISTRY_CDKEY_STR, 0, KEY_ALL_ACCESS, &KeyCDKey);

	switch (res)
	{
	case ERROR_FILE_NOT_FOUND:
		{
			HKEY hKey;
			res = RegOpenKeyEx(HKEY_CURRENT_USER, 
				"", 0, KEY_ALL_ACCESS, &hKey);
			if (res != ERROR_SUCCESS) return;
			DWORD xres;
			res = RegCreateKeyEx(hKey, REGISTRY_CDKEY_STR, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
				NULL, &KeyCDKey, &xres);

			if (hKey) RegCloseKey(hKey);
		}break;
	};
	//-----------------------------------------------------------------------------------
	if (!KeyCDKey) return;
	char KeyValue[1024] = "";
	DWORD KeyValueSize = 1023;
	DWORD KeyValueType = REG_SZ;
	res = RegQueryValueEx(KeyCDKey, "value", NULL, &KeyValueType, (LPBYTE)KeyValue, &KeyValueSize);

	switch (res)
	{
	case ERROR_FILE_NOT_FOUND:
		{
			res = RegSetValueEx(KeyCDKey, "value", NULL, KeyValueType, (LPBYTE)KeyValue, 0);
		}break;
	};	

	if (KeyCDKey) RegCloseKey(KeyCDKey);
*/
}

void CUICDkey::GetCDKey(char* CDKeyStr){
	strcpy(CDKeyStr, gsCDKey);
/*
	HKEY KeyCDKey = 0;
	long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
		REGISTRY_CDKEY_STR, 0, KEY_READ, &KeyCDKey);

	DWORD KeyValueSize = 128;
	DWORD KeyValueType = REG_SZ;
	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		res = RegQueryValueEx(KeyCDKey, "value", NULL, &KeyValueType, (LPBYTE)CDKeyStr, &KeyValueSize);
	};

	if (res == ERROR_PATH_NOT_FOUND ||
		res == ERROR_FILE_NOT_FOUND ||
		KeyValueSize == 0)
	{
		sprintf(CDKeyStr, "" );
	};
*/
}