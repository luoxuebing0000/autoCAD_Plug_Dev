#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 使用acedCommond函数创建一个圆
void AddCircle1();

// 使用acedCmd函数创建一个圆
void AddCircle2();




// 声明一个用于提示用户选择一个实体，在命令窗口中显示该实体的参数的函数
void EntInfo();



AcDbObjectId PostToModelSpace(AcDbEntity *pEnt);

// 声明一个创建多线段的函数
void AddPolyBasic();

// 加载应用程序时被调用的函数
void initApp()
{
	acedRegCmds->addCommand(_T("Resbuf"), _T("AddCircle1"), _T("AddCircle1"), ACRX_CMD_MODAL, AddCircle1);

	acedRegCmds->addCommand(_T("Resbuf"), _T("AddCircle2"), _T("AddCircle2"), ACRX_CMD_MODAL, AddCircle2);

	acedRegCmds->addCommand(_T("Resbuf"), _T("EntInfo"), _T("EntInfo"), ACRX_CMD_MODAL, EntInfo);

	acedRegCmds->addCommand(_T("Resbuf"), _T("AddPolyBasic"), _T("AddPolyBasic"), ACRX_CMD_MODAL, AddPolyBasic);

}

// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("Resbuf"));
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

// 使用acedCmd函数创建一个圆
void AddCircle2()
{
	struct resbuf *rb; // 结果缓冲区
	int rc = RTNORM; // 返回值

	// 创建缓冲区链表
	ads_point ptCenter = { 30,0,0 };
	ads_real radius = 10;
	rb = acutBuildList(RTSTR, "Circle",  // 命令
					   RTPOINT, ptCenter, // 圆心
					   RTREAL, radius, // 半径
					   RTNONE); // 结束命令

	// 创建圆
	if (rb != NULL)
	{
		rc = acedCmdS(rb);
	}

	// 检验返回值
	if (rc != RTNORM)
	{
		acutPrintf(_T("\n创建圆失败！"));
	}

	acutRelRb(rb);

	// 进行缩放
	acedCommandS(RTSTR, "Zoom", RTSTR, "E", RTNONE);
}





// 定义一个用于提示用户选择一个实体，在命令窗口中显示该实体的参数的函数
void EntInfo()
{
	// 提示用户选择实体
	ads_name entName;
	ads_point pt;

	if (acedEntSel(_T("\n选择实体:"), entName, pt) != RTNORM)
	{
		return;
	}

	struct resbuf *rbEnt;  // 保存实体数据的结果缓冲区
	struct resbuf *rb; // 用于遍历rbEnt的结果缓冲区

	// 从entName获得保存实体数据的结果缓冲区
	rbEnt = acdbEntGet(entName);
	rb = rbEnt;

	while (rb != NULL)
	{
		switch (rb->restype)
		{
		case -1:  // 图元名
			acutPrintf(_T("\n图元名: %x"), rb->resval.rstring);
			break;
		case 0:  // 图元类型
			acutPrintf(_T("\n图元类型: %s"), rb->resval.rstring);
			break;
		case 8:  // 图层
			acutPrintf(_T("\n图层:%s"), rb->resval.rstring);
			break;
		case 10: // 圆心
			acutPrintf(_T("\n圆心:(%.2f, %.2f, %.2f)"),
				rb->resval.rpoint[X],
				rb->resval.rpoint[Y],
				rb->resval.rpoint[Z]);
			break;
		case 40: // 半径
			acutPrintf(_T("\n半径:%.4f"), rb->resval.rreal);
			break;
		case 210: // 圆所在平面的法向矢量
			acutPrintf(_T("\n平面的法向矢量:(%.2f, %.2f, %.2f)"),
				rb->resval.rpoint[X],
				rb->resval.rpoint[Y],
				rb->resval.rpoint[Z]);
			break;
		default:
			break;
		}  // switch
		rb = rb->rbnext; // 切换到下一个节点
	}  // while
	if (rbEnt != NULL)
	{
		acutRelRb(rbEnt);
	}
}

// 定义一个创建多线段的函数
void AddPolyBasic()
{
	int index = 2;  // 当前输入点的次数
	ads_point ptStart; // 起点
	if (acedGetPoint(NULL, _T("\n输入第一点： "), ptStart) != RTNORM)
		return;

	ads_point ptPrevious, ptCurrent; // 当前一个参考点，当前拾取的点
	acdbPointSet(ptStart, ptPrevious);
	AcDbObjectId polyId; // 多线段的ID

	while (acedGetPoint(ptPrevious, _T("\n输入下一点："), ptCurrent) == RTNORM)
	{
		if (index == 2)
		{
			// 创建多段线
			AcDbPolyline *pPoly = new AcDbPolyline(2);
			AcGePoint2d ptGe1, ptGe2;  // 两个节点
			ptGe1[X] = ptPrevious[X];
			ptGe1[Y] = ptPrevious[Y];
			ptGe2[X] = ptCurrent[X];
			ptGe2[Y] = ptCurrent[Y];
			pPoly->addVertexAt(0, ptGe1);
			pPoly->addVertexAt(1, ptGe2);
			// 添加到模型空间
			polyId = PostToModelSpace(pPoly);
		}
		else if (index > 2)
		{
			// 修改多段线，添加最后一个顶点
			AcDbPolyline *pPoly;
			acdbOpenObject(pPoly, polyId, AcDb::kForWrite);
			AcGePoint2d ptGe;  // 增加的节点
			ptGe[X] = ptCurrent[X];
			ptGe[Y] = ptCurrent[Y];
			pPoly->addVertexAt(index - 1, ptGe);
			pPoly->close();
		}
	}
}

AcDbObjectId PostToModelSpace(AcDbEntity *pEnt)
{
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	AcDbObjectId entId;
	pBlockTableRecord->appendAcDbEntity(entId, pEnt);

	pBlockTable->close();
	pBlockTableRecord->close();
	pEnt->close();
	return entId;
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