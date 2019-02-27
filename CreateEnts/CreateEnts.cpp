// CreateEnts.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


// 封装计算的相关函数
class CCalculation
{
public:
	static AcGePoint2d MiddlePoint(AcGePoint2d pt1, AcGePoint2d pt2);
	static AcGePoint3d MiddlePoint(AcGePoint3d pt1, AcGePoint3d pt2);
	static AcGePoint3d Pt2dTo3d(AcGePoint2d pt);
	static double PI();
	static double Max(double a, double b);
	static double Min(double a, double b);
	// 弧度转成角度
	static double RtoG(double angle);
	// 角度转成弧度
	static double GtoR(double angle);
};


// 创建一些实体
class CCreateEnt
{
public:
	// 在XYZ平面上画直线
	static AcDbObjectId CreateLine();
	static AcDbObjectId CreateLine(AcGePoint3d ptStart, AcGePoint3d ptEnd);
	static AcDbObjectId PostToModelSpace(AcDbEntity *pEnt);

	// 添加圆
	// 在XYZ平面上画圆
	static AcDbObjectId CreateCircle(AcGePoint3d ptCenter, AcGeVector3d vec, double radius);
	// 在XOY平面上画圆
	static AcDbObjectId CreateCircle(AcGePoint3d ptCenter, double radius);
	// 两点法创建圆
	static AcDbObjectId CreateCircle(AcGePoint2d pt1, AcGePoint2d pt2);
	// 三点法画圆
	static AcDbObjectId CreateCircle(AcGePoint2d pt1, AcGePoint2d pt2, AcGePoint2d pt3);

	// 添加圆弧
	static AcDbObjectId CreateArc(AcGePoint3d ptCenter, AcGeVector3d vec,
		double radius, double startAngle, double endAngle);
	// 添加一个创建位于 XOY 平面上的圆弧的函数
	static AcDbObjectId CreateArc(AcGePoint2d ptCenter, double radius, double startAngle, double endAngle);
	// 三点法创建圆弧
	static AcDbObjectId CreateArc(AcGePoint2d ptStart, AcGePoint2d ptOnArc,
		AcGePoint2d ptEnd);
	// 使用“起点、圆心、终点”方法来创建圆弧
	static AcDbObjectId CreateArcSCE(AcGePoint2d ptStart, AcGePoint2d ptCenter, AcGePoint2d ptEnd);

	// 使用“起点、圆心、圆弧角度”方法来创建圆弧
	static AcDbObjectId CreateArc(AcGePoint2d ptStart, AcGePoint2d ptCenter, double angle);

	// 创建轻量级多线段
	static AcDbObjectId CreatePolyline(AcGePoint2dArray points, double width = 0);

	// 创建仅包含一条直线的多段线
	static AcDbObjectId CreatePolyline(AcGePoint2d ptStart, AcGePoint2d ptEnd, double width);

	// 创建三维多段线的函数
	static AcDbObjectId Create3dPolyline(AcGePoint3dArray points);

	// 创建正多边形的函数。创建正多边形的输入参数为中心、边数、外接圆半径、旋转角度（弧度值）和线宽
	static AcDbObjectId CreatePolygon(AcGePoint2d ptCenter, int number, double radius, double rotation, double width);

	// 用于创建矩形的函数 该函数根据两个角点和线宽来创建矩形
	static AcDbObjectId CreateRectangle(AcGePoint2d pt1, AcGePoint2d pt2, double width);

	// 创建圆的函数，输入参数包括圆心、半径和线宽
	static AcDbObjectId CreatePolyCircle(AcGePoint2d ptCenter, double radius, double width);

	// 用于创建圆弧的函数，输入参数包括圆心、半径、起始角度、终止角度和线宽
	static AcDbObjectId CreatePolyArc(AcGePoint2d ptCenter, double radius, double angleStart, double angleEnd, double width);

	// 创建椭圆
	// 通过创建椭圆的默认构造函数创建椭圆
	static AcDbObjectId CreateEllipse(const AcGePoint3d& center, const AcGeVector3d& unitNormal, const AcGeVector3d& majorAxis, double radiusRatio);
	
