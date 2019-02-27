#include "stdafx.h"


// 声明一个获取当前图层的函数
AcDbViewTableRecord GetCurrentView();

class CommonTools
{
public:
	static void splitStr(CString &str, const wchar_t *flag, CStringArray &strAray);
};

void CommonTools::splitStr(CString &str, const wchar_t *flag, CStringArray &strArray)
{
	int i = 0, j = 0;
	while (i < str.GetLength())
	{
		i = str.Find(flag, i);
		strArray.Add(str.Mid(j, i - 1));
		i++;
		j = i;
	}
}

// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 声明一个在图形中创建一个新图层的函数
void NewLayer();

// 声明一个修改指定图层颜色的函数
void LayerColor();

// 声明一个从图形中删除指定的图层函数
void DelLayer();

// 声明一个用于将当前图形中存在的所有图层及其特性导出到一个文本文件中的函数
void ExportLayer();

// 声明一个能够按照文本文件中的图层列表在当前图形中创建图层，并且符合图层列表中的各项特性的函数
void ImportLayer(); // 未实现完成，不可用

// 声明一个创建新的字体样式的函数
void AddStyle();

// 声明一个创建新的标注样式的函数
void AddDimStyle();

// 声明一个用于更新视图的比例缩放的函数
void ChangeView();

// 声明一个用于模拟窗口缩放的功能的函数
void ZoomWindow();

// 声明一个用于实现在各种视图（包括透视模式）中的范围缩放的函数
void ZoomExtents();

// 加载应用程序时被调用的函数
void initApp()
{
	acedRegCmds->addCommand(_T("OperatorLayer"), _T("NewLayer"), _T("NewLayer"), ACRX_CMD_MODAL, NewLayer);

	// 添加一个修改指定图层颜色的命令
	acedRegCmds->addCommand(_T("OperatorLayer"), _T("LayerColor"), _T("LayerColor"), ACRX_CMD_MODAL,LayerColor);

	// 添加一个从图形中删除指定的图层命令
	acedRegCmds->addCommand(_T("OperatorLayer"), _T("DelLayer"), _T("DelLayer"), ACRX_CMD_MODAL, DelLayer);

	// 添加一个用于将当前图形中存在的所有图层及其特性导出到一个文本文件中的命令
	acedRegCmds->addCommand(_T("OperatorLayer"), _T("ExportLayer"), _T("ExportLayer"), ACRX_CMD_MODAL, ExportLayer);

	// 添加一个能够按照文本文件中的图层列表在当前图形中创建图层，并且符合图层列表中的各项特性的命令
	//acedRegCmds->addCommand(_T("OperatorLayer"), _T("ImportLayer"), _T("ImportLayer"), ACRX_CMD_MODAL, ImportLayer);

	// 添加一个创建新的字体样式的命令
	acedRegCmds->addCommand(_T("OperatorTextStyle"), _T("AddStyle"), _T("AddStyle"), ACRX_CMD_MODAL, AddStyle);

	// 添加一个创建新的标注样式的命令
	acedRegCmds->addCommand(_T("OperatorTextStyle"), _T("AddDimStyle"), _T("AddDimStyle"), ACRX_CMD_MODAL, AddDimStyle);

	// 添加一个用于更新视图的比例缩放的命令
	acedRegCmds->addCommand(_T("OperatorView"), _T("ChangeView"), _T("ChangeView"), ACRX_CMD_MODAL, ChangeView);

	// 添加一个用于模拟窗口缩放的功能的命令
	acedRegCmds->addCommand(_T("OperatorView"), _T("ZoomWindow"), _T("ZoomWindow"), ACRX_CMD_MODAL, ZoomWindow);

	// 添加一个用于实现在各种视图（包括透视模式）中的范围缩放的命令
	acedRegCmds->addCommand(_T("OperatorView"), _T("ZoomExtents"), _T("ZoomExtents"), ACRX_CMD_MODAL, ZoomExtents);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("OperatorLayer"));
	acedRegCmds->removeGroup(_T("OperatorTextStyle"));
	acedRegCmds->removeGroup(_T("OperatorView"));
}

