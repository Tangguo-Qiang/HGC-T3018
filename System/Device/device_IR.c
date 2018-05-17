/*
********************************************************************************
*文件名     : device_IR.c
*作用       : 遥控接收、解码
*原理       : 使用NEC 0007F 38K 
********************************************************************************
*版本     作者            日期            说明
*V0.1      TGQ        2016/04/10       初始版本
********************************************************************************
*/
#include "system.h"
//#include "drive.h"

/* Exported constants --------------------------------------------------------*/
#define IR_GPIO_PORT          GPIOA                /* Port which IR is connected */
#define IR_GPIO_CLK           RCC_AHBPeriph_GPIOA /* Clock of Port which IR is connected */
#define IR_GPIO_PIN           GPIO_Pin_1           /* Pin which IR is connected */
#define IR_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA /* IR EXTI Port source */
#define IR_EXTI_PIN_SOURCE    EXTI_PinSource1      /* IR EXTI Pin source */
#define IR_EXTI_IRQn          EXTI0_1_IRQn           /* IR EXTI IRQ */
#define IR_EXTI_LINE          EXTI_Line1           /* IR EXTI line */
#define IR_EXTI_IRQHandler    EXTI0_1_IRQHandler     /* IR IRQ handler */

#define IR_TIM                TIM3                 /* Timer used for IR decoding */
#define IR_TIM_CLK            RCC_APB1Periph_TIM3  /* Clock of the used timer */
#define IR_TIM_IRQn           TIM3_IRQn            /* IR TIM IRQ */
#define IR_TIM_IRQHandler     TIM3_IRQHandler      /* IR TIM IRQ handler */

#define   IR_TIM_PRESCALER          17

#define   IR_ALLOWABLE_ERROR_US          300

#define   MIN_FULL_BIT_TIME_US_1       (2250-IR_ALLOWABLE_ERROR_US) /*1340 Minimum Full bit time in 祍 */
#define   MAX_FULL_BIT_TIME_US_1       (2250+IR_ALLOWABLE_ERROR_US) /*2220 Maximum Full bit time in 祍 */
#define   MIN_FULL_BIT_TIME_US_0       (1125-IR_ALLOWABLE_ERROR_US) /*1340 Minimum Full bit time in 祍 */
#define   MAX_FULL_BIT_TIME_US_0       (1125+IR_ALLOWABLE_ERROR_US) /*2220 Maximum Full bit time in 祍 */
#define   MIN_START_TIME_US       (13500-IR_ALLOWABLE_ERROR_US) /*1340 Minimum Full bit time in 祍 */
#define   MAX_START_TIME_US       (13500+IR_ALLOWABLE_ERROR_US) /*2220 Maximum Full bit time in 祍 */
#define   MIN_REPEAT_TIME_US       (11500-IR_ALLOWABLE_ERROR_US) /*1340 Minimum Full bit time in 祍 */
#define   MAX_REPEAT_TIME_US       (11500+IR_ALLOWABLE_ERROR_US) /*2220 Maximum Full bit time in 祍 */

#define IR_ONEFREAME_PERIOD  		(uint8_t)108  //110ms

#define IR_BYTE0  0x48
#define IR_BYTE1  0x79
#define IR_BYTE2  0xAA
#define IR_BYTE3  0xBB

#define IR_ORDER_FLOWPLUS		0x99667948
#define IR_ORDER_FLOWMINUS  0xBB447948

static uint IR_Data=0;
static uint IR_DataEx=0;

static ushort Min_FullBit_TimerUs_1 =0;
static ushort Max_FullBit_TimerUs_1 =0;
static ushort Min_FullBit_TimerUs_0 =0;
static ushort Max_FullBit_TimerUs_0 =0;
static ushort Min_Repeat_TimerUs =0;
static ushort Max_Repeat_TimerUs =0;
static ushort Min_Start_TimerUs =0;
static ushort Max_Start_TimerUs =0;
static byte IR1frameTimer=0;
static byte EdgesNumber=0;

