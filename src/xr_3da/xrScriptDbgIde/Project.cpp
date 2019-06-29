// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "Project.h"

#include "WorkspaceWnd.h"
#include "TreeViewFiles.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "MainFrame.h"
#include "ScintillaView.h"
#include "DebugDialog.h"
#include "NewProjectDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CProject::CProject()
{
	m_ss_working_folder  = 	AfxGetApp()->GetProfileString("options","sSafeFolder", "" );
	SetModifiedFlag(FALSE);
}

CProject::~CProject()
{
	RemoveProjectFiles();
}


void CProject::RedrawFilesTree()
{
	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->RemoveAll();

	pTree->AddRoot(GetName());

	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		pTree->AddProjectFile(m_files[i]->GetNameExt(), m_files[i]);

	pTree->ExpandFiles();
}


CProjectFile* CProject::GetProjectFile(CString strPathName)
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i]->Is(strPathName) )
			return m_files[i];

	AddFile(strPathName, FALSE);
	nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i]->Is(strPathName) )
			return m_files[i];

	return NULL;
}

void CProject::AddFiles()
{
	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT, 
		"Script files (*.script)|*.script|All files (*.*)|*.*||", g_mainFrame);

	TCHAR FileNameList[8192]= {0};
	fd.m_ofn.lpstrFile = FileNameList;
	fd.m_ofn.nMaxFile=8192;

	if ( fd.DoModal()==IDOK )
	{
		POSITION pos = fd.GetStartPosition();
		while (pos)
		{
			CString strPathName = fd.GetNextPathName(pos);
			AddFile(strPathName);
			CProjectFile* pF = GetProjectFile(strPathName);
			theApp.OpenProjectFilesView(pF);
		}
	}

}

void CProject::AddFile(CString strPathName, BOOL bFindExisting)
{
 	CProjectFile* pPF;
	if ( (bFindExisting)&&(pPF=GetProjectFile(strPathName)) != NULL )
		return;

	pPF = new CProjectFile;
	pPF->SetPathName(strPathName);

	AddFile(pPF);
}

void CProject::AddFile(CProjectFile* pPF)
{
	m_files.Add(pPF);

	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->AddProjectFile(pPF->GetNameExt(), pPF);

	SetModifiedFlag(TRUE);
}

void CProject::RemoveFile(CProjectFile *pPF)
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i] == pPF )
		{
			delete m_files.GetAt(i);
			m_files.RemoveAt(i);
			break;
		}

	SetModifiedFlag(TRUE);
}

void CProject::RemoveProjectFiles()
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
	{
		CLuaView *pView = theApp.FindProjectFilesView(m_files[i]);
		if ( pView )
			pView->CloseFrame();
	}

	for ( i=0; i<nSize; ++i )
		delete m_files[i];

	m_files.RemoveAll();
}


BOOL CProject::New()
{
	SaveModified();
	Close();
	DWORD				sz_user	= 64;
	char				UserName[64];

	GetUserName			(UserName,&sz_user);
	

	CNewProjectDlg dlg;
	CString s;
	s.Format("%s_scr_dbg",UserName);
	dlg.SetFileName(s);
	if ( dlg.DoModal()!=IDOK ){
		return FALSE;
	}
	m_strName = dlg.GetProjectName();

	RedrawFilesTree();

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;

}

BOOL CProject::Close()
{
	SaveModified();
	SetName("");

	RemoveProjectFiles();

	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->RemoveAll();

	return TRUE;
}

BOOL CProject::Load()
{
	SaveModified();

	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", g_mainFrame);

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	return Load(fd.GetPathName());
}

BOOL CProject::Load(CString sFileName)
{

	CFile fin;
	if( !fin.Open(sFileName,CFile::modeRead) )
	{
		CString msg;
		msg.Format("Unable to open project file %s", sFileName);
		AfxMessageBox(msg);
		return FALSE;
	}


	m_strName = sFileName;

	CArchive ar(&fin, CArchive::load);

	RemoveProjectFiles();
	
	RedrawFilesTree();
	BOOL bResult = Load(ar);

	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CProject::Load(CArchive &ar)
{
	int version;
	ar >> version;
	if( (version > PROJ_VERSION) || (version < 0x03)){
		AfxMessageBox("Project file is corrupted. Re-create project.");
		return FALSE;
	}

	if(version>=0x04){
		CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
		CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
		pTree->Load(ar);
	}

	long nFiles;
	ar >> nFiles;

	for ( int i=0; i<nFiles; ++i )
	{
		CProjectFile *pPF = new CProjectFile;
		pPF->Load(ar);

		AddFile(pPF);
	}

	try{
		ar >> m_command;
		ar >> m_arguments;
		ar >> m_working_dir;

		if(version >0x04){
		int open_doc_cnt;
			ar >> open_doc_cnt;
			for(int i=0; i<open_doc_cnt; ++i){
			CString f_name;
			ar >> f_name;
			CProjectFile* pF = GetProjectFile(f_name);
			theApp.OpenProjectFilesView(pF);
			}
		
		}
	}
	catch( ... )
	{
		//fake.
		//new version of file saving :)
	}

	return TRUE;
}

BOOL CProject::Save()
{
	return Save( GetName() );
}

BOOL CProject::Save(CString strPathName)
{
	CFile fout;
	if ( !fout.Open(strPathName, CFile::modeCreate|CFile::modeWrite) )
	{
		CString msg;
		msg.Format("Unable to save project file %s",strPathName);
		AfxMessageBox(msg);
		return FALSE;
	}

	m_strName = strPathName;

	CArchive ar(&fout, CArchive::store);

	BOOL bResult = Save(ar);

	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CProject::Save(CArchive &ar)
{
	int version = PROJ_VERSION;
	ar << version;

	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->Save(ar);

	long nFiles = m_files.GetSize();
	ar << nFiles;

	for ( int i=0; i<nFiles; ++i )
		m_files[i]->Save(ar);

	ar << m_command;
	ar << m_arguments;
	ar << m_working_dir;

	
	int open_doc_cnt=0;
	POSITION pos = theApp.m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL){
		theApp.m_pLuaTemplate->GetNextDoc(pos);
		++open_doc_cnt;
	}
	ar << open_doc_cnt;

	pos = theApp.m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)theApp.m_pLuaTemplate->GetNextDoc(pos);
		ar << pDoc->GetProjectFile()->GetName();
	}

	return TRUE;
}

