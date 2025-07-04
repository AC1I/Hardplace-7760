
// Hardplace 7760Dlg.h : header file
//

#pragma once

#include "SerialPort.h"
#include <afxwin.h>
#include <utility>
#include "CDataFilterDlg.h"
#include "CExtSpeakerDlg.h"


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

public:
	class CCIVDescriptor : public std::pair<const uint8_t*, size_t> {
	public:
		CCIVDescriptor(const uint8_t* pPkt = nullptr, size_t stPktLen = 0)
		: std::pair<const uint8_t*, size_t>(pPkt, stPktLen) {
		}
	};

	class CCIVArray : private CArray<CCIVDescriptor> {
	public:
		void Remove(std::pair<const uint8_t*, size_t> Msg) {
			AutoLock Lock(m_CritSection);

			for (INT nIndex(0); nIndex < CArray::GetSize(); nIndex++) {
				if (CArray::GetAt(nIndex).second == Msg.second
					&& memcmp(CArray::GetAt(nIndex).first, Msg.first, Msg.second) == 0) {
					CArray::RemoveAt(nIndex);
					break;
				}
			}
		}
		INT_PTR Add(CCIVDescriptor newElement) {
			AutoLock Lock(m_CritSection);

			return CArray::Add(newElement);
		}
		BOOL IsEmpty() {
			AutoLock Lock(m_CritSection);

			return CArray::IsEmpty();
		}
		void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1) {
			AutoLock Lock(m_CritSection);

			CArray::RemoveAt(nIndex, nCount);
		}
		void RemoveAll() {
			AutoLock Lock(m_CritSection);

			CArray::RemoveAll();
		}
		void SetAt(INT_PTR nIndex, CCIVDescriptor newElement) {
			AutoLock Lock(m_CritSection);

			CArray::SetAt(nIndex, newElement);
		}
		CCIVDescriptor& GetAt(INT_PTR nIndex) {
			AutoLock Lock(m_CritSection);

			return CArray::GetAt(nIndex);
		}
		const CCIVDescriptor& GetAt(INT_PTR nIndex) const {
			AutoLock Lock(const_cast<CCIVArray*>(this)->m_CritSection);

			return CArray::GetAt(nIndex);
		}
		INT_PTR GetCount() const {
			AutoLock Lock(const_cast<CCIVArray*>(this)->m_CritSection);

			return CArray::GetCount();
		}
		CCIVDescriptor& operator[](INT_PTR nIndex) {
			return GetAt(nIndex);
		}
		const CCIVDescriptor& operator[](INT_PTR nIndex) const {
			AutoLock Lock(const_cast<CCIVArray*>(this)->m_CritSection);

			return CArray::GetAt(nIndex);
		}
		void Rotate(void) {
			AutoLock Lock(m_CritSection);

			CArray::Add(CArray::GetAt(0));
			CArray::RemoveAt(0);
		}


	private:
		class AutoLock {
		public:
			AutoLock(CCriticalSection& CritSection)
				: m_CritSection(CritSection) {
				m_CritSection.Lock();
			}
			~AutoLock() {
				m_CritSection.Unlock();
			}
		private:
			CCriticalSection& m_CritSection;
		};

	private:
		CCriticalSection m_CritSection;
	};

	void Poll(CCIVDescriptor Pkt) {
		m_IC_7760_PollQueue.Add(Pkt);
	}
	void CeasePolling(CCIVDescriptor Pkt) {
		m_IC_7760_PollQueue.Remove(Pkt);
	}
	void Xmt(CCIVDescriptor Pkt) {
		m_IC_7760_XmtQueue.Add(Pkt);
	}

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
	const UINT_PTR m_idTimerEvent = 1;
	const UINT_PTR m_idTunerEvent = 2;
	const UINT_PTR m_idPowerEvent = 3;
	const UINT m_TimerInterval = 100;
	UINT m_TunerTimeout;
	const UINT m_TimerPower = 10000;
	bool m_TunerMonitorSWR;
	uint16_t m_uTargetSWR;
	CSerialPort m_IC_PW2_Serial;
	CSerialPort m_IC_7760_Serial;
	CArray<uint8_t> m_IC_PW2_RcvBuf;
	CArray<uint8_t> m_IC_7760_RcvBuf;
	CCIVArray m_IC_PW2_XmtQueue;
	CCIVArray m_IC_7760_XmtQueue;
	CCIVArray m_IC_PW2_PollQueue;
	CCIVArray m_IC_7760_PollQueue;
	uint64_t m_FreqInput1;
	uint64_t m_FreqInput2;
	unsigned m_uPower;
	WORD m_wPW2Power;
	uint8_t m_DataMode;
	uint8_t m_uFilterWidth;
	unsigned m_uPwrAlertThreshold;
	bool m_fEnablePwrConstraint;
	bool m_fAlertIssued;
	bool m_fPlacementSet;
	bool m_fTuning;
	bool m_fAbortTuning;
	uint8_t m_uBand;
	uint8_t m_uOperatingMode;
	uint8_t m_uDataMode;
	uint8_t m_uFilter;
	int m_iRFLevel;
	int m_Amp;
	int m_MaxPower;
	int m_PwrOn;
	int m_PW2Tuner;

	CComboBox m_IC_PW2_Port;
	CComboBox m_IC_7760_Port;
	CSliderCtrl m_PwrCtrl;
	CStatic m_Frequency;
	CDataFilterDlg m_DataFilterDlg;
	CExtSpeakerDlg m_SpeakerDlg;

	const uint8_t m_PW2_AmpSetting[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x09, 0xFD };
	const uint8_t m_PW2_PowerSetting[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x0A, 0xFD };
	const uint8_t m_PW2_PowerOut[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x15, 0x11, 0xFD };
	const uint8_t m_PW2_SWR[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x15, 0x12, 0xFD };
	const uint8_t m_PW2_Tuner[7] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1C, 0x01, 0xFD };
	uint8_t m_PW2_TunerCmd[8] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1C, 0x01, 0x00, 0xFD };
	uint8_t m_PW2_AmpCmd[8] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x09, 0x00, 0xFD };
	uint8_t m_PW2_MaxPwrCmd[8] = { 0xFE, 0xFE, 0xAA, 0xE0, 0x1A, 0x0A, 0x00, 0xFD };

	const uint8_t m_IC7760_RFLevel[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x14, 0x0A, 0xFD };
	const uint8_t m_IC7760DataFilter[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1A, 0x06, 0xFD };
	const uint8_t m_IC7760SplitSetting[6] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x0F, 0xFD };
	const uint8_t m_IC7760SWR[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x15, 0x12, 0xFD };
	const uint8_t m_IC7760_Tuner[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1C, 0x01, 0xFD };
	uint8_t m_IC7760LastCommand[32];
	uint8_t m_IC7760SetPower[9] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x14, 0x0A, 0x02, 0x55, 0xFD };
	uint8_t m_IC7760OperatingMode[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x26, 0x00, 0xFD };
	uint8_t m_IC7760OperatingModeCmd[10] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x26, 0x00, 0x00, 0x00, 0x00, 0xFD };
	uint8_t m_IC7760Transmit[8] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1C, 0x00, 0x00, 0xFD };
	uint8_t m_IC7760RFLevelCmd[9] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x14, 0x0A, 0x00, 0x64, 0xFD };
	uint8_t m_IC7760PwrCmd[7] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x18, 0x01, 0xFD };
	uint8_t m_IC7760_TunerCmd[8] = { 0xFE, 0xFE, 0xB2, 0xE0, 0x1C, 0x01, 0x00, 0xFD };

	uint16_t m_PowerMap[256];

