/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_WEAPONBUILDCOST_DATA_ENTRY_H__
#define __CL_WEAPONBUILDCOST_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemTypeDefine.h>
#include <CLItemDefine.h>

struct WeaponBuildData
{
	UInt32 id;
	UInt32 num;
};

struct PrecentRatio
{
	UInt32 beginPrecent;
	UInt32 endPrecent;
	UInt32 ratio;
};

enum MainPropertyType
{
	MPT_BASE = 0,	//基本属性
	MPT_SPECIAL = 1,//特殊属性
	MPT_BUFF = 2,	//buff类属性
};

class WeaponBuildCostDataEntry : public Avalon::DataEntry
{
public:
	WeaponBuildCostDataEntry();
	virtual ~WeaponBuildCostDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// ID
	UInt32                                          id;
	// 打造武器等级
	UInt32											level;
	// 必备材料
	std::vector<ItemReward>							mustItem;
	// 辅助材料1
	ItemReward										unnecessaryItem_1;
	// 辅助材料2
	ItemReward										unnecessaryItem_2;
};

class WeaponBuildCostDataEntryMgr : public Avalon::DataEntryMgrBase<WeaponBuildCostDataEntry>
	, public Avalon::Singleton<WeaponBuildCostDataEntryMgr>
{
public:
	virtual bool AddEntry(WeaponBuildCostDataEntry* entry);


};



class WeaponBuildPropertyPoolDataEntry : public Avalon::DataEntry
{
public:
	WeaponBuildPropertyPoolDataEntry();
	virtual ~WeaponBuildPropertyPoolDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 属性类型
	MainPropertyType								mainPropertyType;
	// 普通属性
	ItemKeyValuesKeyType							propType;
	// 加成数值
	std::vector<Int32>								propertyNum;
	// 装备特性类型
	UInt32											special;
	// 装备特性数值
	UInt32											special_detail;
	// 特殊属性（buff信息表ID）PVE
	UInt32                                          buffinfo_PVE;
	// 特殊属性（buff信息表ID）PVP
	UInt32                                          buffinfo_PVP;
	// 特殊属性（被动机制表ID）PVE
	UInt32                                          mechanism_PVE;
	// 特殊属性（被动机制表ID）PVP
	UInt32                                          mechanism_PVP;
	// 附加评分
	UInt32                                          num;
};

class WeaponBuildPropertyPoolDataEntryMgr : public Avalon::DataEntryMgrBase<WeaponBuildPropertyPoolDataEntry>
	, public Avalon::Singleton<WeaponBuildPropertyPoolDataEntryMgr>
{
public:
	virtual bool AddEntry(WeaponBuildPropertyPoolDataEntry* entry);


};

enum WeaponBuildRandomPoolType
{
	//属性库
	WBRPT_RANDOM_POOL = 0,
	//属性池
	WBRPT_PROPERTY_POOL = 1,
};

class WeaponBuildRandomPoolDataEntry : public Avalon::DataEntry
{
public:
	WeaponBuildRandomPoolDataEntry();
	virtual ~WeaponBuildRandomPoolDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 属性库ID
	UInt32				                            GroupID;
	// 个数方案
	UInt32											chooseNum;
	// 掉落类型
	UInt32                                          dataType;
	// 属性池ID
	UInt32                                          dataID;
	// 权重
	UInt32                                          dataProb;
	// 物品数量
	UInt32				                            dataNum;
	// 武器等级
	UInt32											weaponLevel;
	// 标签
	UInt32                                          randDataType;
	// 武器类型
	std::set<UInt32>								weaponTypeSet;
};


struct WeaponBuildRandomPoolContext 
{
	UInt32 weaponLevel;
	UInt32 type;
	UInt32 weaponType;
};

class WeaponBuildRandomPoolDataEntryMgr : public Avalon::DataEntryMgrBase<WeaponBuildRandomPoolDataEntry>
	, public Avalon::Singleton<WeaponBuildRandomPoolDataEntryMgr>
{
public:
	virtual bool AddEntry(WeaponBuildRandomPoolDataEntry* entry);

	void Init();

	void GenProperty(UInt32 id, std::vector<UInt32>& vec, WeaponBuildRandomPoolContext& context);
private:
	std::map<UInt32, std::vector<WeaponBuildRandomPoolDataEntry>> m_DataMap;
};



class WeaponBuildOccuDataEntry : public Avalon::DataEntry
{
public:
	WeaponBuildOccuDataEntry();
	virtual ~WeaponBuildOccuDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// ID
	UInt32                                          id;
	// 对应橙色品质道具ID（关联模型）
	UInt32                                          yellowItemID;
	// 对应神圣品质假道具ID（关联模型）
	UInt32                                          silverItemID;
	// 属性类型
	UInt32											naturePropertyType;
	// 档次
	UInt32											quality;
	// 可打造的道具三类别
	ItemThirdType                                   itemThirdType;
	// 武器等级
	UInt32                                          level;
};

class WeaponBuildDataEntryMgr : public Avalon::DataEntryMgrBase<WeaponBuildOccuDataEntry>
	, public Avalon::Singleton<WeaponBuildDataEntryMgr>
{
public:
	virtual bool AddEntry(WeaponBuildOccuDataEntry* entry);

	std::pair<UInt32,UInt32> FindItemID(UInt32 quality, UInt32 thirdType, UInt32 needLevel, UInt32 naturePropertyType);
};

class WeaponBuildQualityRandomDataEntry : public Avalon::DataEntry
{
public:
	WeaponBuildQualityRandomDataEntry();
	virtual ~WeaponBuildQualityRandomDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	//ID
	UInt32											id;
	//武器等级
	UInt32											weaponLevel;
	// 档次
	UInt32                                          grade;
	// 权重
	UInt32                                          classWeighted;
	// 辅材1权重
	UInt32                                          classAssitanceWeighted;
	// 随机库ID
	UInt32											groupID;
};

class WeaponBuildQualityRandomDataEntryMgr : public Avalon::DataEntryMgrBase<WeaponBuildQualityRandomDataEntry>
	, public Avalon::Singleton<WeaponBuildQualityRandomDataEntryMgr>
{
public:
	virtual bool AddEntry(WeaponBuildQualityRandomDataEntry* entry);

	std::pair<UInt32, UInt32> GetRandomGradeAndGroupID(UInt32 weaponLevel, bool useExtra);
private:
	std::map<UInt32, UInt32> m_TotalWeightMap;
	std::map<UInt32, UInt32> m_ExtraTotalWeightMap;
	std::map<UInt32, std::vector<WeaponBuildQualityRandomDataEntry*>> m_LevelGradeMap;
};



bool IsAttackProp(ItemKeyValuesKeyType prop);

bool IsFourDimensionalProp(ItemKeyValuesKeyType prop);

bool IsDefProp(ItemKeyValuesKeyType prop);




#endif
