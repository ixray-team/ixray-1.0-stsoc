// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ide2.h"

#include "MainFrame.h"
#include "ScintillaView.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "SSOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ActivateXRAY();
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
CString g_calltip;
CString get_local_time()
{
	return CTime::GetCurrentTime().Format("%H;%M;%S");
}

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_COMMAND_EX(ID_VIEW_OUTPUT, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_WORKSPACE, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_CALLSTACK, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALLSTACK, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_LOCALS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCALS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_WATCHES, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHES, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_THREADS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THREADS, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_TOOLS_ALLOWCONNECTION, OnAllowConnectionCheck)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ALLOWCONNECTION, OnUpdateAllowConnection)

	
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPENPROJECT, OnFileOpenproject)
	ON_COMMAND(ID_FILE_SAVEPROJECT, OnFileSaveproject)
	ON_COMMAND(ID_FILE_SAVEPROJECTAS, OnFileSaveprojectas)
	ON_COMMAND(ID_PROJECT_ADD_FILES, OnProjectAddFiles)
	ON_COMMAND(ID_PROJECT_ADDFOLDER, OnAddFolder)

	ON_COMMAND(ID_PROJECT_RUNPROJECT, OnRunApplication)
	ON_COMMAND(ID_PROJECT_DEBUGGING,  OnDebuggingOptions)
	ON_WM_CLOSE()
	ON_COMMAND(ID_DEBUG_GO, OnDebugGo)
	ON_COMMAND(ID_DEBUG_STEPINTO, OnDebugStepinto)
	ON_COMMAND(ID_DEBUG_STEPOVER, OnDebugStepover)
	ON_COMMAND(ID_DEBUG_STEPOUT, OnDebugStepout)
	ON_COMMAND(ID_DEBUG_RUNTOCURSOR, OnDebugRuntocursor)
	ON_COMMAND(ID_DEBUG_STOPDEBUGGING, OnDebugStopdebugging)
	ON_COMMAND(ID_DEBUG_DEBUGBREAK, OnDebugBreak)

	ON_COMMAND(ID_FILE_NEWPROJECT, OnFileNewproject)
	ON_COMMAND(ID_FILE_CLOSEPROJECT, OnFileCloseproject)

	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPINTO, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPOVER, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPOUT, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_GO, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_RUNTOCURSOR, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STOPDEBUGGING, OnUpdateDebugMenu)


	ON_COMMAND(IDR_TOOLS_OPTIONS, OnToolsOptions)
	ON_UPDATE_COMMAND_UI(IDR_TOOLS_OPTIONS, OnUpdateToolsOptions)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateToolsOptions)
	
	ON_COMMAND(ID_EDIT_CLEAROUTPUT, OnClearOutput)

	ON_COMMAND(ID_FILE_SAVEALLMODIFIED, OnSaveAllModified)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALLMODIFIED, OnUpdateSaveAllModified)

	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	
	InitializeCriticalSection(&cs);

	m_needAnswer = FALSE;
	m_pMailSlotThread = NULL;
	m_mailSlot = NULL;

	m_allowConnections = AfxGetApp()->GetProfileInt("options","AllowConnections", 1);
	AllowConnections(m_allowConnections);
//	m_mailSlot = CreateMailSlotByName(IDE_MAIL_SLOT);
	m_hAccelNoProject = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_NO_PROJECT));
	m_hAccelDebug = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_DEBUG));
	m_hAccelDebugBreak = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_DEBUG_BREAK));
	m_do_thread_end = false;
}

CMainFrame::~CMainFrame()
{
	AfxGetApp()->WriteProfileString("options","last project", GetProject()->GetName() );
	AfxGetApp()->WriteProfileInt("options","AllowConnections", m_allowConnections );

	m_do_thread_end = true;

	if(m_pMailSlotThread){
		m_pMailSlotThread->m_bAutoDelete = FALSE;
		WaitForSingleObject(m_pMailSlotThread->m_hThread, INFINITE);
		delete m_pMailSlotThread;
		CloseHandle(m_mailSlot);
	};

	::DestroyAcceleratorTable(m_hAccelNoProject);
	::DestroyAcceleratorTable(m_hAccelDebug);
	::DestroyAcceleratorTable(m_hAccelDebugBreak);
}

