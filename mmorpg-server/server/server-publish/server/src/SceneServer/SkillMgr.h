#ifndef _SKILL_MGR_H_
#define _SKILL_MGR_H_

#include <CLDefine.h>
#include <CLSkillDefine.h>
#include <map>
#include <AvalonTime.h>
#include <AvalonSimpleTimer.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLSkillDataEntry.h>
#include <CLRecordCallback.h>
#include <CLDropItemDefine.h>
#include <CLSystemValueDataEntry.h>
#include <CLOwnerObjectProperty.h>

class Creature;
class PowerDetail;
class ArenaPlayer;
class Player;
class SkillBar;


class SpMgr : public CLOwnerComplexProperty
{
public:
	SpMgr() { m_Sp.resize(CL_MAX_SKILL_PAGE); }
	~SpMgr() = default;

	void CopyWithoutConfig(const SpMgr & rsh);

	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void Reset();

	void SetCurrentPage(UInt32 page) { m_CurrentPage = page; }

	void AddAllPageSp(UInt32 sp);
	void RemoveAllPageSp(UInt32 sp);

	void SetPageSp(UInt32 page, UInt32 sp) { m_Sp[page] = sp; SetDirty(); }
	UInt32 GetPageSp(UInt32 page) { return m_Sp[page]; SetDirty(); }

	/**
	 *@brief 转为为基本类型
	 */
	operator UInt32() const { return m_Sp[m_CurrentPage]; }
protected:
	SkillConfigType m_SkillConfigType = SKILL_CONFIG_PVE;

private:
	std::vector<UInt32> m_Sp;
	UInt32 m_CurrentPage = 0;
	Player*	m_pOwner;
};

class PveSpMgr : public SpMgr
{
public:
	PveSpMgr() { m_SkillConfigType = SKILL_CONFIG_PVE; };

};

class PvpSpMgr : public SpMgr
{
public:
	PvpSpMgr() { m_SkillConfigType = SKILL_CONFIG_PVP; };

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

	Skill(const Skill & rsh)
	{
		//id
		id = rsh.id;
		//等级
		level = rsh.level;
		//是否脏
		bdirty = true;
		//技能表格数据
		dataEntry = rsh.dataEntry;
	}

	Skill & operator=(const Skill & rsh)
	{
		//id
		id = rsh.id;
		//等级
		level = rsh.level;
		//是否脏
		bdirty = true;
		//技能表格数据
		dataEntry = rsh.dataEntry;
		return *this;
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
 *@brief 单页技能管理器
 */
class SkillMgr
{
	typedef std::map<UInt16, Skill>	SkillMap;

public:
	SkillMgr();
	~SkillMgr();
	SkillMgr(UInt8 pageNum) : SkillMgr() { m_Page = pageNum; }
	SkillMgr& operator=(const SkillMgr& rsh);

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
	void OnCheckSkillSP(bool skipCheck = false);

	/**
	*@brief 任务完成
	*/
	void OnTask(UInt32 id);

	/**
	*@brief 检查任务解锁技能槽位
	*/
	void CheckTaskUnlockSlot();

	/**
	*@brief 任务解锁槽位
	*/
	bool IsTaskUnlockSlot();



protected:

	SkillBar& GetSkillBar();

	UInt32 GetSP();

	bool RemoveSP(const char* reason, UInt32 num);

	void AddSP(const char* reason, UInt32 num);

	UInt32 GetNeedSyncSkillCnt();

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

	void SetDirty();

	void SetConfigType(SkillConfigType type) { m_SkillConfigType = type; }

protected:
	//拥有者
	Creature*	m_pOwner;
	//所有技能
	SkillMap	m_Skills;

	//技能配置方案
	SkillConfigType m_SkillConfigType;

	//tick时间
	Avalon::SimpleTimer m_Timer;

	UInt8 m_Page;
};




/**
 *@brief 技能管理器
 */
class PlayerSkillMgr : public CLOwnerComplexProperty
{
public:
	PlayerSkillMgr();
	~PlayerSkillMgr() = default;
	//PlayerSkillMgr(SkillConfigType type) { m_SkillConfigType = type; }

public:
	void SetOwner(Creature* owner);
	Creature* GetOwner() const { return m_pOwner; }

public:
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

public:
	void ClearSkills();

	bool GetSkillPageUnlockStatus(UInt8 page);
	void UnlockNewPage();
	UInt32 GetSkillPageCnt() { return m_PageCnt; }
	void SetCurrentSkillPage(UInt32 page) { m_CurrentPage = page; SetDirty(); }
	UInt32 GetCurrentSkillPage() { return m_CurrentPage; }

	/**
	*@brief 重置技能
	*/
	UInt32 ForgetAllSkills(bool isConsume = false);

	/**
	*@brief 清除预转职技能
	*/
	void ClearPreOccuSkills();

	/**
	 *@brief 升级、遗忘一个技能
	 */
	UInt32 ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify = false, bool isConsume = true);

	/**
	*@brief 初始化技能
	*/
	UInt32 InitAllSkills();

	/**
	*@brief 推荐技能配置
	*/
	UInt32 RecommendSkills(bool isStudy = false);

	/**
	*@brief 获取公会技能加成
	*/
	void GetGuildSkillDungeonAddition(DungeonAddition& addition);

	/**
	*@brief 设置技能
	*/
	void AddFreeSkill(UInt16 id, UInt8 level = 1);
	void DelFreeSkill(UInt16 id);

	/**
	 *@brief 查找一个技能
	 */
	Skill* FindSkill(UInt16 id);

	/**
	*@brief 设置雕像技能
	*/
	void AddStatueSkill(SystemValueType systemValueType);
	void DelStatueSkill(SystemValueType systemValueType);


	/**
	*@brief 释放能够配置到技能栏中
	*/
	bool IsSetting(SkillDataEntry* dataEntry);

	/**
	 *@brief 访问所有技能
	 */
	void VisitSkills(SkillVisitor& visitor, bool bAll = false);

	/**
	*@brief 检查任务解锁技能槽位
	*/
	void CheckTaskUnlockSlot();
	/**
	*@brief 任务解锁槽位
	*/
	bool IsTaskUnlockSlot();

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
	*@brief 任务完成
	*/
	void OnTask(UInt32 id);

	void CopyWithoutConfigType(const PlayerSkillMgr & rsh);

	/**
	*@brief 同步第一页公会技能到所有页
	*/
	void SyncGuildSkillToAllPage();


protected:
	void SetConfigType(SkillConfigType type);

private:
	//所有页的技能,下标作为页数
	std::vector<SkillMgr> m_SkillMgrVec;
	//拥有者
	Creature*	m_pOwner;
	UInt32 m_PageCnt = 1;
	UInt32 m_CurrentPage = 0;
	SkillConfigType m_SkillConfigType = SKILL_CONFIG_PVE;
};



class PvpSkillMgr : public PlayerSkillMgr
{
public:
	PvpSkillMgr()
	{
		SetConfigType(SkillConfigType::SKILL_CONFIG_PVP);
	}

	~PvpSkillMgr() {};

};

class EqualPvpSkillMgr : public PlayerSkillMgr
{
public:
	EqualPvpSkillMgr()
	{
		SetConfigType(SkillConfigType::SKILL_CONFIG_EQUAL_PVP);
	}

	~EqualPvpSkillMgr() {};

};

#endif
