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
	LCD_T12=0,
	LCD_1E,
	LCD_1G,
	LCD_1F,
	LCD_1D,
	LCD_1C,
	LCD_1B,
	LCD_1A,
	LCD_T13,
	LCD_2E,
	LCD_2G,
	LCD_2F,
	LCD_2D,
	LCD_2C,
	LCD_2B,
	LCD_2A,
	LCD_T14,
	LCD_3E,
	LCD_3G,
	LCD_3F,
	LCD_3D,
	LCD_3C,
	LCD_3B,
	LCD_3A,
	LCD_T15,
	LCD_4E,
	LCD_4G,
	LCD_4F,
	LCD_4D,
	LCD_4C,
	LCD_4B,
	LCD_4A,
	LCD_T4,
	LCD_T3,
	LCD_T2,
	LCD_T1,
	LCD_T8,
	LCD_5E,
	LCD_5G,
	LCD_5F,
	LCD_5D,
	LCD_5C,
	LCD_5B,
	LCD_5A,
	LCD_T7,
	LCD_6E,
	LCD_6G,
	LCD_6F,
	LCD_6D,
	LCD_6C,
	LCD_6B,
	LCD_6A,
	LCD_T5,
	LCD_7E,
	LCD_7G,
	LCD_7F,
	LCD_7D,
	LCD_7C,
	LCD_7B,
	LCD_7A,
	LCD_T6,
	LCD_8E,
	LCD_8G,
	LCD_8F,
	LCD_8D,
	LCD_8C,
	LCD_8B,
	LCD_8A,
	LCD_T17,
	LCD_T11,
	LCD_T10,
	LCD_T9,
	LCD_T18,
	LCD_T19,
	LCD_T20,
	LCD_T21,
	LCD_NULL,
	LCD_T16,
	LCD_T22,
	LCD_T23,

NUM_Full

}FigList;

typedef enum{
	FigObj_Null=(uint8_t)0x00,
	FigObj_CircleMode=(uint8_t)0x01,
	FigObj_FlowVol,
	FigObj_Timer, 
	FigObj_AuxiHeater,
	FigObj_Lock, 
	FigObj_Fault, 
	FigObj_Filter,
	FigObj_WIFI,
	FigObj_RF,
	FigObj_CO2,
	FigObj_PM25,
	FigObj_TempIn,
	FigObj_RHIn,
	
	FigObj_FaultCode,
	FigObj_CtrlVersionCode,
	FigObj_PowerVersionCode
}FigObjDefType;

typedef enum 
{
   NormalDisp=(byte)0,
   InvisibleDisp=(byte)1,
	 FlashDisp=(byte)2,
	 OrderDisp=(byte)3
}DispType;

typedef enum 
{
    GuiDataTypeUnSignedNDec,
    GuiDataTypeSigned10sDec,
    GuiDataTypeUnSigned10sDec,
    GuiDataTypeUnSigned100sDec,
    GuiDataTypeUnSigned1000sDec,
//    GuiDataTypeUintHex,
//    GuiDataTypeFigure
}DataType;

typedef struct DispObjSelf
{
  Bool UpdataFlag;  
	DispType Disp;
//	byte DispTime10ms;
	FigObjDefType ObjNum;
//	DataType Type;
	void * DataPointer;
  struct DispObjSelf * NextObjPointer;
}DispObjType;

typedef struct
{
	byte LoadFresh;
  byte ScreenClean;
  DispObjType * DispObjPointer;
	void (*RefreshDisp)(void);
  void (*KeyProcess)(KeyActEnum);
}FormTypeDef;
//extern SysCtrlParaTypedef * SysCtrlParaPoint;
//extern SensorDataTypedef * SensorDataPoint;
//extern SysStateTypedef* SysStatusPoint;
//extern FormDispTypeDef* FormDispPoint;
	

extern void InitGui(void);
    
#endif 




