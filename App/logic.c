/*
********************************************************************************
*文件名     : logic.c
*作用       : 业务逻辑处理文件
*原理       : 处理各类消息，如按键、传感器等，标准化一些按键的处理模式
********************************************************************************
*版本     作者            日期            说明
*V1.0      TGQ          2016/10/21       修改版本
********************************************************************************
*/


#include "system.h"
#include "app.h"
#include "StorePara.h"

#define SCREEN_WAITLIGHT_1S		120

#define SHUTDOWNTIMER100MS		36000

#define CO2INSIDE_GREEN_LINE    ((ushort)1001)
#define CO2INSIDE_ORANGE_LINE   ((ushort)1501)
#define CO2INSIDE_RED_LINE    	((ushort)2001)

#define PMINSIDE_GREEN_LINE    ((ushort)51)
#define PMINSIDE_ORANGE_LINE   ((ushort)101)
#define PMINSIDE_RED_LINE    	 ((ushort)151)

#define CO2INSIDE_GREEN    	((byte)0x01)
#define CO2INSIDE_ORANGE    ((byte)0x02)
#define CO2INSIDE_RED    		((byte)0x04)
#define PMINSIDE_GREEN    	((byte)0x08)
#define PMINSIDE_ORANGE    	((byte)0x10)
#define PMINSIDE_RED    		((byte)0x20)

#define IAQFLAG_GREEN			(CO2INSIDE_GREEN|PMINSIDE_GREEN)
#define IAQFLAG_ORANGE		(CO2INSIDE_ORANGE|PMINSIDE_ORANGE)
#define IAQFLAG_RED				(CO2INSIDE_RED|PMINSIDE_RED)

typedef struct{
	uint16_t		Moto1PwmPace;
	uint16_t  	Moto1RpmPace;
	uint16_t  	Moto2PwmPace;
	uint16_t  	Moto2RpmPace;
}SysIndex_TypeDef;
SysIndex_TypeDef SysIndex={PWM_MOTO1_OUT_STEP,RPM_MOTO1_OUT_STEP,0,0};

#ifdef __DOUBLE_MOTOS
	uint16_t MotoPFStopPwm = PWM_MOTO2_STOP;
#endif

byte SeqOperDelay= 0; 

static ushort UseTimer10s=0;
static byte OperTimer1s=SCREEN_WAITLIGHT_1S;
//static byte LogicDelayMs=10;
static byte StoreDelay=0;

static byte IAQFlag=0;
static byte IAQFlagNew=0;

#ifdef __EXCHANGE_FLOWS
void SetVentiMoto2Act(void)
{
	switch(App.SysCtrlPara.VentilateRate )
	{
//		case RATE10TO06:
//			SysIndex.Moto2PwmPace = (uint16_t)((PWM_MOTO2_ACT06)/AIRFLOW_TOTAL_STEPS);
//			SysIndex.Moto2RpmPace = (uint16_t)((RPM_MOTO2_ACT06)/AIRFLOW_TOTAL_STEPS);
//			break;
		case RATE10TO08:
			SysIndex.Moto2PwmPace = (uint16_t)((PWM_MOTO2_ACT08)/CTRLFLOW_STEPS);
			SysIndex.Moto2RpmPace = (uint16_t)((RPM_MOTO2_ACT08)/CTRLFLOW_STEPS);
			break;
		case RATE10TO10:
			SysIndex.Moto2PwmPace = (uint16_t)((PWM_MOTO2_ACT10)/CTRLFLOW_STEPS);
			SysIndex.Moto2RpmPace = (uint16_t)((RPM_MOTO2_ACT10)/CTRLFLOW_STEPS);
			break;
		case RATE10TO12:
			SysIndex.Moto2PwmPace = (uint16_t)((PWM_MOTO2_ACT12)/CTRLFLOW_STEPS);
			SysIndex.Moto2RpmPace = (uint16_t)((RPM_MOTO2_ACT12)/CTRLFLOW_STEPS);
			break;
		default:
			App.SysCtrlPara.VentilateRate = RATE10TO08;
			SysIndex.Moto2PwmPace = (uint16_t)((PWM_MOTO2_ACT12)/CTRLFLOW_STEPS);
			SysIndex.Moto2RpmPace = (uint16_t)((RPM_MOTO2_ACT08)/CTRLFLOW_STEPS);
			break;
	}	
}
#endif

