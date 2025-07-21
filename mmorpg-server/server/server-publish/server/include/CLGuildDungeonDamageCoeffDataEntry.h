/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONDAMAGECOEFF_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONDAMAGECOEFF_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class GuildDungeonDamageCoeffDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonDamageCoeffDataEntry();
	virtual ~GuildDungeonDamageCoeffDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 伤害系数（万分比）
	UInt32                                          coefficient;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonDamageCoeffDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonDamageCoeffDataEntry>
	, public Avalon::Singleton<GuildDungeonDamageCoeffDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonDamageCoeffDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	UInt32 GetDamageCoeff(UInt32 occu);

private:
	std::map<UInt32, UInt32> m_OccuDamageCoeff;

// __CUSTOM_MGR_END__
};

#endif
