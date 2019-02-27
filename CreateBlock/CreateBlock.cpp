#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();
// 声明命令的执行函数
void HelloWorld();

// 声明创建块定义的函数
void MakeBlkDef();

// 声明插入块的函数
void InsertBlk();

// 声明一个创建带有属性的块定义的函数
void MakeBlkWithAttribute();

// 声明一个插入带有属性的块参照的函数
void InsertBlkWithAttribute();

// 加载应用程序时被调用的函数
void initApp()
{
	//使用AutoCAD命令机制注册一个新命令
	acedRegCmds->addCommand(_T("Hello1"),
		_T("Hello"),  //输入这两个命令名称均可以在
		_T("Hello"),  //AutoCAD中运行该程序
		ACRX_CMD_MODAL,
		HelloWorld);
	// 添加一个创建块定义的命令
	acedRegCmds->addCommand(_T("MakeBlkDefGroup"), _T("MakeBlkDef"), _T("MakeBlkDef"), ACRX_CMD_MODAL, MakeBlkDef);
	
	// 添加一个插入块的命令
	acedRegCmds->addCommand(_T("InsertBlkGroup"), _T("InsertBlk"), _T("InsertBlk"), ACRX_CMD_MODAL, InsertBlk);

	// 添加一个创建带有属性的快定义的命令
	acedRegCmds->addCommand(_T("MakeBlkDefGroup"), _T("MakeBlkWithAttribute"), _T("MakeBlkWithAttribute"), ACRX_CMD_MODAL, MakeBlkWithAttribute);

	// 添加一个插入带有属性的块参照
	acedRegCmds->addCommand(_T("MakeBlkDefGroup"), _T("InsertBlkWithAttribute"), _T("InsertBlkWithAttribute"), ACRX_CMD_MODAL, InsertBlkWithAttribute);
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	//删除命令组
	acedRegCmds->removeGroup(_T("Hello1"));
	acedRegCmds->removeGroup(_T(" MakeBlkDefGroup"));
}
// 实现Hello命令的函数
void HelloWorld()
{
	acutPrintf(_T("Hello, World!"));
}

// 实现创建块定义的函数
void MakeBlkDef()
{
	// 1. 获得当前图形数据库的块表
	AcDbBlockTable *pBlkTbl;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl, AcDb::kForWrite);

	// 2. 创建新的块表记录
	AcDbBlockTableRecord *pBlkTblRcd = new AcDbBlockTableRecord();

	// 3. 根据用户的输入设置块表记录的名称
	wchar_t blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入块表的名称："), blkName) != RTNORM)
	{
		pBlkTbl->close();
		delete pBlkTblRcd;
		return;
	}
	pBlkTblRcd->setName(blkName);

	// 将块表记录添加到块表中
	AcDbObjectId blkDefId;
	pBlkTbl->add(blkDefId, pBlkTblRcd);
	pBlkTbl->close();

	// 向块表记录中添加实体
	AcGePoint3d ptStart(-10, 0, 0), ptEnd(10, 0, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd); // 创建一条直线
	ptStart.set(0, -10, 0);
	ptEnd.set(0, 10, 0);
	AcDbLine *pLine2 = new AcDbLine(ptStart, ptEnd); //创建一条直线
	AcGeVector3d vecNormal(0, 0, 1);
	AcDbCircle *pCircle = new AcDbCircle(AcGePoint3d::kOrigin, vecNormal, 6); // 创建一个圆

	AcDbObjectId endId;
	pBlkTblRcd->appendAcDbEntity(endId, pLine);
	pBlkTblRcd->appendAcDbEntity(endId, pLine2);
	pBlkTblRcd->appendAcDbEntity(endId, pCircle);

	// 关闭实体和块表记录
	pLine->close();
	pLine2->close();
	pCircle->close();
	pBlkTblRcd->close();
	pBlkTbl->close();
}

// 实现插入块的函数
void InsertBlk()
{
	// 获得用户输入的块定义名称
	wchar_t blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入图块的名称："), blkName) != RTNORM)
	{
		acutPrintf(blkName);
		return;
	}

	// 获取当前数据库的块表
	AcDbBlockTable *pBlkTbl;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl, AcDb::kForWrite);

	// 查找用户指定的块定义是否存在
	CString strBlkDef;
	strBlkDef.Format(_T("%s"), blkName);
	if (!pBlkTbl->has(strBlkDef))
	{
		acutPrintf(_T("\n当前图形中未包含指定名称的块定义"));
		pBlkTbl->close();
		return;
	}
	// 获得用户输入的块参照的插入点
	ads_point pt;
	if (acedGetPoint(NULL, _T("\n输入块参照的插入点: "), pt) != RTNORM)
	{
		pBlkTbl->close();
		return;
	}

	AcGePoint3d ptInsert = asPnt3d(pt);

	// 获得用户指定的块表记录
	AcDbObjectId blkDefId;
	pBlkTbl->getAt(strBlkDef, blkDefId);

	// 创建块参照对象
	AcDbBlockReference *pBlkRef = new AcDbBlockReference(ptInsert, blkDefId);

	// 将块参照添加到模型空间
	AcDbBlockTableRecord *pBlkTblRcd;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,AcDb::kForWrite);
	AcDbObjectId entId;
	pBlkTblRcd->appendAcDbEntity(entId, pBlkRef);

	// 关闭数据库的对象
	pBlkRef->close();
	pBlkTblRcd->close();
	pBlkTbl->close();
}