protected:
	afx_msg void OnClose();
	afx_msg void OnPW2ComOpen();
	afx_msg void On7760ComOpen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnClickedTuner(UINT nId);
	afx_msg void OnClickedAmp(UINT nId);
	afx_msg void OnClickedMaxPower(UINT nId);
	afx_msg void OnClickedRFLevel(UINT nId);
	afx_msg void OnClickedPower(UINT nId);
	afx_msg void OnClickedTune();

	bool OpenCommPort(int iPort, CSerialPort& Port, bool fQuiet = false);
	void onSerialException(CSerialException& ex, CSerialPort& Port);
	DWORD inQueue(HANDLE hPort);
	DWORD outQueue(HANDLE hPort);
	void onIC_PW2Packet();
	void onIC_7760Packet();

	uint8_t bcd2uint8_t(uint8_t uchBCD_Digit)
	{
		return (uchBCD_Digit & 0x0f) + ((uchBCD_Digit >> 4) * 10);
	}
	uint16_t bcd2uint16_t(uint8_t uchBCD_DigitLow, uint8_t uchBCD_DigitHi)
	{
		return uint16_t(bcd2uint8_t(uchBCD_DigitLow))
			+ (100 * uint16_t(bcd2uint8_t(uchBCD_DigitHi)));
	}

	void TracePacket(const LPTSTR lpPrefix, CArray<uint8_t> &rPacket)
	{
		CString Debug(lpPrefix);

		for (int nIndex(0); nIndex < rPacket.GetCount(); nIndex++)
		{
			CString szOctet;
			szOctet.Format(_T("0x%02X "), rPacket[nIndex]);
			Debug += szOctet;
		}
		Debug.TrimRight();
		Debug += _T("\n");
		TRACE(Debug);
	}
};