void GetMotoXFOcupy(uint8_t airflow)
{
	uint32_t temp;
	
	if(airflow )
	{
		temp = (SysIndex.Moto1RpmPace*airflow);
		temp += RPM_MOTO1_MIN;
		App.SysCtrlStatus.XFmotoRPM= (uint16_t)temp;
		
		temp = SysIndex.Moto1PwmPace*airflow;
		temp += (PWM_MOTO1_MIN);
		App.SysCtrlStatus.XFmotoPWM= (uint16_t)temp; 
	}
	else
	{
		App.SysCtrlStatus.XFmotoPWM= 0;
		App.SysCtrlStatus.XFmotoRPM =0;
	}
}
		
#ifdef __DOUBLE_MOTOS
void GetMotoPFOcupy(uint8_t airflow)
{
	uint32_t temp;
	
	if(airflow )
	{
			temp = (SysIndex.Moto2RpmPace*airflow);
			temp += RPM_MOTO2_MIN;
			App.SysCtrlStatus.PFmotoRPM= (uint16_t)temp;
			
					
			temp = SysIndex.Moto2PwmPace*airflow;
			temp += (PWM_MOTO2_MIN);
			App.SysCtrlStatus.PFmotoPWM= (uint16_t)temp; 
		
	#ifdef __EXCHANGE_FLOWS
			if((App.SysCtrlStatus.BypassMode == BYPASS_CIRCLEIN)||(App.SysCtrlStatus.ThermalMode == TEMPMODE_ON))
			{
				App.SysCtrlStatus.PFmotoPWM=  MotoPFStopPwm;
				App.SysCtrlStatus.PFmotoRPM = RPM_MOTO2_STOP;//350;
			}
	#endif
			
	}
	else
	{
		App.SysCtrlStatus.PFmotoPWM= 0;
		App.SysCtrlStatus.PFmotoRPM =0;
	}
	
}
#endif		