// 定义一个在图形中创建一个新图层的函数
void NewLayer()
{
	// 提示用户输入新建图层的名称
	wchar_t layerName[100];
	if (acedGetString(Adesk::kFalse, _T("\n请输入新图层的名称："), layerName) != RTNORM)
	{
		return;
	}

	// 获得当前图形的层表
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	
	// 是否已经包含指定的层表记录
	if (pLayerTbl->has(layerName))
	{
		pLayerTbl->close();
		return;
	}

	// 创建新的层表记录
	AcDbLayerTableRecord *pLayerRcd = new AcDbLayerTableRecord();
	pLayerRcd->setName(layerName);

	// 将新建的层表记录添加到层表中
	AcDbObjectId layerTblRcdId;
	pLayerTbl->add(layerTblRcdId, pLayerRcd);

	acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);

	pLayerRcd->close();
	pLayerTbl->close();
}

// 定义一个修改指定图层颜色的函数
void LayerColor()
{
	// 提示用户输入要修改的图层名称
	wchar_t layerName[100];
	if (acedGetString(Adesk::kFalse, _T("\n输入图层的名称："), layerName) != RTNORM)
	{
		return;
	}

	// 获得当前图形的层表
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

	// 获取指定层表记录的指针
	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTbl->getAt(layerName, pLayerTblRcd, AcDb::kForWrite);
	
	// 弹出 “颜色” 对话框
	AcCmColor oldColor = pLayerTblRcd->color();
	int nCurColor = oldColor.colorIndex(); // 图层修改前的颜色
	int nNewColor = oldColor.colorIndex();
	if (acedSetColorDialog(nNewColor, Adesk::kFalse, nCurColor))
	{
		AcCmColor color;
		color.setColorIndex(nNewColor);
		pLayerTblRcd->setColor(color);
	}

	pLayerTblRcd->close();
	pLayerTbl->close();
}

// 定义一个从图形中删除指定的图层函数
void DelLayer()
{
	// 提示用户输入图层的名称
	wchar_t layerName[100];
	if (acedGetString(Adesk::kFalse, _T("\n请输入图层的名称："), layerName) != RTNORM)
	{
		return;
	}

	// 获得当前图形的层表
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

	// 判断是否包含指定名称的层表记录
	if (!pLayerTbl->has(layerName))
	{
		acutPrintf(_T("不包含您输入的图层名称。"));
		pLayerTbl->close();
		return;
	}

	// 获得指定层表记录的指针
	AcDbLayerTableRecord *pLayerTblRcd;
	pLayerTbl->getAt(layerName, pLayerTblRcd,AcDb::kForWrite);
	pLayerTblRcd->erase();

	// 关闭
	pLayerTblRcd->close();
	pLayerTbl->close();
}

// 定义一个用于将当前图形中存在的所有图层及其特性导出到一个文本文件中的函数
void ExportLayer()
{
	// 创建所要导出的文本文件
	CStdioFile f;
	CFileException e;
	const wchar_t *pFileName = _T("C:/Users/Administrator/Documents/ARX_Learn/exportfile.txt");
	if (!f.Open(pFileName, CFile::modeCreate | CFile::modeWrite, &e))
	{
		acutPrintf(_T("\n创建导出文件失败！"));
		return;
	}

	// 获得层表指针
	AcDbLayerTable *pLayerTbl;
	AcDbLayerTableRecord *pLayerTblRcd;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl,AcDb::kForRead);

	// 使用遍历器访问每一条层表记录
	AcDbLayerTableIterator *pltr;
	pLayerTbl->newIterator(pltr);
	for (pltr->start(); !pltr->done(); pltr->step())
	{
		pltr->getRecord(pLayerTblRcd, AcDb::kForRead);

		// 输出图层信息
		CString strLayerInfo; // 图层名称
		wchar_t *layerName;
		pLayerTblRcd->getName(layerName);
		strLayerInfo = layerName;
		free(layerName);
		strLayerInfo += ",";  // 分隔符

		CString strColor;  // 图层颜色
		AcCmColor color = pLayerTblRcd->color();
		strColor.Format(_T("%d"), color.colorIndex());
		strLayerInfo += strColor;
		strLayerInfo += ",";  // 分隔符

		CString strLineType; // 图层线型
		AcDbLinetypeTableRecord *pLineTypeTblRcd;
		acdbOpenObject(pLineTypeTblRcd, pLayerTblRcd->linetypeObjectId(), AcDb::kForRead);
		wchar_t *lineTypeName;
		pLineTypeTblRcd->getName(lineTypeName);
		strLineType = lineTypeName;
		pLineTypeTblRcd->close();
		free(lineTypeName);
		strLayerInfo += strLineType;
		strLayerInfo += ",";

		CString strLineWeight;  // 图层的线宽
		AcDb::LineWeight lineWeight = pLayerTblRcd->lineWeight();
		strLineWeight.Format(_T("%d"), lineWeight);
		strLayerInfo += strLineWeight;
		
		// 将图层特性写入到文件中
		f.WriteString(strLayerInfo);
		f.WriteString(_T("\n"));
		pLayerTblRcd->close();
	}
	delete pltr;
	pLayerTbl->close();
}

