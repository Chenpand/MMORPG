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
 *@brief ���ܹ�����
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

public:
	// �Լ�����

	// ѧϰ�Լ�����
	void StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl);
	// ������гԼ�����
	void ClearChiJiSkill();
	// �·��Լ�ѡ����
	void SendChiJiChoiceSkill();
	// ѡ��Լ�����
	void ChoiceChiJiSkill(UInt32 skillId);
	// �Ƿ��ܼ�
	bool IsCanPickSkill();

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
	void OnCheckSkillSP();

protected:

	SkillBar& GetSkillBar();

	UInt32 GetSP();

	bool RemoveSP(const char* reason, UInt32 num);

	void AddSP(const char* reason, UInt32 num);

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

protected:
	//ӵ����
	Creature*	m_pOwner;
	//���м���
	SkillMap	m_Skills;

	//�������÷���
	SkillConfigType m_SkillConfigType;

	// �Լ�����ѡ���б�
	std::map<UInt32, UInt32> m_ChijiSkillChoiceMap;

	// �Լ�����ʰȡ�ļ���
	std::set<UInt32>	m_ChijiBoxSkillSet;

	//tickʱ��
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
