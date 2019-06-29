/*
Module : ScintillaCtrl.CPP
Purpose: Defines the implementation for an MFC wrapper class for the Scintilla edit control (www.scintilla.org)
Created: PJN / 19-03-2004
History: PJN / 19-03-2004 1. Initial implementation synchronized to the v1.59 release of Scintilla
         PJN / 19-03-2004 1. Updated the sample app's Find Next and Find Previous marker functions. Now correctly
                          goes to the next and previous markers when a marker is on the current line.
                          2. Updated the sample app's passing of messages to Scintilla from the top level
                          CMainFrame window
         PJN / 06-06-2004 1. Updated class to work with Scintilla v1.61


Copyright (c) 2004 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 


*/



/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "ScintillaCtrl.h"
#include "Scintilla.h"


//////////////////////////////// Statics / Macros /////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



////////////////////////////////// Implementation /////////////////////////////

IMPLEMENT_DYNAMIC(CScintillaCtrl, CWnd)

CScintillaCtrl::CScintillaCtrl()
{
  //Initialize our member variables to sensible defaults
  m_DirectFunction = 0;
  m_DirectPointer = 0;
}

BOOL CScintillaCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwExStyle, LPVOID lpParam)
{
  //Call our base class implementation of CWnd::CreateEx
	BOOL bSuccess = CreateEx(dwExStyle, _T("scintilla"), NULL, dwStyle, rect, pParentWnd, nID, lpParam);

  //Setup the direct access data
  if (bSuccess)
    SetupDirectAccess();

  return bSuccess;
}

void CScintillaCtrl::SetupDirectAccess()
{
  //Setup the direct access data
  m_DirectFunction = GetDirectFunction();
	m_DirectPointer = GetDirectPointer();
}

CScintillaCtrl::~CScintillaCtrl()
{
	DestroyWindow();
}

inline LRESULT CScintillaCtrl::Call(UINT message, WPARAM wParam, LPARAM lParam, BOOL bDirect)
{
  ASSERT(::IsWindow(m_hWnd)); //Window must be valid

  if (bDirect)
  {
    ASSERT(m_DirectFunction); //Direct function must be valid
    return ((SciFnDirect) m_DirectFunction)(m_DirectPointer, message, wParam, lParam);
  }
  else
    return SendMessage(message, wParam, lParam);
}

int CScintillaCtrl::GetDirectFunction()
{
  return SendMessage(SCI_GETDIRECTFUNCTION, 0, 0);
}

int CScintillaCtrl::GetDirectPointer()
{
  return SendMessage(SCI_GETDIRECTPOINTER, 0, 0);
}

CString CScintillaCtrl::GetSelText()
{
  int nStartSel = GetSelectionStart();
  int nEndSel = GetSelectionEnd();
  char* pszSelText = new char[nEndSel - nStartSel + 1];
  GetSelText(pszSelText);
	CString sSelText(pszSelText);
  delete [] pszSelText;

  return sSelText;
}


//Everything else after this point was auto generated using the "ConvertScintillaiface.js" script

void CScintillaCtrl::AddText(int length, const char* text, BOOL bDirect)
{
  Call(SCI_ADDTEXT, (WPARAM) length, (LPARAM) text, bDirect);
}

void CScintillaCtrl::AddStyledText(int length, char* c, BOOL bDirect)
{
  Call(SCI_ADDSTYLEDTEXT, (WPARAM) length, (LPARAM) c, bDirect);
}

void CScintillaCtrl::InsertText(long pos, const char* text, BOOL bDirect)
{
  Call(SCI_INSERTTEXT, (WPARAM) pos, (LPARAM) text, bDirect);
}