void CMainFrame::AllowConnections(BOOL b)
{
	EnterCriticalSection(&cs);
	if(b){
		if(m_mailSlot)
			CloseHandle(m_mailSlot);

		m_mailSlot = CreateMailSlotByName(IDE_MAIL_SLOT);
	}else
		if(m_mailSlot){
			CloseHandle(m_mailSlot);
			m_mailSlot=NULL;
		}
	LeaveCriticalSection(&cs);

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndMDIClient.SubclassWindow (m_hWndMDIClient)) 
	{
              TRACE ("Failed to subclass MDI client window\n");
              return (-1);
    }            

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
/*
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
*/
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	InitDockingWindows();

	LoadBarState(_T("Workspace"));


 	m_pMailSlotThread = AfxBeginThread(StartListener, this,0,0,CREATE_SUSPENDED);
 	if(m_pMailSlotThread!=NULL)
 	{
 		m_pMailSlotThread->m_bAutoDelete=false;
 		m_pMailSlotThread->ResumeThread();
 	}
/*	m_workingFolder = AfxGetApp()->GetProfileString("options","working folder", "e:\\qqqq");
	if (!checkExistingFolder(m_workingFolder+"\\*.*"))
		OnToolsOptions();
*/
	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::InitDockingWindows()
{
	SetInitialSize(125,125,225,225);

	if (!m_wndWorkspace.Create(this, ID_VIEW_WORKSPACE,
		_T("Workspace"), CSize(200,100), CBRS_LEFT))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;		// fail to create
	}
	
	if (!m_wndOutput.Create(this, ID_VIEW_OUTPUT,
		_T("Output"), CSize(300,100), CBRS_BOTTOM, "Debug output"))
	{
		TRACE0("Failed to create dialog bar m_wndOutput\n");
		return -1;		// fail to create
	}

	if (!m_wndCallStack.Create(this, ID_VIEW_CALLSTACK,
		_T("Call Stack"), CSize(300,100), CBRS_RIGHT))
	{
		TRACE0("Failed to create dialog bar m_wndCallStack\n");
		return -1;		// fail to create
	}

	if (!m_wndLocals.Create(this, ID_VIEW_LOCALS,
		_T("Local Variables"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndLocals\n");
		return -1;		// fail to create
	}

	if (!m_wndWatches.Create(this, ID_VIEW_WATCHES,
		_T("Watches"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndWatches\n");
		return -1;		// fail to create
	}
	

	if (!m_wndThreads.Create(this, ID_VIEW_THREADS,
		_T("Script Threads"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndThreads\n");
		return -1;		// fail to create
	}
	
/*	if (!m_consoleBar.Create(this, ID_VIEW_LUARUNTIME,
		_T("Lua Console"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_consoleBar\n");
		return -1;		// fail to create
	}
*/	

	m_wndWorkspace.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndCallStack.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndLocals.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndWatches.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndThreads.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
//	m_consoleBar.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);

	EnableDockingSizeBar(CBRS_ALIGN_ANY);
	DockSizeBar(&m_wndWorkspace);
	DockSizeBar(&m_wndOutput);
	DockSizeBar(&m_wndCallStack);
	DockSizeBar(&m_wndLocals);
	DockSizeBar(&m_wndWatches);
	DockSizeBar(&m_wndThreads);
//	DockSizeBar(&m_consoleBar);


	return TRUE;
}

void CMainFrame::OnFileOpenproject() 
{
	if ( GetProject()->Load() )
	{
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnFileSaveproject() 
{
	GetProject()->Save();
	OnUpdateFrameTitle(TRUE);
}

void CMainFrame::OnFileSaveprojectas() 
{
	GetProject()->SaveAs();
	OnUpdateFrameTitle(TRUE);
}

void CMainFrame::OnProjectAddFiles() 
{
	GetProject()->AddFiles();
}
void CMainFrame::OnAddFolder()
{
	GetProject()->AddFolder();
}
void CMainFrame::OnRunApplication()
{
	GetProject()->OnRunApplication();
}

void CMainFrame::OnDebuggingOptions()
{
	GetProject()->OnDebugOptions();
}

void CMainFrame::OnClose() 
{
	if(m_needAnswer){
		CMailSlotMsg msg;
		msg.w_int(DMSG_DEBUG_GO);
		if(CheckExisting(DEBUGGER_MAIL_SLOT))
			SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
		m_needAnswer = FALSE;
	};

	SaveBarState(_T("Workspace"));
	if ( m_nAppMode!=modeNoProject )
		GetProject()->SaveModified();
	
	CMDIFrameWnd::OnClose();
}



CLuaView* CMainFrame::GetActiveView()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	if ( !pChild )
		return NULL;

	CView *pView = pChild->GetActiveView();
	if ( pView && pView->IsKindOf(RUNTIME_CLASS(CLuaView)) )
		return (CLuaView*)pView;

	return NULL;
}



LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_MDINEXT){
		int uu = 0;
	}
	if ( message >= _DMSG_FIRST_MSG && message <= _DMSG_LAST_MSG )
		return DebugMessage(message, wParam, lParam);
	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

LRESULT CMainFrame::DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch( nMsg )
	{
	case DMSG_NEW_CONNECTION:{
			m_wndCallStack.Clear();
			m_wndLocals.RemoveAll();
			m_wndThreads.RemoveAll();
			//GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Clear();
			CString msg;
			msg.Format("%s %s",get_local_time(),"Script debugger connected...");
			GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(msg);
			OnUpdateFrameTitle(TRUE);
		}break;

	case DMSG_CLOSE_CONNECTION:{
			CString msg;
			msg.Format("%s %s",get_local_time(),"Script debugger disconnected...");
			GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(msg);
			m_needAnswer = FALSE;
			OnUpdateFrameTitle(TRUE);
		}break;

	case DMSG_ACTIVATE_IDE:
			
			m_needAnswer = TRUE;
			OnUpdateFrameTitle(TRUE);
			SetMode(modeDebugBreak);
			m_wndWatches.Redraw();
			::SetForegroundWindow(g_mainFrame->m_hWnd);
		break;

	case DMSG_WRITE_DEBUG:{
			CString msg=(const char*)wParam;
			GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(msg);
			}break;

	case DMSG_GOTO_FILELINE:
			GotoFileLine((const char*)wParam, (int)lParam);
		break;

	case DMSG_DEBUG_START:
			SetMode(modeDebug);
		break;

	case DMSG_DEBUG_END:
			SetMode(modeIdle);
		break;

	case DMSG_CLEAR_STACKTRACE:
			m_wndCallStack.Clear();
		break;

	case DMSG_ADD_STACKTRACE:
			m_wndCallStack.Add(((StackTrace*)wParam)->szDesc, 
				((StackTrace*)wParam)->szFile, 
				((StackTrace*)wParam)->nLine);
		break;

	case DMSG_GOTO_STACKTRACE_LEVEL:
			m_wndCallStack.GotoStackTraceLevel(wParam);
		break;

	case DMSG_GET_STACKTRACE_LEVEL:
			return m_wndCallStack.GetLevel();
		break;

	case DMSG_CLEAR_LOCALVARIABLES:
			m_wndLocals.RemoveAll();
		break;

	case DMSG_ADD_LOCALVARIABLE:
			m_wndLocals.AddVariable((Variable*)wParam);
		break;

	case DMSG_CLEAR_THREADS:
			m_wndThreads.RemoveAll();
		break;

	case DMSG_ADD_THREAD:
			m_wndThreads.AddThread((SScriptThread*)wParam);
		break;

/*	case DMSG_REDRAW_WATCHES:
			m_wndWatches.Redraw();
		break;
*/

	case DMSG_EVAL_WATCH:
		if((int)lParam == 9999){
				g_calltip = (LPSTR)wParam;
				return 0;
		}

			m_wndWatches.SetResult((int)lParam, (LPSTR)wParam);
		break;

	case DMSG_GET_BREAKPOINTS:
			SendBreakPoints();
		break;
	}

	return 0;
}

void CMainFrame::GotoFileLine(const char *szFile, int nLine)
{
	CProjectFile* pPF = GetProject()->GetProjectFile(szFile);

	if ( pPF )
		theApp.OpenProjectFilesView(pPF, nLine);
}

void CMainFrame::SetMode(int nMode)
{
	m_nAppMode = nMode;
	switch ( nMode )
	{
	case modeNoProject:
		m_hAccelTable = m_hAccelNoProject;
		m_wndCallStack.Clear();
		break;
	case modeIdle:
		m_hAccelTable = m_hAccelDebug;
		m_wndCallStack.Clear();
		break;
	case modeDebug:
		m_hAccelTable = m_hAccelDebug;
		m_wndCallStack.Clear();
		break;
	case modeDebugBreak:
		m_hAccelTable = m_hAccelDebugBreak;
		break;
	}

	m_wndMDIClient.ResetMenu();
	DrawMenuBar();

	OnUpdateFrameTitle(TRUE);
}

void CMainFrame::OnUpdateDebugMenu(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_needAnswer);
	if(pCmdUI->m_nID==ID_DEBUG_DEBUGBREAK)
		pCmdUI->Enable(!m_needAnswer);

//	if(pCmdUI->m_nID==ID_DEBUG_RUNTOCURSOR)
//		pCmdUI->Enable(TRUE);//fake

}	

void CMainFrame::OnDebugGo() 
{
	if (!m_needAnswer)return;

	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_GO);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);

	ActivateXRAY();
}


void CMainFrame::OnDebugStepinto() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_INTO);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
	
	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);

	ActivateXRAY();
}

