#include "StdAfx.h"
#include "ASVTest.h"
#include "Utils.h"

#define	RETRY_COUNT				3
#define	LVCC_RETRY_COUNT		3
#define	SCAN_RETRY_COUNT		3
#define RESET_RETRY_COUNT		5

#define	HARDWARE_RESET_TIME		1000


#define NUM_DEVICE_FREQ_TABLE	3


const unsigned int gDeviceFreqTable[NUM_DEVICE_FREQ_TABLE] =
{
	100000000,
	166000000,
	333000000,
};

//////////////////////////////////////////////////////////////////////////////
//
//			PMIC Informatgions
//
//////////////////////////////////////////////////////////////////////////////

enum {
	PMIC_MP8845,
	PMIC_SM5011,
};

static double SM5011_VTable[] = {
	0.6000, 0.6125, 0.6250, 0.6375, 0.6500, 0.6625, 0.6750, 0.6875,
	0.7000, 0.7125, 0.7250, 0.7375, 0.7500, 0.7625, 0.7750, 0.7875,
	0.8000, 0.8125, 0.8250, 0.8375, 0.8500, 0.8625, 0.8750, 0.8875,
	0.9000, 0.9125, 0.9250, 0.9375, 0.9500, 0.9625, 0.9750, 0.9875,
	1.0000, 1.0125, 1.0250, 1.0375, 1.0500, 1.0625, 1.0750, 1.0875,
	1.1000, 1.1125, 1.1250, 1.1375, 1.1500, 1.1625, 1.1750, 1.1875,
	1.2000, 1.2125, 1.2250, 1.2375, 1.2500, 1.2625, 1.2750, 1.2875,
	1.3000, 1.3125, 1.3250, 1.3375, 1.3500, 1.3625, 1.3750, 1.3875,
	1.4000, 1.4125, 1.4250, 1.4375, 1.4500, 1.4625, 1.4750, 1.4875,
};
static const int SM5011_MAX_VTABLE = 72;

static double MP8845_VTable[] = 
{
	0.6000,	0.6067,	0.6134,	0.6201,	0.6268,	0.6335,	0.6401,	0.6468,
	0.6535,	0.6602,	0.6669,	0.6736,	0.6803,	0.6870,	0.6937,	0.7004,
	0.7070,	0.7137,	0.7204,	0.7271,	0.7338,	0.7405,	0.7472,	0.7539,
	0.7606,	0.7673,	0.7739,	0.7806,	0.7873,	0.7940,	0.8007,	0.8074,
	0.8141,	0.8208,	0.8275,	0.8342,	0.8408,	0.8475,	0.8542,	0.8609,
	0.8676,	0.8743,	0.8810,	0.8877,	0.8944,	0.9011,	0.9077,	0.9144,
	0.9211,	0.9278,	0.9345,	0.9412,	0.9479,	0.9546,	0.9613,	0.9680,
	0.9746,	0.9813,	0.9880,	0.9947,	1.0014,	1.0081,	1.0148,	1.0215,
	1.0282,	1.0349,	1.0415,	1.0482,	1.0549,	1.0616,	1.0683,	1.0750,
	1.0817,	1.0884,	1.0951,	1.1018,	1.1084,	1.1151,	1.1218,	1.1285,
	1.1352,	1.1419,	1.1486,	1.1553,	1.1620,	1.1687,	1.1753,	1.1820,
	1.1887,	1.1954,	1.2021,	1.2088,	1.2155,	1.2222,	1.2289,	1.2356,
	1.2422,	1.2489,	1.2556,	1.2623,	1.2690,	1.2757,	1.2824,	1.2891,
	1.2958,	1.3025,	1.3091,	1.3158,	1.3225,	1.3292,	1.3359,	1.3426,
	1.3493,	1.3560,	1.3627,	1.3694,	1.3760,	1.3827,	1.3894,	1.3961,
	1.4028,	1.4095,	1.4162,	1.4229,	1.4296,	1.4363,	1.4429,	1.4496,
};
static const int MP8848_MAX_VTABLE = 128;



//
//	H/W TYPICAL Block Inforamtions
//
#define TYPICAL_CPU_FREQ		(800000000)
#define TYPICAL_CPU_VOLTAGE		(1.00000)

#define TYPICAL_MM_FREQ			(333333334)
#define TYPICAL_MM_VOLTAGE		(1.00000)

#define TYPICAL_USB_FREQ		(333333334)
#define TYPICAL_USB_VOLTAGE		(1.00000)

#define TYPICAL_SYS_FREQ		(333333334)
#define TYPICAL_SYS_VOLTAGE		(1.00000)

//
//	PMIC Information
//
#define PMIC_TYPE				PMIC_SM5011
#define PMCI_SM5001_STEP		(0.0125)


