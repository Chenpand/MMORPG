#ifndef _CL_PLANT_OPACT_TREES_DATAENTRY_H_
#define _CL_PLANT_OPACT_TREES_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct TreeItem 
{
	TreeItem()
		:itemId(0), profic(0), weight(0)
	{
	}
	UInt32  itemId;	//树木id
	UInt32	profic;	//熟练度
	UInt32	weight;	//权重
};

/**
*@brief 种植鉴定数据项
*/
struct PlantOpActTreesDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// 唯一id
	UInt32	id;
	// 熟练度区间
	std::vector<UInt32> proficSect;
	// 树木库
	std::vector<TreeItem> trees;
};

typedef Avalon::DataEntryVisitor<PlantOpActTreesDataEntry> PlantOpActTreesDataEntryVisitor;

class PlantOpActTreesDataEntryMgr : public Avalon::DataEntryMgrBase<PlantOpActTreesDataEntry>
	, public Avalon::Singleton<PlantOpActTreesDataEntryMgr>
{
public:
	TreeItem*  RandomOneTreeItemByProf(UInt32	prof);
};


#endif
