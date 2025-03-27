
// Hardplace 7760Dlg.h : header file
//

#pragma once

#include "SerialPort.h"
#include <afxwin.h>
#include <utility>


// CHardplace7760Dlg dialog
class CHardplace7760Dlg : public CDialogEx
{
	// Construction
public:
	CHardplace7760Dlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HARDPLACE_7760_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	const UINT_PTR m_idTimerEvent = 1;
	const UINT m_TimerInterval = 250;
	CSerialPort m_IC_PW2_Serial;
	CSerialPort m_IC_7760_Serial;
	CArray<uint8_t> m_IC_PW2_RcvBuf;
	CArray<uint8_t> m_IC_7760_RcvBuf;
	CArray<std::pair<const uint8_t*, size_t> > m_IC_PW2_XmtQueue;
	CArray<std::pair<const uint8_t*, size_t> > m_IC_7760_XmtQueue;
	CArray<std::pair<const uint8_t*, size_t> > m_IC_PW2_PollQueue;
	CArray<std::pair<const uint8_t*, size_t> > m_IC_7760_PollQueue;
	uint64_t m_FreqInput1;
	uint64_t m_FreqInput2;
	unsigned m_uPower;
	const unsigned m_uPwrAlertThreshold;
	bool m_fPlacementSet;
	CComboBox m_IC_PW2_Port;
	CComboBox m_IC_7760_Port;
	const uint8_t m_PW2_AmpSetting[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x09, 0xFD };
	const uint8_t m_PW2_PowerSetting[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x0A, 0xFD };
	const uint8_t m_PW2_PowerOut[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x15, 0x11, 0xFD };
	const uint8_t m_IC7760_RFLevel[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x14, 0x0A, 0xFD };
	CSliderCtrl m_PwrCtrl;
	CStatic m_Frequency;

public:
	afx_msg void OnClose();
	afx_msg void OnPW2ComOpen();
	afx_msg void On7760ComOpen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	afx_msg void OnClickedAmp(UINT nId);
	afx_msg void OnClickedMaxPower(UINT nId);
	afx_msg void OnClickedRFLevel(UINT nId);
	afx_msg void OnClickedPower(UINT nId);

	bool OpenCommPort(int iPort, CSerialPort& Port, bool fQuiet = false);
	void onSerialException(CSerialException& ex, CSerialPort& Port);
	void onIC_PW2Packet();
	void onIC_7760Packet();

	uint8_t bcd2uint8_t(uint8_t uchBCD_Digit)
	{
		return (uchBCD_Digit & 0x0f) + ((uchBCD_Digit >> 4) * 10);
	}
	int m_iRFLevel;
	int m_Amp;
	int m_MaxPower;
	int m_PwrOn;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
};
