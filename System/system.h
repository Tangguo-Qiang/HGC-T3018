/*
********************************************************************************
*文件名   : system.h
*作用     : 系统库全局头文件
*创建时间 : 
********************************************************************************
*/


#ifndef __SYSTEM_H
#define __SYSTEM_H


//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>

#include "sysdefs.h"


/*
1、类型定义兼容C#，尽可能用C#标准，常用的为int
2、char在C#中为16bit的unicode，keil C下为8bit
*/
//8bit
//typedef char                sbyte;	    /*char Keil4默认定义为byte，若要带符号，需要更改keil设置*/
typedef unsigned char       byte;       
//16bit
//typedef short             short
typedef unsigned short      ushort;
//32bit
//typedef int               int;
typedef unsigned int        uint;

//字符串
typedef char *              string;

//typedef unsigned char       bool;

typedef void (*function)(void);
#define Function(address)   ((function)(address))()


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


/*
1、类型定义兼容C#，尽可能用C#标准，常用的为int
2、char在C#中为16bit的unicode，keil C下为8bit
*/
//8bit
//typedef char                sbyte;	    /*char Keil4默认定义为byte，若要带符号，需要更改keil设置*/
typedef unsigned char       byte;       
//16bit
//typedef short             short
typedef unsigned short      ushort;
//32bit
//typedef int               int;
typedef unsigned int        uint;

//字符串
typedef char *              string;

typedef unsigned char       Bool;

typedef void (*function)(void);
#define Function(address)   ((function)(address))()



#define TRUE                        1
#define FALSE                       0
#define null                        ((void *) 0)                    // 空指针
#define invalid                     0xFF                            // 无效值
#define MainClock                   72000000                        // 系统时钟频率
#define IdentifyNumberAddress       0x1FFFF7E8                      // 芯片ID号

#define SetBit(data, offset)        ((data) |= 1U << (offset))      // 置位
#define ResetBit(data, offset)      ((data) &= ~(1U << (offset)))   // 复位
#define GetBit(data, offset)        (((data) >> (offset)) & 0x01)   // 获取位

#define GetByte0FromUint(data)      ((byte *)(&(data)))[0]
#define GetByte1FromUint(data)      ((byte *)(&(data)))[1]
#define GetByte2FromUint(data)      ((byte *)(&(data)))[2]
#define GetByte3FromUint(data)      ((byte *)(&(data)))[3]

#define GetUshort0FromUint(data)    ((ushort *)(&(data)))[0]
#define GetUshort1FromUint(data)    ((ushort *)(&(data)))[1]

#define GetMessageType(data)        GetByte3FromUint(data)
#define GetMessageData(data)        (data & 0x00FFFFFF)

#define GetByteFrom(address)        *((byte *)(address)) 
#define GetUshortFrom(address)      *((ushort *)(address)) 
#define GetUintFrom(address)        *((uint *)(address)) 

#define Assert(express)  if (!(express)){printf("\nASSERT: " #express "\n");}   //(void)0


typedef enum
{
    MessageKey       		 = 0x01,
    MessageCommRecv      = 0x02,
    MessageCommTrans     = 0x03,
    MessageIR    				 = 0x04,
    MessageAlarm       = 0x05,
    MessageParaUpdate    = 0x06,
    MessageEchoDate    	 = 0x07,
    MessageWifiRecv      = 0x08,
    MessageWifiCtrl      = 0x09,
    MessageWifiUpload      = 0x0A,
    MessageProcess      = 0x0B,
	
    MessageRfCtrl      = 0x0C,
    MessageRfRecv      = 0x0D,
    MessageTimer   
    /*请填充类型*/
}MessageEnum;

