// CDataFilterDlg.cpp : implementation file
//

#include "pch.h"
#include "Hardplace7760.h"
#include "afxdialogex.h"
#include "CDataFilterDlg.h"
#include "Hardplace7760Dlg.h"


// CDataFilterDlg dialog

IMPLEMENT_DYNAMIC(CDataFilterDlg, CDialogEx)

CDataFilterDlg::CDataFilterDlg(CHardplace7760Dlg* pParent)
	: CDialogEx(IDD_DATAFILTER, pParent)
	, m_pParent(pParent)
	, m_iReceiver(0)
	, m_iDataMode(-1)
	, m_iFilterMode(-1)
	, m_OperatingMode(0)
{
}

CDataFilterDlg::~CDataFilterDlg()
{
}

BOOL CDataFilterDlg::Create(CWnd* pParent)
{
	if (m_hWnd)
	{
		CHardplace7760Dlg* pDlgParent(m_pParent);

		this->~CDataFilterDlg();
		new (this) CDataFilterDlg(pDlgParent);
	}

	BOOL fReturnVal(CDialogEx::Create(IDD_DATAFILTER, pParent));

	if (fReturnVal)
	{
		ShowWindow(SW_SHOW);
		m_pParent->Poll(CHardplace7760Dlg::CCIVDescriptor(m_IC7760_ReadDataFilterCmd, sizeof m_IC7760_ReadDataFilterCmd));
	}

	return fReturnVal;
}

void CDataFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_DATAOFF, m_iDataMode);
	DDX_Radio(pDX, IDC_FILTER1, m_iFilterMode);
	DDX_Radio(pDX, IDC_RCVRMAIN, m_iReceiver);
}


BEGIN_MESSAGE_MAP(CDataFilterDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RCVRMAIN, IDC_RCVRSUB, &CDataFilterDlg::OnClickedReceiver)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DATAOFF, IDC_DATA3, &CDataFilterDlg::OnClickedDataFilterMode)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_FILTER1, IDC_FILTER3, &CDataFilterDlg::OnClickedDataFilterMode)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDataFilterDlg message handlers
void CDataFilterDlg::OnClickedReceiver(UINT nId)
{
	UpdateData();
	m_IC7760_ReadDataFilterCmd[5] = m_iReceiver;
	m_iDataMode = m_iFilterMode = -1;
	UpdateData(FALSE);
}

void CDataFilterDlg::OnClickedDataFilterMode(UINT nId)
{
	UpdateData();
	m_IC7760_DataFilterCmd[5] = m_iReceiver;
	m_IC7760_DataFilterCmd[6] = m_OperatingMode;
	m_IC7760_DataFilterCmd[7] = m_iDataMode;
	m_IC7760_DataFilterCmd[8] = m_iFilterMode + 1;
	m_pParent->Xmt(CHardplace7760Dlg::CCIVDescriptor(m_IC7760_DataFilterCmd, sizeof m_IC7760_DataFilterCmd));
}

void CDataFilterDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_pParent->CeasePolling(CHardplace7760Dlg::CCIVDescriptor(m_IC7760_ReadDataFilterCmd, sizeof m_IC7760_ReadDataFilterCmd));

	CDialogEx::OnClose();
}
