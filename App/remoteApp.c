#include "remoteApp.h"

//*************************************************************************
//
//ModuleStatus 指针中各个有效位具体码值
//
//*************************************************************************
typedef	enum
{
	HB_80 = 0x01,
	HG_300 = 0x02,
}MachineType;

//Hekr模块状态码
typedef	enum
{
	STA_Mode = 0x01,
	HekrConfig_Mode = 0x02,
	AP_Mode = 0x03,
	STA_AP_Mode = 0x04,
	RF_OFF_Mode = 0x05
}HekrModuleWorkCode;

//Hekr WIFI状态码
typedef	enum
{
	RouterConnected = 0x01,
	RouterConnectedFail = 0x02,
	RouterConnecting = 0x03,
	PasswordErr = 0x04,
	NoRouter = 0x05,
	RouterTimeOver = 0x06
}HekrModuleWIFICode;

//Hekr Cloud状态码
typedef	enum
{
	CloudConnected = 0x01,
	DNS_Fail = 0x02,
	CloudTimeOver = 0x03
}HekrModuleCloudCode;


static byte RxValidDataBuffer[WIFIUSER_MAX_LEN]={0};
static byte TxValidDataBuffer[WIFIUSER_MAX_LEN]={0};
static byte ModuleStatus[8]={0};
static WifiTransDataTypeDef WifiTrans;
static uint SysFaultBuffer=0;




static void AppSysParaUpload(void)
{
	byte index=0;
//	byte* pdes=TxValidDataBuffer;
	
	TxValidDataBuffer[index++] =SysParaUpload; //4  0
	
	
	if(App.SysCtrlPara.Power != POWER_OFF)
		TxValidDataBuffer[index++] = POWER_ON;   	//6  2
	else
		TxValidDataBuffer[index++] = POWER_OFF;   	//6  2
	
	TxValidDataBuffer[index++] = App.SysCtrlPara.MuteSet;   //8  3
	TxValidDataBuffer[index++] = App.SysCtrlPara.ChildLock;   //13  4
	
	TxValidDataBuffer[index++] = App.SysCtrlPara.CircleModeSet;   //9  5
//	*pdes++= App.SysCtrlPara.HeatModeSet;   //10  6
	TxValidDataBuffer[index++]= 1;   //10  6
	
	TxValidDataBuffer[index++] = (DISPFLOW_MAX>>8);  //  7
	TxValidDataBuffer[index++] = (DISPFLOW_MAX&0xFF);  //  7
	TxValidDataBuffer[index++] = (DISPFLOW_MIN+DISPFLOW_ONESTEP)>>8;		//  8
	TxValidDataBuffer[index++] = (DISPFLOW_MIN+DISPFLOW_ONESTEP)&0xFF;		//  8
	TxValidDataBuffer[index++] = DISPFLOW_ONESTEP;		//  9
	TxValidDataBuffer[index++] = App.SysCtrlPara.AirFlowRun;			//  10
//	*pdes++ = App.SysCtrlPara.VentilateRate;   //11  11
	TxValidDataBuffer[index++] = 1;   //11  11
	
	TxValidDataBuffer[index++] = App.SensorData.TempOutside+40;   //18  12
	TxValidDataBuffer[index++] = App.SensorData.TempInside+40;   //16  13
	
#ifdef __SENSOR_RH
	TxValidDataBuffer[index++] = App.SensorData.RHInside;   //17  14
#else
	TxValidDataBuffer[index++] =0xFF;
#endif
	
#ifdef __SENSOR_CO2
  if(App.SensorData.CO2Inside<400)
	{
		TxValidDataBuffer[index++] = (400>>8);   //19  15
		TxValidDataBuffer[index++] = (400&0xFF);   //20  16
	}
	else
	{
		TxValidDataBuffer[index++] = (App.SensorData.CO2Inside>>8);   //19  15
		TxValidDataBuffer[index++] = (App.SensorData.CO2Inside&0xFF);   //20  16
	}
#else
	TxValidDataBuffer[index++] =0xFF;
	TxValidDataBuffer[index++] =0xFF;
#endif
	
#ifdef __SENSOR_PM25
	TxValidDataBuffer[index++] = App.SensorData.PMInside>>8;   //21  17
	TxValidDataBuffer[index++] = App.SensorData.PMInside&0xFF;   //22  18
#else
	TxValidDataBuffer[index++] =0xFF;
	TxValidDataBuffer[index++] =0xFF;
#endif

	if(App.SysCtrlPara.ShutTimer)
	{
		TxValidDataBuffer[index++] = 1;   //7  19
		TxValidDataBuffer[index++] = App.SysCtrlPara.ShutTimer;   //12  20
	}
	else
	{
		TxValidDataBuffer[index++] = 0;   //7  3
		TxValidDataBuffer[index++] = 1;   //7  3
	}
		TxValidDataBuffer[index++] = App.SysCtrlPara.AuxiliaryHeatSet;   //7  3

   
	 WifiTrans = 	SysParaUpload;
   System.Device.Wifi.HekrValidDataUpload(index);	
}