void CMainFrame::OnDebugStepover() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_OVER);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);

	ActivateXRAY();
}

void CMainFrame::OnDebugStepout() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_OUT);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);
	ActivateXRAY();
}

void CMainFrame::OnDebugRuntocursor() 
{
/*	SScriptThread st;
	st.active = true;
	st.scriptID = 558;
	m_wndThreads.AddThread(&st);

	return;*/
	CString smsg = "Not implemented yet...";
	GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(smsg);
	SendBreakPoints();
	return;
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_RUN_TO_CURSOR);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);

	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);

	ActivateXRAY();
}

void CMainFrame::OnDebugBreak()
{
	if (m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_DEBUG_BREAK);
	CString smsg;
	if( CheckExisting (DEBUGGER_MAIL_SLOT) ){
		SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);
		smsg = "Initiated debug break...";
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(smsg);
	}else{
		smsg = "Debugger not present...\n";
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(smsg);
	}

	ActivateXRAY();
}


void CMainFrame::OnDebugStopdebugging() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_STOP_DEBUGGING);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);

	m_needAnswer = FALSE;
	OnUpdateFrameTitle(TRUE);
}



void CMainFrame::OpenDefaultProject()
{
	CString sFileName;
	
	DWORD				sz_user	= 64;

	sFileName = AfxGetApp()->GetProfileString("options","last project", "" );
	if(sFileName.IsEmpty())
		return;


  HANDLE hFile = CreateFile(sFileName,
			GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE){
		CString msg;
		msg.Format("Couldn't open last project file %s", sFileName);
		AfxMessageBox(msg);
		return;
  }
	CloseHandle(hFile);

	GetProject()->Load(sFileName);
	SetMode(modeIdle);
	m_wndWorkspace.Enable(TRUE);
	OnUpdateFrameTitle(TRUE);

/*	
	if ( !bFileExist )
	{
		GetProject()->New(sFullFileName);
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}else{
		GetProject()->Load(sFullFileName);
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
*/
}