/*******************************************************************************
* 描述	    : 根据传感器数据判定运行模式
*******************************************************************************/
void ParseEchoData(byte data)
{
	switch(data)
	{
#ifdef __EXCHANGE_FLOWS
		case COMM_TEMPER_RH:
			if(App.SysCtrlPara.ThermalModeSet == TEMPMODE_AUTO)
			{
				if(App.SensorData.TempOutside>22)
				{
					if((App.SensorData.TempInside>App.SysCtrlLine.TempInsideSummerLine)&&(App.SensorData.TempOutside<=(App.SysCtrlLine.TempInsideSummerLine-2)))
						 App.SysCtrlStatus.ThermalMode = TEMPMODE_ON;
					else
						App.SysCtrlStatus.ThermalMode =  TEMPMODE_OFF;
				}
				else if(App.SensorData.TempOutside<21)
				{
					if((App.SensorData.TempInside<App.SysCtrlLine.TempInsideWinterLine)&&(App.SensorData.TempOutside>=(App.SysCtrlLine.TempInsideWinterLine+2)))
						 App.SysCtrlStatus.ThermalMode = TEMPMODE_ON;
					else
						App.SysCtrlStatus.ThermalMode =  TEMPMODE_OFF;
				}
				else
					App.SysCtrlStatus.ThermalMode =  TEMPMODE_OFF;			
				GetMotoPFOcupy(App.SysCtrlPara.AirFlowRun);
			}

  		break;
#endif
		case COMM_CO2_READ:
			if(App.SysCtrlPara.CircleModeSet==CIRCLEMODE_AUTO)
			{
				if(App.SensorData.CO2Inside>App.SysCtrlLine.CO2InsideLine)
				{
					if(App.SysCtrlStatus.BypassMode != BYPASS_CIRCLEOUT)
					{
						App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEOUT;
						PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
					}
//					System.Device.Timer.Start(CO2_WARNING_TIMER,TimerMessage,CO2_WARNING_MS,SwitchCircleMode);
				}
				else if(App.SensorData.CO2Inside<(App.SysCtrlLine.CO2InsideLine-150))
				{
					if(App.SysCtrlStatus.BypassMode != BYPASS_CIRCLEIN)
					{
						App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEIN;
						PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
					}
				}
			}
			IAQFlagNew &= ~(CO2INSIDE_GREEN|CO2INSIDE_ORANGE|CO2INSIDE_RED);
      if(App.SensorData.CO2Inside<CO2INSIDE_GREEN_LINE)
        IAQFlagNew |= CO2INSIDE_GREEN;
      else if(App.SensorData.CO2Inside<CO2INSIDE_ORANGE_LINE)			
        IAQFlagNew |= CO2INSIDE_ORANGE;
			else
        IAQFlagNew |= CO2INSIDE_RED;
			break;
		case COMM_IAQ_READ:
			if(App.SysCtrlPara.CircleModeSet==CIRCLEMODE_AUTO)
			{
				if(App.SensorData.PMInside>App.SysCtrlLine.PMInsideLine)
				{
					App.SysCtrlPara.AirFlowRun++;
					if(App.SysCtrlPara.AirFlowRun>CTRLFLOW_STEPS)
							App.SysCtrlPara.AirFlowRun = CTRLFLOW_STEPS;
					PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
				}
				else if(App.SysCtrlPara.AirFlowRun != App.SysCtrlPara.AirFlowSet)
				{
					if(App.SysCtrlPara.AirFlowRun>App.SysCtrlPara.AirFlowSet )
						App.SysCtrlPara.AirFlowRun--;
					else
						App.SysCtrlPara.AirFlowRun = App.SysCtrlPara.AirFlowSet;

					PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
				}
			}	
			IAQFlagNew &= ~(PMINSIDE_GREEN|PMINSIDE_ORANGE|PMINSIDE_RED);
      if(App.SensorData.PMInside<PMINSIDE_GREEN_LINE)
        IAQFlagNew |= PMINSIDE_GREEN;
      else if(App.SensorData.PMInside<PMINSIDE_ORANGE_LINE)			
        IAQFlagNew |= PMINSIDE_ORANGE;
			else
        IAQFlagNew |= PMINSIDE_RED;
			break;
		case COMM_HEATERSET:
			if(App.SysCtrlStatus.AuxiliaryHeatSet!=App.SysRunStatus.AuxiliaryHeat )
			{
					App.SysFault.AuxiHeater++;
					if(App.SysFault.AuxiHeater>10)
						App.SysState.FaultFlag |= HGA_FAULT;
			}
			else
			{
					App.SysFault.AuxiHeater=0;
					App.SysState.FaultFlag &= ~HGA_FAULT;
			}

				
			break;
		case COMM_XFMOTODUTY:
#ifdef __SELF_ADJUSTMOTO
					if(App.SysFault.MotoXF>20)  //plus 10%
					{
						App.SysState.FaultFlag |= XFMOTO_FAULT;
						App.SysFault.MotoXF=20;
					}
					else
						App.SysState.FaultFlag &= ~XFMOTO_FAULT;

#endif
			break;
#ifdef __DOUBLE_MOTOS
		case COMM_PFMOTODUTY:
#ifdef __SELF_ADJUSTMOTO
					if(App.SysFault.MotoPF>20)  //plus 10%
					{
						App.SysState.FaultFlag |= PFMOTO_FAULT;
						App.SysFault.MotoPF=20;
					}
					else
						App.SysState.FaultFlag &= ~PFMOTO_FAULT;

#endif
			break;
#endif

		case COMM_POWER_SET:
			if(App.SysCtrlStatus.Power == POWER_ON)
			{
				if(App.SysRunStatus.Power ==POWER_ON)
				{
					App.SysFault.PowerBaseFault=0;
					App.SysState.FaultFlag  &= ~POWERBASE_FAULT;	
				}
				else
				{
					App.SysFault.PowerBaseFault++;
					if(App.SysFault.PowerBaseFault>20)
						App.SysState.FaultFlag  |= POWERBASE_FAULT;
				}
			}
			else //if(App.SysCtrlStatus.Power == POWER_OFF)
			{
				if(App.SysRunStatus.Power ==POWER_OFF)
				{
					App.SysFault.PowerBaseFault=0;
					App.SysState.FaultFlag  &= ~POWERBASE_FAULT;
				}
				else
				{
					App.SysFault.PowerBaseFault++;
					if(App.SysFault.PowerBaseFault>20)
						App.SysState.FaultFlag  |= POWERBASE_FAULT;
				}
			}
			break;
		default:
			break;
	}

	
}