#define	MIN_VOLTAGE_STEP		PMCI_SM5001_STEP


#define ALL_CPU_WORKING_VOLTAGE		(1.40000)
#define ALL_MM_WORKING_VOLTAGE		(1.40000)
#define ALL_DEVICE_WORKING_VOLTAGE	(1.40000)


//////////////////////////////////////////////////////////////////////////////
//																			//
//						End of PMIC Informations							//
//																			//
//////////////////////////////////////////////////////////////////////////////


double FindNearlestVoltage( double inVoltage, int type = PMIC_TYPE )
{
	double *vTable;
	int maxTables;

	switch (type)
	{
	case PMIC_MP8845:
		vTable = MP8845_VTable;
		maxTables = MP8848_MAX_VTABLE;
		break;
	case PMIC_SM5011:
		vTable = SM5011_VTable;
		maxTables = SM5011_MAX_VTABLE;
		break;
	}

	if( inVoltage > vTable[maxTables-1] )
	{
		return vTable[maxTables-1];
	}
	if( inVoltage < vTable[0] )
	{
		return vTable[0];
	}

	for( int i=0 ; i<maxTables-1 ; i++ )
	{
		if( (vTable[i] <= inVoltage) && (inVoltage <= vTable[i+1]) )
		{
			if( (inVoltage-vTable[i]) <= ( vTable[i+1]-inVoltage) )
			{
				return vTable[i];
			}
			else
			{
				return vTable[i+1];
			}
		}
	}
	return -1.;
}

CASVTest::CASVTest()
	: m_pcbArg(NULL)
	, m_cbEventFunc(NULL)
	, m_cbMessageFunc(NULL)

	, m_bConfig(false)
	, m_pCom(NULL)

	, m_TestModuleId(ASV_MODULE_ID::ASVM_CPU)
	, m_Frequency( 400000000 )
	, m_MinVolt( 0.5 )
	, m_MaxVolt( 1.5 )
	, m_Timeout( 15 )	//	sec

	, m_bChipIdMode(false)

	, m_RxMsgLen(0)
{
	m_hRxSem = CreateSemaphore( NULL, 0, 4096, NULL );
	InitializeCriticalSection( &m_CritRxMsg );
}

CASVTest::~CASVTest()
{
	CloseHandle(m_hRxSem);
	DeleteCriticalSection( &m_CritRxMsg );
}

void CASVTest::RegisterCallback( void *pArg,
					void (*cbEvent)(void *pArg, ASVT_EVT_TYPE evtCode, void *evtData),
					void (*cbMsg)(void *pArg, char *str, int32_t len) )
{
	m_pcbArg = pArg;
	m_cbEventFunc = cbEvent;
	m_cbMessageFunc = cbMsg;
}

void CASVTest::SetTestConfig( ASV_TEST_CONFIG *config, CComPort *pCom )
{
	if( config )
		m_TestConfig = *config;
	if( pCom )
	{
		m_pCom = pCom;
		if( m_pCom )
			m_pCom->SetRxCallback(this, CASVTest::RxComRxCallbackStub );
	}
	m_bConfig = true;
}

bool CASVTest::Start( bool bChipIdMode )
{
	if( !m_bConfig )
	{
		return false;
	}
	m_bChipIdMode = bChipIdMode;
	m_bThreadExit = false;
	_beginthread( (void (*)(void *))FindLVCCThreadStub, 0, this);
	return true;
}

void CASVTest::Stop()
{
	m_bThreadExit = true;
	Sleep( 30 );
}

bool CASVTest::Scan()
{
	if( !HardwareReset() )
	{
		if( m_cbEventFunc )
			m_cbEventFunc( m_pcbArg, ASVT_EVT_ERR, NULL );
		//MessageBox(NULL, TEXT("Scan : Hardware Reset Failed!!!"), TEXT("ERROR!!!"), MB_OK );
		DbgLogPrint(1, "Scan : Hardware Reset Failed!!!" );
		return false;
	}

#if 1
	unsigned int ids_hpm[10];
	unsigned int ecid[4];
	unsigned int hpm = 0;

	if( GetECID(ecid) && GetIDS(ids_hpm) && GetHPM(&ids_hpm[2]) && GetCpuHPM(&hpm) )
	{
		m_cbEventFunc( m_pcbArg, ASVT_EVT_ECID, ecid );
		m_cbEventFunc( m_pcbArg, ASVT_EVT_IDS_HPM, ids_hpm );
		m_cbEventFunc( m_pcbArg, ASVT_EVT_CPUHPM, &hpm );
		return true;
	}
	return false;
#else
	unsigned int ecid[4];
	if( GetECID(ecid) )
	{
		if( m_cbEventFunc )
		{
			m_cbEventFunc( m_pcbArg, ASVT_EVT_ECID, ecid );
			return true;
		}
	}
	return false;
#endif
}

