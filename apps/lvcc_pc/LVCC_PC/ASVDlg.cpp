
// ASVDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ASV.h"
#include "ASVDlg.h"
#include "afxdialogex.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "..\ASVCommandLib\asv_type.h"
#include "..\ASVCommandLib\asv_command.h"
#include "Comport.h"
#include "Utils.h"
#include "EBPrintf.h"

static const char *gStrCornorTable[] ={
	"SF",
	"FF",
	"NN",
	"SS",
	"FS",
};

static const char *gStrDeviceFreqTable[]  = {
	"100",
	"166",
	"333",
	"ALL",
};


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CASVDlg 대화 상자




CASVDlg::CASVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CASVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pCom = NULL;
}

void CASVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_GUID, m_EdtEcid);
	DDX_Control(pDX, IDC_EDT_CPU_FREQ_START, m_EdtCpuFreqStart);
	DDX_Control(pDX, IDC_EDT_CPU_FREQ_END, m_EdtCpuFreqEnd);
	DDX_Control(pDX, IDC_EDT_CPU_FREQ_STEP, m_EdtCpuFreqStep);
	DDX_Control(pDX, IDC_EDT_VOLT_START, m_EdtVoltStart);
	DDX_Control(pDX, IDC_EDT_VOLT_END, m_EdtVoltEnd);
	DDX_Control(pDX, IDC_COMBO1, m_CmbComPort);
	DDX_Control(pDX, IDC_EDT_DEBUG, m_EdtDebug);
	DDX_Control(pDX, IDC_EDT_RESULT, m_EdtResult);
	DDX_Control(pDX, IDC_CHK_CPU, m_ChkCpu);
	DDX_Control(pDX, IDC_EDT_OUT_PATH, m_EdtOutFile);
	DDX_Control(pDX, IDC_BTN_START, m_BtnStart);
	DDX_Control(pDX, IDC_BTN_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_BTN_HW_RESET, m_BtnHWReset);
	DDX_Control(pDX, IDC_BTN_OUT_PATH, m_BtnOutPath);
	DDX_Control(pDX, IDC_CMB_CPU_SINGLE_FREQ, m_CmbCpuSingleFreq);
	DDX_Control(pDX, IDC_CMB_CORNER, m_CmbCorner);
	DDX_Control(pDX, IDC_EDT_TEMPORATURE, m_EdtTemp);
	DDX_Control(pDX, IDC_EDT_BOARD_NO, m_EdtBrdNo);
	DDX_Control(pDX, IDC_CHK_CHIP_INFO_MODE, m_ChkChipInfoMode);
	DDX_Control(pDX, IDC_EDT_NUM_AGING, m_EdtNumAging);
	DDX_Control(pDX, IDC_CHK_ENABLE_AGING, m_ChkEnAging);
	DDX_Control(pDX, IDC_EDT_CHIP_NO, m_EdtChipNo);
	DDX_Control(pDX, IDC_CHK_DEVICE, m_ChkDevice);
	DDX_Control(pDX, IDC_EDT_VOLT_DEVICE_START, m_EdtVoltDeviceStart);
	DDX_Control(pDX, IDC_EDT_VOLT_DEVICE_END, m_EdtVoltDeviceEnd);
	DDX_Control(pDX, IDC_CMB_DEVICE_FREQ, m_CmbDeviceFreq);
	DDX_Control(pDX, IDC_EDT_RESET_TIMEOUT, m_EdtResetTimeout);
	DDX_Control(pDX, IDC_EDT_TEST_TIMEOUT, m_EdtTestTimeout);
}

BEGIN_MESSAGE_MAP(CASVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CASVDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_HW_RESET, &CASVDlg::OnBnClickedBtnHwReset)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LOG, &CASVDlg::OnBnClickedBtnClearLog)
	ON_CBN_SELCHANGE(IDC_CMB_COM_PORT, &CASVDlg::OnCbnSelchangeCmbComPort)
	ON_BN_CLICKED(IDC_BTN_STOP, &CASVDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_OUT_PATH, &CASVDlg::OnBnClickedBtnOutPath)
	ON_BN_CLICKED(IDC_CHK_CHIP_INFO_MODE, &CASVDlg::OnBnClickedChkChipInfoMode)
	ON_BN_CLICKED(IDC_CHK_CPU, &CASVDlg::OnBnClickedChkCpu)
	ON_BN_CLICKED(IDC_BTN_SAVE_CONFIG, &CASVDlg::OnBnClickedBtnSaveConfig)
	ON_BN_CLICKED(IDC_CHK_DEVICE, &CASVDlg::OnBnClickedChkDevice)
END_MESSAGE_MAP()


void CASVDlg::OnOK()
{
}


// CASVDlg 메시지 처리기