void CScintillaCtrl::ClearAll(BOOL bDirect)
{
  Call(SCI_CLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::ClearDocumentStyle(BOOL bDirect)
{
  Call(SCI_CLEARDOCUMENTSTYLE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLength(BOOL bDirect)
{
  return Call(SCI_GETLENGTH, 0, 0, bDirect);
}

int CScintillaCtrl::GetCharAt(long pos, BOOL bDirect)
{
  return Call(SCI_GETCHARAT, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::GetCurrentPos(BOOL bDirect)
{
  return Call(SCI_GETCURRENTPOS, 0, 0, bDirect);
}

long CScintillaCtrl::GetAnchor(BOOL bDirect)
{
  return Call(SCI_GETANCHOR, 0, 0, bDirect);
}

int CScintillaCtrl::GetStyleAt(long pos, BOOL bDirect)
{
  return Call(SCI_GETSTYLEAT, (WPARAM) pos, 0, bDirect);
}

void CScintillaCtrl::Redo(BOOL bDirect)
{
  Call(SCI_REDO, 0, 0, bDirect);
}

void CScintillaCtrl::SetUndoCollection(BOOL collectUndo, BOOL bDirect)
{
  Call(SCI_SETUNDOCOLLECTION, (WPARAM) collectUndo, 0, bDirect);
}

void CScintillaCtrl::SelectAll(BOOL bDirect)
{
  Call(SCI_SELECTALL, 0, 0, bDirect);
}

void CScintillaCtrl::SetSavePoint(BOOL bDirect)
{
  Call(SCI_SETSAVEPOINT, 0, 0, bDirect);
}

int CScintillaCtrl::GetStyledText(TextRange* tr, BOOL bDirect)
{
  return Call(SCI_GETSTYLEDTEXT, 0, (LPARAM) tr, bDirect);
}

BOOL CScintillaCtrl::CanRedo(BOOL bDirect)
{
  return Call(SCI_CANREDO, 0, 0, bDirect);
}

int CScintillaCtrl::MarkerLineFromHandle(int handle, BOOL bDirect)
{
  return Call(SCI_MARKERLINEFROMHANDLE, (WPARAM) handle, 0, bDirect);
}

void CScintillaCtrl::MarkerDeleteHandle(int handle, BOOL bDirect)
{
  Call(SCI_MARKERDELETEHANDLE, (WPARAM) handle, 0, bDirect);
}

BOOL CScintillaCtrl::GetUndoCollection(BOOL bDirect)
{
  return Call(SCI_GETUNDOCOLLECTION, 0, 0, bDirect);
}

int CScintillaCtrl::GetViewWS(BOOL bDirect)
{
  return Call(SCI_GETVIEWWS, 0, 0, bDirect);
}

void CScintillaCtrl::SetViewWS(int viewWS, BOOL bDirect)
{
  Call(SCI_SETVIEWWS, (WPARAM) viewWS, 0, bDirect);
}

long CScintillaCtrl::PositionFromPoint(int x, int y, BOOL bDirect)
{
  return Call(SCI_POSITIONFROMPOINT, (WPARAM) x, (LPARAM) y, bDirect);
}

long CScintillaCtrl::PositionFromPointClose(int x, int y, BOOL bDirect)
{
  return Call(SCI_POSITIONFROMPOINTCLOSE, (WPARAM) x, (LPARAM) y, bDirect);
}

void CScintillaCtrl::GotoLine(int line, BOOL bDirect)
{
  Call(SCI_GOTOLINE, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::GotoPos(long pos, BOOL bDirect)
{
  Call(SCI_GOTOPOS, (WPARAM) pos, 0, bDirect);
}

void CScintillaCtrl::SetAnchor(long posAnchor, BOOL bDirect)
{
  Call(SCI_SETANCHOR, (WPARAM) posAnchor, 0, bDirect);
}

int CScintillaCtrl::GetCurLine(int length, char* text, BOOL bDirect)
{
  return Call(SCI_GETCURLINE, (WPARAM) length, (LPARAM) text, bDirect);
}

long CScintillaCtrl::GetEndStyled(BOOL bDirect)
{
  return Call(SCI_GETENDSTYLED, 0, 0, bDirect);
}

void CScintillaCtrl::ConvertEOLs(int eolMode, BOOL bDirect)
{
  Call(SCI_CONVERTEOLS, (WPARAM) eolMode, 0, bDirect);
}

int CScintillaCtrl::GetEOLMode(BOOL bDirect)
{
  return Call(SCI_GETEOLMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetEOLMode(int eolMode, BOOL bDirect)
{
  Call(SCI_SETEOLMODE, (WPARAM) eolMode, 0, bDirect);
}

void CScintillaCtrl::StartStyling(long pos, int mask, BOOL bDirect)
{
  Call(SCI_STARTSTYLING, (WPARAM) pos, (LPARAM) mask, bDirect);
}

void CScintillaCtrl::SetStyling(int length, int style, BOOL bDirect)
{
  Call(SCI_SETSTYLING, (WPARAM) length, (LPARAM) style, bDirect);
}

BOOL CScintillaCtrl::GetBufferedDraw(BOOL bDirect)
{
  return Call(SCI_GETBUFFEREDDRAW, 0, 0, bDirect);
}

void CScintillaCtrl::SetBufferedDraw(BOOL buffered, BOOL bDirect)
{
  Call(SCI_SETBUFFEREDDRAW, (WPARAM) buffered, 0, bDirect);
}

void CScintillaCtrl::SetTabWidth(int tabWidth, BOOL bDirect)
{
  Call(SCI_SETTABWIDTH, (WPARAM) tabWidth, 0, bDirect);
}

int CScintillaCtrl::GetTabWidth(BOOL bDirect)
{
  return Call(SCI_GETTABWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetCodePage(int codePage, BOOL bDirect)
{
  Call(SCI_SETCODEPAGE, (WPARAM) codePage, 0, bDirect);
}

void CScintillaCtrl::SetUsePalette(BOOL usePalette, BOOL bDirect)
{
  Call(SCI_SETUSEPALETTE, (WPARAM) usePalette, 0, bDirect);
}

void CScintillaCtrl::MarkerDefine(int markerNumber, int markerSymbol, BOOL bDirect)
{
  Call(SCI_MARKERDEFINE, (WPARAM) markerNumber, (LPARAM) markerSymbol, bDirect);
}

void CScintillaCtrl::MarkerSetFore(int markerNumber, COLORREF fore, BOOL bDirect)
{
  Call(SCI_MARKERSETFORE, (WPARAM) markerNumber, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::MarkerSetBack(int markerNumber, COLORREF back, BOOL bDirect)
{
  Call(SCI_MARKERSETBACK, (WPARAM) markerNumber, (LPARAM) back, bDirect);
}

int CScintillaCtrl::MarkerAdd(int line, int markerNumber, BOOL bDirect)
{
  return Call(SCI_MARKERADD, (WPARAM) line, (LPARAM) markerNumber, bDirect);
}

void CScintillaCtrl::MarkerDelete(int line, int markerNumber, BOOL bDirect)
{
  Call(SCI_MARKERDELETE, (WPARAM) line, (LPARAM) markerNumber, bDirect);
}

void CScintillaCtrl::MarkerDeleteAll(int markerNumber, BOOL bDirect)
{
  Call(SCI_MARKERDELETEALL, (WPARAM) markerNumber, 0, bDirect);
}

int CScintillaCtrl::MarkerGet(int line, BOOL bDirect)
{
  return Call(SCI_MARKERGET, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::MarkerNext(int lineStart, int markerMask, BOOL bDirect)
{
  return Call(SCI_MARKERNEXT, (WPARAM) lineStart, (LPARAM) markerMask, bDirect);
}

int CScintillaCtrl::MarkerPrevious(int lineStart, int markerMask, BOOL bDirect)
{
  return Call(SCI_MARKERPREVIOUS, (WPARAM) lineStart, (LPARAM) markerMask, bDirect);
}

void CScintillaCtrl::MarkerDefinePixmap(int markerNumber, const char* pixmap, BOOL bDirect)
{
  Call(SCI_MARKERDEFINEPIXMAP, (WPARAM) markerNumber, (LPARAM) pixmap, bDirect);
}

void CScintillaCtrl::SetMarginTypeN(int margin, int marginType, BOOL bDirect)
{
  Call(SCI_SETMARGINTYPEN, (WPARAM) margin, (LPARAM) marginType, bDirect);
}

int CScintillaCtrl::GetMarginTypeN(int margin, BOOL bDirect)
{
  return Call(SCI_GETMARGINTYPEN, (WPARAM) margin, 0, bDirect);
}

void CScintillaCtrl::SetMarginWidthN(int margin, int pixelWidth, BOOL bDirect)
{
  Call(SCI_SETMARGINWIDTHN, (WPARAM) margin, (LPARAM) pixelWidth, bDirect);
}

int CScintillaCtrl::GetMarginWidthN(int margin, BOOL bDirect)
{
  return Call(SCI_GETMARGINWIDTHN, (WPARAM) margin, 0, bDirect);
}

void CScintillaCtrl::SetMarginMaskN(int margin, int mask, BOOL bDirect)
{
  Call(SCI_SETMARGINMASKN, (WPARAM) margin, (LPARAM) mask, bDirect);
}

int CScintillaCtrl::GetMarginMaskN(int margin, BOOL bDirect)
{
  return Call(SCI_GETMARGINMASKN, (WPARAM) margin, 0, bDirect);
}

void CScintillaCtrl::SetMarginSensitiveN(int margin, BOOL sensitive, BOOL bDirect)
{
  Call(SCI_SETMARGINSENSITIVEN, (WPARAM) margin, (LPARAM) sensitive, bDirect);
}

BOOL CScintillaCtrl::GetMarginSensitiveN(int margin, BOOL bDirect)
{
  return Call(SCI_GETMARGINSENSITIVEN, (WPARAM) margin, 0, bDirect);
}

void CScintillaCtrl::StyleClearAll(BOOL bDirect)
{
  Call(SCI_STYLECLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::StyleSetFore(int style, COLORREF fore, BOOL bDirect)
{
  Call(SCI_STYLESETFORE, (WPARAM) style, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::StyleSetBack(int style, COLORREF back, BOOL bDirect)
{
  Call(SCI_STYLESETBACK, (WPARAM) style, (LPARAM) back, bDirect);
}

void CScintillaCtrl::StyleSetBold(int style, BOOL bold, BOOL bDirect)
{
  Call(SCI_STYLESETBOLD, (WPARAM) style, (LPARAM) bold, bDirect);
}

void CScintillaCtrl::StyleSetItalic(int style, BOOL italic, BOOL bDirect)
{
  Call(SCI_STYLESETITALIC, (WPARAM) style, (LPARAM) italic, bDirect);
}

void CScintillaCtrl::StyleSetSize(int style, int sizePoints, BOOL bDirect)
{
  Call(SCI_STYLESETSIZE, (WPARAM) style, (LPARAM) sizePoints, bDirect);
}

void CScintillaCtrl::StyleSetFont(int style, const char* fontName, BOOL bDirect)
{
  Call(SCI_STYLESETFONT, (WPARAM) style, (LPARAM) fontName, bDirect);
}

void CScintillaCtrl::StyleSetEOLFilled(int style, BOOL filled, BOOL bDirect)
{
  Call(SCI_STYLESETEOLFILLED, (WPARAM) style, (LPARAM) filled, bDirect);
}

void CScintillaCtrl::StyleResetDefault(BOOL bDirect)
{
  Call(SCI_STYLERESETDEFAULT, 0, 0, bDirect);
}

void CScintillaCtrl::StyleSetUnderline(int style, BOOL underline, BOOL bDirect)
{
  Call(SCI_STYLESETUNDERLINE, (WPARAM) style, (LPARAM) underline, bDirect);
}

void CScintillaCtrl::StyleSetCase(int style, int caseForce, BOOL bDirect)
{
  Call(SCI_STYLESETCASE, (WPARAM) style, (LPARAM) caseForce, bDirect);
}

void CScintillaCtrl::StyleSetCharacterSet(int style, int characterSet, BOOL bDirect)
{
  Call(SCI_STYLESETCHARACTERSET, (WPARAM) style, (LPARAM) characterSet, bDirect);
}

void CScintillaCtrl::StyleSetHotSpot(int style, BOOL hotspot, BOOL bDirect)
{
  Call(SCI_STYLESETHOTSPOT, (WPARAM) style, (LPARAM) hotspot, bDirect);
}

void CScintillaCtrl::SetSelFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
  Call(SCI_SETSELFORE, (WPARAM) useSetting, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::SetSelBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
  Call(SCI_SETSELBACK, (WPARAM) useSetting, (LPARAM) back, bDirect);
}

void CScintillaCtrl::SetCaretFore(COLORREF fore, BOOL bDirect)
{
  Call(SCI_SETCARETFORE, (WPARAM) fore, 0, bDirect);
}

void CScintillaCtrl::AssignCmdKey(DWORD km, int msg, BOOL bDirect)
{
  Call(SCI_ASSIGNCMDKEY, (WPARAM) km, (LPARAM) msg, bDirect);
}

void CScintillaCtrl::ClearCmdKey(DWORD km, BOOL bDirect)
{
  Call(SCI_CLEARCMDKEY, (WPARAM) km, 0, bDirect);
}

void CScintillaCtrl::ClearAllCmdKeys(BOOL bDirect)
{
  Call(SCI_CLEARALLCMDKEYS, 0, 0, bDirect);
}

void CScintillaCtrl::SetStylingEx(int length, const char* styles, BOOL bDirect)
{
  Call(SCI_SETSTYLINGEX, (WPARAM) length, (LPARAM) styles, bDirect);
}

void CScintillaCtrl::StyleSetVisible(int style, BOOL visible, BOOL bDirect)
{
  Call(SCI_STYLESETVISIBLE, (WPARAM) style, (LPARAM) visible, bDirect);
}

int CScintillaCtrl::GetCaretPeriod(BOOL bDirect)
{
  return Call(SCI_GETCARETPERIOD, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretPeriod(int periodMilliseconds, BOOL bDirect)
{
  Call(SCI_SETCARETPERIOD, (WPARAM) periodMilliseconds, 0, bDirect);
}

void CScintillaCtrl::SetWordChars(const char* characters, BOOL bDirect)
{
  Call(SCI_SETWORDCHARS, 0, (LPARAM) characters, bDirect);
}

void CScintillaCtrl::BeginUndoAction(BOOL bDirect)
{
  Call(SCI_BEGINUNDOACTION, 0, 0, bDirect);
}

void CScintillaCtrl::EndUndoAction(BOOL bDirect)
{
  Call(SCI_ENDUNDOACTION, 0, 0, bDirect);
}

void CScintillaCtrl::IndicSetStyle(int indic, int style, BOOL bDirect)
{
  Call(SCI_INDICSETSTYLE, (WPARAM) indic, (LPARAM) style, bDirect);
}

int CScintillaCtrl::IndicGetStyle(int indic, BOOL bDirect)
{
  return Call(SCI_INDICGETSTYLE, (WPARAM) indic, 0, bDirect);
}

void CScintillaCtrl::IndicSetFore(int indic, COLORREF fore, BOOL bDirect)
{
  Call(SCI_INDICSETFORE, (WPARAM) indic, (LPARAM) fore, bDirect);
}

COLORREF CScintillaCtrl::IndicGetFore(int indic, BOOL bDirect)
{
  return Call(SCI_INDICGETFORE, (WPARAM) indic, 0, bDirect);
}

void CScintillaCtrl::SetWhitespaceFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
  Call(SCI_SETWHITESPACEFORE, (WPARAM) useSetting, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::SetWhitespaceBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
  Call(SCI_SETWHITESPACEBACK, (WPARAM) useSetting, (LPARAM) back, bDirect);
}

void CScintillaCtrl::SetStyleBits(int bits, BOOL bDirect)
{
  Call(SCI_SETSTYLEBITS, (WPARAM) bits, 0, bDirect);
}

int CScintillaCtrl::GetStyleBits(BOOL bDirect)
{
  return Call(SCI_GETSTYLEBITS, 0, 0, bDirect);
}

void CScintillaCtrl::SetLineState(int line, int state, BOOL bDirect)
{
  Call(SCI_SETLINESTATE, (WPARAM) line, (LPARAM) state, bDirect);
}

int CScintillaCtrl::GetLineState(int line, BOOL bDirect)
{
  return Call(SCI_GETLINESTATE, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::GetMaxLineState(BOOL bDirect)
{
  return Call(SCI_GETMAXLINESTATE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetCaretLineVisible(BOOL bDirect)
{
  return Call(SCI_GETCARETLINEVISIBLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineVisible(BOOL show, BOOL bDirect)
{
  Call(SCI_SETCARETLINEVISIBLE, (WPARAM) show, 0, bDirect);
}

COLORREF CScintillaCtrl::GetCaretLineBack(BOOL bDirect)
{
  return Call(SCI_GETCARETLINEBACK, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineBack(COLORREF back, BOOL bDirect)
{
  Call(SCI_SETCARETLINEBACK, (WPARAM) back, 0, bDirect);
}

void CScintillaCtrl::StyleSetChangeable(int style, BOOL changeable, BOOL bDirect)
{
  Call(SCI_STYLESETCHANGEABLE, (WPARAM) style, (LPARAM) changeable, bDirect);
}

void CScintillaCtrl::AutoCShow(int lenEntered, const char* itemList, BOOL bDirect)
{
  Call(SCI_AUTOCSHOW, (WPARAM) lenEntered, (LPARAM) itemList, bDirect);
}

void CScintillaCtrl::AutoCCancel(BOOL bDirect)
{
  Call(SCI_AUTOCCANCEL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCActive(BOOL bDirect)
{
  return Call(SCI_AUTOCACTIVE, 0, 0, bDirect);
}

long CScintillaCtrl::AutoCPosStart(BOOL bDirect)
{
  return Call(SCI_AUTOCPOSSTART, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCComplete(BOOL bDirect)
{
  Call(SCI_AUTOCCOMPLETE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCStops(const char* characterSet, BOOL bDirect)
{
  Call(SCI_AUTOCSTOPS, 0, (LPARAM) characterSet, bDirect);
}

void CScintillaCtrl::AutoCSetSeparator(int separatorCharacter, BOOL bDirect)
{
  Call(SCI_AUTOCSETSEPARATOR, (WPARAM) separatorCharacter, 0, bDirect);
}

int CScintillaCtrl::AutoCGetSeparator(BOOL bDirect)
{
  return Call(SCI_AUTOCGETSEPARATOR, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSelect(const char* text, BOOL bDirect)
{
  Call(SCI_AUTOCSELECT, 0, (LPARAM) text, bDirect);
}

void CScintillaCtrl::AutoCSetCancelAtStart(BOOL cancel, BOOL bDirect)
{
  Call(SCI_AUTOCSETCANCELATSTART, (WPARAM) cancel, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetCancelAtStart(BOOL bDirect)
{
  return Call(SCI_AUTOCGETCANCELATSTART, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetFillUps(const char* characterSet, BOOL bDirect)
{
  Call(SCI_AUTOCSETFILLUPS, 0, (LPARAM) characterSet, bDirect);
}

void CScintillaCtrl::AutoCSetChooseSingle(BOOL chooseSingle, BOOL bDirect)
{
  Call(SCI_AUTOCSETCHOOSESINGLE, (WPARAM) chooseSingle, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetChooseSingle(BOOL bDirect)
{
  return Call(SCI_AUTOCGETCHOOSESINGLE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetIgnoreCase(BOOL ignoreCase, BOOL bDirect)
{
  Call(SCI_AUTOCSETIGNORECASE, (WPARAM) ignoreCase, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetIgnoreCase(BOOL bDirect)
{
  return Call(SCI_AUTOCGETIGNORECASE, 0, 0, bDirect);
}

void CScintillaCtrl::UserListShow(int listType, const char* itemList, BOOL bDirect)
{
  Call(SCI_USERLISTSHOW, (WPARAM) listType, (LPARAM) itemList, bDirect);
}

void CScintillaCtrl::AutoCSetAutoHide(BOOL autoHide, BOOL bDirect)
{
  Call(SCI_AUTOCSETAUTOHIDE, (WPARAM) autoHide, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetAutoHide(BOOL bDirect)
{
  return Call(SCI_AUTOCGETAUTOHIDE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetDropRestOfWord(BOOL dropRestOfWord, BOOL bDirect)
{
  Call(SCI_AUTOCSETDROPRESTOFWORD, (WPARAM) dropRestOfWord, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetDropRestOfWord(BOOL bDirect)
{
  return Call(SCI_AUTOCGETDROPRESTOFWORD, 0, 0, bDirect);
}

void CScintillaCtrl::RegisterImage(int type, const char* xpmData, BOOL bDirect)
{
  Call(SCI_REGISTERIMAGE, (WPARAM) type, (LPARAM) xpmData, bDirect);
}

void CScintillaCtrl::ClearRegisteredImages(BOOL bDirect)
{
  Call(SCI_CLEARREGISTEREDIMAGES, 0, 0, bDirect);
}

int CScintillaCtrl::AutoCGetTypeSeparator(BOOL bDirect)
{
  return Call(SCI_AUTOCGETTYPESEPARATOR, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetTypeSeparator(int separatorCharacter, BOOL bDirect)
{
  Call(SCI_AUTOCSETTYPESEPARATOR, (WPARAM) separatorCharacter, 0, bDirect);
}

void CScintillaCtrl::SetIndent(int indentSize, BOOL bDirect)
{
  Call(SCI_SETINDENT, (WPARAM) indentSize, 0, bDirect);
}

int CScintillaCtrl::GetIndent(BOOL bDirect)
{
  return Call(SCI_GETINDENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetUseTabs(BOOL useTabs, BOOL bDirect)
{
  Call(SCI_SETUSETABS, (WPARAM) useTabs, 0, bDirect);
}

BOOL CScintillaCtrl::GetUseTabs(BOOL bDirect)
{
  return Call(SCI_GETUSETABS, 0, 0, bDirect);
}

void CScintillaCtrl::SetLineIndentation(int line, int indentSize, BOOL bDirect)
{
  Call(SCI_SETLINEINDENTATION, (WPARAM) line, (LPARAM) indentSize, bDirect);
}

int CScintillaCtrl::GetLineIndentation(int line, BOOL bDirect)
{
  return Call(SCI_GETLINEINDENTATION, (WPARAM) line, 0, bDirect);
}

long CScintillaCtrl::GetLineIndentPosition(int line, BOOL bDirect)
{
  return Call(SCI_GETLINEINDENTPOSITION, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::GetColumn(long pos, BOOL bDirect)
{
  return Call(SCI_GETCOLUMN, (WPARAM) pos, 0, bDirect);
}

void CScintillaCtrl::SetHScrollBar(BOOL show, BOOL bDirect)
{
  Call(SCI_SETHSCROLLBAR, (WPARAM) show, 0, bDirect);
}

BOOL CScintillaCtrl::GetHScrollBar(BOOL bDirect)
{
  return Call(SCI_GETHSCROLLBAR, 0, 0, bDirect);
}

void CScintillaCtrl::SetIndentationGuides(BOOL show, BOOL bDirect)
{
  Call(SCI_SETINDENTATIONGUIDES, (WPARAM) show, 0, bDirect);
}

BOOL CScintillaCtrl::GetIndentationGuides(BOOL bDirect)
{
  return Call(SCI_GETINDENTATIONGUIDES, 0, 0, bDirect);
}

void CScintillaCtrl::SetHighlightGuide(int column, BOOL bDirect)
{
  Call(SCI_SETHIGHLIGHTGUIDE, (WPARAM) column, 0, bDirect);
}

int CScintillaCtrl::GetHighlightGuide(BOOL bDirect)
{
  return Call(SCI_GETHIGHLIGHTGUIDE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLineEndPosition(int line, BOOL bDirect)
{
  return Call(SCI_GETLINEENDPOSITION, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::GetCodePage(BOOL bDirect)
{
  return Call(SCI_GETCODEPAGE, 0, 0, bDirect);
}

COLORREF CScintillaCtrl::GetCaretFore(BOOL bDirect)
{
  return Call(SCI_GETCARETFORE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetUsePalette(BOOL bDirect)
{
  return Call(SCI_GETUSEPALETTE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetReadOnly(BOOL bDirect)
{
  return Call(SCI_GETREADONLY, 0, 0, bDirect);
}

void CScintillaCtrl::SetCurrentPos(long pos, BOOL bDirect)
{
  Call(SCI_SETCURRENTPOS, (WPARAM) pos, 0, bDirect);
}

void CScintillaCtrl::SetSelectionStart(long pos, BOOL bDirect)
{
  Call(SCI_SETSELECTIONSTART, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::GetSelectionStart(BOOL bDirect)
{
  return Call(SCI_GETSELECTIONSTART, 0, 0, bDirect);
}

void CScintillaCtrl::SetSelectionEnd(long pos, BOOL bDirect)
{
  Call(SCI_SETSELECTIONEND, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::GetSelectionEnd(BOOL bDirect)
{
  return Call(SCI_GETSELECTIONEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetPrintMagnification(int magnification, BOOL bDirect)
{
  Call(SCI_SETPRINTMAGNIFICATION, (WPARAM) magnification, 0, bDirect);
}

int CScintillaCtrl::GetPrintMagnification(BOOL bDirect)
{
  return Call(SCI_GETPRINTMAGNIFICATION, 0, 0, bDirect);
}

void CScintillaCtrl::SetPrintColourMode(int mode, BOOL bDirect)
{
  Call(SCI_SETPRINTCOLOURMODE, (WPARAM) mode, 0, bDirect);
}

int CScintillaCtrl::GetPrintColourMode(BOOL bDirect)
{
  return Call(SCI_GETPRINTCOLOURMODE, 0, 0, bDirect);
}

long CScintillaCtrl::FindText(int flags, TextToFind* ft, BOOL bDirect)
{
  return Call(SCI_FINDTEXT, (WPARAM) flags, (LPARAM) ft, bDirect);
}

long CScintillaCtrl::FormatRange(BOOL draw, RangeToFormat* fr, BOOL bDirect)
{
  return Call(SCI_FORMATRANGE, (WPARAM) draw, (LPARAM) fr, bDirect);
}

int CScintillaCtrl::GetFirstVisibleLine(BOOL bDirect)
{
  return Call(SCI_GETFIRSTVISIBLELINE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLine(int line, char* text, BOOL bDirect)
{
  return Call(SCI_GETLINE, (WPARAM) line, (LPARAM) text, bDirect);
}

int CScintillaCtrl::GetLineCount(BOOL bDirect)
{
  return Call(SCI_GETLINECOUNT, 0, 0, bDirect);
}

void CScintillaCtrl::SetMarginLeft(int pixelWidth, BOOL bDirect)
{
  Call(SCI_SETMARGINLEFT, 0, (LPARAM) pixelWidth, bDirect);
}

int CScintillaCtrl::GetMarginLeft(BOOL bDirect)
{
  return Call(SCI_GETMARGINLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::SetMarginRight(int pixelWidth, BOOL bDirect)
{
  Call(SCI_SETMARGINRIGHT, 0, (LPARAM) pixelWidth, bDirect);
}

int CScintillaCtrl::GetMarginRight(BOOL bDirect)
{
  return Call(SCI_GETMARGINRIGHT, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetModify(BOOL bDirect)
{
  return Call(SCI_GETMODIFY, 0, 0, bDirect);
}

void CScintillaCtrl::SetSel(long start, long end, BOOL bDirect)
{
  Call(SCI_SETSEL, (WPARAM) start, (LPARAM) end, bDirect);
}

int CScintillaCtrl::GetSelText(char* text, BOOL bDirect)
{
  return Call(SCI_GETSELTEXT, 0, (LPARAM) text, bDirect);
}

int CScintillaCtrl::GetTextRange(TextRange* tr, BOOL bDirect)
{
  return Call(SCI_GETTEXTRANGE, 0, (LPARAM) tr, bDirect);
}

void CScintillaCtrl::HideSelection(BOOL normal, BOOL bDirect)
{
  Call(SCI_HIDESELECTION, (WPARAM) normal, 0, bDirect);
}

int CScintillaCtrl::PointXFromPosition(long pos, BOOL bDirect)
{
  return Call(SCI_POINTXFROMPOSITION, 0, (LPARAM) pos, bDirect);
}

int CScintillaCtrl::PointYFromPosition(long pos, BOOL bDirect)
{
  return Call(SCI_POINTYFROMPOSITION, 0, (LPARAM) pos, bDirect);
}

int CScintillaCtrl::LineFromPosition(long pos, BOOL bDirect)
{
  return Call(SCI_LINEFROMPOSITION, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::PositionFromLine(int line, BOOL bDirect)
{
  return Call(SCI_POSITIONFROMLINE, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::LineScroll(int columns, int lines, BOOL bDirect)
{
  Call(SCI_LINESCROLL, (WPARAM) columns, (LPARAM) lines, bDirect);
}

void CScintillaCtrl::ScrollCaret(BOOL bDirect)
{
  Call(SCI_SCROLLCARET, 0, 0, bDirect);
}

void CScintillaCtrl::ReplaceSel(const char* text, BOOL bDirect)
{
  Call(SCI_REPLACESEL, 0, (LPARAM) text, bDirect);
}

void CScintillaCtrl::SetReadOnly(BOOL readOnly, BOOL bDirect)
{
  Call(SCI_SETREADONLY, (WPARAM) readOnly, 0, bDirect);
}

void CScintillaCtrl::Null(BOOL bDirect)
{
  Call(SCI_NULL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CanPaste(BOOL bDirect)
{
  return Call(SCI_CANPASTE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CanUndo(BOOL bDirect)
{
  return Call(SCI_CANUNDO, 0, 0, bDirect);
}

void CScintillaCtrl::EmptyUndoBuffer(BOOL bDirect)
{
  Call(SCI_EMPTYUNDOBUFFER, 0, 0, bDirect);
}

void CScintillaCtrl::Undo(BOOL bDirect)
{
  Call(SCI_UNDO, 0, 0, bDirect);
}

void CScintillaCtrl::Cut(BOOL bDirect)
{
  Call(SCI_CUT, 0, 0, bDirect);
}

void CScintillaCtrl::Copy(BOOL bDirect)
{
  Call(SCI_COPY, 0, 0, bDirect);
}

void CScintillaCtrl::Paste(BOOL bDirect)
{
  Call(SCI_PASTE, 0, 0, bDirect);
}

void CScintillaCtrl::Clear(BOOL bDirect)
{
  Call(SCI_CLEAR, 0, 0, bDirect);
}

void CScintillaCtrl::SetText(const char* text, BOOL bDirect)
{
  Call(SCI_SETTEXT, 0, (LPARAM) text, bDirect);
}

int CScintillaCtrl::GetText(int length, char* text, BOOL bDirect)
{
  return Call(SCI_GETTEXT, (WPARAM) length, (LPARAM) text, bDirect);
}

int CScintillaCtrl::GetTextLength(BOOL bDirect)
{
  return Call(SCI_GETTEXTLENGTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetOvertype(BOOL overtype, BOOL bDirect)
{
  Call(SCI_SETOVERTYPE, (WPARAM) overtype, 0, bDirect);
}

BOOL CScintillaCtrl::GetOvertype(BOOL bDirect)
{
  return Call(SCI_GETOVERTYPE, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretWidth(int pixelWidth, BOOL bDirect)
{
  Call(SCI_SETCARETWIDTH, (WPARAM) pixelWidth, 0, bDirect);
}

int CScintillaCtrl::GetCaretWidth(BOOL bDirect)
{
  return Call(SCI_GETCARETWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetTargetStart(long pos, BOOL bDirect)
{
  Call(SCI_SETTARGETSTART, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::GetTargetStart(BOOL bDirect)
{
  return Call(SCI_GETTARGETSTART, 0, 0, bDirect);
}

void CScintillaCtrl::SetTargetEnd(long pos, BOOL bDirect)
{
  Call(SCI_SETTARGETEND, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::GetTargetEnd(BOOL bDirect)
{
  return Call(SCI_GETTARGETEND, 0, 0, bDirect);
}

int CScintillaCtrl::ReplaceTarget(int length, const char* text, BOOL bDirect)
{
  return Call(SCI_REPLACETARGET, (WPARAM) length, (LPARAM) text, bDirect);
}

int CScintillaCtrl::ReplaceTargetRE(int length, const char* text, BOOL bDirect)
{
  return Call(SCI_REPLACETARGETRE, (WPARAM) length, (LPARAM) text, bDirect);
}

int CScintillaCtrl::SearchInTarget(int length, const char* text, BOOL bDirect)
{
  return Call(SCI_SEARCHINTARGET, (WPARAM) length, (LPARAM) text, bDirect);
}

void CScintillaCtrl::SetSearchFlags(int flags, BOOL bDirect)
{
  Call(SCI_SETSEARCHFLAGS, (WPARAM) flags, 0, bDirect);
}

int CScintillaCtrl::GetSearchFlags(BOOL bDirect)
{
  return Call(SCI_GETSEARCHFLAGS, 0, 0, bDirect);
}

void CScintillaCtrl::CallTipShow(long pos, const char* definition, BOOL bDirect)
{
  Call(SCI_CALLTIPSHOW, (WPARAM) pos, (LPARAM) definition, bDirect);
}

void CScintillaCtrl::CallTipCancel(BOOL bDirect)
{
  Call(SCI_CALLTIPCANCEL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CallTipActive(BOOL bDirect)
{
  return Call(SCI_CALLTIPACTIVE, 0, 0, bDirect);
}

long CScintillaCtrl::CallTipPosStart(BOOL bDirect)
{
  return Call(SCI_CALLTIPPOSSTART, 0, 0, bDirect);
}

void CScintillaCtrl::CallTipSetHlt(int start, int end, BOOL bDirect)
{
  Call(SCI_CALLTIPSETHLT, (WPARAM) start, (LPARAM) end, bDirect);
}

void CScintillaCtrl::CallTipSetBack(COLORREF back, BOOL bDirect)
{
  Call(SCI_CALLTIPSETBACK, (WPARAM) back, 0, bDirect);
}

void CScintillaCtrl::CallTipSetFore(COLORREF fore, BOOL bDirect)
{
  Call(SCI_CALLTIPSETFORE, (WPARAM) fore, 0, bDirect);
}

void CScintillaCtrl::CallTipSetForeHlt(COLORREF fore, BOOL bDirect)
{
  Call(SCI_CALLTIPSETFOREHLT, (WPARAM) fore, 0, bDirect);
}

int CScintillaCtrl::VisibleFromDocLine(int line, BOOL bDirect)
{
  return Call(SCI_VISIBLEFROMDOCLINE, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::DocLineFromVisible(int lineDisplay, BOOL bDirect)
{
  return Call(SCI_DOCLINEFROMVISIBLE, (WPARAM) lineDisplay, 0, bDirect);
}

void CScintillaCtrl::SetFoldLevel(int line, int level, BOOL bDirect)
{
  Call(SCI_SETFOLDLEVEL, (WPARAM) line, (LPARAM) level, bDirect);
}

int CScintillaCtrl::GetFoldLevel(int line, BOOL bDirect)
{
  return Call(SCI_GETFOLDLEVEL, (WPARAM) line, 0, bDirect);
}

int CScintillaCtrl::GetLastChild(int line, int level, BOOL bDirect)
{
  return Call(SCI_GETLASTCHILD, (WPARAM) line, (LPARAM) level, bDirect);
}

int CScintillaCtrl::GetFoldParent(int line, BOOL bDirect)
{
  return Call(SCI_GETFOLDPARENT, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::ShowLines(int lineStart, int lineEnd, BOOL bDirect)
{
  Call(SCI_SHOWLINES, (WPARAM) lineStart, (LPARAM) lineEnd, bDirect);
}

void CScintillaCtrl::HideLines(int lineStart, int lineEnd, BOOL bDirect)
{
  Call(SCI_HIDELINES, (WPARAM) lineStart, (LPARAM) lineEnd, bDirect);
}

BOOL CScintillaCtrl::GetLineVisible(int line, BOOL bDirect)
{
  return Call(SCI_GETLINEVISIBLE, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::SetFoldExpanded(int line, BOOL expanded, BOOL bDirect)
{
  Call(SCI_SETFOLDEXPANDED, (WPARAM) line, (LPARAM) expanded, bDirect);
}

BOOL CScintillaCtrl::GetFoldExpanded(int line, BOOL bDirect)
{
  return Call(SCI_GETFOLDEXPANDED, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::ToggleFold(int line, BOOL bDirect)
{
  Call(SCI_TOGGLEFOLD, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::EnsureVisible(int line, BOOL bDirect)
{
  Call(SCI_ENSUREVISIBLE, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::SetFoldFlags(int flags, BOOL bDirect)
{
  Call(SCI_SETFOLDFLAGS, (WPARAM) flags, 0, bDirect);
}

void CScintillaCtrl::EnsureVisibleEnforcePolicy(int line, BOOL bDirect)
{
  Call(SCI_ENSUREVISIBLEENFORCEPOLICY, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::SetTabIndents(BOOL tabIndents, BOOL bDirect)
{
  Call(SCI_SETTABINDENTS, (WPARAM) tabIndents, 0, bDirect);
}

BOOL CScintillaCtrl::GetTabIndents(BOOL bDirect)
{
  return Call(SCI_GETTABINDENTS, 0, 0, bDirect);
}

void CScintillaCtrl::SetBackSpaceUnIndents(BOOL bsUnIndents, BOOL bDirect)
{
  Call(SCI_SETBACKSPACEUNINDENTS, (WPARAM) bsUnIndents, 0, bDirect);
}

BOOL CScintillaCtrl::GetBackSpaceUnIndents(BOOL bDirect)
{
  return Call(SCI_GETBACKSPACEUNINDENTS, 0, 0, bDirect);
}

void CScintillaCtrl::SetMouseDwellTime(int periodMilliseconds, BOOL bDirect)
{
  Call(SCI_SETMOUSEDWELLTIME, (WPARAM) periodMilliseconds, 0, bDirect);
}

int CScintillaCtrl::GetMouseDwellTime(BOOL bDirect)
{
  return Call(SCI_GETMOUSEDWELLTIME, 0, 0, bDirect);
}

int CScintillaCtrl::WordStartPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect)
{
  return Call(SCI_WORDSTARTPOSITION, (WPARAM) pos, (LPARAM) onlyWordCharacters, bDirect);
}

int CScintillaCtrl::WordEndPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect)
{
  return Call(SCI_WORDENDPOSITION, (WPARAM) pos, (LPARAM) onlyWordCharacters, bDirect);
}

void CScintillaCtrl::SetWrapMode(int mode, BOOL bDirect)
{
  Call(SCI_SETWRAPMODE, (WPARAM) mode, 0, bDirect);
}

int CScintillaCtrl::GetWrapMode(BOOL bDirect)
{
  return Call(SCI_GETWRAPMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapVisualFlags(int wrapVisualFlags, BOOL bDirect)
{
  Call(SCI_SETWRAPVISUALFLAGS, (WPARAM) wrapVisualFlags, 0, bDirect);
}

int CScintillaCtrl::GetWrapVisualFlags(BOOL bDirect)
{
  return Call(SCI_GETWRAPVISUALFLAGS, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation, BOOL bDirect)
{
  Call(SCI_SETWRAPVISUALFLAGSLOCATION, (WPARAM) wrapVisualFlagsLocation, 0, bDirect);
}

int CScintillaCtrl::GetWrapVisualFlagsLocation(BOOL bDirect)
{
  return Call(SCI_GETWRAPVISUALFLAGSLOCATION, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapStartIndent(int indent, BOOL bDirect)
{
  Call(SCI_SETWRAPSTARTINDENT, (WPARAM) indent, 0, bDirect);
}

int CScintillaCtrl::GetWrapStartIndent(BOOL bDirect)
{
  return Call(SCI_GETWRAPSTARTINDENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetLayoutCache(int mode, BOOL bDirect)
{
  Call(SCI_SETLAYOUTCACHE, (WPARAM) mode, 0, bDirect);
}

int CScintillaCtrl::GetLayoutCache(BOOL bDirect)
{
  return Call(SCI_GETLAYOUTCACHE, 0, 0, bDirect);
}

void CScintillaCtrl::SetScrollWidth(int pixelWidth, BOOL bDirect)
{
  Call(SCI_SETSCROLLWIDTH, (WPARAM) pixelWidth, 0, bDirect);
}

int CScintillaCtrl::GetScrollWidth(BOOL bDirect)
{
  return Call(SCI_GETSCROLLWIDTH, 0, 0, bDirect);
}

int CScintillaCtrl::TextWidth(int style, const char* text, BOOL bDirect)
{
  return Call(SCI_TEXTWIDTH, (WPARAM) style, (LPARAM) text, bDirect);
}

void CScintillaCtrl::SetEndAtLastLine(BOOL endAtLastLine, BOOL bDirect)
{
  Call(SCI_SETENDATLASTLINE, (WPARAM) endAtLastLine, 0, bDirect);
}

int CScintillaCtrl::GetEndAtLastLine(BOOL bDirect)
{
  return Call(SCI_GETENDATLASTLINE, 0, 0, bDirect);
}

int CScintillaCtrl::TextHeight(int line, BOOL bDirect)
{
  return Call(SCI_TEXTHEIGHT, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::SetVScrollBar(BOOL show, BOOL bDirect)
{
  Call(SCI_SETVSCROLLBAR, (WPARAM) show, 0, bDirect);
}

BOOL CScintillaCtrl::GetVScrollBar(BOOL bDirect)
{
  return Call(SCI_GETVSCROLLBAR, 0, 0, bDirect);
}

void CScintillaCtrl::AppendText(int length, const char* text, BOOL bDirect)
{
  Call(SCI_APPENDTEXT, (WPARAM) length, (LPARAM) text, bDirect);
}

BOOL CScintillaCtrl::GetTwoPhaseDraw(BOOL bDirect)
{
  return Call(SCI_GETTWOPHASEDRAW, 0, 0, bDirect);
}

void CScintillaCtrl::SetTwoPhaseDraw(BOOL twoPhase, BOOL bDirect)
{
  Call(SCI_SETTWOPHASEDRAW, (WPARAM) twoPhase, 0, bDirect);
}

void CScintillaCtrl::TargetFromSelection(BOOL bDirect)
{
  Call(SCI_TARGETFROMSELECTION, 0, 0, bDirect);
}

void CScintillaCtrl::LinesJoin(BOOL bDirect)
{
  Call(SCI_LINESJOIN, 0, 0, bDirect);
}

void CScintillaCtrl::LinesSplit(int pixelWidth, BOOL bDirect)
{
  Call(SCI_LINESSPLIT, (WPARAM) pixelWidth, 0, bDirect);
}

void CScintillaCtrl::SetFoldMarginColour(BOOL useSetting, COLORREF back, BOOL bDirect)
{
  Call(SCI_SETFOLDMARGINCOLOUR, (WPARAM) useSetting, (LPARAM) back, bDirect);
}

void CScintillaCtrl::SetFoldMarginHiColour(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
  Call(SCI_SETFOLDMARGINHICOLOUR, (WPARAM) useSetting, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::LineDown(BOOL bDirect)
{
  Call(SCI_LINEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::LineDownExtend(BOOL bDirect)
{
  Call(SCI_LINEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineUp(BOOL bDirect)
{
  Call(SCI_LINEUP, 0, 0, bDirect);
}

void CScintillaCtrl::LineUpExtend(BOOL bDirect)
{
  Call(SCI_LINEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeft(BOOL bDirect)
{
  Call(SCI_CHARLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeftExtend(BOOL bDirect)
{
  Call(SCI_CHARLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharRight(BOOL bDirect)
{
  Call(SCI_CHARRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::CharRightExtend(BOOL bDirect)
{
  Call(SCI_CHARRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeft(BOOL bDirect)
{
  Call(SCI_WORDLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftExtend(BOOL bDirect)
{
  Call(SCI_WORDLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRight(BOOL bDirect)
{
  Call(SCI_WORDRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightExtend(BOOL bDirect)
{
  Call(SCI_WORDRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::Home(BOOL bDirect)
{
  Call(SCI_HOME, 0, 0, bDirect);
}

void CScintillaCtrl::HomeExtend(BOOL bDirect)
{
  Call(SCI_HOMEEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEnd(BOOL bDirect)
{
  Call(SCI_LINEEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndExtend(BOOL bDirect)
{
  Call(SCI_LINEENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentStart(BOOL bDirect)
{
  Call(SCI_DOCUMENTSTART, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentStartExtend(BOOL bDirect)
{
  Call(SCI_DOCUMENTSTARTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentEnd(BOOL bDirect)
{
  Call(SCI_DOCUMENTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentEndExtend(BOOL bDirect)
{
  Call(SCI_DOCUMENTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageUp(BOOL bDirect)
{
  Call(SCI_PAGEUP, 0, 0, bDirect);
}

void CScintillaCtrl::PageUpExtend(BOOL bDirect)
{
  Call(SCI_PAGEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageDown(BOOL bDirect)
{
  Call(SCI_PAGEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::PageDownExtend(BOOL bDirect)
{
  Call(SCI_PAGEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::EditToggleOvertype(BOOL bDirect)
{
  Call(SCI_EDITTOGGLEOVERTYPE, 0, 0, bDirect);
}

void CScintillaCtrl::Cancel(BOOL bDirect)
{
  Call(SCI_CANCEL, 0, 0, bDirect);
}

void CScintillaCtrl::DeleteBack(BOOL bDirect)
{
  Call(SCI_DELETEBACK, 0, 0, bDirect);
}

void CScintillaCtrl::Tab(BOOL bDirect)
{
  Call(SCI_TAB, 0, 0, bDirect);
}

void CScintillaCtrl::BackTab(BOOL bDirect)
{
  Call(SCI_BACKTAB, 0, 0, bDirect);
}

void CScintillaCtrl::NewLine(BOOL bDirect)
{
  Call(SCI_NEWLINE, 0, 0, bDirect);
}

void CScintillaCtrl::FormFeed(BOOL bDirect)
{
  Call(SCI_FORMFEED, 0, 0, bDirect);
}

void CScintillaCtrl::VCHome(BOOL bDirect)
{
  Call(SCI_VCHOME, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeExtend(BOOL bDirect)
{
  Call(SCI_VCHOMEEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::ZoomIn(BOOL bDirect)
{
  Call(SCI_ZOOMIN, 0, 0, bDirect);
}

void CScintillaCtrl::ZoomOut(BOOL bDirect)
{
  Call(SCI_ZOOMOUT, 0, 0, bDirect);
}

void CScintillaCtrl::DelWordLeft(BOOL bDirect)
{
  Call(SCI_DELWORDLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::DelWordRight(BOOL bDirect)
{
  Call(SCI_DELWORDRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::LineCut(BOOL bDirect)
{
  Call(SCI_LINECUT, 0, 0, bDirect);
}

void CScintillaCtrl::LineDelete(BOOL bDirect)
{
  Call(SCI_LINEDELETE, 0, 0, bDirect);
}

void CScintillaCtrl::LineTranspose(BOOL bDirect)
{
  Call(SCI_LINETRANSPOSE, 0, 0, bDirect);
}

void CScintillaCtrl::LineDuplicate(BOOL bDirect)
{
  Call(SCI_LINEDUPLICATE, 0, 0, bDirect);
}

void CScintillaCtrl::LowerCase(BOOL bDirect)
{
  Call(SCI_LOWERCASE, 0, 0, bDirect);
}

void CScintillaCtrl::UpperCase(BOOL bDirect)
{
  Call(SCI_UPPERCASE, 0, 0, bDirect);
}

void CScintillaCtrl::LineScrollDown(BOOL bDirect)
{
  Call(SCI_LINESCROLLDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::LineScrollUp(BOOL bDirect)
{
  Call(SCI_LINESCROLLUP, 0, 0, bDirect);
}

void CScintillaCtrl::DeleteBackNotLine(BOOL bDirect)
{
  Call(SCI_DELETEBACKNOTLINE, 0, 0, bDirect);
}

void CScintillaCtrl::HomeDisplay(BOOL bDirect)
{
  Call(SCI_HOMEDISPLAY, 0, 0, bDirect);
}

void CScintillaCtrl::HomeDisplayExtend(BOOL bDirect)
{
  Call(SCI_HOMEDISPLAYEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndDisplay(BOOL bDirect)
{
  Call(SCI_LINEENDDISPLAY, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndDisplayExtend(BOOL bDirect)
{
  Call(SCI_LINEENDDISPLAYEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::HomeWrap(BOOL bDirect)
{
  Call(SCI_HOMEWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::HomeWrapExtend(BOOL bDirect)
{
  Call(SCI_HOMEWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndWrap(BOOL bDirect)
{
  Call(SCI_LINEENDWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndWrapExtend(BOOL bDirect)
{
  Call(SCI_LINEENDWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeWrap(BOOL bDirect)
{
  Call(SCI_VCHOMEWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeWrapExtend(BOOL bDirect)
{
  Call(SCI_VCHOMEWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineCopy(BOOL bDirect)
{
  Call(SCI_LINECOPY, 0, 0, bDirect);
}

void CScintillaCtrl::MoveCaretInsideView(BOOL bDirect)
{
  Call(SCI_MOVECARETINSIDEVIEW, 0, 0, bDirect);
}

int CScintillaCtrl::LineLength(int line, BOOL bDirect)
{
  return Call(SCI_LINELENGTH, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::BraceHighlight(long pos1, long pos2, BOOL bDirect)
{
  Call(SCI_BRACEHIGHLIGHT, (WPARAM) pos1, (LPARAM) pos2, bDirect);
}

void CScintillaCtrl::BraceBadLight(long pos, BOOL bDirect)
{
  Call(SCI_BRACEBADLIGHT, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::BraceMatch(long pos, BOOL bDirect)
{
  return Call(SCI_BRACEMATCH, (WPARAM) pos, 0, bDirect);
}

BOOL CScintillaCtrl::GetViewEOL(BOOL bDirect)
{
  return Call(SCI_GETVIEWEOL, 0, 0, bDirect);
}

void CScintillaCtrl::SetViewEOL(BOOL visible, BOOL bDirect)
{
  Call(SCI_SETVIEWEOL, (WPARAM) visible, 0, bDirect);
}

int CScintillaCtrl::GetDocPointer(BOOL bDirect)
{
  return Call(SCI_GETDOCPOINTER, 0, 0, bDirect);
}

void CScintillaCtrl::SetDocPointer(int pointer, BOOL bDirect)
{
  Call(SCI_SETDOCPOINTER, 0, (LPARAM) pointer, bDirect);
}

void CScintillaCtrl::SetModEventMask(int mask, BOOL bDirect)
{
  Call(SCI_SETMODEVENTMASK, (WPARAM) mask, 0, bDirect);
}

int CScintillaCtrl::GetEdgeColumn(BOOL bDirect)
{
  return Call(SCI_GETEDGECOLUMN, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeColumn(int column, BOOL bDirect)
{
  Call(SCI_SETEDGECOLUMN, (WPARAM) column, 0, bDirect);
}

int CScintillaCtrl::GetEdgeMode(BOOL bDirect)
{
  return Call(SCI_GETEDGEMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeMode(int mode, BOOL bDirect)
{
  Call(SCI_SETEDGEMODE, (WPARAM) mode, 0, bDirect);
}

COLORREF CScintillaCtrl::GetEdgeColour(BOOL bDirect)
{
  return Call(SCI_GETEDGECOLOUR, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeColour(COLORREF edgeColour, BOOL bDirect)
{
  Call(SCI_SETEDGECOLOUR, (WPARAM) edgeColour, 0, bDirect);
}

void CScintillaCtrl::SearchAnchor(BOOL bDirect)
{
  Call(SCI_SEARCHANCHOR, 0, 0, bDirect);
}

int CScintillaCtrl::SearchNext(int flags, const char* text, BOOL bDirect)
{
  return Call(SCI_SEARCHNEXT, (WPARAM) flags, (LPARAM) text, bDirect);
}

int CScintillaCtrl::SearchPrev(int flags, const char* text, BOOL bDirect)
{
  return Call(SCI_SEARCHPREV, (WPARAM) flags, (LPARAM) text, bDirect);
}

int CScintillaCtrl::LinesOnScreen(BOOL bDirect)
{
  return Call(SCI_LINESONSCREEN, 0, 0, bDirect);
}

void CScintillaCtrl::UsePopUp(BOOL allowPopUp, BOOL bDirect)
{
  Call(SCI_USEPOPUP, (WPARAM) allowPopUp, 0, bDirect);
}

BOOL CScintillaCtrl::SelectionIsRectangle(BOOL bDirect)
{
  return Call(SCI_SELECTIONISRECTANGLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetZoom(int zoom, BOOL bDirect)
{
  Call(SCI_SETZOOM, (WPARAM) zoom, 0, bDirect);
}

int CScintillaCtrl::GetZoom(BOOL bDirect)
{
  return Call(SCI_GETZOOM, 0, 0, bDirect);
}

int CScintillaCtrl::CreateDocument(BOOL bDirect)
{
  return Call(SCI_CREATEDOCUMENT, 0, 0, bDirect);
}

void CScintillaCtrl::AddRefDocument(int doc, BOOL bDirect)
{
  Call(SCI_ADDREFDOCUMENT, 0, (LPARAM) doc, bDirect);
}

void CScintillaCtrl::ReleaseDocument(int doc, BOOL bDirect)
{
  Call(SCI_RELEASEDOCUMENT, 0, (LPARAM) doc, bDirect);
}

int CScintillaCtrl::GetModEventMask(BOOL bDirect)
{
  return Call(SCI_GETMODEVENTMASK, 0, 0, bDirect);
}

void CScintillaCtrl::SCISetFocus(BOOL focus, BOOL bDirect)
{
  Call(SCI_SETFOCUS, (WPARAM) focus, 0, bDirect);
}

BOOL CScintillaCtrl::GetFocus(BOOL bDirect)
{
  return Call(SCI_GETFOCUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetStatus(int statusCode, BOOL bDirect)
{
  Call(SCI_SETSTATUS, (WPARAM) statusCode, 0, bDirect);
}

int CScintillaCtrl::GetStatus(BOOL bDirect)
{
  return Call(SCI_GETSTATUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetMouseDownCaptures(BOOL captures, BOOL bDirect)
{
  Call(SCI_SETMOUSEDOWNCAPTURES, (WPARAM) captures, 0, bDirect);
}

BOOL CScintillaCtrl::GetMouseDownCaptures(BOOL bDirect)
{
  return Call(SCI_GETMOUSEDOWNCAPTURES, 0, 0, bDirect);
}

void CScintillaCtrl::SetCursor(int cursorType, BOOL bDirect)
{
  Call(SCI_SETCURSOR, (WPARAM) cursorType, 0, bDirect);
}

int CScintillaCtrl::GetCursor(BOOL bDirect)
{
  return Call(SCI_GETCURSOR, 0, 0, bDirect);
}

void CScintillaCtrl::SetControlCharSymbol(int symbol, BOOL bDirect)
{
  Call(SCI_SETCONTROLCHARSYMBOL, (WPARAM) symbol, 0, bDirect);
}

int CScintillaCtrl::GetControlCharSymbol(BOOL bDirect)
{
  return Call(SCI_GETCONTROLCHARSYMBOL, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartLeft(BOOL bDirect)
{
  Call(SCI_WORDPARTLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartLeftExtend(BOOL bDirect)
{
  Call(SCI_WORDPARTLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartRight(BOOL bDirect)
{
  Call(SCI_WORDPARTRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartRightExtend(BOOL bDirect)
{
  Call(SCI_WORDPARTRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetVisiblePolicy(int visiblePolicy, int visibleSlop, BOOL bDirect)
{
  Call(SCI_SETVISIBLEPOLICY, (WPARAM) visiblePolicy, (LPARAM) visibleSlop, bDirect);
}

void CScintillaCtrl::DelLineLeft(BOOL bDirect)
{
  Call(SCI_DELLINELEFT, 0, 0, bDirect);
}

void CScintillaCtrl::DelLineRight(BOOL bDirect)
{
  Call(SCI_DELLINERIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::SetXOffset(int newOffset, BOOL bDirect)
{
  Call(SCI_SETXOFFSET, (WPARAM) newOffset, 0, bDirect);
}

int CScintillaCtrl::GetXOffset(BOOL bDirect)
{
  return Call(SCI_GETXOFFSET, 0, 0, bDirect);
}

void CScintillaCtrl::ChooseCaretX(BOOL bDirect)
{
  Call(SCI_CHOOSECARETX, 0, 0, bDirect);
}

void CScintillaCtrl::GrabFocus(BOOL bDirect)
{
  Call(SCI_GRABFOCUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetXCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect)
{
  Call(SCI_SETXCARETPOLICY, (WPARAM) caretPolicy, (LPARAM) caretSlop, bDirect);
}

void CScintillaCtrl::SetYCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect)
{
  Call(SCI_SETYCARETPOLICY, (WPARAM) caretPolicy, (LPARAM) caretSlop, bDirect);
}

void CScintillaCtrl::SetPrintWrapMode(int mode, BOOL bDirect)
{
  Call(SCI_SETPRINTWRAPMODE, (WPARAM) mode, 0, bDirect);
}

int CScintillaCtrl::GetPrintWrapMode(BOOL bDirect)
{
  return Call(SCI_GETPRINTWRAPMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetHotspotActiveFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
  Call(SCI_SETHOTSPOTACTIVEFORE, (WPARAM) useSetting, (LPARAM) fore, bDirect);
}

void CScintillaCtrl::SetHotspotActiveBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
  Call(SCI_SETHOTSPOTACTIVEBACK, (WPARAM) useSetting, (LPARAM) back, bDirect);
}

void CScintillaCtrl::SetHotspotActiveUnderline(BOOL underline, BOOL bDirect)
{
  Call(SCI_SETHOTSPOTACTIVEUNDERLINE, (WPARAM) underline, 0, bDirect);
}

void CScintillaCtrl::SetHotspotSingleLine(BOOL singleLine, BOOL bDirect)
{
  Call(SCI_SETHOTSPOTSINGLELINE, (WPARAM) singleLine, 0, bDirect);
}

void CScintillaCtrl::ParaDown(BOOL bDirect)
{
  Call(SCI_PARADOWN, 0, 0, bDirect);
}

void CScintillaCtrl::ParaDownExtend(BOOL bDirect)
{
  Call(SCI_PARADOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::ParaUp(BOOL bDirect)
{
  Call(SCI_PARAUP, 0, 0, bDirect);
}

void CScintillaCtrl::ParaUpExtend(BOOL bDirect)
{
  Call(SCI_PARAUPEXTEND, 0, 0, bDirect);
}

long CScintillaCtrl::PositionBefore(long pos, BOOL bDirect)
{
  return Call(SCI_POSITIONBEFORE, (WPARAM) pos, 0, bDirect);
}

long CScintillaCtrl::PositionAfter(long pos, BOOL bDirect)
{
  return Call(SCI_POSITIONAFTER, (WPARAM) pos, 0, bDirect);
}

void CScintillaCtrl::CopyRange(long start, long end, BOOL bDirect)
{
  Call(SCI_COPYRANGE, (WPARAM) start, (LPARAM) end, bDirect);
}

void CScintillaCtrl::CopyText(int length, const char* text, BOOL bDirect)
{
  Call(SCI_COPYTEXT, (WPARAM) length, (LPARAM) text, bDirect);
}

void CScintillaCtrl::SetSelectionMode(int mode, BOOL bDirect)
{
  Call(SCI_SETSELECTIONMODE, (WPARAM) mode, 0, bDirect);
}

int CScintillaCtrl::GetSelectionMode(BOOL bDirect)
{
  return Call(SCI_GETSELECTIONMODE, 0, 0, bDirect);
}

long CScintillaCtrl::GetLineSelStartPosition(int line, BOOL bDirect)
{
  return Call(SCI_GETLINESELSTARTPOSITION, (WPARAM) line, 0, bDirect);
}

long CScintillaCtrl::GetLineSelEndPosition(int line, BOOL bDirect)
{
  return Call(SCI_GETLINESELENDPOSITION, (WPARAM) line, 0, bDirect);
}

void CScintillaCtrl::LineDownRectExtend(BOOL bDirect)
{
  Call(SCI_LINEDOWNRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineUpRectExtend(BOOL bDirect)
{
  Call(SCI_LINEUPRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeftRectExtend(BOOL bDirect)
{
  Call(SCI_CHARLEFTRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharRightRectExtend(BOOL bDirect)
{
  Call(SCI_CHARRIGHTRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::HomeRectExtend(BOOL bDirect)
{
  Call(SCI_HOMERECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeRectExtend(BOOL bDirect)
{
  Call(SCI_VCHOMERECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndRectExtend(BOOL bDirect)
{
  Call(SCI_LINEENDRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageUpRectExtend(BOOL bDirect)
{
  Call(SCI_PAGEUPRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageDownRectExtend(BOOL bDirect)
{
  Call(SCI_PAGEDOWNRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageUp(BOOL bDirect)
{
  Call(SCI_STUTTEREDPAGEUP, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageUpExtend(BOOL bDirect)
{
  Call(SCI_STUTTEREDPAGEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageDown(BOOL bDirect)
{
  Call(SCI_STUTTEREDPAGEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageDownExtend(BOOL bDirect)
{
  Call(SCI_STUTTEREDPAGEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftEnd(BOOL bDirect)
{
  Call(SCI_WORDLEFTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftEndExtend(BOOL bDirect)
{
  Call(SCI_WORDLEFTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightEnd(BOOL bDirect)
{
  Call(SCI_WORDRIGHTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightEndExtend(BOOL bDirect)
{
  Call(SCI_WORDRIGHTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetWhitespaceChars(const char* characters, BOOL bDirect)
{
  Call(SCI_SETWHITESPACECHARS, 0, (LPARAM) characters, bDirect);
}

void CScintillaCtrl::SetCharsDefault(BOOL bDirect)
{
  Call(SCI_SETCHARSDEFAULT, 0, 0, bDirect);
}

int CScintillaCtrl::AutoCGetCurrent(BOOL bDirect)
{
  return Call(SCI_AUTOCGETCURRENT, 0, 0, bDirect);
}

void CScintillaCtrl::Allocate(int bytes, BOOL bDirect)
{
  Call(SCI_ALLOCATE, (WPARAM) bytes, 0, bDirect);
}

void CScintillaCtrl::StartRecord(BOOL bDirect)
{
  Call(SCI_STARTRECORD, 0, 0, bDirect);
}

void CScintillaCtrl::StopRecord(BOOL bDirect)
{
  Call(SCI_STOPRECORD, 0, 0, bDirect);
}

void CScintillaCtrl::SetLexer(int lexer, BOOL bDirect)
{
  Call(SCI_SETLEXER, (WPARAM) lexer, 0, bDirect);
}

int CScintillaCtrl::GetLexer(BOOL bDirect)
{
  return Call(SCI_GETLEXER, 0, 0, bDirect);
}

void CScintillaCtrl::Colourise(long start, long end, BOOL bDirect)
{
  Call(SCI_COLOURISE, (WPARAM) start, (LPARAM) end, bDirect);
}

void CScintillaCtrl::SetProperty(const char* key, const char* value, BOOL bDirect)
{
  Call(SCI_SETPROPERTY, (WPARAM) key, (LPARAM) value, bDirect);
}

void CScintillaCtrl::SetKeyWords(int keywordSet, const char* keyWords, BOOL bDirect)
{
  Call(SCI_SETKEYWORDS, (WPARAM) keywordSet, (LPARAM) keyWords, bDirect);
}

void CScintillaCtrl::SetLexerLanguage(const char* language, BOOL bDirect)
{
  Call(SCI_SETLEXERLANGUAGE, 0, (LPARAM) language, bDirect);
}

void CScintillaCtrl::LoadLexerLibrary(const char* path, BOOL bDirect)
{
  Call(SCI_LOADLEXERLIBRARY, 0, (LPARAM) path, bDirect);
}
