#pragma once
#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief	 地牢资源数据项
*/

class LostDungeonResDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonResDataEntry();
	~LostDungeonResDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	//唯一id
	UInt32		id;
	//资源组id
	UInt32		resGroupId;
	//生成道具id
	std::vector<UInt32> items;
	//初始刷新时间
	UInt32		initRefreshTime;
	//刷新间隔
	UInt32		intervalRefreshTime;
	//地图id
	UInt32		mapId;
};

/**
*@brief 地牢资源管理器
*/
class LostDungeonResDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonResDataEntry>
	, public Avalon::Singleton<LostDungeonResDataEntryMgr>
{
public:
	LostDungeonResDataEntryMgr() {};
	~LostDungeonResDataEntryMgr() {};
public:
	bool AddEntry(LostDungeonResDataEntry* dataEntry);
	std::vector<LostDungeonResDataEntry*>& GetDatasByMapId(UInt32 mapId);

	std::map<UInt32, std::vector<LostDungeonResDataEntry*>> m_datas;

};