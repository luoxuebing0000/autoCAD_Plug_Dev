// ChangeColor.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"

static int count = 0;
// 颜色
int colorIndex = 0;
// 选择的dwg文件的目录
CString filePath;
// 日志路径
CString logpath;
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

void MyColor(CString &filePath, int colorIndex, void(*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b));

// 获取文件列表
bool BrowseCurrentAllFile(CString &filePath, CStringList &filePathList,const CString &strFilter);
// 修正文件路径格式
bool CorrectFilePath(CString &filePath);
// 改变实体的颜色命令
void ChangeColor();
// 颜色是否已经改变
void isChangedColor();

void ProcessColor(AcDbBlockTableRecordIterator *&pBlkTblRcdItr, void (*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b), Adesk::UInt16 color, bool &b);
// 修改颜色
void changeColor(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b);

// 用于判断是否已经修改成功了
void isChanged(Adesk::UInt16 color, AcDbEntity *&pEnt,bool &b);

// 修改出错时用于提示是否继续修改
int Iscontinue();
// 是否删除.bak文件
bool IsDelBak();
// 删除bak文件
void DelBak();

// 获取要修改的dwg文件的目录
bool getFilePath(CString &filePath);
// 获取要修改成的颜色值
Adesk::Boolean getColorIndex(int &colorIndex);

// 遍历CStringList链表里的内容
void PrintStringList(const CStringList &strList);
// 将CstringList打印到文件里
void PrintStringList(const CStringList &strList, std::ofstream &of);

// 处理颜色
void ProcessTableColor(AcDbDatabase *&pDb,int colorIndex, void(*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b),bool &b);

// 打印处理结果
void showResultInfo(const CStringList &successList, const CStringList &failedList, const CStringList &filePathList);
// 记录处理信息
void RecordResultInfo(const CStringList &successList, const CStringList &failedList, const CStringList &filePathList);

// 加载应用程序时被调用的函数
void initApp()
{
	acedRegCmds->addCommand(_T("Color"), _T("ChangeColor"), _T("ChangeColor"), ACRX_CMD_MODAL, ChangeColor);
	acedRegCmds->addCommand(_T("Color"), _T("isChangedColor"), _T("isChangedColor"), ACRX_CMD_MODAL, isChangedColor);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("Color"));
}
void MyColor(CString &filePath, int colorIndex, void(*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b))
{
	CStringList filePathList, successList, failedList;
	
	// 获取当前目录下所有的dwg文件的全路径
	// 设置过滤器
	CString strFilter = _T("/*.dwg");
	BrowseCurrentAllFile(filePath, filePathList, strFilter);

	// 判断输入目录下是否有dwg文件
	if (filePathList.IsEmpty())
	{
		acutPrintf(_T("\n您选择的目录无dwg文件"));
		return;
	}
	int flage = 0;
	// 修改指定目录下获取的每个dwg文件实体的颜色
	while (!filePathList.IsEmpty())
	{
		// 创建新的图形数据库，分配内存空间
		AcDbDatabase *pDb = new AcDbDatabase(Adesk::kFalse);
		Acad::ErrorStatus esRead = pDb->readDwgFile(filePathList.GetHead(), _SH_DENYRW);
		if (esRead != Acad::eOk && flage == 0)
		{
			acutPrintf(_T("\n更改颜色失败图层路径为：%s"), filePathList.GetHead());
			int ret = Iscontinue();
			if (ret == 2)	// 结束修改
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				filePath.ReleaseBuffer();
				delete pDb;
				break;
			}
			else if (ret == 0)
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
			else if (ret == 3)
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				flage = 1;
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
			else  // 继续修改
			{
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
		}
		else if(esRead != Acad::eOk && flage != 0)
		{
			failedList.AddHead(filePathList.GetHead());
			filePathList.RemoveHead();
			filePath.ReleaseBuffer();
			delete pDb;
			continue;
		}
		bool b = false;
		ProcessTableColor(pDb, colorIndex, Process, b);
		if (b == false && flage == 0)
		{
			int retVal = Iscontinue();
			if (retVal == 0)
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
			else if (retVal == 2)
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				filePath.ReleaseBuffer();
				delete pDb;
				break;
			}
			else if (retVal == 3)
			{
				failedList.AddHead(filePathList.GetHead());
				filePathList.RemoveHead();
				flage = 1;
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
			else
			{
				filePath.ReleaseBuffer();
				delete pDb;
				continue;
			}
		}
		else if(b == false && flage != 0)
		{
			failedList.AddHead(filePathList.GetHead());
			filePathList.RemoveHead();
			filePath.ReleaseBuffer();
			delete pDb;
			continue;
		}
		// 判断是否保存成功
		Acad::ErrorStatus esSave = pDb->saveAs(filePathList.GetHead());
		if (esSave == Acad::eOk)
		{
			successList.AddTail(filePathList.GetHead());
			filePathList.RemoveHead();
		}
		else
		{
			failedList.AddTail(filePathList.GetHead());
			filePathList.RemoveHead();
		}
		filePath.ReleaseBuffer();
		delete pDb;
	}
	// 打印结果信息
	//showResultInfo(successList, failedList, filePathList);
	RecordResultInfo(successList, failedList, filePathList);
	// 操作完成提示
	acutPrintf(_T("\n操作完成，日志信息请查看该路径下的文件：%s"), logpath);
	if (IsDelBak())
		DelBak();
}
// 改变实体的颜色
void ChangeColor()
{
	// 获取要修改的dwg文件的目录
	bool bGetFilePath = getFilePath(filePath);
	if (bGetFilePath == false)
	{
		acutPrintf(_T("\n获取dwg目录失败！！！"));
		return;
	}
	// 获取要设置的颜色
	Adesk::Boolean b = getColorIndex(colorIndex);
	if (b == Adesk::kFalse)
	{
		acutPrintf(_T("选择颜色失败，终止修改图层实体颜色！！！"));
		return;
	}
	MyColor(filePath,colorIndex,changeColor);
}

