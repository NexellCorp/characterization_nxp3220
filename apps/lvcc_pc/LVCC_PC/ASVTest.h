#ifndef __ASVTEST_H__
#define __ASVTEST_H__

#include "..\ASVCommandLib\asv_type.h"
#include "..\ASVCommandLib\asv_command.h"
#include "Comport.h"

#define	NUM_VPU_FREQ_TABLE	4
#define	NUM_3D_FREQ_TABLE	4
extern const unsigned int gVpuFreqTable[NUM_VPU_FREQ_TABLE];
extern const unsigned int g3DFreqTable[NUM_3D_FREQ_TABLE];

#define	MAX_FREQ_TABLE		64

#define ENABLE_CMD_TEST		0

typedef enum {
	ASVT_EVT_ERR = -1,
	ASVT_EVT_DONE = 0,
	ASVT_EVT_ECID,
	ASVT_EVT_IDS_HPM,
	ASVT_EVT_CPUHPM,
	ASVT_EVT_REPORT_RESULT,
} ASVT_EVT_TYPE;

typedef struct {
	ASV_COMMAND		cmd;
	ASV_MODULE_ID	module;
	ASV_PARAM		param;
} ASVT_REPORT_TYPE;

typedef struct {
	unsigned int	freqStart;
	unsigned int	freqEnd;
	unsigned int	freqStep;
	unsigned int	freqOther;
	double			voltStart;
	double			voltEnd;
	double			voltTypical;
} ASV_COMMON_PARAM;

typedef struct {
	//	General Config
	int					temporature;
	int					boardNumber;
	int					chipNumber;

	//	CPU Config
	int					enableCpu;
	ASV_COMMON_PARAM	arm;

	//	MM Block Confing
	int					enableMM;
	unsigned int		mmAxiFreq;
	ASV_COMMON_PARAM	mm;

	//	USB Block Config
	int					enableUSB;
	ASV_COMMON_PARAM	usb;

	//	System Bus Config
	int					enableSysBus;
	ASV_COMMON_PARAM	bus;

	int					resetTimeout;
	int					testTimeout;
} ASV_TEST_CONFIG;

typedef struct {
	ASV_MODULE_ID	module;
	unsigned int	frequency;
	unsigned int	hpm;			//	cpu HPM
	unsigned int	coreHpm;		//	Core HPM
	double			lvcc;			//	volt
	int				time;			//	processing time in milli second
	int				tmuStart;
	int				tmuEnd;
} ASV_EVT_DATA;


//	Fuse	MSB	Description
//	0~20	0	Lot ID
//	21~25	21	Wafer No
//	26~31	26	X_POS_H
//	0~1	32	X_POS_L
//	2~9	34	Y_POS
//	10~15	42	"000000" fix
//	16~23	48	CPU_IDS
//	24~31	56	CPU_RO
//	64~79	64	"0000000000000000" fix
//	80~95	80	"0000000000000000" fix
//	96~111	96	"0001011100100000" fix  (0x04E8)
//	112~127	112	"0010110001001000" fix  (0x1234)

typedef struct {
	unsigned int	lotID;
	unsigned int	waferNo;
	unsigned int	xPos, yPos;
	unsigned int	ids;
	unsigned int	ro;
	unsigned int	usbProductId;
	unsigned int	usbVendorId;
} CHIP_INFO;

class CASVTest{
public:
	CASVTest();
	virtual ~CASVTest();

	void RegisterCallback(
		void *pArg,															//	Argument
		void (*cbEvent)(void *pArg, ASVT_EVT_TYPE evtCode, void *evtData),	//	Event Callback
		void (*cbMsg)(void *pArg, char *str, int32_t len) );				//	Message Callback

	//
	//	Min/Max Voltage, Voltage Step
	//
	//
	void SetTestConfig( ASV_TEST_CONFIG *config, CComPort *pCom );
	bool Start( bool bChipIdMode = false );
	void Stop();
	bool Scan();
	void ParseECID( unsigned int ECID[4], CHIP_INFO *chipInfo);

private:
	void *m_pcbArg;
	void (*m_cbEventFunc)(void *pArg, ASVT_EVT_TYPE evtCode, void *evtData);
	void (*m_cbMessageFunc)(void *pArg, char *str, int32_t len);
	void DbgLogPrint( int flag, char *fmt,... );

	static void FindLVCCThreadStub( void *pArg )
	{
		((CASVTest *)pArg)->FindLVCCThread();
	}
	void FindLVCCThread();
	double FastTestLoop( ASV_MODULE_ID module, ASV_COMMON_PARAM *param, unsigned int frequency, int tmu[2] );
	//
	bool	SetFrequency(ASV_MODULE_ID module, unsigned int frequency);
	bool	SetMMAxiFrequency( ASV_MODULE_ID module, unsigned int frequency );
	bool	SetVoltage( ASV_MODULE_ID module, double voltage );
	bool	StartTest( ASV_MODULE_ID module, int retryCnt = 1 );
	double	SelectNextVoltage( double min, double current, double step );


	bool	TestLowestVolt( ASV_MODULE_ID module, unsigned int freq, unsigned int typical, double volt );

	//	TMU Read Functions
	bool	GetTMUResponse( int *pTMU );
	bool	GetTMUInformation( int *pTMU0, int *pTMU1 );

	bool	GetECID(unsigned int ecid[4]);
	bool	GetECIDCmdResponse(unsigned int ecid[4]);

	bool	GetIDS(unsigned int IDS[2]);
	bool	GetIDSResponse(unsigned int IDS[2]);
	bool	GetHPM(unsigned int RO[8]);
	bool	GetHPMResponse(unsigned int RO[8]);

	bool	GetCpuHPM(unsigned int* hpm);
	bool	GetHPMRUNCPUResponse(unsigned int *hpm);
	bool	GetCoreHPM(unsigned int* hpm);
	bool	GetHPMRunSysResponse(unsigned int* hpm);

	static void RxComRxCallbackStub( void *pArg, char *buf, int len )
	{
		((CASVTest *)pArg)->RxComRxCallback( buf, len );
	}
	void RxComRxCallback( char *buf, int len );

	//
	bool CheckCommandResponse();
	bool HardwareReset();
	bool WaitBootOnMessage();


	//	Test Mode Functions
	void TestCommand();

private:
	bool			m_bConfig;
	CComPort		*m_pCom;

	ASV_TEST_CONFIG	m_TestConfig;

	ASV_MODULE_ID	m_TestModuleId;
	uint32_t		m_Frequency;
	double			m_MinVolt;
	double			m_MaxVolt;
	uint32_t		m_Timeout;

	ASV_TEST_CONFIG	m_LastConfig;

	bool			m_bThreadExit;
	LONG			m_nSem;

	bool			m_bChipIdMode;

	//	Response Event
	HANDLE			m_hRxSem;

	CRITICAL_SECTION m_CritRxMsg;

	char			m_RxMessage[4096];
	int				m_RxMsgLen;
	char			m_DbgStr[4096];

};

#endif	// __ASVTEST_H__