void CMainFrame::OnFileNewproject() 
{
	if ( GetProject()->New() )
	{
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnFileCloseproject() 
{
	if ( GetProject()->Close() )
	{
		SetMode(modeNoProject);
		m_wndWorkspace.Enable(FALSE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

	CLuaView* pView = GetActiveView();
	if ( pView && bAddToTitle )
		UpdateFrameTitleForDocument(pView->GetDocument()->GetTitle());
	else
		UpdateFrameTitleForDocument(NULL);
}

void CMainFrame::UpdateFrameTitleForDocument(LPCTSTR lpszDocName)
{
	// copy first part of title loaded at time of frame creation
	TCHAR szText[256+_MAX_PATH];

	if (GetStyle() & FWS_PREFIXTITLE)
	{
		szText[0] = '\0';   // start with nothing

		// get name of currently active view
		if (lpszDocName != NULL)
		{
			lstrcpy(szText, lpszDocName);
			// add current window # if needed
			if (m_nWindow > 0)
				wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
			lstrcat(szText, _T(" - "));
		}
		lstrcat(szText, m_strTitle);
	}
	else
	{
		// get name of currently active view
		if ( m_nAppMode!=modeNoProject )
		{
			lstrcpy(szText, m_project.GetName());
			lstrcat(szText, _T(" - "));
//			lstrcat(szText, m_strTitle);
		}else
			lstrcpy(szText, m_strTitle);

		if(m_needAnswer)
			lstrcat(szText, _T(" [DEBUG BREAK] "));
		else
			lstrcat(szText, _T(" [NOT ACTIVE] "));

	}

	// set title if changed, but don't remove completely
	// Note: will be excessive for MDI Frame with maximized child
	AfxSetWindowText(m_hWnd, szText);
}

void CMainFrame::StackLevelChanged(int nLevel)
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
//	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int (DMSG_GOTO_STACKTRACE_LEVEL);
	msg.w_int(nLevel);
	if(CheckExisting(DEBUGGER_MAIL_SLOT))
		SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);
//	m_needAnswer = TRUE;
	OnUpdateFrameTitle(TRUE);
}

void CMainFrame::TranslateMsg( CMailSlotMsg& msg )
{
	int nType;
	msg.r_int(nType);
	
	switch(nType) {
	case DMSG_GET_BREAKPOINTS:
			SendMessage(DMSG_GET_BREAKPOINTS,0,0);
		break;
	case DMSG_NEW_CONNECTION:
			SendMessage(DMSG_NEW_CONNECTION,0,0);
		break;

	case DMSG_CLOSE_CONNECTION:
			SendMessage(DMSG_CLOSE_CONNECTION,0,0);
		break;

	case DMSG_ACTIVATE_IDE:
			SendMessage(DMSG_ACTIVATE_IDE,0,0);
		break;

	case DMSG_WRITE_DEBUG:{
			char str[2048];str[0]=0;
			msg.r_string(str);
			SendMessage(DMSG_WRITE_DEBUG,(WPARAM)(&str),0);
		}break;

	case DMSG_GOTO_FILELINE:{
			char str[2048];str[0]=0;
			int i;
			msg.r_string(str);
			msg.r_int(i);
			SendMessage (DMSG_GOTO_FILELINE,(WPARAM)str,(LPARAM)i);
		}break;

	case DMSG_CLEAR_STACKTRACE:{
			SendMessage(DMSG_CLEAR_STACKTRACE,0,0);
		}break;

	case DMSG_ADD_STACKTRACE:{
			StackTrace st;
			msg.r_buff((void*)(&st),sizeof(StackTrace));
			SendMessage(DMSG_ADD_STACKTRACE,(WPARAM)(&st),0);
		}break;

	case DMSG_CLEAR_LOCALVARIABLES:{
			SendMessage(DMSG_CLEAR_LOCALVARIABLES,0,0);
		}break;

	case DMSG_ADD_LOCALVARIABLE:{
			Variable v;
			msg.r_buff(&v,sizeof(Variable));
			SendMessage(DMSG_ADD_LOCALVARIABLE,(WPARAM)(&v),0);
		}break;

	case DMSG_CLEAR_THREADS:{
			SendMessage(DMSG_CLEAR_THREADS,0,0);
		}break;

	case DMSG_ADD_THREAD:{
			SScriptThread st;
			msg.r_buff(&st,sizeof(SScriptThread));
			SendMessage(DMSG_ADD_THREAD,(WPARAM)(&st),0);
		}break;
	
	case DMSG_EVAL_WATCH:{
			char str[2048];str[0]=0;
			int i;
			
			msg.r_string(str);
			msg.r_int(i);
			SendMessage(DMSG_EVAL_WATCH,(WPARAM)(str),(LPARAM)(i));
		}break;

	default:
		break;
	}
}
void CMainFrame::SendBreakPoints()
{
	CMailSlotMsg msg;
	msg.w_int(DMSG_GET_BREAKPOINTS);
	GetProject()->FillBreakPoints(&msg);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
}

UINT CMainFrame::StartListener( LPVOID pParam )
{
	return ((CMainFrame*)pParam)->StartListener();
}

UINT CMainFrame::StartListener()
{
	CMailSlotMsg msg;
	while(true){
		EnterCriticalSection(&cs);
		if( m_mailSlot && CheckMailslotMessage(m_mailSlot, msg) ){
			TranslateMsg(msg);
		};
		LeaveCriticalSection(&cs);
		Sleep(10);
		if(m_do_thread_end)
			AfxEndThread(0);
	};

}

BOOL file_exist(LPCSTR src)
{
	WIN32_FIND_DATA fd;
	HANDLE h = ::FindFirstFile(src, &fd);
	 BOOL res = h!=INVALID_HANDLE_VALUE;

	 FindClose(h);
	 return res;
}

BOOL CMainFrame::ErrorStringToFileLine(CString strError, CString &strPathName, int &nLine)
{
/*	CString strFileLine = strError;
	if ( strError.Left(4)=="luac" )
	strFileLine = strError.Mid(6);

	int nPos1 = strFileLine.Find(':',2);
	if ( nPos1==-1 )
	return FALSE;
	int nPos2 = strFileLine.Find(':', nPos1+1);
	if ( nPos2==-1 )
	return FALSE;

	CString strNum = strFileLine.Mid(nPos1+1, nPos2-nPos1-1);
	nLine = atoi(strNum);
	strPathName = strFileLine.Left(nPos1);
*/
	nLine = 1;
	char file_ext[] = ".script";
	int end_pos = strError.Find(file_ext);
	if(end_pos==-1)
		return FALSE;
	int end_file_pos = end_pos+strlen(file_ext);
	int start_linenum_pos = end_file_pos;
	//find file name
	int start_file_pos = strError.Find('&');
	if(start_file_pos==-1)
		return FALSE;
	strPathName = strError.Mid(start_file_pos,end_file_pos-start_file_pos);

	//find line num
	CString sLineNum;
	if(strError[start_linenum_pos]==':' )
		++start_linenum_pos;

	while( isdigit(strError[++start_linenum_pos] ) )
		sLineNum.AppendChar(strError[start_linenum_pos]);
	nLine =atoi( sLineNum.GetBuffer() );
	

	return TRUE;

}

void CMainFrame::ThreadChanged(int nThreadID)
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_THREAD_CHANGED);
	msg.w_int(nThreadID);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
}

