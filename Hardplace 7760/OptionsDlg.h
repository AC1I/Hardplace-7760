#pragma once
#include "afxdialogex.h"


// COptionsDlg dialog

class COptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(
		CWnd* pParent = nullptr
		, unsigned uTuneSec = 5
		, bool fMonitorSWR = false
		, unsigned uPwrMax = 149
		, bool fPwrConstraint = true);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	bool MonitorSWR(void) const {
		return m_fMonitorSWR;
	}
	bool PowerConstrained(void) const {
		return m_fPwrConstraint;
	}
	unsigned PowerMax(void) const {
		return m_uPwrMax;
	}
	unsigned TunerSeconds(void) const {
		return m_uTuneSec;
	}

protected:
	virtual void OnOK();

protected:
	CSpinButtonCtrl m_TuneSpin;
	CSpinButtonCtrl m_PwrAlarmSpin;
	CButton m_PwrConstraint;
	unsigned m_PwrAlarm;
	unsigned m_TuneSeconds;
	unsigned m_uTuneSec;
	bool m_fMonitorSWR;
	unsigned m_uPwrMax;
	bool m_fPwrConstraint;
	CButton m_MonitorSWR;
};
