#include "stdafx.h"
// 声明初始化函数和卸载函数
void initApp();
void unloadApp();

// 提示用户选择一个实体，并将一些附加的数据保存到该实体的扩展字典中
void AddXRecord();

void ViewXRecord();

//	向当前的图形数据库中添加一个字典
void AddNameDict();

void ViewNameDict();

// 提示用户选择几个实体，将其创建成一个编组
void AddGroup();

// 用于删除图形中的编组“MyGroup”
void DelGroup();

// 声明一个创建组的函数
void CreateGroup(AcDbObjectIdArray &objIds, wchar_t *pGroupName);

// 用于添加一个新的多线样式
void AddMlStyle();

//	用于删除 AddMlStyle 命令创建的多线样式
void DelMlStyle();

// 根据输入的线型名称获得其 ObjectId
Acad::ErrorStatus GetLinetypeId(const wchar_t *linetype,AcDbObjectId &linetypeId);

// 加载应用程序时被调用的函数
void initApp()
{
	// 扩展记录
	acedRegCmds->addCommand(_T("XRecord"), _T("AddXRecord"), _T("AddXRecord"), ACRX_CMD_MODAL, AddXRecord);
	acedRegCmds->addCommand(_T("XRecord"), _T("ViewXRecord"), _T("ViewXRecord"), ACRX_CMD_MODAL, ViewXRecord);
	acedRegCmds->addCommand(_T("XRecord"), _T("AddNameDict"), _T("AddNameDict"), ACRX_CMD_MODAL, AddNameDict);
	acedRegCmds->addCommand(_T("XRecord"), _T("ViewNameDict"), _T("ViewNameDict"), ACRX_CMD_MODAL, ViewNameDict);

	// 编组
	acedRegCmds->addCommand(_T("Group"), _T("AddGroup"), _T("AddGroup"), ACRX_CMD_MODAL, AddGroup);
	acedRegCmds->addCommand(_T("Group"), _T("DelGroup"), _T("DelGroup"), ACRX_CMD_MODAL, DelGroup);

	// 多线样式
	acedRegCmds->addCommand(_T("MlineStyle"), _T("AddMlStyle"), _T("AddMlStyle"), ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, AddMlStyle);
	acedRegCmds->addCommand(_T("MlineStyle"), _T("DelMlStyle"), _T("DelMlStyle"), ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, DelMlStyle);

	// 在命令窗口中提示注册命令
	acutPrintf(_T("\n注册AddMlStyle命令,用于创建新的多线样式"));
	acutPrintf(_T("\n注册DelMlStyle命令,用于删除新的多线样式"));
}
// 卸载应用程序时被调用的函数
void unloadApp()
{
	acedRegCmds->removeGroup(_T("XRecord"));
	acedRegCmds->removeGroup(_T("Group"));
}

// 提示用户选择一个实体，并将一些附加的数据保存到该实体的扩展字典中
void AddXRecord()
{
	// 提示用户选择所要添加扩展记录的图形对象
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n选择所要添加扩展记录的实体："), en, pt) != RTNORM)
	{
		return;
	}

	AcDbObjectId entId; // 要添加扩展记录的实体ID
	Acad::ErrorStatus es = acdbGetObjectId(entId, en);

	AcDbXrecord *pXrec = new AcDbXrecord;
	AcDbObject *pObj;
	AcDbObjectId dictObjId, xRecObjId;
	AcDbDictionary *pDict;

	// 要在记录中保存的字符串
	wchar_t entType[] = { _T("直线") };
	struct resbuf *pRb;

	// 向实体中添加扩展字典
	acdbOpenObject(pObj, entId, AcDb::kForWrite);
	pObj->createExtensionDictionary();
	dictObjId = pObj->extensionDictionary();
	pObj->close();

	// 想扩展字典中添加一条记录
	acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
	pDict->setAt(_T("XRecord"), pXrec, xRecObjId);
	pDict->close();

	// 设置扩展记录的内容
	pRb = acutBuildList(AcDb::kDxfText, entType,
		AcDb::kDxfInt32, 12,
		AcDb::kDxfReal, 3.14,
		AcDb::kDxfXCoord, pt,
		RTNONE);

	pXrec->setFromRbChain(*pRb);
	pXrec->close();

	acutRelRb(pRb);
}

