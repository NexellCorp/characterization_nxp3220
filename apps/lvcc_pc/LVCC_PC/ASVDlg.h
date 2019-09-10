
// ASVDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "Comport.h"
#include "ASVTest.h"

#define	MAX_LVCC_DATA	2048

// CASVDlg ��ȭ ����
class CASVDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CASVDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ASV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
	void OnOK();

	//	User Application Variables
private:
	//	UI Get / Set Control's Paramters
	void GetControlParam();
	void SetControlParam();
	static void RxComRxCallback( void *pArg, char *buf, int len );
	static void ASVEventCallback( void *pArg, ASVT_EVT_TYPE evtCode, void *evtData );
	void HarewareOnOff( bool on );

	void WriteEventData( ASVT_EVT_TYPE evtType, void *evtData );
	void WriteLVCCData( ASVT_EVT_TYPE evtType, void *evtData );
	void WriteEndDelimiter(void *evtData);	// js.park
	void WriteStartLog();

	//	Configuration Save/Load
	void LoadConfiguration();
	void SaveConfiguration();
	void SetDefaultConfiguration();

	CComPort		*m_pCom;
	CASVTest		*m_pASVTest;
	DWORD			m_CurComPort;
	TCHAR			m_CurComPortName[32];

	int				m_Temporature;
	TCHAR			m_Corner[8];
	CHIP_INFO		m_ChipInfo;

	unsigned int	m_IDS[2];
	unsigned int	m_HPM[8];
	unsigned int 	m_cpuHPM;
	unsigned int 	m_rcpuHPM;

	unsigned int	m_BoardNumber, m_ChipNumber;
	unsigned int	m_FreqStart, m_FreqEnd, m_FreqStep;
	double			m_SysVoltStart, m_SysVoltEnd, m_SysVoltStep;
	double			m_DeviceVoltStart, m_DeviceVoltEnd, m_DeviceVoltStep;
	int				m_TestTimeout, m_ResetTimeout;

	double			m_ArmBootUpVolt;
	double			m_ArmFaultStartVolt, m_ArmFaultEndVolt;
	double			m_VoltFixedCore;
	bool			m_ChipInfoMode;

	//	H/W Reset
	bool			m_bHardwareOff;
	bool			m_bStartTesting;

	//	Output File
	bool			m_bOpenOutputFile;

	int				m_OutputNumber;
	char			m_OutputFileName[MAX_PATH];			//	Output File Name
	char			m_ResultLogFileName[MAX_PATH];		//	Result Log File Name
	char			m_DbgLogFileName[MAX_PATH];			//	Debug Log File Name

	DWORD			m_StartTick, m_EndTick;

	int				m_ComLastIndex;

	unsigned int	m_frequency[30];	// js.park
	char 			m_lvccStr[30][64];	// js.park

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_ChkChipInfoMode;

	CComboBox m_CmbComPort;

	CEdit m_EdtEcid;

	//	CPU Config
	CButton m_ChkCpu;
	CEdit m_EdtCpuFreqStart;
	CEdit m_EdtCpuFreqEnd;
	CEdit m_EdtCpuFreqStep;
	CEdit m_EdtVoltStart;
	CEdit m_EdtVoltEnd;

	//	Device Config
	CButton m_ChkDevice;
	CComboBox m_CmbDeviceFreq;

	//	Core Config
	CButton m_ChkVpu;
	CButton m_Chk3D;
	CEdit m_EdtVoltDeviceStart;
	CEdit m_EdtVoltDeviceEnd;

	CButton m_BtnStart;
	CButton m_BtnStop;
	CButton m_BtnHWReset;

	CComboBox m_CmbCpuSingleFreq;

	//	Debug Log Related
	CEdit m_EdtDebug;		//	Debug Window
	//	Result Log Related
	CEdit m_EdtResult;		//	Result Window

	//	Output Relative Controls
	CEdit m_EdtOutFile;
	CButton m_BtnOutPath;


	CComboBox m_CmbCorner;
	CEdit m_EdtTemp;
	CEdit m_EdtBrdNo;
	CEdit m_EdtChipNo;

	CEdit m_EdtFixedCoreVolt;

	//	Aging Tool
	CEdit m_EdtNumAging;
	CButton m_ChkEnAging;

	//	Timeouts
	CEdit m_EdtResetTimeout;
	CEdit m_EdtTestTimeout;

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnHwReset();
	afx_msg void OnBnClickedBtnClearLog();

	afx_msg void OnCbnSelchangeCmbComPort();	//	Comport ComboBox
	afx_msg void OnBnClickedBtnOutPath();
	afx_msg void OnBnClickedChkChipInfoMode();
	afx_msg void OnBnClickedChkCpu();
	afx_msg void OnBnClickedChkDevice();
	afx_msg void OnBnClickedBtnSaveConfig();
};