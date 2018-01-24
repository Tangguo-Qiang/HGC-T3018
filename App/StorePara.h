#ifndef __STOREPARA_H
	#define	__STOREPARA_H

#include "app.h"

#define SIZEOF_USEDTIMER    (sizeof(App.UsedHours))									//16 bytes
#define SIZEOF_SYSPARA      (sizeof(App.SysCtrlPara))									//16 bytes
#define SIZEOF_FILTERTIMER  (sizeof(App.FilterTimerHour))		//12 bytes
#define SIZEOF_FILTERLIMIT  (sizeof(App.FilterHourLimit))		//4 bytes
#define SIZEOF_ENVPARA  		(sizeof(App.SysCtrlLine))		//180 bytes
#define SIZEOF_WEEKTIMER  	(sizeof(App.WeekTimerMode))		
#define SIZEOF_WORKDAYMODE  (sizeof(App.DayTimerMode.WorkDayTimer))		
#define SIZEOF_WEEKENDMODE  (sizeof(App.DayTimerMode.WeekendTimer))		
#define SIZEOF_HOLIDAYMODE  (sizeof(App.DayTimerMode.HolidayTimer))		
#define SIZEOF_PARTYMODE  	(sizeof(App.DayTimerMode.OfficeTimer))		
#define SIZEOF_ENERGYMODE   (sizeof(App.DayTimerMode.EnergyTimer))		



#define EEPROM_ADDRESS_START   				0x01		//180 bytes
#define EEPROM_ADDRESS_USEDTIMER     (uint16_t)EEPROM_ADDRESS_START									//16 bytes
#define EEPROM_ADDRESS_SYSPARA      (EEPROM_ADDRESS_START+SIZEOF_USEDTIMER)									//16 bytes
#define EEPROM_ADDRESS_FILTERTIMER  (EEPROM_ADDRESS_SYSPARA+SIZEOF_SYSPARA)		//12 bytes
#define EEPROM_ADDRESS_FILTERLIMIT  (EEPROM_ADDRESS_FILTERTIMER+SIZEOF_FILTERTIMER)		//4 bytes
#define EEPROM_ADDRESS_ENVPARA  		(EEPROM_ADDRESS_FILTERLIMIT+SIZEOF_FILTERLIMIT)		//180 bytes
#define EEPROM_ADDRESS_WEEKTIMER  	(EEPROM_ADDRESS_ENVPARA+SIZEOF_ENVPARA)		//180 bytes
#define EEPROM_ADDRESS_WORKDAYMODE  (EEPROM_ADDRESS_WEEKTIMER+SIZEOF_WEEKTIMER)		//180 bytes
#define EEPROM_ADDRESS_WEEKENDMODE  (EEPROM_ADDRESS_WORKDAYMODE+SIZEOF_WORKDAYMODE)		//180 bytes
#define EEPROM_ADDRESS_HOLIDAYMODE  (EEPROM_ADDRESS_WEEKENDMODE+SIZEOF_WEEKENDMODE)		//180 bytes
#define EEPROM_ADDRESS_PARTYMODE  	(EEPROM_ADDRESS_HOLIDAYMODE+SIZEOF_HOLIDAYMODE)		//180 bytes
#define EEPROM_ADDRESS_ENERGYMODE   (EEPROM_ADDRESS_PARTYMODE+SIZEOF_PARTYMODE)		//180 bytes
#define EEPROM_ADDRESS_AVAILABLE   (EEPROM_ADDRESS_ENERGYMODE+SIZEOF_ENERGYMODE)		//180 bytes

//#define EEPROM_ADDRESS_INITFLAG  (uint16_t)0x3FF
#define EEPROM_ADDRESS_INITFLAG  (uint8_t)0x00

//#define STORE_PARA_NONE				0x00000000
//#define STORE_PARA_ALL				0x0000000F
//#define STORE_PARA_SYSPARA				0x00000001
//#define STORE_PARA_CTRLPARA       0x00000002
//#define STORE_PARA_FILTERTIMER    0x00000004   
//#define STORE_PARA_WEEKTIMER      0x00000008  

typedef enum{
	STORE_USEDTIMER=(uint8_t)0,
	STORE_SYSPARA=(uint8_t)1,
	STORE_FILTERTIMER=(uint8_t)2,
	STORE_FILTERLIMIT=(uint8_t)3,
	STORE_ENVPARA =(uint8_t)4,
//	STORE_WEEKTIMER =(uint8_t)5,
//	STORE_WORKDAYMODE =(uint8_t)6,
//	STORE_WEEKENDMODE =(uint8_t)7,
//	STORE_HOLIDAYMODE =(uint8_t)8,
//	STORE_PARTYMODE =(uint8_t)9,
//	STORE_ENERGYMODE =(uint8_t)10,
	STORE_PARAINIT 
}StorePara_TypDef;

typedef enum{
	READFROMSTORE=(uint8_t)1,
	WRITETOSTORE=(uint8_t)2
}StoreOpt_TypDef;



void InitPara(void);
TestStatus StoreParaOpt(StorePara_TypDef paratype,StoreOpt_TypDef oper);
void StorePara(void);
void StorePost(StorePara_TypDef paratype);


#endif