static void AppEnvParaSet(void)
{
	byte* psrc=RxValidDataBuffer;
	byte val;

	psrc++ ; //5
	App.SysCtrlLine.TempInsideSummerLine= *psrc++; //6
	App.SysCtrlLine.TempInsideWinterLine= *psrc++;   	//7
	val = *psrc++;
	switch(val)
	{
		case 1:
			App.SysCtrlLine.PMInsideLine= 100;   //8
			break;
		case 2:
			App.SysCtrlLine.PMInsideLine= 150;   //8
			break;
		case 3:
			App.SysCtrlLine.PMInsideLine= 200;   //8
			break;
		case 4:
			App.SysCtrlLine.PMInsideLine= 250;   //8
			break;
		default:
			break;
	}
	val = *psrc++;
	switch(val)
	{
		case 1:
			 App.SysCtrlLine.CO2InsideLine= 800;   //9
			break;
		case 2:
			 App.SysCtrlLine.CO2InsideLine= 1000;   //9
			break;
		case 3:
			 App.SysCtrlLine.CO2InsideLine= 1200;   //9
			break;
		default:
			break;
	}
}

static void AppEnvParaUpload(void)
{
	byte index=0;

	TxValidDataBuffer[index++] =EnvParaUpload; //5  0
	TxValidDataBuffer[index++] =App.SysCtrlLine.TempInsideSummerLine; //6 1
	TxValidDataBuffer[index++] = App.SysCtrlLine.TempInsideWinterLine;   	//7  2
	switch(App.SysCtrlLine.PMInsideLine)
	{
		case 100:
			TxValidDataBuffer[index++] = 1;   //8  3
			break;
		case 150:
			TxValidDataBuffer[index++] = 2;   //8
			break;
		case 200:
			TxValidDataBuffer[index++] = 3;   //8
			break;
		case 250:
			TxValidDataBuffer[index++] = 4;   //8
			break;
		default:
			TxValidDataBuffer[index++] = 0;
			break;
	}
	switch(App.SysCtrlLine.CO2InsideLine)
	{
		case 800:
			TxValidDataBuffer[index++] = 1;   //9  4
			break;
		case 1000:
			TxValidDataBuffer[index++] = 2;   //9
			break;
		case 1200:
			TxValidDataBuffer[index++] = 3;   //9
			break;
		default:
			TxValidDataBuffer[index++] = 0;
			break;
	}
  
  System.Device.Wifi.HekrValidDataUpload(index);	
	WifiTrans = 	EnvParaUpload;
}

static void AppFilterSet(void)
{
	byte* psrc=RxValidDataBuffer;
	ushort val;
	
	psrc++ ; //5
	val = *psrc++;
	val <<=8;
	val |= *psrc++;
	App.FilterHourLimit.FirstFilterHourLimit = val; //6
	val = *psrc++;
	val <<=8;
	val |= *psrc++;
	App.FilterHourLimit.MidFilterHourLimit = val;   	//7
	val = *psrc++;
	val <<=8;
	val |= *psrc++;
	App.FilterHourLimit.ESPFilterHourLimit = val;   //8
	val = *psrc++;
	val <<=8;
	val |= *psrc;
	App.FilterHourLimit.HPFilterHourLimit = val;   //9
  StorePost(STORE_FILTERLIMIT);
}

