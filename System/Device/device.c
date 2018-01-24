/*
********************************************************************************
*文件名       : device.c
*作用         : 设备接口
*原理         : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "system.h"

byte TimeOut10ms=0;

//extern void InitUsart1(void);
extern void Init_USART2(void);

extern void Init_IRReceiver(void);

extern void Init_E2prom(void) ;

extern void InitSystick(void);

extern void Init_Lcd(void);

extern void Init_Led(void);

//extern void Init_RTC(void);

extern void Init_WifiUSART1(void);

extern void Init_TouchButtons(void);

extern void Init_Timer(void);

/*******************************************************************************
* 描述	    : 初始化设备
*******************************************************************************/
static void RTC_SetFlag(byte reg)
{
   	  /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5*/
//	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
	
	switch(reg)
	{
		case 2:
			 RTC_WriteBackupRegister(RTC_BKP_DR1, 0xb5b5);
			break;
		case 3:
			 RTC_WriteBackupRegister(RTC_BKP_DR2, 0xc5c5);
			break;
		default:
			break;
	}

}

void InitDevice(void)
{    

    Init_USART2();	
	
		Init_IRReceiver();	

    Init_E2prom();
	
	  Init_Timer();
	
  	Init_Led();
	
//		Init_RTC();

    Init_Lcd();
	
	Init_TouchButtons();
	
#ifdef __WIFI_VALIDE
	  Init_WifiUSART1();
#endif	
    InitSystick();
		System.Device.Rtc.RTC_SetFlag=RTC_SetFlag;
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     // 抢占优先级|非抢占优先级,0为高
                                                        // bit3 bit2 | bit1 bit0 
}


