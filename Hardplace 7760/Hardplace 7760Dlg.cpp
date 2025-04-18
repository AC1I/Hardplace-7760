
// Hardplace 7760Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Hardplace 7760.h"
#include "Hardplace 7760Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHardplace7760Dlg dialog



CHardplace7760Dlg::CHardplace7760Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HARDPLACE_7760_DIALOG, pParent)
	, m_FreqInput1(0), m_FreqInput2(0)
	, m_iRFLevel(-1), m_uPower(0), m_DataMode(0), m_uFilterWidth(0)
	, m_TunerTimeout(theApp.GetProfileInt(_T("Settings"), _T("TunerTimeout"), 1000 * 5))
	, m_TunerMonitorSWR(theApp.GetProfileInt(_T("Settings"), _T("TunerMonitorSWR"), false))
	, m_uPwrAlertThreshold(theApp.GetProfileInt(_T("Settings"), _T("PowerAlarmThreshold"), 0xFFFF))
	, m_fAlertIssued(false), m_fPlacementSet(false), m_fTuning(false)
	, m_Amp(-1)
	, m_MaxPower(-1)
	, m_PwrOn(-1)
{
	memset(m_IC7760LastCommand, '\0', sizeof m_IC7760LastCommand);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_IC_PW2_PollQueue.Add(std::make_pair(m_PW2_AmpSetting, sizeof m_PW2_AmpSetting));
	m_IC_PW2_PollQueue.Add(std::make_pair(m_PW2_PowerSetting, sizeof m_PW2_PowerSetting));
	m_IC_PW2_PollQueue.Add(std::make_pair(m_PW2_PowerOut, sizeof m_PW2_PowerOut));
	m_IC_7760_PollQueue.Add(std::make_pair(m_IC7760_RFLevel, sizeof m_IC7760_RFLevel));
	m_IC_7760_PollQueue.Add(std::make_pair(m_IC7760DataFilter, sizeof m_IC7760DataFilter));
}

void CHardplace7760Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PW2_Port, m_IC_PW2_Port);
	DDX_Control(pDX, IDC_7760_PORT, m_IC_7760_Port);
	DDX_Control(pDX, IDC_POWER, m_PwrCtrl);
	DDX_Control(pDX, IDC_FREQUENCY, m_Frequency);
	DDX_Radio(pDX, IDC_50W, m_iRFLevel);
	DDX_Radio(pDX, IDC_AMP_OFF, m_Amp);
	DDX_Radio(pDX, IDC_AMP_500W, m_MaxPower);
	DDX_Radio(pDX, IDC_ON, m_PwrOn);
}

BEGIN_MESSAGE_MAP(CHardplace7760Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PW2_COM_OPEN, &CHardplace7760Dlg::OnPW2ComOpen)
	ON_BN_CLICKED(IDC_7760_COM_OPEN, &CHardplace7760Dlg::On7760ComOpen)
	ON_WM_TIMER()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AMP_OFF, IDC_AMP_ON, &CHardplace7760Dlg::OnClickedAmp)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AMP_500W, IDC_AMP_1KW, &CHardplace7760Dlg::OnClickedMaxPower)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_50W, IDC_200W, &CHardplace7760Dlg::OnClickedRFLevel)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ON, IDC_OFF, &CHardplace7760Dlg::OnClickedPower)
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TUNE, &CHardplace7760Dlg::OnClickedTune)
END_MESSAGE_MAP()


// CHardplace7760Dlg message handlers

