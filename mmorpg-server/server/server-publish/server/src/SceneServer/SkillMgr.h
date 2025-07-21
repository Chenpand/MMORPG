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
	 *@brief תΪΪ��������
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
 *@brief ����
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
		//�ȼ�
		level = rsh.level;
		//�Ƿ���
		bdirty = true;
		//���ܱ������
		dataEntry = rsh.dataEntry;
	}

	Skill & operator=(const Skill & rsh)
	{
		//id
		id = rsh.id;
		//�ȼ�
		level = rsh.level;
		//�Ƿ���
		bdirty = true;
		//���ܱ������
		dataEntry = rsh.dataEntry;
		return *this;
	}

public:
	/**
	 *@brief ��ʼ��                                                                     
	 */
	bool Init();

	/**
	 *@brief �Ƿ���ɾ��
	 */
	bool IsValid(){ return level != 0; }

public://�߽���
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
	//�ȼ�
	UInt8	level;
	//�Ƿ���
	bool	bdirty;

	//���ܱ������
	SkillDataEntry*	dataEntry;
};

//���ܷ�����
typedef CLVisitor<Skill>	SkillVisitor;

/**
 *@brief ��ҳ���ܹ�����
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
	 *@brief ����һ������
	 */
	Skill* FindSkill(UInt16 id);

	/**
	*@brief �����ü����Ƿ�����
	*/
	bool CheckNextSkill(UInt16 id, UInt8 level);

	/**
	 *@brief �������м���
	 */
	void VisitSkills(SkillVisitor& visitor,bool bAll = false);