	// 添加根据外接矩形创建椭圆的函数，输入参数为外接矩形的两个角点
	static AcDbObjectId CreateEllipse(AcGePoint2d pt1, AcGePoint2d pt2);

	// 创建样条曲线
	// 添加创建样条曲线的函数，输入参数包括样条曲线的拟合点、拟合曲线的阶数和允许的拟合误差
	static AcDbObjectId CreateSpline(const AcGePoint3dArray& points,int order = 4, double fitTolerance = 0.0);
	// 指定样条曲线起点和终点的切线方向
	static AcDbObjectId CreateSpline(const AcGePoint3dArray& points, const AcGeVector3d& startTangent, const AcGeVector3d &endTangent, int order = 4, double fitTolerance = 0.0);
};

class CModifyEnt
{
public:
	// 改变实体的颜色
	static Acad::ErrorStatus __ChangeColor(AcDbObjectId entId, Adesk::UInt16
		colorIndex);
	// 改变实体的图层
	static Acad::ErrorStatus ChangeLayer(AcDbObjectId entId, CString strLayerName);
	// 改变实体的线型
	static Acad::ErrorStatus ChangeLineType(AcDbObjectId entId, CString strLineType);

	// 旋转实体
	static Acad::ErrorStatus Rotate(AcDbObjectId entId, AcGePoint2d ptBase, double rotation);

	// 移动实体
	static Acad::ErrorStatus Move(AcDbObjectId entId, AcGePoint3d ptBase, AcGePoint3d ptDest);

	// 缩放实体
	static Acad::ErrorStatus Scale(AcDbObjectId entId, AcGePoint3d ptBase, double scaleFactor);
};



AcDbObjectId CCreateEnt::CreateLine()
{
	// 在内存上创建一个新的AcDbLine对象
	AcGePoint3d ptStart(0, 0, 0);
	AcGePoint3d ptEnd(200, 200, 0);
	AcDbLine  *pLine = new AcDbLine(ptStart, ptEnd);

	// 获得指向块表的指针
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	// // 获得指向特定的块表记录（模型空间）的指针
	AcDbBlockTableRecord *pBlockTableRecorder;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecorder, AcDb::kForWrite);

	// 将AcDbLine类的对象添加到块表记录中
	AcDbObjectId lineId;
	pBlockTableRecorder->appendAcDbEntity(lineId, pLine);

	// 关闭图形数据库的各种对象
	pBlockTable->close();
	pBlockTableRecorder->close();
	pLine->close();
	return lineId;
}

AcDbObjectId CCreateEnt::CreateLine(AcGePoint3d ptStart, AcGePoint3d ptEnd)
{
	AcDbLine  *pLine = new AcDbLine(ptStart, ptEnd);
	// 将实体添加到图形数据库中去
	AcDbObjectId lineId;
	lineId = CCreateEnt::PostToModelSpace(pLine);
	return lineId;
}


AcDbObjectId CCreateEnt::PostToModelSpace(AcDbEntity *pEnt)
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

// 在XYZ平面上画圆
AcDbObjectId CCreateEnt::CreateCircle(AcGePoint3d ptCenter, AcGeVector3d vec, double radius)
{
	AcDbCircle *pCircle = new AcDbCircle(ptCenter, vec, radius);
	AcDbObjectId lineId = CCreateEnt::PostToModelSpace(pCircle);
	return lineId;
}

// 在XOY平面上画圆
AcDbObjectId CCreateEnt::CreateCircle(AcGePoint3d ptCenter, double radius)
{
	AcGeVector3d vec(0, 0, 1);
	return CCreateEnt::CreateCircle(ptCenter, vec, radius);
}

// 两点法创建圆
AcDbObjectId CCreateEnt::CreateCircle(AcGePoint2d pt1, AcGePoint2d pt2)
{
	// 计算圆心和半径
	AcGePoint2d pt = CCalculation::MiddlePoint(pt1, pt2);
	AcGePoint3d ptCenter(pt[X], pt[Y], 0); // 圆心
	double radius = pt1.distanceTo(pt2) / 2;

	// 创建圆
	return CCreateEnt::CreateCircle(ptCenter, radius);
}

