/*
********************************************************************************
*文件名     : device_wifi.c
*作用       : 串口设备
*原理       : 发送采用Buffer发送，接收采用App注册函数方式接收
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
*V0.2    Wangsw        2013/09/13       第一次更改
********************************************************************************
*/

#include "system.h"

#define USART1_CLK                       RCC_APB2Periph_USART1

//通用帧头格式 
typedef struct
{
	unsigned char header;
	unsigned char length;
	unsigned char type;
	unsigned char number;
}GeneralHeader;


//Hekr各帧长度
typedef	enum
{
	ModuleQueryFrameLength = 0x07,
	ModuleProdKeyFrameLength = 0x16,
	ModuleResponseFrameLength = 0x0B,
	ErrorFrameLength = 0x07
}AllFrameLength;

//Hekr错误码取值
typedef	enum
{
	ErrorOperation = 0x01,
	ErrorSumCheck = 0x02,
	ErrorDataRange = 0x03,
	ErrorNoCMD = 0xFF
}AllErrorValue;

//Hekr各帧类型
typedef	enum
{
	DeviceUploadType = 0x01,
	ModuleDownloadType = 0x02,
	ModuleOperationType = 0xFE,
	ErrorFrameType = 0xFF
}AllFrameType;

//模块查询帧格式
typedef struct
{
	//通用帧头
	GeneralHeader header;
	//有效数据
	unsigned char CMD;
	unsigned char Reserved;
	//和校验
	unsigned char SUM;
}ModuleQueryFrame; 


//错误帧格式
typedef struct
{
	//通用帧头
	GeneralHeader header;
	//有效数据
	unsigned char ErrorCode;
	unsigned char Reserved;
	//和校验
	unsigned char SUM;
}ErrorFrame; 

#define HEKR_DATA_LEN 0x05u
#define HEKR_FRAME_HEADER 0x48u

#define WIFIBUFFER_LEN		(WIFIUSER_MAX_LEN+HEKR_DATA_LEN+1)

#define TxdWIFIBufferSum (WIFIBUFFER_LEN)
#define RxdWIFIBufferSum (WIFIBUFFER_LEN)

/*Single Direction Machine*/
const byte ProdKey[16]={0xa6,0xb0,0x7b,0x2d,0xe4,0x1b,0x8a,0x8e,0xe4,0x11,0x42,0xb1,0x9e,0x67,0x8e,0x5f};

static byte TxdWifiBuffer[TxdWIFIBufferSum];
static byte RxdWifiBuffer[TxdWIFIBufferSum];
static byte* pTxValidDataBuffer;
static byte* pRxValidDataBuffer;
static byte* pModuleStatus;
static byte RecvTimer1ms = 0;
static byte TransTimer1ms = 0;
static byte Frame_No = 0;
static byte RxdDataLength = 0;
static byte RxdDataIndex = 0;

static unsigned char SumCalculate(unsigned char* data)
{
	unsigned char temp;
	unsigned char i;
	unsigned char len = data[1] - 1;
	temp = 0;
	for(i = 0;i < len; i++)
	{
			temp += data[i];
	}
	return temp;
}

static void HekrValidDataCopy(void)
{
	unsigned char len,i;
	len = RxdWifiBuffer[1]- HEKR_DATA_LEN;
	for(i = 0 ;i < len ; i++)
		*(pRxValidDataBuffer+i) = RxdWifiBuffer[i+4];
}

static unsigned char SumCheckIsErr(unsigned char* data)
{
	unsigned char temp = SumCalculate(data);
	unsigned char len = data[1] - 1;
	if(temp == data[len])
		return 0;
	return 1;
}

static void StartTransData(void)
{
	byte i,len =TxdWifiBuffer[1];
	TxdWifiBuffer[len-1] = SumCalculate(TxdWifiBuffer);
	TransTimer1ms=5;
	for(i=0;i<len;i++)
	{
		while((USART1->ISR&0x40)==0)
		{
			if(!TransTimer1ms)
				break;
		}
    USART1->TDR = (ushort)TxdWifiBuffer[i];//*pTxd++;
		TransTimer1ms=5;

	}

	
	
}

static void HekrSendFrame(byte *data)
{
	byte len = data[1];
	byte i=0;
	for(i=0;i<len;i++)
		TxdWifiBuffer[i]=*data++;
	for(i=0;i<len;i++)
	{
		while((USART1->ISR&0x40)==0)
		{
			if(!TransTimer1ms)
				break;
		}
    USART1->TDR = (ushort)TxdWifiBuffer[i];//*pTxd++;
		TransTimer1ms=5;

	}

}

