/*
********************************************************************************
*文件名     : device_key.c
*作用       : 按键设备
*原理       : 基于SystemTick 100/秒 10mS扫描获取按键值，按键值分为长按与短按两种
********************************************************************************
*版本     作者            日期            说明
*V0.1    TGQ        2013/07/21       初始版本
********************************************************************************
*/



#include "system.h"

#define RHT_SCL_H        GPIOF->BSRR = GPIO_Pin_5  //PbOut(6)=1 
#define RHT_SCL_L        GPIOF->BRR  = GPIO_Pin_5  //PbOut(6)=0   
    
#define RHT_SDA_H        GPIOF->BSRR = GPIO_Pin_4  //PbOut(7)=1  
#define RHT_SDA_L        GPIOF->BRR  = GPIO_Pin_4 //PbOut(7)=0   

#define RHT_SCL_read     (GPIOF->IDR  & GPIO_Pin_5)  //PbIn(6)  
#define RHT_SDA_read     (GPIOF->IDR  & GPIO_Pin_4)  //PbIn(7)  

#define HTU21ADDR 0x80
#define ACK		0
#define NACK	1 

static void RHT_I2CDelay (uint8_t t)
{
	while(t--);
}

/*================================================================
?? ??void I2CInit(void)
?? ??I2C???,????
?? ??
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static void RHT_I2CInit(void)
{
	RHT_SDA_H; 
	RHT_SCL_H;
}


/*================================================================
?? ??void I2CStart(void)
?? ??I2C????
?? ??SCL?SDA???,SDA??????,SCL????
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static void RHT_I2CStart(void)
{
  	RHT_SDA_H;
  	RHT_SCL_H;
  	RHT_I2CDelay(10);
  	RHT_SDA_L;
  	RHT_I2CDelay(20);
  	RHT_SCL_L;
  	RHT_I2CDelay(20);
}

/*================================================================
?? ??void I2CStop(void)
?? ??I2C????
?? ??SCL?SDA???,SCL??????,SDA????
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static void RHT_I2CStop(void)
{
  	RHT_SDA_L;
  	RHT_SCL_L;
		RHT_I2CDelay(10);
  	RHT_SCL_H;
  	RHT_I2CDelay(10);
  	RHT_SDA_H;
  	RHT_I2CDelay(10);
}



/*================================================================
?? ??unsigned char I2CWRByte(unsigned char WRByte)
?? ??I2C???????,??ACK??NACK
?? ??????,????
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static uint8_t RHT_I2CWRByte(uint8_t WRByte)
{
	uint8_t i;
	
	RHT_SCL_L;
	for(i=0;i<8;i++)
	{
		if(WRByte&0x80)
		{
			RHT_SDA_H;	
		}
		else
		{
			RHT_SDA_L;
		}
		RHT_I2CDelay(10);
		RHT_SCL_H;			//??SDA???,??SCL?????,????????????
		RHT_I2CDelay(50);
		RHT_SCL_L;
		RHT_I2CDelay(10);
		WRByte <<= 1;
	} 
	RHT_SDA_H;
	RHT_SCL_H;
	RHT_I2CDelay(20);
	if(RHT_SDA_read)			//SDA??,??NACK
	{
		RHT_SCL_L;
		RHT_I2CDelay(50);
		return NACK;	
	}
	else 				//SDA??,??NACK
	{
		RHT_SCL_L;
		RHT_I2CDelay(50);
		return ACK;
	}
}


/*================================================================
?? ??unsigned char I2CRDByte(unsigned char AckValue)
?? ??I2C???????,????????????,ACK??NACK
?? ??????,????
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static uint8_t RHT_I2CRDByte(uint8_t AckValue)
{
	uint8_t i,RDByte=0;
	
	RHT_SCL_L;
	RHT_SDA_H;			//????	
	for (i=0;i<8;i++) 
	{
		RDByte <<= 1;	//??
		RHT_SCL_H;		//?????
		RHT_I2CDelay(30);	//????????
		if(RHT_SDA_read) 		//??????
		{
			RDByte |= 0x01;
		}
		else
		{
			RDByte &= 0xfe;
		}
		RHT_SCL_L;   		//???,????????
		RHT_I2CDelay(10);
	}
//	SDA =  AckValue;	//????
  if(AckValue)
		RHT_SDA_H;
	else
		RHT_SDA_L;
	RHT_I2CDelay(10);
	RHT_SCL_H;                         
  	RHT_I2CDelay(50);          
  	RHT_SCL_L;
  	RHT_SDA_H;                
  	RHT_I2CDelay(1);
	return RDByte;
}

/*================================================================
?? ??void HTU21Init(void)
?? ??HTU21?????,??????
?? ??????,????
?? ??
?? ??2012?8?18?11:24:09
static void HTU21Init(void)
{
	RHT_I2CInit();//I2C???
	RHT_I2CStart();//??I2C
	RHT_I2CWRByte(HTU21ADDR&0xfe);//?HTU21I2C??
	RHT_I2CWRByte(0xfe);
	RHT_I2CStop();//?I2C
	
	RHT_I2CDelay(200);//???
	RHT_I2CDelay(200);//???
	RHT_I2CDelay(200);//???
}
================================================================*/


/*================================================================
?? ??long ReadHtu21(char whatdo)
?? ????????
?? ??
?? ??
?? ??2012?8?18?11:24:09
================================================================*/
static int32_t ReadRhTSensor(RHTReadTypedef whatdo)
{
	uint32_t temp;
	uint8_t Humidity,HumidityH,HumidityL;
	int32_t wendu;

	RHT_I2CStart();
	if(RHT_I2CWRByte(HTU21ADDR&0xfe)==ACK)
	{
		if(RHT_I2CWRByte(whatdo)==ACK)
		{
			do
			{
				RHT_I2CDelay(120);
				RHT_I2CStart();	
			}while(RHT_I2CWRByte(HTU21ADDR|0x01)==NACK);

			HumidityH = RHT_I2CRDByte(ACK);
			HumidityL = RHT_I2CRDByte(ACK);
			RHT_I2CRDByte(NACK);
			RHT_I2CStop();
			HumidityL &= 0xfc;
			temp = HumidityH*256+HumidityL;

			if (whatdo==HIMIDITY_READ)
			{
//				Humidity = (uint8_t)((temp*125)/65536-6)%0xff;
				temp  *= 125;
				temp /= 65536;
				temp -=6;
				Humidity = (uint8_t)temp;
				return Humidity;
			}
			else
			{
				temp=((int32_t)17572)*temp;
				temp=temp/((int32_t)65536);
				wendu =temp-((int32_t)4685);
				wendu /= 100;
				return wendu;		//??
			}
		}
	}
	return 0;
}

void Init_rht(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF , ENABLE);

  /*Configure GPIO pin : PF4-SCL,PF5-SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	RHT_I2CInit();//I2C???
	RHT_I2CStart();//??I2C
	RHT_I2CWRByte(HTU21ADDR&0xfe);//?HTU21I2C??
	RHT_I2CWRByte(0xfe);
	RHT_I2CStop();//?I2C
	
	RHT_I2CDelay(200);//???
	RHT_I2CDelay(200);//???
	RHT_I2CDelay(200);//???

	System.Device.RhT.ReadRhTSensor = ReadRhTSensor;
}