// 定义一个能够按照文本文件中的图层列表在当前图形中创建图层，并且符合图层列表中的各项特性的函数
void ImportLayer()
{
	// 打开所要导入的文本文件
	CStdioFile f;
	CFileException e;
	const wchar_t *pFileName = _T("C:/Users/Administrator/Documents/ARX_Learn/exportfile.txt");

	if (!f.Open(pFileName, CFile::modeRead, &e))
	{
		acutPrintf(_T("\n打开导入文件失败！"));
		return;
	}

	// 获得层表指针
	AcDbLayerTable *pLayerTbl;
	AcDbLayerTableRecord *pLayerTblRcd;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForRead);
	
	// 读取文件中的每一行数据
	CString strLineText; // 一行文字
	while (f.ReadString(strLineText))
	{
		// 跳过空行
		if (strLineText.IsEmpty())
			continue;
		
		// 解析出屠城名称、颜色、线型和线宽
		CStringArray layerInfo;
		CommonTools::splitStr(strLineText, _T(","), layerInfo);

		// 创建新的层表记录，或者打开存在的块表记录
		AcDbLayerTableRecord *pLayerTblRcd;
		AcDbObjectId layerTblRcdId;
		if (pLayerTbl->has(layerInfo.GetAt(0)))
		{
			pLayerTbl->getAt(layerInfo.GetAt(0), layerTblRcdId);
		}
		else
		{
			pLayerTblRcd = new AcDbLayerTableRecord();
			pLayerTblRcd->setName(layerInfo.GetAt(0));
			pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
			pLayerTblRcd->close();
		}
		
		acdbOpenObject(pLayerTblRcd, layerTblRcdId, AcDb::kForWrite);

		// 设置层表记录的颜色
		AcCmColor color;
		//auto colorIndex = atoi(layerInfo.GetAt(1));

		// 未写完，有问题，放弃编写
	}

}

// 定义一个创建新的字体样式的函数
void AddStyle()
{
	// 获得字体样式表
	AcDbTextStyleTable *pTextStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);

	// 创建新的字体样式表记录
	AcDbTextStyleTableRecord *pTextStyleTblRcd = new AcDbTextStyleTableRecord();

	// 设置字体样式表记录的名称
	pTextStyleTblRcd->setName(_T("仿宋体"));

	// 设置字体文件名称
	pTextStyleTblRcd->setFileName(_T("simfang.ttf"));

	// 设置宽高比例
	pTextStyleTblRcd->setXScale(0.7);

	// 将新的记录添加到字体样式表
	pTextStyleTbl->add(pTextStyleTblRcd);

	// 关闭
	pTextStyleTblRcd->close();
	pTextStyleTbl->close();
}

// 定义一个创建新的标注样式的函数
void AddDimStyle()
{
	// 获得要创建的标注样式名称
	wchar_t styleName[100];
	if (acedGetString(Adesk::kFalse, _T("\n输入新样式的名称"), styleName) != RTNORM)
	{
		return;
	}

	// 获得当前图形的标注样式表
	AcDbDimStyleTable *pDimStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);

	// 判断当前图形的标注样式表是否包含用户输入的样式表名称
	if (pDimStyleTbl->has(styleName))
	{
		acutPrintf(_T("\n您输入的样式表名已经存在"));
		pDimStyleTbl->close();
		return;
	}

	// 创建新的标注样式表记录
	AcDbDimStyleTableRecord *pDimStyleTblRcd = new AcDbDimStyleTableRecord();

	// 设置标注样式的特性
	pDimStyleTblRcd->setName(styleName); // 样式名称
	pDimStyleTblRcd->setDimasz(3); // 箭头长度
	pDimStyleTblRcd->setDimexe(3); // 尺寸界线与标注点的偏移量
	pDimStyleTblRcd->setDimtad(1); // 文字位于标注线的上方
	pDimStyleTblRcd->setDimtxt(3); // 标注文字的高度

	// 将标注样式表记录添加到标注样式表中
	pDimStyleTbl->add(pDimStyleTblRcd);

	pDimStyleTblRcd->close();
	pDimStyleTbl->close();
}