BOOL CProject::SaveAs()
{
	CFileDialog fd(FALSE, "lpr", GetName(), OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)", g_mainFrame);

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	SetName (fd.GetPathName() );

	return Save();
}


void CProject::SaveModified()
{
	if ( m_bModified )
	{
		if ( AfxMessageBox("Your project has changed. Do you want to save it?", MB_YESNO)==IDYES )
			Save();
	}
}

void CProject::FillBreakPoints(CMailSlotMsg* msg)
{
	if(!m_files.GetSize()){
		msg->w_int(0);
		return;
	};
		
	int nCnt = 0;
	CProjectFile* pf;
	for(int i=0;i<m_files.GetSize();++i)
	{
		pf = m_files[i];
		if( pf->HasBreakPoint() )
			++nCnt;
	};

	msg->w_int(nCnt);
	for(int i=0;i<m_files.GetSize();++i)
	{
		pf = m_files[i];
		if( pf->HasBreakPoint() )
			pf->FillBreakPoints(msg);
	};
}

BOOL CProject::HasBreakPoint(const char *szFile, int nLine)
{
	CProjectFile *pPF = GetProjectFile(szFile);
	if ( pPF==NULL )
		return FALSE;

	return pPF->HasBreakPoint(nLine);
}

void CProject::OnRunApplication()
{
	if( m_command.IsEmpty() )
	{
		AfxMessageBox("Debugging params not defined");
		OnDebugOptions();
		return;
	}
	
	CString strCmdLine;
	strCmdLine.Format("%s%s",m_working_dir.GetBuffer(),m_command.GetBuffer() );

	CString curDir;
	if(!m_working_dir.IsEmpty()){
		GetCurrentDirectoryA(2048,curDir.GetBuffer(2048));
		SetCurrentDirectoryA(m_working_dir.GetBuffer());
	};
	spawnl(_P_NOWAIT, m_command.GetBuffer(), (m_arguments.IsEmpty())?" ":m_arguments.GetBuffer());
	if(!m_working_dir.IsEmpty())
		SetCurrentDirectoryA(curDir.GetBuffer());
}

void CProject::OnDebugOptions()
{
	CDebugDialog d;

	d.m_command			= m_command;
	d.m_arguments		= m_arguments;
	d.m_working_dir		= m_working_dir;

	if ( d.DoModal()!=IDOK )
		return ;

	m_command			= d.m_command;
	m_arguments			= d.m_arguments;
	m_working_dir		= d.m_working_dir;

	SetModifiedFlag(TRUE);
}

void CProject::SetModifiedFlag(BOOL bModified) 
{ 
	m_bModified = bModified; 
};

void CProject::AddFolder()
{
	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	CString f_name("New Folder");
	pTree->AddFolder(f_name);
	SetModifiedFlag(TRUE);
}

void CProject::CreateBreakPointList(CString& str)
{
	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		m_files[i]->CreateBreakPointList(str);
}

void CProject::CreateFunctionsList(CString& str)
{
	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		m_files[i]->CreateFunctionsList(str);
}

void CProject::CreateWordList(LPSTR start_word,CString& str)
{
	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		m_files[i]->CreateWordList(start_word,str);
}

void CProject::NewFile(CString& fn)
{
	CFile fin;

	if( !fin.Open(fn,CFile::modeCreate) ){
		CString msg;
		msg.Format("Unable to create new file %s", fn);
		AfxMessageBox(msg);
		return;
	}
	fin.Close();

	CProjectFile *pPF = new CProjectFile;
	pPF->SetPathName( fn );
	AddFile(pPF);
//	pPF->Load();
}
