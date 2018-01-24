/*
********************************************************************************
*文件名     : gui.c
*作用       : 图形库文件
*原理       : 面向对象设计，把一个Menu分为多个Form,一个Form包含多个控件，如Chart、
*           : Label、TextBox等，建立一个最低优先级任务MenuTask去解析执行这些控件信息
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"
//#include "Zimo.h"

typedef enum{
	GRID_FAULT = (byte)0x01,
	GRID_RF = (byte)0x02,
	GRID_WIFI = (byte)0x04,
	GRID_LOCK = (byte)0x08,
	GRID_HEATER = (byte)0x10,
	GRID_FILTER = (byte)0x20,
}GridFlagTypedef;

const LCD_DispTypeDef FigInSys[NUM_Full]={
{0,0x01},{0,0x02},{0,0x04},{0,0x08},{0,0x10},{0,0x20},{0,0x40},{0,0x80},
{1,0x01},{1,0x02},{1,0x04},{1,0x08},{1,0x10},{1,0x20},{1,0x40},{1,0x80},
{2,0x01},{2,0x02},{2,0x04},{2,0x08},{2,0x10},{2,0x20},{2,0x40},{2,0x80},
{3,0x01},{3,0x02},{3,0x04},{3,0x08},{3,0x10},{3,0x20},{3,0x40},{3,0x80},
{4,0x01},{4,0x02},{4,0x04},{4,0x08},{4,0x10},{4,0x20},{4,0x40},{4,0x80},
{5,0x01},{5,0x02},{5,0x04},{5,0x08},{5,0x10},{5,0x20},{5,0x40},{5,0x80},
{6,0x01},{6,0x02},{6,0x04},{6,0x08},{6,0x10},{6,0x20},{6,0x40},{6,0x80},
{7,0x01},{7,0x02},{7,0x04},{7,0x08},{7,0x10},{7,0x20},{7,0x40},{7,0x80},
{8,0x01},{8,0x02},{8,0x04},{8,0x08},{8,0x10},{8,0x20},{8,0x40},{8,0x80},
{9,0x01},{9,0x02},{9,0x04},{9,0x08},{9,0x10},{9,0x20},{9,0x40},{9,0x80},
{10,0x01},{10,0x02},{10,0x04},{10,0x08},{10,0x10},{10,0x20},{10,0x40},{10,0x80},
{11,0x01},{11,0x02},{11,0x04},{11,0x08},{11,0x10},{11,0x20},{11,0x40},{11,0x80},
{12,0x01},{12,0x02},{12,0x04},{12,0x08},{12,0x10},{12,0x20},{12,0x40},{12,0x80},
{13,0x01},{13,0x02},{13,0x04},{13,0x08},{13,0x10},{13,0x20},{13,0x40},{13,0x80},
{14,0x01},{14,0x02},{14,0x04},{14,0x08},{14,0x10},{14,0x20},{14,0x40},{14,0x80},
{15,0x01},{15,0x02},{15,0x04},{15,0x08},{15,0x10},{15,0x20},{15,0x40},{15,0x80},
{16,0x01},{16,0x02},{16,0x04},{16,0x08},{16,0x10},{16,0x20},{16,0x40},{16,0x80},
{17,0x01},{17,0x02},{17,0x04},{17,0x08},{17,0x10},{17,0x20},{17,0x40},{17,0x80},
{18,0x01},{18,0x02},{18,0x04},{18,0x08},{18,0x10},{18,0x20},{18,0x40},{18,0x80},
{19,0x01},{19,0x02},{19,0x04},{19,0x08},{19,0x10},{19,0x20},{19,0x40},{19,0x80},
};

/* bit7-0 G F E D C B A  */
const byte NumLed[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

byte GuiTimer100ms=0;
byte GuiTimer10ms=0;

SysCtrlParaTypedef * SysCtrlParaPoint;
SensorDataTypedef * SensorDataPoint;
SysStateTypedef* SysStatusPoint;
FormDispTypeDef* FormDispPoint;

static SysCtrlParaTypedef  SysCtrlParaDisp={POWER_EMPTY,CIRCLEMODE_EMPTY,TEMPMODE_EMPTY,HEATER_OFF,
                                            RATEEMPTY,CTRLFLOW_STOP,CTRLFLOW_STOP,0x0f,MUTEMODE_OFF,CHILD_UNLOCK};
static SensorDataTypedef   SensorDataDisp={0};
static SysStateTypedef SysStateDisp={0};

//static byte GifWheelFlash10ms=0;
//static byte TextStay100ms=0;
static byte FigFlash=0;
static byte GifAnionFlash100ms=0;
//static byte VersionDisp=0;
//static 	byte GridFlag=0;

/**********************************************
static void GifWheelToGui(void)
{
	static byte i=0;
	byte speed;
		
	speed = FormDispPoint->WheelGifSpeed10ms;
	if(speed)
	{
		if(GifWheelFlash10ms==speed)
		{
			GifWheelFlash10ms =0;
			switch(i)
			{
				case 0:
					System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
					break;
				case 1:
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T6]);
					System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
					break;
				case 2:
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T6]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
					System.Device.Lcd.DispUnit(FigInSys[LCD_T8]);
					break;
			}
			System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);
			if(++i==3)
				i=0;
		}
	}
	else
	{
		System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T8]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);
	}    
}
*********************************************************/

static void GifAnionToGui(void)
{
	static byte i=0;
	byte speed;
		
	speed = FormDispPoint->AnionGifSpeed100ms;		
	if(speed)
	{
		if(GifAnionFlash100ms==speed)
		{
			GifAnionFlash100ms =0;
			switch(i)
			{
				case 0:
					System.Device.Lcd.DispUnit(FigInSys[LCD_S1]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S2]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S3]);
					break;
				case 1:
					System.Device.Lcd.DispUnit(FigInSys[LCD_S2]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S1]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S3]);
					break;
				case 2:
					System.Device.Lcd.DispUnit(FigInSys[LCD_S3]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S2]);
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_S1]);
					break;
			}
			if(++i>2)
				i=0;
		}
	}
	else
	{
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_S1]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_S2]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_S3]);
	} 
	
