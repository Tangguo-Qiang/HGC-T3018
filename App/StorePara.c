#include "app.h"

#define SYSTEM_INITFLAG  (uint8_t)0x33
	
TestStatus StoreParaOpt(StorePara_TypDef paratype,StoreOpt_TypDef oper)
{
	uint8_t* pBuffer=0;
	uint16_t OperAddr=0;
	uint16_t NumByteToOper=0; //max=1023
	TestStatus flag=FAILED;
	
	switch(paratype)
	{
		case STORE_USEDTIMER:
			pBuffer = (uint8_t*)&(App.UsedHours);
			OperAddr = (uint16_t)EEPROM_ADDRESS_USEDTIMER;
			NumByteToOper = SIZEOF_USEDTIMER;
			break;
		case STORE_SYSPARA:
			pBuffer = (uint8_t*)&(App.SysCtrlPara);
			OperAddr = (uint16_t)EEPROM_ADDRESS_SYSPARA;
			NumByteToOper = SIZEOF_SYSPARA;
			break;
		case STORE_FILTERTIMER:
			pBuffer = (uint8_t*)&App.FilterTimerHour;
			OperAddr = EEPROM_ADDRESS_FILTERTIMER;
			NumByteToOper = SIZEOF_FILTERTIMER;
			break;
		case STORE_FILTERLIMIT:
			pBuffer = (uint8_t*)&App.FilterHourLimit;
			OperAddr = EEPROM_ADDRESS_FILTERLIMIT;
			NumByteToOper = SIZEOF_FILTERLIMIT;
			break;
		case STORE_ENVPARA:
			pBuffer = (uint8_t*)&App.SysCtrlLine;
			OperAddr = EEPROM_ADDRESS_ENVPARA;
			NumByteToOper = SIZEOF_ENVPARA;
			break;
//		case STORE_WEEKTIMER:
//			pBuffer = (uint8_t*)&App.WeekTimerMode;
//			OperAddr = EEPROM_ADDRESS_WEEKTIMER;
//			NumByteToOper = SIZEOF_WEEKTIMER;
//			break;
//		case STORE_WORKDAYMODE:
//			pBuffer = (uint8_t*)&App.DayTimerMode.WorkDayTimer;
//			OperAddr = EEPROM_ADDRESS_WORKDAYMODE;
//			NumByteToOper = SIZEOF_WORKDAYMODE;
//			break;
//		case STORE_WEEKENDMODE:
//			pBuffer = (uint8_t*)&App.DayTimerMode.WeekendTimer;
//			OperAddr = EEPROM_ADDRESS_WEEKENDMODE;
//			NumByteToOper = SIZEOF_WEEKENDMODE;
//			break;
//		case STORE_HOLIDAYMODE:
//			pBuffer = (uint8_t*)&App.DayTimerMode.HolidayTimer;
//			OperAddr = EEPROM_ADDRESS_HOLIDAYMODE;
//			NumByteToOper = SIZEOF_HOLIDAYMODE;
//			break;
//		case STORE_PARTYMODE:
//			pBuffer = (uint8_t*)&App.DayTimerMode.OfficeTimer;
//			OperAddr = EEPROM_ADDRESS_PARTYMODE;
//			NumByteToOper = SIZEOF_PARTYMODE;
//			break;
//		case STORE_ENERGYMODE:
//			pBuffer = (uint8_t*)&App.DayTimerMode.EnergyTimer;
//			OperAddr = EEPROM_ADDRESS_ENERGYMODE;
//			NumByteToOper = SIZEOF_ENERGYMODE;
//			break;
		case STORE_RFCONNECT:
			pBuffer = (uint8_t*)&App.RfAddrID;
			OperAddr = EEPROM_ADDRESS_RFADDRID;
			NumByteToOper = SIZEOF_RFADDRID;
			break;

		default:
			break;
	}
	if(NumByteToOper>0)
	{
		if(oper==READFROMSTORE)
			flag = System.Device.Eeprom.I2C_EE_BufferRead(pBuffer,  OperAddr,  NumByteToOper);
		else if(oper==WRITETOSTORE)
			flag = System.Device.Eeprom.I2C_EE_BufferWrite(pBuffer,  OperAddr,  NumByteToOper);
	}
	else
		flag = PASSED;
	
	return flag;
}


TestStatus IfSysInit(void)
{
	uint8_t flag=0;
	
	System.Device.Eeprom.I2C_EE_BufferRead(&flag,  EEPROM_ADDRESS_INITFLAG,  1);
	if(flag == SYSTEM_INITFLAG)
		return PASSED;
	else
	{
		flag= SYSTEM_INITFLAG;
		System.Device.Eeprom.I2C_EE_BufferWrite(&flag,  EEPROM_ADDRESS_INITFLAG,  1);
		return FAILED;
	}
	
}

void StorePost(StorePara_TypDef paratype)
{
	byte i;
	uint val = 1;
	
	i = (byte)paratype;
	App.StoreState |= val<<i;
}

