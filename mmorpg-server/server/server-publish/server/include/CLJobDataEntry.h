#ifndef _CL_JOB_DATAENTRY_H_
#define _CL_JOB_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>

/**
*@brief 职业
*/
enum Occupation
{
    OCCU_NONE = 0,				//无职业
    OCCU_GUIJIANSHI = 10,		//鬼剑士
    OCCU_JIANHUN,
    OCCU_KUANGZHANSHI,
    OCCU_GUIQI,
    OCCU_AXIULUO,

    OCCU_SHENQIANGSHOU = 20,	//神枪手
    OCCU_MANYOU,
    OCCU_QIANGPAOSHI,
    OCCU_JIXIESHI,
    OCCU_DANYAOZHUANJIA,

    OCCU_MOFASHI = 30,			//魔法师
    OCCU_YUANSUSHI,
    OCCU_ZHANDOUFASHI,
    OCCU_ZHAOHUANSHI,

    OCCU_GEDOUJIA = 40,			//格斗家
    OCCU_QIGONGSHI,
    OCCU_SANDA,
    OCCU_JIEBA,
    OCCU_ROUDAOJIA,

	OCCU_SHENQIANGSHOU_NV = 50,	//神枪手女
	OCCU_MANYOU_NV,
	OCCU_QIANGPAOSHI_NV,
	OCCU_JIXIESHI_NV,
	OCCU_DANYAOZHUANJIA_NV,

	OCCU_SHENGZHIZHE = 60,	//圣职者
	OCCU_QUMO = 61,
	OCCU_SHENGQISHI = 62,	//圣骑士

	OCCU_MUSHI = 70,		//牧师

	//TODO
	OCCU_MAX = 128,
};

/**
*@brief 职业数据项
*/
class JobDataEntry : public Avalon::DataEntry
{
	typedef std::vector<UInt8>	OccuVec;
	typedef std::vector<UInt16>	SkillVec;

public:
    JobDataEntry();
    ~JobDataEntry();

public:
    UInt32 GetKey() const { return (UInt32)occu; }

    bool Read(Avalon::DataRow& row);

	bool CheckTargetOccu(UInt8 occu);

	UInt32 GetInitLevel(UInt16 id);

	UInt32 GetPreSkillLevel(UInt16 id);

	SkillVec GetInitSkill() { return skills; }

	static Occupation GetBaseOccu(UInt8 occu) { return (Occupation)(occu / 10 * 10); }

public:
    Occupation	occu;

	std::string name;

	//是否开放
	bool	isOpen;

	//职业赠送技能
	SkillVec skills;

	//觉醒赠送技能
	SkillVec awakenSkills;

	//预转职技能
	SkillVec preJobSkills;

	//进阶职业列表
	OccuVec targetOccus;

	//能被创角
    bool        canCreateRole;

	//转职奖励物品
	UInt32	itemId;

	//公平竞技场总技能点数
	UInt32 EqualPvPSP;

	//是否是吃鸡职业
	UInt32 chiJiOccu;
};


class JobDataEntryMgr : public Avalon::DataEntryMgrBase<JobDataEntry>
	, public Avalon::Singleton<JobDataEntryMgr>
{
public:
	virtual bool AddEntry(JobDataEntry* entry);

	// 获取开放的转职职业数量
	UInt32 GetOpenChangeOccuNum() { return m_OpenChangeOccuVec.size(); }

	// 获取吃鸡职业列表
	std::vector<UInt32>& GetChiJiOccuVec() { return m_ChiJiOccuVec; }

private:
	std::vector<UInt8> m_OpenChangeOccuVec;

	std::vector<UInt32> m_ChiJiOccuVec;
};

#endif
