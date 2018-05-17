/*
********************************************************************************
*�ļ���       : device.c
*����         : �豸�ӿ�
*ԭ��         : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
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

extern void Init_Beep(void);

extern void Init_rht(void);

extern void Si4463_Init(void);

/*******************************************************************************
* ����	    : ��ʼ���豸
*******************************************************************************/
static void RTC_SetFlag(byte reg)
{
   	  /*������ʱ��鱸�ݼĴ���BKP_DR1��������ݲ���0xA5A5*/
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
	Init_Beep();
#ifdef __WIFI_VALIDE
	  Init_WifiUSART1();
#endif	
    InitSystick();
		Init_rht();
		Si4463_Init();
		System.Device.Rtc.RTC_SetFlag=RTC_SetFlag;
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     // ��ռ���ȼ�|����ռ���ȼ�,0Ϊ��
                                                        // bit3 bit2 | bit1 bit0 
}