/**
  * @brief  Decode the IR frame.
  * @param  None
  * @retval IR structure
  */
static void IR_Decode(void)
{
   byte val = 0;
	byte key =0;
	uint order;
	
	order = IR_Data;
	IR_DataEx = IR_Data;
   
    
   val = (order&0xff);
	 
 	 if(val == IR_BYTE0)
	 {
		 order >>=8;
		 val = (order&0xff);
		 if(val == IR_BYTE1)
		 {
			 order >>=8;
			 key = (order&0xff);
			 order >>=8;
			 val = (order&0xff);
			 val += key;
			 if(0xFF !=val)
				 key =0;			 
		 }		 
	 }
	 
	 if(key)
	 {
		 PostMessage(MessageIR, key);
	 }
		 
}

void IR_MeasureLowDuration(void)
{
  ushort LowDuration=0;
  
	EdgesNumber++;

    /* If it was the first edge */
    if(EdgesNumber == 1)
    {
      if(!IR1frameTimer)
			{
				/* Reset the IR_TIM counter */
				IR_TIM->CNT = 0;
				
				/* Enable IR_TIM counter */
				TIM_Cmd(IR_TIM, ENABLE); 
				
				IR1frameTimer = IR_ONEFREAME_PERIOD;  
			}
			else 
				EdgesNumber=0;
    }
    /* If it was the 2nd edge */
    else if(EdgesNumber == 2)
    {
      /* Disable IR_TIM counter */
      TIM_Cmd(IR_TIM, DISABLE);
      
      /* Read IR_TIM counter to get the first low duration */
      LowDuration = IR_TIM->CNT;
      
      /* Reset IR_TIM counter */
      IR_TIM->CNT = 0;
    
			if ((LowDuration > Min_Start_TimerUs) && (LowDuration <Max_Start_TimerUs)) 
      {


        /* Enable IR_TIM counter */
        TIM_Cmd(IR_TIM, ENABLE);
				IR_Data=0;


      }
			else if ((LowDuration > Min_Repeat_TimerUs) && (LowDuration <Max_Repeat_TimerUs)) 
      {
				if((IR_ORDER_FLOWPLUS==IR_DataEx)||(IR_ORDER_FLOWMINUS==IR_DataEx))
				{
					EdgesNumber = 34;
					IR_Data = IR_DataEx;
				}
				else
				{
					/* Initialize edges detected number */
					EdgesNumber = 0;
					/* Reset IR_TIM counter */
					IR_TIM->CNT = 0;
					 
					IR1frameTimer = 0;
				}


      }
      else /* If the first low duration is not in the right timing range */
      {  
        /* Initialize edges detected number */
        EdgesNumber = 0;
        /* Reset IR_TIM counter */
        IR_TIM->CNT = 0;
         
        IR1frameTimer = 0;
      }
    }
		else
		{
      /* Disable IR_TIM counter */
      TIM_Cmd(IR_TIM, DISABLE);
      
      /* Read IR_TIM counter to get the first low duration */
      LowDuration = IR_TIM->CNT;
      
      /* Reset IR_TIM counter */
      IR_TIM->CNT = 0;
      /* Enable IR_TIM counter */
      TIM_Cmd(IR_TIM, ENABLE);
    
			if ((LowDuration > Min_FullBit_TimerUs_0) && (LowDuration <Max_FullBit_TimerUs_0)) 
      {
				IR_Data >>=1;
			}
			else if ((LowDuration > Min_FullBit_TimerUs_1) && (LowDuration <Max_FullBit_TimerUs_1)) 
      {
				IR_Data >>=1;
				IR_Data |= 0x80000000;
			}
			else
			{
        /* Enable IR_TIM counter */
        TIM_Cmd(IR_TIM, DISABLE);
      /* Reset IR_TIM counter */
      IR_TIM->CNT = 0;
				EdgesNumber = 0;
				IR1frameTimer = 0;
			}
		}
		
		if(EdgesNumber>33)
		{
			IR_Decode();
			EdgesNumber =0;
		}
   
}

