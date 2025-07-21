/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EQUIPBASESCOREMOD_DATA_ENTRY_H__
#define __CL_EQUIPBASESCOREMOD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

#define EQ_BASESC_PARAM(value) (float)value / 1000

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct EquipBaseScoreModEntryKey
{
	EquipBaseScoreModEntryKey(UInt8	subType_, UInt8 quality_)
	:subType(subType_), quality(quality_){}

	bool operator<(const EquipBaseScoreModEntryKey& other) const
	{
		if (other.subType != subType)
		{
			return subType < other.subType;
		}
		return quality < other.quality;
	}

	UInt8	subType;	//子类别
	UInt8	quality;	//颜色
};

// __CUSTOM_STRUCT_END__

class EquipBaseScoreModDataEntry : public Avalon::DataEntry
{
public:
	EquipBaseScoreModDataEntry();
	virtual ~EquipBaseScoreModDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// ID
	UInt32										id;
	// 子类别
	UInt8										itemSubType;
	// 颜色
	UInt8										itemQuality;
	// 攻击力系数1
	UInt32										attrMod1;
	// 攻击力系数2
	UInt32                                      attrMod2;
	// 四维系数1
	UInt32                                      powerMod1;
	// 四维系数2
	UInt32                                      powerMod2;
	// 四维系数3
	UInt32                                      powerMod3;
	// 防御力品质系数
	UInt32                                      defMod1;
	// 防御力部位系数
	UInt32										defMod2;
	// 强化品质系数A
	UInt32                                      strenthQualityMod1;
	// 强化品质系数B
	UInt32                                      strenthQualityMod2;
	// 强化系数（武器）
	std::vector<UInt32>                        strengthMod1;
	// 强化系数（防御）
	std::vector<UInt32>                        strengthMod2;
	// 强化减伤
	std::vector<UInt32>                        strengthMod3;
// __CUSTOM_ENTRYFIELD_END__
};

class EquipBaseScoreModDataEntryMgr : public Avalon::DataEntryMgrBase<EquipBaseScoreModDataEntry>
	, public Avalon::Singleton<EquipBaseScoreModDataEntryMgr>
{
public:
	typedef std::map<EquipBaseScoreModEntryKey, EquipBaseScoreModDataEntry*> DataEntryMap;
	virtual bool AddEntry(EquipBaseScoreModDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	EquipBaseScoreModDataEntry*	GetEntry(UInt8 subType, UInt8 quailty);
// __CUSTOM_MGR_END__
private:
	DataEntryMap m_Entrys;
};

#endif