void isChangedColor()
{
	if (filePath.IsEmpty())
	{
		acutPrintf(_T("\n路径无效，请重新选择"));
		// 获取要修改的dwg文件的目录
		bool bGetFilePath = getFilePath(filePath);
		if (bGetFilePath == false)
		{
			acutPrintf(_T("\n获取dwg目录失败！！！"));
			return;
		}
	}
	MyColor(filePath,colorIndex, isChanged);
}

bool CorrectFilePath(CString &filePath)
{
	int index = 0;
	while (index < filePath.GetLength())
	{
		index = filePath.Find(_T("\\"));
		if (index == -1)
			break;
		else
			filePath.Replace(_T("\\"), _T("/"));
	}
	return true;
}

bool BrowseCurrentAllFile(CString &filePath, CStringList &filePathList,const CString &strFilter)
{
	if (filePath == _T(""))
		return false;
	else
		CorrectFilePath(filePath);
	CFileFind finder;
	CString strPath;
	BOOL IsFind = finder.FindFile(filePath + strFilter);
	while (IsFind)
	{
		IsFind = finder.FindNextFileW();
		strPath = finder.GetFilePath();
		if (finder.IsDirectory() && !finder.IsDots())
		{
			BrowseCurrentAllFile(strPath, filePathList, strFilter);
		}
		else if(!finder.IsDirectory() && !finder.IsDots())
		{
			CString str = strPath.Right(4);
			if(str.Compare(strFilter.Right(4)) == 0)
				filePathList.AddTail(strPath);
		}
		else
		{
			continue;
		}
	}
	return true;
}

int Iscontinue()
{
	wchar_t C[100];
	if (acedGetString(Adesk::kFalse, _T("\n重试(R)/继续(C)/终止(B)/忽略所有提示(A): "), C) != RTNORM)
	{
		return -1;
	}
	if (wcscmp(C, _T("C")) == 0 || wcscmp(C, _T("c")) == 0)
	{
		return 0;
	}
	else if (wcscmp(C, _T("R")) == 0 || wcscmp(C, _T("r")) == 0)
	{
		return 1;
	}
	else if (wcscmp(C, _T("A")) == 0 || wcscmp(C, _T("a")) == 0)
	{
		return 3;
	}
	else
	{
		return 2;
	}
}