void Function_Run(void)
{
//	uint16_t temp=0;
	static byte seconds=0;

	seconds++;
	
//	if((App.SysState.WifiState&WIFI_STATE_STA)&&(App.SysState.WifiState&0x07C00000))
//			WifiUpload();
		
	switch(seconds) //10s
	{
		case 1:
		  if(App.SysCtrlStatus.Power != App.SysRunStatus.Power )
			{
//				seconds=0;
				PostMessage(MessageCommTrans, COMM_POWER_SET);
			}
			else
			{
				if(App.SysCtrlStatus.AuxiliaryHeatSet != App.SysRunStatus.AuxiliaryHeat )
					PostMessage(MessageCommTrans, COMM_HEATERSET);
//				if(App.SysCtrlStatus.XFmotoPWM != App.SysRunStatus.XFmotoPWM )
//					PostMessage(MessageCommTrans, COMM_XFMOTODUTY);
				if(App.SysCtrlStatus.BypassMode != App.SysRunStatus.BypassMode )
					PostMessage(MessageCommTrans, COMM_CIRCLEMODE);
			}
		break;
		case 2:
			PostMessage(MessageCommTrans, COMM_XFMOTODUTY);
		break;
		case 3:
#ifdef __DOUBLE_MOTOS
			PostMessage(MessageCommTrans, COMM_PFMOTODUTY);
#endif
		break;
		case 4:
			PostMessage(MessageCommTrans, COMM_TEMPER_RH);
		break;
		case 5:
			PostMessage(MessageCommTrans, COMM_IAQ_READ);
		break;
		case 6:
			PostMessage(MessageCommTrans, COMM_CO2_READ);
		break;
		case 7:
//				if(!(App.SysState.WifiState&WIFI_STATE_STA))
//					WifiCtrlCode(ModuleQuery);
			PostMessage(MessageCommTrans, COMM_XFMOTODUTY);
		break;
		case 8:
		if(App.SysCtrlPara.Power == POWER_SLEEP)
			App.SensorData.RHInside = (uint8_t)System.Device.RhT.ReadRhTSensor(HIMIDITY_READ);
#ifdef __DOUBLE_MOTOS
			PostMessage(MessageCommTrans, COMM_PFMOTODUTY);
#endif
		break;
		case 9:
//			PostMessage(MessageCommTrans, COMM_CO2_READ);
		if(App.SysCtrlPara.Power == POWER_SLEEP)
		{
			App.SensorData.TempInside = (int8_t)System.Device.RhT.ReadRhTSensor(TEMPER_READ);
			PostMessage(MessageProcess, IAQFLAG_DISP);
		}
		
			break;
		case 10:
			seconds=0;
			UseTimer10s++;
#ifdef __TESTFILTER			
			PostMessage(MessageProcess, FILTER_ONEHOUR);
#endif
			if(UseTimer10s>360)
			{
				PostMessage(MessageProcess, FILTER_ONEHOUR);
				UseTimer10s=0;

			}
			break;
		default:
			break;
	}


}
		
static void AppSystick10(void)
{
	static byte second;
	
	
	if(++second==10)
	{
		second =0;
		if(App.SysCtrlStatus.Power==POWER_OFF)
		{
			if(App.SysRunStatus.Power != POWER_OFF)
				PostMessage(MessageCommTrans, COMM_POWER_SET);
			return;
		}
		Function_Run();
		if(OperTimer1s)
		{
			OperTimer1s--;
				if(!OperTimer1s)
				{
					LCD_BL_OFF;
					IAQFlag=0;
					PostMessage(MessageProcess, IAQFLAG_DISP);
					App.SysCtrlPara.Power = POWER_SLEEP;
				}
				
		}
	}
	
	if(SeqOperDelay)
	{
		SeqOperDelay--;
		if(!SeqOperDelay)
		{
			PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
			System.Device.Beep.BeepOn(BEEP_SHORT);
//			CommTalk_Trans(COMM_BEEPONE);
		}
	}
	
}
static void SysPowerOff(void)
{
	App.SysCtrlPara.Power = POWER_OFF;
	App.SysCtrlStatus.Power = POWER_OFF;	
//	App.SysCtrlPara.AirFlowRun = CTRLFLOW_STEP;
	App.SysCtrlPara.AuxiliaryHeatSet = HEATER_OFF;
	App.SysCtrlPara.ChildLock =CHILD_UNLOCK;
	App.SysCtrlPara.ShutTimer =0;
	System.Device.Timer.Stop(TIMER1);
	App.Menu.MainForm.LoadFresh = FocusOn_START;
	App.Menu.FocusFormPointer= &App.Menu.MainForm;
	LCD_BL_OFF;
	System.Device.Led.LedModeSet(LED_RED,TURN_ON);
	System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
}

