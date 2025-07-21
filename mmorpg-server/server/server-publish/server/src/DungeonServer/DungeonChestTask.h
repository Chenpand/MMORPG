#ifndef __DUNGEON_CHEST_TASK_H__
#define __DUNGEON_CHEST_TASK_H__

#include <CLDefine.h>
#include <CLDungeonDefine.h>
#include <CLDropItemDefine.h>
#include <AvalonObjectPool.h>

class DungeonChestTask
{
    AVALON_DEFINE_OBJECTPOOL(DungeonChestTask, Avalon::NullMutex)
public:
    DungeonChestTask();
    ~DungeonChestTask();

public:
    inline void SetRoleID(ObjID_t id) { m_roleId = id; }
    inline ObjID_t GetRoleID() const { return m_roleId; }

    inline void SetGoldReward(UInt32 gold) { m_goldReward = gold; }
    inline UInt32 GetGoldReward() const { return m_goldReward; }

	inline void SetPayChestCostItemID(UInt32 id) { m_payChestCostItemId = id; }
	inline UInt32 GetPayChestCostItemID() const { return m_payChestCostItemId; }

    inline void SetPayChestCost(UInt32 cost) { m_payChestCost = cost; }
    inline UInt32 GetPayChestCost() const { return m_payChestCost; }

    bool AddChest(const DungeonChest& chest);
    DungeonChest* GetChest(DungeonChestType type);

    inline const VDungeonChest& GetAllChest() const { return m_chests; }

    inline void SetTimeoutTime(Avalon::Time now) { m_timeoutTime = now; }
    bool IsTimeOut(const Avalon::Time& now) const ;

	inline void SetOpenedNormalChest() { m_openNormalChest = true; }
	inline bool IsOpenedNormalChest() const { return m_openNormalChest; }

private:
    ObjID_t                 m_roleId;
    Avalon::Time            m_timeoutTime;
    UInt32                  m_goldReward;
    VDungeonChest           m_chests;
	UInt32					m_payChestCostItemId;
    UInt32                  m_payChestCost;
	bool					m_openNormalChest;
};
typedef std::vector<DungeonChestTask> VDungeonChestTask;

#endif