//	if(FigFlash%2)
//	{
//		System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
//		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
//	}
//	else
//	{
//		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T6]);
//		System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
//	}

}

static void DigitToDisp(byte val,byte LocNumD)
{
	byte num;
	
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD]); //D
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+1]); //C
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+2]); //B
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+3]); //A
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+5]); //E
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+6]); //G
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+7]); //F
	
	if(val<10)
	{
		num = NumLed[val];
		if(num&0x01) //A
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+3]);
		if(num&0x02) //B
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+2]);
		if(num&0x04) //C
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+1]);
		if(num&0x08) //D
			System.Device.Lcd.DispUnit(FigInSys[LocNumD]);
		if(num&0x10) //E
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+5]);
		if(num&0x20) //F
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+7]);
		if(num&0x40) //G
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+6]);
	}
}

static void DigitToDispRev(byte val,byte LocNumD)
{
	byte num;
	
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD]); //D
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-1]); //C
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-2]); //B
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-3]); //A
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-5]); //E
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-6]); //G
	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-7]); //F
	
	if(val<10)
	{
		num = NumLed[val];
		if(num&0x01) //A
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-3]);
		if(num&0x02) //B
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-2]);
		if(num&0x04) //C
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-1]);
		if(num&0x08) //D
			System.Device.Lcd.DispUnit(FigInSys[LocNumD]);
		if(num&0x10) //E
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-5]);
		if(num&0x20) //F
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-7]);
		if(num&0x40) //G
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-6]);
	}
	else if(val>10)
	{		
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-6]); //G
	}

}

static void FlowToGuiLcd(void)
{
	byte num=0;
	ushort flowtext;
	
	num = SysCtrlParaPoint->AirFlowRun;
	if(SysCtrlParaDisp.AirFlowRun != num)
	{

		SysCtrlParaDisp.AirFlowRun = num;
		
		flowtext = num* DISPFLOW_ONESTEP;
		flowtext += DISPFLOW_MIN;
		
		num = flowtext/100;
		flowtext %= 100;
		if(!num)
			num=10;		
		DigitToDispRev(num,LCD_8D);
		num = flowtext/10;
		DigitToDispRev(num,LCD_9D);
		num= flowtext%10;
		DigitToDispRev(num,LCD_10D); 
		
		System.Device.Lcd.DispUnit(FigInSys[LCD_T5]);

		/* Display Wheel */
		System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);

	}
}

