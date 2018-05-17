/*
********************************************************************************
*�ļ���     : app.c
*����       : Ӧ�ó������
*ԭ��       : Ӧ�ò���ڣ�Ҳ��ϵͳ��ڣ���Ҫ��ʼ����������������
********************************************************************************
*�汾     ����            ����            ˵��
*V1.0    tgq        2016/05/12       ��ʼ�汾
********************************************************************************
*/


#include "system.h"
#include "app.h"

AppStruct App;

/*******************************************************************************
* ����	    : ��ʼ�����ݿ�
*******************************************************************************/
static void InitData(void)
{
	App.SysVersion.CtrlVersion=MAIN_VERSION;
	App.SysVersion.CtrlVersion <<= 8;
	App.SysVersion.CtrlVersion |= SUB_VERSION;
	App.SysVersion.PowerVersion=1;
	App.SysVersion.PowerVersion <<= 8;
	App.SysVersion.PowerVersion |= 10;
	
	App.SensorData.PMInside =50;
  App.SensorData.CO2Inside= 600;
	App.SensorData.RHInside = 50;
	App.SensorData.TempInside = 25;
	App.SensorData.TempOutside = 31;
	
	App.SysFault.MotoXF=0;
	App.SysFault.MotoPF = 0;
	App.SysFault.CO2Sensor =0;
	App.SysFault.PMSensor =0;
	App.SysFault.RHSensor =0;
	App.SysFault.TempOutSensors =0;
	App.SysFault.TempInSensors =0;
	
	App.SysCtrlPara.Power = POWER_ON;
	App.SysCtrlPara.AirFlowSet =CTRLFLOW_STEPS;
	App.SysCtrlPara.AirFlowRun= App.SysCtrlPara.AirFlowSet;
	App.SysCtrlPara.CircleModeSet= CIRCLEMODE_OUT;
	App.SysCtrlPara.VentilateRate = RATE10TO08;
	App.SysCtrlPara.ThermalModeSet = TEMPMODE_OFF;
	App.SysCtrlPara.ShutTimer = 0;
	App.SysCtrlPara.ChildLock = CHILD_UNLOCK;
	App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
	App.SysCtrlPara.AuxiliaryHeatSet = HEATER_OFF;
	
	App.FilterTimerHour.FirstFilterTimerHour =0;
	App.FilterTimerHour.MidFilterTimerHour =0;
	App.FilterTimerHour.ESPFilterTimerHour =0;
	App.FilterTimerHour.HPFilterTimerHour  =0;
	
	App.FilterHourLimit.FirstFilterHourLimit = 2000;
	App.FilterHourLimit.MidFilterHourLimit = 2000;
	App.FilterHourLimit.ESPFilterHourLimit = 1000;
	App.FilterHourLimit.HPFilterHourLimit = 3000;
	
	App.SysCtrlLine.CO2InsideLine = 1000;
	App.SysCtrlLine.PMInsideLine = 150;
	App.SysCtrlLine.TempInsideSummerLine = 25;
	App.SysCtrlLine.TempInsideWinterLine = 18;
	
	
	App.SysCtrlStatus.BypassMode = BYPASS_UNKNOW;
	App.SysCtrlStatus.AuxiliaryHeatSet =App.SysCtrlPara.AuxiliaryHeatSet;
	App.SysCtrlStatus.BypassMode = BYPASS_CIRCLEOUT;
//	App.SysCtrlStatus.XFmotoPWM = 0;
	App.SysCtrlStatus.XFmotoRPM = 0;
	App.SysCtrlStatus.PFmotoRPM = 0;
	
	App.SysRunStatus.BypassMode = BYPASS_UNKNOW;
	App.SysRunStatus.XFmotoPWM = 0;
	App.SysRunStatus.PFmotoPWM = 0;
	App.SysRunStatus.Power = POWER_EMPTY;

	App.SysState.WifiState =0;
	App.SysState.RFState = 0;
	App.SysState.FaultFlag = 0;
	App.SysState.FilterWarning = 0x00;
	
	App.RfAddrID =0;
	App.UsedHours=0;
	
	App.Menu.FocusFormPointer = &App.Menu.MainForm;
	
	InitPara();
	
}

/*******************************************************************************
* ����	    : C�������Main����
*******************************************************************************/
int main(void) 
{      
  System.Init();                          //��ʼ��ϵͳ��
  InitData();
    
  System.OS.Start();
	
	InitMenu();	
	LogicTask();
}

