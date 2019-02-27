#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 用于向实体追加指定的扩展数据
void AddXData();
void ViewXData();

// 加载应用程序时被调用的函数
void initApp()
{
	// 用于向实体追加指定的扩展数据
	acedRegCmds->addCommand(_T("XData"), _T("AddXData"), _T("AddXData"), ACRX_CMD_MODAL, AddXData);

	acedRegCmds->addCommand(_T("XData"), _T("ViewXData"), _T("ViewXData"), ACRX_CMD_MODAL, ViewXData);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("XData"));
}


// 用于向实体追加指定的扩展数据
void AddXData()
{
	// 提示用户选择所要添加扩展数据的图形对象
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n选择所要添加扩展数据的实体："), en, pt) != RTNORM)
		return;

	AcDbObjectId entId;
	Acad::ErrorStatus es = acdbGetObjectId(entId, en);

	// 扩展数据的内容
	struct resbuf *pRb;
	wchar_t appName[] = { _T("XData") };
	wchar_t typeName[] = { _T("道路中心线") };

	// 注册应用程序名
	acdbRegApp(_T("XData"));

	// 创建结果缓冲区链表
	pRb = acutBuildList(AcDb::kDxfRegAppName, appName, // 应用程序名
		AcDb::kDxfXdAsciiString, typeName,	// 字符串
		AcDb::kDxfXdInteger32, 2,	// 整数
		AcDb::kDxfXdReal, 3.14,		// 实数
		AcDb::kDxfXdWorldXCoord, pt,	// 点坐标值
		RTNONE);

	// 为实体添加扩展数据
	AcDbEntity *pEnt;
	acdbOpenAcDbEntity(pEnt, entId, AcDb::kForWrite);
	struct resbuf *pTemp;
	pTemp = pEnt->xData(_T("XData"));
	// 如果已经包含扩展数据，就不再添加新的扩展数据
	if (pTemp != NULL)
	{
		acutRelRb(pTemp);
		acutPrintf(_T("\n所选实体已经包含扩展数据了！"));
	}
	else
	{
		pEnt->setXData(pRb);
	}
	pEnt->close();
	acutRelRb(pRb);

}

void ViewXData()
{
	// 提示用户选择所要查看扩展数据的类型对象
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n选择所要查看扩展数据的实体："), en, pt) != RTNORM)
		return;

	AcDbObjectId entId;
	Acad::ErrorStatus es = acdbGetObjectId(entId, en);

	// 打开图形对象，查看是否包含扩展数据
	AcDbEntity *pEnt;
	acdbOpenAcDbEntity(pEnt,entId, AcDb::kForRead);
	struct resbuf *pRb;
	pRb = pEnt->xData(_T("XData"));
	pEnt->close();

	if (pRb != NULL)
	{
		// 在命令行显示所有的扩展数据
		struct resbuf *pTemp;
		pTemp = pRb;

		// 首先要跳过应用程序的名称这一项
		pTemp->rbnext;
		acutPrintf(_T("\n字符串类型的扩展数据是：%s"), pTemp->resval.rstring);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n整数类型的扩展数据是：%d"), pTemp->resval.rint);

		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n实数类型的扩展数据是：%.2f"),pTemp->resval.rreal);

		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n点坐标类型的扩展数据是：(%.2f, %.2f, %.2f)"),pTemp->resval.rpoint[X], pTemp->resval.rpoint[Y],pTemp->resval.rpoint[Z]);
		acutRelRb(pRb);
	}
	else
	{
		acutPrintf(_T("\n所选择的实体不包含任何的扩展数据！"));
	}
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