static void CO2ToGuiLcd(void)
{
	byte num=0,flag=0;
	ushort val;
	
	val = SensorDataPoint->CO2Inside;
	if(SensorDataDisp.CO2Inside!=val)
	{
		SensorDataDisp.CO2Inside =val;
		num = val/1000;
		val %= 1000;
		if(!num)
			num=10;
    else
      flag =1;			
		DigitToDispRev(num,LCD_4D);
		
		num = val/100;
		val %= 100;
		if((!num)&&(!flag))
			num=10;		
		DigitToDispRev(num,LCD_5D);
		num = val/10;
		val %= 10;
		DigitToDispRev(num,LCD_6D);
		DigitToDispRev(val,LCD_7D); 
		
		System.Device.Lcd.DispUnit(FigInSys[LCD_T2]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T4]);
	}
	
}

static void PM25ToGuiLcd(void)
{
	byte num=0,flag=0;
	ushort val;
	
	val = SensorDataPoint->PMInside;
	if(SensorDataDisp.PMInside!=val)
	{
		SensorDataDisp.PMInside =val;
	
		num = val/100;
		val %= 100;
		if(!num)
			num=10;
    else
      flag =1;			
		DigitToDispRev(num,LCD_1D);
		num = val/10;
		val %= 10;
		if((!num)&&(!flag))
			num=10;		
		DigitToDispRev(num,LCD_2D);
		DigitToDispRev(val,LCD_3D); 
		
		System.Device.Lcd.DispUnit(FigInSys[LCD_T1]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T3]);
	}
}


static void TempRHToGui(void)
{
	signed char val;
	byte num;
		
	val = SensorDataPoint->RHInside;
	if(SensorDataDisp.RHInside != val)
	{
		SensorDataDisp.RHInside = val;	
		System.Device.Lcd.DispUnit(FigInSys[LCD_T10]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T14]);
		num = val/10;
		DigitToDisp(num,LCD_15D);
		num = val%10;
		DigitToDisp(num,LCD_16D);
	}		

	val = SensorDataPoint->TempOutside;
	if(SensorDataDisp.TempOutside != val)
	{
		SensorDataDisp.TempOutside = val;
		if(val<0)
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T13]);
			val *= -1;
		}
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T13]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T12]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T15]);
		num = val/10;
		DigitToDisp(num,LCD_11D);
		num = val%10;
		DigitToDisp(num,LCD_12D);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T34]);
	}

	val = SensorDataPoint->TempInside;
	if(SensorDataDisp.TempInside != val)
	{
		SensorDataDisp.TempInside = val;
		if(val<0)
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T32]);
			val *= -1;
		}
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T32]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T11]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T31]);
		num = val/10;
		DigitToDisp(num,LCD_13D);
		num = val%10;
		DigitToDisp(num,LCD_14D);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T35]);
	}
}



static void CtrlParaToGuiLcd(void)
{
	byte val;
	
	if(SysCtrlParaPoint->CircleModeSet != SysCtrlParaDisp.CircleModeSet)
	{
		SysCtrlParaDisp.CircleModeSet = SysCtrlParaPoint->CircleModeSet;
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T19]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T20]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T21]);	
		switch(SysCtrlParaDisp.CircleModeSet)
		{
			case CIRCLEMODE_AUTO:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T19]);
				break;
			case CIRCLEMODE_OUT:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T21]);
				break;
			case CIRCLEMODE_IN:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T20]);
				break;
			default:
				break;
		}
	}
	
#ifdef __EXCHANGE_FLOWS	
	if(SysCtrlParaPoint->ThermalModeSet != SysCtrlParaDisp.ThermalModeSet)
	{
		SysCtrlParaDisp.ThermalModeSet = SysCtrlParaPoint->ThermalModeSet;
		if(SysCtrlParaDisp.ThermalModeSet==TEMPMODE_ON)
			System.Device.Lcd.DispUnit(FigInSys[LCD_T30]);
		else  // TEMPMODE_OFF:
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T30]);
	}
  else if(SysCtrlParaDisp.ThermalModeSet==TEMPMODE_AUTO)
	{
		if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T30]);
		else if(!FigFlash)
			System.Device.Lcd.DispUnit(FigInSys[LCD_T30]);
	}		