// 三点法画圆
AcDbObjectId CCreateEnt::CreateCircle(AcGePoint2d pt1, AcGePoint2d pt2, AcGePoint2d pt3)
{
	//// 使用数学方法
	//double xysm, xyse, xy;
	//AcGePoint3d ptCenter;
	//double radius;
	//xy = pow(pt1[X], 2) + pow(pt1[Y], 2);
	//xyse = xy - pow(pt3[X], 2) - pow(pt3[Y], 2);
	//xysm = xy - pow(pt2[X], 2) - pow(pt2[Y], 2);
	//xy = (pt1[X] - pt2[X]) * (pt1[Y] - pt3[Y]) - (pt1[X] - pt3[X]) * (pt1[Y] -
	//	pt2[Y]);

	//// 判断参数有效性
	//if (fabs(xy) < 0.000001)
	//{
	//	//AfxMessageBox("所输入的参数无法创建圆形！");
	//	return 0;
	//}
	//// 获得圆心和半径
	//ptCenter[X] = (xysm * (pt1[Y] - pt3[Y]) - xyse * (pt1[Y] - pt2[Y])) / (2 *
	//	xy);
	//ptCenter[Y] = (xyse * (pt1[X] - pt2[X]) - xysm * (pt1[X] - pt3[X])) / (2 *
	//	xy);
	//ptCenter[Z] = 0;
	//radius = sqrt((pt1[X] - ptCenter[X]) * (pt1[X] - ptCenter[X]) +
	//	(pt1[Y] - ptCenter[Y]) * (pt1[Y] - ptCenter[Y]));
	//if (radius < 0.000001)
	//{
	//	//AfxMessageBox("半径过小！");
	//	return 0;
	//}
	//return CCreateEnt::CreateCircle(ptCenter, radius);

	// 使用几何类
	AcGeCircArc2d geArc(pt1, pt2, pt3);
	AcGePoint3d ptCenter(geArc.center().x, geArc.center().y, 0);
	return CCreateEnt::CreateCircle(ptCenter, geArc.radius());
}

// 向模型空间添加一个圆弧
AcDbObjectId CCreateEnt::CreateArc(AcGePoint3d ptCenter, AcGeVector3d vec,
	double radius, double startAngle, double endAngle)
{
	AcDbArc *pArc = new AcDbArc(ptCenter, vec, radius, startAngle, endAngle);
	AcDbObjectId arcId = CCreateEnt::PostToModelSpace(pArc);
	return arcId;
}

// 添加一个创建位于 XOY 平面上的圆弧的函数
AcDbObjectId CCreateEnt::CreateArc(AcGePoint2d ptCenter, double radius, double startAngle, double endAngle)
{
	AcGeVector3d vec(0, 0, 1);
	return CCreateEnt::CreateArc(CCalculation::Pt2dTo3d(ptCenter), vec, radius, startAngle, endAngle);
}

// 三点法创建圆弧
AcDbObjectId CCreateEnt::CreateArc(AcGePoint2d ptStart, AcGePoint2d ptOnArc,
	AcGePoint2d ptEnd)
{
	// 使用几何类获得圆心、半径
	AcGeCircArc2d geArc(ptStart, ptOnArc, ptEnd);
	AcGePoint2d ptCenter = geArc.center();
	double radius = geArc.radius();

	// 计算起始和终止角度
	AcGeVector2d vecStart(ptStart.x - ptCenter.x, ptStart.y - ptCenter.y);
	AcGeVector2d vecEnd(ptEnd.x - ptCenter.x, ptEnd.y - ptCenter.y);
	double startAngle = vecStart.angle();
	double endAngle = vecEnd.angle();

	return CCreateEnt::CreateArc(ptCenter, radius, startAngle, endAngle);
}

// 使用“起点、圆心、终点”方法来创建圆弧
AcDbObjectId CCreateEnt::CreateArcSCE(AcGePoint2d ptStart, AcGePoint2d ptCenter, AcGePoint2d ptEnd)
{
	// 计算半径
	double radius = ptCenter.distanceTo(ptStart);

	// 计算起、终点角度
	AcGeVector2d vecStart(ptStart.x - ptCenter.x, ptStart.y - ptCenter.y);
	AcGeVector2d vecEnd(ptEnd.x - ptCenter.x, ptEnd.y - ptCenter.y);
	double startAngle = vecStart.angle();
	double endAngle = vecEnd.angle();

	// 创建圆弧
	return CCreateEnt::CreateArc(ptCenter, radius, startAngle, endAngle);
}

