#ifndef _CL_SKILL_DATAENTRY_H_
#define _CL_SKILL_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemTypeDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <sstream>

#define CL_MAKE_AWAKEN_KEY(Awaken,Occu) ((UInt32(Awaken) << 16) + Occu)

/**
*@brief 技能武器掩码
*/
enum SkillWeaponFlag
{
	SKILL_WEAPON_AXE = 1 << 0,	//斧
	SKILL_WEAPON_SWARD = 1 << 1,	//剑
	SKILL_WEAPON_CANE = 1 << 2,	//杖
	SKILL_WEAPON_BOW = 1 << 3,	//弓
};

/**
*@brief 根据装备类型获取技能武器掩码
*/
inline UInt16 GetSkillWeaponFlag(Int32 weaponType)
{
	// 	switch(weaponType)
	// 	{
	// 	case EQUIP_TYPE_AXE:	return SKILL_WEAPON_AXE;
	// 	case EQUIP_TYPE_SWARD:	return SKILL_WEAPON_SWARD;
	// 	case EQUIP_TYPE_BOW:	return SKILL_WEAPON_BOW;
	// 	}
	return 0;
}

struct SkillDataLevel
{
public:
	//技能ID
	UInt16	id;
	//技能等级
	UInt8	level;

};
typedef std::vector<SkillDataLevel>	NeedSkillVec;


/**
*@brief 技能配置表
*/
struct SkillDataEntry : public Avalon::DataEntry
{
	typedef std::vector<UInt8>	OccuVec;

public:
	SkillDataEntry();
	~SkillDataEntry();

public:
	UInt32 GetKey() const
	{
		return id;
	}

	bool Read(Avalon::DataRow& row);
	bool CheckOccu(UInt8 occu);

public:
	//技能ID
	UInt16			id;
	//技能名称
	std::string		name;
	//职业
	OccuVec			occus;
	//技能使用类型
	UInt8			useType;
	//技能类型
	UInt8			type;
	//是否buff
	bool			isBuff;
	//是否QTE技能
	bool			isQTE;
	//技能pvp携带方式
	UInt8           canUseInPVP;
	//buffer描述列表
	std::vector<UInt32> buffInfoList;
	//是否预转职技能
	bool			isPreJob;
	//是否自动学习
	bool			isStudy;
	//玩家等级
	UInt16			playerLevel;
	//升级玩家等级间隔
	UInt8			upLevelInterval;
	//最高等级
	UInt8			maxLevel;
	//前置技能
	NeedSkillVec	needSkills;
	//消耗SP
	UInt32			needSP;
	//是否必然同步
	bool			isForceSync;

};

/**
*@brief 技能配置表
*/
/*
struct SkillDataEntry : public Avalon::DataEntry
{
public:
SkillDataEntry();
~SkillDataEntry();

public:
UInt32 GetKey() const
{
return id;
}

bool Read(Avalon::DataRow& row);

public:
//技能id
UInt16		id;
//技能名称
std::string	name;
//职业
UInt8		occu;
//使用方式
UInt8		usetype;
//技能禁用组(用于地图对技能的限制)
UInt8		disablegroup;
//技能类型
UInt8		type;
//需要buff
UInt16		needbuff;
//需要武器掩码
UInt16		weaponflag;
//共享cd组
UInt8		cdgroup;
//共享cd(毫秒)
UInt32		groupcd;
//阻挡类型
UInt8		blockflag;
//目标范围
UInt8		scope;
//范围形状参数1
UInt8		scopeshapeparam1;
//范围形状参数2
UInt8		scopeshapeparam2;
//目标类型掩码
UInt16		targetflag;
//目标距离下限
UInt8		distancefloor;
//目标距离上线
UInt8		distancetop;
//0中性 1有益 2有害
UInt8		pkattr;
//学习后是否自动添加到挂机技能，0 否，1 是
UInt8		autohook;
};
*/

/**
*@brief 技能配置表项管理器
*/
//typedef Avalon::DataEntryMgr<SkillDataEntry>	SkillDataEntryMgr;
class SkillDataEntryMgr : public Avalon::DataEntryMgrBase<SkillDataEntry>, public Avalon::Singleton<SkillDataEntryMgr>
{
public:
	typedef std::vector<UInt16> SkillIdVec;
	typedef std::map<UInt16, SkillIdVec> SkillIdVecMap;

	typedef std::vector<SkillDataEntry*> SkillVec;
	typedef std::map<UInt8, SkillVec> AwakenSkillMap;

	typedef std::map<UInt8, SkillVec> LevelSkillMap;
	typedef std::map<UInt8, LevelSkillMap> AutoStudySkillMap;

	SkillDataEntryMgr();
	~SkillDataEntryMgr();

public:
	bool AddEntry(SkillDataEntry* entry);

	const SkillIdVec& GetPostSkills(UInt16 skillId) const;

	const SkillVec& GetAwakenSkills(UInt8 occu) const;

	const SkillVec& GetAutoStudySkills(UInt8 level, UInt8 occu) const;

private:
	//后置技能Map	表示某个技能的后置技能列表
	SkillIdVecMap m_PostSkills;

	//觉醒技能Map	表示某职业的觉醒技能
	AwakenSkillMap m_AwakenSkills;

	//自动学习技能
	AutoStudySkillMap m_AutoStudySkills;
};

#endif
