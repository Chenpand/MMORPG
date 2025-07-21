#ifndef _SKILL_MGR_H_
#define _SKILL_MGR_H_

#include <CLDefine.h>
#include <CLSkillDefine.h>
#include <map>
#include <set>
#include <AvalonTime.h>
#include <AvalonSimpleTimer.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLSkillDataEntry.h>
#include <CLRecordCallback.h>
#include <CLDropItemDefine.h>
#include <CLSystemValueDataEntry.h>

class Creature;
class PowerDetail;
class ArenaPlayer;
class Player;
class SkillBar;


class SpMgr : public CLComplexProperty
{
public:
	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

private:
	Player*	m_pOwner;
};

/** 
 *@brief 技能
 */
struct Skill
{
public:
	Skill():id(0),level(0),bdirty(false)
	{
		dataEntry = NULL;
	}
	Skill(UInt16 _id, UInt8 _level) :id(_id),level(_level),bdirty(false)
	{
		dataEntry = NULL;
	}

public:
	/**
	 *@brief 初始化                                                                     
	 */
	bool Init();

	/**
	 *@brief 是否已删除
	 */
	bool IsValid(){ return level != 0; }

public://边界码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id;
		if(id != 0)
		{
			stream & level;
		}
	}

public:
	//id
	UInt16	id;
	//等级
	UInt8	level;
	//是否脏
	bool	bdirty;

	//技能表格数据
	SkillDataEntry*	dataEntry;
};

//技能访问器
typedef CLVisitor<Skill>	SkillVisitor;

/**
 *@brief 技能管理器
 */
class SkillMgr : public CLComplexProperty
{

	typedef std::map<UInt16, Skill>	SkillMap;

public:
	SkillMgr();
	~SkillMgr();

public:
	void SetOwner(Creature* owner);
	Creature* GetOwner() const { return m_pOwner; }

public:
	/**
	 *@brief 查找一个技能
	 */
	Skill* FindSkill(UInt16 id);

	/**
	*@brief 检查后置技能是否满足
	*/
	bool CheckNextSkill(UInt16 id, UInt8 level);

	/**
	 *@brief 访问所有技能
	 */
	void VisitSkills(SkillVisitor& visitor,bool bAll = false);

public:
	/**
	 *@brief 升级、遗忘一个技能 by huchenhui
	 */
	UInt32 ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify = false, bool isConsume = true);

	/**
	*@brief 重置技能 by huchenhui
	*/
	UInt32 ForgetAllSkills(bool isConsume = false);

	/**
	*@brief 检查要改变的技能是否满足条件 by huchenhui
	*/
	UInt32 CheckChangeSkills(Player* player, std::vector<ChangeSkill> skills, SkillMap& skillMap, Int32& sp, UInt32& removeGlod, bool isConsume = true);

	/**
	*@brief 检查技能等级,职业限定 by huchenhui
	*/
	UInt32 CheckSkill(Player* player, SkillDataEntry* dataEntry, Int32 skillLevel, UInt32 initLevel, Int8 difLevel);

	/**
	*@brief 检查技能关系  by huchenhui
	*/
	UInt32 CheckSkillRelation(SkillMap skillMap);

	/**
	*@brief 检查前置技能  by huchenhui
	*/
	UInt32 CheckPreSkill(SkillMap skillMap, Skill skill);

	/**
	*@brief 检查后置技能  by huchenhui
	*/
	UInt32 CheckPostSkill(SkillMap skillMap, Skill skill);

	/**
	*@brief 初始化技能 by zhengfeng
	*/
	UInt32 InitAllSkills();

	/**
	*@brief 推荐技能配置 by zhengfeng
	*/
	UInt32 RecommendSkills(bool isStudy = false);

	/**
	*@brief 清空技能 by huchenhui
	*/
	void ClearSkills();

	/**
	*@brief 清空预转职技能 by huchenhui
	*/
	void ClearPreOccuSkills();

	/**
	*@brief 获取技能使用的SP点
	*/
	UInt32 GetSkillSP();

	/**
	*@brief 释放能够配置到技能栏中 by huchenhui
	*/
	bool IsSetting(SkillDataEntry* dataEntry);

	/**
	*@brief 设置技能列表
	*/
	void SetSkills(SkillMap skills) { m_Skills = skills; }

	/**
	*@brief 设置技能
	*/
	void AddFreeSkill(UInt16 id, UInt8 level = 1);
	void DelFreeSkill(UInt16 id);

	/**
	*@brief 获取公会技能加成
	*/
	void GetGuildSkillDungeonAddition(DungeonAddition& addition);

	/**
	*@brief 设置雕像技能
	*/
	void AddStatueSkill(SystemValueType systemValueType);
	void DelStatueSkill(SystemValueType systemValueType);

public:
	// 吃鸡技能

	// 学习吃鸡技能
	void StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl);
	// 清除所有吃鸡技能
	void ClearChiJiSkill();
	// 下发吃鸡选择技能
	void SendChiJiChoiceSkill();
	// 选择吃鸡技能
	void ChoiceChiJiSkill(UInt32 skillId);
	// 是否能捡
	bool IsCanPickSkill();

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 上线
	*/
	void OnOnline();

	/**
	*@brief 转职触发   by huchenhui
	*/
	void OnChangeOccu();

	/**
	*@brief 出生触发   by huchenhui
	*/
	void OnBirth();

	/**
	*@brief 觉醒 by huchenhui
	*/
	void OnAwaken();

	/**
	*@brief 升级 by huchenhui
	*/
	void OnLevelUp();

	/**
	*@brief 检查SP和技能
	*/
	void OnCheckSkillSP();

protected:

	SkillBar& GetSkillBar();

	UInt32 GetSP();

	bool RemoveSP(const char* reason, UInt32 num);

	void AddSP(const char* reason, UInt32 num);

protected:
	void OnAwakenOne();

	void RecoverySP(UInt32 sp);

public: //编码
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);
	
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void ClearInvalidData();

protected:
	//拥有者
	Creature*	m_pOwner;
	//所有技能
	SkillMap	m_Skills;

	//技能配置方案
	SkillConfigType m_SkillConfigType;

	// 吃鸡技能选择列表
	std::map<UInt32, UInt32> m_ChijiSkillChoiceMap;

	// 吃鸡宝箱拾取的技能
	std::set<UInt32>	m_ChijiBoxSkillSet;

	//tick时间
	Avalon::SimpleTimer m_Timer;
};

class PvpSkillMgr : public SkillMgr
{
public:
	PvpSkillMgr()
	{
		m_SkillConfigType = SkillConfigType::SKILL_CONFIG_PVP;
	}

	~PvpSkillMgr(){};

};

#endif
