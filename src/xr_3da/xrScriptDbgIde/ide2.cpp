// ide2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ide2.h"

#include "MainFrame.h"
#include "LuaFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "TreeViewFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMainFrame* g_mainFrame = NULL;
void checkRegVal();
/////////////////////////////////////////////////////////////////////////////
// CIdeApp

BEGIN_MESSAGE_MAP(CIdeApp, CWinApp)
	//{{AFX_MSG_MAP(CIdeApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdeApp construction

void CIdeApp::OnFileNew()
{
	CString fn;
	if( DoPromptFileName(fn, ID_FILE_NEW,
			0, TRUE, m_pLuaTemplate))
	g_mainFrame->OnNewFile(fn);
}

CIdeApp::CIdeApp()
{
	m_hScintilla = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIdeApp object

CIdeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIdeApp initialization

BOOL CIdeApp::InitInstance()
{
	m_hScintilla = LoadLibrary("SciLexer.DLL");
	if ( !m_hScintilla )
	{
		AfxMessageBox("Can't load Scintilla dll 'SciLexer.dll'",
			MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if(CheckExisting(IDE_MAIL_SLOT))
		return FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("xrScriptDebugIde"));
	checkRegVal();

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)




	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pLuaTemplate = new CMultiDocTemplate(
		IDR_IDE2TYPE,
		RUNTIME_CLASS(CLuaDoc),
		RUNTIME_CLASS(CLuaFrame), // custom MDI child frame
		RUNTIME_CLASS(CLuaView));
	AddDocTemplate(m_pLuaTemplate);

	// create main MDI Frame window
	 g_mainFrame = new CMainFrame;

	m_pMainWnd = g_mainFrame;

	if (!g_mainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	g_mainFrame->SetMode(CMainFrame::modeNoProject);
//	g_mainFrame->OpenDefaultProject();

	// Parse command line for standard shell commands, DDE, file open
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	// must be here because load frame of child view takes accel table from parent
	// and we want it to be null then
	// The main window has been initialized, so show and update it.
	g_mainFrame->ShowWindow(m_nCmdShow);
	g_mainFrame->UpdateWindow();
	
	LoadIcon(IDR_MAINFRAME);

CString ss_ini  = 	GetProfileString("options","sSafeIniFile", "" );
if(ss_ini.GetLength()>0){
		CString ss_username  = 	GetProfileString("options","sSafeUserName", "" );
		CString ss_userpass  = 	encrypt( GetProfileString("options","sSafeUserPassword", "" ) );
		m_ssConnection.b_Connect(ss_username,ss_userpass,ss_ini);
}

	m_comparerCmd = GetProfileString("options","fileComaprer", "" );
	m_comparerFormat = GetProfileString("options","fileComparerFormat", "" );

	g_mainFrame->OpenDefaultProject();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIdeApp message handlers


int CIdeApp::ExitInstance() 
{
	free(g_breakpointsArray);
	free(g_wordArray);
	free(g_funcArray);

	if ( m_hScintilla )
		FreeLibrary(m_hScintilla);		
	return CWinApp::ExitInstance();
}

CLuaView* CIdeApp::FindProjectFilesView(CProjectFile *pPF)
{
	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( pDoc->GetProjectFile() == pPF )
			return pDoc->GetView();
	}

	return NULL;
}

CLuaView* CIdeApp::LoadProjectFilesView(CProjectFile *pPF)
{
	CString path_name = pPF->GetPathName();
	if(!fileExist(path_name))
		return NULL;

	CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->OpenDocumentFile(path_name,TRUE);
	if ( pDoc )
		return pDoc->GetView();
	else
		return NULL;
}

CProjectFile * g_pPF = NULL;
CLuaView* CIdeApp::OpenProjectFilesView(CProjectFile *pPF, int nLine)
{
	g_pPF = pPF;
	pPF->UpdateSS_status();
	CLuaView* pView = FindProjectFilesView(pPF);
	if ( !pView )
		pView = LoadProjectFilesView(pPF);

	if ( pView )
	{
		pView->Activate();
		HTREEITEM itm = g_mainFrame->GetWorkspaceWnd()->GetTreeViewFiles()->FindFile(pPF->GetNameExt());
		g_mainFrame->GetWorkspaceWnd()->GetTreeViewFiles()->VSSUpdateStatus(itm);
		if ( nLine>=0 )
			pView->GetEditor()->GotoLine(nLine);

		pPF->SetLuaView(pView);
		pPF->Check_view();
	}

	return pView;
}

BOOL CIdeApp::SaveModifiedDocuments()
{
	BOOL bModified = FALSE;

	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( !pDoc->IsInProject() )
			continue;

		if ( pDoc->IsModified() )
		{
			pDoc->DoFileSave();
			bModified = TRUE;

			// writing new lines changes breakpoint positions
			pDoc->GetView()->GetEditor()->SetBreakPointsIn(pDoc->GetProjectFile());
		}
	}

	return bModified;
}

BOOL CIdeApp::fileExist(CString path_name)
{
	CFile f_test;

	if ( !f_test.Open(path_name, CFile::modeRead) )
	{
		CString msg;
		msg.Format("Unable to open file %s",path_name);
		AfxMessageBox(msg);
		return FALSE;
	}

	return TRUE;
}

void checkRegVal()
{
const char *g_rOptionsKey               = "SOFTWARE\\xrScriptDebugIde\\xRayScriptDebugger\\options";
const char *g_rSSafeIniName             = "sSafeIniFile";
const char *g_rSSafeFolder				= "sSafeFolder";
const char *g_rComparerCmd				= "fileComaprer";
const char *g_rComparerFormat			= "fileComparerFormat";
const char *g_rSSUserName				= "sSafeUserName";
const char *g_rSSUserPass				= "sSafeUserPassword";
const char *g_rAllowConn				= "AllowConnections";

//CString ss_username  = 	GetProfileString("options","sSafeUserName", "" );
//CString ss_userpass  = 	GetProfileString("options","sSafeUserPassword", "" );

const char *g_rSSafeIniDefValue			    = "\\\\X-RAY\\VSS$\\srcsafe.ini";
const char *g_rSSafeFolderDefValue          = "$/xrStalker/Scripts/";
const char *g_rComparerCmdDef				= "x:\\wincmp.exe";
const char *g_rComparerFormatDef			= " %s %s";
DWORD g_rAllowDef = 1;
	char g_rSSUserNameDef[128];
	DWORD	sz_user		= sizeof(g_rSSUserNameDef);
	GetUserName(g_rSSUserNameDef, &sz_user);


const char *g_rSSUserPassDef				= "";

HKEY hk;
DWORD keytype = REG_SZ;
DWORD keysize = MAX_PATH;
LONG res;

    if( ERROR_SUCCESS==RegOpenKey( HKEY_CURRENT_USER,g_rOptionsKey,&hk ) ){
            char keyvalue[32]="";
            
            keytype = REG_SZ;
            keysize = 32;
            res = RegQueryValueEx(hk,g_rSSafeIniName,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rSSafeIniName,0,REG_SZ,(LPBYTE)g_rSSafeIniDefValue,strlen(g_rSSafeIniDefValue)+1);

			res = RegQueryValueEx(hk,g_rSSafeFolder,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rSSafeFolder,0,REG_SZ,(LPBYTE)g_rSSafeFolderDefValue,strlen(g_rSSafeFolderDefValue)+1);

			res = RegQueryValueEx(hk,g_rComparerCmd,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rComparerCmd,0,REG_SZ,(LPBYTE)g_rComparerCmdDef,strlen(g_rComparerCmdDef)+1);

			res = RegQueryValueEx(hk,g_rComparerFormat,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rComparerFormat,0,REG_SZ,(LPBYTE)g_rComparerFormatDef,strlen(g_rComparerFormatDef)+1);

			res = RegQueryValueEx(hk,g_rSSUserName,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rSSUserName,0,REG_SZ,(LPBYTE)g_rSSUserNameDef,strlen(g_rSSUserNameDef)+1);

			res = RegQueryValueEx(hk,g_rSSUserPass,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rSSUserPass,0,REG_SZ,(LPBYTE)g_rSSUserPassDef,strlen(g_rSSUserPassDef)+1);

			res = RegQueryValueEx(hk,g_rAllowConn,0,&keytype,(LPBYTE)keyvalue, &keysize );
			if(res == ERROR_FILE_NOT_FOUND)
				RegSetValueEx(hk,g_rAllowConn,0,REG_DWORD,(LPBYTE)&g_rAllowDef,sizeof(DWORD));

			RegCloseKey( hk );
    }
}