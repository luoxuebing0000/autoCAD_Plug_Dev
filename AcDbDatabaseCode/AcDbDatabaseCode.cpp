#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 用于创建一个新的图形文件
void CreateDwg();

// 用于获得当前运行的 AutoCAD 程序的acad.exe 的位置
bool GetAcadPath(CString &acadPath);

// 加载应用程序时被调用的函数
void initApp()
{
	acedRegCmds->addCommand(_T("AcDbDatabaseCode"), _T("CreateDwg"), _T("CreateDwg"), ACRX_CMD_MODAL, CreateDwg);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("AcDbDatabaseCode"));
}

// 用于创建一个新的图形文件
void CreateDwg()
{
	// 创建新的图形数据库，分配内存空间
	AcDbDatabase *pDb = new AcDbDatabase();

	AcDbBlockTable *pBlkTbl;
	pDb->getSymbolTable(pBlkTbl, AcDb::kForRead);

	AcDbBlockTableRecord *pBlkTblRcd;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForWrite);
	pBlkTbl->close();

	// 创建两个圆
	AcDbCircle *pCir1 = new AcDbCircle(AcGePoint3d(1,1,1),AcGeVector3d(0,0,1),1.0);
	AcDbCircle *pCir2 = new AcDbCircle(AcGePoint3d(4, 4, 4), AcGeVector3d(0, 0, 1), 2.0);
	pBlkTblRcd->appendAcDbEntity(pCir1);
	pCir1->close();
	pBlkTblRcd->appendAcDbEntity(pCir2);
	pCir2->close();
	pBlkTblRcd->close();

	CString acadPath;
	GetAcadPath(acadPath);	// 获得acad.exe的位置
	// 去掉路径最后的"acad.exe"字符串，得到autoCAD安装路径
	acadPath = acadPath.Left(acadPath.GetLength() - 8);
	CString filePath = acadPath + "test.dwg";
	// 使用saveAs成员函数时，必须指定包含dwg扩展名的文件名称
	pDb->saveAs(filePath.GetBuffer(0));
	filePath.ReleaseBuffer();
	delete pDb; // pDb不是数据库的常驻对象，必须手工销毁

}

// 用于获得当前运行的 AutoCAD 程序的acad.exe 的位置
bool GetAcadPath(CString &acadPath)
{
	DWORD dwRet = ::GetModuleFileName(acedGetAcadWinApp()->m_hInstance, acadPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	acadPath.ReleaseBuffer();
	if (dwRet == 0)
		return false;
	else
		return true;
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