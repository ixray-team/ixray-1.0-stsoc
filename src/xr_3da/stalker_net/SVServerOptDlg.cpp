// SVServerOpt.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "Version_Define.h"
#include "SVServerOptDlg.h"
#include ".\svserveroptdlg.h"


// SVServerOpt dialog

IMPLEMENT_DYNAMIC(SVServerOptDlg, CSubDlg)
SVServerOptDlg::SVServerOptDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVServerOptDlg::IDD, pParent)
{
}

SVServerOptDlg::~SVServerOptDlg()
{
}

void SVServerOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOSTNAME, m_pHostName);
	DDX_Control(pDX, IDC_PASSWORD, m_pPassword);
	DDX_Control(pDX, IDC_MAXPLAYERS, m_pMaxPlayers);
	DDX_Control(pDX, IDC_SPECTATORONLY, m_pSpectrMode);
	DDX_Control(pDX, IDC_SPECTRTIME, m_pSpectrSwitchTime);
	DDX_Control(pDX, IDC_DEDICATED, m_pDedicated);
	DDX_Control(pDX, IDC_PUBLIC, m_pPublic);
	DDX_Control(pDX, IDC_CDKEY, m_pCheckCDKey);
//	DDX_Control(pDX, IDC_MAPROTATION, m_pMapRotation);
//	DDX_Control(pDX, IDC_MAPROTFILE, m_pMapRotationFile);
	DDX_Control(pDX, IDC_VOTINGALLOWED, m_pVotingAllowed);
}

BOOL SVServerOptDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();
	//-------------------------------------------------
	char CompName[1024];
	DWORD CompNameSize = 1024;
	GetComputerName(CompName, &CompNameSize);
	//-------------------------------------------------	
	CompName[MAX_SERVERNAME_LEN] = 0;
	m_pHostName.SetWindowText(CompName);
	m_pHostName.LimitText(MAX_SERVERNAME_LEN);
	
	m_pPassword.SetWindowText("");
	m_pPassword.LimitText(MAX_SERVERPASSW_LEN);

	m_pMaxPlayers.SetWindowText("32");
	m_pSpectrMode.SetCheck(0);
	m_pSpectrSwitchTime.SetWindowText("20"); 
	m_pSpectrSwitchTime.EnableWindow(FALSE);
	m_pDedicated.SetCheck(0);

	m_pPublic.SetCheck(0);
	m_pCheckCDKey.SetCheck(0);
	m_pCheckCDKey.EnableWindow(FALSE);

//	m_pMapRotation.SetCheck(0);
//	m_pMapRotation.EnableWindow(TRUE);
//	m_pMapRotationFile.SetWindowText("maprot_list");
//	m_pMapRotationFile.EnableWindow(FALSE);

	m_pVotingAllowed.EnableWindow(TRUE);
	m_pVotingAllowed.SetCheck(1);

	return TRUE;  // return TRUE  unless you set the focus to a control
};
BEGIN_MESSAGE_MAP(SVServerOptDlg, CSubDlg)
	ON_BN_CLICKED(IDC_SPECTATORONLY, OnBnClickedSpectatoronly)
	ON_BN_CLICKED(IDC_PUBLIC, OnBnClickedPublic)
	ON_BN_CLICKED(IDC_MAPROTATION, OnBnClickedMapRotation)
END_MESSAGE_MAP()


// SVServerOpt message handlers

void SVServerOptDlg::OnBnClickedSpectatoronly()
{
	// TODO: Add your control notification handler code here
	if (m_pSpectrMode.GetCheck())
	{
		m_pSpectrSwitchTime.EnableWindow(TRUE);
	}
	else
	{
		m_pSpectrSwitchTime.EnableWindow(FALSE);
	}
}

void SVServerOptDlg::OnBnClickedPublic()
{
	// TODO: Add your control notification handler code here
	if (m_pPublic.GetCheck())
	{
		m_pCheckCDKey.EnableWindow(TRUE);
	}
	else
	{
		m_pCheckCDKey.EnableWindow(FALSE);
	}
}

void SVServerOptDlg::OnBnClickedMapRotation()
{
	// TODO: Add your control notification handler code here
//	if (m_pMapRotation.GetCheck())
//	{
//		m_pMapRotationFile.EnableWindow(TRUE);
//	}
//	else
//	{
//		m_pMapRotationFile.EnableWindow(FALSE);
//	};
}