bool CASVTest::TestLowestVolt( ASV_MODULE_ID module, unsigned int freq, unsigned int typical, double volt )
{
	DbgLogPrint(1, "\n[%s] Lowest Voltage %fv, freq = %dHz, typical = %dHz (lowest).\n", ASVModuleIDToString(module), volt, freq, typical );
	//	높은 주파수로 가야 하기 때문에 전압부터 변경
	if( freq > typical )
	{
		if( !SetVoltage(module, volt) )
		{
			HardwareReset();
			return true;
		}
		else
		{
			if( m_bThreadExit )
				return false;
			if( !SetFrequency(module, freq) )
			{
				HardwareReset();
				return true;
			}
			else
			{
				if( !StartTest( module, LVCC_RETRY_COUNT ) )
				{
					HardwareReset();
					return true;
				}
				else
				{
					DbgLogPrint(1, "[Warnning] Lowest Voltage Too High\n");
					return false;
				}
			}
		}
	}
	else
	{
		//	주파수가 typical보다 낮기 때문에 주파수 변경은 무조건 가능 해야함.
		if( !SetFrequency(module, freq) )
		{
			HardwareReset();
			return true;
		}
		else
		{
			if( m_bThreadExit )
				return false;
			if( !SetVoltage(module, volt) )
			{
				HardwareReset();
				return true;
			}
			else
			{
				if( !StartTest( module, LVCC_RETRY_COUNT ) )
				{
					HardwareReset();
					return true;
				}
				else
				{
					DbgLogPrint(1, "[Warnning] Lowest Voltage Too High\n");
					return false;
				}
			}
		}
	}
	return false;
}


