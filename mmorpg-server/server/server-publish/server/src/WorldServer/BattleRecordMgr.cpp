#include "BattleRecordMgr.h"

BattleRecordMgr::BattleRecordMgr()
{

}

BattleRecordMgr::~BattleRecordMgr()
{
	Clear();
}

void BattleRecordMgr::Clear()
{
	for (auto entry : m_Entrys)
	{
		if (entry)
		{
			delete entry;
		}
	}

	m_Entrys.clear();
}

void BattleRecordMgr::AddEntry(BattleRecordEntry* entry)
{
	if (!entry)
	{
		return;
	}

	entry->SetIndex(GetSize());
	entry->SetTime((UInt32)CURRENT_TIME.Sec());

	m_Entrys.push_back(entry);
}

UInt32 BattleRecordMgr::GetSize() const
{
	return (UInt32)m_Entrys.size();
}

std::vector<BattleRecordEntry*> BattleRecordMgr::GetRecord(UInt32 startPos, UInt32 count) const
{
	if (startPos == 0)
	{
		return std::vector<BattleRecordEntry*>();
	}

	if (count > GetPreTimeMaxCount())
	{
		count = GetPreTimeMaxCount();
	}

	std::vector<BattleRecordEntry*> allEntry;
	allEntry.reserve(count);

	if (startPos > GetSize())
	{
		startPos = GetSize();
	}

	for (int i = startPos - 1; count > 0 && i >= 0; i--,count--)
	{
		auto entry = m_Entrys[i];
		if (entry)
		{
			allEntry.push_back(entry);
		}
	}

	return allEntry;
}

std::vector<BattleRecordEntry*> BattleRecordMgr::GetSelfRecord(ObjID_t roleId) const
{
	std::vector<BattleRecordEntry*> allEntry;

	for (auto entry : m_Entrys)
	{
		if (entry->IsOwner(roleId))
		{
			allEntry.push_back(entry);
		}
	}

	return allEntry;
}