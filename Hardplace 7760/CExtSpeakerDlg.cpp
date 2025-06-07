// CExtSpeakerDlg.cpp : implementation file
//

#include "pch.h"
#include "Hardplace 7760.h"
#include "afxdialogex.h"
#include "CExtSpeakerDlg.h"
#include "Hardplace 7760Dlg.h"

// CExtSpeakerDlg dialog

IMPLEMENT_DYNAMIC(CExtSpeakerDlg, CDialogEx)

CExtSpeakerDlg::CExtSpeakerDlg(CHardplace7760Dlg* pParent /*=nullptr*/)
	: CDialogEx(IDD_LINEOUT, pParent)
	, m_pParent(pParent)
	, m_LineOut(-1)
{

}

CExtSpeakerDlg::~CExtSpeakerDlg()
{
}

void CExtSpeakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_LINEOUT_MAIN, m_LineOut);
}

BOOL CExtSpeakerDlg::Create(CWnd* pParent)
{
	if (m_hWnd)
	{
		CHardplace7760Dlg* pDlgParent(m_pParent);

		this->~CExtSpeakerDlg();
		new (this) CExtSpeakerDlg(pDlgParent);
	}

	BOOL fReturnVal(CDialogEx::Create(IDD_LINEOUT, pParent));

	if (fReturnVal)
	{
		ShowWindow(SW_SHOW);
		m_pParent->m_IC_7760_PollQueue.Add(std::make_pair(m_IC7760AFOutputPoll, sizeof m_IC7760AFOutputPoll));
	}

	return fReturnVal;
}

BEGIN_MESSAGE_MAP(CExtSpeakerDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LINEOUT_MAIN, IDC_LINEOUT_SUB, &CExtSpeakerDlg::OnClickedReceiver)
END_MESSAGE_MAP()


// CExtSpeakerDlg message handlers

void CExtSpeakerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_pParent->m_IC_7760_PollQueue.Remove(std::make_pair(m_IC7760AFOutputPoll, sizeof m_IC7760AFOutputPoll));

	CDialogEx::OnClose();
}

void CExtSpeakerDlg::OnClickedReceiver(UINT nId)
{
	UpdateData();
	m_IC7760AFOutputCmd[8] = m_LineOut;
	m_LineOut = -1;
	UpdateData(FALSE);
	m_pParent->m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760AFOutputCmd, sizeof m_IC7760AFOutputCmd));
}