// Version 1.0.4 : 0.1 Volt씩 증가하면서 찾는 알고리즘
double CASVTest::FastTestLoop( ASV_MODULE_ID module, ASV_COMMON_PARAM *param, unsigned int frequency, int tmu[2] )
{
	double lowVolt, highVolt, curVolt, bootup, lvcc = -1, hightestVolt, allWorkingVolt;
	int tryCount = 0;
	bool lastSuccess = false;
	unsigned int typicalFreq;

	DbgLogPrint(1, "================ Start %s %dMHz LVCC =====================\n", ASVModuleIDToString(module), frequency/1000000);

	bootup = param->voltTypical;
	lowVolt = param->voltStart;
	highVolt = param->voltEnd;
	hightestVolt = param->voltEnd;

	switch ( module )
	{
	case ASVM_CPU:
		typicalFreq = TYPICAL_CPU_FREQ;
		allWorkingVolt = ALL_CPU_WORKING_VOLTAGE;
		break;
	case ASVM_MM:
		typicalFreq = TYPICAL_MM_FREQ;
		allWorkingVolt = ALL_MM_WORKING_VOLTAGE;
		break;
	case ASVM_USB:
		typicalFreq = TYPICAL_USB_FREQ;
		allWorkingVolt = ALL_DEVICE_WORKING_VOLTAGE;
		break;
	case ASVM_SYS:
		typicalFreq = TYPICAL_SYS_FREQ;
		allWorkingVolt = ALL_DEVICE_WORKING_VOLTAGE;
		break;
	default:
		break;
	}

	//	최소 voltage test를 통해서 전원 설정이 정상인지 확인을 한다.
	//	만약 최소 전원에서 문제가 발생하면 전원 설정에 이상이 발생한 것이니 바로 멈추어야 한다.
	DbgLogPrint(1, "<< Try : %dHz, %fVolt >>\n", frequency, lowVolt);
	if( !TestLowestVolt( module, frequency, typicalFreq, lowVolt ) )
		return lowVolt;

	DbgLogPrint(1, "================ Start 1st Loop =====================\n");
	//	Loop 1 : 0.1 Volt씩 전압을 조종하면서 1차 LVCC를 구한다.
	lowVolt  = FindNearlestVoltage(lowVolt);
	curVolt  = FindNearlestVoltage(lowVolt + 0.1);
	while( curVolt < highVolt)
	{
		if( m_bThreadExit )
			return lvcc;

		DbgLogPrint(1, "<< Try : %dHz, %fVolt >>\n", frequency, curVolt);

		//	for NXP3220 MM Block Test
		if ( ASVM_MM == module )
		{
			//	ignore this command's return value
			SetMMAxiFrequency(module, param->freqOther);
		}

		if( frequency > typicalFreq )
		{
			if( !SetVoltage(module, curVolt) || !SetFrequency(module, frequency) || !StartTest( module, RETRY_COUNT ) )
			{
				HardwareReset();
				lowVolt = curVolt;
			}
			else
			{
				HardwareReset();
				lvcc = highVolt = curVolt;
				break;
			}
		}
		else
		{
			if( !SetFrequency(module, frequency) || !SetVoltage(module, curVolt) || !StartTest( module, RETRY_COUNT ) )
			{
				HardwareReset();
				lowVolt = curVolt;
			}
			else
			{
				HardwareReset();
				lvcc = highVolt = curVolt;
				break;
			}
		}
		curVolt = FindNearlestVoltage(curVolt + 0.1);
	}

	if( lvcc < 0 )
	{
		return lvcc;
	}

	DbgLogPrint(1, "================ Start 2nd Loop(%f~%f) =====================\n", lowVolt, highVolt);
	//	Loop2 : 첫 번째 성공한 전압과 이전 실패한 전압사이에서 MIN_VOLTAGE_STEP Volt 씩 증가하며
	//			최초 성공 값을 구한다.
	curVolt = lowVolt;
	while( curVolt <= highVolt )
	{
		if( m_bThreadExit )
			return lvcc;

		//	for NXP3220 MM Block Test
		if (ASVM_MM == module)
		{
			//	ignore this command's return value
			SetMMAxiFrequency(module, param->freqOther);
		}

		if( frequency > typicalFreq )
		{
			if( !SetVoltage(module, curVolt) || !SetFrequency(module, frequency) || !StartTest( module, RETRY_COUNT ) )
			{
				HardwareReset();
				lowVolt = curVolt;
			}
			else
			{
				HardwareReset();
				lvcc = curVolt;
				break;
			}
		}
		else
		{
			if(!SetFrequency(module, frequency) ||  !SetVoltage(module, curVolt) || !StartTest( module, RETRY_COUNT ) )
			{
				HardwareReset();
				lowVolt = curVolt;
			}
			else
			{
				HardwareReset();
				lvcc = curVolt;
				break;
			}
		}

		curVolt = FindNearlestVoltage(curVolt + MIN_VOLTAGE_STEP);
		if( curVolt >= highVolt )
		{
			break;
		}
	}

	//
	//	이전에 찾아졌던 LVCC에서 Voltage Step 만큼 낮춘 후 다시 Test 
	//	진행성 테스트로 인한 발열로 인하여 LVCC가 높게 나오는 경우에 대한 대비책
	//
	lvcc = FindNearlestVoltage( lvcc - MIN_VOLTAGE_STEP );
	if( lvcc > 0 )
	{
		DbgLogPrint( 1, "\nFound LVCC = %f, Check LVCC Validity\n", lvcc);
		while( !m_bThreadExit )
		{
			if( frequency > typicalFreq )
			{
				SetVoltage( module, allWorkingVolt);
			}
			GetTMUInformation( &tmu[0], NULL );

			//	for NXP3220 MM Block Test
			if (ASVM_MM == module)
			{
				//	ignore this command's return value
				SetMMAxiFrequency(module, param->freqOther);
			}

			if( !SetFrequency( module, frequency ) || !SetVoltage( module, lvcc ) || !StartTest( module, LVCC_RETRY_COUNT ) )
			{
				DbgLogPrint( 1, "LVCC Validity Failed(lvcc=%fvolt), Try next voltage(%fvolt)\n", lvcc, FindNearlestVoltage(lvcc+MIN_VOLTAGE_STEP));
				HardwareReset();
				if( lvcc >= hightestVolt )
				{
					DbgLogPrint( 1, "===> Validation Failed !!! Exit Loop!!\n");
					lvcc = -1;
					break;
				}
				lvcc = FindNearlestVoltage (lvcc + MIN_VOLTAGE_STEP);
			}
			else
			{
				GetTMUInformation( &tmu[1], NULL );
				break;
			}
		}
	}
	DbgLogPrint(1, "================ End %s %dMHz LVCC(%f) =====================\n", ASVModuleIDToStringSimple(module), frequency/1000000, lvcc);
	return lvcc;
}


