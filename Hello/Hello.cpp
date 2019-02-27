#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();
// 声明命令的执行函数
void HelloWorld();
// 加载应用程序时被调用的函数
void initApp()
{
	//使用AutoCAD命令机制注册一个新命令
	acedRegCmds->addCommand(_T("Hello1"),
		_T("Hello"),  //输入这两个命令名称均可以在
		_T("Hello"),  //AutoCAD中运行该程序
		ACRX_CMD_MODAL,
		HelloWorld);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	//删除命令组
	acedRegCmds->removeGroup(_T("Hello1"));
}
// 实现Hello命令的函数
void HelloWorld()
{
	acutPrintf(_T("Hello, World!"));
}

// 使用acedCommond函数创建一个圆
void AddCircle1()
{
	// 声明ADS变量
	ads_point ptCenter = { 0, 0, 0 };  // 圆心
	ads_real radius = 10; // 半径

	// 调用acedCommand函数创建圆
	acedCommandS(RTSTR, "Circle",  // 命令
		RTPOINT, ptCenter, // 圆心
		RTREAL, radius, // 半径
		RTNONE); // 结束命令
}

// 入口点函数
extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	default:
		break;
	}
	return AcRx::kRetOK;
}