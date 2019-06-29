// ProjectFile.h: interface for the CProjectFile class.
//
#pragma once

class CLuaEditor;
class CMailSlotMsg;

enum EVSSStatus{
	vss_unknown = 0,
	vss_no_ss,
	vss_not_checked_out,
	vss_checked_out,
	vss_checked_out_me
};

class CProjectFile
{
	CLuaView*	m_pluaview;


public:
	CLuaView*	GetLuaView(){return m_pluaview;}
	void		SetLuaView(CLuaView* lw){m_pluaview = lw;}
	bool		SaveFile();
	void		ReloadFile();
	BOOL		Save(CArchive& ar);
	BOOL		Load(CArchive& ar);
	void		RemoveBreakPoint(int nLine);
	BOOL		Is(CString& strPathName);
	void		SetBreakPointsIn(CLuaEditor* pEditor);
	BOOL		HasBreakPoint(int nLine);
				CProjectFile();
	virtual		~CProjectFile();

	void		AddDebugLine(int nLine);
	void		RemoveAllDebugLines();
	void		AddBreakPoint(int nLine);
	void		RemoveAllBreakPoints();

	void		SetPathName(CString strPathName) { m_strPathName=strPathName; };
	CString		GetPathName() { return m_strPathName; };
	CString		GetName();
	CString		GetNameExt();
	void		FillBreakPoints(CMailSlotMsg*);
	BOOL		HasBreakPoint(){return m_breakPoints.GetSize();};

	void		CreateBreakPointList(CString& str);
	void		CreateFunctionsList(CString& str);
	void		CreateWordList(LPSTR start_word,CString& str);

	EVSSStatus		GetSS_status		();
	void			UpdateSS_status		();
	void			SS_check_in			();
	void			SS_check_out		();
	void			SS_undo_check_out	();
	void			SS_difference		();
	void			SS_get_latest		();
	void			SS_add_to_ss		();
	void			Check_view			();
protected:
	const CString&	getWorkingFolder	();
	void			Change_status(EVSSStatus st);
	CString			GetFileName(CString& str );


	EVSSStatus						m_ssStatus;
	CString							m_strPathName;
	CMap<int, int, BOOL, BOOL>		m_breakPoints;
	int								m_nMinBreakPoint;
	int								m_nMaxBreakPoint;
	CMap<int, int, BOOL, BOOL>		m_debugLines;
	int								m_nMinDebugLine;
	int								m_nMaxDebugLine;
};
