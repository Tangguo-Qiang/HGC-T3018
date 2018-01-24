/*
********************************************************************************
*文件名     : gui.h
*作用       : 
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/



#ifndef __GUI_H
#define __GUI_H



typedef enum{
	LCD_16D=0,
	LCD_16C,
	LCD_16B,
	LCD_16A,
	LCD_T14,
	LCD_16E,
	LCD_16G,
	LCD_16F,
	LCD_15D,
	LCD_15C,
	LCD_15B,
	LCD_15A,
	LCD_T10,
	LCD_15E,
	LCD_15G,
	LCD_15F,
	LCD_14D,
	LCD_14C,
	LCD_14B,
	LCD_14A,
	LCD_T31,
	LCD_14E,
	LCD_14G,
	LCD_14F,
	LCD_13D,
	LCD_13C,
	LCD_13B,
	LCD_13A,
	LCD_T32,
	LCD_13E,
	LCD_13G,
	LCD_13F,
	LCD_12D,
	LCD_12C,
	LCD_12B,
	LCD_12A,
	LCD_T15,
	LCD_12E,
	LCD_12G,
	LCD_12F,
	LCD_11D,
	LCD_11C,
	LCD_11B,
	LCD_11A,
	LCD_T13,
	LCD_11E,
	LCD_11G,
	LCD_11F,
	LCD_4F,
	LCD_4G,
	LCD_4E,
	LCD_T2,
	LCD_4A,
	LCD_4B,
	LCD_4C,
	LCD_4D,
	LCD_5F,
	LCD_5G,
	LCD_5E,
	LCD_T12,
	LCD_5A,
	LCD_5B,
	LCD_5C,
	LCD_5D,
	LCD_6F,
	LCD_6G,
	LCD_6E,
	LCD_T34,
	LCD_6A,
	LCD_6B,
	LCD_6C,
	LCD_6D,
	LCD_7F,
	LCD_7G,
	LCD_7E,
	LCD_T11,
	LCD_7A,
	LCD_7B,
	LCD_7C,
	LCD_7D,
	LCD_1F,
	LCD_1G,
	LCD_1E,
	LCD_T1,
	LCD_1A,
	LCD_1B,
	LCD_1C,
	LCD_1D,
	LCD_2F,
	LCD_2G,
	LCD_2E,
	LCD_T4,
	LCD_2A,
	LCD_2B,
	LCD_2C,
	LCD_2D,
	LCD_3F,
	LCD_3G,
	LCD_3E,
	LCD_T35,
	LCD_3A,
	LCD_3B,
	LCD_3C,
	LCD_3D,
	LCD_T26,
	LCD_T25,
	LCD_T3,
	LCD_SS,
	LCD_W4,
	LCD_W3,
	LCD_W2,
	LCD_W1,
	LCD_T30,
	LCD_T27,
	LCD_T24,
	LCD_T29,
	LCD_T18,
	LCD_T17,
	LCD_T16,
	LCD_T19,
	LCD_8F,
	LCD_8G,
	LCD_8E,
	LCD_T20,
	LCD_8A,
	LCD_8B,
	LCD_8C,
	LCD_8D,
	LCD_9F,
	LCD_9G,
	LCD_9E,
	LCD_T21,
	LCD_9A,
	LCD_9B,
	LCD_9C,
	LCD_9D,
	LCD_10F,
	LCD_10G,
	LCD_10E,
	LCD_T28,
	LCD_10A,
	LCD_10B,
	LCD_10C,
	LCD_10D,
	LCD_T6,
	LCD_T7,
	LCD_T9,
	LCD_T5,
	LCD_S1,
	LCD_S2,
	LCD_S3,
	LCD_T22,
	LCD_17F,
	LCD_17G,
	LCD_17E,
	LCD_T23,
	LCD_17A,
	LCD_17B,
	LCD_17C,
	LCD_17D,

NUM_Full=160

}FigList;



typedef struct
{
//  byte WheelGifSpeed10ms;
  byte AnionGifSpeed100ms;
  byte FigFlashSpeed100ms;
//  byte TempRHStay100ms;
}FormDispTypeDef;

//extern SysCtrlParaTypedef * SysCtrlParaPoint;
//extern SensorDataTypedef * SensorDataPoint;
//extern SysStateTypedef* SysStatusPoint;
//extern FormDispTypeDef* FormDispPoint;
	

extern void InitGui(void);
    
#endif 




