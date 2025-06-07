// COptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "Hardplace7760.h"
#include "afxdialogex.h"
#include "OptionsDlg.h"


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/, unsigned uTuneSec, bool fMonitorSWR, uint16_t uTargetSWR, unsigned uPwrMax, bool fPwrConstraint)
	: CDialogEx(IDD_OPTIONS, pParent)
	, m_TuneSeconds(uTuneSec)
	, m_uTuneSec(uTuneSec)
	, m_fMonitorSWR(fMonitorSWR)
	, m_uTargetSWR(uTargetSWR)
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
	ON_NOTIFY(UDN_DELTAPOS, IDC_TARGET_SWR_SPIN, &COptionsDlg::OnDeltaposTargetSwrSpin)
END_MESSAGE_MAP()


// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_TuneSpin.SetRange(0, 30);
	m_PwrAlarmSpin.SetRange(0, 255);

	m_MonitorSWR.SetCheck(m_fMonitorSWR);
	UpdateSWREdit(m_uTargetSWR);
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

void COptionsDlg::OnDeltaposTargetSwrSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	const uint16_t uStep((m_uTargetSWR >= 80) ? 4 : 8);

	if (pNMUpDown->iDelta > 0) // Down
	{
		m_uTargetSWR -= (m_uTargetSWR >= uStep) ? uStep : m_uTargetSWR;
	}
	else if (pNMUpDown->iDelta < 0) // Up
	{
		m_uTargetSWR += uStep;
	}
	if (m_uTargetSWR > 120)
	{
		m_uTargetSWR = 120;
	}
	UpdateSWREdit(m_uTargetSWR);
	*pResult = 0;
}

void COptionsDlg::UpdateSWREdit(uint16_t uSWR)
{
	/* Read SWR meter level
	(0000 = SWR1.0, 0040 = SWR1.5,
		0080 = SWR2.0, 0120 = SWR3.0) */
	CString sText;
	if (uSWR >= 120)
	{
		sText = _T("3.");
	}
	else if (uSWR >= 80)
	{
		sText = _T("2.");
	}
	else
	{
		sText = _T("1.");
	}
	if (uSWR == 0
		|| uSWR == 80
		|| uSWR >= 120)
	{
		sText += _T("0");
	}
	else if (uSWR < 80)
	{
		CString sRemainder;

		sRemainder.Format(_T("%01u"), unsigned(uSWR * 0.125));
		sText += sRemainder;
	}
	else if (uSWR < 120)
	{
		CString sRemainder;

		uSWR -= 80;
		sRemainder.Format(_T("%01u"), unsigned(uSWR * 0.25));
		sText += sRemainder;
	}
	SetDlgItemText(IDC_TARGET_SWR_EDIT, sText);
	// TODO: Add your implementation code here.
}
