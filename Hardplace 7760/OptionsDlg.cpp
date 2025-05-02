// COptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "Hardplace 7760.h"
#include "afxdialogex.h"
#include "OptionsDlg.h"


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/, unsigned uTuneSec, bool fMonitorSWR, unsigned uPwrMax, bool fPwrConstraint)
	: CDialogEx(IDD_OPTIONS, pParent)
	, m_TuneSeconds(uTuneSec)
	, m_uTuneSec(uTuneSec)
	, m_fMonitorSWR(fMonitorSWR)
	, m_PwrAlarm(uPwrMax)
	, m_uPwrMax(uPwrMax)
	, m_fPwrConstraint(fPwrConstraint)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TUNESECSPIN, m_TuneSpin);
	DDX_Control(pDX, IDC_PWRALARMSPIN, m_PwrAlarmSpin);
	DDX_Control(pDX, IDC_PWRCONSTRAINT, m_PwrConstraint);
	DDX_Text(pDX, IDC_PWRALARM, m_PwrAlarm);
	DDX_Text(pDX, IDC_TUNESEC, m_TuneSeconds);
	DDX_Control(pDX, IDC_MONITORSWR, m_MonitorSWR);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_TuneSpin.SetRange(0, 30);
	m_PwrAlarmSpin.SetRange(0, 255);

	m_MonitorSWR.SetCheck(m_fMonitorSWR);
	m_PwrConstraint.SetCheck(m_fPwrConstraint);
	m_PwrAlarm = m_uPwrMax;
	m_TuneSeconds = m_uTuneSec;
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	m_fMonitorSWR = m_MonitorSWR.GetCheck();
	m_fPwrConstraint = m_PwrConstraint.GetCheck();
	m_uPwrMax = m_PwrAlarm;
	m_uTuneSec = m_TuneSeconds;

	CDialogEx::OnOK();
}