static void ErrResponse(unsigned char data)
{
		TxdWifiBuffer[0] = HEKR_FRAME_HEADER;
		TxdWifiBuffer[1] = ErrorFrameLength;
		TxdWifiBuffer[2] = ErrorFrameType;
		TxdWifiBuffer[3] = Frame_No++;
		TxdWifiBuffer[4] = data;
		TxdWifiBuffer[5] = 0x00;
		StartTransData();
}

static void HekrModuleStateCopy(void)
{
	unsigned char len,i;
	len = RxdWifiBuffer[1]- HEKR_DATA_LEN;
	for(i = 0 ;i < len ; i++)
		*(pModuleStatus+i) = RxdWifiBuffer[i+4];
}


unsigned char HekrRecvDataHandle(void)
{
	//检查和校验
	if(SumCheckIsErr(RxdWifiBuffer))
	{
		ErrResponse(ErrorSumCheck);
		return RecvDataSumCheckErr;
	}
	//确认帧类型
	switch(RxdWifiBuffer[2])
	{
	case DeviceUploadType://MCU上传信息反馈 不需要处理 
	                        return MCU_UploadACK;
	case ModuleDownloadType://WIFI下传信息
	                        HekrSendFrame(RxdWifiBuffer);
	                        HekrValidDataCopy();
	                        return ValidDataUpdate;
	case ModuleOperationType://Hekr模块状态
													if(RxdWifiBuffer[1] != ModuleResponseFrameLength)
														return MCU_ControlModuleACK;
	                        HekrModuleStateCopy();
	                        return HekrModuleStateUpdate;
	case ErrorFrameType://上一帧发送错误	
	                        return LastFrameSendErr;
	default:ErrResponse(ErrorNoCMD);break;
	}
	return RecvDataUseless;
}

static void HekrValidDataUpload(byte len)
{
	unsigned char i;
	TxdWifiBuffer[0] = HEKR_FRAME_HEADER;
	TxdWifiBuffer[1] = len + 5;;
	TxdWifiBuffer[2] = DeviceUploadType;
	TxdWifiBuffer[3] = Frame_No++;
	for(i = 0; i < len ; i++)
		TxdWifiBuffer[i+4] = *(pTxValidDataBuffer+i);
	StartTransData();
}

static void HekrModuleControl(unsigned char data)
{
	TxdWifiBuffer[0] = HEKR_FRAME_HEADER;
	TxdWifiBuffer[1] = ModuleQueryFrameLength;
	TxdWifiBuffer[2] = ModuleOperationType;
	TxdWifiBuffer[3] = Frame_No++;
	TxdWifiBuffer[4] = data;
	TxdWifiBuffer[5] = 0x00;
	StartTransData();
}


static TestStatus HekrModuleProdKey(void)
{
	byte i;
	uint cnt=0;
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 
	TxdWifiBuffer[0] = HEKR_FRAME_HEADER;
	TxdWifiBuffer[1] = ModuleProdKeyFrameLength;
	TxdWifiBuffer[2] = ModuleOperationType;
	TxdWifiBuffer[3] = Frame_No++;
	TxdWifiBuffer[4] = HekrProdKey;	
	for(i=0;i<16;i++)
	 TxdWifiBuffer[i+5] = ProdKey[i];
	
	for(i=0;i<0x17;i++)
	 RxdWifiBuffer[i] = 0;
	
	RxdDataIndex=0;
	StartTransData();	
	while((RxdDataIndex<0x17)&&(++cnt<480000))
	{
		if(USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET) 
		{
			RxdWifiBuffer[RxdDataIndex] = (byte)USART_ReceiveData(USART1);
			while(USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET);
			if(RxdDataIndex==0)
			{
				if(RxdWifiBuffer[0]==0x48)
					RxdDataIndex++;
			}
			else if(RxdWifiBuffer[0]==0x48)
				RxdDataIndex++;
		}
	}
	
	return Buffercmp((byte*)&RxdWifiBuffer[5],(byte*)&ProdKey[0],16);

}

