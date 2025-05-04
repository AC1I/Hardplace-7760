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
		, uint16_t uTargetSWR = 40
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
	uint16_t TargetSWR(void) const {
		return m_uTargetSWR;
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
	uint16_t m_uTargetSWR;
	unsigned m_uPwrMax;
	bool m_fPwrConstraint;
	CButton m_MonitorSWR;
public:
	afx_msg void OnDeltaposTargetSwrSpin(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	void UpdateSWREdit(uint16_t uSWR);
};