#endif

#ifdef __EXCHANGE_FLOWS	
	if(SysCtrlParaPoint->VentilateRate != SysCtrlParaDisp.VentilateRate)
	{
		SysCtrlParaDisp.VentilateRate = SysCtrlParaPoint->VentilateRate;
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T16]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T17]);
		System.Device.Lcd.NonDispUnit(FigInSys[LCD_T18]);	
		switch(SysCtrlParaDisp.VentilateRate)
		{
			case RATE10TO08:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T16]);
				break;
			case RATE10TO10:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T17]);
				break;
			case RATE10TO12:
				System.Device.Lcd.DispUnit(FigInSys[LCD_T18]);
				break;
			default:
				break;
		}
	}
#endif
	
	if(SysCtrlParaPoint->ShutTimer != SysCtrlParaDisp.ShutTimer)
	{
		SysCtrlParaDisp.ShutTimer = SysCtrlParaPoint->ShutTimer;
		val = SysCtrlParaPoint->ShutTimer;
		SysCtrlParaDisp.ShutTimer = val;
		
		if(!val)
			val = 0xff;
		DigitToDispRev(val,LCD_17D);
			
		System.Device.Lcd.DispUnit(FigInSys[LCD_T22]);
		System.Device.Lcd.DispUnit(FigInSys[LCD_T23]);
	}
	
	if(SysCtrlParaPoint->ChildLock != SysCtrlParaDisp.ChildLock)
	{
		SysCtrlParaDisp.ChildLock = SysCtrlParaPoint->ChildLock;
		if(SysCtrlParaDisp.ChildLock==CHILD_UNLOCK)
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T27]);
				
		}
		else
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T27]);
			
		}			
	}
	
#ifdef __AUXI_HEATER	
	if(SysCtrlParaPoint->AuxiliaryHeatSet != SysCtrlParaDisp.AuxiliaryHeatSet)
	{
			SysCtrlParaDisp.AuxiliaryHeatSet = SysCtrlParaPoint->AuxiliaryHeatSet;
			if(SysCtrlParaDisp.AuxiliaryHeatSet>HEATER_OFF)
			{
				System.Device.Lcd.DispUnit(FigInSys[LCD_T28]);
			}
			else
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T28]);
				
			}
	}
	else if(SysCtrlParaDisp.AuxiliaryHeatSet == HEATER_HALF)
	{
		if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T28]);
		}
		else if(!FigFlash)
			System.Device.Lcd.DispUnit(FigInSys[LCD_T28]);	
	}