static void SysPowerOn(void)
{
	App.SysCtrlPara.Power = POWER_ON;
	App.SysCtrlStatus.Power = POWER_ON;
	App.SysCtrlPara.AuxiliaryHeatSet = HEATER_OFF;
	App.SysCtrlStatus.AuxiliaryHeatSet = App.SysCtrlPara.AuxiliaryHeatSet;
	App.SysCtrlPara.AirFlowRun= App.SysCtrlPara.AirFlowSet;
	if(App.SysCtrlPara.ChildLock >CHILD_LOCK)
		App.SysCtrlPara.ChildLock =CHILD_UNLOCK;
	
	App.SysRunStatus.XFmotoPWM = 20;
	App.SysRunStatus.Power = POWER_EMPTY;
	App.SysRunStatus.BypassMode = BYPASS_UNKNOW;
	
	PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);  
	PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);  
	if(App.SysCtrlPara.ShutTimer <10)
		PostMessage(MessageParaUpdate, PARA_WORKMODE);
  else 
   App.SysCtrlPara.ShutTimer=0;		
	System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
	System.Device.Led.LedModeSet(LED_RED,TURN_OFF);
	
#ifdef __EXCHANGE_FLOWS
	SetVentiMoto2Act();
#endif
//	InitHmi();
//	START MOTOS AND CHECK CIRCLEMODE
//	WifiCtrlCode(ModuleQuery);
	App.SensorData.RHInside = (uint8_t)System.Device.RhT.ReadRhTSensor(HIMIDITY_READ);
	App.SensorData.TempInside = (int8_t)System.Device.RhT.ReadRhTSensor(TEMPER_READ);
	
	LCD_BL_ON;
}