void CASVTest::FindLVCCThread()
{
	int freqIndex = 0;
	double lvcc = -1;
	int tmu[2];
	unsigned int hpm, coreHpm;
	ASV_EVT_DATA evtData;
	DWORD startTick, endTick;
	unsigned int frequency;

#if ENABLE_CMD_TEST
	TestCommand();
#endif


	if( !Scan() )
	{
		DbgLogPrint(1, "==================== Scan Failed !!! =======================\n");
		if( m_cbEventFunc )
		{
			if( m_cbEventFunc )
				m_cbEventFunc( m_pcbArg, ASVT_EVT_ERR, NULL );
			MessageBox(NULL, 
				TEXT("==============================================\n\n")
				TEXT("\t\tScan Failed!!!!\n\n")
				TEXT("==============================================\n")
				, TEXT("ERROR!!!"), MB_OK );
		}
		return ;
	}

	if( m_bChipIdMode )
		return;

	//	CPU Loop
	frequency = m_TestConfig.arm.freqEnd;
	if( m_TestConfig.enableCpu )
	{
		DbgLogPrint(1, "---------- Start CPU LVCC ----------\n");
		while( !m_bThreadExit && (frequency >= m_TestConfig.arm.freqStart) )
		{
			startTick = GetTickCount();
			HardwareReset();
			lvcc = FastTestLoop( ASVM_CPU, &m_TestConfig.arm, frequency, tmu );
			GetCpuHPM(&hpm);
			//	Find TMU Data
			endTick = GetTickCount();
			if( m_cbEventFunc )
			{
				memset(&evtData, 0, sizeof(evtData));
				evtData.module = ASVM_CPU;
				evtData.frequency = frequency;
				evtData.hpm =  hpm;
				evtData.lvcc = lvcc;
				evtData.time = endTick - startTick;
				evtData.tmuStart = tmu[0];
				evtData.tmuEnd= tmu[1];
				m_cbEventFunc( m_pcbArg, ASVT_EVT_REPORT_RESULT, &evtData );
			}
			frequency -= m_TestConfig.arm.freqStep;
		}
		DbgLogPrint(1, "---------- End CPU LVCC ----------\n");
	}

	//	Device Loop
	frequency = m_TestConfig.mm.freqEnd;
	if (m_TestConfig.enableMM)
	{
		DbgLogPrint(1, "---------- Start MM Block LVCC ----------\n");
		while (!m_bThreadExit && (frequency >= m_TestConfig.mm.freqStart))
		{
			startTick = GetTickCount();
			HardwareReset();
			lvcc = FastTestLoop(ASVM_MM, &m_TestConfig.mm, frequency, tmu);
			GetCpuHPM(&hpm);
			GetCoreHPM(&coreHpm);
			endTick = GetTickCount();
			if (m_cbEventFunc)
			{
				memset(&evtData, 0, sizeof(evtData));
				evtData.module = ASVM_MM;
				evtData.frequency = frequency;
				evtData.hpm = hpm;
				evtData.coreHpm = coreHpm;
				evtData.lvcc = lvcc;
				evtData.time = endTick - startTick;
				evtData.tmuStart = tmu[0];
				evtData.tmuEnd = tmu[1];
				//	write freqOther ( AXI Clock ) Information using reserved area.
				evtData.reserved[0] = m_TestConfig.mm.freqOther;
				m_cbEventFunc(m_pcbArg, ASVT_EVT_REPORT_RESULT, &evtData);
			}
			frequency -= m_TestConfig.mm.freqStep;
		}
		DbgLogPrint(1, "---------- End MM Block LVCC ----------\n");
	}

	//	System Bus Loop
	frequency = m_TestConfig.bus.freqEnd;
	if (m_TestConfig.enableSysBus)
	{
		DbgLogPrint(1, "---------- Start System Bus Block LVCC ----------\n");
		while (!m_bThreadExit && (frequency >= m_TestConfig.bus.freqStart))
		{
			startTick = GetTickCount();
			HardwareReset();
			lvcc = FastTestLoop(ASVM_SYS, &m_TestConfig.bus, frequency, tmu);
			GetCpuHPM(&hpm);
			GetCoreHPM(&coreHpm);
			endTick = GetTickCount();
			if (m_cbEventFunc)
			{
				memset(&evtData, 0, sizeof(evtData));
				evtData.module = ASVM_SYS;
				evtData.frequency = frequency;
				evtData.hpm = hpm;
				evtData.coreHpm = coreHpm;
				evtData.lvcc = lvcc;
				evtData.time = endTick - startTick;
				evtData.tmuStart = tmu[0];
				evtData.tmuEnd = tmu[1];
				m_cbEventFunc(m_pcbArg, ASVT_EVT_REPORT_RESULT, &evtData);
			}
			frequency -= m_TestConfig.bus.freqStep;
		}
		DbgLogPrint(1, "---------- End MM Block LVCC ----------\n");
	}


	if( m_cbEventFunc )
		m_cbEventFunc( m_pcbArg, ASVT_EVT_DONE, &evtData );
	_endthread();
}

bool FindLineFeed( char *str, int size, int *pos )
{
	int i=0;
	for( ; i<size ; i++ )
	{
		if( str[i] == '\n' )
		{
			*pos = i+1;
			return true;
		}
	}
	return false;
}

