/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_HONORLEVEL_DATA_ENTRY_H__
#define __CL_HONORLEVEL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class HonorLevelDataEntry : public Avalon::DataEntry
{
public:
	HonorLevelDataEntry();
	virtual ~HonorLevelDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 序号
	UInt32                                          id;
	// 荣誉等级级别
	UInt32                                          honorLvlGrade;
	// 需要经验
	UInt32                                          needExp;
	// 荣誉经验周增加上限
	UInt32                                          weekAddLimit;
	// 商店折扣(商店id_折扣|商店id_折扣...)
	std::vector<std::string>                        shopDiscount;
	// 升级需要前排名占比（万分比）
	UInt32                                          lvlNeedRankRate;
	// 参与排名最低人数
	UInt32                                          rankLeastNum;
	// 头衔
	UInt32                                          title;
	// 头衔有效期（分）
	UInt32                                          titleDueTime;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 商店折扣(key->商店id，val->折扣)
	std::map<UInt32, UInt32>						shopDiscountMap;

// __CUSTOM_ENTRYFIELD_END__
};

class HonorLevelDataEntryMgr : public Avalon::DataEntryMgrBase<HonorLevelDataEntry>
	, public Avalon::Singleton<HonorLevelDataEntryMgr>
{
public:
	virtual bool AddEntry(HonorLevelDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

public:
	// 根据经验获取荣誉等级
	UInt32 GetHonorLvl(UInt32 exp);

	// 获取商店折扣
	UInt32 GetShopDiscount(UInt32 honorLvl, UInt32 shopId);

	// 获取数据项
	const HonorLevelDataEntry* FindEntry(UInt32 honorLvl);

private:
	std::map<UInt32, UInt32> m_LvlNeedExpMap;

// __CUSTOM_MGR_END__
};

#endif
