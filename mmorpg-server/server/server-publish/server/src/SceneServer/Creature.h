#ifndef _CREATURE_H_
#define _CREATURE_H_

#include <CLSkillProtocol.h>
#include <CLItemDefine.h>
#include <set>
#include "SkillMgr.h"
#include "BuffMgr.h"
#include "SceneObject.h"

class Item;
class Player;
class CreatureAI;

class Creature;
typedef CLVisitor<Creature> CreatureVisitor;

class Player;

#define MAX_CREATUREPARAM_NAME_LEN 16
#define PLAYER_INIT_MOVE_SPEED 1000

/**
 *@brief 生物，能自主活动的场景物体
 */
class Creature : public SceneObject
{
public:
	typedef std::vector<ObjID_t>	SelectList;
	typedef std::set<Creature*>		HaterSet;

public:
	Creature();
	~Creature();

public:
	/**
	 *@brief 设置获取数据id
	 */
	virtual void SetDataID(UInt32 id){ }
	virtual UInt32 GetDataID() const { return 0; }

	/**
	 *@brief 设置数据配置
	 */
	virtual bool SetupBase() { return true; }

	/**
	 *@brief 设置获取等级
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	virtual UInt8 GetOccu() const{ return 0; }

	/**
	 *@brief 设置获取性别
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	 *@brief 设置获取状态
	 */
	void SetStatus(UInt8 status){ m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	 *@brief 获取vip等级
	 */
	virtual UInt8 GetVipLvl() { return 0; }

	virtual bool HasGmAuthority(UInt8 authority) const { return false; };
	/**
	 *@brief 设置获取存档时间
	 */
	void SetSavebackTime(UInt64 time){ m_SavebackTime = time; }
	UInt64 GetSavebackTime() const { return m_SavebackTime; }

	/**
	 *@brief 如果是玩家，转化为玩家对象                                                                     
	 */
	Player* ToPlayer() const;

public://获取基础属性值
	UInt32 GetMoveInterval() const;

	UInt16 GetMoveSpeed() const { return m_MoveSpeed; }
	void SetMoveSpeed(UInt32 speed) { m_MoveSpeed = speed; }
	void AddMoveSpeed(Int16 val);

public:
	/**
	 *@brief 技能相关操作
	 */
	 bool UseSkill(UInt16 id, const CLPosition& pos, ObjID_t targetid, bool bPrepared = false);
	bool UseSkill(Skill* skill, const CLPosition& pos, ObjID_t targetid, bool bPrepared = false);
	Skill* FindSkill(UInt16 id);
	PlayerSkillMgr& GetSkillMgr(){ return m_SkillMgr; }
	bool ScriptUseSkillDirect(UInt16 id, UInt8 level, ObjID_t targetid);

	void ClearSkills() { m_SkillMgr.ClearSkills(); }
	void ForgetAllSkills() { m_SkillMgr.ForgetAllSkills(); }

	/**
	 *@brief buff相关
	 */
	Buff* FindBuff(UInt16 buffid);
	bool HasBuff(UInt16 buffid);
	BuffMgr& GetBuffMgr(){ return m_BuffMgr; }

	/**
	*@brief 是否隐身
	*/
	bool IsHidden(){ return m_isHidden; }
    void SetHidden(bool hidden) { m_isHidden = hidden; }

public: //关系相关
	/**
	 *@brief 判断是否是队友
	 */
	virtual bool IsTeammate(Creature* creature) const { return false; }

public: //聊天相关
	/**
	 *@brief 发送聊天语句
	 */
	virtual void Speak(UInt8 channel, const std::string& word, std::string voiceKey = "", UInt8 voiceDuration = 0, ObjID_t target = 0, bool bLink = false, UInt32 headFrame = 0);

	virtual void ScriptSpeak(UInt8 channel, const char* word);

public://事件
	/**
	 *@brief 心跳事件
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief 离线
	 */
	virtual void OnOffline();

	/**
	 *@brief 进入场景
	 */
	virtual void OnEnterScene();

	/**
	 *@brief 离开场景
	 */
	virtual void OnLeaveScene();

	/**
	 *@brief 场景切换
	 */
	virtual void OnSceneChanged();

	/**
	 *@brief 移动
	 */
	virtual void OnMoved(const CLPosition& oldpos);

	/**
	*@brief 同步属性变化
	*/
	void SyncProperty();

public:	//编码解码相关
	CL_SYNC_PROPERTY_BEGIN(SceneObject)
	CL_SYNC_PROPERTY(m_Level, SOA_LEVEL, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_Sex, SOA_SEX, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_Status, SOA_STATUS, SST_TEST | SST_SELF | SST_AROUND | SST_SCENE | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)

	CL_SYNC_PROPERTY(m_MoveSpeed, SOA_MOVESPEED, SST_SELF | SST_AROUND | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)

	CL_SYNC_PROPERTY(m_SkillMgr,	SOA_SKILLS,			SST_SELF|SST_SKILLBUFF|SST_TEST)
	CL_SYNC_PROPERTY(m_BuffMgr,		SOA_BUFFS,			SST_SELF|SST_SELECTED|SST_TEAM|SST_SKILLBUFF|SST_TEST)

	CL_SYNC_PROPERTY_END()


	CL_DBPROPERTY_BEGIN(SceneObject)
	CL_DBPROPERTY("sex",		m_Sex)
	CL_DBPROPERTY("level",		m_Level)
	// hp一直在变化,干扰recored定时存DB modified by huchenhui 
	//CL_DBPROPERTY("hp",			m_Hp)
	CL_DBPROPERTY("skills",		m_SkillMgr)
	CL_DBPROPERTY("buffs",		m_BuffMgr)
	CL_DBPROPERTY_END()

	DEFINE_CHANGESCENE_SERIALIZATION(SceneObject)
	{
		stream & m_SavebackTime;
		stream & m_SkillMgr & m_BuffMgr;
	}

	//清除无效数据
	void ClearInvalidData();
	
private:
	//等级
	OwnerObjUInt16	m_Level;
	//性别
	OwnerObjUInt8	m_Sex;
	//状态
	OwnerObjUInt8	m_Status;
	
	//移动速度
	OwnerObjUInt16	m_MoveSpeed;

	//技能
	PlayerSkillMgr	m_SkillMgr;
	//buff
	BuffMgr		m_BuffMgr;

	//存档时间
	UInt64		m_SavebackTime;

    // 是否隐身
    bool        m_isHidden;
};

#endif