static void AppFilterUpload(void)
{
	byte index=0;
	
	TxValidDataBuffer[index++] =FilterUpload; //5
	TxValidDataBuffer[index++] =App.FilterHourLimit.FirstFilterHourLimit>>8; //6
	TxValidDataBuffer[index++] =App.FilterHourLimit.FirstFilterHourLimit&0xff; //7
	TxValidDataBuffer[index++] = App.FilterHourLimit.MidFilterHourLimit>>8;   	//8
	TxValidDataBuffer[index++] = App.FilterHourLimit.MidFilterHourLimit&0xff;   	//9
	TxValidDataBuffer[index++] = App.FilterHourLimit.ESPFilterHourLimit>>8;   //10
	TxValidDataBuffer[index++] = App.FilterHourLimit.ESPFilterHourLimit&0xff;   //11
	TxValidDataBuffer[index++] = App.FilterHourLimit.HPFilterHourLimit>>8;   //12
	TxValidDataBuffer[index++] = App.FilterHourLimit.HPFilterHourLimit&0xff;   //13
	
	TxValidDataBuffer[index++] = App.FilterTimerHour.FirstFilterTimerHour>>8;   //14
	TxValidDataBuffer[index++] = App.FilterTimerHour.FirstFilterTimerHour&0xff;   //15
	TxValidDataBuffer[index++] = App.FilterTimerHour.MidFilterTimerHour>>8;   //16
	TxValidDataBuffer[index++] = App.FilterTimerHour.MidFilterTimerHour&0xff;   //17
	TxValidDataBuffer[index++] = App.FilterTimerHour.ESPFilterTimerHour>>8;   //18
	TxValidDataBuffer[index++] = App.FilterTimerHour.ESPFilterTimerHour&0xff;   //19
	TxValidDataBuffer[index++] = App.FilterTimerHour.HPFilterTimerHour>>8;   //20
	TxValidDataBuffer[index++] = App.FilterTimerHour.HPFilterTimerHour&0xff;   //21
  
	WifiTrans = 	FilterUpload;
  System.Device.Wifi.HekrValidDataUpload(index);	
}

//static void AppDevTimeSet(void)
//{
//	byte* psrc=RxValidDataBuffer;
//	byte* pdes;
//	byte i;
//	
//	psrc++ ; //5
//	pdes =(byte*)&App.SysTime;
//	for(i=0;i<14;i++)
//	  *pdes++ = *psrc++;
//  
//}

static void AppSYSFaultReport(void)
{
	uint val=0x10;
	byte i=0;
	byte index=0;
//	byte* pdes=TxValidDataBuffer;

	SysFaultBuffer = App.SysState.FaultFlag;
	TxValidDataBuffer[index++] =SYSFaultReport; //5
	
	val=POWERBASE_FAULT;
	for(i=0;i<8;i++)
	{
		if(SysFaultBuffer&val)
			TxValidDataBuffer[index++] =1;
		else
			TxValidDataBuffer[index++] =0;
		val <<=1;
	}
	val = FIRSTFILTER_CHECK;
	for(i=0;i<4;i++)
	{
		if(SysFaultBuffer&val)
			TxValidDataBuffer[index++] =1;
		else
			TxValidDataBuffer[index++] =0;
		val <<=1;
	}
#ifdef  __SENSOR_CO2
	val = CO2INSIDEBEYOND;
	if(SysFaultBuffer&val)
		TxValidDataBuffer[index++] =1;
	else
#endif
		TxValidDataBuffer[index++] =0;
	
#ifdef  __SENSOR_PM25
	val = PMINSIDEBEYONG;
	if(SysFaultBuffer&val)
		TxValidDataBuffer[index++] =1;
	else
#endif
		TxValidDataBuffer[index++] =0;
	
	val = HGA_FAULT;
	if(SysFaultBuffer&val)
		TxValidDataBuffer[index++] =1;
	else
		TxValidDataBuffer[index++] =0;
	
	WifiTrans = 	SYSFaultReport;
  System.Device.Wifi.HekrValidDataUpload(index);	
}

static void AppHistoryGraghDataReport(void)
{
	byte* pdes=TxValidDataBuffer;

	*pdes++ =HistoryGraghDataReport; //5
	*pdes++ = (App.SensorData.CO2Inside>>8);   //6
	*pdes++ = (App.SensorData.CO2Inside&0xFF);   //7
	*pdes++ = App.SensorData.PMInside>>8;   //8
	*pdes++ = App.SensorData.PMInside&0xFF;   //9

	WifiTrans = 	HistoryGraghDataReport;
  System.Device.Wifi.HekrValidDataUpload(5);	
}


