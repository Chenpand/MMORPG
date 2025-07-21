/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EQUIPINSCRIPTIONHOLE_DATA_ENTRY_H__
#define __CL_EQUIPINSCRIPTIONHOLE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class EquipInscriptionHoleDataEntry : public Avalon::DataEntry
{
public:
	EquipInscriptionHoleDataEntry();
	virtual ~EquipInscriptionHoleDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 MakeKey(UInt32 color, UInt32 color2, UInt32 subType);

	struct  HoleDataConfig
	{
		UInt32 index;
		UInt32 type;
	};
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 装备品质
	UInt32                                          color;
	// 颜色
	UInt32                                          color2;
	// 装备子类别
	UInt32											subType;
	// 可开孔总数
	UInt32                                          inscriptionHoleNum;
	// 对应孔颜色
	std::string                                     inscriptionHoleColor;
	// 单次开孔器消耗
	std::string                                     itemConsume;
	// 金币抵扣开孔器消耗
	std::string                                     goldConsume;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	//孔信息
	std::vector<HoleDataConfig> holeVec;
	//材料信息
	UInt32 matId;
	UInt32 matNum;
	//抵扣信息
	UInt32 moneyId;
	UInt32 moneyNum;
// __CUSTOM_ENTRYFIELD_END__
};

class EquipInscriptionHoleDataEntryMgr : public Avalon::DataEntryMgrBase<EquipInscriptionHoleDataEntry>
	, public Avalon::Singleton<EquipInscriptionHoleDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipInscriptionHoleDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