// 定义一个用于更新视图的比例缩放的函数
void ChangeView()
{
	// 提示用户输入缩放的比例因子
	ads_real scale;
	if (acedGetReal(_T("\n输入缩放比例因子："), &scale) != RTNORM)
	{
		return;
	}

	// 获得当前视图
	AcDbViewTableRecord view = GetCurrentView();

	// 修改视图
	view.setWidth(view.width() / scale);
	view.setHeight(view.height() / scale);

	// 更新视图
	acedSetCurrentView(&view, NULL);
}

// 定义一个用于模拟窗口缩放的功能的函数
void ZoomWindow()
{
	// 提示用户选择定义缩放窗口的两个角点
	ads_point pt1, pt2;
	if (acedGetPoint(NULL, _T("\n输入第一个角点："), pt1) != RTNORM)
		return;
	if (acedGetPoint(NULL, _T("\n输入第二个角点："), pt2) != RTNORM)
		return;
	struct resbuf wcs, ucs, dcs; // 转换坐标时使用的坐标系统标记 

	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	ucs.restype = RTSHORT;
	ucs.resval.rint = 1;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;
	acedTrans(pt1, &ucs, &dcs, 0, pt1);
	acedTrans(pt2, &ucs, &dcs, 0, pt2);
	AcDbViewTableRecord view = GetCurrentView();

	// 设置视图的中心点
	view.setCenterPoint(AcGePoint2d((pt1[X] + pt2[X]) / 2,(pt1[Y] + pt2[Y]) / 2));

	// 设置视图的高度和宽度
	view.setHeight(fabs(pt1[Y] - pt2[Y]));
	view.setWidth(fabs(pt1[X] - pt2[X]));
	// 将视图对象设置为当前视图
	acedSetCurrentView(&view, NULL);
}

// 定义一个用于实现在各种视图（包括透视模式）中的范围缩放的函数
void ZoomExtents()
{
	AcDbBlockTable *pBlkTbl;
	AcDbBlockTableRecord *pBlkTblRcd;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl, AcDb::kForRead);
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,
		AcDb::kForRead);
	pBlkTbl->close();

	// 获得当前图形中所有实体的最小包围盒
	AcDbExtents extent;
	extent.addBlockExt(pBlkTblRcd);
	pBlkTblRcd->close();

	// 计算长方形的顶点
	ads_point pt[7];
	pt[0][X] = pt[3][X] = pt[4][X] = pt[7][X] = extent.minPoint().x;
	pt[1][X] = pt[2][X] = pt[5][X] = pt[6][X] = extent.maxPoint().x;
	pt[0][Y] = pt[1][Y] = pt[4][Y] = pt[5][Y] = extent.minPoint().y;
	pt[2][Y] = pt[3][Y] = pt[6][Y] = pt[7][Y] = extent.maxPoint().y;
	pt[0][Z] = pt[1][Z] = pt[2][Z] = pt[3][Z] = extent.maxPoint().z;
	pt[4][Z] = pt[5][Z] = pt[6][Z] = pt[7][Z] = extent.minPoint().z;

	// 将长方体的所有角点转移到DCS中
	struct resbuf wcs, dcs; // 转换坐标时使用的坐标系统标记 
	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;

	acedTrans(pt[0], &wcs, &dcs, 0, pt[0]);
	acedTrans(pt[1], &wcs, &dcs, 0, pt[1]);
	acedTrans(pt[2], &wcs, &dcs, 0, pt[2]);
	acedTrans(pt[3], &wcs, &dcs, 0, pt[3]);
	acedTrans(pt[4], &wcs, &dcs, 0, pt[4]);
	acedTrans(pt[5], &wcs, &dcs, 0, pt[5]);
	acedTrans(pt[6], &wcs, &dcs, 0, pt[6]);
	acedTrans(pt[7], &wcs, &dcs, 0, pt[7]);
	// 获得所有角点在DCS中最小的包围矩形
	double xMax = pt[0][X], xMin = pt[0][X];
	double yMax = pt[0][Y], yMin = pt[0][Y];
	for (int i = 1; i <= 7; i++)
	{
		if (pt[i][X] > xMax)
			xMax = pt[i][X];
		if (pt[i][X] < xMin)
			xMin = pt[i][X];
		if (pt[i][Y] > yMax)
			yMax = pt[i][Y];
		if (pt[i][Y] < yMin)
			yMin = pt[i][Y];
	}
	AcDbViewTableRecord view = GetCurrentView();
	// 设置视图的中心点
	view.setCenterPoint(AcGePoint2d((xMin + xMax) / 2,
		(yMin + yMax) / 2));
	// 设置视图的高度和宽度
	view.setHeight(fabs(yMax - yMin));
	view.setWidth(fabs(xMax - xMin));
	// 将视图对象设置为当前视图
	Acad::ErrorStatus es = acedSetCurrentView(&view, NULL);

}