void CtrlParaUpdate(ParaOperTypedef data)
{
	switch(data)
	{
		case PARA_XFMOTODUTY:
			GetMotoXFOcupy(App.SysCtrlPara.AirFlowRun);
#ifdef __DOUBLE_MOTOS
			GetMotoPFOcupy(App.SysCtrlPara.AirFlowRun);
#endif
			PostMessage(MessageCommTrans, COMM_XFMOTODUTY);
			StorePost(STORE_SYSPARA);
			break;
		case PARA_PFMOTODUTY:
#ifdef __EXCHANGE_FLOWS
			GetMotoPFOcupy(App.SysCtrlPara.AirFlowRun);
			PostMessage(MessageCommTrans, COMM_PFMOTODUTY);
#endif		
			break;
		case PARA_CIRCLEMODE:
		  switch(App.SysCtrlPara.CircleModeSet)
			{
				case CIRCLEMODE_OUT:
					App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEOUT;
				 SysIndex.Moto1PwmPace = PWM_MOTO1_OUT_STEP;
				 SysIndex.Moto1RpmPace = RPM_MOTO1_OUT_STEP;
					PostMessage(MessageCommTrans, COMM_CIRCLEMODE);
				break;
				case CIRCLEMODE_IN:
					App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEIN;
				 SysIndex.Moto1PwmPace = PWM_MOTO1_IN_STEP;
				 SysIndex.Moto1RpmPace = RPM_MOTO1_IN_STEP;
//					PostMessage(MessageParaUpdate, PARA_PFMOTODUTY);
					PostMessage(MessageCommTrans, COMM_CIRCLEMODE);
				break;
				case CIRCLEMODE_AUTO:
					if(App.SysCtrlStatus.BypassMode == BYPASS_CIRCLEIN)
					{
						 SysIndex.Moto1PwmPace = PWM_MOTO1_IN_STEP;
						 SysIndex.Moto1RpmPace = RPM_MOTO1_IN_STEP;
					}
					else
					{
						App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEOUT;
					  SysIndex.Moto1PwmPace = PWM_MOTO1_OUT_STEP;
					  SysIndex.Moto1RpmPace = RPM_MOTO1_OUT_STEP;
					}
				break;
				default:
					App.SysCtrlPara.CircleModeSet =CIRCLEMODE_OUT;
					App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEOUT;
				  SysIndex.Moto1PwmPace = PWM_MOTO1_OUT_STEP;
				  SysIndex.Moto1RpmPace = RPM_MOTO1_OUT_STEP;
					PostMessage(MessageCommTrans, COMM_CIRCLEMODE);
				break;
			}
			PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);			
			StorePost(STORE_SYSPARA);
			break;
		case PARA_HEATSET:
			App.SysCtrlStatus.AuxiliaryHeatSet = App.SysCtrlPara.AuxiliaryHeatSet;
			PostMessage(MessageCommTrans, COMM_HEATERSET);
			break;
		case PARA_MUTEMODE:
			if(App.SysCtrlPara.MuteSet == MUTEMODE_ACT)
			{
					LCD_BL_OFF;
					System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
					System.Device.Led.LedModeSet(LED_RED,TURN_OFF);
					App.SysCtrlPara.AirFlowRun = CTRLFLOW_STEP_MUTE;
					App.SysCtrlPara.Power = POWER_SLEEP;
				OperTimer1s=0;
			}
			else
			{
				App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
				OperTimer1s=SCREEN_WAITLIGHT_1S;
				System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
				App.SysCtrlPara.Power = POWER_ON;
				App.SysCtrlPara.AirFlowRun = App.SysCtrlPara.AirFlowSet;
				LCD_BL_ON;
				
			}
			PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
			break;
		case PARA_VENTILATE:
#ifdef __EXCHANGE_FLOWS
			SetVentiMoto2Act();
  		GetMotoPFOcupy(App.SysCtrlPara.AirFlowRun);
			PostMessage(MessageCommTrans, COMM_PFMOTODUTY);
			StorePost(STORE_SYSPARA);
#endif
			break;
		case PARA_THERMALMODE:
#ifdef __EXCHANGE_FLOWS
			switch(App.SysCtrlPara.ThermalModeSet)
			{
				case TEMPMODE_AUTO:
					break;
				case TEMPMODE_ON:
					App.SysCtrlStatus.ThermalMode = TEMPMODE_ON;
					break;
				case TEMPMODE_OFF:
					App.SysCtrlStatus.ThermalMode = TEMPMODE_OFF;
					break;
				default:
					App.SysCtrlPara.ThermalModeSet = TEMPMODE_OFF;
					App.SysCtrlStatus.ThermalMode = TEMPMODE_OFF;
					break;
			}
			GetMotoPFOcupy(App.SysCtrlPara.AirFlowRun);
			PostMessage(MessageCommTrans, COMM_PFMOTODUTY);
			StorePost(STORE_SYSPARA);
#endif
			break;
		case PARA_CHILDLOCK:
			break;
		case PARA_WORKMODE:
			if(App.SysCtrlPara.ShutTimer >0)
			{
				System.Device.Timer.Start(TIMER1,TimerMessage,SHUTDOWNTIMER100MS,ALARM_SOUND_ONEHOUR);
			}
			else
				System.Device.Timer.Stop(TIMER1);
			
			break;
		case PARA_POWER_SET:
			if(App.SysCtrlPara.Power == POWER_ON)
			{
				SysPowerOn();
				OperTimer1s=SCREEN_WAITLIGHT_1S;
			}
			else
			{
				SysPowerOff();
			}
			PostMessage(MessageCommTrans, COMM_POWER_SET);
			StorePost(STORE_SYSPARA);

			break;
			default:
			break;
	}
}

void AlarmOper(AlarmTypedef AlarmOper)
{
	switch(AlarmOper)
	{
		case ALARM_SOUND_ONEHOUR:
			if(App.SysCtrlPara.ShutTimer >0)
			{
				App.SysCtrlPara.ShutTimer--;
				if(App.SysCtrlPara.ShutTimer)
					System.Device.Timer.Start(TIMER1,TimerMessage,SHUTDOWNTIMER100MS, ALARM_SOUND_ONEHOUR);
				else
				{
					App.SysCtrlPara.Power = POWER_OFF;
					PostMessage(MessageParaUpdate, PARA_POWER_SET);
				}
			}
			break;
		case ALARM_START:
			break;
		case ALARM_INIT:
			break;
		default:
			break;
	}
}

