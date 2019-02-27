#pragma once

wchar_t* c2w(const char *str);

class CPubFunction
{
public:
	// 根据指定的多段线创建对应的几何类曲线，包含两个参数，pPline 指定已知的多段线，pGeCurve 参数输出创建的几何曲线
	static bool PolyToGeCurve(AcDbPolyline *&pPline, AcGeCurve2d *&pGeCurve);
	
	// 用于根据指定的一组点创建一个结果缓冲区链表，它接受一个参数 arrPoints，该参数指定一组点，而函数返回创建的结果缓冲区链表
	static struct resbuf* BuidRbFromPtArray(AcGePoint2dArray &arrPoints);

	//	选择指定多段线内部（或者与多段线构成的区域相交）的所有实体
	static bool SelectEntInPoly(AcDbPolyline *pPline, AcDbObjectIdArray &ObjectIdArray, const char *selectMode, double approxEps);

	//	保存当前视口两个角点的坐标
	static bool GetViewPortBound(AcGePoint2d &ptMin, AcGePoint2d &ptMax);

	static void ZoomExtents();
	static AcDbViewTableRecord GetCurrentView();
	static Acad::ErrorStatus WindowZoom(const AcGePoint2d &ptMin, const AcGePoint2d &ptMax, double scale);
};	

bool CPubFunction::PolyToGeCurve(AcDbPolyline *&pPline, AcGeCurve2d *&pGeCurve)
{
	int nSegs; // 多线段的段数
	AcGeLineSeg2d line, *pLine; // 几何曲线的直线段部分
	AcGeCircArc2d arc, *pArc; // 几何曲线的圆弧部分
	AcGeVoidPointerArray geCurves; // 指向组成几何曲线各分段的指针数组

	nSegs = pPline->numVerts() - 1;

	// 根据多线段创建对应的分段几何曲线
	for (int i = 0; i < nSegs; i++)
	{
		if (pPline->segType(i) == AcDbPolyline::kLine)
		{
			pPline->getLineSegAt(i, line);
			pLine = new AcGeLineSeg2d(line);
			geCurves.append(pLine);
		}
		else if (pPline->segType(i) == AcDbPolyline::kArc)
		{
			pPline->getArcSegAt(i, arc);
			pArc = new AcGeCircArc2d(arc);
			geCurves.append(pArc);
		}
	}

	// 处理闭合多段线最后一段是圆弧的情况
	if (pPline->isClosed() && pPline->segType(nSegs) == AcDbPolyline::kArc)
	{
		pPline->getArcSegAt(nSegs, arc);
		pArc = new AcGeCircArc2d(arc);
		pArc->setAngles(arc.startAng(), arc.endAng() - (arc.endAng() - arc.startAng()) / 100);
		geCurves.append(pArc);
	}

	// 根据分段的几何曲线创建对应的复合曲线
	if (geCurves.length() == 1)
	{
		pGeCurve = (AcGeCircArc2d *)geCurves[0];
	}
	else
	{
		pGeCurve = new AcGeCompositeCurve2d(geCurves);
	}

	// 释放动态分配的内存
	if (geCurves.length() > 1)
	{
		for (int i = 0; i < geCurves.length(); i++)
		{
			delete geCurves[i];
		}
	}

	return true;
}

