#ifndef _CL_NEW_TITLE_DATAENTRY_H_
#define _CL_NEW_TITLE_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 头衔表数据项
*/
class NewTitleItemDataEntry : public Avalon::DataEntry
{
public:
	NewTitleItemDataEntry() {};
	~NewTitleItemDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };
	
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; };

public:
	//唯一ID
	UInt32				iD;
	//头衔拼接类型
	UInt8				nameType;
	//拥有者类型
	UInt8				ownerType;
	//类目
	UInt8				category;
	//类目参数
	UInt32				categoryParam;
	//名字
	std::string			name;
	//风格
	UInt8				style;
};

typedef Avalon::DataEntryVisitor<NewTitleItemDataEntry> NewTitleDataEntryVisitor;

class NewTitleItemDataEntryMgr : public Avalon::DataEntryMgrBase<NewTitleItemDataEntry>
	, public Avalon::Singleton<NewTitleItemDataEntryMgr>
{
public:
	UInt32 GetMasterTitleId();

	UInt32 GetGuidTitleId(UInt8 guidPost);

	UInt32 GetYbtTitleId();

	UInt32 GetYbtTitleIdForSortList(UInt32 sortRanking);
};


#endif