void ProcessParse(ProcessTypedef ProcessMessage)
{
	switch(ProcessMessage)
	{
		case FILTER_ONEHOUR:
			App.UsedHours++;
			if(App.UsedHours>99999)
				App.UsedHours = 99999;
			StorePost(STORE_USEDTIMER);
			if(App.SysCtrlPara.AirFlowRun>0)
			{
				App.FilterTimerHour.FirstFilterTimerHour++;
				if(App.FilterTimerHour.FirstFilterTimerHour > 9999)
					App.FilterTimerHour.FirstFilterTimerHour = 9999;
				
				App.FilterTimerHour.MidFilterTimerHour++;
				if(App.FilterTimerHour.MidFilterTimerHour > 9999)
					App.FilterTimerHour.MidFilterTimerHour = 9999;
				
				App.FilterTimerHour.ESPFilterTimerHour++;
				if(App.FilterTimerHour.ESPFilterTimerHour > 9999)
					App.FilterTimerHour.ESPFilterTimerHour = 9999;
				
				App.FilterTimerHour.HPFilterTimerHour++;
				if(App.FilterTimerHour.HPFilterTimerHour > 9999)
					App.FilterTimerHour.HPFilterTimerHour = 9999;
				
				StorePost(STORE_FILTERTIMER);
				PostMessage(MessageProcess, FILTER_CHECK);
			}
			break;
		case FILTER_CLEAR:
			if(App.SysState.FilterWarning&FIRSTFILTER_CHECK)
			{
				App.FilterTimerHour.FirstFilterTimerHour =0;
			}
			App.SysState.FilterWarning &= ~FIRSTFILTER_CHECK;
			
			if(App.SysState.FilterWarning&MIDFILTER_CHECK)
			{
				App.FilterTimerHour.MidFilterTimerHour =0;
			}
			App.SysState.FilterWarning &= ~MIDFILTER_CHECK;
			
			if(App.SysState.FilterWarning&ESPFILTER_CHECK)
			{
				App.FilterTimerHour.ESPFilterTimerHour =0;
			}
			App.SysState.FilterWarning &= ~ESPFILTER_CHECK;
			
			if(App.SysState.FilterWarning&HPFILTER_CHECK)
			{
				App.FilterTimerHour.HPFilterTimerHour =0;
			}
			App.SysState.FilterWarning &= ~HPFILTER_CHECK;
			
			StorePost(STORE_FILTERTIMER);
			break;
		case FILTER_CHECK:
			if(App.FilterTimerHour.FirstFilterTimerHour> App.FilterHourLimit.FirstFilterHourLimit)
			{
//				System.Device.Led.LedModeSet(LED_RED, TURN_ON);
				App.SysState.FilterWarning |= FIRSTFILTER_CHECK;
			}
			else
				App.SysState.FilterWarning &= ~FIRSTFILTER_CHECK;
				
			if(App.FilterTimerHour.MidFilterTimerHour> App.FilterHourLimit.MidFilterHourLimit)
			{
//				System.Device.Led.LedModeSet(LED_RED, TURN_ON);
				App.SysState.FilterWarning |= MIDFILTER_CHECK;
			}
			else
				App.SysState.FilterWarning &= ~MIDFILTER_CHECK;
			
			if(App.FilterTimerHour.ESPFilterTimerHour> App.FilterHourLimit.ESPFilterHourLimit)
			{
//				System.Device.Led.LedModeSet(LED_RED, TURN_ON);
				App.SysState.FilterWarning |= ESPFILTER_CHECK;
			}
			else
				App.SysState.FilterWarning &= ~ESPFILTER_CHECK;
			
			if(App.FilterTimerHour.HPFilterTimerHour> App.FilterHourLimit.HPFilterHourLimit)
			{
//				System.Device.Led.LedModeSet(LED_RED, TURN_ON);
				App.SysState.FilterWarning |= HPFILTER_CHECK;
			}
			else
				App.SysState.FilterWarning &= ~HPFILTER_CHECK;
			break;
		case FILTER_POWERON:
			if(App.FilterTimerHour.FirstFilterTimerHour> App.FilterHourLimit.FirstFilterHourLimit)
			{
				App.FilterTimerHour.FirstFilterTimerHour =0;
				StorePost(STORE_FILTERTIMER);
			}
			
			if(App.FilterTimerHour.MidFilterTimerHour> App.FilterHourLimit.MidFilterHourLimit)
			{
				App.FilterTimerHour.MidFilterTimerHour =0;
				StorePost(STORE_FILTERTIMER);
			}
			
			if(App.FilterTimerHour.ESPFilterTimerHour> App.FilterHourLimit.ESPFilterHourLimit)
			{
				App.FilterTimerHour.ESPFilterTimerHour =0;
				StorePost(STORE_FILTERTIMER);
			}
			
			if(App.FilterTimerHour.HPFilterTimerHour> App.FilterHourLimit.HPFilterHourLimit)
			{
				App.FilterTimerHour.HPFilterTimerHour =0;
				StorePost(STORE_FILTERTIMER);
			}
			break;
		case FAULTINFO_DISP:
			OperTimer1s = 0;
			break;
		
		case IAQFLAG_DISP:
			if((IAQFlagNew != IAQFlag)&&(App.SysCtrlPara.MuteSet == MUTEMODE_OFF))
			{
				IAQFlag = IAQFlagNew;
				if(IAQFlag&IAQFLAG_RED)
				{
					System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
					System.Device.Led.LedModeSet(LED_RED,BREATH_ON);
				}
				else if(IAQFlag&IAQFLAG_ORANGE)
				{
					System.Device.Led.LedModeSet(LED_RED,BREATH_ON);
					System.Device.Led.LedModeSet(LED_GREEN,BREATH_ON);
				}
				else
				{
					System.Device.Led.LedModeSet(LED_RED,TURN_OFF);
					System.Device.Led.LedModeSet(LED_GREEN,BREATH_ON);
				}
			}
			break;
	}
}

