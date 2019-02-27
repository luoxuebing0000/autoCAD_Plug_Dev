#include "stdafx.h"
#include "CPubFunction.h"


// 声明初始化函数和卸载函数
void initApp();
void unloadApp();


// 声明一个用于演示选择集的创建和删除的函数
void CreateSSet();
void SelectEnt();

// 声明一个选择文件的函数
void SelectFile();

// 提示用户选择一条多段线，在命令窗口中显示与多段线形成的区域相交的实体的个数
void SelectEntInPoly();

// 创建一个带有通配符的过滤器
void Filter1();

// 创建包含逻辑运算符的过滤器
void Filter2();

//	创建包含关系运算符的过滤器
void Filter3();

// 创建包含关系运算符和通配符的过滤器
void Filter4();

// 创建过滤扩展数据的过滤器
void Filter5();

// 用于进行创建选择集的测试
void Test2();

// 快速获得需要的组码
void EntInfo();

// 加载应用程序时被调用的函数
void initApp()
{
	acedRegCmds->addCommand(_T("SelectionSet"), _T("CreateSSet"), _T("CreateSSet"), ACRX_CMD_MODAL, CreateSSet);

	acedRegCmds->addCommand(_T("SelectionSet"), _T("SelectEnt"), _T("SelectEnt"), ACRX_CMD_MODAL, SelectEnt);

	acedRegCmds->addCommand(_T("SelectionSet"), _T("SelectFile"), _T("SelectFile"), ACRX_CMD_MODAL, SelectFile);

	acedRegCmds->addCommand(_T("SelectionSet"), _T("SelectEntInPoly"), _T("SelectEntInPoly"), ACRX_CMD_MODAL, SelectEntInPoly);

	// 创建一个带有通配符的过滤器的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Filter1"), _T("Filter1"), ACRX_CMD_MODAL, Filter1);

	//	创建包含关系运算符的过滤器的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Filter2"), _T("Filter2"), ACRX_CMD_MODAL, Filter2);

	//	创建包含关系运算符的过滤器的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Filter3"), _T("Filter3"), ACRX_CMD_MODAL, Filter3);

	//	创建包含关系运算符和通配符的过滤器的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Filter4"), _T("Filter4"), ACRX_CMD_MODAL, Filter4);

	// 创建过滤扩展数据的过滤器的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Filter5"), _T("Filter5"), ACRX_CMD_MODAL, Filter5);

	// 用于进行创建选择集的测试的命令
	acedRegCmds->addCommand(_T("SelectionSet"), _T("Test2"), _T("Test2"), ACRX_CMD_MODAL, Test2);

	// 快速获得需要的组码
	acedRegCmds->addCommand(_T("SelectionSet"), _T("EntInfo"), _T("EntInfo"), ACRX_CMD_MODAL, EntInfo);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("SelectionSet"));
}


// 定义一个用于演示选择集的创建和删除的函数
void CreateSSet()
{
	ads_name sset; // 选择集名称
	// 选择图形数据库中所有的实体
	acedSSGet(_T("A"), NULL, NULL, NULL, sset);

	// 进行其他操作

	acedSSFree(sset);
}

void SelectEnt()
{
	ads_point pt1, pt2, pt3, pt4;
	struct resbuf *pointlist; // 结果缓冲区链表
	ads_name ssname;	// 选择集的图原名
	pt1[X] = pt1[Y] = pt1[Z] = 0.0;
	pt2[X] = pt2[Y] = 5.0;
	pt2[Z] = 0.0;

	// 如果已经选择到了实体，就获得当前的PICKFIRST选择集
	// 否则提示用户选择实体
	acedSSGet(NULL, NULL, NULL, NULL, ssname);

	// 如果存在，就获得当前的PickFirst选择集
	acedSSGet(_T("I"), NULL, NULL, NULL, ssname);

	// 选择最近创建的选择集
	acedSSGet(_T("P"), NULL, NULL, NULL, ssname);

	// 选择通过点(5,5)的所有实体
	acedSSGet(NULL, pt2, NULL, NULL, ssname);
	// 选择位于角点(0,0)和(5,5)组成的窗口内所有的实体
	acedSSGet(_T("W"), pt1, pt2, NULL, ssname);
	
	// 选择被指定的多边形包围的所有实体
	pt3[X] = 10.0; pt3[Y] = 5.0; pt3[Z] = 0.0;
	pt4[X] = 5.0; pt4[Y] = pt4[Z] = 0.0;
	pointlist = acutBuildList(RTPOINT, pt1, RTPOINT, pt2,RTPOINT, pt3, RTPOINT, pt4, 0);
	acedSSGet(_T("WP"), pointlist, NULL, NULL, ssname);
	// 选择与角点(0,0)和(5,5)组成的区域相交的所有实体
	acedSSGet(_T("C"), pt1, pt2, NULL, ssname);
	// 选择与指定多边形区域相交的所有实体
	acedSSGet(_T("CP"), pointlist, NULL, NULL, ssname);
	acutRelRb(pointlist);
	// 选择与选择栏相交的所有对象
	pt4[Y] = 15.0; pt4[Z] = 0.0;
	pointlist = acutBuildList(RTPOINT, pt1, RTPOINT, pt2,
		RTPOINT, pt3, RTPOINT, pt4, 0);
	acedSSGet(_T("F"), pointlist, NULL, NULL, ssname);
	acutRelRb(pointlist);
	acedSSFree(ssname);
}