bool CPubFunction::SelectEntInPoly(AcDbPolyline *pPline, AcDbObjectIdArray &ObjectIdArray, const char *selectMode, double approxEps)
{
	// 判断selectMode的有效性
	if (strcmp(selectMode, "CP") != 0 && strcmp(selectMode, "WP") != 0)
	{
		acedAlert(_T("函数SelectEntInPline中，指定了无效的选择模式"));
		return false;
	}
	
	// 清除数组中所有的ObjectId
	for (int i = 0; i < ObjectIdArray.length(); i++)
	{
		ObjectIdArray.removeAt(i);
	}

	AcGeCurve2d *pGeCurve; // 多线段对应的几何曲线

	Adesk::Boolean bClosed = pPline->isClosed(); // 多线段是否闭合

	// 确保多线段作为选择界时是闭合的
	if (bClosed != Adesk::kTrue)
	{
		pPline->setClosed(!bClosed);
	}

	// 创建对应的几何类曲线
	CPubFunction::PolyToGeCurve(pPline, pGeCurve);

	AcGePoint2dArray SamplePtArray; // 存储曲线的样本点
	AcGeDoubleArray ParamArray;  // 存储样本点对应的参数值
	AcGePoint2d ptStart, ptEnd; // 几何曲线的起点和终点

	Adesk::Boolean bRet = pGeCurve->hasStartPoint(ptStart);
	bRet = pGeCurve->hasEndPoint(ptStart);

	double valueSt = pGeCurve->paramOf(ptStart);
	double valueEn = pGeCurve->paramOf(ptEnd);

	pGeCurve->getSamplePoints(valueSt, valueEn, approxEps, SamplePtArray, ParamArray);

	delete pGeCurve; // 在函数PolyToGeCurve终分配了内存

	// 确保杨店的起点和重点不重合
	AcGeTol tol;
	tol.setEqualPoint(0.01);
	AcGePoint2d ptFirst = SamplePtArray[0];
	AcGePoint2d ptLast = SamplePtArray[SamplePtArray.length() - 1];
	if (ptFirst.isEqualTo(ptLast))
	{
		SamplePtArray.removeLast();
	}

	// 根据样本点创建结果缓冲区链表
	struct  resbuf *rb;
	rb = CPubFunction::BuidRbFromPtArray(SamplePtArray);

	// 使用acedSSGet函数创建选择集
	ads_name ssName;
	int rt = acedSSGet(c2w(selectMode), rb, NULL, NULL, ssName);

	if (rt != RTNORM)
	{
		acutRelRb(rb);	// 释放结果缓冲区链表
		return false;
	}

	// 将选择集中所有的对象添加到ObjectIdArray
	long length;
	acedSSLength(ssName, &length);
	for (int i = 0; i < length; i++)
	{
		// 获得指定元素的ObjectId
		ads_name ent;
		acedSSName(ssName, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);

		// 获得指向当前元素的指针
		AcDbEntity *pEnt;
		Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead);

		// 选择到作为边界的多线段了，直接跳过该次循环
		if (es == Acad::eWasOpenForWrite)
			continue;

		ObjectIdArray.append(pEnt->objectId());

		pEnt->close();
	}

	// 释放内存
	acutRelRb(rb);	// 释放结果缓冲区链表
	acedSSFree(ssName); // 删除选择集
	return true;
}

struct resbuf* CPubFunction::BuidRbFromPtArray(AcGePoint2dArray &arrPoints)
{
	struct resbuf *retRb = NULL;
	int count = arrPoints.length();
	if (count <= 1)
	{
		acedAlert(_T("函数BuildBbFromPtArray中，点数组包含元素个数不足！"));
		return retRb;
	}

	// 使用第一个点来构建结果缓冲区链表的头结点
	ads_point adsPt;
	adsPt[X] = arrPoints[0].x;
	adsPt[Y] = arrPoints[0].y;
	retRb = acutBuildList(RTPOINT, adsPt, RTNONE);

	struct resbuf *nextRb = retRb;	// 辅助指针

	for (int i = 1; i < count; i++)	// 注意：不考虑第一个元素，因此i从1开始
	{
		adsPt[X] = arrPoints[i].x;
		adsPt[Y] = arrPoints[i].y;
		// 动态创建新的节点，并将其链接到原来的链表尾部
		nextRb->rbnext = acutBuildList(RTPOINT, adsPt, RTNONE);
		nextRb = nextRb->rbnext;
	}
	return retRb;
}

