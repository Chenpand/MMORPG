/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChijiRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChijiRewardDataEntry::ChijiRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChijiRewardDataEntry::~ChijiRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChijiRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChijiRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	minRank = row.ReadUInt32();
	maxRank = row.ReadUInt32();
	rewardBoxID = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChijiRewardDataEntryMgr::AddEntry(ChijiRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChijiRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
const ChijiRewardDataEntry* ChijiRewardDataEntryMgr::GetChiJiRewardDataEntry(UInt32 rank)
{
	class ChijiRewardDataEntryVistor : public Avalon::DataEntryVisitor<ChijiRewardDataEntry>
	{
	public:
		ChijiRewardDataEntryVistor(UInt32 rank) : rank_(rank), data(NULL) {}
		bool operator()(ChijiRewardDataEntry* dataEntry)
		{
			if (dataEntry->minRank >= rank_ && rank_ >= dataEntry->maxRank)
			{
				data = dataEntry;
				return false;
			}
			return true;
		}

		UInt32 rank_;
		ChijiRewardDataEntry* data;
	};

	ChijiRewardDataEntryVistor vistor(rank);
	Visit(vistor);

	return vistor.data;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

