#include "system.h"


static  RTC_TimeTypeDef  RTC_TimeStruct;
static  RTC_DateTypeDef  RTC_DateStruct;

/**
  * @brief  Configures the RTC peripheral and select the clock source.
  * @param  None
  * @retval None
  */
static void RTC_Config(void)
{
  RTC_InitTypeDef  RTC_InitStructure;



  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

//  /* Reset RTC Domain */
//  RCC_BackupResetCmd(ENABLE);
//  RCC_BackupResetCmd(DISABLE);
    
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  

  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  /* Configure the RTC data register and RTC prescaler */
  /* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);
  
		
  RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours = 0;
  
  RTC_TimeStruct.RTC_Minutes = 00;
  RTC_TimeStruct.RTC_Seconds = 00;
  /* Configure the RTC time register */
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	
	RTC_DateStruct.RTC_Year = 16;
	RTC_DateStruct.RTC_Month = RTC_Month_August;
	RTC_DateStruct.RTC_Date = 26;
	RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Friday;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

  /* Disable the Alarm A */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		
		
}

/**
  * @brief  Configures the RTC Alarm.
  * @param  None
  * @retval None
  */
static void RTC_AlarmConfig(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

    
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void RTC_Alarm1hourSet(void)
{
  RTC_AlarmTypeDef RTC_AlarmStructure;
	
		/* Enable the RTC Alarm A Interrupt */
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		/* Disable the Alarm A */
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

  RTC_AlarmStructInit(&RTC_AlarmStructure);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	
//	RTC_TimeStruct.RTC_Hours +=	1;

	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = (RTC_TimeStruct.RTC_Hours+1)%24;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_TimeStruct.RTC_Minutes;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_TimeStruct.RTC_Seconds;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	
	/* Enable the RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	/* Enable the Alarm A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

/**
  * @brief  This function handles RTC Alarm interrupt (A and B) request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
  /* Check on the AlarmA flag and on the number of interrupts per Second (60*8) */
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) 
  { 
    /* Clear RTC AlarmA Flags */
    RTC_ClearITPendingBit(RTC_IT_ALRA);
  /* Clear the EXTIL line 17 */
  EXTI_ClearITPendingBit(EXTI_Line17);
 		
		/* Enable the RTC Alarm A Interrupt */
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		/* Disable the Alarm A */
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    /* ALARM is enabled */
 		PostMessage(MessageAlarm, ALARM_SOUND);
    
  }
 
}

void Init_RTC(void)
{
	RTC_Config();
	RTC_AlarmConfig();
	
	System.Device.Rtc.RTC_Alarm1hourSet = RTC_Alarm1hourSet;
}