bool CASVTest::HardwareReset()
{
	DbgLogPrint(1, "\nReset Hardware!!!\n");

#if 0
	//	Reset H/W
	m_pCom->ClearDTR();
	Sleep(HARDWARE_RESET_TIME);
	m_pCom->SetDTR();
	DbgLogPrint(1, "Wait Hardware Booting Message....\n");
	if( !WaitBootOnMessage() )
	{
		return false;
	}
	return true;
#else
	int retry = RESET_RETRY_COUNT;
	//	Reset H/W
	m_pCom->ClearDTR();
	Sleep(HARDWARE_RESET_TIME);
	m_pCom->SetDTR();
	DbgLogPrint(1, "Wait Hardware Booting Message....\n");

	do
	{
		if(!WaitBootOnMessage())
		{
			retry --;
			m_pCom->ClearDTR();
			Sleep(HARDWARE_RESET_TIME);
			m_pCom->SetDTR();
		}
		else{
			return true;
		}
	} while(retry);

	return true;
#endif
}


bool CASVTest::WaitBootOnMessage()
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.resetTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "BOOT DONE", 9 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "BOOT FAIL", 9 ) )
				{
					if( m_cbMessageFunc )
					{
						char s[128];
						sprintf( s, "Boot Fail");
						m_cbMessageFunc( m_pcbArg, s, strlen(s));
						m_RxMessage[m_RxMsgLen] = '\n';
						m_RxMessage[m_RxMsgLen+1] = '\0';
						m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
					}
					m_RxMsgLen=0;
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "WaitBootOnMessage : Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
}


bool CASVTest::CheckCommandResponse()
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "CheckCommandResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
}

bool CASVTest::GetECID(unsigned int ecid[4])
{
	char cmdBuf[MAX_CMD_STR];
	memset( cmdBuf, 0, sizeof(cmdBuf) );

	ASV_PARAM param = {0};
	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_ECID, ASVM_CPU, param );

	//	Send Command
	if( m_pCom )
	{
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return GetECIDCmdResponse(ecid);
}


bool CASVTest::GetECIDCmdResponse( unsigned int ecid[4] )
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{
					CHIP_INFO chipInfo;
					ParseECID( ecid, &chipInfo );
					sscanf( m_RxMessage, "SUCCESS : ECID=%x-%x-%x-%x\n", &ecid[0], &ecid[1], &ecid[2], &ecid[3] );
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "GetECIDCmdResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
	return true;
}

bool CASVTest::SetFrequency( ASV_MODULE_ID module, unsigned int frequency )
{
	char cmdBuf[MAX_CMD_STR];
	//	Set Target Voltage
	ASV_PARAM param;
	param.u32 = frequency;
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_SET_FREQ, module, param );

	//	Send Command
	if( m_pCom )
	{
		DbgLogPrint(1, "Set Frequency (%dMHz)\n", frequency/1000000 );
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return CheckCommandResponse();
}

bool CASVTest::SetMMAxiFrequency(ASV_MODULE_ID module, unsigned int frequency)
{
	char cmdBuf[MAX_CMD_STR];
	//	Set Target Voltage
	ASV_PARAM param;
	param.u32 = frequency;
	memset(cmdBuf, 0, sizeof(cmdBuf));
	MakeCommandString(cmdBuf, sizeof(cmdBuf), ASVC_SET_AXI_FREQ, module, param);
	DbgLogPrint(1, "cmd > : %s\n", cmdBuf);

	//	Send Command
	if (m_pCom)
	{
		DbgLogPrint(1, "Set MM AXI Frequency (%dMHz)\n", frequency / 1000000);
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return CheckCommandResponse();
}

bool CASVTest::SetVoltage( ASV_MODULE_ID module, double voltage )
{
	char cmdBuf[MAX_CMD_STR];
	//	Set Target Voltage
	ASV_PARAM param;
	bool ret;
	param.f64 = voltage;
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_SET_VOLT, module, param );

	//	Send Command
	if( m_pCom )
	{
		DbgLogPrint(1, "Set Voltage (%f volt)\n", voltage );
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}
	ret = CheckCommandResponse();

	Sleep( 3000 );
	return ret;
}

bool CASVTest::StartTest( ASV_MODULE_ID module, int retryCnt )
{
	int i=0;
	int max = retryCnt;
	do{
		DbgLogPrint(1, "Start %s Module Test( Try Count = %d/%d )\n", ASVModuleIDToStringSimple(module), ++i, max );
		char cmdBuf[MAX_CMD_STR];
		ASV_PARAM param;
		memset( cmdBuf, 0, sizeof(cmdBuf) );
		param.u64 = 0;
		MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_RUN, module, param );
		//	Send Command
		if( m_pCom )
		{
			m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
		}
		if( true != CheckCommandResponse() )
		{
			return false;
		}
		retryCnt --;
	}while( retryCnt > 0 && !m_bThreadExit );

	return true;
}


