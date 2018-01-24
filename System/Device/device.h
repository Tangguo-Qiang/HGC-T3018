/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.3.1
  * @date    17-January-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H
#define __DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"


/* Exported types ------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
/* Exported constants --------------------------------------------------------*/



#define AIRFLOWCTRL_RUNSTEP_PERSENT	100   //1%

#define MOTO_STARTDUTY	6000

#define LCD_BL_ON   {GPIOC->BSRR = GPIO_Pin_9;GPIOA->BSRR = GPIO_Pin_8;}
#define LCD_BL_OFF  {GPIOC->BRR  = GPIO_Pin_9;GPIOA->BRR = GPIO_Pin_8;} 

typedef enum 
{
    KeyPlus              = 0x01, //key1
    KeyLongPlus          = 0x31, //key1
    KeyMinus             = 0x02, //key2
    KeyLongMinus         = 0x32, //key2
    KeyMode              = 0x03, //key3
    KeyLongMode          = 0x33, //key3
    KeyPower             = 0x04, //key4
    KeyLongPower         = 0x34, //key4
    KeyHeater             = 0x05, //key5
    KeyLongHeater         = 0x35, //key5
	
	  KeyModeMinus         =0x06,
	  KeyLongModeMinus     =0x36,
	  KeyModePlus          =0x07,
	  KeyLongModePlus      =0x37,
	  KeyPlusMinus         =0x08,
	  KeyLongPlusMinus     =0x38,
	  KeyModeHeater     =0x09,
	  KeyLongModeHeater     =0x39

    
	
}KeyActEnum;

typedef enum 
{
    TimerSystick            = 0,
    TimerMessage            = 1	
}TimerModeEnum;	

typedef enum
{
    Systick10000,
    Systick1000,
    Systick100,
    Systick10
}SystickEnum;

typedef enum{
	TEMP_TWINKLE = (byte)0x01,
	LED_TWINKLE = (byte)0x02,
	TURN_ON = (byte)0x03,
	TURN_OFF = (byte)0x04,
	BREATH_ON = (byte)0x05
}LedModeTypedef;

typedef enum{
	LED_RED = (byte)0x01,
	LED_GREEN = (byte)0x02,
	LED_DOUBLE = (byte)0x03
}LedTypedef;


//typedef enum{
//        AIRFLOW_STOP=(byte)0x00,                 /* 0 */
//        AIRFLOW_STEP1=(byte)0x01,                 /* 0 */
//        AIRFLOW_STEP2=(byte)0x02,                /* 1 */
//        AIRFLOW_STEP3=(byte)0x03,                /* 2 */
//        AIRFLOW_STEP4=(byte)0x04,                /* 3 */
//        AIRFLOW_STEP5=(byte)0x05,                /* 4 */
//        AIRFLOW_STEP6=(byte)0x06,                /* 5 */
//        AIRFLOW_STEP7=(byte)0x07,                /* 6 */
//        AIRFLOW_STEP8=(byte)0x08,                /* 7 */
//        AIRFLOW_STEP9=(byte)0x09,                /* 8 */
//        AIRFLOW_STEP10=(byte)0x0A,                /* 9 */
//        AIRFLOW_STEP11=(byte)0x0B,                /* 10 */
//        AIRFLOW_STEP12=(byte)0x0C,                /* 11 */
//        AIRFLOW_STEP13=(byte)0x0D,                /* 12 */
//        AIRFLOW_STEP14=(byte)0x0E,                /* 13 */
//        AIRFLOW_STEP15=(byte)0x0F,                /* 14 */
//        AIRFLOW_STEP16=(byte)0x10,                /* 15 */
//        AIRFLOW_STEP17=(byte)0x11,                /* 16 */
//        AIRFLOW_STEP18=(byte)0x12,                /* 17 */
//        AIRFLOW_STEP19=(byte)0x13,                /* 18 */
//        AIRFLOW_STEP20=(byte)0x14,               /* 19 */
//}AirFlowSetEnum;



typedef	enum
{
	RecvDataSumCheckErr = (byte)0x01,
	LastFrameSendErr = (byte)0x02,
	MCU_UploadACK = (byte)0x03,
	ValidDataUpdate = (byte)0x04,
	RecvDataUseless = (byte)0x05,
	HekrModuleStateUpdate = (byte)0x06,
	MCU_ControlModuleACK = (byte)0x07,
	MCU_ProdKeyModuleACK = (byte)0x08
}RecvDataHandleCode;

//HekrÄ£¿é¿ØÖÆÂë
typedef	enum
{
	ModuleQuery = 0x01,
	ModuleRestart = 0x02,
	ModuleRecover = 0x03,
	HekrConfig = 0x04,
	HekrProdKey = 0x21
}HekrModuleControlCode;

#define WIFIUSER_MAX_LEN 50u

