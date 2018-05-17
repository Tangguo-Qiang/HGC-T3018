#include "system.h"

#define SYSDIS	0x00
#define SYSEN		0x02
#define LCDOFF	0x04
#define LCDON		0x06
#define TONEON	0x12
#define TONEOFF	0x10
#define XTAL		0x28
#define RC			0x30
#define BIAS		0x52  //1/3bias,1/4duty
#define WDTDIS	0x0A
#define WDTED		0x0E

#define DATA_H      GPIOC->BSRR = GPIO_Pin_12  //PbOut(6)=1 
#define DATA_L      GPIOC->BRR  = GPIO_Pin_12  //PbOut(6)=0   
    
#define WR_H        GPIOC->BSRR = GPIO_Pin_11  //PbOut(7)=1  
#define WR_L        GPIOC->BRR  = GPIO_Pin_11 //PbOut(7)=0   
    
#define CS_H        GPIOC->BSRR = GPIO_Pin_10  //PbOut(7)=1  
#define CS_L        GPIOC->BRR  = GPIO_Pin_10 //PbOut(7)=0   


static byte ComData[22]={0};

static void LcdDelay(uint i) 
{ 
   while(i)  
   {  
     i--;  
   }  
} 

static void WriteToLcdMSF(byte Data,byte Cnt)
{
	byte i;
	for(i=0;i<Cnt;i++)
	{
		WR_L;
		if(Data&0x80)
			DATA_H;
		else
			DATA_L;
		
		LcdDelay(200);
		WR_H;
		Data <<=1;
		LcdDelay(200);
	}
	WR_L;
	DATA_L;
}

static void WriteToLcdLSF(byte Data,byte Cnt)
{
	byte i;
	for(i=0;i<Cnt;i++)
	{
		WR_L;
		if(Data&0x01)
			DATA_H;
		else
			DATA_L;
		
		LcdDelay(200);
		WR_H;
		Data >>=1;
		LcdDelay(200);
	}
	WR_L;
	DATA_L;
}

static void WriteCmd(byte Command)
{
	CS_L;
	LcdDelay(200);
	WriteToLcdMSF(0x80,4);
	WriteToLcdMSF(Command,8);
	
	CS_H;
	LcdDelay(200);
}

static void WriteByte(byte Seg,byte Com)
{
	CS_L;
	LcdDelay(200);
	WriteToLcdMSF(0xA0,3);
	Seg <<=2;
	WriteToLcdMSF(Seg,6);
	WriteToLcdLSF(Com,4);
	CS_H;
	LcdDelay(200);
}

static void ScreenClear(void)
{
	byte i;
	
	CS_L;
	LcdDelay(200);
	WriteToLcdMSF(0xA0,3);
	WriteToLcdMSF(0,6);
	for(i=0;i<22;i++)
	{
		WriteToLcdLSF(0x0,4);
		ComData[i]=0;
	}
	CS_H;
	LcdDelay(200);
}

static void DispUnit(LCD_DispTypeDef Icon)
{
	ComData[Icon.Addr] |= Icon.ComBit;
	WriteByte(Icon.Addr,ComData[Icon.Addr]);
}

static void NonDispUnit(LCD_DispTypeDef Icon)
{
	ComData[Icon.Addr] &= ~Icon.ComBit;
	WriteByte(Icon.Addr,ComData[Icon.Addr]);
}

void Init_Lcd(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC , ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);

  /*Configure GPIO pin : PC10-12 LED*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /*Configure GPIO pin : PC10-12 LED*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	LCD_BL_OFF;
	CS_H;
	WR_H;
	DATA_H;
	
	LcdDelay(200);
	WriteCmd(BIAS);
	WriteCmd(RC);
	WriteCmd(SYSDIS);
	WriteCmd(WDTDIS);
	WriteCmd(SYSEN);
	WriteCmd(LCDON);
	LcdDelay(48000);
	ScreenClear();
	LCD_BL_OFF;
	System.Device.Lcd.ScreenClear = ScreenClear;
	System.Device.Lcd.DispUnit = DispUnit;
	System.Device.Lcd.NonDispUnit = NonDispUnit;
}
