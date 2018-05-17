#include "system.h"

#define		FLASHSPEED_100MS	((byte)5)
#define 	LEAFSGIF_100MS		((byte)10)

const LCD_DispTypeDef FigInSys[NUM_Full]={
{21,0x01},{21,0x02},{21,0x04},{21,0x08},
{20,0x01},{20,0x02},{20,0x04},{20,0x08},
{19,0x01},{19,0x02},{19,0x04},{19,0x08},
{18,0x01},{18,0x02},{18,0x04},{18,0x08},
{17,0x01},{17,0x02},{17,0x04},{17,0x08},
{16,0x01},{16,0x02},{16,0x04},{16,0x08},
{15,0x01},{15,0x02},{15,0x04},{15,0x08},
{14,0x01},{14,0x02},{14,0x04},{14,0x08},
{13,0x01},{13,0x02},{13,0x04},{13,0x08},
{12,0x01},{12,0x02},{12,0x04},{12,0x08},
{11,0x01},{11,0x02},{11,0x04},{11,0x08},
{10,0x01},{10,0x02},{10,0x04},{10,0x08},
{9,0x01},{9,0x02},{9,0x04},{9,0x08},
{8,0x01},{8,0x02},{8,0x04},{8,0x08},
{7,0x01},{7,0x02},{7,0x04},{7,0x08},
{6,0x01},{6,0x02},{6,0x04},{6,0x08},
{5,0x01},{5,0x02},{5,0x04},{5,0x08},
{4,0x01},{4,0x02},{4,0x04},{4,0x08},
{3,0x01},{3,0x02},{3,0x04},{3,0x08},
{2,0x01},{2,0x02},{2,0x04},{2,0x08},
};

/* bit7-0 G F E D C B A  */
const byte NumLed[12]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x79};
										/*  0,   1,   2,    3,   4,   5,   6,   7,   8,   9, none,  E,*/	
byte GuiTimer100ms=0;
byte GuiTimer10ms=0;

static byte 		GifLeafs100ms=0;
static byte 		FigFlash=0;
static byte 		FaultFlag=0;


static void DigitToDisp(byte val,byte LocNumD)
{
	byte num;
	
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD]); //D
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+1]); //C
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+2]); //B
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+3]); //A
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+5]); //E
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+6]); //G
//	System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+7]); //F
	
	if(val<12)
	{
		num = NumLed[val];
		if(num&0x01) //A
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+3]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+3]);
			
		if(num&0x02) //B
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+2]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+2]);
			
		if(num&0x04) //C
			System.Device.Lcd.DispUnit(FigInSys[LocNumD+1]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+1]);
			
		if(num&0x08) //D
			System.Device.Lcd.DispUnit(FigInSys[LocNumD]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD]);
			
		if(num&0x10) //E
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-3]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-3]);
			
		if(num&0x20) //F
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-1]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-1]);
			
		if(num&0x40) //G
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-2]);
		else
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-2]);
			
	}
	else
	{
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+3]);
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+2]);
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD+1]);
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-3]);
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD-1]);
			System.Device.Lcd.NonDispUnit(FigInSys[LocNumD]);
			System.Device.Lcd.DispUnit(FigInSys[LocNumD-2]);
	}
}


static void AddDispObj(FormTypeDef * formPointer, DispObjType *objPointer)
{
	DispObjType *pointer;
	
	objPointer->ObjNum = FigObj_Null;        
	objPointer->Disp = NormalDisp;
	objPointer->UpdataFlag = TRUE;
	if (formPointer->DispObjPointer == null)
	{
			 formPointer->DispObjPointer = objPointer;
			 return;
	}
	pointer = formPointer->DispObjPointer;
	while (pointer->NextObjPointer != null)
			pointer = pointer->NextObjPointer;

	pointer->NextObjPointer = objPointer;	
	
}