void ViewXRecord()
{
	// 提示用户选择所要查看扩展记录的图形对象
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n选择所要查看扩展记录的实体："), en, pt) != RTNORM)
		return;

	AcDbObjectId entId;
	Acad::ErrorStatus es = acdbGetObjectId(entId, en);

	// 打开图形对象，获得肢体扩展字典的ObjectId
	AcDbEntity *pEnt;
	acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead);
	AcDbObjectId dictObjId = pEnt->extensionDictionary();
	pEnt->close();

	// 查看实体是否包含扩展字典
	if (dictObjId == AcDbObjectId::kNull)
	{
		acutPrintf(_T("\n做选择的实体不包含扩展字典！"));
		return;
	}

	// 打开扩展字典，获得与关键字“XRecord”关联的扩展记录
	AcDbDictionary *pDict;
	AcDbXrecord *pXrec;
	acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
	pDict->getAt(_T("XRecord"), (AcDbObject *&)pXrec, AcDb::kForRead);
	pDict->close();

	// 获得扩展记录的数据链表并关闭扩展数据对象
	struct resbuf *pRb;
	pXrec->rbChain(&pRb);
	pXrec->close();

	if (pRb != NULL)
	{
		// 在命令行显示扩展记录内容
		struct resbuf *pTemp;
		pTemp = pRb;

		acutPrintf(_T("\n字符串类型的扩展数据是：%s"), pTemp->resval.rstring);

		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n整数类型的扩展数据是：%d"), pTemp->resval.rint);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n实数类型的扩展数据是：%.2f"),pTemp->resval.rreal);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n点坐标类型的扩展数据是：(%.2f, %.2f, %.2f)"),pTemp->resval.rpoint[X], pTemp->resval.rpoint[Y],pTemp->resval.rpoint[Z]);
		acutRelRb(pRb);
	}
}

//	向当前的图形数据库中添加一个字典
void AddNameDict()
{
	// 要在扩展记录中保存的字符串
	wchar_t entType[] = { _T("直线") };
	struct resbuf *pRb;
	// 获得有名对象字典，向其中添加指定的字典项
	AcDbDictionary *pNameObjDict, *pDict;
	acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNameObjDict, AcDb::kForWrite);

	// 检查所要添加的字典项是否已经存在
	AcDbObjectId dictObjId;
	if (pNameObjDict->getAt(_T("MyDict"), (AcDbObject *&)pDict, AcDb::kForWrite) == Acad::eKeyNotFound)
	{
		pDict = new AcDbDictionary;
		pNameObjDict->setAt(_T("MyDict"), pDict, dictObjId);
		pDict->close();
	}
	pNameObjDict->close();

	// 向新建的字典中添加一个扩展记录
	AcDbXrecord *pXrec = new AcDbXrecord;
	AcDbObjectId xrecObjId;
	acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
	pDict->setAt(_T("XRecord"), pXrec, xrecObjId);
	pDict->close();

	// 设置扩展记录的内容
	ads_point pt;
	pt[X] = 100;
	pt[Y] = 100;
	pt[Z] = 0;
	pRb = acutBuildList(AcDb::kDxfText, entType,
		AcDb::kDxfInt32, 12,
		AcDb::kDxfReal, 3.14,
		AcDb::kDxfXCoord, pt,
		RTNONE);
	pXrec->setFromRbChain(*pRb);
	pXrec->close();
	acutRelRb(pRb);
}

void ViewNameDict()
{
	// 获得对象有名字典中指定的字典项
	AcDbDictionary *pNameObjDict, *pDict;
	Acad::ErrorStatus es;
	acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNameObjDict, AcDb::kForRead);

	es = pNameObjDict->getAt(_T("MyDict"), (AcDbObject *&)pDict, AcDb::kForRead);
	pNameObjDict->close();

	// 如果不存在指定的字典项，退出程序
	if (es == Acad::eKeyNotFound)
	{
		return;
	}

	// 获得指定的对象字典
	AcDbXrecord *pXrec;
	pDict->getAt(_T("XRecord"), (AcDbObject *&)pXrec, AcDb::kForRead);
	pDict->close();

	// 获得扩展记录的数据链表并关闭扩展数据对象
	struct resbuf *pRb;
	pXrec->rbChain(&pRb);
	pXrec->close();

	if (pRb != NULL)
	{
		// 在命令行显示扩展记录内容
		struct resbuf *pTemp;
		pTemp = pRb;
		acutPrintf(_T("\n字符串类型的扩展数据是：%s"),
			pTemp->resval.rstring);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n整数类型的扩展数据是：%d"), pTemp->resval.rint);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n实数类型的扩展数据是：%.2f"),
			pTemp->resval.rreal);
		pTemp = pTemp->rbnext;
		acutPrintf(_T("\n点坐标类型的扩展数据是：(%.2f, %.2f, %.2f)"),
			pTemp->resval.rpoint[X], pTemp->resval.rpoint[Y],
			pTemp->resval.rpoint[Z]);
		acutRelRb(pRb);
	}
}

