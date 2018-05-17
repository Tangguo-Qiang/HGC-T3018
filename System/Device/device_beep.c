/*
********************************************************************************
*文件名     : device_beep.c
*作用       : 系统节拍
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V1.1     tgq       2016/5/12       初始版本
********************************************************************************
*/

#include "system.h"

#define BEEP_PORT	GPIOB
#define BEEP_PIN	GPIO_Pin_13
#define BEEP_OFF  GPIO_ResetBits(BEEP_PORT,BEEP_PIN)
#define BEEP_ON  	GPIO_SetBits(BEEP_PORT,BEEP_PIN)

#define BEEPDIRVE_TIM		   				TIM14
#define BEEPDIRVE_TIM_CLOCK		   	RCC_APB1Periph_TIM14
#define BEEPDIRVE_TIM_IRQ  				TIM14_IRQn
#define BEEPDIRVE_TIM_IRQHandler  TIM14_IRQHandler

#define BEEP_INVERSE			GPIOB->ODR ^= BEEP_PIN

//音阶常亮
//#define	Do 262  
//#define Re 294  
//#define Mi 330  
//#define Fa 349  
//#define Sol 392  
//#define La 440  
//#define Si 494  
#define St	0
#define	Do 	1  
#define Re 	2  
#define Mi 	3  
#define Fa 	4  
#define Sol	5  
#define La 	6  
#define Si 	7 

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//static short int scale[] =    {Do,   Re,  Mi,  Fa,  Sol,La,  Si};
const uint16_t NoteHalfWide[]={0,1908,1700,1515,1432,1275,1136,1012};
const uint16_t NoteHalfWideMid[]={0,954,850,757,716,638,568,506};
const uint16_t NoteHalfWideHigh[]={0,477,425,378,358,319,284,253};

const uint8_t PowerOnNote[]= {Si,Mi,Mi,St};
const uint8_t PowerOnTime10ms[]= {5,10,5,St};
const uint8_t PowerOffNote[]= {Mi,Mi,Si,St};
const uint8_t PowerOffTime10ms[]= {5,10,5,St};
const uint8_t ShortMiNote[]= {Mi,St};
const uint8_t ShortMi10ms[]= {10,St};
const uint8_t Short2MiNote[]= {Mi,St,Mi,St};
const uint8_t Short2Mi10ms[]= {15,10,15,10};
const uint8_t Short3MiNote[]= {Mi,St,Mi,St,Mi,St};
const uint8_t Short3Mi10ms[]= {10,10,10,10,10,St};
const uint8_t LongMiNote[]= {Fa,Sol,La,St};
const uint8_t LongMi10ms[]= {10,10,10,St};

static uint8_t BeepTim10ms=0;

static const uint8_t (*NotePointer);
static const uint8_t (*Time10msPointer);

static void BeepOn(BeepTypedef val)
{
	if(!val)
	{
		BEEP_OFF;
		return;
	}
	
	switch(val)
	{
		case BEEP_POWERON:
			NotePointer = PowerOnNote;
		  Time10msPointer = PowerOnTime10ms;
			break;
		case BEEP_POWEROFF:
			NotePointer = PowerOffNote;
		  Time10msPointer = PowerOffTime10ms;
			break;
		case BEEP_SHORT:
			NotePointer = ShortMiNote;
		  Time10msPointer = ShortMi10ms;
			break;
		case BEEP_2SHORT:
			NotePointer = Short2MiNote;
		  Time10msPointer = Short2Mi10ms;
			break;
		case BEEP_3SHORT:
			NotePointer = Short3MiNote;
		  Time10msPointer = Short3Mi10ms;
			break;
		case BEEP_LONG:
			NotePointer = LongMiNote;
		  Time10msPointer = LongMi10ms;
			break;
		default:
			break;

	}
	BeepTim10ms=*Time10msPointer;
	TIM_TimeBaseStructure.TIM_Period = (NoteHalfWideHigh[*NotePointer]);
  TIM_TimeBaseInit(BEEPDIRVE_TIM, &TIM_TimeBaseStructure);
	TIM_Cmd(BEEPDIRVE_TIM, ENABLE);   
	TIM_ITConfig(BEEPDIRVE_TIM, TIM_IT_Update, ENABLE);
}

void BeepSystick100Routine(void)
{
	if(BeepTim10ms)
	{
		BeepTim10ms--;
		if(!BeepTim10ms)
		{
			BeepTim10ms=*(++Time10msPointer);
			if(BeepTim10ms)
			{
				NotePointer++;
				TIM_TimeBaseStructure.TIM_Period = (NoteHalfWideHigh[*NotePointer]); 
				TIM_TimeBaseInit(BEEPDIRVE_TIM, &TIM_TimeBaseStructure);
			}
			else
			{
				TIM_Cmd(BEEPDIRVE_TIM, DISABLE); 
				TIM_ITConfig(BEEPDIRVE_TIM, TIM_IT_Update, DISABLE);
				BEEP_OFF;
			}
			
		}
	}

}

void TIM14_IRQHandler(void) //10us
{
  if (TIM_GetITStatus(BEEPDIRVE_TIM, TIM_IT_Update) != RESET)  //per 10us
  {
    TIM_ClearITPendingBit(BEEPDIRVE_TIM, TIM_IT_Update );
		BEEP_INVERSE;

	}
}

/*******************************************************************************
* 描述	    : TIM14作为蜂鸣器的基本定时器，时钟频率1us
*           : 
*******************************************************************************/
static void TimBeepDriver_ModeConfig(void)  //休眠灯PWM发生器
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	uint16_t PrescalerValue;


  /* TIM15 clock enable */
  RCC_APB1PeriphClockCmd(BEEPDIRVE_TIM_CLOCK, ENABLE);

/* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 10-1; //10us
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(BEEPDIRVE_TIM, &TIM_TimeBaseStructure);


//  /* TIM4 enable counter */
//  TIM_Cmd(TIM6, ENABLE);
//  /* TIM IT enable */
//  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
  /* --------------------------NVIC Configuration -------------------------------*/
  /* Enable the TIM14 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = BEEPDIRVE_TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}


void Init_Beep(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /*Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	/* BEEP  PA15*/
  GPIO_InitStruct.GPIO_Pin = BEEP_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(BEEP_PORT, &GPIO_InitStruct);
	BEEP_OFF;

	TimBeepDriver_ModeConfig();
	
//	System.Device.Beep.BeepOnLong =BeepOnLong;
	System.Device.Beep.BeepOn =BeepOn;

}