BOOL CASVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	m_EdtDebug.SetLimitText(MAX_EDIT_BUF_SIZE);
	m_EdtResult.SetLimitText(MAX_EDIT_BUF_SIZE);

	m_pCom = NULL;

	//
	LoadConfiguration();
	m_Temporature	= 25;
	m_BoardNumber	= 1;
	m_ChipNumber    = 1;

	SetControlParam();

	m_ChipInfoMode = false;
	m_bHardwareOff = true;
	m_bOpenOutputFile = false;
	m_bStartTesting = false;
	m_OutputNumber = 0;

	//
	m_pASVTest = new CASVTest();
	m_pASVTest->RegisterCallback( this, ASVEventCallback, RxComRxCallback );

	TCHAR szTemp[128];
	m_CmbComPort.ResetContent();
	for(int cnt=0; cnt<=99; cnt++)
	{
		memset( szTemp, 0, sizeof(szTemp));
		if(!GetComPortFromReg(cnt, szTemp))
			continue;
		else
		{
			m_CmbComPort.AddString(szTemp);
		}
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CASVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CASVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CASVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CASVDlg::OnBnClickedBtnStart()
{
	if( !m_pCom )
	{
		MessageBox(TEXT("Comport Error!!!"));
		return;
	}

	//	Update Output File Name
	CString strOutFileName;
	m_EdtOutFile.GetWindowText(strOutFileName);
	memset( m_OutputFileName, 0, sizeof(m_OutputFileName) );
	WideCharToMultiByte( CP_ACP, 0, strOutFileName, strOutFileName.GetLength(), m_OutputFileName, sizeof(m_OutputFileName), NULL, NULL );

	GetControlParam();
	ASV_TEST_CONFIG cfg;

	memset( &cfg, 0, sizeof(cfg) );

	cfg.enableCpu     = ( BST_CHECKED == m_ChkCpu.GetCheck() ) ? 1 : 0;
	cfg.enableDevice  = ( BST_CHECKED == m_ChkDevice.GetCheck() ) ? 1 : 0;

	cfg.freqStart     = m_FreqStart;
	cfg.freqEnd       = m_FreqEnd;
	cfg.freqStep      = m_FreqStep;
	cfg.armBootUp     = m_ArmBootUpVolt;
	cfg.armFaultStart = m_ArmFaultStartVolt;
	cfg.armFaultEnd   = m_ArmFaultEndVolt;
	cfg.armVoltStart  = m_SysVoltStart;
	cfg.armVoltEnd    = m_SysVoltEnd;
	cfg.armVoltStep   = m_SysVoltStep;
	cfg.deviceTypical   = 1.0000;
	cfg.deviceVoltStart = m_DeviceVoltStart;
	cfg.deviceVoltEnd   = m_DeviceVoltEnd;
	cfg.deviceVoltStep  = m_DeviceVoltStep;

	cfg.resetTimeout    = m_ResetTimeout;
	cfg.testTimeout     = m_TestTimeout;

	//
	//	Change Control Status
	//
	m_CmbComPort.EnableWindow( FALSE );
	m_CmbCorner.EnableWindow( FALSE );
	m_BtnStart.EnableWindow( FALSE );
	m_BtnHWReset.EnableWindow( FALSE );
	m_ChkCpu.EnableWindow( FALSE );
	m_ChkDevice.EnableWindow( FALSE );
	m_CmbCpuSingleFreq.EnableWindow( FALSE );
	m_ChkChipInfoMode.EnableWindow( FALSE );

	m_EdtEcid.SetWindowText(TEXT(""));

	m_ComLastIndex = -1;

	if( m_pASVTest )
	{
		if( BST_CHECKED != m_ChkChipInfoMode.GetCheck() )
		{
			m_OutputNumber = 0;
			WriteStartLog();
		}
		m_pASVTest->SetTestConfig( &cfg, m_pCom );

		m_StartTick = GetTickCount();

		m_pASVTest->Start( m_ChipInfoMode );
		m_bHardwareOff = false;
		m_BtnHWReset.SetWindowText( TEXT("H/W ON") );
		m_bStartTesting = true;
	}
}

void CASVDlg::OnBnClickedBtnStop()
{
	if( m_pASVTest )
		m_pASVTest->Stop();

	if( !m_bStartTesting )
	{
		m_CmbComPort.EnableWindow( TRUE );
		m_CmbCorner.EnableWindow( TRUE );
		m_BtnStart.EnableWindow( TRUE );
		m_BtnHWReset.EnableWindow( TRUE );
		m_ChkCpu.EnableWindow( TRUE );
		m_ChkDevice.EnableWindow( TRUE );

		m_CmbCpuSingleFreq.EnableWindow( TRUE );

		m_ChkChipInfoMode.EnableWindow( TRUE );

		HarewareOnOff( false );
	}
}

void CASVDlg::OnBnClickedBtnHwReset()
{
	if( m_pCom )
	{
		EB_Printf(&m_EdtDebug, TEXT("Power : %d --> %d\n"), !m_bHardwareOff, m_bHardwareOff );

		//	Toggle
		if( m_bHardwareOff )
		{
			HarewareOnOff( m_bHardwareOff );
		}
		else
		{
			HarewareOnOff( m_bHardwareOff );
		}
	}
}


void CASVDlg::OnBnClickedBtnClearLog()
{
	m_EdtDebug.SetSel(0, -1);
	m_EdtDebug.ReplaceSel(TEXT(""));
	m_EdtResult.SetSel(0, -1);
	m_EdtResult.ReplaceSel(TEXT(""));
}


void CASVDlg::GetControlParam()
{
	CString strFreqStart, strFreqEnd, strFreqStep;
	CString strSysVoltStart, strSysVoltEnd, strSysVoltStep;
	CString strCoreVoltStart, strCoreVoltEnd, strCoreVoltStep;
	CString strTempo, strBoardNo, strChipNo;
	CString strVoltFixedCore, strVoltFixedSys;
	CString strArmFaultStartVolt, starArmFaultEndVolt;
	CString strResetTimeout, strTestTimeout;

	//	CPU
	m_EdtCpuFreqStart.GetWindowText(strFreqStart);
	m_EdtCpuFreqEnd.GetWindowText(strFreqEnd);
	m_EdtCpuFreqStep.GetWindowText(strFreqStep);
	m_EdtVoltStart.GetWindowText(strSysVoltStart);
	m_EdtVoltEnd.GetWindowText(strSysVoltEnd);

	m_FreqStart    = wcstoul( strFreqStart.GetString(), NULL, 10 ) * 1000000;
	m_FreqEnd      = wcstoul( strFreqEnd.GetString(), NULL, 10 ) * 1000000;
	m_FreqStep     = wcstoul( strFreqStep.GetString(), NULL, 10 ) * 1000000;
	m_SysVoltStart = _wtof( strSysVoltStart.GetString() );
	m_SysVoltEnd   = _wtof( strSysVoltEnd.GetString() );
	m_SysVoltStep  = _wtof( strSysVoltStep.GetString() );
	m_ArmFaultStartVolt = _wtof( strArmFaultStartVolt.GetString() );
	m_ArmFaultEndVolt   = _wtof( starArmFaultEndVolt.GetString() );


	m_EdtVoltDeviceStart.GetWindowText(strCoreVoltStart);
	m_EdtVoltDeviceEnd.GetWindowText(strCoreVoltEnd);
	m_DeviceVoltStart = _wtof( strCoreVoltStart.GetString() );
	m_DeviceVoltEnd   = _wtof( strCoreVoltEnd.GetString() );
	m_DeviceVoltStep  = _wtof( strCoreVoltStep.GetString() );

	m_EdtTemp.GetWindowText(strTempo);
	m_EdtBrdNo.GetWindowText(strBoardNo);
	m_EdtChipNo.GetWindowText(strChipNo);

	m_BoardNumber = _wtoi( strBoardNo.GetString() );
	m_ChipNumber = _wtoi( strChipNo.GetString() );
	m_Temporature = _wtoi( strTempo.GetString() );

	//	Timeouts
	m_EdtResetTimeout.GetWindowText(strResetTimeout);
	m_EdtTestTimeout.GetWindowText(strTestTimeout);
	m_ResetTimeout = _wtoi( strResetTimeout.GetString() );
	m_TestTimeout = _wtoi( strTestTimeout.GetString() );
}

void CASVDlg::SetControlParam()
{
	CString strFreqStart, strFreqEnd, strFreqStep;
	CString strSysVoltStart, strSysVoltEnd, strSysVoltStep;
	CString strDeviceVoltStart, strDeviceVoltEnd, strDeviceVoltStep;
	CString strResetTimeout, strTestTimeout;
	CString strTempo, strBoardNo, strChipNo;
	CString strVoltFixedCore, strVoltFixedSys;
	CString strArmBootUpVolt;
	CString strArmFaultStartVolt, strArmFaultEndVolt;
	CString strDevFreq;

	strTempo.Format(TEXT("%d"), m_Temporature);
	strBoardNo.Format(TEXT("%d"), m_BoardNumber);
	strChipNo.Format(TEXT("%d"), m_ChipNumber);

	strFreqStart.Format(TEXT("%d"), m_FreqStart/1000000);
	strFreqEnd.Format(TEXT("%d"), m_FreqEnd/1000000);
	strFreqStep.Format(TEXT("%d"), m_FreqStep/1000000);
	strSysVoltStart.Format(TEXT("%f"), m_SysVoltStart);
	strSysVoltEnd.Format(TEXT("%f"),   m_SysVoltEnd);
	strSysVoltStep.Format(TEXT("%f"),  m_SysVoltStep);
	strArmBootUpVolt.Format(TEXT("%f"),  m_ArmBootUpVolt);
	strArmFaultStartVolt.Format(TEXT("%f"),  m_ArmFaultStartVolt);
	strArmFaultEndVolt.Format(TEXT("%f"),  m_ArmFaultEndVolt);


	for( int i=0 ; i< 4 ; i++ )
	{
		strDevFreq.Format(TEXT("%S"),  gStrDeviceFreqTable[i]);
		m_CmbDeviceFreq.AddString( strDevFreq );
	}
	m_CmbDeviceFreq.SetCurSel(3);


	strDeviceVoltStart.Format(TEXT("%f"), m_DeviceVoltStart);
	strDeviceVoltEnd.Format(TEXT("%f"), m_DeviceVoltEnd);
	strDeviceVoltStep.Format(TEXT("%f"), m_DeviceVoltStep);

	//	Timeout
	strResetTimeout.Format(TEXT("%d"), m_ResetTimeout);
	strTestTimeout.Format(TEXT("%d"), m_TestTimeout);
	m_EdtResetTimeout.SetWindowText( strResetTimeout );
	m_EdtTestTimeout.SetWindowText( strTestTimeout );

	m_EdtTemp.SetWindowText( strTempo );
	m_EdtBrdNo.SetWindowText( strBoardNo );
	m_EdtChipNo.SetWindowText( strChipNo );
	m_EdtCpuFreqStart.SetWindowText(strFreqStart);
	m_EdtCpuFreqEnd.SetWindowText(strFreqEnd);
	m_EdtCpuFreqStep.SetWindowText(strFreqStep);
	m_EdtVoltStart.SetWindowText(strSysVoltStart);
	m_EdtVoltEnd.SetWindowText  (strSysVoltEnd);

	m_EdtVoltDeviceStart.SetWindowText(strDeviceVoltStart);
	m_EdtVoltDeviceEnd.SetWindowText  (strDeviceVoltEnd);

	CString strDefOutFileName;
	SYSTEMTIME time;
	GetLocalTime(&time);
	strDefOutFileName.Format(TEXT(".\\OutResult_%04d%02d%02d_%02d%02d%02d.txt"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	m_EdtOutFile.SetWindowText(strDefOutFileName);

	//
	m_CmbCorner.AddString(TEXT("SF"));
	m_CmbCorner.AddString(TEXT("FF"));
	m_CmbCorner.AddString(TEXT("NN"));
	m_CmbCorner.AddString(TEXT("SS"));
	m_CmbCorner.AddString(TEXT("FS"));
	m_CmbCorner.SetCurSel(2);

	CString str;
	m_CmbCpuSingleFreq.AddString(TEXT("ALL"));
	for( int i=0 ; (m_FreqStart + i*m_FreqStep) <= m_FreqEnd ; i++ )
	{
		str.Format(TEXT("%d"), (m_FreqStart + i*m_FreqStep)/1000000);
		m_CmbCpuSingleFreq.AddString(str);
	}
	m_CmbCpuSingleFreq.SetCurSel(0);
	m_CmbCpuSingleFreq.EnableWindow( FALSE );

	OnBnClickedChkDevice();
	OnBnClickedChkCpu();
}


void CASVDlg::RxComRxCallback( void *pArg, char *buf, int len )
{
	CASVDlg *pObj = (CASVDlg *)pArg;
	if( len > 0 )
	{
		EB_Printf( &pObj->m_EdtDebug, TEXT("%S"), buf );
	}
	else
	{
		EB_Printf( &pObj->m_EdtDebug, TEXT("%S"), buf );
	}
}


static const char gst36StrTable[36] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z'
};

static void LotIDNum2String( unsigned int lotId, char str[6] )
{
	unsigned int value[3];
	unsigned int mad[3];

	value[0] = lotId / 36;
	mad[0] = lotId % 36;

	value[1] = value[0] / 36;
	mad[1] = value[0] % 36;

	value[2] = value[1] / 36;
	mad[2] = value[1] % 36;

	// Lot ID String
	// value[2] mad[2] mad[1] mad[0]
	str[0] = 'N';
	str[1] = gst36StrTable[ value[2] ];
	str[2] = gst36StrTable[ mad[2] ];
	str[3] = gst36StrTable[ mad[1] ];
	str[4] = gst36StrTable[ mad[0] ];
	str[5] = '\0';
}


void CASVDlg::ASVEventCallback( void *pArg, ASVT_EVT_TYPE evtCode, void *evtData )
{
	CASVDlg *pObj = (CASVDlg *)pArg;
	if( ASVT_EVT_REPORT_RESULT == evtCode  )
	{
		ASV_EVT_DATA *pEvtData = (ASV_EVT_DATA *)evtData;
		char lvccStr[32];
		if( pEvtData->lvcc < 0 )
		{
			sprintf( lvccStr, "N/A" );
		}
		else
		{
			sprintf( lvccStr, "%fv", pEvtData->lvcc );
		}

		EB_Printf( &pObj->m_EdtResult, TEXT("[Result] %S, %dMHz/LVCC=%S/Time=%d msec\n"),
			ASVModuleIDToString(pEvtData->module), pEvtData->frequency/1000000, lvccStr, pEvtData->time );
		pObj->WriteLVCCData( evtCode, evtData );
	}
	else if( ASVT_EVT_ECID == evtCode )
	{
		pObj->WriteEventData( evtCode, evtData );
		if( pObj->m_ChipInfoMode )
		{
			//	Write Chip Info Data
			pObj->m_bStartTesting = false;
			pObj->OnBnClickedBtnStop();
		}
	}
	else if( ASVT_EVT_IDS_HPM == evtCode )
	{
		pObj->WriteEventData( evtCode, evtData );
		if( pObj->m_ChipInfoMode )
		{
			//	Write Chip Info Data
			pObj->m_bStartTesting = false;
			pObj->OnBnClickedBtnStop();
		}
	}
	else if( ASVT_EVT_CPUHPM == evtCode )
	{
		pObj->WriteEventData( evtCode, evtData );
		if( pObj->m_ChipInfoMode )
		{
			//	Write Chip Info Data
			pObj->m_bStartTesting = false;
			pObj->OnBnClickedBtnStop();
		}
	}
	else if( ASVT_EVT_DONE == evtCode )
	{
		EB_Printf( &pObj->m_EdtResult, TEXT("==================== All Tests Done ======================\n") );
		//pObj->WriteOutputData( evtCode, evtData );
		pObj->m_bStartTesting = false;
		pObj->OnBnClickedBtnStop();
		pObj->WriteEndDelimiter(evtData); // js.park

		if( BST_CHECKED == pObj->m_ChkEnAging.GetCheck() )
		{
			CString strNumAging;
			pObj->m_EdtNumAging.GetWindowText( strNumAging );
			unsigned int numAging = _wtoi(strNumAging.GetString());

			numAging--;

			if( numAging > 0 )
			{
				EB_Printf( &pObj->m_EdtResult, TEXT("================ Agigng Mode Aging Count(%d) ================\n"), numAging );
				strNumAging.Format(TEXT("%d"), numAging);
				pObj->m_EdtNumAging.SetWindowText(strNumAging);
				pObj->OnBnClickedBtnStart();
			}
		}
		::sndPlaySound( TEXT("./notify.wav"), SND_SYNC );
	}
	else if( ASVT_EVT_ERR == evtCode )
	{
		pObj->m_bStartTesting = false;
		pObj->OnBnClickedBtnStop();
		::sndPlaySound( TEXT("./error.wav"), SND_SYNC );
	}
}

void CASVDlg::HarewareOnOff( bool on )
{
	if( on )
	{
		m_pCom->SetDTR();
		m_BtnHWReset.SetWindowText( TEXT("H/W ON") );
	}
	else
	{
		m_pCom->ClearDTR();
		m_BtnHWReset.SetWindowText( TEXT("H/W OFF") );
	}
	m_bHardwareOff = !on;
}

void CASVDlg::OnCbnSelchangeCmbComPort()
{
	int curIdx = m_CmbComPort.GetCurSel();	//	Zero Based Index
	if( curIdx == CB_ERR )
		return;

	//	Find Comport Number & Name
	m_CmbComPort.GetLBText( curIdx, m_CurComPortName );
	swscanf(m_CurComPortName, TEXT("COM%d"), &m_CurComPort);

	if( m_pCom )
	{
		if( IDOK != MessageBoxW(TEXT("[Warning] Comport Already Opend!!\nIf you want to change Comport ?"), TEXT("Wanring!!"), MB_OKCANCEL ) )
		{
			m_CmbComPort.SetCurSel(m_ComLastIndex);
			return ;
		}
		delete m_pCom;
	}
	m_ComLastIndex = curIdx;
	m_pCom = new CComPort();
	if( !m_pCom->OpenComPort( m_CurComPort ) )
	{
		delete m_pCom;
		m_pCom = NULL;
		MessageBox(TEXT("Error !!! Comport Open Failed !!!"));
		m_CmbComPort.SetCurSel(-1);
		return;
	}
	m_pCom->Flush();
	HarewareOnOff( false );
	//m_pCom->SetRxCallback( this, CASVDlg::RxComRxCallback );
}


void CASVDlg::OnBnClickedBtnOutPath()
{
#if 0	//	Output Path
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR szDisplayName[MAX_PATH];
	szDisplayName[0]  = 0;
	bi.hwndOwner      = NULL;
	bi.pidlRoot       = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle      = _T("Please select a folder for out folder :");
	bi.ulFlags        = BIF_RETURNONLYFSDIRS;
	bi.lParam         = NULL;
	bi.iImage         = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if( NULL != pidl )
	{
		TCHAR pathName[MAX_PATH];
		BOOL bRet = SHGetPathFromIDList(pidl,pathName);
		if(FALSE == bRet)
			return;
		m_EdtOutFile.SetWindowText( pathName );
	}
#else
	CString strDefOutFileName;
	SYSTEMTIME time;
	GetLocalTime(&time);
	strDefOutFileName.Format(TEXT(".\\OutResult_%04d%02d%02d_%02d%02d%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	CFileDialog openDlg(FALSE, TEXT("txt"), strDefOutFileName, OFN_SHOWHELP | OFN_OVERWRITEPROMPT, TEXT("Save Debug Log(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if( IDOK == openDlg.DoModal() )
	{
		m_OutputNumber = 0;
		m_EdtOutFile.SetWindowText( openDlg.GetPathName() );
		m_EdtOutFile.SetSel(0, -1);
		m_EdtOutFile.SetSel(-1, -1);
	}
#endif
}



void CASVDlg::WriteEventData( ASVT_EVT_TYPE evtType, void *evtData )
{
	if( ASVT_EVT_ECID == evtType )
	{
		unsigned int *ecid = (unsigned int *)evtData;
		CString str;
		str.Format(TEXT("%08X-%08X-%08X-%08X"), ecid[0], ecid[1], ecid[2], ecid[3] );
		m_EdtEcid.SetWindowText(str);

		if( m_pASVTest )
		{
			m_pASVTest->ParseECID( ecid, &m_ChipInfo );
		}
	}
	
	else if( ASVT_EVT_CPUHPM == evtType )
	{
		unsigned int *hpm = (unsigned int *)evtData;
		m_cpuHPM = *hpm;
		EB_Printf( &m_EdtResult, TEXT("HPM : %d\n"),m_cpuHPM);
	}
	else if( ASVT_EVT_IDS_HPM == evtType )
	{
		unsigned int *ids_hpm = (unsigned int*)evtData;
		m_IDS[0] = ids_hpm[0];
		m_IDS[1] = ids_hpm[1];
		m_HPM[0] = ids_hpm[2];
		m_HPM[1] = ids_hpm[3];
		m_HPM[2] = ids_hpm[4];
		m_HPM[3] = ids_hpm[5];
		m_HPM[4] = ids_hpm[6];
		m_HPM[5] = ids_hpm[7];
		m_HPM[6] = ids_hpm[8];
		m_HPM[7] = ids_hpm[9];


		char strLotID[6];
		LotIDNum2String( m_ChipInfo.lotID, strLotID );
		EB_Printf( &m_EdtResult, TEXT("LotID=(%S), WavferNo(%d), X(%d), Y(%d), IDS(%d,%d) RO(%d,%d,%d,%d,%d,%d,%d,%d)\n"),
			strLotID, m_ChipInfo.waferNo, m_ChipInfo.xPos, m_ChipInfo.yPos,m_IDS[0], m_IDS[1], m_HPM[0], m_HPM[1], m_HPM[2], m_HPM[3], m_HPM[4], m_HPM[5], m_HPM[6], m_HPM[7]);

		if( BST_CHECKED == m_ChkChipInfoMode.GetCheck() )
		{
			FILE *fd;
			if( m_OutputNumber == 0 )
			{
				fd = fopen( m_OutputFileName, "a" );
				if( fd == NULL )
				{
					MessageBox(TEXT("Open Failed!!\nPlease Check Output Filename or Path!!"));
					return;
				}
				fprintf( fd, "No.\tCornor\tLotID\tWavferNo\tX\tY\tIDS_0\tIDS_1\tHPM_0\tHPM_1\tHPM_2\tHPM_3\tHPM_4\tHPM_5\tHPM_6\tHPM_7\n");
				fclose(fd);
			}
			fd = fopen( m_OutputFileName, "a" );
			if( fd )
			{
				char strLotID[6];
				LotIDNum2String( m_ChipInfo.lotID, strLotID );
				fprintf( fd, "%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
					++m_OutputNumber, gStrCornorTable[m_CmbCorner.GetCurSel()], strLotID, m_ChipInfo.waferNo, m_ChipInfo.xPos, m_ChipInfo.yPos,
					m_IDS[0], m_IDS[1], m_HPM[0], m_HPM[1], m_HPM[2], m_HPM[3], m_HPM[4], m_HPM[5], m_HPM[6], m_HPM[7]);
				fclose(fd);
			}
		}

	}
}

void CASVDlg::WriteLVCCData( ASVT_EVT_TYPE evtType, void *evtData )
{
	if( ASVT_EVT_REPORT_RESULT == evtType )
	{
		ASV_EVT_DATA *pEvtData = (ASV_EVT_DATA *)evtData;
		FILE *fd;
		char strLotID[6];
		LotIDNum2String( m_ChipInfo.lotID, strLotID );

		if( m_OutputNumber == 0 )
		{
			fd = fopen( m_OutputFileName, "a" );
			if( fd == NULL )
			{
				MessageBox(TEXT("Open Failed!!\nPlease Check Output Filename or Path!!"));
				return;
			}
			fprintf( fd,
				"\tChip No."
				"\tCornor"
				"\tLotID"
				"\tWavferNo"
				"\tX"
				"\tY"
				"\tIDS_0"
				"\tIDS_1"
				"\tHPM_0"
				"\tHPM_1"
				"\tHPM_2"
				"\tHPM_3"
				"\tHPM_4"
				"\tHPM_5"
				"\tHPM_6"
				"\tHPM_7"
				"\tCPU_HPM"
				"\tRUN_HPM"
				"\tBoard No."
				"\tTemp."
				"\tDomain"
				"\tSpeed"
				"\tTMU Start"
				"\tTMU End"
				"\tLVCC"
				"\tTest Time(sec)"
				"\tDate"
				"\n");
			fclose(fd);
		}
		fd = fopen( m_OutputFileName, "a" );
		if( fd )
		{
			char dateStr[64];
			char lvccStr[64];
			m_EndTick = GetTickCount();

			SYSTEMTIME time;
			GetLocalTime(&time);
			sprintf( dateStr, "%d-%d-%d-%d:%d:%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

			if( pEvtData->lvcc < 0 )
			{
				sprintf( lvccStr, "N/A" );
			}
			else
			{
				sprintf( lvccStr, "%.2f", pEvtData->lvcc*1000. );
			}
			fprintf( fd,
				"\t%d"		//	Chip Number
				"\t%s"		//	Cornor
				"\t%s"		//	LotID
				"\t%d"		//	Wavfer Number
				"\t%d"		//	X
				"\t%d"		//	Y
				"\t%d"		//	IDS_0
				"\t%d"		//	IDS_1
				"\t%d"		//	HPM_0
				"\t%d"		//	HPM_1
				"\t%d"		//	HPM_2
				"\t%d"		//	HPM_3
				"\t%d"		//	HPM_4
				"\t%d"		//	HPM_5
				"\t%d"		//	HPM_6
				"\t%d"		//	HPM_7
				"\t%d"		//	BOOT CPUHPMe
				"\t%d"		//	RUN HPM
				"\t%x"		//	Board Number
				"\t%d"		//	Temporature
				"\t%s"		//	Domain
				"\t%d"		//	Speed
				"\t%d"		//	TMU Start
				"\t%d"		//	TMU End
				"\t%s"		//	LVCC
				"\t%d"		//	Test Time
				"\t%s\n",		//	Date & Time
				m_ChipNumber,
				gStrCornorTable[m_CmbCorner.GetCurSel()],
				strLotID,
				m_ChipInfo.waferNo,
				m_ChipInfo.xPos,
				m_ChipInfo.yPos,
				m_IDS[0],
				m_IDS[1],
				m_HPM[0],
				m_HPM[1],
				m_HPM[2],
				m_HPM[3],
				m_HPM[4],
				m_HPM[5],
				m_HPM[6],
				m_HPM[7],
				m_cpuHPM,
				pEvtData->cpu_hpm,
				m_BoardNumber,
				m_Temporature,
				ASVModuleIDToStringSimple( pEvtData->module ),
				pEvtData->frequency / 1000000,
				pEvtData->tmuStart,
				pEvtData->tmuEnd,
				lvccStr,
				pEvtData->time / 1000,
				dateStr);
			fclose(fd);
		}

		m_OutputNumber ++;
	}
}

void CASVDlg::WriteStartLog()
{
	FILE *fd;
	fd = fopen( m_OutputFileName, "a" );
	if( fd )
	{
		CString strDefOutFileName;
		SYSTEMTIME time;
		GetLocalTime(&time);
		fprintf( fd, "--------------------------------------------------------------------------------------------------------------------------------------------------------------\n" );
		fprintf( fd, "  Start Test \n");
		fprintf( fd, "  Date & Time : %04d-%02d-%02d, %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fprintf( fd, "--------------------------------------------------------------------------------------------------------------------------------------------------------------\n" );
		fclose( fd );
	}
}

void CASVDlg::WriteEndDelimiter(void *evtData)
{
	FILE *fd;
	fd = fopen( m_OutputFileName, "a" );
	if( fd )
	{
		fprintf( fd, "--------------------------------------------------------------------------------------------------------------------------------------------------------------\n" );
		fclose( fd );
	}
}


typedef struct ASV_SAVE_CONFIG {
	int				enableCpu;
	int				enableDevice;
	unsigned int	cpuFreqStart, cpuFreqEnd, cpuFreqStep;
	double			armVoltStart, armVoltEnd, armVoltStep;
	double			armFaultStart, armFaultEnd;
	double			armBootUpVolt;
	//	Device Config
	double			deviceVoltStart;
	double			deviceVoltEnd;
	double			deviceVoltStep;
	//	Timeout Config
	int				resetTimeout;
	int				testTimeout;
}ASV_SAVE_CONFIG;


//
//	Configuration Load & Save Functions
//
#define	CFG_FILE_NAME	L".\\ASV_Config.dat"
void CASVDlg::LoadConfiguration()
{
	DWORD readSize, writtenSize;
	HANDLE hCfgFile = CreateFile( CFG_FILE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if( INVALID_HANDLE_VALUE != hCfgFile )
	{
		ASV_SAVE_CONFIG *pAllCfg = (ASV_SAVE_CONFIG *)malloc( sizeof(ASV_SAVE_CONFIG) );
		if( !ReadFile( hCfgFile, pAllCfg, sizeof(ASV_SAVE_CONFIG), &readSize, NULL ) || readSize != sizeof(ASV_SAVE_CONFIG) )
		{
			//	Missmatch config size ==> Make Default Configration
			free( pAllCfg );
			CloseHandle( hCfgFile );
			SetDefaultConfiguration();
			SaveConfiguration();
			return;
		}
		CloseHandle( hCfgFile );

		if( pAllCfg->enableCpu ) m_ChkCpu.SetCheck( BST_CHECKED ); else m_ChkCpu.SetCheck( BST_UNCHECKED );
		if( pAllCfg->enableDevice) m_ChkDevice.SetCheck( BST_CHECKED ); else m_ChkDevice.SetCheck( BST_UNCHECKED );

		//	CPU Test
		m_FreqStart     = pAllCfg->cpuFreqStart ;
		m_FreqEnd       = pAllCfg->cpuFreqEnd   ;
		m_FreqStep      = pAllCfg->cpuFreqStep  ;
		m_SysVoltStart  = pAllCfg->armVoltStart ;
		m_SysVoltEnd    = pAllCfg->armVoltEnd   ;
		m_SysVoltStep   = pAllCfg->armVoltStep  ;
		m_ArmBootUpVolt = pAllCfg->armBootUpVolt;
		m_ArmFaultStartVolt= pAllCfg->armFaultStart;
		m_ArmFaultEndVolt  = pAllCfg->armFaultEnd;

		//	Device Test
		m_DeviceVoltStart	= pAllCfg->deviceVoltStart;
		m_DeviceVoltEnd		= pAllCfg->deviceVoltEnd;
		m_DeviceVoltStep	= pAllCfg->deviceVoltStep;
		//	Timeouts
		m_ResetTimeout		= pAllCfg->resetTimeout;
		m_TestTimeout		= pAllCfg->testTimeout;

		free( pAllCfg );
	}
	else
	{
		SetDefaultConfiguration();
		SaveConfiguration();
	}
}

void CASVDlg::SaveConfiguration()
{
	DWORD wSize = sizeof(DWORD);
	ASV_SAVE_CONFIG cfg;
	HANDLE hWriteFile = CreateFile( CFG_FILE_NAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	cfg.enableCpu       = ( BST_CHECKED == m_ChkCpu.GetCheck() )? 1 : 0;
	cfg.cpuFreqStart    = m_FreqStart    ;
	cfg.cpuFreqEnd      = m_FreqEnd      ;
	cfg.cpuFreqStep     = m_FreqStep     ;
	cfg.armVoltStart    = m_SysVoltStart ;
	cfg.armVoltEnd      = m_SysVoltEnd   ;
	cfg.armVoltStep     = m_SysVoltStep  ;
	cfg.armBootUpVolt   = m_ArmBootUpVolt;
	cfg.armFaultStart   = m_ArmFaultStartVolt;
	cfg.armFaultEnd     = m_ArmFaultEndVolt;

	cfg.enableDevice	= ( BST_CHECKED == m_ChkDevice.GetCheck() )? 1 : 0;
	cfg.deviceVoltStart = m_DeviceVoltStart;
	cfg.deviceVoltEnd   = m_DeviceVoltEnd  ;
	cfg.deviceVoltStep  = m_DeviceVoltStep ;

	cfg.resetTimeout	= m_ResetTimeout;
	cfg.testTimeout		= m_TestTimeout;

	if( INVALID_HANDLE_VALUE != hWriteFile )
	{
		WriteFile( hWriteFile, &cfg, sizeof(ASV_SAVE_CONFIG), &wSize, NULL );
		FlushFileBuffers( hWriteFile );
		CloseHandle( hWriteFile );
	}
}

void CASVDlg::SetDefaultConfiguration()
{
	//	CPU Test
	m_ChkCpu.SetCheck(BST_CHECKED);
	m_FreqStart		= 100000000;
	m_FreqEnd		= 1000000000;
	m_FreqStep		= 100000000;
	m_SysVoltStart  = 0.80;
	m_SysVoltEnd    = 1.45;
	m_SysVoltStep   = 0.0125;
	m_ArmBootUpVolt = 1.0000;
	m_ArmFaultStartVolt = 1.0;
	m_ArmFaultEndVolt   = 1.1;

	//	Device Test
	m_ChkDevice.SetCheck(BST_CHECKED);
	m_DeviceVoltStart = 0.8;
	m_DeviceVoltEnd   = 1.2;
	m_DeviceVoltStep  = 0.0125;

	//	ResetTimeout
	m_ResetTimeout	= 15;
	m_TestTimeout	= 15;

}


//
//		En/Disable Controls
//

void CASVDlg::OnBnClickedChkChipInfoMode()
{
	if( BST_CHECKED == m_ChkChipInfoMode.GetCheck() )
	{
		//	CPU Test
		m_ChkCpu.EnableWindow(FALSE);
		m_CmbCpuSingleFreq.EnableWindow(FALSE);
		m_EdtCpuFreqStart.EnableWindow(FALSE);
		m_EdtCpuFreqEnd.EnableWindow(FALSE);
		m_EdtCpuFreqStep.EnableWindow(FALSE);
		m_EdtVoltStart.EnableWindow(FALSE);
		m_EdtVoltEnd.EnableWindow(FALSE);
		m_ChipInfoMode = true;

		m_ChkDevice.EnableWindow(FALSE);
		m_EdtVoltDeviceStart.EnableWindow(FALSE);
		m_EdtVoltDeviceEnd.EnableWindow(FALSE);
	}
	else
	{
		//	CPU Test
		m_ChkCpu.EnableWindow(TRUE);
		m_CmbCpuSingleFreq.EnableWindow(TRUE);
		m_EdtCpuFreqStart.EnableWindow(TRUE);
		m_EdtCpuFreqEnd.EnableWindow(TRUE);
		m_EdtCpuFreqStep.EnableWindow(TRUE);
		m_EdtVoltStart.EnableWindow(TRUE);
		m_EdtVoltEnd.EnableWindow(TRUE);
		m_ChipInfoMode = false;

		//	Device Test
		m_ChkDevice.EnableWindow(TRUE);
		m_EdtVoltDeviceStart.EnableWindow(TRUE);
		m_EdtVoltDeviceEnd.EnableWindow(TRUE);
	}
}


void CASVDlg::OnBnClickedChkCpu()
{
	if( BST_CHECKED == m_ChkCpu.GetCheck() )
	{
		m_EdtCpuFreqStart.EnableWindow( TRUE );
		m_EdtCpuFreqEnd.EnableWindow( TRUE );
		m_EdtCpuFreqStep.EnableWindow( TRUE );
		m_EdtVoltStart.EnableWindow( TRUE );
		m_EdtVoltEnd.EnableWindow( TRUE );
	}
	else
	{
		m_EdtCpuFreqStart.EnableWindow( FALSE );
		m_EdtCpuFreqEnd.EnableWindow( FALSE );
		m_EdtCpuFreqStep.EnableWindow( FALSE );
		m_EdtVoltStart.EnableWindow( FALSE );
		m_EdtVoltEnd.EnableWindow( FALSE );
	}
}

void CASVDlg::OnBnClickedChkDevice()
{
	if( BST_CHECKED == m_ChkDevice.GetCheck() )
	{
		m_CmbDeviceFreq.EnableWindow( TRUE );
		m_EdtVoltDeviceStart.EnableWindow( TRUE );
		m_EdtVoltDeviceEnd.EnableWindow( TRUE );
	}
	else
	{
		m_CmbDeviceFreq.EnableWindow( FALSE );
		m_EdtVoltDeviceStart.EnableWindow( FALSE );
		m_EdtVoltDeviceEnd.EnableWindow( FALSE );
	}
}

void CASVDlg::OnBnClickedBtnSaveConfig()
{
	//	Get Prameters From Controls
	GetControlParam();
	//	Save Paramters To File
	SaveConfiguration();
}