//
//	Low / High
//
double CASVTest::SelectNextVoltage( double low, double high, double step )
{
	int numStep;

	if( low == high || ((high-low)<(step*1.5)) )
		return -1;

	numStep = (int)((high - low)/step);
	if( numStep > 3 )
	{
		numStep /= 2;
		return (high - numStep*step);
	}
	return ((high - step) > low)?high-step : -1;
}

void CASVTest::RxComRxCallback( char *buf, int len )
{
	//	Send Event
	CNXAutoLock lock(&m_CritRxMsg);
	memcpy( m_RxMessage + m_RxMsgLen, buf, len );
	m_RxMsgLen += len;
	ReleaseSemaphore( m_hRxSem, 2, &m_nSem);
	DbgMsg( TEXT("m_nSem = %d\n"), m_nSem );
	if( m_cbMessageFunc )
	{
		if( len > 0 )
			m_cbMessageFunc( m_pcbArg, buf, len );
	}
}

void CASVTest::DbgLogPrint( int flag, char *fmt,... )
{
	va_list ap;
	va_start(ap,fmt);
	vsnprintf( m_DbgStr, 4095, fmt, ap );
	va_end(ap);
	if( m_cbMessageFunc )
	{
		m_cbMessageFunc( m_pcbArg, m_DbgStr, 0 );
	}
}

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

//	96~111	96	"0001011100100000" fix
//	112~127	112	"0010110001001000" fix

unsigned int ConvertMSBLSB( unsigned int data, int bits )
{
	unsigned int result = 0;
	unsigned int mask = 1;

	int i=0;
	for( i=0; i<bits ; i++ )
	{
		if( data&(1<<i) )
		{
			result |= mask<<(bits-i-1);
		}
	}
	return result;
}

void CASVTest::ParseECID( unsigned int ECID[4], CHIP_INFO *chipInfo)
{
	//	Read GUID
	chipInfo->lotID			= ConvertMSBLSB( ECID[0] & 0x1FFFFF, 21 );
	chipInfo->waferNo		= ConvertMSBLSB( (ECID[0]>>21) & 0x1F, 5 );
	chipInfo->xPos			= ConvertMSBLSB( ((ECID[0]>>26) & 0x3F) | ((ECID[1]&0x3)<<6), 8 );
	chipInfo->yPos			= ConvertMSBLSB( (ECID[1]>>2) & 0xFF, 8 );
	chipInfo->ids			= ConvertMSBLSB( (ECID[1]>>16) & 0xFF, 8 );
	chipInfo->ro				= ConvertMSBLSB( (ECID[1]>>24) & 0xFF, 8 );
	chipInfo->usbProductId	= ECID[3] & 0xFFFF;
	chipInfo->usbVendorId	= (ECID[3]>>16) & 0xFFFF;
}


//
//	TMU Data
//

bool CASVTest::GetTMUResponse( int *pTMU )
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{
					sscanf( m_RxMessage, "SUCCESS : TMU=%d\n", pTMU );
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "GetTMUResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
	return true;
}

bool CASVTest::GetTMUInformation( int *pTMU0, int *pTMU1 )
{
	char cmdBuf[MAX_CMD_STR];
	//	Set Target Voltage
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	ASV_PARAM param = {0};

	if( pTMU0 )
	{
		//	Get TMU 0 Value
		MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_TMU0, ASVM_CPU, param );
		//	Send Command
		if( m_pCom )
		{
			m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
		}
		if( !GetTMUResponse(pTMU0) )
		{
			return false;
		}
	}

	if( pTMU1 )
	{
		//	Get TMU 0 Value
		MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_TMU1, ASVM_CPU, param );
		//	Send Command
		if( m_pCom )
		{
			m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
		}
		if( !GetTMUResponse(pTMU1) )
		{
			return false;
		}
	}
	return true;
}


//	Get IDS
bool CASVTest::GetIDS(unsigned int IDS[2])
{
	char cmdBuf[MAX_CMD_STR];
	//	Set Target Voltage
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	ASV_PARAM param = {0};

	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_IDS, ASVM_CPU, param );
	if( m_pCom )
	{
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return GetIDSResponse( IDS );
}

bool CASVTest::GetIDSResponse( unsigned int IDS[2] )
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{
					sscanf( m_RxMessage, "SUCCESS : IDS=%02x-%02x\n", &IDS[0], &IDS[1] );
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "GetIDSResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
	return true;
}


//	Get RO
bool CASVTest::GetHPM( unsigned int hpm[8] )
{
	char cmdBuf[MAX_CMD_STR];
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	ASV_PARAM param = {0};

	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_HPM, ASVM_CPU, param );
	if( m_pCom )
	{
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return GetHPMResponse( hpm );
}