void CMainFrame::OpenVarTable(char* varName)
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_GET_VAR_TABLE);
	msg.w_string(varName);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
}

CString encrypt(CString& src)
{
	CString res;
	for(int i=0; i<src.GetLength();++i){
		res.AppendChar(src.GetAt(i)^7);
	};
	return res;
}

void CMainFrame::EvalWatch(CString watch, int iItem)
{
//	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_EVAL_WATCH);
	msg.w_string(watch.GetBuffer());
	msg.w_int(iItem);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

}


void CMainFrame::OnToolsOptions()
{
	CSSOptionsDlg dlg;

	dlg.m_ss_ini_name	= AfxGetApp()->GetProfileString("options","sSafeIniFile", "" );
	dlg.m_ss_folder		= AfxGetApp()->GetProfileString("options","sSafeFolder", "" );
	dlg.m_ss_username	= AfxGetApp()->GetProfileString("options","sSafeUserName", "" );
	dlg.m_ss_userpass	= encrypt( AfxGetApp()->GetProfileString("options","sSafeUserPassword", "" ) );
	dlg.m_no_ss			= (dlg.m_ss_ini_name.GetLength()==0);

	if(dlg.DoModal()==IDOK){
		AfxGetApp()->WriteProfileString("options","sSafeIniFile", dlg.m_ss_ini_name );
		AfxGetApp()->WriteProfileString("options","sSafeFolder", dlg.m_ss_folder );
		AfxGetApp()->WriteProfileString("options","sSafeUserName", dlg.m_ss_username );
		AfxGetApp()->WriteProfileString("options","sSafeUserPassword", encrypt(dlg.m_ss_userpass) );
		AfxMessageBox("Restart the application for the changes to take effect");
	}
}

