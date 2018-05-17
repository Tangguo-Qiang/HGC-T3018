#include "app.h"


/*******************************************************************************
* 描述	    : 按键消息处理函数
* 输入参数  : key，任意按键值
*******************************************************************************/
void KeyProcessMainForm(KeyActEnum key)
{
	if(App.SysCtrlPara.Power == POWER_SLEEP)
	{
		System.Device.Beep.BeepOn(BEEP_SHORT);
		if(App.SysCtrlPara.MuteSet == MUTEMODE_ACT)
		{
			App.SysCtrlPara.MuteSet = MUTEMODE_OFF;
			PostMessage(MessageParaUpdate, PARA_MUTEMODE);
		}
		else
		{
			System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
			System.Device.Led.LedModeSet(LED_RED,TURN_OFF);
			App.SysCtrlPara.Power = POWER_ON;
		}
		System.Device.Key.KeyLedSet(5);
		LCD_BL_ON;
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
					if(App.SysCtrlPara.Power == POWER_OFF)
					{
						App.SysCtrlPara.Power = POWER_ON;
							System.Device.Beep.BeepOn(BEEP_POWERON);

						System.Device.Key.KeyLedSet(5);
					}
					else
					{
						App.SysCtrlPara.Power = POWER_OFF;
							System.Device.Beep.BeepOn(BEEP_POWEROFF);
						System.Device.Key.KeyLedSet(1);
					}
					PostMessage(MessageParaUpdate, PARA_POWER_SET);
            break;
        case KeyPowerPlus:
        case KeyLongPowerPlus:
					System.Device.Key.KeyLedSet(5);
					System.Device.Beep.BeepOn(BEEP_LONG);
					App.SysCtrlPara.CircleModeSet++;
					if(App.SysCtrlPara.CircleModeSet>CIRCLEMODE_IN)
						App.SysCtrlPara.CircleModeSet = CIRCLEMODE_AUTO;
					PostMessage(MessageParaUpdate, PARA_CIRCLEMODE);
            break;
				
        case KeyPowerMinus :
        case KeyLongPowerMinus :
					System.Device.Key.KeyLedSet(5);
					System.Device.Beep.BeepOn(BEEP_LONG);
 #ifdef HD_GJ_180Z
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
            break;

            
            
        case KeyPlus:
					System.Device.Key.KeyLedSet(5);
//					System.Device.Beep.BeepOn(BEEP_SHORT);
            App.SysCtrlPara.AirFlowSet++;
						if(App.SysCtrlPara.AirFlowSet>CTRLFLOW_STEPS)
							App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEPS;
						App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
						App.Menu.MainForm.LoadFresh |= 0x02;
//						PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
						SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
            break;
        case KeyMinus:
					System.Device.Key.KeyLedSet(5);
//					System.Device.Beep.BeepOn(BEEP_SHORT);
             App.SysCtrlPara.AirFlowSet--;
						if(App.SysCtrlPara.AirFlowSet<CTRLFLOW_STEP)
							App.SysCtrlPara.AirFlowSet = CTRLFLOW_STEP;
						App.SysCtrlPara.AirFlowRun=App.SysCtrlPara.AirFlowSet;
//						PostMessage(MessageParaUpdate, PARA_XFMOTODUTY);
						App.Menu.MainForm.LoadFresh |= 0x02;
						SeqOperDelay =SEQUENCEOPER_DELAY_100MS;
            break;
				case KeyLongPlus:
					break;
				case KeyPlusMinus:
				case KeyLongPlusMinus:
#ifdef __WIFI_VALIDE
					PostMessage(MessageWifiCtrl, HekrConfig);
				System.Device.Key.KeyLedSet(5);
				System.Device.Beep.BeepOn(BEEP_3SHORT);
#endif
#ifdef __RF_VALIDE
					PostMessage(MessageRfCtrl, RfConfig);
				System.Device.Key.KeyLedSet(5);
				System.Device.Beep.BeepOn(BEEP_3SHORT);
#endif					
				break;
				case KeyLongPower:	
					System.Device.Key.KeyLedSet(5);
					if(App.SysCtrlPara.Power == POWER_ON)
					{
							System.Device.Beep.BeepOn(BEEP_3SHORT);
							App.Menu.FaultForm.LoadFresh = FocusOn_START;
							App.Menu.FocusFormPointer= &App.Menu.FaultForm;
					}
					break;
				default:
					break;
         
    }
}

void KeyProcessFaultForm(KeyActEnum key)
{
		switch(key)
		{
			case KeyMode:
			case KeyHeater:
				break;
			case KeyMinus:
				System.Device.Beep.BeepOn(BEEP_SHORT);
				App.Menu.FaultForm.LoadFresh |= FocusOn_2ND;
				System.Device.Key.KeyLedSet(5);
				break;
			case KeyPlus:
				System.Device.Beep.BeepOn(BEEP_SHORT);
				App.Menu.FaultForm.LoadFresh |= FocusOn_3RD;
				System.Device.Key.KeyLedSet(5);
				break;
			case KeyLongMode:
			case KeyLongPlus:
			case KeyLongMinus:				
				break;
			case KeyLongPlusMinus:

				break;
			case KeyPower:
				System.Device.Beep.BeepOn(BEEP_LONG);
				App.Menu.MainForm.LoadFresh = FocusOn_START;
				App.Menu.FocusFormPointer= &App.Menu.MainForm;
				System.Device.Key.KeyLedSet(5);
			  break;
			case KeyLongPower:
				App.SysCtrlPara.Power = POWER_OFF;
				System.Device.Beep.BeepOn(BEEP_POWEROFF);
				System.Device.Key.KeyLedSet(1);
				PostMessage(MessageParaUpdate, PARA_POWER_SET);
				break;
				default:
					break;
		}
	
}