void IRSystick1000Routine(void) 
{
	if(IR1frameTimer)
	{
		IR1frameTimer--;
		if(!IR1frameTimer)
		{
				IR_TIM->CNT = 0;
				
				/* Enable IR_TIM counter */
				TIM_Cmd(IR_TIM, DISABLE); 
			EdgesNumber=0;
		}
	}
}

/* I/O线中断，中断线为PA1*/
void IR_EXTI_IRQHandler(void)
{
/* Mesure first duration to validate the IR frame */
	if(EXTI_GetITStatus(IR_EXTI_LINE) != RESET)
	{
		IR_MeasureLowDuration();
    /* Clear the IR EXTI line pending bit */
    EXTI_ClearITPendingBit(IR_EXTI_LINE);
	}

//	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
//	{
//		bRadio_Check_Tx_RX();
//    /* Clear the IR EXTI line pending bit */
//    EXTI_ClearITPendingBit(EXTI_Line0);
//	}
}

static void IR_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = IR_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority  = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

static void RemoteIRCtrl(FunctionalState state)
{
	EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Enable EXTIx to detect the start bit of the IR frame */
  EXTI_ClearITPendingBit(IR_EXTI_LINE);
  EXTI_InitStructure.EXTI_Line = IR_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = state;
  EXTI_Init(&EXTI_InitStructure);
	
 	IR1frameTimer = 0;
	EdgesNumber =0;

}


void Init_IRReceiver(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	uint IR_TIM_CLKValuekHz = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* GPIOx clock enable for IR input pin */
  RCC_AHBPeriphClockCmd(IR_GPIO_CLK  , ENABLE);
	
  /* System Clocks Configuration for IR reception */
  RCC_APB1PeriphClockCmd(IR_TIM_CLK, ENABLE);


  /*Configure GPIO pin :  IR-PA0*/
  GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(IR_EXTI_PORT_SOURCE,IR_EXTI_PIN_SOURCE);

  /* Configure the GPIO port for IR reception */
//  IR_GPIO_Configuration();

  /* Get frequency input of IR_TIM in Khz */
  IR_TIM_CLKValuekHz = (SystemCoreClock/(IR_TIM_PRESCALER+1))/1000;
  
  /* Compute the different timing tolerences in TIM count */
  Min_FullBit_TimerUs_1 = (IR_TIM_CLKValuekHz * MIN_FULL_BIT_TIME_US_1)/1000;
  Max_FullBit_TimerUs_1 = (IR_TIM_CLKValuekHz * MAX_FULL_BIT_TIME_US_1)/1000;
	
  Min_FullBit_TimerUs_0 = (IR_TIM_CLKValuekHz * MIN_FULL_BIT_TIME_US_0)/1000;
  Max_FullBit_TimerUs_0 = (IR_TIM_CLKValuekHz * MAX_FULL_BIT_TIME_US_0)/1000;
	
  Min_Start_TimerUs =   (IR_TIM_CLKValuekHz * MIN_START_TIME_US)/1000;
  Max_Start_TimerUs =   (IR_TIM_CLKValuekHz * MAX_START_TIME_US)/1000;
	
  Min_Repeat_TimerUs =  (IR_TIM_CLKValuekHz * MIN_REPEAT_TIME_US)/1000;
  Max_Repeat_TimerUs =  (IR_TIM_CLKValuekHz * MAX_REPEAT_TIME_US)/1000;

  /* RC5_TIM Configuration ------------------------------------------------------*/
  TIM_DeInit(IR_TIM); 
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_Prescaler = IR_TIM_PRESCALER;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(IR_TIM, &TIM_TimeBaseStructure);
  /* NVIC configuration for IR reception */
  IR_NVIC_Configuration(); 
  
 	IR1frameTimer = 0;
	
	System.Device.RemoteIR.RemoteIRCtrl = RemoteIRCtrl;
}
