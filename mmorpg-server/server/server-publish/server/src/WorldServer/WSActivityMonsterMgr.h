#ifndef __WS_ACTIVITY_MONSTER_MGR_H__
#define __WS_ACTIVITY_MONSTER_MGR_H__

#include <CLDefine.h>
#include <CLDungeonAreaDataEntry.h>
#include <AvalonSingleton.h>
#include <CLActivityMonsterDataEntry.h>
#include <CLActivityDefine.h>
#include <AvalonSimpleTimer.h>

#include "DungeonMonsterGenerator.h"

// �����ÿһ����Ϣ
struct WSActivityMonsterRoll
{
	WSActivityMonsterRoll() : activity(false), startTime(0), endTime(0), isOver(false) {}

	bool					activity;
	// ��ʼʱ��
	UInt32					startTime;
	// ����ʱ��
	UInt32					endTime;
	// ��ʾ��ʱ��
	Avalon::SimpleTimer		noticeTimer;
	// �Ƿ��Ѿ�ˢ����
	bool					isOver;
};
typedef std::vector<WSActivityMonsterRoll> VWSActivityMonsterRoll;

// ��ǰ����Ļ����
struct WSActivityMonster
{
	WSActivityMonster() : activity(false) {}

	UInt32 GetDropSetByLevel(UInt32 level)
	{
		for (auto& drop : drops)
		{
			if (drop.minLevel <= level && drop.maxLevel >= level)
			{
				return drop.dropSetId;
			}
		}

		return 0;
	}

	// �Ƿ񼤻�״̬
	bool					activity;
	// �ģ��ID
	UInt32					id;
	// ����
	std::string				name;
	// ���³�ID
	UInt32					dungeonId;
	// ÿ�ֹ�������
	UInt32					perRollMonsterNum;
	// ÿ�ֳ���ʱ��(s)
	UInt32					perRollDurningSec;
	// ˢ�ֵ�����
	DungeonAreaPointType	pointType;
	// ˢ�ֱ�ID
	UInt32					groupId;
	// ����
	UInt32					prob;
	// ����ID
	std::vector<ActivityMonsterDropInfo>	drops;
	// ���ܵ���
	std::vector<DropItem>	mayDropItems;
	// ������Ϣ
	VWSActivityMonsterRoll	rolls;
	// ���ʼ��ʾ
	UInt32					startNotice;
	// ɱ����ʾ
	UInt32					killNotice;
	// ��ˢ����ʾ
	UInt32					clearNotice;
	// �������ʾ
	UInt32					overNotice;
	// ���³ǵȼ�Ҫ��(��С�ȼ�)
	UInt32					needDungeonLevel;
};

class ActivitySuitAdditionDataEntry;
class DungeonPlayer;
class WSActivityMonsterMgr : public Avalon::Singleton<WSActivityMonsterMgr>
{
public:
	WSActivityMonsterMgr();
	~WSActivityMonsterMgr();

public:
	bool Init();
	void OnTick(const Avalon::Time& now);

	void Reload();

	void SetOpen(bool open);

public:
	ActivityMonsterGroup GetMonsterGroupID(UInt32 dungeonId, DungeonAreaPointType type);
	ActivityMonsterInfo GetActivityMonsterInfo(UInt32 id);
	WSActivityMonster* FindMonster(UInt32 id);

	/**
	*@brief ��ȡʱװ�ӳ�
	*/
	ActivitySuitAdditionDataEntry* GetActivitySuitAddition(UInt32 activityMonsterId, DungeonPlayer* player);

	/**
	*@brief ��ȡ�¿��ӳ�
	*/
	ActivitySuitAdditionDataEntry* GetActivityMonthCardAddition(UInt32 activityMonsterId, DungeonPlayer* player);

public:
	void OnMonsterActive(WSActivityMonster& monster);
	void OnMonsterDeActive(WSActivityMonster& monster);

protected:
	std::vector<WSActivityMonster> _GenMonsters();
	UInt32 _GenMonsterDungeonID(UInt32 minLevel, UInt32 maxLevel);

private:
	// �Ƿ��������
	bool									m_Open;
	// �����˵Ļ
	std::vector<WSActivityMonster>			m_Monsters;
};

#endif