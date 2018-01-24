//#include "drive.h"
#include "system.h"

/* EEPROM Addresses defines */
#define EEPROM_ADDRESS 0xA0  

#define I2C_Speed              100000
//#define I2C_PageSize           16			/* AT24C02???8??? */
#define I2C_PageSize           8			/* AT24C02???8??? */
//#define I2C1_SLAVE_ADDRESS7    0xA0


#define SCL_H        GPIOB->BSRR = GPIO_Pin_11  //PbOut(6)=1 
#define SCL_L        GPIOB->BRR  = GPIO_Pin_11  //PbOut(6)=0   
    
#define SDA_H        GPIOB->BSRR = GPIO_Pin_10  //PbOut(7)=1  
#define SDA_L        GPIOB->BRR  = GPIO_Pin_10 //PbOut(7)=0   

#define SCL_read     (GPIOB->IDR  & GPIO_Pin_11)  //PbIn(6)  
#define SDA_read     (GPIOB->IDR  & GPIO_Pin_10)  //PbIn(7)  


/* Private macro -------------------------------------------------------------*/ 
/* Private variables ---------------------------------------------------------*/ 
__IO uint8_t FRAM_ADDRESS=EEPROM_ADDRESS;

/* Private function prototypes -----------------------------------------------*/ 

/**/ 





static void I2C_delay(void) 
{ 
   uint8_t i=150; 
   while(i)  
   {  
     i--;  
   }  
} 

static TestStatus I2C_Start(void) 
{
	SDA_H; 
	SCL_H; 
	I2C_delay(); 
	if(!SDA_read)
		return FAILED; //SDA?????????,?? 
	SDA_L; 
	I2C_delay(); 
	if(SDA_read) 
		return FAILED; //SDA??????????,?? 
	SDA_L; 
	I2C_delay(); 
	return PASSED; 
} 

static void I2C_Stop(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
} 

static void I2C_Ack(void) 
{
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

static void I2C_NoAck(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

static TestStatus I2C_WaitAck(void)   //???:=1?ACK,=0?ACK 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	if(SDA_read) 
	{	
		SCL_L; 
		return FAILED; 
	}
	SCL_L; 
	return PASSED; 
} 

static void I2C_SendByte(uint8_t SendByte) //????????// 
{
	uint8_t i=8; 

	while(i--) 
	{ 
		SCL_L; 
		I2C_delay();
		
		if(SendByte&0x80) 
			SDA_H;
		else
			SDA_L;
		
		SendByte<<=1; 
		I2C_delay(); 
		SCL_H; 
		I2C_delay(); 
	}
	SCL_L; 
} 

static uint8_t I2C_ReceiveByte(void)  //????????// 
{
	uint8_t i=8; 
	uint8_t ReceiveByte=0; 
	
	SDA_H; 
	while(i--) 
	{
		ReceiveByte<<=1;
    SCL_L;
		I2C_delay(); 
		SCL_H;
		I2C_delay(); 
		if(SDA_read) 
			ReceiveByte|=0x01; 
	}
	SCL_L;
	return ReceiveByte; 
} 

static TestStatus I2C_EE_WaitEepromStandbyState(void)
{
	uint timeout=0;
	
	timeout = 200;
	while(timeout)
	{
		I2C_Start();
		I2C_SendByte(FRAM_ADDRESS);
		if (I2C_WaitAck())
			break;	
    timeout--;		
	}
	if(timeout)
		return PASSED;
	else
		return FAILED;

}
	
static TestStatus I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr)
{
	uint8_t addr;
	
	if (!I2C_Start()) 
		return FAILED; 

//	addr = WriteAddr>>8;
//	addr &= 0x03;
//	addr <<= 1;
//	addr |= FRAM_ADDRESS;
	addr = FRAM_ADDRESS;
	I2C_SendByte(addr);//??????+??? 
	if (!I2C_WaitAck()) 
	{
		I2C_Stop(); 
		return FAILED; 
	}
	
	addr = (uint8_t)(WriteAddr&0xff);

	I2C_SendByte(addr);   
	I2C_WaitAck(); 
	I2C_SendByte(*pBuffer); 
  I2C_WaitAck(); 
	I2C_Stop(); 
	return PASSED; 
} 

static TestStatus I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
	uint8_t addr;
	
	if (!I2C_Start()) 
		return FAILED; 

//	addr = WriteAddr>>8;
//	addr &= 0x03;
//	addr <<= 1;
//	addr |= FRAM_ADDRESS;
	addr = FRAM_ADDRESS;
	I2C_SendByte(addr);//??????+??? 
	if (!I2C_WaitAck()) 
	{
		I2C_Stop(); 
		return FAILED; 
	}
	
	addr = (uint8_t)(WriteAddr&0xff);
	I2C_SendByte(addr);   
	if (!I2C_WaitAck()) 
	{
		I2C_Stop(); 
		return FAILED; 
	}