bool CASVTest::GetHPMResponse( unsigned int hpm[8] )
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{
				sscanf( m_RxMessage, "SUCCESS : HPM=%03x-%03x-%03x-%03x-%03x-%03x-%03x-%03x\n",
							&hpm[0], &hpm[1], &hpm[2], &hpm[3], &hpm[4], &hpm[5], &hpm[6], &hpm[7] );
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "GetIDSResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
	return true;
}

bool CASVTest::GetCpuHPM(unsigned int *hpm)
{
	char cmdBuf[MAX_CMD_STR];
	memset( cmdBuf, 0, sizeof(cmdBuf) );
	ASV_PARAM param = {0};

	MakeCommandString( cmdBuf, sizeof(cmdBuf), ASVC_GET_CPUHPM, ASVM_CPU, param );
	if( m_pCom )
	{
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return GetHPMRUNCPUResponse( hpm );
}

bool CASVTest::GetHPMRUNCPUResponse(unsigned int *hpm)
{
	DWORD waitResult;
	int pos;
	do{
		waitResult = WaitForSingleObject( m_hRxSem, m_TestConfig.testTimeout*1000 );
		if( WAIT_OBJECT_0 == waitResult )
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if( FindLineFeed( m_RxMessage, m_RxMsgLen, &pos ) )
			{
				if( !strncmp( m_RxMessage, "SUCCESS", 7 ) )
				{

					sscanf( m_RxMessage, "SUCCESS : HPM=%03x\n",hpm);
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return true;
				}
				else if( !strncmp( m_RxMessage, "FAIL", 4 ) )
				{
					m_RxMsgLen-=pos;
					memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
					return false;
				}
				m_RxMsgLen-=pos;
				memmove( m_RxMessage, m_RxMessage + pos, m_RxMsgLen );
				continue;
			}
		}
		else if( WAIT_TIMEOUT == waitResult )
		{
			if( m_cbMessageFunc )
			{
				char s[128];
				sprintf( s, "GetCPUHPMResponse : Response Timeout");
				m_cbMessageFunc( m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen+1] = '\0';
				m_cbMessageFunc( m_pcbArg, m_RxMessage, 0 );
			}
			return false;
		}
	}while(1);
	return true;
}


bool CASVTest::GetCoreHPM(unsigned int* hpm)
{
	char cmdBuf[MAX_CMD_STR];
	memset(cmdBuf, 0, sizeof(cmdBuf));
	ASV_PARAM param = { 0 };

	MakeCommandString(cmdBuf, sizeof(cmdBuf), ASVC_GET_CORE_HPM, ASVM_SYS, param);
	if (m_pCom)
	{
		m_pCom->WriteData(cmdBuf, strlen(cmdBuf));
	}

	return GetHPMRunSysResponse(hpm);
}

bool CASVTest::GetHPMRunSysResponse(unsigned int* hpm)
{
	DWORD waitResult;
	int pos;
	do {
		waitResult = WaitForSingleObject(m_hRxSem, m_TestConfig.testTimeout * 1000);
		if (WAIT_OBJECT_0 == waitResult)
		{
			CNXAutoLock lock(&m_CritRxMsg);
			if (FindLineFeed(m_RxMessage, m_RxMsgLen, &pos))
			{
				if (!strncmp(m_RxMessage, "SUCCESS", 7))
				{

					sscanf(m_RxMessage, "SUCCESS : CORE HPM=%03x\n", hpm);
					m_RxMsgLen -= pos;
					memmove(m_RxMessage, m_RxMessage + pos, m_RxMsgLen);
					return true;
				}
				else if (!strncmp(m_RxMessage, "FAIL", 4))
				{
					m_RxMsgLen -= pos;
					memmove(m_RxMessage, m_RxMessage + pos, m_RxMsgLen);
					return false;
				}
				m_RxMsgLen -= pos;
				memmove(m_RxMessage, m_RxMessage + pos, m_RxMsgLen);
				continue;
			}
		}
		else if (WAIT_TIMEOUT == waitResult)
		{
			if (m_cbMessageFunc)
			{
				char s[128];
				sprintf(s, "GetCoreHPMResponse : Response Timeout");
				m_cbMessageFunc(m_pcbArg, s, strlen(s));
				m_RxMessage[m_RxMsgLen] = '\n';
				m_RxMessage[m_RxMsgLen + 1] = '\0';
				m_cbMessageFunc(m_pcbArg, m_RxMessage, 0);
			}
			return false;
		}
	} while (1);
	return true;
}




void CASVTest::TestCommand()
{
	double lvcc = -1;
	int tmu[2];
	unsigned int frequency;
	frequency = m_TestConfig.mm.freqEnd;
	lvcc = FastTestLoop(ASVM_MM, &m_TestConfig.mm, frequency, tmu);
	MessageBox(NULL, TEXT("TestMode"), TEXT("Test Mode Ended : exit program!!!"), MB_OK);
	exit(1);
}