BOOL CHardplace7760Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetDlgItemText(IDC_FREQUENCY, _T(""));
	SetDlgItemText(IDC_POWERALARM, _T(""));

	m_PwrCtrl.SetRange(0, 255);
	m_PwrCtrl.SetPageSize(m_PwrCtrl.GetRangeMax() / 10);
	m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
	m_PwrCtrl.SetTicFreq(25);

	ULONG uPortsFound(0);
	if (GetCommPorts(0, 0, &uPortsFound) == ERROR_MORE_DATA)
	{
#pragma warning(push)
#pragma warning(disable : 6001 )
		CAutoPtr<ULONG> pPortBuf(new ULONG[uPortsFound]);
#pragma warning(pop)

		if (GetCommPorts(pPortBuf, uPortsFound, &uPortsFound) == ERROR_SUCCESS)
		{
			for (unsigned nIndex(0); nIndex < uPortsFound; nIndex++)
			{
				CString comPort;

				comPort.Format(_T("COM%u"), pPortBuf[nIndex]);
				if (m_IC_PW2_Port.FindStringExact(-1, comPort) == CB_ERR)
				{
					int iIndex(m_IC_PW2_Port.AddString(comPort));
					if (iIndex >= 0)
					{
						m_IC_PW2_Port.SetItemData(iIndex, pPortBuf[nIndex]);
					}
				}
				if (m_IC_7760_Port.FindStringExact(-1, comPort) == CB_ERR)
				{
					int iIndex(m_IC_7760_Port.AddString(comPort));
					if (iIndex >= 0)
					{
						m_IC_7760_Port.SetItemData(iIndex, pPortBuf[nIndex]);
					}
				}
			}

			UINT uDefaultPort(theApp.GetProfileInt(_T("Settings"), _T("IC_PW2_Port"), 0));
			CString findString;

			findString.Format(_T("COM%u"), uDefaultPort);
			int nIndex(m_IC_PW2_Port.FindStringExact(-1, findString));

			if (nIndex != CB_ERR)
			{
				m_IC_PW2_Port.SetCurSel(nIndex);
				OpenCommPort(uDefaultPort, m_IC_PW2_Serial, true);
			}
			else
			{
				m_IC_PW2_Port.SelectString(-1, _T("COM"));
			}

			uDefaultPort = theApp.GetProfileInt(_T("Settings"), _T("IC_7760_Port"), 0);

			findString.Format(_T("COM%u"), uDefaultPort);
			nIndex = m_IC_7760_Port.FindStringExact(-1, findString);

			if (nIndex != CB_ERR)
			{
				m_IC_7760_Port.SetCurSel(nIndex);
				OpenCommPort(uDefaultPort, m_IC_7760_Serial, true);
			}
			else
			{
				m_IC_7760_Port.SelectString(-1, _T("COM"));
			}
		}
		else
		{
			AfxMessageBox(IDS_NOTFOUND);
		}
	}
	SetTimer(m_idTimerEvent, m_TimerInterval, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHardplace7760Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHardplace7760Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHardplace7760Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHardplace7760Dlg::OnClose()
{
	KillTimer(m_idTimerEvent);
	try
	{
		if (m_IC_PW2_Serial.IsOpen())
		{
			m_IC_PW2_Serial.Flush();
			m_IC_PW2_Serial.Close();
		}
	}
	catch (CSerialException ex)
	{

	}
	try
	{
		if (m_IC_7760_Serial.IsOpen())
		{
			m_IC_7760_Serial.Flush();
			m_IC_7760_Serial.Close();
		}
	}
	catch (CSerialException ex)
	{

	}

	CDialogEx::OnClose();
}

void CHardplace7760Dlg::OnPW2ComOpen()
{
	try
	{
		if (m_IC_PW2_Serial.IsOpen())
		{
			m_IC_PW2_Serial.Flush();
			m_IC_PW2_Serial.Close();
			SetDlgItemText(IDC_PW2_COM_OPEN, _T("Open"));
			m_MaxPower = -1;
			m_Amp = -1;
			UpdateData(FALSE);
		}
		else
		{
			OpenCommPort(int(int(m_IC_PW2_Port.GetItemData(m_IC_PW2_Port.GetCurSel()))), m_IC_PW2_Serial);
			theApp.WriteProfileInt(_T("Settings"), _T("IC_PW2_Port"), int(m_IC_PW2_Port.GetItemData(m_IC_PW2_Port.GetCurSel())));
		}
	}
	catch (CSerialException ex)
	{
	}
}

void CHardplace7760Dlg::On7760ComOpen()
{
	try
	{
		if (m_IC_7760_Serial.IsOpen())
		{
			m_IC_7760_Serial.Flush();
			m_IC_7760_Serial.Close();
			SetDlgItemText(IDC_7760_COM_OPEN, _T("Open"));
			SetDlgItemText(IDC_RFLEVEL, _T("RF Level:"));
			m_iRFLevel = -1;
			m_uPower = 0;
			m_PwrOn = -1;
			m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
			UpdateData(FALSE);
		}
		else
		{
			OpenCommPort(int(m_IC_7760_Port.GetItemData(m_IC_7760_Port.GetCurSel())), m_IC_7760_Serial);
			theApp.WriteProfileInt(_T("Settings"), _T("IC_7760_Port"), int(m_IC_7760_Port.GetItemData(m_IC_7760_Port.GetCurSel())));
		}
	}
	catch (CSerialException ex)
	{

	}
}

bool CHardplace7760Dlg::OpenCommPort(int iPort, CSerialPort& Port, bool fQuiet)
{
	if (Port == m_IC_7760_Serial)
	{
		m_fTuning = false;
	}
	try
	{
		if (!Port.IsOpen())
		{
			CWaitCursor wait;

			Port.Open(iPort, DWORD(19200));
			if (Port.IsOpen())
			{
				COMMTIMEOUTS timeOuts;

				Port.GetTimeouts(timeOuts);
				timeOuts.ReadIntervalTimeout = 250;
				timeOuts.ReadTotalTimeoutConstant = 250;
				timeOuts.WriteTotalTimeoutConstant = 10000;
				Port.SetTimeouts(timeOuts);

				if (Port == m_IC_PW2_Serial) {
					SetDlgItemText(IDC_PW2_COM_OPEN, _T("Close"));
				}
				else if (Port == m_IC_7760_Serial) {
					SetDlgItemText(IDC_7760_COM_OPEN, _T("Close"));
				}
			}
			else if (!fQuiet)
			{
				AfxMessageBox(IDS_PORTBUSY);
			}
		}
	}
	catch (CSerialException ex) {
		if (!fQuiet)
		{
			onSerialException(ex, Port);
		}
	}

	return Port.IsOpen();
}

void CHardplace7760Dlg::onSerialException(CSerialException& ex, CSerialPort& Port)
{
	TCHAR achErrorStr[80];

	ex.GetErrorMessage2(achErrorStr, sizeof achErrorStr / sizeof(TCHAR));
	try
	{
		if (Port.IsOpen())
		{
			Port.Close();
		}
		if (Port == m_IC_7760_Serial)
		{
			m_fTuning = false;
		}
	}
	catch (CSerialException ex)
	{
	}
	AfxMessageBox(achErrorStr);
}

DWORD CHardplace7760Dlg::OctetsBuffered(HANDLE hPort)
{
	DWORD dwAvailable(0);
	DWORD dwErrors;
	COMSTAT Status;

	if (ClearCommError(hPort, &dwErrors, &Status))
	{
		dwAvailable = Status.cbInQue;
	}
	return dwAvailable;
}

void CHardplace7760Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_idTimerEvent)
	{
		if (m_IC_PW2_Serial.IsOpen())
		{
			DWORD dwAvailable(OctetsBuffered(HANDLE(m_IC_PW2_Serial)));
			
			if (dwAvailable)
			{
				for (uint8_t uchByte(0); dwAvailable; dwAvailable--)
				{
					try
					{
						m_IC_PW2_Serial.Read(&uchByte, 1);
						m_IC_PW2_RcvBuf.Add(uchByte);
						if (uchByte == 0xFD)
						{
							onIC_PW2Packet();
						}
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_PW2_Serial);
						m_IC_PW2_RcvBuf.RemoveAll();
						m_IC_PW2_XmtQueue.RemoveAll();
						break;
					}
				}
			}
			if (m_IC_PW2_RcvBuf.IsEmpty())
			{
				// transmit here
				if (!m_IC_PW2_XmtQueue.IsEmpty())
				{
					try
					{
						m_IC_PW2_Serial.Write(m_IC_PW2_XmtQueue[0].first, DWORD(m_IC_PW2_XmtQueue[0].second));
						m_IC_PW2_XmtQueue.RemoveAt(0);
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_PW2_Serial);
						m_IC_PW2_RcvBuf.RemoveAll();
						m_IC_PW2_XmtQueue.RemoveAll();
					}
				}
				else if (!m_IC_PW2_PollQueue.IsEmpty())
				{
					try
					{
						m_IC_PW2_Serial.Write(m_IC_PW2_PollQueue[0].first, DWORD(m_IC_PW2_PollQueue[0].second));
						m_IC_PW2_PollQueue.Add(std::make_pair(m_IC_PW2_PollQueue[0].first, m_IC_PW2_PollQueue[0].second));
						m_IC_PW2_PollQueue.RemoveAt(0);
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_PW2_Serial);
						m_IC_PW2_RcvBuf.RemoveAll();
						m_IC_PW2_XmtQueue.RemoveAll();
					}
				}
			}
		}
		if (m_IC_7760_Serial.IsOpen())
		{
			DWORD dwAvailable(OctetsBuffered(HANDLE(m_IC_7760_Serial)));

			if (dwAvailable)
			{
				for (uint8_t uchByte(0); dwAvailable; dwAvailable--)
				{
					try
					{
						m_IC_7760_Serial.Read(&uchByte, 1);
						m_IC_7760_RcvBuf.Add(uchByte);
						if (uchByte == 0xFD)
						{
							onIC_7760Packet();
						}
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_7760_Serial);
						m_IC_7760_RcvBuf.RemoveAll();
						break;
					}
				}
			}
			if (m_IC_7760_RcvBuf.IsEmpty())
			{
				// Transmit here
				if (!m_IC_7760_XmtQueue.IsEmpty())
				{
					memcpy(m_IC7760LastCommand, m_IC_7760_XmtQueue[0].first, m_IC_7760_XmtQueue[0].second);
					try
					{
						m_IC_7760_Serial.Write(m_IC_7760_XmtQueue[0].first, DWORD(m_IC_7760_XmtQueue[0].second));
						m_IC_7760_XmtQueue.RemoveAt(0);
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_7760_Serial);
						m_IC_7760_RcvBuf.RemoveAll();
						m_IC_7760_XmtQueue.RemoveAll();
					}
				}
				else if (!m_IC_7760_PollQueue.IsEmpty())
				{
					memcpy(m_IC7760LastCommand, m_IC_7760_PollQueue[0].first, m_IC_7760_PollQueue[0].second);
					try
					{
						m_IC_7760_Serial.Write(m_IC_7760_PollQueue[0].first, DWORD(m_IC_7760_PollQueue[0].second));
						m_IC_7760_PollQueue.Add(std::make_pair(m_IC_7760_PollQueue[0].first, m_IC_7760_PollQueue[0].second));
						m_IC_7760_PollQueue.RemoveAt(0);
					}
					catch (CSerialException ex) {
						onSerialException(ex, m_IC_7760_Serial);
						m_IC_7760_RcvBuf.RemoveAll();
						m_IC_7760_PollQueue.RemoveAll();
					}
				}
			}
		}
	}
	else if (nIDEvent == m_idTunerEvent)
	{
		m_fTuning = false;

		m_IC7760Transmit[6] = 0x00;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760Transmit, sizeof m_IC7760Transmit));

		m_IC7760OperatingModeCmd[5] = m_uBand;
		m_IC7760OperatingModeCmd[6] = m_uOperatingMode;
		m_IC7760OperatingModeCmd[7] = m_uDataMode;
		m_IC7760OperatingModeCmd[8] = m_uFilter;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760OperatingModeCmd, sizeof m_IC7760OperatingModeCmd));

		KillTimer(m_idTunerEvent);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CHardplace7760Dlg::OnClickedAmp(UINT nId)
{
	UpdateData();

	switch (m_Amp)
	{
	case 0:
		m_PW2_AmpCmd[6] = 0x00;
		m_IC_PW2_XmtQueue.Add(std::make_pair(m_PW2_AmpCmd, sizeof m_PW2_AmpCmd));
	break;

	case 1:
		m_PW2_AmpCmd[6] = 0x01;
		m_IC_PW2_XmtQueue.Add(std::make_pair(m_PW2_AmpCmd, sizeof m_PW2_AmpCmd));
	break;

	default:
		break;
	}
}