#endif
	
	if(SysStatusPoint->FilterWarning != SysStateDisp.FilterWarning)
	{
		SysStateDisp.FilterWarning =SysStatusPoint->FilterWarning;
		if(SysStateDisp.FilterWarning)
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T29]);
			if(SysStateDisp.FilterWarning&FIRSTFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_W1]);	
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_W1]);	
			
			if(SysStateDisp.FilterWarning&MIDFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_W2]);	
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_W2]);
			
			if(SysStateDisp.FilterWarning&ESPFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_W3]);	
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_W3]);
			
			if(SysStateDisp.FilterWarning&HPFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_W4]);	
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_W4]);	
		}			
    else
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T29]);
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_W1]);
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_W2]);
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_W3]);
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_W4]);
		}
		
	}
	
	if(SysStatusPoint->FaultFlag != SysStateDisp.FaultFlag)
	{
		SysStateDisp.FaultFlag =SysStatusPoint->FaultFlag;
		if(SysStateDisp.FaultFlag)  // no faultflag early
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T24]);
		}
    else
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T24]);
		}
			
			System.Device.Lcd.DispUnit(FigInSys[LCD_T5]);
				/* Display Wheel */
			System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);
			
			System.Device.Lcd.DispUnit(FigInSys[LCD_T1]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T2]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T10]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T11]);
			System.Device.Lcd.DispUnit(FigInSys[LCD_T12]);
		
	}
	else if(SysStateDisp.FaultFlag)
	{
		if(SysStateDisp.FaultFlag&(XFMOTO_FAULT|PFMOTO_FAULT))
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T5]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T5]);	
		}
		
		if(SysStateDisp.FaultFlag&HGA_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				/* Display Wheel */
				System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
				System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);
			}
			else if(!FigFlash)
			{
				/* NonDisplay Wheel */
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T6]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T9]);
			}
		}
		
		if(SysStateDisp.FaultFlag&STORE_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T24]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T24]);	
		}

		
		if(SysStateDisp.FaultFlag&SENSORCO2_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T2]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T2]);	
		}
		
		if(SysStateDisp.FaultFlag&SENSORPM_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T1]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T1]);	
		}
		
		if(SysStateDisp.FaultFlag&SENSOROUTTEMP_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T12]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T12]);	
		}
		
		if(SysStateDisp.FaultFlag&SENSORRH_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T10]);
			}
			else if(!FigFlash)
			{
				System.Device.Lcd.DispUnit(FigInSys[LCD_T10]);	
			}
		}
		if(SysStateDisp.FaultFlag&SENSORINTEMP_FAULT)
		{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T11]);
			}
			else if(!FigFlash)
			{
				System.Device.Lcd.DispUnit(FigInSys[LCD_T11]);	
			}
		}
	}
	
	if(SysStatusPoint->WifiState != SysStateDisp.WifiState)
	{
		SysStateDisp.WifiState =SysStatusPoint->WifiState;
		if(SysStateDisp.WifiState & (WIFI_STATE_STA|WIFI_STATE_CONFIG))
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T26]);	
		}
		else
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T26]);	
		}
	}
	else if(SysStateDisp.WifiState & WIFI_STATE_CONFIG)
	{
			if(FigFlash==(FormDispPoint->FigFlashSpeed100ms))
			{
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T26]);
			}
			else if(!FigFlash)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T26]);	
	}

	
	if(SysStatusPoint->RFState != SysStateDisp.RFState)
	{
		SysStateDisp.RFState =SysStatusPoint->RFState;
		if(SysStateDisp.RFState & RF_STATE_CONFIG)
		{
				if(FigFlash==(2*FormDispPoint->FigFlashSpeed100ms))
				{
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T25]);
				}
				else if(FigFlash==FormDispPoint->FigFlashSpeed100ms)
					System.Device.Lcd.DispUnit(FigInSys[LCD_T25]);	
		}
		else if(SysStateDisp.RFState & RF_STATE_STA)
		{
			System.Device.Lcd.DispUnit(FigInSys[LCD_T25]);	
		}
		else
		{
			System.Device.Lcd.NonDispUnit(FigInSys[LCD_T25]);	
		}
				
	}
	
	
}

static void ParseForm(void)
{
//	if(GuiTimer10ms)
//	{
//		GifWheelFlash10ms++; 
//		GifWheelToGui();
//	}
	if(GuiTimer100ms)
	{
		GuiTimer100ms=0;
		GifAnionFlash100ms++;
		FigFlash++;
	
	if(FigFlash==(2*FormDispPoint->FigFlashSpeed100ms))
		FigFlash = 0;
		
		CtrlParaToGuiLcd();
		TempRHToGui();
	/* for VersionDisp, co2UPLOAD first*/
		PM25ToGuiLcd();
		CO2ToGuiLcd();
		FlowToGuiLcd();
		GifAnionToGui();
	}
}

static void InitDispSensorData(SensorDataTypedef* SensorData)
{
	SensorDataPoint = SensorData;
//	SensorDataDisp.PMInside=SensorDataPoint->PMInside;
}

static void InitDispSysCtrlPara(SysCtrlParaTypedef* SysCtrlPara)
{
	SysCtrlParaPoint = SysCtrlPara;
	System.Device.Lcd.DispUnit(FigInSys[LCD_SS]);
}

static void InitDispSysStatus(SysStateTypedef* SysStatus)
{
	SysStatusPoint = SysStatus;
}

static void LoadForm(FormDispTypeDef* FormDisp)
{
	FormDispPoint = FormDisp;

}

void InitGui(void)
{
	
  System.Gui.InitDispSensorData = InitDispSensorData;
  System.Gui.InitDispSysCtrlPara = InitDispSysCtrlPara;
  System.Gui.InitDispSysStatus = InitDispSysStatus;
	System.Gui.LoadForm = LoadForm;
  System.Gui.ParseForm = ParseForm;    
    

}



