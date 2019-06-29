#include "stdafx.h"
#include "LuaEditor.h"
#include "LuaView.h"
#include "ProjectFile.h"


CString CLuaEditor::GetSelText()
{
  int nStartSel = GetSelectionStart();
  int nEndSel = GetSelectionEnd();
  char* pszSelText = new char[nEndSel - nStartSel + 1];
  GetSelText(pszSelText);
	CString sSelText(pszSelText);
  delete [] pszSelText;

  return sSelText;

}

int CLuaEditor::GetSelText(char* text, BOOL bDirect)
{
  return Sci(SCI_GETSELTEXT, 0, (LPARAM) text/*, bDirect*/);
}


long CLuaEditor::GetSelectionStart(BOOL bDirect)
{
  return Sci(SCI_GETSELECTIONSTART, 0, 0/*, bDirect*/);
}

long CLuaEditor::GetSelectionEnd(BOOL bDirect)
{
  return Sci(SCI_GETSELECTIONEND, 0, 0/*, bDirect*/);
}

int CLuaEditor::GetLength(BOOL bDirect)
{
  return Sci(SCI_GETLENGTH, 0, 0/*, bDirect*/);
}

void CLuaEditor::HideSelection(BOOL normal, BOOL bDirect)
{
  Sci(SCI_HIDESELECTION, (WPARAM) normal, 0/*, bDirect*/);
}

void CLuaEditor::ReplaceSel(const char* text, BOOL bDirect)
{
  Sci(SCI_REPLACESEL, 0, (LPARAM) text/*, bDirect*/);
}

long CLuaEditor::FindText(int flags, TextToFind* ft, BOOL bDirect)
{
  return Sci(SCI_FINDTEXT, (WPARAM) flags, (LPARAM) ft/*, bDirect*/);
}

long CLuaEditor::FindTextNext(int flags, TextToFind* ft, BOOL bDirect)
{
  return Sci(SCI_SEARCHNEXT, (WPARAM) flags, (LPARAM) ft/*, bDirect*/);
}

void CLuaEditor::SetSel(long start, long end, BOOL bDirect)
{
  Sci(SCI_SETSEL, (WPARAM) start, (LPARAM) end/*, bDirect*/);
}

void CLuaEditor::CompleteWord(const char* word)
{
  	int word_end = GetSelectionEnd();
	int word_start = GetWordStart(word_end);//Sci(SCI_WORDSTARTPOSITION,word_end);
	SetSel(word_start,word_end);
	ReplaceSel(word);
}

long CLuaEditor::GetWordStart(long end)
{
	long word_start = end;
	char ch;
	while(1){
		ch = Sci(SCI_GETCHARAT,word_start-1);
		if( ch==' ' || ch=='\t' || ch=='\n' || ch=='\r')
			break;
		--word_start;
	}
	return word_start;
}

int CLuaEditor::PointXFromPosition(long pos, BOOL bDirect)
{
  return Sci(SCI_POINTXFROMPOSITION, 0, (LPARAM) pos/*, bDirect*/);
}

int CLuaEditor::PointYFromPosition(long pos, BOOL bDirect)
{
  return Sci(SCI_POINTYFROMPOSITION, 0, (LPARAM) pos/*, bDirect*/);
}



bool CLuaEditor::createWordList(CMenu& mnu)
{
	TextToFind ttf;
	int word_end = GetSelectionEnd();

	int word_start = GetWordStart(word_end);
//	int word_start = Sci(SCI_WORDSTARTPOSITION,word_end);

	char buff[2048];
	TextRange tr;
	tr.lpstrText = buff;
	tr.chrg.cpMin = word_start;
	tr.chrg.cpMax = word_end;
	Sci(SCI_GETTEXTRANGE,0,(int)&tr);
	char str_word[2048];
	strcpy(str_word,tr.lpstrText);

	ttf.lpstrText = str_word;
	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = GetLength();
	int r;
	CStringList sl;
	sl.AddHead(str_word);

	char trim[] = "\t\n\r";

	while(-1 != (r=FindText(SCFIND_WORDSTART,&ttf)) ){
		tr.chrg.cpMin = ttf.chrgText.cpMin;
		tr.chrg.cpMax = Sci(SCI_WORDENDPOSITION,ttf.chrgText.cpMax);
		Sci(SCI_GETTEXTRANGE,0,(int)&tr);

		StrTrim(tr.lpstrText,trim);

		if(sl.Find(tr.lpstrText)==NULL){
			sl.AddHead(tr.lpstrText);
			mnu.AppendMenu(MF_STRING,sl.GetSize(),tr.lpstrText);
		}
		ttf.chrg.cpMin = r+1;
	}
	return (sl.GetSize()>1);
}