// 定义一个获取当前图层的函数
AcDbViewTableRecord GetCurrentView()
{
	AcDbViewTableRecord view;
	struct resbuf rb;
	struct resbuf wcs, ucs, dcs; // 转换坐标时使用的坐标系统标记

	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	ucs.restype = RTSHORT;
	ucs.resval.rint = 1;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;

	// 获得当前视口的 “查看” 模式
	acedGetVar(_T("VIEWMODE"), &rb);
	view.setPerspectiveEnabled(rb.resval.rint & 1);
	view.setFrontClipEnabled(rb.resval.rint & 2);
	view.setBackClipEnabled(rb.resval.rint & 4);
	view.setFrontClipAtEye(rb.resval.rint & 16);

	// 当前视口中视图的中心点（UCS坐标）
	acedGetVar(_T("VIEWCTR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &dcs, 0, rb.resval.rpoint);
	view.setCenterPoint(AcGePoint2d(rb.resval.rpoint[X], rb.resval.rpoint[Y]));

	// 当前视口中目标点的位置（以 UCS 坐标表示）
	acedGetVar(_T("TARGET"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 0, rb.resval.rpoint);
	view.setTarget(AcGePoint3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));

	// 当前视口的观察方向（UCS）
	acedGetVar(_T("VIEWDIR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 1, rb.resval.rpoint);
	view.setViewDirection(AcGeVector3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));
	// 当前视口的视图高度（图形单位）
	acedGetVar(_T("VIEWSIZE"), &rb);
	view.setHeight(rb.resval.rreal);
	double height = rb.resval.rreal;
	// 以像素为单位的当前视口的大小（X 和 Y 值）
	acedGetVar(_T("SCREENSIZE"), &rb);
	view.setWidth(rb.resval.rpoint[X] / rb.resval.rpoint[Y] * height);
	// 当前视口的视图扭转角
	acedGetVar(_T("VIEWTWIST"), &rb);
	view.setViewTwist(rb.resval.rreal);
	// 将模型选项卡或最后一个布局选项卡置为当前
	acedGetVar(_T("TILEMODE"), &rb);
	int tileMode = rb.resval.rint;
	// 设置当前视口的标识码
	acedGetVar(_T("CVPORT"), &rb);
	int cvport = rb.resval.rint;
	// 是否是模型空间的视图
	bool paperspace = ((tileMode == 0) && (cvport == 1)) ? true : false;
	view.setIsPaperspaceView(paperspace);
	if (!paperspace)
	{
		// 当前视口中前向剪裁平面到目标平面的偏移量
		acedGetVar(_T("FRONTZ"), &rb);
		view.setFrontClipDistance(rb.resval.rreal);
		// 获得当前视口后向剪裁平面到目标平面的偏移值
		acedGetVar(_T("BACKZ"), &rb);
		view.setBackClipDistance(rb.resval.rreal);
	}
	else
	{
		view.setFrontClipDistance(0.0);
		view.setBackClipDistance(0.0);
	}
	return view;
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


