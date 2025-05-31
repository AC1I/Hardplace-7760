#pragma once
#include "afxdialogex.h"
#include <utility>

class CHardplace7760Dlg;

// CDataFilterDlg dialog

class CDataFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataFilterDlg)

public:
	CDataFilterDlg(CHardplace7760Dlg* pParent);   // standard constructor
	virtual ~CDataFilterDlg();

public:
	virtual BOOL Create(CWnd* pParent = nullptr);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATAFILTER };
#endif

public:
	void UpdateDialog(uint8_t uchReceiver, uint8_t uchOperatingMode, uint8_t uchDataMode, uint8_t uchFilter) {
		if (uchReceiver == m_iReceiver)
		{
			m_OperatingMode = uchOperatingMode;
			m_iDataMode = int(uchDataMode);
			m_iFilterMode = int(uchFilter) - 1;
			if (IsWindowVisible())
			{
				UpdateData(FALSE);
			}
		}
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnClickedReceiver(UINT nId);
	afx_msg void OnClickedDataFilterMode(UINT nId);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

	uint8_t m_IC7760_ReadDataFilterCmd[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x26, 0x00, 0xFD };
	uint8_t m_IC7760_DataFilterCmd[10] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x26, 0x00, 0x00, 0x00, 0x00, 0xFD };
	CHardplace7760Dlg* m_pParent;
	int m_iReceiver;
	int m_iDataMode;
	int m_iFilterMode;
	uint8_t m_OperatingMode;
};
