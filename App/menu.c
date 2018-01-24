/*

********************************************************************************
*文件名     : menu.c
*作用       : 界面设计
*原理       : 例子程序中包含五个窗体Form，每一个窗体可以加载BackText背景文字、
*           : Chart表、Label标签、TextBox文本四类控件，采用Add方法加载之后，
*           : 需要初始化值。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"
#include "app.h"
#include "menu.h"
#include "hmi.h"

void InitMenu(void)
{
  App.Menu.AnionGifSpeed100ms=6;
	App.Menu.FigFlashSpeed100ms=5;
//	App.Menu.TempRHStay100ms=20;
//	App.Menu.WheelGifSpeed10ms=0;
	
	System.Gui.InitDispSensorData((SensorDataTypedef*)&(App.SensorData));
	System.Gui.InitDispSysCtrlPara((SysCtrlParaTypedef*)&(App.SysCtrlPara));
	System.Gui.InitDispSysStatus((SysStateTypedef*)&(App.SysState));
	System.Gui.LoadForm((FormDispTypeDef*)&(App.Menu));

//	FormDispPoint=&(App.Menu);
//	SysStatusPoint =&(App.SysState);
//	SensorDataPoint =&(App.SensorData);
//	SysCtrlParaPoint =&(App.SysCtrlPara);
}

void MenuTask(void)
{
	System.Gui.ParseForm();
}


