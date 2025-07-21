/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLEquipBaseScoreDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLItemDataEntry.h"
// __CUSTOM_HEADER_END__

EquipBaseScoreDataEntry::EquipBaseScoreDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

EquipBaseScoreDataEntry::~EquipBaseScoreDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 EquipBaseScoreDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool EquipBaseScoreDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	subType = row.ReadUInt32();
	thirdType = row.ReadUInt32();
	needLevel = row.ReadUInt32();
	color = row.ReadUInt32();
	color2 = row.ReadUInt32();
	suitId = row.ReadUInt32();
	score = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool EquipBaseScoreDataEntryMgr::AddEntry(EquipBaseScoreDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquipBaseScoreDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	
	EquipBaseScoreIndex index;
	index.type = entry->type;
	index.subType = entry->subType;
	index.thirdType = entry->thirdType;
	index.needLevel = entry->needLevel;
	index.color = entry->color;
	index.color2 = entry->color2;
	index.suitId = entry->suitId;

	auto myEntry = GetDataEntry(index);
	if (myEntry)
	{
		ErrorLogStream << "Repetitive data(" << entry->GetKey() << ") with another entry(" << myEntry->GetKey() << ")!" << LogStream::eos;
		return false;
	}

	m_MyDataEntrys.insert(std::make_pair(index, entry));

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

EquipBaseScoreDataEntry* EquipBaseScoreDataEntryMgr::GetDataEntry(const EquipBaseScoreIndex& index)
{
	auto itr = m_MyDataEntrys.find(index);
	if (itr != m_MyDataEntrys.end())
	{
		return itr->second;
	}
	return NULL;
}

EquipBaseScoreDataEntry* EquipBaseScoreDataEntryMgr::GetDataEntryByItemData(const ItemDataEntry* itemData)
{
	if (!itemData) return NULL;

	EquipBaseScoreIndex index;
	index.type = itemData->type;
	index.subType = itemData->subType;
	index.thirdType = itemData->thirdType;
	index.needLevel = itemData->needLevel;
	index.color = itemData->color;
	index.color2 = itemData->color2;
	index.suitId = itemData->suitId;

	return GetDataEntry(index);
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