// 提示用户选择几个实体，将其创建成一个编组
void AddGroup()
{
	// 提示用户选择多个实体
	ads_name sset;
	acutPrintf(_T("\n选择要成组的实体："));
	if (acedSSGet(NULL, NULL, NULL, NULL, sset) != RTNORM)
		return;

	long length;
	acedSSLength(sset, &length);
	AcDbObjectIdArray objIds;
	for (long i = 0; i < length; i++)
	{
		// 获得指定元素的ObjectId
		ads_name ent;
		acedSSName(sset, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);

		// 获得指向当前元素的指针
		AcDbEntity *pEnt;
		acdbOpenObject(pEnt, objId, AcDb::kForRead);
		objIds.append(pEnt->objectId());

		pEnt->close();
	}

	acedSSFree(sset);

	// 创建组
	wchar_t groupName[] = { _T("MyGroup") };
	CreateGroup(objIds, groupName);
}

// 用于删除图形中的编组“MyGroup”
void DelGroup()
{
	// 获得组字典
	AcDbDictionary *pGroupDict;
	acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pGroupDict, AcDb::kForWrite);

	if (pGroupDict->has(_T("MyGroup")))
	{
		pGroupDict->remove(_T("MyGroup"));
	}
	pGroupDict->close();
}



// 定义一个创建组的函数
void CreateGroup(AcDbObjectIdArray &objIds, wchar_t *pGroupName)
{
	AcDbGroup *pGroup = new AcDbGroup(pGroupName);
	for (int i = 0; i < objIds.length(); i++)
	{
		pGroup->append(objIds[i]);
	}

	// 将组添加到有名对象字典的组字典中
	AcDbDictionary *pGroupDict;
	acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pGroupDict, AcDb::kForWrite);

	AcDbObjectId pGroupId;
	pGroupDict->setAt(pGroupName, pGroup, pGroupId);
	pGroupDict->close();
	pGroup->close();
}


// 用于添加一个新的多线样式
void AddMlStyle()
{
	// 加载线型（两种方法）
	Acad::ErrorStatus es;
	es = acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile(_T("CENTER"), _T("acadiso.lin"));
	es = acdbLoadLineTypeFile(_T("HIDDEN"), _T("acadiso.lin"), acdbHostApplicationServices()->workingDatabase());

	// 创建新的AcDbMlineStyle对象
	AcDbMlineStyle *pMlStyle = new AcDbMlineStyle;
	pMlStyle->initMlineStyle();
	pMlStyle->setName(_T("NewStyle"));

	int index; // 多线样式中的元素索引
	AcCmColor color;	// 颜色
	AcDbObjectId linetypeId; // 线型的ID

	// 添加第一个元素（红色的中心线）
	color.setColorIndex(1); // 红色
	GetLinetypeId(_T("CENTER"), linetypeId);
	pMlStyle->addElement(index, 0, color, linetypeId);

	// 添加第二个元素（蓝色的虚线）
	color.setColorIndex(5); // 蓝色
	GetLinetypeId(_T("HIDDEN"), linetypeId);
	pMlStyle->addElement(index, 0.5, color, linetypeId);
	// 添加第三个元素（蓝色的虚线）
	pMlStyle->addElement(index, -0.5, color, linetypeId);
	// 将多线样式添加到多线样式字典中
	AcDbDictionary *pDict;
	acdbHostApplicationServices()->workingDatabase()
		->getMLStyleDictionary(pDict, AcDb::kForWrite);
	AcDbObjectId mlStyleId;
	es = pDict->setAt(_T("NewStyle"), pMlStyle, mlStyleId);
	pDict->close();
	pMlStyle->close();
}

//	用于删除 AddMlStyle 命令创建的多线样式
void DelMlStyle()
{
	// 获得多线样式字典
	AcDbDictionary *pDict;
	acdbHostApplicationServices()->workingDatabase()->getMLStyleDictionary(pDict, AcDb::kForWrite);

	if (pDict->has(_T("NewStyle")))
	{
		pDict->remove(_T("NewStyle"));
	}
	pDict->close();
}

// 根据输入的线型名称获得其 ObjectId
Acad::ErrorStatus GetLinetypeId(const wchar_t *linetype, AcDbObjectId &linetypeId)
{
	AcDbLinetypeTable *pLtpTbl;
	acdbHostApplicationServices()->workingDatabase()->getLinetypeTable(pLtpTbl, AcDb::kForRead);

	if (!pLtpTbl->has(linetype))
	{
		pLtpTbl->close();
		return Acad::eBadLinetypeName;
	}

	pLtpTbl->getAt(linetype, linetypeId);
	pLtpTbl->close();

	return Acad::eOk;
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