#include "app.h"


/*******************************************************************************
* 描述	    : 按键消息处理函数
* 输入参数  : key，任意按键值
*******************************************************************************/
void KeyProcess(KeyActEnum key)
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
		LCD_BL_ON;
			System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
			App.SysCtrlPara.Power = POWER_ON;
			System.Device.Key.KeyLedSet(5);
		}
		return;
	}

	if((App.SysCtrlPara.Power == POWER_OFF)&&(key!=KeyPower))
		return;
	
  if(App.SysCtrlPara.ChildLock==CHILD_LOCK)
	{
		System.Device.Led.LedModeSet(LED_GREEN,TEMP_TWINKLE);
		
		return;
	}
	
	

		switch(key)
    {
			case KeyPower:
        CommTalk_Trans(COMM_BEEPLONG);			
			break;
			case KeyMode:
        CommTalk_Trans(COMM_BEEPONE);
			System.Device.Key.KeyLedSet(5);
				break;
			case KeyHeater:
        CommTalk_Trans(COMM_BEEPONE);
			System.Device.Key.KeyLedSet(5);
				break;
			case KeyPlus:
			case KeyMinus:
//        CommTalk_Trans(COMM_BEEPONE);
			System.Device.Key.KeyLedSet(5);
				break;
#ifdef __WIFI_VALIDE
			case KeyLongPlus:
        CommTalk_Trans(COMM_BEEPTWO);
			System.Device.Key.KeyLedSet(5);
				break;
			case KeyLongPlusMinus:
        CommTalk_Trans(COMM_BEEPTWO);
			System.Device.Key.KeyLedSet(5);
				break;
#endif
			default:
				break;
		}
		
		switch(key)
    {
        case KeyPower:                
					if(App.SysCtrlPara.Power == POWER_OFF)
					{
						App.SysCtrlPara.Power = POWER_ON;
						System.Device.Key.KeyLedSet(5);
					}
					else
					{
						App.SysCtrlPara.Power = POWER_OFF;
						System.Device.Key.KeyLedSet(1);
					}
					PostMessage(MessageParaUpdate, PARA_POWER_SET);
            break;
        case KeyMode:
					App.SysCtrlPara.CircleModeSet++;
				if(App.SysCtrlPara.CircleModeSet>CIRCLEMODE_IN)
					App.SysCtrlPara.CircleModeSet = CIRCLEMODE_AUTO;
				PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
            break;
        case KeyHeater :
#ifdef __EXCHANGE_FLOWS
		  App.SysCtrlPara.VentilateRate++;
		  if(App.SysCtrlPara.VentilateRate>RATE10TO12)
				App.SysCtrlPara.VentilateRate=RATE10TO08;
			PostMessage(MessageParaUpdate, PARA_VENTILATE);
#else
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
#endif
            break;

            
            
        case KeyPlus:
            App.SysCtrlPara.AirFlowSet++;
						if(App.SysCtrlPara.AirFlowSet>CTRLFLOW_STEPS)
							App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEPS;
						App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
//						PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
						SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
            break;
        case KeyMinus:
             App.SysCtrlPara.AirFlowSet--;
						if(App.SysCtrlPara.AirFlowSet<CTRLFLOW_STEP)
							App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEP;
						App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
//						PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
						SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
            break;
				case KeyLongPlus:
					break;
				case KeyLongPlusMinus:
#ifdef __WIFI_VALIDE
						PostMessage(MessageWifiCtrl, HekrConfig);
#endif
					break;
				default:
					break;
         
    }
}

