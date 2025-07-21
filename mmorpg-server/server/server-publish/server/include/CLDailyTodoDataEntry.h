#ifndef __CL_DAILAY_TODO_DATA_ENTRY_H__
#define __CL_DAILAY_TODO_DATA_ENTRY_H__
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

class DungeonDataEntry;
/**
*@brief 每日必做数据项
*/
class DailyTodoItemDataEntry : public Avalon::DataEntry
{
public:
	DailyTodoItemDataEntry() {}
	~DailyTodoItemDataEntry() {}

public:
	UInt32 GetKey() const { return iD; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; }

public:
	//唯一ID
	UInt32		iD;
	//类型
	UInt8		type;
	//子类型
	UInt8		subType;
	//地下城子类型
	std::vector<UInt8>	dungeonSubTypes;
	//日推荐次数
	UInt32		dayRecommendNum;
	//周推荐天数
	UInt32		weekRecommendDayNum;
	//周几刷新
	UInt32		refreshWeek;
};


class DailyTodoItemDataEntryMgr : public Avalon::DataEntryMgrBase<DailyTodoItemDataEntry>
	, public Avalon::Singleton<DailyTodoItemDataEntryMgr>
{
public:
	typedef std::map<UInt8, DailyTodoItemDataEntry*> DataEntryMap;
	typedef std::map<UInt8, UInt8> DungTpMapDailyTp;
	bool AddEntry(DailyTodoItemDataEntry* dataEntry);

	UInt8 DungeonTypeToDailyToDoSubType(const DungeonDataEntry* dungeonData);
	DailyTodoItemDataEntry*	GetEntryBySubType(UInt8 subType);
	DailyTodoItemDataEntry* GetEntryByDungeonSubType(const DungeonDataEntry* dungeonData);
private:
	DataEntryMap	 m_DataEntrys;
	DungTpMapDailyTp m_DungTpMapDailyTps;
};

#endif