BOOL CMainFrame::checkExistingFolder(CString str)
{
	WIN32_FIND_DATA  data;
	HANDLE hr = FindFirstFile(str,&data);
	return (INVALID_HANDLE_VALUE != hr);
	
}

void CMainFrame::OnUpdateToolsOptions(CCmdUI* pCmdUI)
{
	if(pCmdUI->m_nID == ID_FILE_OPEN)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateSaveAllModified(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSaveAllModified()
{
	AfxGetApp()->SaveAllModified();
}
void CMainFrame::OnClearOutput()
{
	GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Clear();
}

void CMainFrame::GetCalltip(const char* str)
{
	g_calltip="";
	CMailSlotMsg msg;
	msg.w_int(DMSG_EVAL_WATCH);
	msg.w_string( str );
	msg.w_int(9999);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	DWORD tk_start = GetTickCount();
    MSG         msg_;
	while(!g_calltip.GetLength()){
		Sleep(500);
		if(PeekMessage( &msg_, NULL, 0U, 0U, PM_REMOVE )){
              TranslateMessage	( &msg_ );
              DispatchMessage	( &msg_ );
		}

		if(GetTickCount()-tk_start>5000)
			break;
	}
}

void CMainFrame::OnNewFile(CString& fn)
{
	GetProject()->NewFile(fn);
}

void ActivateXRAY()
{

	HWND h = FindWindow("_XRAY_","XRAY Engine");
	if(h){
		::SetForegroundWindow(h);
	}
}
/*
void CMainFrame::OnActivate(   UINT nState,   CWnd* pWndOther,   BOOL bMinimized )
{
	if(nState==WA_INACTIVE && pActive)
		this->MDIActivate(pActive);
	
	if((nState==WA_ACTIVE)||(nState==WA_CLICKACTIVE))
		pActive = this->MDIGetActive();
}*/




BOOL CMainFrame::OnAllowConnectionCheck(  UINT nID )
{
	m_allowConnections = !m_allowConnections;
	AllowConnections(m_allowConnections);
	return TRUE;
}

void CMainFrame::OnUpdateAllowConnection( CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_allowConnections);
}