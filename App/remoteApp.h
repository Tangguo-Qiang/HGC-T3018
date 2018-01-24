#ifndef __REMOTEAPP_H
 #define __REMOTEAPP_H

#include "system.h"
#include "app.h"

typedef enum{
	SysParaQuery 		= (byte)0,
	SysParaUpload 	= (byte)1,
	DevPowerSet 	  = (byte)2,
	DevTimeOffSet 	= (byte)3,
	DevCircleModeSet 	= (byte)4,
	DevMuteSet 	    = (byte)5,
	DevChildLockSet = (byte)6,
	DevHeatModeSet 	= (byte)7,
	DevVentilateRateSet 	= (byte)8,
	DevTimeSet 			= (byte)9,
	DevAirflowSet 	= (byte)10,
	DevElcHeatSet 	= (byte)11,
	DevRunModeSet 	= (byte)12,
	
	WeekTimerQuery 	= (byte)20,
	WeekTimerUpload 	= (byte)21,
	WeekTimerSet 		= (byte)22,
	TimerModeQuery 	= (byte)23,
	TimerModeUpload = (byte)24,
	TimerModeSet		= (byte)25,
	
	EnvParaQuery 		= (byte)30,
	EnvParaUpload 	= (byte)31,
	EnvParaSet 			= (byte)32,
	
	FilterQuery 		= (byte)40,
	FilterUpload 		= (byte)41,
	FilterLimitSet 	= (byte)42,
	
	HistoryGraghDataReport = (byte)51,
	SYSFaultReport 	= (byte)52,
	
	TRANSFINISH 		= (byte)0x88
	
}WifiTransDataTypeDef;

#define WIFI_OPER_WEEKTIMERSET		0x80000000
#define WIFI_OPER_TIMERMODESET		0x40000000
#define WIFI_OPER_ENVPARASET			0x20000000
#define WIFI_OPER_FILTERLIMITSET	0x10000000
#define WIFI_OPER_DEVTIMESET			0x08000000
#define WIFI_UPLOAD_SYSPARA				0x04000000
#define WIFI_UPLOAD_WEEKTIMER			0x02000000
#define WIFI_UPLOAD_ENVPARA				0x01000000
#define WIFI_UPLOAD_FILTER				0x00800000
#define WIFI_UPLOAD_GRAPHDATA			0x00400000
#define WIFI_UPLOAD_FAULT					0x00200000

void AppWifiInit(void);
void WifiRecvParse(byte type);
void WifiUpload(void);
void WifiCtrlCode(byte code);

#endif