//	I2C_WaitAck(); 

	while(NumByteToWrite--) 
	{
		I2C_SendByte(*pBuffer);
		I2C_WaitAck();
		pBuffer++;
	}
	
	I2C_Stop(); 
	return PASSED; 
	
}

/*********
????1??,?????256???
*******************/
static TestStatus I2C_EE_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0,  count = 0;
	uint16_t addr=0;
	TestStatus flag=FAILED;

  addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - addr;
	if(addr)
	{
		if(count<NumByteToWrite)
		{
			NumOfPage =  (NumByteToWrite-count) / I2C_PageSize;
			NumOfSingle = (NumByteToWrite-count) % I2C_PageSize;
		}
		else
		{
			NumOfPage =  0;
			NumOfSingle = NumByteToWrite;
		}
	}
	else
	{
		NumOfPage =  NumByteToWrite / I2C_PageSize;
		NumOfSingle = NumByteToWrite % I2C_PageSize;
	}
		
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      flag = I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      if(flag == PASSED)
				flag = I2C_EE_WaitEepromStandbyState();
      if(flag == FAILED)
				return flag;
			
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        flag = I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
				if(flag == PASSED)
					flag = I2C_EE_WaitEepromStandbyState();
				if(flag == FAILED)
					return flag;
				
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        flag = I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
				if(flag == PASSED)
					flag = I2C_EE_WaitEepromStandbyState();
				if(flag == FAILED)
					return flag;
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
		if(count<NumByteToWrite)
		{
			flag = I2C_EE_PageWrite(pBuffer, WriteAddr, count);
      if(flag == PASSED)
				flag = I2C_EE_WaitEepromStandbyState();
      if(flag == FAILED)
				return flag;
			
			pBuffer += count;
			WriteAddr += count;
			NumByteToWrite -= count;
		}
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      flag = I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      if(flag == PASSED)
				flag = I2C_EE_WaitEepromStandbyState();
      if(flag == FAILED)
				return flag;
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
//      NumByteToWrite -= count;
//      NumOfPage =  NumByteToWrite / I2C_PageSize;
//      NumOfSingle = NumByteToWrite % I2C_PageSize;	
//      
//      if(count != 0)
//      {  
//        flag = I2C_EE_PageWrite(pBuffer, WriteAddr, count);
//				if(flag == PASSED)
//					flag = I2C_EE_WaitEepromStandbyState();
//				if(flag == FAILED)
//					return flag;
//				
//        WriteAddr += count;
//        pBuffer += count;
//      } 
      
      while(NumOfPage--)
      {
        flag = I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
				if(flag == PASSED)
					flag = I2C_EE_WaitEepromStandbyState();
				if(flag == FAILED)
					return flag;
				
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        flag = I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
				if(flag == PASSED)
					flag = I2C_EE_WaitEepromStandbyState();
				if(flag == FAILED)
					return flag;
      }
    }
  }
	return flag;
}


static TestStatus I2C_EE_BufferRead(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	uint8_t addr;
	
	if (!I2C_Start()) 
		return FAILED;
	
//	addr = ReadAddr>>8;
//	addr &= 0x03;
//	addr <<= 1;
//	addr |=FRAM_ADDRESS;
	addr =FRAM_ADDRESS;
	I2C_SendByte(addr);
	if (!I2C_WaitAck()) 
	{
		I2C_Stop();
		return FAILED; 
	}
	addr = (uint8_t)(ReadAddr&0xff);
	I2C_SendByte(addr);   
	I2C_WaitAck(); 
  I2C_Start(); 
//	addr = ReadAddr>>8;
//	addr &= 0x03;
//	addr <<= 1;
//	addr |=FRAM_ADDRESS;
	addr =FRAM_ADDRESS;
	I2C_SendByte(addr | 0x01);
	I2C_WaitAck(); 
	while(NumByteToRead) 
	{
		*pBuffer = I2C_ReceiveByte(); 
		if(NumByteToRead == 1)
			I2C_NoAck();
		else I2C_Ack(); 
		pBuffer++; 
    NumByteToRead--; 
	}
	I2C_Stop();
	return PASSED; 
} 

void Init_E2prom(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);

  /*Configure GPIO pin : PB6-7 LED*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	System.Device.Eeprom.I2C_EE_BufferRead = I2C_EE_BufferRead;
	System.Device.Eeprom.I2C_EE_BufferWrite = I2C_EE_BufferWrite;
}


