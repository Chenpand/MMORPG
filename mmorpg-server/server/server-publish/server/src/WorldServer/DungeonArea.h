#ifndef __DUNGEON_AREA_H__
#define __DUNGEON_AREA_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLDropItemDefine.h>
#include <CLMonsterDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLMonsterAttributeTable.h>

#include "DungeonPlayer.h"

struct DungeonMonster
{
    DungeonMonster()
    {
        id = 0;
		areaId = 0;
        typeId = 0;
        isDead = false;
        attrDataEntry = NULL;
        summonerId = 0;
        level = 1;
		mode = 0;
        exp = 0;
        type = MT_MONSTER;
        willSummonMonster = false;
        isHell = false;
        hellWave = 0;
		activityMonsterId = 0;
		dailyMaxDropNum = 0;
		monsterId = 0;
    }

    UInt32 GetExp(DungeonPlayer* dungeonPlayer) const
    {
		// ��ս�����ɱ�־���Ϊ0
		if (dungeonPlayer && dungeonPlayer->IsAssist())
		{
			return 0;
		}

        int expAddition = 0;
        if (!prefixes.empty())
        {
            // ��׺�־����мӳ�
            expAddition += 10;
        }

		if (dungeonPlayer)
        {
			//expAddition += dungeonPlayer->GetExpAddition();
			expAddition += dungeonPlayer->GetDungenAdditionByType(DART_EXP_RATE);
        }

        return exp * (100 + expAddition) / 100;
    }

    UInt32 GetMonsterBaseTypeID()
    {
        return typeId / 10000 * 10000 + typeId % 100;
    }

	UInt32 GetMonsterNormalHardTypeID()
	{
		return GetMonsterBaseTypeID() / 10 * 10 + 1;
	}

    CLProtocol::SceneDungeonMonster GetInfo(ObjID_t roleId)
    {
        CLProtocol::SceneDungeonMonster dungeonMonster;
        dungeonMonster.id = id;
        dungeonMonster.pointId = pointId;
        dungeonMonster.typeId = typeId;
        dungeonMonster.prefixes = prefixes;
        dungeonMonster.summonerId = summonerId;

        for (const auto& item : dropItems)
        {
            if (item.roleId != roleId)
            {
                continue;
            }

            dungeonMonster.dropItems.push_back(item.GetSceneDungeonDropItem());
        }

        return dungeonMonster;
    }

    // ΨһID
    UInt32                          id;
	// AreaId
	UInt32							areaId;
    // λ��ID
    UInt32                          pointId;
    // ����ID
    UInt32                          typeId;
    // ��������
    MonsterType                     type;
	// ����ģʽ
	UInt8							mode;
    // ��������
    MonsterAttributeDataEntry*      attrDataEntry;
    // �Ƿ��Ѿ�����
    bool                            isDead;
    // ����
    VPlayerDropItem                 dropItems;
    // ��׺
    std::vector<UInt8>              prefixes;
    // �ٻ���ID������Ǳ��ٻ������Ļ���
    UInt32                          summonerId;
    // �Ƿ���ٻ�������
    bool                            willSummonMonster;
    // ����ȼ�
    UInt16                          level;
    // �����������
    UInt32                          exp;
    // �����б�
    std::vector<UInt32>             dropSet;
	// ����ǻ����ID
	UInt32							activityMonsterId;
	// ÿ�����������������0�������ޣ�
	UInt32							dailyMaxDropNum;


    /*
        ��Ԩ���
    */
    // �ǲ�����Ԩ��
    bool                            isHell;
    // ��Ԩ���Σ���һ���֣��ڶ�����...��
    UInt8                           hellWave;

	UInt32							monsterId;
};
typedef std::vector<DungeonMonster> VDungeonMonster;

class DungeonArea
{
    AVALON_DEFINE_OBJECTPOOL(DungeonArea, Avalon::NullMutex)
public:
    DungeonArea();
    ~DungeonArea();

    inline void SetID(UInt32 id) { m_dungeonAreaId = id; }
    inline UInt32 GetID() const { return m_dungeonAreaId; }

public:
    void AddMonster(DungeonMonster& monster);
    DungeonMonster* FindMonster(UInt32 id);

    VDungeonMonster& GetAllMonster() { return m_monsters; }

    void GetInfo(UInt64 roleId, CLProtocol::SceneDungeonArea& area);

    PlayerDropItem* FindDropItem(UInt32 itemId);

private:
    UInt32                  m_dungeonAreaId;        // ����ID
    VDungeonMonster         m_monsters;             // �����б�
    std::vector<UInt64>     m_enteredPlayers;       // ����������������

};
typedef std::vector<DungeonArea*> VDungeonArea;

#endif