// 使用“起点、圆心、圆弧角度”方法来创建圆弧
AcDbObjectId CCreateEnt::CreateArc(AcGePoint2d ptStart, AcGePoint2d ptCenter, double angle)
{
	// 计算半径
	double radius = ptCenter.distanceTo(ptStart);
	// 计算起、终点角度
	AcGeVector2d vecStart(ptStart.x - ptCenter.x, ptStart.y - ptCenter.y);
	double startAngle = vecStart.angle();
	double endAngle = startAngle + angle;
	// 创建圆弧
	return CCreateEnt::CreateArc(ptCenter, radius, startAngle, endAngle);
}

// 创建轻量级多线段
AcDbObjectId CCreateEnt::CreatePolyline(AcGePoint2dArray points, double width)
{
	int numVertices = points.length();
	AcDbPolyline *pPoly = new AcDbPolyline(numVertices);
	for (int i = 0; i < numVertices; i++)
	{
		pPoly->addVertexAt(i, points.at(i), 0, width, width);
	}
	AcDbObjectId polyId;
	polyId = CCreateEnt::PostToModelSpace(pPoly);
	return polyId;
}

// 创建仅包含一条直线的多段线
AcDbObjectId CCreateEnt::CreatePolyline(AcGePoint2d ptStart, AcGePoint2d ptEnd, double width)
{
	AcGePoint2dArray points;
	points.append(ptStart);
	points.append(ptEnd);
	return CCreateEnt::CreatePolyline(points, width);
}

// 创建三维多段线的函数
AcDbObjectId CCreateEnt::Create3dPolyline(AcGePoint3dArray points)
{
	AcDb3dPolyline *pPoly3d = new
		AcDb3dPolyline(AcDb::k3dSimplePoly, points);
	return CCreateEnt::PostToModelSpace(pPoly3d);
}

// 创建正多边形的函数。创建正多边形的输入参数为中心、边数、外接圆半径、旋转角度（弧度值）和线宽
AcDbObjectId CCreateEnt::CreatePolygon(AcGePoint2d ptCenter, int number, double radius, double rotation, double width)
{
	AcGePoint2dArray points;
	double angle = 2 * CCalculation::PI() / (double)number;
	for (int i = 0; i < number; i++)
	{
		AcGePoint2d pt;
		pt.x = ptCenter.x + radius * cos(i * angle);
		pt.y = ptCenter.y + radius * sin(i * angle);
		points.append(pt);
	}
	AcDbObjectId polyId = CCreateEnt::CreatePolyline(points, width);
	// 将其闭合
	AcDbEntity *pEnt;
	acdbOpenAcDbEntity(pEnt, polyId, AcDb::kForWrite);
	AcDbPolyline *pPoly = AcDbPolyline::cast(pEnt);
	if (pPoly != NULL)
	{
		pPoly->setClosed(Adesk::kTrue);
	}
	pEnt->close();
	CModifyEnt::Rotate(polyId, ptCenter, rotation);
	return polyId;
}

// 用于创建矩形的函数 该函数根据两个角点和线宽来创建矩形
AcDbObjectId CCreateEnt::CreateRectangle(AcGePoint2d pt1, AcGePoint2d pt2, double width)
{
	// 提取两个角点的坐标值
	double x1 = pt1.x, x2 = pt2.x;
	double y1 = pt1.y, y2 = pt2.y;

	// 计算矩形的角点
	AcGePoint2d ptLeftBottom(CCalculation::Min(x1, x2),
		CCalculation::Min(y1, y2));
	AcGePoint2d ptRightBottom(CCalculation::Max(x1, x2),
		CCalculation::Min(y1, y2));
	AcGePoint2d ptRightTop(CCalculation::Max(x1, x2),
		CCalculation::Max(y1, y2));
	AcGePoint2d ptLeftTop(CCalculation::Min(x1, x2),
		CCalculation::Max(y1, y2));

	// 创建对应的多段线
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	pPoly->addVertexAt(0, ptLeftBottom, 0, width, width);
	pPoly->addVertexAt(1, ptRightBottom, 0, width, width);
	pPoly->addVertexAt(2, ptRightTop, 0, width, width);
	pPoly->addVertexAt(3, ptLeftTop, 0, width, width);
	pPoly->setClosed(Adesk::kTrue);

	// 将多段线添加到模型空间
	AcDbObjectId polyId;
	polyId = CCreateEnt::PostToModelSpace(pPoly);
	return polyId;

}