typedef enum{
	COMM_TEMPER_RH =(byte)1,
	COMM_XFMOTODUTY =(byte)2,
	COMM_PFMOTODUTY =(byte)3,
	COMM_CIRCLEMODE =(byte)4,
//	COMM_THERMALMODE =(byte)5,
//	COMM_VENTILATE =(byte)6,
	COMM_IAQ_READ =(byte)7,
	COMM_CO2_READ =(byte)8,
	COMM_POWER_SET =(byte)9,
	COMM_BEEPONE =(byte)0x0A,
	COMM_BEEPTWO =(byte)0x0B,
	COMM_BEEPTHREE =(byte)0x0C,
	COMM_BEEPLONG =(byte)0x0D,
	COMM_HEATERSET =(byte)0x0E,
	COMM_VERSION =(byte)0xEE
}CommOrderTypedef;


typedef enum{
	PARA_POWER_SET = (uint)0x10,
	PARA_XFMOTODUTY = (uint)0x20,
	PARA_PFMOTODUTY = (uint)0x30,
	PARA_CIRCLEMODE = (uint)0x40,
	PARA_THERMALMODE = (uint)0x50,
	PARA_VENTILATE = (uint)0x60,
	PARA_MUTEMODE = (uint)0x70,
	PARA_WORKMODE = (uint)0x80,
	PARA_CHILDLOCK = (uint)0x90,
	PARA_HEATSET = (uint)0xA0
}ParaOperTypedef;

typedef enum{
	ALARM_SOUND_ONEHOUR= (uint)0x100,
	ALARM_START = (uint)0x200,
	ALARM_INIT = (uint)0x300
}AlarmTypedef;

typedef enum{
	FILTER_ONEHOUR = (uint)0x1000,
	FILTER_CLEAR = (uint)0x2000,
	FILTER_CHECK = (uint)0x3000,
	FILTER_POWERON = (uint)0x4000,
	FAULTINFO_DISP = (uint)0x5000,
	IAQFLAG_DISP = (uint)0x6000
}ProcessTypedef;

typedef enum{
	FIRSTFILTER_CHECK = (uint)0x01,
	MIDFILTER_CHECK = (uint)0x02,
	ESPFILTER_CHECK = (uint)0x04,
	HPFILTER_CHECK = (uint)0x08,
	POWERBASE_FAULT = (uint)0x10,
	SENSOROUTTEMP_FAULT = (uint)0x20,
	SENSORINTEMP_FAULT = (uint)0x40,
	SENSORRH_FAULT = (uint)0x80,
	SENSORCO2_FAULT = (uint)0x100,
	SENSORPM_FAULT = (uint)0x200,
	XFMOTO_FAULT = (uint)0x400,
	PFMOTO_FAULT = (uint)0x800,
	CO2INSIDEBEYOND=(uint)0x1000,
	PMINSIDEBEYONG=(uint)0x2000,
	HGA_FAULT = (uint)0x4000,
	STORE_FAULT=(uint)0x8000,

	FAULTICON_DISP = (uint)0x10000
}FaultTypedef;

typedef enum{
	TIMER1 = (uint)0x01,
	TIMER2 = (uint)0x02,
	TIMER3 = (uint)0x04,
	TIMER4 = (uint)0x08
}TimerIDTypedef;

typedef enum{
	BEEP_POWERON = (uint)0x01,
	BEEP_POWEROFF = (uint)0x02,
	BEEP_SHORT = (uint)0x03,
	BEEP_LONG = (uint)0x04,
	BEEP_3SHORT = (uint)0x05,
	BEEP_2SHORT = (uint)0x06
}BeepTypedef;

typedef enum{
	HIMIDITY_READ = (byte)0xF5,
	TEMPER_READ = (byte)0xF3,
}RHTReadTypedef;

#define RADIO_MAX_PACKET_LENGTH     50u

#include "device.h"  
#include "logicctrl.h"

#include "os.h"
#include "gui.h"


extern byte GuiTimer100ms;
extern byte GuiTimer10ms;
extern uint32_t Empty;
extern void Dummy(void);
extern uint16_t CriticalNesting;
#define	EnterCritical()		__disable_irq(); CriticalNesting++;
#define ExitCritical()		if(--CriticalNesting == 0){__enable_irq();}

#define EnableIrq()         __enable_irq();
#define DisableIrq()        __disable_irq();