void CHardplace7760Dlg::OnClickedMaxPower(UINT nId)
{
	UpdateData();

	switch (m_MaxPower)
	{
	case 0:
		m_PW2_MaxPwrCmd[6] = 0x00;
		m_IC_PW2_XmtQueue.Add(std::make_pair(m_PW2_MaxPwrCmd, sizeof m_PW2_MaxPwrCmd));
	break;

	case 1:
		m_PW2_MaxPwrCmd[6] = 0x01;
		m_IC_PW2_XmtQueue.Add(std::make_pair(m_PW2_MaxPwrCmd, sizeof m_PW2_MaxPwrCmd));
	break;

	default:
		break;
	}
}

void CHardplace7760Dlg::OnClickedRFLevel(UINT nId)
{
	UpdateData();

	switch (m_iRFLevel)
	{
	case 0:
		m_IC7760RFLevelCmd[6] = 0x00;
		m_IC7760RFLevelCmd[7] = 0x64;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760RFLevelCmd, sizeof m_IC7760RFLevelCmd));
	break;

	case 1:
		m_IC7760RFLevelCmd[6] = 0x01;
		m_IC7760RFLevelCmd[7] = 0x28;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760RFLevelCmd, sizeof m_IC7760RFLevelCmd));
	break;

	case 2:
		m_IC7760RFLevelCmd[6] = 0x01;
		m_IC7760RFLevelCmd[7] = 0x92;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760RFLevelCmd, sizeof m_IC7760RFLevelCmd));
	break;

	case 3:
		m_IC7760RFLevelCmd[6] = 0x02;
		m_IC7760RFLevelCmd[7] = 0x55;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760RFLevelCmd, sizeof m_IC7760RFLevelCmd));
	break;

	default:
		break;
	}
	m_iRFLevel = -1;
	UpdateData(FALSE);
}