void WifiRecvParse(byte type)
{
	switch(type)
	{
		case RecvDataSumCheckErr:
			break;
		case LastFrameSendErr:
			break;
		case MCU_UploadACK:
			WifiTrans = TRANSFINISH;
			break;
		case ValidDataUpdate:
			switch(RxValidDataBuffer[0])
			{
				case SysParaQuery:
					AppSysParaUpload();
					break;
				case DevPowerSet:
					App.SysCtrlPara.Power =(PowerSetTypedef)RxValidDataBuffer[1];   	//7
					PostMessage(MessageParaUpdate, PARA_POWER_SET);
					break;
				case DevTimeOffSet:
						if(RxValidDataBuffer[1])
						{
							App.SysCtrlPara.ShutTimer = (byte)RxValidDataBuffer[2];
						}
						else
							App.SysCtrlPara.ShutTimer = 0;
							
						if(App.SysCtrlPara.ShutTimer<10)
								PostMessage(MessageParaUpdate, PARA_WORKMODE);
						else
							App.SysCtrlPara.ShutTimer=0;
					break;
				case DevCircleModeSet:
					App.SysCtrlPara.CircleModeSet =(CircleModeSetTypedef)RxValidDataBuffer[1];   //10
					PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
					break;
				case DevMuteSet:
					App.SysCtrlPara.MuteSet =(MutSetTypedef)RxValidDataBuffer[1];   //9
					PostMessage(MessageParaUpdate, PARA_MUTEMODE);
					break;
				case DevChildLockSet:
					App.SysCtrlPara.ChildLock =(ChildLockTypedef)RxValidDataBuffer[1];   //14
					PostMessage(MessageParaUpdate, PARA_CHILDLOCK);
					break;
				case DevHeatModeSet:
//					App.SysCtrlPara.HeatModeSet =(HeatModeTypedef)RxValidDataBuffer[1];   //11
//					PostMessage(MessageParaUpdate, PARA_THERMALMODE);
					break;
				case DevVentilateRateSet:
//					App.SysCtrlPara.VentilateRate =(VentilateRateTypedef)RxValidDataBuffer[1];   //12
//					PostMessage(MessageParaUpdate, PARA_VENTILATE);
					break;
				case DevAirflowSet:
						App.SysCtrlPara.AirFlowSet =(byte)RxValidDataBuffer[1];  
				    if(App.SysCtrlPara.MuteSet == MUTEMODE_ACT)
						{
							App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
							PostMessage(MessageParaUpdate, PARA_MUTEMODE);
						}
						else
						{
							App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
							PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
						}
					break;
				case DevElcHeatSet:
					App.SysCtrlPara.AuxiliaryHeatSet =(AuxiliaryHeatTypedef)RxValidDataBuffer[1];   //8
					PostMessage(MessageParaUpdate, PARA_HEATSET);
					break;
//				case DevTimeSet:
//					AppDevTimeSet();				
//					break;
//				case WeekTimerQuery:
//					AppWeekTimerUpload();
//					break;
//				case WeekTimerSet:
//					AppWeekTimerSet();
//	StorePost(STORE_WEEKTIMER);
//	PostMessage(MessageAlarm, ALARM_INIT);
//					break;
//				case TimerModeQuery:
//					AppTimerModeUpload(RxValidDataBuffer[1]);
//					break;
//				case TimerModeSet:
//					AppTimerModeSet();
//	PostMessage(MessageAlarm, ALARM_INIT);
//					break;
				case EnvParaQuery:
					AppEnvParaUpload();
					break;
				case EnvParaSet:
					AppEnvParaSet();
  StorePost(STORE_ENVPARA);
					break;
				case FilterQuery:
					AppFilterUpload();
					break;
				case FilterLimitSet:
					AppFilterSet();
				PostMessage(MessageProcess, FILTER_CHECK);
					break;
				default:
					break;
			}
			break;
		case RecvDataUseless:
			break;
		case HekrModuleStateUpdate:
			switch(ModuleStatus[1])
			{
				case STA_Mode:
					App.SysState.WifiState &= ~WIFI_STATE;
				if(ModuleStatus[3]==CloudConnected)
					App.SysState.WifiState |= WIFI_STATE_STA;
				else
					App.SysState.WifiState |= WIFI_STATE_STA_AP;
					break;
				case HekrConfig_Mode:
					App.SysState.WifiState &= ~WIFI_STATE;
					App.SysState.WifiState |= WIFI_STATE_CONFIG;
					break;
				case AP_Mode:
					break;
				case STA_AP_Mode:
					App.SysState.WifiState &= ~WIFI_STATE;
					App.SysState.WifiState |= WIFI_STATE_STA_AP;
					break;
				default:
					break;
			}
			/* HEKR_TEST_V4 Router Found, Module Signal Receive Normal*/
//			if(ModuleStatus[5]==0x01)
//				System.Device.Led.LedModeSet(LED_GREEN,LED_TWINKLE);
			
			
			break;
		case MCU_ControlModuleACK:
			break;
		default:
			break;
	}
}