// 声明一个创建带有属性的快定义的函数
void MakeBlkWithAttribute()
{
	// 获得当前图形数据库的块表
	AcDbBlockTable *pBlkTbl;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl,AcDb::kForWrite);

	// 创建新的块表记录
	AcDbBlockTableRecord *pBlkTblRcd = new AcDbBlockTableRecord();

	// 根据用户输入设置块表记录的名称
	wchar_t blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入图块的名称： "), blkName) != RTNORM)
	{
		acutPrintf(_T("您输入的图块名已存在"));
		pBlkTblRcd->close();
		pBlkTbl->close();
		return;
	}
	pBlkTblRcd->setName(blkName);

	// 将块表记录添加到块表中
	AcDbObjectId blkDefId;
	pBlkTbl->add(blkDefId, pBlkTblRcd);
	pBlkTbl->close();

	// 向标快记录中添加实体
	AcGePoint3d ptStart(-10, 0, 0), ptEnd(10, 0, 0);
	AcDbLine *pLine1 = new AcDbLine(ptStart, ptEnd); // 创建一条直线
	ptStart.set(0, -10, 0);
	ptEnd.set(0, 10, 0);
	AcDbLine *pLine2 = new AcDbLine(ptStart, ptEnd); // 创建一条直线
	AcGeVector3d vecNormal(0, 0, 1);
	AcDbCircle *pCircle = new AcDbCircle(AcGePoint3d::kOrigin, vecNormal, 6);

	// 创建一个属性，输入直径
	AcDbAttributeDefinition *pAttDef = new AcDbAttributeDefinition(ptEnd, _T("20"), _T("直径"), _T("输入直径"));
	AcDbObjectId entId;
	pBlkTblRcd->appendAcDbEntity(entId, pLine1);
	pBlkTblRcd->appendAcDbEntity(entId, pLine2);
	pBlkTblRcd->appendAcDbEntity(entId, pCircle);
	pBlkTblRcd->appendAcDbEntity(entId, pAttDef);

	// 关闭实体和块表记录
	pLine1->close();
	pLine2->close();
	pCircle->close();
	pAttDef->close();
	pBlkTblRcd->close();
}

// 定义一个插入带有属性的块参照的函数
void InsertBlkWithAttribute()
{
	// 获得用户输入的块定义名称 
	wchar_t blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入图块的名称： "), blkName) != RTNORM)
	{
		return;
	}

	// 获得当前数据库的块表
	AcDbBlockTable *pBlkTbl;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl,AcDb::kForWrite);

	// 查找用户指定的块定义是否存在
	CString strBlkDef;
	strBlkDef.Format(_T("%s"), blkName);
	if (!pBlkTbl->has(blkName))
	{
		acutPrintf(_T("\n当前图形中未包含指定名称的块定义！"));
		pBlkTbl->close();
		return;
	}

	// 获得用户输入的块参照的插入点
	ads_point pt;
	if (acedGetPoint(NULL, _T("\n输入块参照的插入点： "), pt) != RTNORM)
	{
		pBlkTbl->close();
		return;
	}
	AcGePoint3d ptInsert = asPnt3d(pt);

	// 获得用户指定的块表记录
	AcDbObjectId blkDefId;
	pBlkTbl->getAt(strBlkDef, blkDefId);

	// 创建块参照对象
	AcDbBlockReference *pBlkRef = new AcDbBlockReference();

	// 将块参照添加到模型空间
	AcDbBlockTableRecord *pBlkTblRcd;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForWrite);
	pBlkTbl->close();
	
	AcDbObjectId entId;
	pBlkTblRcd->appendAcDbEntity(entId, pBlkRef);

	// 判断指定的块表记录是否包含属性定义
	AcDbBlockTableRecord *pBlkDefRcd;
	acdbOpenObject(pBlkDefRcd, blkDefId, AcDb::kForRead);
	if (pBlkDefRcd->hasAttributeDefinitions())
	{
		AcDbBlockTableRecordIterator *pItr;
		pBlkDefRcd->newIterator(pItr);
		AcDbEntity *pEnt;
		for (pItr->start(); !pItr->done(); pItr->step())
		{
			pItr->getEntity(pEnt, AcDb::kForRead);
			// 检查是否是属性定义
			AcDbAttributeDefinition *pAttDef;
			pAttDef = AcDbAttributeDefinition::cast(pEnt);
			if (pAttDef != NULL)
			{
				// 创建一个新的属性对象
				AcDbAttribute *pAtt = new AcDbAttribute();
				// 从属性定义获得属性对象的对象特性
				pAtt->setPropertiesFrom(pAttDef);
				// 设置属性对象的其他特性
				pAtt->setInvisible(pAttDef->isInvisible());
				AcGePoint3d ptBase = pAttDef->position();
				ptBase += pBlkRef->position().asVector();
				pAtt->setPosition(ptBase);
				pAtt->setHeight(pAttDef->height());
				pAtt->setRotation(pAttDef->rotation());
				// 获得属性对象的Tag、Prompt和TextString
				wchar_t *pStr;
				pStr = pAttDef->tag();
				pAtt->setTag(pStr);
				free(pStr);
				pStr = pAttDef->prompt();
				acutPrintf(_T("%s%s"), _T("\n"), pStr);
				free(pStr);
				pAtt->setFieldLength(30);
				pAtt->setTextString(_T("40"));
				// 向块参照追加属性对象
				pBlkRef->appendAttribute(pAtt);
				pAtt->close();
			}
			pEnt->close();
		}
		delete pItr;
	}
	// 关闭数据库的对象
	pBlkRef->close();
	pBlkTblRcd->close();
	pBlkDefRcd->close();
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