static void InitLogic(void)
{
  System.Device.Systick.Register(Systick10, AppSystick10);
#ifdef __WIFI_VALIDE
	AppWifiInit();
#endif
#ifdef __RF_VALIDE
	UnitRFInit();
#endif
}

void LogicTask(void)
{
    uint message;
    uint data; 
    InitLogic();


    
	//  System.OS.DelayMs(100);
//	SysPowerOn();

	PostMessage(MessageCommTrans, COMM_VERSION);
	PostMessage(MessageParaUpdate, PARA_POWER_SET);
	System.Device.RemoteIR.RemoteIRCtrl(ENABLE);
    
    //逻辑业务任务获取消息，分配处理
    while(TRUE)
    {     
        message = System.OS.PendMessageQueue();
        
        data = GetMessageData(message);
        switch(GetMessageType(message))
        {
            case MessageKey:                    //按键消息
               App.Menu.FocusFormPointer->KeyProcess((KeyActEnum)data);
							OperTimer1s=SCREEN_WAITLIGHT_1S;
                break;
            case MessageCommRecv:
                if(TRUE==CommTalk_Echo(data))  //20161118
								{
									App.SysFault.PowerBaseFault=0;
									App.SysState.FaultFlag &= ~POWERBASE_FAULT;
								}
                break;
            case MessageCommTrans:
                CommTalk_Trans(data);
						    App.SysFault.PowerBaseFault++;
						if(App.SysFault.PowerBaseFault>50)
						{
							App.SysState.FaultFlag |= POWERBASE_FAULT;
							App.SysState.FaultFlag &= ~FAULTICON_DISP;
						}
                break;
            case MessageIR:
							OperTimer1s=SCREEN_WAITLIGHT_1S;
                ParseRemoteIR(data);
                break;
            case MessageEchoDate:
                ParseEchoData(data);
                break;
            case MessageAlarm:
								AlarmOper((AlarmTypedef)data);
                break;
						case MessageProcess:
							ProcessParse((ProcessTypedef)data);
							break;
            case MessageParaUpdate:
                CtrlParaUpdate((ParaOperTypedef)data);
								App.SysState.WifiState |= WIFI_UPLOAD_SYSPARA;
                break;
            case MessageWifiRecv:
                WifiRecvParse(data);
                break;
            case MessageWifiCtrl:
                WifiCtrlCode(data);
                break;
            case MessageRfRecv:
                RfRecvParse(data);
                break;
            case MessageRfCtrl:
                RfCtrlParse(data);
                break;
            default:
							if(App.SysCtrlPara.Power>POWER_ON)
							{
								StoreDelay++;
								if(StoreDelay>100)
								{
									StoreDelay=0;
									StorePara();
								}
							}
							else
								StoreDelay=0;
                break;
        }
				
//				if(App.SysCtrlPara.Power == POWER_OFF)
//					System.Device.Iwdog.IwdogReload();

				MenuTask();
				System.OS.DelayMs(25);   
    }
}
