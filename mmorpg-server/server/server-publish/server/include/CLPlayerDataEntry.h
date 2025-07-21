#ifndef _CL_PLAYER_DATAENTRY_H_
#define _CL_PLAYER_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLTaskDefine.h"

#define CL_MAKE_PLAYERDATA_KEY(Level,Occu) ((UInt32(Level) << 16) + Occu)

/**
 *@brief ��ҵȼ�������
 */
struct PlayerDataEntry : public Avalon::DataEntry
{
public:
	PlayerDataEntry();
	~PlayerDataEntry();

public:
	UInt32 GetKey() const
	{
		return level;
	}

	bool Read(Avalon::DataRow& row);

public:
	//�ȼ�
	UInt16	level;
	//��������
	UInt32	upgradeexp;
	//����������SP
	UInt32 sp;

	//�ճ������
	std::vector<DailyTaskSelector> dailyTaskPool;

	//������λ��
	UInt32 skillGridSize;
	//��Ʒ��λ��
	UInt32 itemGridSize;

	//��Ӳ�λ����
	UInt32 retinueSize;


	//����
	UInt32	maxhp;
	//����
	UInt32	attack;
	//����
	UInt32	defense;
	//����
	UInt8	hit;
	//����
	UInt8	dodge;
	//����
	UInt32	thump;
	//�ֵ�
	UInt32	holdoff;
	//����Ч��
	UInt32	thumpeffect;
	//���繥��
	UInt32	chaosAttack;
	//�������
	UInt32	chaosDefense;
	//ѣ��
	UInt32	swim;
	//����
	UInt32	speeddown;
	//����
	UInt32	immobile;
	//��Ĭ
	UInt32	silent;
	//ѣ�ο���
	UInt32	swimDef;
	//���ٿ���
	UInt32	speeddownDef;
	//������
	UInt32	immobileDef;
	//��Ĭ����
	UInt32	silentDef;
	//������������
	UInt32	reviveCost;
	//ð�նӾ���ӳɰٷֱ�
	UInt32 adventureTeamAdditionRate;
	//����
	UInt32 score;
	//���õ�ȯ�»������
	UInt32 creditPointMonthGetLimit;
	//���õ�ȯӵ������
	UInt32 creditPointHoldLimit;
	//���õ�ȯת������
	UInt32 creditPointTransferLimit;
};

//typedef Avalon::DataEntryMgr<PlayerDataEntry> PlayerDataEntryMgr;
class PlayerDataEntryMgr : public Avalon::DataEntryMgrBase<PlayerDataEntry>, public Avalon::Singleton<PlayerDataEntryMgr>
{
public:
	typedef std::map<UInt16, UInt64>	SumExpMap;
	typedef std::map<UInt16, UInt32>	SumSPMap;

	PlayerDataEntryMgr();
	~PlayerDataEntryMgr();

public:
	bool AddEntry(PlayerDataEntry* entry);

	UInt64 GetSumExp(UInt16 level);

	UInt32 GetSumSP(UInt16 level);

	UInt32 GetMaxSkillGridSize();

	UInt16 GetMaxLevel() { return m_MaxLevel; }

	UInt32 GetCreditPointMonthGetLimit(UInt32 level);

	UInt32 GetCreditPointHoldLimit(UInt32 level);

	UInt32 GetCreditPointTransferLimit(UInt32 level);

private:
	SumExpMap			m_SumExpMap;

	SumSPMap			m_SumSPMap;

	UInt32				m_MaxSkillGridSize;

	UInt16				m_MaxLevel;

};

#endif
