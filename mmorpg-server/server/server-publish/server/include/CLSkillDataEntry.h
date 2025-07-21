#ifndef _CL_SKILL_DATAENTRY_H_
#define _CL_SKILL_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemTypeDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <sstream>

#define CL_MAKE_AWAKEN_KEY(Awaken,Occu) ((UInt32(Awaken) << 16) + Occu)

/**
*@brief ������������
*/
enum SkillWeaponFlag
{
	SKILL_WEAPON_AXE = 1 << 0,	//��
	SKILL_WEAPON_SWARD = 1 << 1,	//��
	SKILL_WEAPON_CANE = 1 << 2,	//��
	SKILL_WEAPON_BOW = 1 << 3,	//��
};

/**
*@brief ����װ�����ͻ�ȡ������������
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
	//����ID
	UInt16	id;
	//���ܵȼ�
	UInt8	level;

};
typedef std::vector<SkillDataLevel>	NeedSkillVec;


/**
*@brief �������ñ�
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
	//����ID
	UInt16			id;
	//��������
	std::string		name;
	//ְҵ
	OccuVec			occus;
	//����ʹ������
	UInt8			useType;
	//��������
	UInt8			type;
	//�Ƿ�buff
	bool			isBuff;
	//�Ƿ�QTE����
	bool			isQTE;
	//����pvpЯ����ʽ
	UInt8           canUseInPVP;
	//buffer�����б�
	std::vector<UInt32> buffInfoList;
	//�Ƿ�Ԥתְ����
	bool			isPreJob;
	//�Ƿ��Զ�ѧϰ
	bool			isStudy;
	//��ҵȼ�
	UInt16			playerLevel;
	//������ҵȼ����
	UInt8			upLevelInterval;
	//��ߵȼ�
	UInt8			maxLevel;
	//ǰ�ü���
	NeedSkillVec	needSkills;
	//����SP
	UInt32			needSP;
	//�Ƿ��Ȼͬ��
	bool			isForceSync;

};

/**
*@brief �������ñ�
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
//����id
UInt16		id;
//��������
std::string	name;
//ְҵ
UInt8		occu;
//ʹ�÷�ʽ
UInt8		usetype;
//���ܽ�����(���ڵ�ͼ�Լ��ܵ�����)
UInt8		disablegroup;
//��������
UInt8		type;
//��Ҫbuff
UInt16		needbuff;
//��Ҫ��������
UInt16		weaponflag;
//����cd��
UInt8		cdgroup;
//����cd(����)
UInt32		groupcd;
//�赲����
UInt8		blockflag;
//Ŀ�귶Χ
UInt8		scope;
//��Χ��״����1
UInt8		scopeshapeparam1;
//��Χ��״����2
UInt8		scopeshapeparam2;
//Ŀ����������
UInt16		targetflag;
//Ŀ���������
UInt8		distancefloor;
//Ŀ���������
UInt8		distancetop;
//0���� 1���� 2�к�
UInt8		pkattr;
//ѧϰ���Ƿ��Զ���ӵ��һ����ܣ�0 ��1 ��
UInt8		autohook;
};
*/

/**
*@brief �������ñ��������
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
	//���ü���Map	��ʾĳ�����ܵĺ��ü����б�
	SkillIdVecMap m_PostSkills;

	//���Ѽ���Map	��ʾĳְҵ�ľ��Ѽ���
	AwakenSkillMap m_AwakenSkills;

	//�Զ�ѧϰ����
	AutoStudySkillMap m_AutoStudySkills;
};

#endif