void CHardplace7760Dlg::OnClickedPower(UINT nId)
{
	UpdateData();

	switch (m_PwrOn)
	{
	case 0:
		m_IC7760PwrCmd[5] = 0x01;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760PwrCmd, sizeof m_IC7760PwrCmd));
	break;

	case 1:
		m_IC7760PwrCmd[5] = 0x00;
		m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760PwrCmd, sizeof m_IC7760PwrCmd));
	break;

	default:
		break;
	}
}

void CHardplace7760Dlg::OnClickedTune()
{
	m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760SplitSetting, sizeof m_IC7760SplitSetting));
}

void CHardplace7760Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == (CScrollBar*)(&m_PwrCtrl))
	{
		switch (nSBCode)
		{
		default:
			break;

		case TB_ENDTRACK:
		case TB_THUMBPOSITION:
		{
			int iLevel(abs(m_PwrCtrl.GetPos() - m_PwrCtrl.GetRangeMax()));
			unsigned uLevelLow(iLevel % 100);
			unsigned uLevelHigh(iLevel / 100);

			m_IC7760SetPower[6] = ((uLevelHigh / 10) << 4) + uLevelHigh % 10;
			m_IC7760SetPower[7] = ((uLevelLow / 10) << 4) + uLevelLow % 10;
			m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760SetPower, sizeof m_IC7760SetPower));
		}
		break;
		}
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CHardplace7760Dlg::onIC_PW2Packet()
{
	while (m_IC_PW2_RcvBuf.GetCount() > 5)
	{
		if (m_IC_PW2_RcvBuf[0] == 0xFE
			&& m_IC_PW2_RcvBuf[1] == 0xFE)
		{
			break;
		}
		m_IC_PW2_RcvBuf.RemoveAt(0);
	}
	if (m_IC_PW2_RcvBuf.GetCount() > 5
		&& m_IC_PW2_RcvBuf[3] == 0xAA)
	{
		CString textVal;
		LPCTSTR szFreqFmt(_T("%llu.%03llu.%02llu\t%llu.%03llu.%02llu"));

		switch (m_IC_PW2_RcvBuf[4])
		{
		case 0:
			if (m_IC_PW2_RcvBuf.GetCount() == 11)
			{
				m_FreqInput1 = bcd2uint8_t(m_IC_PW2_RcvBuf[5]);   // Hz
				m_FreqInput1 += 100 * bcd2uint8_t(m_IC_PW2_RcvBuf[6]);   // 100Hz
				m_FreqInput1 += 10000 * bcd2uint8_t(m_IC_PW2_RcvBuf[7]);   // 10 kHz
				m_FreqInput1 += 1000000 * bcd2uint8_t(m_IC_PW2_RcvBuf[8]);   // 1 MHz
				m_FreqInput1 += 100000000 * bcd2uint8_t(m_IC_PW2_RcvBuf[9]);   // 100 MHz

				textVal.Format(szFreqFmt,
					m_FreqInput1 / 1000000, (m_FreqInput1 % 1000000) / 1000, (m_FreqInput1 % 1000) / 10,
					m_FreqInput2 / 1000000, (m_FreqInput2 % 1000000) / 1000, (m_FreqInput2 % 1000) / 10);
				SetDlgItemText(IDC_FREQUENCY, textVal);
			}
			break;

		case 0x15:
			switch (m_IC_PW2_RcvBuf[5])
			{
			case 0x11:
				if (m_IC_PW2_RcvBuf.GetCount() == 9
					&& m_DataMode != 0
					&& unsigned(MAKEWORD(m_IC_PW2_RcvBuf[7], m_IC_PW2_RcvBuf[6])) >= m_uPwrAlertThreshold)
				{
					//CString szDebug;
					//for (INT_PTR stIndex(0); stIndex < m_IC_PW2_RcvBuf.GetCount(); stIndex++)
					//{
					//	CString szOctet;
					//	szOctet.Format(_T("0x%02X "), unsigned(m_IC_PW2_RcvBuf[stIndex]));
					//	szDebug += szOctet;
					//}
					//szDebug.TrimRight();
					//szDebug += _T("\n");
					//OutputDebugString(szDebug);
					if (!m_fAlertIssued)
					{
						SetDlgItemText(IDC_POWERALARM, _T("POWER!"));
						MessageBeep(MB_ICONERROR);
						m_fAlertIssued = true;
					}
				}
				else if (m_fAlertIssued)
				{
					SetDlgItemText(IDC_POWERALARM, _T(""));
					m_fAlertIssued = false;
				}
				break;

			default:
				break;
			}
			break;

		case 0x1A:
			switch (m_IC_PW2_RcvBuf[5])
			{
			case 0x09:
				if (m_IC_PW2_RcvBuf.GetCount() == 8
					&& m_Amp != m_IC_PW2_RcvBuf[6])
				{
					// TRACE("Amp Response\n");
					m_Amp = m_IC_PW2_RcvBuf[6];
					UpdateData(FALSE);
				}
				break;

			case 0x0A:
				if (m_IC_PW2_RcvBuf.GetCount() == 8
					&& m_MaxPower != m_IC_PW2_RcvBuf[6])
				{
					//TRACE("Power Response\n");
					m_MaxPower = m_IC_PW2_RcvBuf[6];
					UpdateData(FALSE);
				}
				break;

			default:
				break;
			}
			break;

		case 0x1C:
			switch (m_IC_PW2_RcvBuf[5])
			{
			case 3:
				if (m_IC_PW2_RcvBuf.GetCount() == 12)
				{
					m_FreqInput2 = bcd2uint8_t(m_IC_PW2_RcvBuf[6]);   // Hz
					m_FreqInput2 += 100 * bcd2uint8_t(m_IC_PW2_RcvBuf[7]);   // 100Hz
					m_FreqInput2 += 10000 * bcd2uint8_t(m_IC_PW2_RcvBuf[8]);   // 10 kHz
					m_FreqInput2 += 1000000 * bcd2uint8_t(m_IC_PW2_RcvBuf[9]);  // 1 MHz
					m_FreqInput2 += 100000000 * bcd2uint8_t(m_IC_PW2_RcvBuf[10]);   // 100 MHz

					textVal.Format(szFreqFmt,
						m_FreqInput1 / 1000000, (m_FreqInput1 % 1000000) / 1000, (m_FreqInput1 % 1000) / 10,
						m_FreqInput2 / 1000000, (m_FreqInput2 % 1000000) / 1000, (m_FreqInput2 % 1000) / 10);
					SetDlgItemText(IDC_FREQUENCY, textVal);
				}
				break;

			default:
				break;
			}
			break;

		case 0xFA:
			UpdateData();
			if (m_Amp != -1
				|| m_MaxPower != -1)
			{
				SetDlgItemText(IDC_FREQUENCY, _T(""));
				m_Amp = -1;
				m_MaxPower = -1;
				UpdateData(FALSE);
			}
			break;

		default:
			break;
		}
	}

	m_IC_PW2_RcvBuf.RemoveAll();
}