/*******************************************************************************
* 描述	    : 系统节拍1000/S,即1mS一次调用此服务程序接收缓冲区数据
*******************************************************************************/
void Usart1RecvSystick1000Routine(void) 
{
  if(RecvTimer1ms)
	{
		RecvTimer1ms--;
		if(!RecvTimer1ms)
		{
			if((RxdDataIndex==RxdDataLength)&&RxdDataIndex)
			{
				PostMessage(MessageWifiRecv, HekrRecvDataHandle());
				RecvTimer1ms = 0;
			}
			RxdDataIndex =0;
			RxdDataLength =0;
		}
	}
		
}

/*******************************************************************************
* 描述	    : 系统节拍1000/S,即1mS一次调用此服务程序发送缓冲区数据
*******************************************************************************/
void Usart1TransSystick1000Routine(void) 
{
//	if (TransDataLength == 0) return;

//    if ((USART1->SR&0x40)==0) return;
//    USART1->DR = *pTxd++;
//    TransDataLength--;
	if(TransTimer1ms)
		TransTimer1ms--;
    

}

/*******************************************************************************
* 描述	    : 串口中断处理函数
*******************************************************************************/
void USART1_IRQHandler(void) 
{
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
			RxdWifiBuffer[RxdDataIndex] = (byte)USART_ReceiveData(USART1);
			if (RxdWifiBuffer[0]==HEKR_FRAME_HEADER) 
					RxdDataIndex++;

			if (RxdDataIndex==4)
			{
				RxdDataLength =RxdWifiBuffer[1];
				if(RxdDataLength>WIFIBUFFER_LEN)
					RxdDataIndex = 0;  // refuse to receive
			}

			RecvTimer1ms =100;
			if((RxdDataIndex==RxdDataLength)&&RxdDataIndex)
			{
				PostMessage(MessageWifiRecv, HekrRecvDataHandle());
				RxdDataIndex =0;
				RecvTimer1ms = 0;
				RxdDataLength =0;
			}
			
			if(RxdDataIndex>WIFIBUFFER_LEN)
				RxdDataIndex = 0;  // refuse to receive
			
    } 
		
		if(USART_GetFlagStatus(USART1, USART_FLAG_ORE)!= RESET)
		{
			USART_ReceiveData(USART1);
			USART_ClearFlag(USART1, USART_FLAG_ORE);
		}
		if(USART_GetFlagStatus(USART1, USART_FLAG_NE)!= RESET)
		{
			USART_ClearFlag(USART1, USART_FLAG_NE);
		}
		if(USART_GetFlagStatus(USART1, USART_FLAG_FE)!= RESET)
		{
			USART_ClearFlag(USART1, USART_FLAG_FE);
		}
		if(USART_GetFlagStatus(USART1, USART_FLAG_PE)!= RESET)
		{
			USART_ClearFlag(USART2, USART_FLAG_PE);
		}
		
} 

static void InitWifi(byte* RxValidDateBuffer,byte* TxValidDateBuffer,byte* ModuleStatus)
{
	pRxValidDataBuffer=RxValidDateBuffer;
	pTxValidDataBuffer=TxValidDateBuffer;
	pModuleStatus = ModuleStatus;
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //开启中断
	USART_Cmd(USART1, ENABLE);
}

void Init_WifiUSART1(void) 
{
	USART_InitTypeDef USART_InitStructure;
//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	  /** USART2 GPIO Configuration  
  PA9   ------> USART2_TX
  PA10   ------> USART2_RX
  */

  /*Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /*Configure GPIO pin : PA */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*Configure GPIO pin alternate function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);

  /*Configure GPIO pin alternate function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

	RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);
//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority=3;

  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);	
  
//USART 初始化设置
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 
   
  DelayMsNull(3000);
	
#ifdef __WIFI_VALIDE
  USART_Cmd(USART1, ENABLE);                              //使能串口 
		while(1)
		{
			System.Device.Led.LedModeSet(LED_GREEN,TURN_ON);
				if(HekrModuleProdKey() == PASSED)
					break;
			System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
				if(HekrModuleProdKey() == PASSED)
					break;
		}
#endif
  USART_Cmd(USART1, DISABLE);                              //使能串口 
	
	System.Device.Led.LedModeSet(LED_GREEN,TURN_OFF);
  System.Device.Wifi.InitWifi = InitWifi;
  System.Device.Wifi.HekrModuleControl = HekrModuleControl;
  System.Device.Wifi.HekrValidDataUpload = HekrValidDataUpload;
	
}

