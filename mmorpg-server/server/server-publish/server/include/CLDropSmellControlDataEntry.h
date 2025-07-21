/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_DROPSMELLCONTROL_DATA_ENTRY_H__
#define __CL_DROPSMELLCONTROL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct DropSmellControlKey
{
	bool operator <(const DropSmellControlKey& other) const
	{
		if (lv != other.lv)
		{
			return lv < other.lv;
		}

		if (color != other.color)
		{
			return color < other.color;
		}

		if (color2 != other.color2)
		{
			return color2 < other.color2;
		}

		if (dungeonSubType != other.dungeonSubType)
		{
			return dungeonSubType < other.dungeonSubType;
		}

		if (dungeonHard != other.dungeonHard)
		{
			return dungeonHard < other.dungeonHard;
		}

		return dungeonID < other.dungeonID;
	}
	// 装备LV
	UInt32                                          lv;
	// 颜色
	UInt32                                          color;
	// 颜色
	UInt32                                          color2;
	// 地下城类型
	UInt32                                          dungeonSubType;
	// 地下城难度
	UInt32                                          dungeonHard;
	// 地下城ID
	UInt32                                          dungeonID;
};

struct SmellControlKey
{
	bool operator <(const SmellControlKey& other) const
	{
		if (type < other.type)
		{
			return true;
		}
		else if (type == other.type)
		{
			if (lv < other.lv)
			{
				return true;
			}
			else if (lv == other.lv)
			{
				if (color < other.color)
				{
					return true;
				}
				else if (color == other.color)
				{
					if (color2 < other.color2)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	// 类型
	UInt32                                          type;
	// 装备LV
	UInt32                                          lv;
	// 颜色
	UInt32                                          color;
	// 颜色
	UInt32                                          color2;
};

// __CUSTOM_STRUCT_END__

class DropSmellControlDataEntry : public Avalon::DataEntry
{
public:
	DropSmellControlDataEntry();
	virtual ~DropSmellControlDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 类型
	UInt32                                          type;
	// 装备LV
	UInt32                                          lv;
	// 颜色
	UInt32                                          color;
	// 颜色
	UInt32                                          color2;
	// 子类型
	std::vector<UInt32>                             dungeonSubType;
	// 地下城难度
	std::vector<UInt32>                             dungeonHard;
	// 地下城ID
	UInt32                                          dungeonID;
	// 出现红字的概率(10000为100%）
	UInt32                                          probability;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class DropSmellControlDataEntryMgr : public Avalon::DataEntryMgrBase<DropSmellControlDataEntry>
	, public Avalon::Singleton<DropSmellControlDataEntryMgr>
{
public:
	virtual bool AddEntry(DropSmellControlDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	/**
	*@brief 根据掉落条件随机生成装备类型
	*/
	UInt8 RandEquipType(UInt32 lv, UInt32 color, UInt32 color2, UInt32 dungeonSubType, UInt32 dungeonHard, UInt32 dungeonID);
	UInt8 CommonRandEquipType(UInt32 type, UInt32 lv, UInt32 color, UInt32 color2);
private:
	/**
	*@brief 生成查询用的key
	*/
	DropSmellControlKey _MakeKey(UInt32 lv, UInt32 color, UInt32 color2, UInt32 dungeonSubType, UInt32 dungeonHard, UInt32 dungeonID);

private:
	// 根据地下城类型排序
	std::map<DropSmellControlKey, DropSmellControlDataEntry*>	m_DataByDungeonType;
	// 根据地下城ID排序
	std::map<DropSmellControlKey, DropSmellControlDataEntry*>	m_DataByDungeonID;
	// 地下城外专用
	std::map<SmellControlKey, DropSmellControlDataEntry*>	m_Data;
// __CUSTOM_MGR_END__
};

#endif
