#ifndef __APP_H
#define __APP_H





#include "hmi.h"
#include "remoteIR.h"
#include "remoteApp.h"
#include "logic.h"

#include "menu.h"

#include "comm.h"
#include "StorePara.h"



#ifdef __BYPASS_INVERSE
	typedef enum{
		BYPASS_UNKNOW = (byte)CIRCLEMODE_EMPTY,
		BYPASS_CIRCLEOUT = (byte)0x02,//(CIRCLEMODE_IN-1),
		BYPASS_CIRCLEIN = (byte)0x01//(CIRCLEMODE_OUT-1)
	}BypassModeTypedef;
#else
	typedef enum{
		BYPASS_UNKNOW = (byte)CIRCLEMODE_EMPTY,
		BYPASS_CIRCLEOUT = (byte)0x01,//(CIRCLEMODE_OUT-1),
		BYPASS_CIRCLEIN = (byte)0x02//(CIRCLEMODE_IN-1)
	}BypassModeTypedef;
#endif




/*******************************************************************************
* 描述	    : msOS中最重要的一个结构体，封装了App层的所有数据及窗口控件
*******************************************************************************/
typedef struct
{
    SensorDataTypedef SensorData;

		SysCtrlParaTypedef SysCtrlPara; 
			
		struct
		{
			ushort FirstFilterTimerHour;
			ushort MidFilterTimerHour;
			ushort ESPFilterTimerHour;
			ushort HPFilterTimerHour;
		}FilterTimerHour;
		
		struct
		{
			ushort FirstFilterHourLimit;
			ushort MidFilterHourLimit;
			ushort ESPFilterHourLimit;
			ushort HPFilterHourLimit;
		}FilterHourLimit;

		struct
		{
			char TempInsideSummerLine;
			char TempInsideWinterLine;
			ushort PMInsideLine;
			ushort CO2InsideLine;
		}SysCtrlLine;

		uint UsedHours;

//		RTC_TimeTypeDef SysTime;
//		AlarmSysTypeDef AlarmSys;
		
		struct 
		{			
			PowerSetTypedef Power;
			BypassModeTypedef BypassMode;
			AuxiliaryHeatTypedef AuxiliaryHeatSet;
			TempModeTypedef ThermalMode;
			ushort XFmotoPWM; //persent of full speed
			ushort XFmotoRPM; //persent of full speed
			ushort PFmotoPWM; //persent of full speed
			ushort PFmotoRPM; //persent of full speed
		}SysCtrlStatus; 
	
		struct 
		{			
			BypassModeTypedef BypassMode;
			AuxiliaryHeatTypedef AuxiliaryHeat;
			ushort XFmotoPWM; 
			ushort PFmotoPWM; 
			ushort XFmotoRPM; 
			ushort PFmotoRPM; 
			PowerSetTypedef Power;
		}SysRunStatus; 

		
    struct SysFault
    {
        byte PowerBaseFault;                 
        byte TempOutSensors;                 
        byte TempInSensors;                 
        byte RHSensor;
        byte CO2Sensor;
        byte PMSensor;
			  byte MotoXF;
			  byte MotoPF;
			  byte AuxiHeater;
		}SysFault;

		struct SysVersion
		{
			byte CtrlMainVersion;
			byte CtrlSubVersion;
			byte PowerMainVersion;
			byte PowerSubVersion;
		}SysVersion;
		
    FormDispTypeDef Menu;
		
		SysStateTypedef SysState;
		
		uint StoreState;
}AppStruct;

extern AppStruct App;

#endif