// 声明一个选择文件的函数
void SelectFile()
{
	const wchar_t* title = _T("选择图形文件");
	const wchar_t* path = _T("C:\\");
	struct resbuf *fileName;
	fileName = acutNewRb(RTSTR);

	if (acedGetFileD(title, path, _T("dwg;dxf"), 0, fileName) == RTNORM)
	{
		acedAlert(fileName->resval.rstring);
	}

	acutRelRb(fileName);
}

// 提示用户选择一条多段线，在命令窗口中显示与多段线形成的区域相交的实体的个数
void SelectEntInPoly()
{
	// 提示用户选择多段线
	ads_name entName;
	ads_point pt;
	if (acedEntSel(_T("\n选择多线段："), entName, pt) != RTNORM)
	{
		return;
	}

	AcDbObjectId entId;
	acdbGetObjectId(entId, entName);

	// 判断选择的实体是否是多段线
	AcDbEntity *pEnt;
	acdbOpenObject(pEnt, entId, AcDb::kForWrite);

	if (pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pPoly = AcDbPolyline::cast(pEnt);
		AcDbObjectIdArray ObjectIdArray; // 选择到的实体ID集合
		CPubFunction::SelectEntInPoly(pPoly, ObjectIdArray, "CP", 1);
		acutPrintf(_T("\n选择到%d个实体."), ObjectIdArray.length());
	}
	pEnt->close();

}

// 创建一个带有通配符的过滤器
void Filter1()
{
	struct resbuf *rb; // 结果缓冲区链表 
	ads_name ssname;
	rb = acutBuildList(RTDXF0, "TEXT", // 实体类型
		8, "0,图层1",  // 图层
		1, "*cadhelp*",  // 包含的字符串
		RTNONE);

	// 选择符合要求的文字
	acedSSGet(_T("X"), NULL, NULL, rb, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数:%d"), length);
	acutRelRb(rb);
	acedSSFree(ssname);
}

// 创建一个带有通配符的过滤器
void Filter2()
{
	struct resbuf *rb; // 结果缓冲区链表 
	ads_name ssname;
	rb = acutBuildList(-4, "<OR",  // 逻辑运算符开始
		RTDXF0, "TEXT",  // 一个条件
		RTDXF0, "MTEXT", // 另一个条件
		-4, "OR>", // 逻辑运算符结束
		RTNONE);

	// 选择符合要求的实体
	acedSSGet(_T("X"), NULL, NULL, rb, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数：%d"), length);
	acutRelRb(rb);
	acedSSFree(ssname);

}

//	创建包含关系运算符的过滤器
void Filter3()
{
	struct resbuf *rb;
	ads_name ssname;
	rb = acutBuildList(RTDXF0, "CIRCLE", // 实体类型
		-4, ">=",	// 关系运算符
		40, 30,		// 半径
		RTNONE);

	acedSSGet(_T("X"), NULL, NULL, rb, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数：%d"), length);
	acutRelRb(rb);
	acedSSFree(ssname);
}

// 创建包含关系运算符和通配符的过滤器
void Filter4()
{
	ads_name ssname;
	struct resbuf *rb;
	AcGePoint2d pt1, pt2;
	pt1[X] = 0.0;
	pt1[Y] = 0.0;
	pt2[X] = 100.0;
	pt2[Y] = 100.0;
	rb = acutBuildList(RTDXF0, "CIRCLE",
		-4, ">,>,*",
		10, pt1,
		-4, "<,<,*",
		10, pt2,
		RTNONE);
	acedSSGet(_T("X"), NULL, NULL, rb, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数：%d"), length);
	acutRelRb(rb);
	acedSSFree(ssname);
}

// 创建过滤扩展数据的过滤器
void Filter5()
{
	struct resbuf *rb;
	ads_name ssname;
	rb = acutBuildList(1001, "XData",  // 扩展数据的应用程序名 
						RTNONE);

	acedSSGet(_T("X"), NULL, NULL, rb, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数：%d"), length);
	acutRelRb(rb);
	acedSSFree(ssname);
}

// 用于进行创建选择集的测试
void Test2()
{
	ads_name ssname;
	ads_point pt1, pt2;
	pt1[X] = pt1[Y] = pt1[Z] = 0;
	pt2[X] = pt2[Y] = 100;
	pt2[Z] = 0;

	// 选择图形中与pt1和pt2组成的窗口相交的所有对象
	acedSSGet(_T("C"), pt1, pt2, NULL, ssname);
	long length;
	acedSSLength(ssname, &length);
	acutPrintf(_T("\n实体数:%d"), length);
	acedSSFree(ssname);
}

// 快速获得需要的组码
void EntInfo()
{
	// acDocManager 是一个用#define 语句定义的宏，等效于全局函数 acDocManagerPtr，此函数返回指向文档管理器的指针
	// curDocument 函数返回当前活动的图形文档，sendStringToExecute 函数将一个字符串发送到指定文档的命令行，在字符串的末尾加上一个空格就可以执行该字符串

	acDocManager->sendStringToExecute(acDocManager->curDocument(),_T("(entget(car(entsel))) "));  // 字符串的末尾包含一个空字符
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