public:
	/**
	 *@brief ����������һ������ by huchenhui
	 */
	UInt32 ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify = false, bool isConsume = true);

	/**
	*@brief ���ü��� by huchenhui
	*/
	UInt32 ForgetAllSkills(bool isConsume = false);

	/**
	*@brief ���Ҫ�ı�ļ����Ƿ��������� by huchenhui
	*/
	UInt32 CheckChangeSkills(Player* player, std::vector<ChangeSkill> skills, SkillMap& skillMap, Int32& sp, UInt32& removeGlod, bool isConsume = true);

	/**
	*@brief ��鼼�ܵȼ�,ְҵ�޶� by huchenhui
	*/
	UInt32 CheckSkill(Player* player, SkillDataEntry* dataEntry, Int32 skillLevel, UInt32 initLevel, Int8 difLevel);

	/**
	*@brief ��鼼�ܹ�ϵ  by huchenhui
	*/
	UInt32 CheckSkillRelation(SkillMap skillMap);

	/**
	*@brief ���ǰ�ü���  by huchenhui
	*/
	UInt32 CheckPreSkill(SkillMap skillMap, Skill skill);

	/**
	*@brief �����ü���  by huchenhui
	*/
	UInt32 CheckPostSkill(SkillMap skillMap, Skill skill);

	/**
	*@brief ��ʼ������ by zhengfeng
	*/
	UInt32 InitAllSkills();

	/**
	*@brief �Ƽ��������� by zhengfeng
	*/
	UInt32 RecommendSkills(bool isStudy = false);

	/**
	*@brief ��ռ��� by huchenhui
	*/
	void ClearSkills();

	/**
	*@brief ���Ԥתְ���� by huchenhui
	*/
	void ClearPreOccuSkills();

	/**
	*@brief ��ȡ����ʹ�õ�SP��
	*/
	UInt32 GetSkillSP();

	/**
	*@brief �ͷ��ܹ����õ��������� by huchenhui
	*/
	bool IsSetting(SkillDataEntry* dataEntry);

	/**
	*@brief ���ü����б�
	*/
	void SetSkills(SkillMap skills) { m_Skills = skills; }

	/**
	*@brief ���ü���
	*/
	void AddFreeSkill(UInt16 id, UInt8 level = 1);
	void DelFreeSkill(UInt16 id);

	/**
	*@brief ��ȡ���Ἴ�ܼӳ�
	*/
	void GetGuildSkillDungeonAddition(DungeonAddition& addition);

	/**
	*@brief ���õ�����
	*/
	void AddStatueSkill(SystemValueType systemValueType);
	void DelStatueSkill(SystemValueType systemValueType);

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ����
	*/
	void OnOnline();

	/**
	*@brief תְ����   by huchenhui
	*/
	void OnChangeOccu();

	/**
	*@brief ��������   by huchenhui
	*/
	void OnBirth();

	/**
	*@brief ���� by huchenhui
	*/
	void OnAwaken();

	/**
	*@brief ���� by huchenhui
	*/
	void OnLevelUp();

	/**
	*@brief ���SP�ͼ���
	*/
	void OnCheckSkillSP(bool skipCheck = false);

	/**
	*@brief �������
	*/
	void OnTask(UInt32 id);

	/**
	*@brief �������������ܲ�λ
	*/
	void CheckTaskUnlockSlot();

	/**
	*@brief ���������λ
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

public: //����
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
	//ӵ����
	Creature*	m_pOwner;
	//���м���
	SkillMap	m_Skills;

	//�������÷���
	SkillConfigType m_SkillConfigType;

	//tickʱ��
	Avalon::SimpleTimer m_Timer;

	UInt8 m_Page;
};




/**
 *@brief ���ܹ�����
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
	*@brief ���ü���
	*/
	UInt32 ForgetAllSkills(bool isConsume = false);

	/**
	*@brief ���Ԥתְ����
	*/
	void ClearPreOccuSkills();

	/**
	 *@brief ����������һ������
	 */
	UInt32 ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify = false, bool isConsume = true);

	/**
	*@brief ��ʼ������
	*/
	UInt32 InitAllSkills();

	/**
	*@brief �Ƽ���������
	*/
	UInt32 RecommendSkills(bool isStudy = false);

	/**
	*@brief ��ȡ���Ἴ�ܼӳ�
	*/
	void GetGuildSkillDungeonAddition(DungeonAddition& addition);

	/**
	*@brief ���ü���
	*/
	void AddFreeSkill(UInt16 id, UInt8 level = 1);
	void DelFreeSkill(UInt16 id);

	/**
	 *@brief ����һ������
	 */
	Skill* FindSkill(UInt16 id);

	/**
	*@brief ���õ�����
	*/
	void AddStatueSkill(SystemValueType systemValueType);
	void DelStatueSkill(SystemValueType systemValueType);


	/**
	*@brief �ͷ��ܹ����õ���������
	*/
	bool IsSetting(SkillDataEntry* dataEntry);

	/**
	 *@brief �������м���
	 */
	void VisitSkills(SkillVisitor& visitor, bool bAll = false);

	/**
	*@brief �������������ܲ�λ
	*/
	void CheckTaskUnlockSlot();
	/**
	*@brief ���������λ
	*/
	bool IsTaskUnlockSlot();

	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ����
	*/
	void OnOnline();

	/**
	*@brief תְ����   by huchenhui
	*/
	void OnChangeOccu();

	/**
	*@brief ��������   by huchenhui
	*/
	void OnBirth();

	/**
	*@brief ���� by huchenhui
	*/
	void OnAwaken();

	/**
	*@brief ���� by huchenhui
	*/
	void OnLevelUp();

	/**
	*@brief �������
	*/
	void OnTask(UInt32 id);

	void CopyWithoutConfigType(const PlayerSkillMgr & rsh);

	/**
	*@brief ͬ����һҳ���Ἴ�ܵ�����ҳ
	*/
	void SyncGuildSkillToAllPage();


protected:
	void SetConfigType(SkillConfigType type);

private:
	//����ҳ�ļ���,�±���Ϊҳ��
	std::vector<SkillMgr> m_SkillMgrVec;
	//ӵ����
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