bool CPubFunction::GetViewPortBound(AcGePoint2d &ptMin, AcGePoint2d &ptMax)
{
	// 获得当前视口的高度
	double viewHeight; // 视口高度（用图形单位表示）
	struct resbuf rbViewSize;
	if (acedGetVar(_T("VIEWSIZE"), &rbViewSize) != RTNORM)
	{
		return false;
	}

	viewHeight = rbViewSize.resval.rreal;

	// 获得当前视口的宽度
	double viewWidth;	// 视口宽度（用图形单位表示）
	struct resbuf rbScreenSize;
	if (acedGetVar(_T("SCREENSIZE"), &rbScreenSize) != RTNORM)
	{
		return false;
	}

	// width / height = rpoint[X] / rpoint[Y] 利用高宽比计算
	viewWidth = (rbScreenSize.resval.rpoint[X] / rbScreenSize.resval.rpoint[Y]) *viewHeight;

	// 获得当前视口中心点WCS坐标
	AcGePoint3d viewCenterPt; // 视口中心
	struct resbuf rbViewCtr;
	if (acedGetVar(_T("VIEWCTR"), &rbViewCtr) != RTNORM)
	{
		return false;
	}

	struct resbuf UCS, WCS;
	WCS.restype = RTSHORT;
	WCS.resval.rint = 0;
	UCS.restype = RTSHORT;
	UCS.resval.rint = 1;

	acedTrans(rbViewCtr.resval.rpoint, &UCS, &WCS, 0, rbViewCtr.resval.rpoint);
	viewCenterPt = asPnt3d(rbViewCtr.resval.rpoint);

	// 设置视口角点坐标
	ptMin[X] = viewCenterPt[X] - viewWidth / 2;
	ptMin[Y] = viewCenterPt[Y] - viewHeight / 2;
	ptMax[X] = viewCenterPt[X] + viewWidth / 2;
	ptMax[Y] = viewCenterPt[Y] + viewHeight / 2;
	return true;
}

void CPubFunction::ZoomExtents()
{
	AcDbBlockTable *pBlkTbl;
	AcDbBlockTableRecord *pBlkTblRcd;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl,AcDb::kForRead);
	
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
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


AcDbViewTableRecord CPubFunction::GetCurrentView()
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
	// 获得当前视口的"查看"模式
	acedGetVar(_T("VIEWMODE"), &rb);
	view.setPerspectiveEnabled(rb.resval.rint & 1);
	view.setFrontClipEnabled(rb.resval.rint & 2);
	view.setBackClipEnabled(rb.resval.rint & 4);
	view.setFrontClipAtEye(!(rb.resval.rint & 16));
	// 当前视口中视图的中心点（UCS坐标）
	acedGetVar(_T("VIEWCTR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &dcs, 0, rb.resval.rpoint);
	view.setCenterPoint(AcGePoint2d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y]));
	// 当前视口透视图中的镜头焦距长度（单位为毫米）
	acedGetVar(_T("LENSLENGTH"), &rb);
	view.setLensLength(rb.resval.rreal);
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

Acad::ErrorStatus CPubFunction::WindowZoom(const AcGePoint2d &ptMin, const AcGePoint2d &ptMax, double scale)
{
	AcDbViewTableRecord view;
	AcGePoint2d ptCenter2d((ptMin[X] + ptMax[X]) / 2,
		(ptMin[Y] + ptMax[Y]) / 2);
	view.setCenterPoint(ptCenter2d);
	view.setWidth((ptMax[X] - ptMin[X]) / scale);
	view.setHeight((ptMax[Y] - ptMin[Y]) / scale);
	Acad::ErrorStatus es = acedSetCurrentView(&view, NULL);
	return es;
}

wchar_t* c2w(const char *str)
{
	int length = strlen(str) + 1;
	wchar_t *t = (wchar_t*)malloc(sizeof(wchar_t)*length);
	memset(t, 0, length * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);
	return t;
}
