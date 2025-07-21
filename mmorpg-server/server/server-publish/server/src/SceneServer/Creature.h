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
 *@brief �����������ĳ�������
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
	 *@brief ���û�ȡ����id
	 */
	virtual void SetDataID(UInt32 id){ }
	virtual UInt32 GetDataID() const { return 0; }

	/**
	 *@brief ������������
	 */
	virtual bool SetupBase() { return true; }

	/**
	 *@brief ���û�ȡ�ȼ�
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	virtual UInt8 GetOccu() const{ return 0; }

	/**
	 *@brief ���û�ȡ�Ա�
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	 *@brief ���û�ȡ״̬
	 */
	void SetStatus(UInt8 status){ m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	 *@brief ��ȡvip�ȼ�
	 */
	virtual UInt8 GetVipLvl() { return 0; }

	virtual bool HasGmAuthority(UInt8 authority) const { return false; };
	/**
	 *@brief ���û�ȡ�浵ʱ��
	 */
	void SetSavebackTime(UInt64 time){ m_SavebackTime = time; }
	UInt64 GetSavebackTime() const { return m_SavebackTime; }

	/**
	 *@brief �������ң�ת��Ϊ��Ҷ���                                                                     
	 */
	Player* ToPlayer() const;

public://��ȡ��������ֵ
	UInt32 GetMoveInterval() const;

	UInt16 GetMoveSpeed() const { return m_MoveSpeed; }
	void SetMoveSpeed(UInt32 speed) { m_MoveSpeed = speed; }
	void AddMoveSpeed(Int16 val);

public:
	/**
	 *@brief ������ز���
	 */
	 bool UseSkill(UInt16 id, const CLPosition& pos, ObjID_t targetid, bool bPrepared = false);
	bool UseSkill(Skill* skill, const CLPosition& pos, ObjID_t targetid, bool bPrepared = false);
	Skill* FindSkill(UInt16 id);
	PlayerSkillMgr& GetSkillMgr(){ return m_SkillMgr; }
	bool ScriptUseSkillDirect(UInt16 id, UInt8 level, ObjID_t targetid);

	void ClearSkills() { m_SkillMgr.ClearSkills(); }
	void ForgetAllSkills() { m_SkillMgr.ForgetAllSkills(); }

	/**
	 *@brief buff���
	 */
	Buff* FindBuff(UInt16 buffid);
	bool HasBuff(UInt16 buffid);
	BuffMgr& GetBuffMgr(){ return m_BuffMgr; }

	/**
	*@brief �Ƿ�����
	*/
	bool IsHidden(){ return m_isHidden; }
    void SetHidden(bool hidden) { m_isHidden = hidden; }

public: //��ϵ���
	/**
	 *@brief �ж��Ƿ��Ƕ���
	 */
	virtual bool IsTeammate(Creature* creature) const { return false; }

public: //�������
	/**
	 *@brief �����������
	 */
	virtual void Speak(UInt8 channel, const std::string& word, std::string voiceKey = "", UInt8 voiceDuration = 0, ObjID_t target = 0, bool bLink = false, UInt32 headFrame = 0);

	virtual void ScriptSpeak(UInt8 channel, const char* word);

public://�¼�
	/**
	 *@brief �����¼�
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	virtual void OnOffline();

	/**
	 *@brief ���볡��
	 */
	virtual void OnEnterScene();

	/**
	 *@brief �뿪����
	 */
	virtual void OnLeaveScene();

	/**
	 *@brief �����л�
	 */
	virtual void OnSceneChanged();

	/**
	 *@brief �ƶ�
	 */
	virtual void OnMoved(const CLPosition& oldpos);

	/**
	*@brief ͬ�����Ա仯
	*/
	void SyncProperty();

public:	//����������
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
	// hpһֱ�ڱ仯,����recored��ʱ��DB modified by huchenhui 
	//CL_DBPROPERTY("hp",			m_Hp)
	CL_DBPROPERTY("skills",		m_SkillMgr)
	CL_DBPROPERTY("buffs",		m_BuffMgr)
	CL_DBPROPERTY_END()

	DEFINE_CHANGESCENE_SERIALIZATION(SceneObject)
	{
		stream & m_SavebackTime;
		stream & m_SkillMgr & m_BuffMgr;
	}

	//�����Ч����
	void ClearInvalidData();
	
private:
	//�ȼ�
	OwnerObjUInt16	m_Level;
	//�Ա�
	OwnerObjUInt8	m_Sex;
	//״̬
	OwnerObjUInt8	m_Status;
	
	//�ƶ��ٶ�
	OwnerObjUInt16	m_MoveSpeed;

	//����
	PlayerSkillMgr	m_SkillMgr;
	//buff
	BuffMgr		m_BuffMgr;

	//�浵ʱ��
	UInt64		m_SavebackTime;

    // �Ƿ�����
    bool        m_isHidden;
};

#endif