void WifiCtrlCode(byte code)
{
	switch(code)
	{
		case ModuleQuery:
			System.Device.Wifi.HekrModuleControl(ModuleQuery);
			break;
		case ModuleRestart:
			System.Device.Wifi.HekrModuleControl(ModuleRestart);
			break;
		case ModuleRecover:
			System.Device.Wifi.HekrModuleControl(ModuleRecover);
			App.SysState.WifiState = 0;
			break;
		case HekrConfig:
			System.Device.Wifi.HekrModuleControl(HekrConfig);
			App.SysState.WifiState &= ~WIFI_STATE;
			App.SysState.WifiState |= WIFI_STATE_CONFIG;
			break;
		case HekrProdKey:
			break;
	}
}

void WifiUpload(void)
{
	byte i;
	uint val=0x80000000;
	
	if(WifiTrans == TRANSFINISH)
	{
		if(SysFaultBuffer != App.SysState.FaultFlag)
			AppSYSFaultReport();
	
		for(i=0;i<16;i++)
		{
			if(App.SysState.WifiState&val)
				break;
			else
				val >>= 1;
		}		

		switch(i)
		{
			case 5:
				AppSysParaUpload();
				App.SysState.WifiState &= ~WIFI_UPLOAD_SYSPARA;
				WifiTrans = SysParaUpload;
				break;
			case 6:
//				AppWeekTimerUpload();
//				App.SysState.WifiState &= ~WIFI_UPLOAD_WEEKTIMER;
//				WifiTrans = WeekTimerUpload;
				break;
			case 7:
				AppEnvParaUpload();
				App.SysState.WifiState &= ~WIFI_UPLOAD_ENVPARA;
				WifiTrans = EnvParaUpload;
				break;
			case 8:
				AppFilterUpload();
				App.SysState.WifiState &= ~WIFI_UPLOAD_FILTER;
				WifiTrans = FilterUpload;
				break;
			case 9:
				AppHistoryGraghDataReport();
				App.SysState.WifiState &= ~WIFI_UPLOAD_GRAPHDATA;
				WifiTrans = HistoryGraghDataReport;
				break;
			case 10:
				if(SysFaultBuffer != App.SysState.FaultFlag)
				{
					AppSYSFaultReport();
					WifiTrans = SYSFaultReport;
				}
				App.SysState.WifiState &= ~WIFI_UPLOAD_FAULT;
				break;
			default :
				break;
		}
	}
	else
	{
		switch(WifiTrans)
		{
			case SysParaUpload:
				AppSysParaUpload();
				break;
			case WeekTimerUpload:
//				AppWeekTimerUpload();
				break;
			case EnvParaUpload:
				AppEnvParaUpload();
				break;
			case FilterUpload:
				AppFilterUpload();
				break;
			case HistoryGraghDataReport:
				AppHistoryGraghDataReport();
				break;
			default :
				WifiTrans = TRANSFINISH;
				break;
		}
	}
}

static void WifiSystick10(void)
{
	static byte second=0;
	static ushort graphtimer=0;
	
	if(++second>9)   //per 1s
	{
		second=0;
		if((App.SysState.WifiState&WIFI_STATE_STA)&&(App.SysState.WifiState&0x07E00000))
				WifiUpload();
	
				
	
		graphtimer++;
		if((graphtimer%10==0)&&(!(App.SysState.WifiState&WIFI_STATE_STA)))
					WifiCtrlCode(ModuleQuery);
		if(graphtimer>900)
		{
			graphtimer =1;
				if(App.SysState.FaultFlag)
					App.SysState.WifiState |= WIFI_UPLOAD_FAULT;
			
#ifdef  __SENSOR_CO2
			if(App.SysCtrlPara.Power != POWER_OFF)
				App.SysState.WifiState |= WIFI_UPLOAD_GRAPHDATA;
#endif
		}
	}
	
}


void AppWifiInit(void)
{
	System.Device.Systick.Register(Systick10, WifiSystick10);
	System.Device.Wifi.InitWifi(RxValidDataBuffer,TxValidDataBuffer,ModuleStatus);
	WifiCtrlCode(ModuleQuery);
//	System.Device.Wifi.HekrModuleControl(HekrConfig);
}


