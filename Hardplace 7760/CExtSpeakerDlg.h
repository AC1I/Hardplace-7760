#pragma once
#include "afxdialogex.h"


// CExtSpeakerDlg dialog

class CHardplace7760Dlg;

class CExtSpeakerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExtSpeakerDlg)

public:
	CExtSpeakerDlg(CHardplace7760Dlg* pParent = nullptr);   // standard constructor
	virtual ~CExtSpeakerDlg();

public:
	virtual BOOL Create(CWnd* pParent = nullptr);

public:
	void UpdateDialog(uint8_t uchLineOut) {
		m_LineOut = uchLineOut;
		if (IsWindowVisible())
		{
			UpdateData(FALSE);
		}
	}

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LINEOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClose();
	afx_msg void OnClickedReceiver(UINT nId);

protected:
	CHardplace7760Dlg* m_pParent;
	const uint8_t m_IC7760AFOutputPoll[9] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1A, 0x05, 0x01, 0x09, 0xFD };
	uint8_t m_IC7760AFOutputCmd[10] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1A, 0x05, 0x01, 0x09, 0x00, 0xFD };
	int m_LineOut;
};