// 创建圆的函数，输入参数包括圆心、半径和线宽
AcDbObjectId CCreateEnt::CreatePolyCircle(AcGePoint2d ptCenter, double radius, double width)
{
	// 计算顶点的位置
	AcGePoint2d pt1, pt2, pt3;
	pt1.x = ptCenter.x + radius;
	pt1.y = ptCenter.y;
	pt2.x = ptCenter.x - radius;
	pt2.y = ptCenter.y;
	pt3.x = ptCenter.x + radius;
	pt3.y = ptCenter.y;

	// 创建多段线
	AcDbPolyline *pPoly = new AcDbPolyline(3);
	pPoly->addVertexAt(0, pt1, 1, width, width);
	pPoly->addVertexAt(1, pt2, 1, width, width);
	pPoly->addVertexAt(2, pt3, 1, width, width);
	pPoly->setClosed(Adesk::kTrue);

	// 将多段线添加到模型空间
	AcDbObjectId polyId;
	polyId = CCreateEnt::PostToModelSpace(pPoly);
	return polyId;
}

// 用于创建圆弧的函数，输入参数包括圆心、半径、起始角度、终止角度和线宽
AcDbObjectId CCreateEnt::CreatePolyArc(AcGePoint2d ptCenter, double radius, double angleStart, double angleEnd, double width)
{
	// 计算顶点的位置
	AcGePoint2d pt1, pt2;
	pt1.x = ptCenter.x + radius * cos(angleStart);
	pt1.y = ptCenter.y + radius * sin(angleStart);
	pt2.x = ptCenter.x + radius * cos(angleEnd);
	pt2.y = ptCenter.y + radius * sin(angleEnd);

	// 创建多段线
	AcDbPolyline *pPoly = new AcDbPolyline(2);
	pPoly->addVertexAt(0, pt1, tan((angleEnd - angleStart) / 4), width, width);
	pPoly->addVertexAt(0, pt1, tan((angleEnd - angleStart) / 4), width, width);

	// 将多段线添加到模型空间
	AcDbObjectId polyId = CCreateEnt::PostToModelSpace(pPoly);
	return polyId;

}

// 通过创建椭圆的默认构造函数创建椭圆
AcDbObjectId CCreateEnt::CreateEllipse(const AcGePoint3d& center, const AcGeVector3d& unitNormal, const AcGeVector3d& majorAxis, double radiusRatio)
{
	AcDbEllipse *pEllipse = new AcDbEllipse(center, unitNormal, majorAxis, radiusRatio);
	return CCreateEnt::PostToModelSpace(pEllipse);
}

// 添加根据外接矩形创建椭圆的函数，输入参数为外接矩形的两个角点
AcDbObjectId CCreateEnt::CreateEllipse(AcGePoint2d pt1, AcGePoint2d pt2)
{
	AcGePoint3d ptCenter = CCalculation::MiddlePoint(CCalculation::Pt2dTo3d(pt1), CCalculation::Pt2dTo3d(pt2));
	AcGeVector3d vecNormal(0, 0, 1);
	AcGeVector3d majorAxis(fabs(pt1.x - pt2.x) / 2, 0, 0);
	double radio = fabs((pt1.y - pt2.y) / (pt1.x, pt2.x));
	return CCreateEnt::CreateEllipse(ptCenter, vecNormal, majorAxis, radio);
}

