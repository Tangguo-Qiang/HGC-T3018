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

#define LCD_SDA_H      GPIOC->BSRR = GPIO_Pin_12  //PbOut(6)=1 
#define LCD_SDA_L      GPIOC->BRR  = GPIO_Pin_12  //PbOut(6)=0   
#define LCD_SDA_RD     (GPIOC->IDR  & GPIO_Pin_12)  //PbIn(7)  
    
#define LCD_SCL_H        GPIOC->BSRR = GPIO_Pin_11  //PbOut(7)=1  
#define LCD_SCL_L        GPIOC->BRR  = GPIO_Pin_11 //PbOut(7)=0   
    
//#define CS_H        GPIOC->BSRR = GPIO_Pin_10  //PbOut(7)=1  
//#define CS_L        GPIOC->BRR  = GPIO_Pin_10 //PbOut(7)=0   

#define DELAY_TIMER	50

static byte ComData[20]={0};

static void LcdDelay(uint i) 
{ 
   while(i)  
   {  
     i--;  
   }  
} 

static TestStatus LCD_StartI2C(void) 
{
	LCD_SDA_H; 
	LCD_SCL_H; 
	LcdDelay(DELAY_TIMER); 
	if(!LCD_SDA_RD)
		return FAILED; //SDA?????????,?? 
	LCD_SDA_L; 
	LcdDelay(DELAY_TIMER); 
	if(LCD_SDA_RD) 
		return FAILED; //SDA??????????,?? 
	LCD_SCL_L; 
	LcdDelay(DELAY_TIMER); 
	return PASSED; 
} 

static void LCD_StopI2C(void) 
{ 
	LCD_SCL_L; 
	LcdDelay(DELAY_TIMER); 
	LCD_SDA_L; 
	LcdDelay(DELAY_TIMER); 
	LCD_SCL_H; 
	LcdDelay(DELAY_TIMER); 
	LCD_SDA_H; 
	LcdDelay(DELAY_TIMER); 
} 

static TestStatus LCD_WaitAckI2C(void)   //???:=1?ACK,=0?ACK 
{ 
	LCD_SCL_L; 
	LcdDelay(DELAY_TIMER); 
	LCD_SDA_H; 
	LcdDelay(DELAY_TIMER); 
	LCD_SCL_H; 
	LcdDelay(DELAY_TIMER); 
	if(LCD_SDA_RD) 
	{	
		LCD_SCL_L; 
		return FAILED; 
	}
	LCD_SCL_L; 
	return PASSED; 
} 

static void LCD_SendByteI2C(uint8_t SendByte) //????????// 
{
	uint8_t i=8; 

	while(i--) 
	{ 
		LCD_SCL_L; 
	LcdDelay(DELAY_TIMER); 
		
		if(SendByte&0x01) 
			LCD_SDA_H;
		else
			LCD_SDA_L;
		
		SendByte >>=1; 
	LcdDelay(DELAY_TIMER); 
		LCD_SCL_H; 
	LcdDelay(DELAY_TIMER); 
	}
	LCD_SCL_L; 
} 



static TestStatus WriteCmdInit(void)	//(byte Command)
{
	if (!LCD_StartI2C()) 
		return FAILED; 

	LCD_SendByteI2C(0x27); 
	if (!LCD_WaitAckI2C()) 
	{
		LCD_StopI2C(); 
		return FAILED; 
	}

	LcdDelay(200);
	return PASSED; 
}

static TestStatus WriteByte(byte Addr,byte ComBit)
{	
	if (!LCD_StartI2C()) 
		return FAILED; 

	Addr |= 0xC0;
	LCD_SendByteI2C(Addr); 
	if (!LCD_WaitAckI2C()) 
	{
		LCD_StopI2C(); 
		return FAILED; 
	}
	
	LCD_SendByteI2C(ComBit); 
  LCD_WaitAckI2C(); 
	LCD_StopI2C(); 
	return PASSED; 
}

static TestStatus ScreenClear(void)
{
	byte i;
	
	LCD_SendByteI2C(0xC0); 
	if (!LCD_WaitAckI2C()) 
	{
		LCD_StopI2C(); 
		return FAILED; 
	}
	for(i=0;i<20;i++)
	{
		LCD_SendByteI2C(0x00); 
		LCD_WaitAckI2C(); 
	}
	LCD_StopI2C(); 
	return PASSED; 
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

  /*Configure GPIO pin : PC10-11 LCD_SCLK,SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /*Configure GPIO pin : PC9 BK_LIGHT*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /*Configure GPIO pin : PA8 BK_LIGHT*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LCD_BL_ON;
	LcdDelay(200);
	WriteCmdInit();
	LcdDelay(200);
	ScreenClear();
	LCD_BL_OFF;
	System.Device.Lcd.ScreenClear = ScreenClear;
	System.Device.Lcd.DispUnit = DispUnit;
	System.Device.Lcd.NonDispUnit = NonDispUnit;
}