// 是否删除.bak文件
bool IsDelBak()
{
	wchar_t IsDelInfo[20];
	if (acedGetString(Adesk::kFalse, _T("\n修改完成，是否删除.bak文件？ 是(Y)/否(N)"), IsDelInfo) != RTNORM)
	{
		acutPrintf(_T("\n输入失败！！！"));
		return false;
	}
	if (wcscmp(IsDelInfo, _T("Y")) == 0 || wcscmp(IsDelInfo, _T("y")) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 删除bak文件
void DelBak()
{
	CStringList bakPathInfo;
	CString filter = _T("/*.bak");
	BrowseCurrentAllFile(filePath, bakPathInfo, filter);
	POSITION rPos = bakPathInfo.GetHeadPosition();
	while (rPos != NULL)
	{
		DeleteFile(bakPathInfo.GetAt(rPos));
		bakPathInfo.GetNext(rPos);
	}
}

bool getFilePath(CString &filePath)
{
	

	wchar_t szPath[MAX_PATH];     //存放选择的目录路径

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择指定的dwg文件目录");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//弹出选择目录对话框  
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp && SHGetPathFromIDList(lp, szPath))
	{
		filePath = CString(szPath);
		logpath = filePath + "operation.log";
		return true;
	}
	else
		return false;
}

Adesk::Boolean getColorIndex(int &colorIndex)
{
	int nCurColor = 0;
	return acedSetColorDialog(colorIndex, Adesk::kFalse, nCurColor);
}

void PrintStringList(const CStringList &strList)
{
	if (strList.IsEmpty())
		return;
	POSITION rPos;
	rPos = strList.GetHeadPosition();
	while (rPos != NULL)
	{
		acutPrintf(_T("\n\t%s"), strList.GetNext(rPos));
	}
}

void PrintStringList(const CStringList &strList, std::ofstream &of)
{
	if (strList.IsEmpty())
		return;
	POSITION rPos;
	rPos = strList.GetHeadPosition();
	while (rPos != NULL)
	{
		{
			std::setlocale(LC_ALL, ".936");
			size_t len = strList.GetAt(rPos).GetLength() + 1;
			char *p = new char[len];
			size_t n = wcstombs(p, strList.GetAt(rPos).GetString(), len);
			std::string str(p, n);
			delete[]p;
			of << str << std::endl;
			setlocale(LC_ALL, "C");
			strList.GetNext(rPos);
		}
	}
}


void showResultInfo(const CStringList &successList, const CStringList &failedList, const CStringList &filePathList)
{
	if (!successList.IsEmpty())
	{
		acutPrintf(_T("\n修改成功的dwg文件的全路径"));
		PrintStringList(successList);
	}
	if (!failedList.IsEmpty())
	{
		acutPrintf(_T("\n修改失败的dwg文件的全路径"));
		PrintStringList(failedList);
	}
	if (!filePathList.IsEmpty())
	{
		acutPrintf(_T("\n未进行修改的dwg文件的全路径"));
		PrintStringList(filePathList);
	}
}

// 记录处理信息
void RecordResultInfo(const CStringList &successList, const CStringList &failedList, const CStringList &filePathList)
{
	std::ofstream of;
	of.open(logpath, std::ios::out);
	of << "第 " << ++count << " 次执行命令结果：" << std::endl;
	if (!successList.IsEmpty())
	{
		of << "修改成功的dwg文件的总数量为：" << successList.GetCount() << std::endl;
		PrintStringList(successList,of);
	}
	if (!failedList.IsEmpty())
	{
		of << "\n修改失败的dwg文件的总数量为：" << failedList.GetCount() << std::endl;
		PrintStringList(failedList,of);
	}
	if (!filePathList.IsEmpty())
	{
		of << "\n未进行修改的dwg文件的总数量为：" << filePathList.GetCount() <<  std::endl;
		PrintStringList(filePathList,of);
	}
	of.close();
}
void ProcessTableColor(AcDbDatabase *&pDb,int colorIndex, void(*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b), bool &b)
{
	AcDbBlockTable *pBlkTbl;
	Acad::ErrorStatus es2 = pDb->getSymbolTable(pBlkTbl, AcDb::kForRead);
	AcDbBlockTableRecord *pBlkTblRcd;
	Acad::ErrorStatus es3 = pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,
		AcDb::kForWrite);
	pBlkTbl->close();

	AcDbBlockTableRecordIterator *pBlkTblRcdItr;
	Acad::ErrorStatus es4 = pBlkTblRcd->newIterator(pBlkTblRcdItr);
	
	// 处理颜色
	ProcessColor(pBlkTblRcdItr, Process, colorIndex, b);
	pBlkTblRcd->close();
	delete pBlkTblRcdItr;
}

void ProcessColor(AcDbBlockTableRecordIterator *&pBlkTblRcdItr, void(*Process)(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b), Adesk::UInt16 color, bool &b)
{
	AcDbEntity *pEnt;
	for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
	{
		pBlkTblRcdItr->getEntity(pEnt, AcDb::kForWrite);
		Process(color,pEnt,b);
		pEnt->close();
		if (b == false)
			return;
	}
	b = true;
}

// 修改颜色
void  changeColor(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b)
{
	Acad::ErrorStatus es = pEnt->setColorIndex(color);
	if (es == Acad::eOk)
		b = true;
	else
		b = false;
}

// 判断颜色是否已经修改
void isChanged(Adesk::UInt16 color, AcDbEntity *&pEnt, bool &b)
{
	if (pEnt->colorIndex() == color)
		b = true;
	else
		b = false;
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