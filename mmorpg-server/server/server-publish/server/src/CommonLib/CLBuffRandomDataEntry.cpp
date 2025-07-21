/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLBuffRandomDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <random>
#include <algorithm>
#include "DropItemMgr.h"
// __CUSTOM_HEADER_END__

BuffRandomDataEntry::BuffRandomDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

BuffRandomDataEntry::~BuffRandomDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 BuffRandomDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool BuffRandomDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	buffId = row.ReadUInt32();
	weight = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool BuffRandomDataEntryMgr::AddEntry(BuffRandomDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BuffRandomDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
Int32 BuffRandomDataEntryMgr::RandBuff(const std::vector<UInt32>& uids)
{
	std::vector<BuffRandomDataEntry*> datas;

	std::vector<UInt32> randUids = uids;

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(randUids.begin(), randUids.end(), g);

	for (auto uid : randUids)
	{
		auto data = FindEntry(uid);
		if (!data)
		{
			ErrorLogStream << "Can not find buff random data(" << uid << ")!" << LogStream::eos;
			continue;
		}

		datas.push_back(data);
	}

	std::vector<UInt32> wts;

	for (auto data : datas)
	{
		if (!data) continue;
		wts.push_back(data->weight);
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= (Int32)datas.size() || hitIdx < 0)
	{
		ErrorLogStream << "Rand failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return -1;
	}

	auto data = datas[hitIdx];
	if (!data)
	{
		ErrorLogStream << "Rand failed, data entry ptr is null!" << LogStream::eos;
		return -1;
	}

	return data->buffId;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