typedef enum{
	SWITCH_AC_RS1=(uint8_t)0x01,
	SWITCH_HEAT1=(uint8_t)0x02,
	SWITCH_HEAT2=(uint8_t)0x04,
	SWITCH_ESP=(uint8_t)0x08,
	SWITCH_12V_RS1=(uint8_t)0x10,
	SWITCH_12V_RS2=(uint8_t)0x20
}SwitchTypedef;

typedef struct
{
	byte Addr;
	byte ComBit;
}LCD_DispTypeDef;


//typedef enum{
//	NOFLOW=(byte)0,
//	STARTFLOW=(byte)0x01,
//	STOPFLOW=(byte)0x02
//}AlarmGoalTypeDef;

//typedef struct{
//	byte Nextnum;
//	AlarmGoalTypeDef Alarmgoal;
//	AirFlowSetEnum AirflowSet;
//}AlarmSysTypeDef;

typedef enum{
	POWER_EMPTY =(byte)0x00,
	POWER_ON =(byte)0x01,
	POWER_OFF= (byte)0x02,
	POWER_SLEEP = (byte)0x03,
	POWER_WAKEUP = (byte)0x04
}PowerSetTypedef;


typedef enum{
	RATEEMPTY  = (byte)0x00,
	RATE10TO08 = (byte)0x01,
	RATE10TO10 = (byte)0x02,
	RATE10TO12 = (byte)0x03
}VentilateRateTypedef;

typedef enum{
	CIRCLEMODE_EMPTY = (byte)0x00,
	CIRCLEMODE_AUTO = (byte)0x01,
	CIRCLEMODE_OUT = (byte)0x02,
	CIRCLEMODE_IN = (byte)0x03
}CircleModeSetTypedef;

typedef enum{
	TEMPMODE_EMPTY = (byte)0x00,
	TEMPMODE_AUTO = (byte)0x01,
	TEMPMODE_ON = (byte)0x02,
	TEMPMODE_OFF = (byte)0x03
}TempModeTypedef;


typedef enum{
	CHILD_UNLOCK = (byte)0x00,
	CHILD_LOCK = (byte)0x01
}ChildLockTypedef;

typedef enum{
	HEATER_OFF = (byte)0x00,
	HEATER_HALF = (byte)0x01,
	HEATER_FULL = (byte)0x02
}AuxiliaryHeatTypedef;

typedef enum{
	TEMPTIMER_ON = 0x00,
	WEEKTIMER_ON = 0x01,
	ALLBYHAND = 0x02
}RunModeTypedef;

typedef enum{
	MUTEMODE_OFF = (byte)0x00,
	MUTEMODE_ACT = (byte)0x01,
}MutSetTypedef;


typedef enum{
	FIRSTFILTER_CHECK = (uint)0x01,
	MIDFILTER_CHECK = (uint)0x02,
	ESPFILTER_CHECK = (uint)0x04,
	HPFILTER_CHECK = (uint)0x08,
	POWERBASE_FAULT = (uint)0x10,
	SENSOROUTTEMP_FAULT = (uint)0x20,
	SENSORINTEMP_FAULT = (uint)0x40,
	SENSORRH_FAULT = (uint)0x80,
	SENSORCO2_FAULT = (uint)0x100,
	SENSORPM_FAULT = (uint)0x200,
	XFMOTO_FAULT = (uint)0x400,
	PFMOTO_FAULT = (uint)0x800,
	CO2INSIDEBEYOND=(uint)0x1000,
	PMINSIDEBEYONG=(uint)0x2000,
	HGA_FAULT = (uint)0x4000,
	STORE_FAULT=(uint)0x8000,

	FAULTICON_DISP = (uint)0x10000
}FaultTypedef;


typedef struct 
{
  signed char TempInside;                 
  byte RHInside;
  signed char TempOutside;
  ushort CO2Inside;
  ushort PMInside;
}SensorDataTypedef;


typedef	struct 
{
	PowerSetTypedef Power;
	CircleModeSetTypedef CircleModeSet;
	TempModeTypedef ThermalModeSet;
	AuxiliaryHeatTypedef AuxiliaryHeatSet;
	VentilateRateTypedef VentilateRate;
	byte  AirFlowSet; 
	byte  AirFlowRun; 
	byte ShutTimer;
	MutSetTypedef MuteSet;
	ChildLockTypedef ChildLock;
}SysCtrlParaTypedef; 

typedef struct
{
	uint WifiState;
	uint RFState;
	uint FaultFlag;
	uint FilterWarning;
}SysStateTypedef;


#define WIFI_STATE					0x00000007
#define WIFI_STATE_CONFIG		0x00000001
#define WIFI_STATE_STA			0x00000002
#define WIFI_STATE_STA_AP		0x00000004


#define RF_STATE					0x00000007
#define RF_STATE_CONFIG		0x00000001
#define RF_STATE_STA			0x00000002
#define RF_STATE_STA_AP		0x00000004

extern void InitDevice(void);

#endif 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