void CHardplace7760Dlg::onIC_7760Packet()
{
	while (m_IC_7760_RcvBuf.GetCount() > 5)
	{
		if (m_IC_7760_RcvBuf[0] == 0xFE
			&& m_IC_7760_RcvBuf[1] == 0xFE)
		{
			break;
		}
		m_IC_7760_RcvBuf.RemoveAt(0);
	}
	if (m_IC_7760_RcvBuf.GetCount() > 5
		&& m_IC_7760_RcvBuf[3] == 0xB2)
	{
		switch (m_IC_7760_RcvBuf[4])
		{
		case 0x0F:
			m_IC7760OperatingMode[5] = m_IC_7760_RcvBuf[5];
			m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760OperatingMode, sizeof m_IC7760OperatingMode));
			break;

		case 0x14:
			switch (m_IC_7760_RcvBuf[5])
			{
			case 0x0A:
				if (m_IC_7760_RcvBuf.GetCount() == 9)
				{
					// TRACE("RF Level\n");
					unsigned uPower(0);
					unsigned uMultiplier(1);

					for (size_t nIndex(static_cast<size_t>(m_IC_7760_RcvBuf.GetCount()) - 2); nIndex >= 6; nIndex--)
					{
						uPower += bcd2uint8_t(m_IC_7760_RcvBuf[nIndex]) * uMultiplier;
						uMultiplier *= 100;
					}
					if (m_uPower != uPower)
					{
						m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax() - int(uPower));

						CString szRfLevel;
						szRfLevel.Format(_T("RF Level: %01uW"), static_cast<unsigned>(static_cast<float>(uPower) / 1.275));
						SetDlgItemText(IDC_RFLEVEL, szRfLevel);

						switch (uPower)
						{
						case 64:
							m_iRFLevel = 0;
							break;
						case 128:
							m_iRFLevel = 1;
							break;
						case 192:
							m_iRFLevel = 2;
							break;
						case 255:
							m_iRFLevel = 3;
							break;
						default:
							m_iRFLevel = -1;
							break;
						}
						m_PwrOn = 0;
						UpdateData(FALSE);
					}
					m_uPower = uPower;
				}
				break;

			default:
				break;
			}
			break;

		case 0x15:
			switch (m_IC_7760_RcvBuf[5])
			{
			case 0x12: // SWR
				if (m_IC_7760_RcvBuf[6] == 0x00 && m_IC_7760_RcvBuf[7] <= 0x48) // <= 1.5
				{
#if 0
					TracePacket(_T("SWR:"), m_IC_7760_RcvBuf);
#else
					OnTimer(m_idTunerEvent); // Stop Tuning
#endif
				}
				else
				{
					m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760SWR, sizeof m_IC7760SWR));
				}
				break;

			default:
				break;
			}
			break;

		case 0x1A:
			switch (m_IC_7760_RcvBuf[5])
			{
			case 0x06:
				if (m_IC_7760_RcvBuf.GetCount() == 9)
				{
					m_DataMode = m_IC_7760_RcvBuf[6];
					m_uFilterWidth = m_IC_7760_RcvBuf[7];
				}
				break;

			default:
				break;
			}
			break;

		case 0x26: // Operating Mode
			m_uBand = m_IC_7760_RcvBuf[5];
			m_uOperatingMode = m_IC_7760_RcvBuf[6];
			m_uDataMode = m_IC_7760_RcvBuf[7];
			m_uFilter = m_IC_7760_RcvBuf[8];

			m_fTuning = true;

			m_IC7760OperatingModeCmd[5] = m_IC_7760_RcvBuf[5];
			m_IC7760OperatingModeCmd[6] = 0x04; // RTTY
			m_IC7760OperatingModeCmd[7] = 0x00; // Data Mode off
			m_IC7760OperatingModeCmd[8] = m_IC_7760_RcvBuf[8];
			m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760OperatingModeCmd, sizeof m_IC7760OperatingModeCmd));

			SetTimer(m_idTunerEvent, m_TunerTimeout, NULL);
			break;

		case 0xFA: // NG Response
			if (m_PwrOn != 1
				|| m_Amp != -1
				|| m_iRFLevel != -1)
			{
				m_iRFLevel = -1;
				m_Amp = -1;
				m_MaxPower = -1;
				m_uPower = 0;
				m_PwrOn = 1;
				m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
				SetDlgItemText(IDC_FREQUENCY, _T(""));
				UpdateData(FALSE);
			}
			m_fTuning = false;
			break;

		case 0xFB: // OK response
			switch (m_IC7760LastCommand[4])
			{
			case 0x1C:
				switch (m_IC7760LastCommand[5])
				{
				case 0x00: // Transmit
					if (m_TunerMonitorSWR
						&& m_IC7760LastCommand[6] != 0x00)
					{
						m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760SWR, sizeof m_IC7760SWR));
					}
					break;

				default:
					break;
				}
				break;

			case 0x26: // operating mode
				if (m_fTuning)
				{
					m_IC7760Transmit[6] = 0x01;
					m_IC_7760_XmtQueue.Add(std::make_pair(m_IC7760Transmit, sizeof m_IC7760Transmit));
				}
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	m_IC_7760_RcvBuf.RemoveAll();
}

void CHardplace7760Dlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && !IsWindowVisible() && !m_fPlacementSet)
	{
		WINDOWPLACEMENT* lwp(0);
		UINT nl;

		m_fPlacementSet = true;

		if (theApp.GetProfileBinary(_T("Settings"), _T("Window"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}

void CHardplace7760Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	theApp.WriteProfileBinary(_T("Settings"), _T("Window"), (LPBYTE)&wp, wp.length);
	//theApp.WriteProfileInt(_T("Settings"), _T("OnTop"), (GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0 ? 1 : 0);
}
