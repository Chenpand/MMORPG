#include "DungeonChestTask.h"

DungeonChestTask::DungeonChestTask()
{
    m_roleId = 0;
    m_goldReward = 0;
    m_payChestCost = 0;
	m_openNormalChest = false; 
	m_payChestCostItemId = 0;
}

DungeonChestTask::~DungeonChestTask()
{

}

bool DungeonChestTask::AddChest(const DungeonChest& chest)
{
    // 检测下这个类型的宝箱有没有
    if (GetChest((DungeonChestType)chest.type))
    {
        ErrorLogStream << "repeat to add same type(" << chest.type << ") chest." << LogStream::eos;
        return false;
    }

    if (chest.type >= DCT_NUM)
    {
        ErrorLogStream << "invalid chest type(" << chest.type << ") chest." << LogStream::eos;
        return false;
    }

    m_chests.push_back(chest);

    return true;
}

DungeonChest* DungeonChestTask::GetChest(DungeonChestType type)
{
    for (auto& chest : m_chests)
    {
        if (chest.type == type)
        {
            return &chest;
        }
    }

    return NULL;
}

bool DungeonChestTask::IsTimeOut(const Avalon::Time& now) const
{
    if (m_timeoutTime.MSec() == 0 || IsOpenedNormalChest())
    {
        return false;
    }

    if (now.MSec() > m_timeoutTime.MSec())
    {
        return true;
    }

    return false;
}