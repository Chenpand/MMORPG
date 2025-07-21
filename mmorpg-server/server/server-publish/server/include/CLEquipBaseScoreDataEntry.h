/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EQUIPBASESCORE_DATA_ENTRY_H__
#define __CL_EQUIPBASESCORE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
class ItemDataEntry;
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct EquipBaseScoreIndex
{
	EquipBaseScoreIndex()
	:type(0), subType(0), thirdType(0), needLevel(0)
	, color(0), color2(0), suitId(0)
	{
	}

	bool operator<(const EquipBaseScoreIndex& rhs) const
	{
		if (type != rhs.type)
			return type < rhs.type;
		if (subType != rhs.subType)
			return subType < rhs.subType;
		if (thirdType != rhs.thirdType)
			return thirdType < rhs.thirdType;
		if (needLevel != rhs.needLevel)
			return needLevel < rhs.needLevel;
		if (color != rhs.color)
			return color < rhs.color;
		if (color2 != rhs.color2)
			return color2 < rhs.color2;
		if (suitId != rhs.suitId)
			return suitId < rhs.suitId;

		return false;
	}

	// 类别
	UInt32     type;
	// 子类别
	UInt32     subType;
	// 三类别
	UInt32     thirdType;
	// 需要等级
	UInt32     needLevel;
	// 颜色
	UInt32     color;
	// 颜色2
	UInt32     color2;
	// 时装套装id
	UInt32		suitId;
};

// __CUSTOM_STRUCT_END__

class EquipBaseScoreDataEntry : public Avalon::DataEntry
{
public:
	EquipBaseScoreDataEntry();
	virtual ~EquipBaseScoreDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 类别
	UInt32                                          type;
	// 子类别
	UInt32                                          subType;
	// 三类别
	UInt32                                          thirdType;
	// 需要等级
	UInt32                                          needLevel;
	// 颜色
	UInt32                                          color;
	// 颜色2
	UInt32                                          color2;
	// 时装套装id
	UInt32											suitId;
	// 基础评分
	UInt32                                          score;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class EquipBaseScoreDataEntryMgr : public Avalon::DataEntryMgrBase<EquipBaseScoreDataEntry>
	, public Avalon::Singleton<EquipBaseScoreDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipBaseScoreDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	EquipBaseScoreDataEntry* GetDataEntry(const EquipBaseScoreIndex& index);
	EquipBaseScoreDataEntry* GetDataEntryByItemData(const ItemDataEntry* itemData);

private:
	std::map<EquipBaseScoreIndex, EquipBaseScoreDataEntry*> m_MyDataEntrys;

// __CUSTOM_MGR_END__
};

#endif
