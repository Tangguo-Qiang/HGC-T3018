#include "system.h"
#include "app.h"
#include "remoteIR.h"


void ParseRemoteIR(byte order)
{
	if(App.SysCtrlPara.Power == POWER_SLEEP)
	{
		CommTalk_Trans(COMM_BEEPONE);
		if(App.SysCtrlPara.MuteSet == MUTEMODE_ACT)
		{
			App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
			PostMessage(MessageParaUpdate, PARA_MUTEMODE);
		}
		else
		{
			System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
			App.SysCtrlPara.Power = POWER_ON;
		LCD_BL_ON;
		}
		return;
	}
	
	if((App.SysCtrlPara.Power == POWER_OFF)&&(order!=IR_REMOTE_POWER))
		return;
	
  if((App.SysCtrlPara.ChildLock==CHILD_LOCK)&&(order!=IR_REMOTE_LOCK)&&(order!=IR_REMOTE_MUTE))
	{
		System.Device.Led.LedModeSet(LED_GREEN,TEMP_TWINKLE);
		return;
	}
  
	App.SysState.WifiState |= WIFI_UPLOAD_SYSPARA;
	switch(order)
	{
		case IR_REMOTE_AIRFLOW_PLUS:
      App.SysCtrlPara.AirFlowSet++;
			if(App.SysCtrlPara.AirFlowSet>CTRLFLOW_STEPS)
				App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEPS;
			App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
//			PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
//			CommTalk_Trans(COMM_BEEPONE);
			SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
			
			break;
		case IR_REMOTE_AIRFLOW_MINUS:
        App.SysCtrlPara.AirFlowSet--;
	  		if(App.SysCtrlPara.AirFlowSet<CTRLFLOW_STEP)
					App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEP;
				App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
//		PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
//		CommTalk_Trans(COMM_BEEPONE);
			SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
				
			break;
		case IR_REMOTE_LOCK:
			if(App.SysCtrlPara.ChildLock == CHILD_UNLOCK)
				App.SysCtrlPara.ChildLock = CHILD_LOCK;
			else
				App.SysCtrlPara.ChildLock = CHILD_UNLOCK;
			PostMessage(MessageParaUpdate, PARA_CHILDLOCK);
			CommTalk_Trans(COMM_BEEPONE);
			
			break;
		case IR_REMOTE_VENTIRATE_PLUS:
#ifdef __EXCHANGE_FLOWS
		  App.SysCtrlPara.VentilateRate++;
		  if(App.SysCtrlPara.VentilateRate>RATE10TO12)
				App.SysCtrlPara.VentilateRate=RATE10TO12;
			PostMessage(MessageParaUpdate, PARA_VENTILATE);
			CommTalk_Trans(COMM_BEEPONE);
#endif	
			break;
		case IR_REMOTE_VENTIRATE_MINUS:
#ifdef __EXCHANGE_FLOWS
		  App.SysCtrlPara.VentilateRate--;
		  if(App.SysCtrlPara.VentilateRate<RATE10TO08)
				App.SysCtrlPara.VentilateRate=RATE10TO08;
			PostMessage(MessageParaUpdate, PARA_VENTILATE);
			CommTalk_Trans(COMM_BEEPONE);
#endif	
			break;
		case IR_REMOTE_POWER:
			if(App.SysCtrlPara.Power == POWER_OFF)
				App.SysCtrlPara.Power = POWER_ON;
			else
				App.SysCtrlPara.Power = POWER_OFF;
			PostMessage(MessageParaUpdate, PARA_POWER_SET);
			CommTalk_Trans(COMM_BEEPLONG);
			
			break;
		case IR_REMOTE_MUTE:
			if(App.SysCtrlPara.MuteSet == MUTEMODE_OFF)
				App.SysCtrlPara.MuteSet = MUTEMODE_ACT;
			else
				App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
			PostMessage(MessageParaUpdate, PARA_MUTEMODE);
			CommTalk_Trans(COMM_BEEPONE);
			
			break;
		case IR_REMOTE_CIRCLEOUT:
			App.SysCtrlPara.CircleModeSet = CIRCLEMODE_OUT;
			PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
			CommTalk_Trans(COMM_BEEPONE);
		
			break;
		case IR_REMOTE_CIRCLEIN:
			App.SysCtrlPara.CircleModeSet = CIRCLEMODE_IN;
			PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);			
			CommTalk_Trans(COMM_BEEPONE);
		
			break;
		case IR_REMOTE_CIRCLEAUTO:
			App.SysCtrlPara.CircleModeSet = CIRCLEMODE_AUTO;
			PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);			
			CommTalk_Trans(COMM_BEEPONE);
		
			break;
		
		case IR_REMOTE_THERMALMODE:
#ifdef __EXCHANGE_FLOWS
		  App.SysCtrlPara.ThermalModeSet++;
		  if(App.SysCtrlPara.ThermalModeSet>TEMPMODE_OFF)
				App.SysCtrlPara.ThermalModeSet=TEMPMODE_AUTO;
			PostMessage(MessageParaUpdate, PARA_THERMALMODE);
			CommTalk_Trans(COMM_BEEPONE);
		
#else
 #ifdef __AUXI_HEATER
   #ifdef HD_GJ_160Z
			if(App.SysCtrlPara.AuxiliaryHeatSet==HEATER_OFF)
				App.SysCtrlPara.AuxiliaryHeatSet = HEATER_FULL;
			else
				App.SysCtrlPara.AuxiliaryHeatSet = HEATER_OFF;
						
   #else
			App.SysCtrlPara.AuxiliaryHeatSet++;
			if(App.SysCtrlPara.AuxiliaryHeatSet>HEATER_FULL)
				App.SysCtrlPara.AuxiliaryHeatSet = HEATER_OFF;						
   #endif
			PostMessage(MessageParaUpdate, PARA_HEATSET);
			CommTalk_Trans(COMM_BEEPONE);
 #endif
#endif			
			break;
		case IR_REMOTE_TIMERMODE:
			App.SysCtrlPara.ShutTimer++;
			if(App.SysCtrlPara.ShutTimer>9)
			{
				App.SysCtrlPara.ShutTimer = 0;
			}
			PostMessage(MessageParaUpdate, PARA_WORKMODE);			
			CommTalk_Trans(COMM_BEEPONE);			
			
			break;
		default:
			break;
	}
}