// 添加创建样条曲线的函数，输入参数包括样条曲线的拟合点、拟合曲线的阶数和允许的拟合误差
AcDbObjectId CCreateEnt::CreateSpline(const AcGePoint3dArray& points, int order, double fitTolerance)
{
	assert(order >= 2 && order <= 26);
	AcDbSpline *pSpline = new AcDbSpline(points, order, fitTolerance);
	return CCreateEnt::PostToModelSpace(pSpline);
}

// 指定样条曲线起点和终点的切线方向
AcDbObjectId CCreateEnt::CreateSpline(const AcGePoint3dArray& points, const AcGeVector3d& startTangent, const AcGeVector3d &endTangent, int order, double fitTolerance)
{
	assert(order >= 2 && order <= 26);
	AcDbSpline *pSpline = new AcDbSpline(points, startTangent, endTangent, order, fitTolerance);
	return CCreateEnt::PostToModelSpace(pSpline);
}

// 改变实体的颜色
Acad::ErrorStatus CModifyEnt::__ChangeColor(AcDbObjectId entId, Adesk::UInt16
	colorIndex)
{
	AcDbEntity *pEntity;
	// 打开图形数据库中的对象
	acdbOpenObject(pEntity, entId, AcDb::kForWrite);
	// 修改实体的颜色
	pEntity->setColorIndex(colorIndex);
	// 使用完之后，及时关闭
	pEntity->close();
	return Acad::eOk;
}

// 改变实体的图层
Acad::ErrorStatus CModifyEnt::ChangeLayer(AcDbObjectId entId, CString strLayerName)
{
	AcDbEntity *pEntity;
	// 打开图形数据库对象
	acdbOpenObject(pEntity, entId, AcDb::kForWrite);

	// 修改实体的图层
	pEntity->setLayer(strLayerName);

	// 用完之后，及时关闭
	pEntity->close();
	return Acad::eOk;
}

// 改变实体的线型
Acad::ErrorStatus CModifyEnt::ChangeLineType(AcDbObjectId entId, CString strLineType)
{
	AcDbEntity *pEntity;
	// 打开图形数据库中的对象
	acdbOpenObject(pEntity, entId, AcDb::kForWrite);
	// 修改实体的线型
	pEntity->setLayer(strLineType);
	pEntity->close();

	return Acad::eOk;
}

// 计算平面两点的中点
AcGePoint2d CCalculation::MiddlePoint(AcGePoint2d pt1, AcGePoint2d pt2)
{
	AcGePoint2d pt;
	pt[X] = pt1[X] + (pt2[X] - pt1[X]) / 2;
	pt[Y] = pt1[Y] + (pt2[Y] - pt1[Y]) / 2;
	return pt;
}

// 计算立体面两点的中点
AcGePoint3d CCalculation::MiddlePoint(AcGePoint3d pt1, AcGePoint3d pt2)
{
	AcGePoint3d pt;
	pt[X] = pt1[X] + (pt2[X] - pt1[X]) / 2;
	pt[Y] = pt1[Y] + (pt2[Y] - pt1[Y]) / 2;
	pt[Z] = pt1[Z] + (pt2[Z] - pt1[Z]) / 2;
	return pt;
}

// 将平面的点转成立体面的点
AcGePoint3d CCalculation::Pt2dTo3d(AcGePoint2d pt)
{
	AcGePoint3d pTemp(pt.x, pt.y, 0);
	return pTemp;
}

// 计算PI的值
double CCalculation::PI()
{
	return 4 * atan(1.0);
}

