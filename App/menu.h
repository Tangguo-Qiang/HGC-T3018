/*
********************************************************************************
*文件名     : menu.h
*作用       : 菜单设计
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/


#ifndef __MENU_H
#define __MENU_H

typedef enum{
  FocusOn_START= (byte)0x01,	
  FocusOn_2ND= (byte)0x02,	
  FocusOn_3RD= (byte)0x04,	
  FocusOn_4TH= (byte)0x08,	
  FocusOn_NULL= (byte)0x10	
}FocusDataType;


extern void InitMenu(void);

extern void MenuTask (void);

#endif /*__MMI_H*/