extern void DelayNull(int times);

extern void DelayUsNull(int times);

extern void DelayMsNull(int times);

extern byte HexToAscii(byte hex);

extern TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
extern void Fill_BufferZero(uint8_t *pBuffer, uint16_t BufferLength);

extern void PostMessage(MessageEnum message, uint data);

/*******************************************************************************
* 结构体名	: SystemStruct
* 描述	    : msOS中最重要的一个结构体，封装了System层的所有接口
********************************************************************************
*版本     作者            日期            说明
*V0.1    tgq        2013/09/11       初始版本
*******************************************************************************/
typedef struct
{
    void (*Init)(void);
    
    struct Device
    {     
      
        struct RemoteIR
        {
            void(*RemoteIRCtrl)(FunctionalState state);
        }RemoteIR;
      
       struct Eeprom
        {
            TestStatus (*I2C_EE_BufferRead)(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
					  TestStatus (*I2C_EE_BufferWrite)(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
        }Eeprom;
				
        struct Led
        {
            void(*LedModeSet)(LedTypedef led,LedModeTypedef mode);
        }Led;
				
        struct Key
        {
            void(*KeyLedSet)(byte OnSeconds);
        }Key;

        struct Beep
        {
            void(*BeepOn)(BeepTypedef val);
        }Beep;
        
        struct Usart2
        {
            void (*Open)(void);
            void (*Close)(void);
//            TestStatus (*WriteByte)(byte data);
            TestStatus (*WriteString)(byte* data,byte length);
            TestStatus (*ReadByte)(byte* data); 

        }Usart2;				
        				
        struct Lcd
        {
            void (*ScreenClear)(void);
            void (*DispUnit)(LCD_DispTypeDef Icon);
            void (*NonDispUnit)(LCD_DispTypeDef Icon);
        }Lcd;
 				
        
        struct Rtc
        {
//            void (*RTC_Alarm1hourSet)(void); 
            void (*RTC_SetFlag)(byte reg);
        }Rtc;
        
        struct Timer
        {
            void (*Start)(TimerIDTypedef id, TimerModeEnum mode, int times, AlarmTypedef TimerMode);
            void (*Stop)(TimerIDTypedef id); 
        }Timer;


        struct Systick
        {
            uint8_t (*Register)(SystickEnum type, function registerFunction);
        }Systick;
      
        struct Wifi
        {
					void (*InitWifi)(byte* RxValidDateBuffer,byte* TxValidDateBuffer,byte* ModuleStatus);
					void (*HekrModuleControl)(byte data);
					void (*HekrValidDataUpload)(byte len);
        }Wifi;
        
        struct RhT
        {
            int32_t (*ReadRhTSensor)(RHTReadTypedef whatdo);
        }RhT;
				
        struct RF
        {
					void (*RfInit)(uint8_t *pCustomPacked);
          void (*RfStartRX)(uint8_t channel, uint8_t packetLenght );
					void (*RfStartTX)(uint8_t channel, uint8_t *pioRadioPacket, uint8_t length);
					uint8_t (*RfCheck)(void)
;
        }RF;


    }Device;
		
		struct Gui
    {
			void (*ParseForm)(FormTypeDef * formPointer);
			void (*AddDispObj)(FormTypeDef * formPointer, DispObjType *objPointer);
			void (*LoadForm)(FormTypeDef * formPointer);
//			void (*LoadForm)(FormDispTypeDef* FormDisp);
//			void (*InitDispSensorData)(SensorDataTypedef* SensorData);
//			void (*InitDispSysCtrlPara)(SysCtrlParaTypedef* SysCtrlPara);
//			void (*InitDispSysStatus)(SysStateTypedef* SysStatus);
    }Gui;

    struct OS
    {
        uint8_t (* PostMessageQueue)(uint message);
        uint (* PendMessageQueue)(void);
        void (* Start)(void); 
        void (* DelayMs)(int times); 
    }OS;


}SystemStruct;



extern SystemStruct System;

#endif 