double CCalculation::Max(double a, double b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

double CCalculation::Min(double a, double b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

// 弧度转成角度
double CCalculation::RtoG(double angle)
{
	return angle * 180 / CCalculation::PI();
}
// 角度转成弧度
double CCalculation::GtoR(double angle)
{
	return angle * CCalculation::PI() / 180;
}

// 旋转实体
Acad::ErrorStatus CModifyEnt::Rotate(AcDbObjectId entId, AcGePoint2d ptBase, double rotation)
{
	AcGeMatrix3d xform;
	AcGeVector3d vec(0, 0, 1);
	xform.setToRotation(rotation, vec, CCalculation::Pt2dTo3d(ptBase));
	AcDbEntity *pEnt;
	Acad::ErrorStatus es = acdbOpenObject(pEnt, entId, AcDb::kForWrite, false);
	pEnt->transformBy(xform);
	pEnt->close();
	return es;
}

// 移动实体
Acad::ErrorStatus CModifyEnt::Move(AcDbObjectId entId, AcGePoint3d ptBase, AcGePoint3d ptDest)
{
	AcGeMatrix3d xform;
	AcGeVector3d vec(ptDest.x - ptBase.x, ptDest.y - ptBase.y, ptDest.z - ptBase.z);
	xform.setToTranslation(vec);

	AcDbEntity *pEnt;
	Acad::ErrorStatus  es = acdbOpenObject(pEnt, entId, AcDb::kForWrite);
	pEnt->transformBy(xform);
	pEnt->close();
	return es;
}

// 缩放实体
Acad::ErrorStatus CModifyEnt::Scale(AcDbObjectId entId, AcGePoint3d ptBase, double scaleFactor)
{
	// 设置变换矩阵的参数
	AcGeMatrix3d xform;
	xform.setToScaling(scaleFactor, ptBase);

	AcDbEntity *pEnt;
	Acad::ErrorStatus es = acdbOpenObject(pEnt, entId, AcDb::kForWrite, false);
	pEnt->transformBy(xform);
	pEnt->close();
	return es;

}

// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 声明创建修改实体的颜色的执行函数
void ChangeColor();

// 声明创建一条线段的颜色的执行函数
void AddLine();

// 声明创建圆的函数
void AddCircle();

// 声明创建圆弧的函数
void AddArc();

// 声明一个创建多线段的函数
void AddPolyline();

// 声明一个创建椭圆的函数
void AddEllipse();

// 声明一个用于测试创建样条曲线的函数
void AddSpline();

// 加载应用程序时被调用的函数
void initApp()
{
	// 添加一个修改线段颜色命令
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("ChangeCorlor"), _T("ChangeCorlor"), ACRX_CMD_MODAL, ChangeColor);

	// 添加一个新建线段颜色命令
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddLine"), _T("AddLine"), ACRX_CMD_MODAL, AddLine);
	// 添加一个创建圆的函数
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddCircle"), _T("AddCircle"), ACRX_CMD_MODAL, AddCircle);
	// 添加一个画圆弧的函数
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddArc"), _T("AddArc"), ACRX_CMD_MODAL, AddArc);

	// 添加一个创建多线段的命令
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddPolyline"), _T("AddPolyline"), ACRX_CMD_MODAL, AddPolyline);

	// 添加一个用于测试创建椭圆的命令
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddEllipse"), _T("AddEllipse"), ACRX_CMD_MODAL, AddEllipse);

	// 添加一个用于测试创建样条曲线的命令
	acedRegCmds->addCommand(_T("CreateGrapics"), _T("AddSpline"), _T("AddSpline"), ACRX_CMD_MODAL, AddSpline);

}

// 卸载应用程序时被调用的函数
void unloadApp()
{
	// 删除命令组
	acedRegCmds->removeGroup(_T("CreateGrapics"));
}

//实现创建修改实体的颜色的执行函数
void ChangeColor()
{
	AcDbObjectId lineId = CCreateEnt::CreateLine();
	CModifyEnt::__ChangeColor(lineId, 1);
}
// 实现创建一条线段的颜色的执行函数
void AddLine()
{
	AcGePoint3d ptStart(0, 0, 0);
	AcGePoint3d ptEnd(200, 200, 0);
	AcDbObjectId lineId = CCreateEnt::CreateLine(ptStart, ptEnd);
	CModifyEnt::__ChangeColor(lineId, 1);
	CModifyEnt::ChangeLayer(lineId, _T("虚线"));
	CModifyEnt::ChangeLineType(lineId, _T("中心线"));
}

// 实现创建圆的函数
void AddCircle()
{
	// “圆心、半径”法创建一个圆
	AcGePoint3d ptCenter(100, 100, 0);
	CCreateEnt::CreateCircle(ptCenter, 20);

	// 两点法创建一个圆
	AcGePoint2d pt1(70, 100);
	AcGePoint2d pt2(130, 100);
	CCreateEnt::CreateCircle(pt1, pt2);

	// 三点法创建一个圆
	pt1.set(60, 100);
	pt2.set(140, 100);
	AcGePoint2d pt3(100, 60);
	CCreateEnt::CreateCircle(pt1, pt2, pt3);
}

// 定义创建圆弧的函数
void AddArc()
{
	// 创建位于XOY平面上的圆弧
	AcGePoint2d ptCenter(50, 50);
	CCreateEnt::CreateArc(ptCenter, 100 * sqrt(2.0) / 2, 5 * CCalculation::PI() / 4, 7 * CCalculation::PI() / 4);
	// 三点法创建圆弧
	AcGePoint2d ptStart(100, 0);
	AcGePoint2d ptOnArc(120, 50);
	AcGePoint2d ptEnd(100, 100);
	CCreateEnt::CreateArc(ptStart, ptOnArc, ptEnd);
	// “起点、圆心、终点”创建圆弧
	ptStart.set(100, 100);
	ptCenter.set(50, 50);
	ptEnd.set(0, 100);
	CCreateEnt::CreateArcSCE(ptStart, ptCenter, ptEnd);
	// “起点、圆心、圆弧角度”创建圆弧
	ptStart.set(0, 100);
	ptCenter.set(50, 50);
	CCreateEnt::CreateArc(ptStart, ptCenter, CCalculation::PI() / 2);
}

// 实现一个创建多线段的函数
void AddPolyline()
{
	// 创建仅包含一段直线的多段线
	AcGePoint2d ptStart(0, 0), ptEnd(100, 100);
	CCreateEnt::CreatePolyline(ptStart, ptEnd, 1);
	// 创建三维多段线
	AcGePoint3d pt1(0, 0, 0), pt2(100, 0, 0), pt3(100, 100, 0);
	AcGePoint3dArray points;
	points.append(pt1);
	points.append(pt2);
	points.append(pt3);
	CCreateEnt::Create3dPolyline(points);

	// 创建正多边形
	CCreateEnt::CreatePolygon(AcGePoint2d::kOrigin, 6, 30, 0, 1);
	// 创建矩形
	AcGePoint2d pt(60, 70);
	CCreateEnt::CreateRectangle(pt, ptEnd, 1);
	// 创建圆
	pt.set(50, 50);
	CCreateEnt::CreatePolyCircle(pt, 30, 1);
	// 创建圆弧
	CCreateEnt::CreatePolyArc(pt, 50, CCalculation::GtoR(45),
		CCalculation::GtoR(225), 1);
}

// 实现一个创建椭圆的函数
void AddEllipse()
{
	// 使用中心点、所在平面、长轴矢量和短长轴比例来创建椭圆
	AcGeVector3d vecNormal(0, 0, 1);
	AcGeVector3d majorAxis(40, 0, 0);
	AcDbObjectId entId;
	// AcGePoint3d::kOrigin 返回点（0，0，0）
	entId = CCreateEnt::CreateEllipse(AcGePoint3d::kOrigin, vecNormal, majorAxis, 0.5);

	// 使用外接矩形来创建椭圆
	AcGePoint2d pt1(60, 80), pt2(140, 120);
	CCreateEnt::CreateEllipse(pt1, pt2);
}

// 声明一个用于测试创建样条曲线的函数
void AddSpline()
{
	// 使用样本点直接创建样条曲线
	AcGePoint3d pt1(0, 0, 0), pt2(10, 30, 0), pt3(60, 80, 0), pt4(100, 100,0);
	AcGePoint3dArray points;
	points.append(pt1);
	points.append(pt2);
	points.append(pt3);
	points.append(pt4);
	CCreateEnt::CreateSpline(points);
	// 指定起始点和终止点的切线方向，创建样条曲线
	pt2.set(30, 10, 0);
	pt3.set(80, 60, 0);
	points.removeSubArray(0, 3);
	points.append(pt1);
	points.append(pt2);
	points.append(pt3);
	points.append(pt4);
	AcGeVector3d startTangent(5, 1, 0);
	AcGeVector3d endTangent(5, 1, 0);
	CCreateEnt::CreateSpline(points, startTangent, endTangent);
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