void StorePara(void)
{
	byte i;
	uint val=0x00000001;
	TestStatus flag;
	
	if(App.StoreState)
	{
		for(i=0;i<32;i++)
		{
			if(App.StoreState&val)
				break;
			else
				val <<= 1;
		}
		switch(i)
		{
			case STORE_USEDTIMER:
				flag = StoreParaOpt(STORE_USEDTIMER,WRITETOSTORE);
				break;
			case STORE_SYSPARA:
				flag = StoreParaOpt(STORE_SYSPARA,WRITETOSTORE);
				break;
			case STORE_FILTERTIMER:
				flag = StoreParaOpt(STORE_FILTERTIMER,WRITETOSTORE);
				break;
			case STORE_FILTERLIMIT:
				flag = StoreParaOpt(STORE_FILTERLIMIT,WRITETOSTORE);
				break;
			case STORE_ENVPARA:
				flag = StoreParaOpt(STORE_ENVPARA,WRITETOSTORE);
				break;
//			case STORE_WEEKTIMER:
//				flag = StoreParaOpt(STORE_WEEKTIMER,WRITETOSTORE);
//				break;
//			case STORE_WORKDAYMODE:
//				flag = StoreParaOpt(STORE_WORKDAYMODE,WRITETOSTORE);
//				break;
//			case STORE_WEEKENDMODE:
//				flag = StoreParaOpt(STORE_WEEKENDMODE,WRITETOSTORE);
//				break;
//			case STORE_HOLIDAYMODE:
//				flag = StoreParaOpt(STORE_HOLIDAYMODE,WRITETOSTORE);
//				break;
//			case STORE_PARTYMODE:
//				flag = StoreParaOpt(STORE_PARTYMODE,WRITETOSTORE);
//				break;
//			case STORE_ENERGYMODE:
//				flag = StoreParaOpt(STORE_ENERGYMODE,WRITETOSTORE);
//				break;
			case STORE_RFCONNECT:
				flag = StoreParaOpt(STORE_RFCONNECT,WRITETOSTORE);
				break;
			default:
				break;
		}
		if(flag == FAILED)
			App.StoreState |= val;
		else
			App.StoreState &= ~val;
	}
}

static TestStatus CheckStore(void)
{
	byte i;
	ushort* pushort;
	
	pushort = (ushort*)&App.FilterTimerHour.FirstFilterTimerHour;
	for(i=0;i<4;i++)
	{
		if(*pushort>9999)
			return FAILED;
		pushort++;
	}
	
	pushort = (ushort*)&App.FilterHourLimit.FirstFilterHourLimit;
	for(i=0;i<4;i++)
	{
		if((*pushort>5000)||(*pushort<200))
		{
			App.FilterHourLimit.FirstFilterHourLimit = 2000;
			App.FilterHourLimit.MidFilterHourLimit = 2000;
			App.FilterHourLimit.ESPFilterHourLimit = 1000;
			App.FilterHourLimit.HPFilterHourLimit = 3000;
			StoreParaOpt(STORE_FILTERLIMIT,WRITETOSTORE);
			i=4;
//			return FAILED;
		}
		pushort++;
	}
	

	return PASSED;
}

void InitPara(void)
{
	StorePara_TypDef paratyp;
	TestStatus flag;
	byte i;
	
	
	if(IfSysInit()==FAILED)
	{
		i=3;
		while(--i)
		{
			for(paratyp=STORE_SYSPARA;paratyp<STORE_PARAINIT;paratyp++)
			{
				flag =StoreParaOpt(paratyp,WRITETOSTORE);		
				if(flag == FAILED)
					break;
			}
			if(flag==PASSED)
				break;
		}
		
		if(flag == FAILED)
		{
			System.Device.Led.LedModeSet(LED_DOUBLE,TURN_ON);
			while(1){};
		}
	}
	
	for(paratyp=STORE_SYSPARA;paratyp<STORE_PARAINIT;paratyp++)
	{
		flag =StoreParaOpt(paratyp,READFROMSTORE);	
		if(flag == FAILED)
			break;
	}
	if(flag == FAILED)
	{
		System.Device.Led.LedModeSet(LED_DOUBLE,TURN_ON);
		while(1){};
	}
	 
	i=5;
	while(i--)
	{
	  if(CheckStore()==FAILED)
		{
			for(paratyp=STORE_SYSPARA;paratyp<STORE_PARAINIT;paratyp++)
			{
				flag =StoreParaOpt(paratyp,READFROMSTORE);	
				if(flag == FAILED)
					break;
			}
		}
		else
			break;
	}
	if(!i)
	{
		App.SysState.FaultFlag |= STORE_FAULT;
		
		App.FilterTimerHour.FirstFilterTimerHour =0;
		App.FilterTimerHour.MidFilterTimerHour =0;
		App.FilterTimerHour.ESPFilterTimerHour =0;
		App.FilterTimerHour.HPFilterTimerHour  =0;
		
		App.FilterHourLimit.FirstFilterHourLimit = 2000;
		App.FilterHourLimit.MidFilterHourLimit = 2000;
		App.FilterHourLimit.ESPFilterHourLimit = 1000;
		App.FilterHourLimit.HPFilterHourLimit = 3000;
	}
	
	if(App.SysCtrlPara.Power != POWER_OFF)
		App.SysCtrlPara.Power = POWER_ON;
	
	if(App.SysCtrlPara.AirFlowSet>CTRLFLOW_STEPS)
		App.SysCtrlPara.AirFlowSet=CTRLFLOW_STEPS;
	
	if((App.SysCtrlLine.CO2InsideLine<800)||(App.SysCtrlLine.CO2InsideLine>1200))
		App.SysCtrlLine.CO2InsideLine=1000;
	if((App.SysCtrlLine.PMInsideLine <100)||(App.SysCtrlLine.PMInsideLine >250))
		App.SysCtrlLine.PMInsideLine =150;
	if((App.SysCtrlLine.TempInsideSummerLine<25)||(App.SysCtrlLine.TempInsideSummerLine>29))
		App.SysCtrlLine.TempInsideSummerLine=25;
	if((App.SysCtrlLine.TempInsideWinterLine<14)||(App.SysCtrlLine.TempInsideWinterLine>18))
		App.SysCtrlLine.TempInsideWinterLine=18;
	
	PostMessage(MessageProcess, FILTER_POWERON);
//	PostMessage(MessageParaUpdate, PARA_POWER_SET);
		

}