static void ParseForm(FormTypeDef * formPointer)
{
	byte num=0,flag=0;
	ushort val;
	uint lval;
	signed char snum;
	DispObjType *objPointer= null;
	
	if(GuiTimer100ms)
	{
		GuiTimer100ms=0;
		GifLeafs100ms++;
		FigFlash++;
	
		if(FigFlash>(FLASHSPEED_100MS*2))
			FigFlash = 0;
	}
    
	objPointer = formPointer->DispObjPointer;
	while(objPointer != null)
	{
		if(objPointer->UpdataFlag!=TRUE)
		{
			objPointer = objPointer->NextObjPointer;
			continue;
		}
		objPointer->UpdataFlag =FALSE;
		switch(objPointer->ObjNum)
		{
			case FigObj_CircleMode:
				num = (*(byte*)objPointer->DataPointer);		
				switch(num)
				{
					case CIRCLEMODE_AUTO:
						System.Device.Lcd.DispUnit(FigInSys[LCD_T3]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T2]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T1]);	
						break;
					case CIRCLEMODE_OUT:
						System.Device.Lcd.DispUnit(FigInSys[LCD_T1]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T2]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T3]);
						break;
					case CIRCLEMODE_IN:
						System.Device.Lcd.DispUnit(FigInSys[LCD_T2]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T1]);
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T3]);	
						break;
					default:
						break;
				}				
				break;
			case FigObj_FlowVol:
				num = (*(byte*)objPointer->DataPointer);		
				val = num* DISPFLOW_ONESTEP;
				val += DISPFLOW_MIN;
				
				DigitToDisp(10,LCD_1D);
				num = val/100;
				val %= 100;
				if(!num)
					num=10;		
				DigitToDisp(num,LCD_2D);
				num = val/10;
				DigitToDisp(num,LCD_3D);
				num= val%10;
				DigitToDisp(num,LCD_4D); 
				
				System.Device.Lcd.DispUnit(FigInSys[LCD_T11]);

				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T9]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T10]);
				break;

			case FigObj_Timer:
				num = (*(byte*)objPointer->DataPointer);		
				if(!num)
				{
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T16]);
					DigitToDisp(num,LCD_4D);
				}
				else
				{
					System.Device.Lcd.DispUnit(FigInSys[LCD_T16]);
					DigitToDisp(0xFF,LCD_1D);
					DigitToDisp(0xFF,LCD_2D);
					DigitToDisp(0xFF,LCD_3D);
					DigitToDisp(num,LCD_4D);
				}
					
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T9]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T10]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T11]);
				break;
			case FigObj_AuxiHeater:
				num = (*(byte*)objPointer->DataPointer);		
				if(num == (byte)HEATER_HALF)
				{
					if(FigFlash==(FLASHSPEED_100MS))
					{
						System.Device.Lcd.NonDispUnit(FigInSys[LCD_T4]);
					}
					else if(!FigFlash)
						System.Device.Lcd.DispUnit(FigInSys[LCD_T4]);
					objPointer->UpdataFlag = TRUE;
				}					
				else if(num==HEATER_FULL)
					System.Device.Lcd.DispUnit(FigInSys[LCD_T4]);
				else
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T4]);				
				
			break;
			case FigObj_Lock:
				num = (*(byte*)objPointer->DataPointer);		
				if(num==(byte)CHILD_UNLOCK)
				{
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T22]);		
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T23]);		
				}					
				else
				{
					System.Device.Lcd.DispUnit(FigInSys[LCD_T22]);
					System.Device.Lcd.DispUnit(FigInSys[LCD_T23]);
				}
				break;
			case FigObj_Fault:
				lval= (*(uint*)objPointer->DataPointer);
			  if(lval&0xFFF0)
					System.Device.Lcd.DispUnit(FigInSys[LCD_T17]);

				break;
			case FigObj_Filter:
				lval= (*(uint*)objPointer->DataPointer);			
			if(lval&FIRSTFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T18]);
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T18]);				
			if(lval&MIDFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T19]);
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T19]);
			if(lval&ESPFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T20]);
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T20]);
			if(lval&HPFILTER_CHECK)
				System.Device.Lcd.DispUnit(FigInSys[LCD_T21]);
			else
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T21]);
			
				break;
			case FigObj_WIFI:
				lval= (*(uint*)objPointer->DataPointer);
				if(lval & (WIFI_STATE_STA))
				{
					System.Device.Lcd.DispUnit(FigInSys[LCD_T12]);	
					System.Device.Lcd.DispUnit(FigInSys[LCD_T13]);	
				}
				else if(lval & WIFI_STATE_CONFIG)
				{
					System.Device.Lcd.DispUnit(FigInSys[LCD_T13]);	
						if(FigFlash==FLASHSPEED_100MS)
						{
							System.Device.Lcd.NonDispUnit(FigInSys[LCD_T12]);
						}
						else if(!FigFlash)
							System.Device.Lcd.DispUnit(FigInSys[LCD_T12]);	
						objPointer->UpdataFlag =TRUE;
				}
				else
				{
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T12]);	
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T13]);	
				}
				break;
			case FigObj_RF:
				lval= (*(uint*)objPointer->DataPointer);
				if(lval & RF_STATE_CONFIG)
				{
						if(FigFlash==FLASHSPEED_100MS)
						{
							System.Device.Lcd.NonDispUnit(FigInSys[LCD_T14]);
							System.Device.Lcd.NonDispUnit(FigInSys[LCD_T15]);
						}
						else if(!FigFlash)
						{
							System.Device.Lcd.DispUnit(FigInSys[LCD_T14]);	
							System.Device.Lcd.DispUnit(FigInSys[LCD_T15]);
						}							
						objPointer->UpdataFlag =TRUE;
				}
				else if(lval & RF_STATE_COMM)
				{
						if(FigFlash==FLASHSPEED_100MS)
						{
							System.Device.Lcd.NonDispUnit(FigInSys[LCD_T14]);
						}
						else if(!FigFlash)
							System.Device.Lcd.DispUnit(FigInSys[LCD_T14]);	
						objPointer->UpdataFlag =TRUE;
				}
				else if(lval & (RF_STATE_CONNECT))
				{
					System.Device.Lcd.DispUnit(FigInSys[LCD_T14]);	
					System.Device.Lcd.DispUnit(FigInSys[LCD_T15]);	
				}
				else
				{
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T14]);	
					System.Device.Lcd.NonDispUnit(FigInSys[LCD_T15]);	
				}
				
				break;
			case FigObj_CO2:
				val = (*(ushort*)objPointer->DataPointer);	
				num = val/1000;
				val %= 1000;
				if(!num)
					num=10;
				else
					flag =1;			
				DigitToDisp(num,LCD_1D);
				
				num = val/100;
				val %= 100;
				if((!num)&&(!flag))
					num=10;		
				DigitToDisp(num,LCD_2D);
				num = val/10;
				val %= 10;
				DigitToDisp(num,LCD_3D);
				DigitToDisp(val,LCD_4D); 
				
				System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.DispUnit(FigInSys[LCD_T9]);					
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T10]);					
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T11]);					
				break;
			case FigObj_PM25:
				val = (*(ushort*)objPointer->DataPointer);	
				DigitToDisp(10,LCD_1D);
				num = val/100;
				val %= 100;
				if(!num)
					num=10;
				else
					flag =1;			
				DigitToDisp(num,LCD_2D);
				num = val/10;
				val %= 10;
				if((!num)&&(!flag))
					num=10;		
				DigitToDisp(num,LCD_3D);
				DigitToDisp(val,LCD_4D); 
				
				System.Device.Lcd.DispUnit(FigInSys[LCD_T8]);
				System.Device.Lcd.DispUnit(FigInSys[LCD_T10]);
				
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T9]);					
				System.Device.Lcd.NonDispUnit(FigInSys[LCD_T11]);					
				break;
			case FigObj_TempIn:
				snum = (*(signed char*)objPointer->DataPointer);		
				System.Device.Lcd.DispUnit(FigInSys[LCD_T5]);					
				if(snum<0)
				{
					DigitToDisp(0xFF,LCD_5D);
					snum *= -1;
					if(snum>9)
						snum =9;
					DigitToDisp(0xFF,LCD_6D);
				}
				else
				{
					num = snum/10;
					DigitToDisp(num,LCD_5D);
					num = snum%10;
					DigitToDisp(num,LCD_6D);
				}
				
				break;
			case FigObj_RHIn:
				num = (*(byte*)objPointer->DataPointer);		
				System.Device.Lcd.DispUnit(FigInSys[LCD_T6]);
				val = num/10;
				DigitToDisp(val,LCD_7D);
				num %= 10;
				DigitToDisp(num,LCD_8D);
				break;
			case FigObj_FaultCode:			
				lval= (*(uint*)objPointer->DataPointer);
			  if(lval)
					System.Device.Lcd.DispUnit(FigInSys[LCD_T17]);
				
				DigitToDisp(11,LCD_2D);			  
			  lval >>=4;
			  lval >>= FaultFlag;
			  while(lval)
				{
					if(lval&0x01)
					{
            FaultFlag++;	
						break;
					}
					else
					{
						FaultFlag++;
						lval >>=1;
					}
				}
				num = FaultFlag/10;
				DigitToDisp(num,LCD_3D);
				num = FaultFlag%10;
				DigitToDisp(num,LCD_4D);

				break;
			case FigObj_CtrlVersionCode:
				val = (*(ushort*)objPointer->DataPointer);	
				DigitToDisp(10,LCD_1D);
				num = val>>8;
				num %= 10;
				DigitToDisp(num,LCD_2D);
				val &= 0xFF;
				num = val/10;
				DigitToDisp(num,LCD_3D);
				num = val%10;
				DigitToDisp(num,LCD_4D); 
			  System.Device.Lcd.DispUnit(FigInSys[LCD_T7]);
			  System.Device.Lcd.NonDispUnit(FigInSys[LCD_T8]);
				
				break;
			case FigObj_PowerVersionCode:
				val = (*(ushort*)objPointer->DataPointer);	
				DigitToDisp(10,LCD_1D);
				num = val>>8;
				num %= 10;
				DigitToDisp(num,LCD_2D);
				val &= 0xFF;
				num = val/10;
				DigitToDisp(num,LCD_3D);
				num = val%10;
				DigitToDisp(num,LCD_4D); 
			  System.Device.Lcd.DispUnit(FigInSys[LCD_T8]);
			  System.Device.Lcd.NonDispUnit(FigInSys[LCD_T7]);
				
				break;
			default:
				break;
			}
		objPointer = objPointer->NextObjPointer;
	}
	
}

static void LoadForm(FormTypeDef * formPointer)
{
	DispObjType *objPointer= null;
	
	objPointer = formPointer->DispObjPointer;
	while(objPointer != null)
	{
		objPointer->UpdataFlag = TRUE;
		objPointer = objPointer->NextObjPointer;
	}

	if(formPointer->ScreenClean)
		System.Device.Lcd.ScreenClear();
	
	formPointer->LoadFresh=0;
		
}
	
void InitGui(void)
{
	System.Gui.ParseForm = ParseForm;
  System.Gui.AddDispObj = AddDispObj;    
  System.Gui.LoadForm = LoadForm;    

}

