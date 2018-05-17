#include "system.h"

#define KEYACT_DELAY10MS	5
#define KEYLED_LIGHTON10MS	500

#define BTNLED_PORT1 								GPIOB
#define BTNLED_PIN1 								(GPIO_Pin_14)
#define BTNLED_PORT2 								GPIOB
#define BTNLED_PIN2 								(GPIO_Pin_15)

#define LED_ALL_ON		{GPIOB->BSRR = (BTNLED_PIN1|BTNLED_PIN2);}
#define LED_ALL_OFF		{GPIOB->BRR = (BTNLED_PIN1|BTNLED_PIN2);}

#define BTNTOUCH_PORT 								GPIOB
#define BTNTOUCH_PINS 								(GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7)
	
#define NOBTN_PRESS   0x07


#define ShortInterval       3		// 短按按键间隔，不得低于30ms
#define LongInterval        80		// 长按按键间隔
#define InvalidInterval     3       // 无效识别长度
#define DoubleHitInterval   20		// 防连续双击误动间隔
#define KeyBeepInterval     20      // 按键音长度

#define POSTKEY_INTERVALMIN_10MS			10
	
//static byte Delay10ms=0;
static ushort LedLightOn10ms=0;

static byte KeyIntervalMin10ms=0;

static byte ScanData[3];

static byte ValidCounter = 0;
static byte InvalidCounter = 0;
static byte DoubleHitCounter = 0;


static byte RemapKey(byte scan) 
{
    switch(scan)
    {
        case 0x05:  return(KeyPower);         
        case 0x06:  return(KeyMinus);   
        case 0x03:  return(KeyPlus);
        case 0x02:  return(KeyPlusMinus);
        case 0x04:  return(KeyPowerMinus);   
        case 0x01:  return(KeyPowerPlus);
        default:    return(NOBTN_PRESS);  
    }
}

static byte RemapLongKey(byte scan) 
{
    switch(scan)
    {
        case 0x05:  return(KeyLongPower);
        case 0x06:  return(KeyLongMinus);
        case 0x03:  return(KeyLongPlus);
        case 0x02:  return(KeyLongPlusMinus);
        case 0x04:  return(KeyLongPowerMinus);   
        case 0x01:  return(KeyLongPowerPlus);
        default:    return(NOBTN_PRESS);
    }	
}


static byte ScanPin(void)
{   
  uint16_t btntouch;
  
	btntouch = GPIO_ReadInputData(BTNTOUCH_PORT); 
	btntouch &= (uint16_t)BTNTOUCH_PINS;
	btntouch >>= 5;
    

    return(btntouch&NOBTN_PRESS);
}


/*******************************************************************************
* 描述	    : 系统节拍100/S，即10mS一次扫描获取按键值，在多次检测确认后，
*           : 发送按键的映射消息到LogicTask的消息队列中。
*******************************************************************************/
void KeySystick100Routine(void) 
{
    byte scan;
    byte key;

		if(KeyIntervalMin10ms)
			KeyIntervalMin10ms--;
		

	if(LedLightOn10ms)
	{
		LedLightOn10ms--;
		if(!LedLightOn10ms)
			LED_ALL_OFF;
	}

	scan = ScanPin();
	
    if (scan != NOBTN_PRESS) 
    {
        ValidCounter++;
        InvalidCounter = 0;
		
        if (ValidCounter == 1) 
            ScanData[0] = scan;
        else if(ValidCounter == 2)
				{
           ScanData[1] = scan;
					if (ScanData[0] != ScanData[1])
					{
						ScanData[0] = ScanData[1];
						ValidCounter =1;
					}
				}
        else if(ValidCounter == 3)
				{
            ScanData[2] = scan;
					if(ScanData[1] != ScanData[2])
					{
						ScanData[0] = ScanData[2];
						ValidCounter =1;
					}
				}
				else if(ScanData[0] != scan)
				{
					ScanData[0] = scan;
					ValidCounter =1;
				}
        else if (ValidCounter > LongInterval) 
				{ 
						ValidCounter = LongInterval;
//						key = RemapLongKey(key);
//						PostMessage(MessageKey, key); 
				}
    }
    else
	  {
        InvalidCounter++;
        if (InvalidCounter >= InvalidInterval)
        {   
            InvalidCounter = InvalidInterval;
       
            if(DoubleHitCounter)
            {
                DoubleHitCounter--;
                ValidCounter = 0;
                return;
            }

            if (ValidCounter < ShortInterval) return;
						if(ValidCounter > LongInterval) return;

//            if (ScanData[0] == ScanData[1])
//                key = ScanData[0];
//            else if (ScanData[0] == ScanData[2])
//                key = ScanData[0];
//            else if (ScanData[1] == ScanData[2])
                key = ScanData[1];
//            else
//            {
//                ValidCounter = 0;
//                return;
//            }

            if (ValidCounter == LongInterval) 
                key = RemapLongKey(key);
            else if (ValidCounter >= ShortInterval) 
                key = RemapKey(key);
            else
                key = NOBTN_PRESS;
            
            if (key != NOBTN_PRESS)
            {
              if(!KeyIntervalMin10ms)
							{
								PostMessage(MessageKey, key); 
								KeyIntervalMin10ms=POSTKEY_INTERVALMIN_10MS;
							}								
                DoubleHitCounter = DoubleHitInterval;
            }
            ValidCounter = 0;
    	} 
    }

}

static void KeyLedSet(byte OnSeconds)
{
	LedLightOn10ms=OnSeconds*100;
	LED_ALL_ON;
}

void Init_TouchButtons(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
 	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Pin = BTNTOUCH_PINS;
  GPIO_Init(BTNTOUCH_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Pin = BTNLED_PIN1;
  GPIO_Init(BTNLED_PORT1, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Pin = BTNLED_PIN2;
  GPIO_Init(BTNLED_PORT2, &GPIO_InitStructure);
	

	LED_ALL_OFF;
	
	System.Device.Key.KeyLedSet=KeyLedSet;
  
}



