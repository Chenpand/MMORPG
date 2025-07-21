#include "CLActivityMonsterProbDataEntry.h"

bool ActivityMonsterProbDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	activityId = row.ReadUInt32();

	{
		auto tagStrs = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(tagStrs, strs, "|");
		for (auto str : strs) 
		{
			UInt32 tag = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			if (tag == 0 || tag >= 32)
			{
				ErrorLogStream << "id(" << id << ") invalid tag(" << tagStrs << ")." << LogStream::eos;
				return false;
			}

			tags.set(tag);
		}
	}

	prob = row.ReadUInt32();
	return true;
}

std::vector<ActivityMonsterProbDataEntry*> ActivityMonsterProbDataEntryMgr::GetDatasByActivity(UInt32 activityId)
{
	class ActivityMonsterProbVistor : public Avalon::DataEntryVisitor<ActivityMonsterProbDataEntry>
	{
	public:
		ActivityMonsterProbVistor(UInt32 activityId) : activityId_(activityId) {}
		bool operator()(ActivityMonsterProbDataEntry* dataEntry)
		{
			if (dataEntry->activityId == activityId_)
			{
				datas.push_back(dataEntry);
			}
			return true;
		}

		UInt32 activityId_;
		std::vector<ActivityMonsterProbDataEntry*> datas;
	};

	ActivityMonsterProbVistor vistor(activityId);
	Visit(vistor);

	return vistor.datas;
}

UInt32 ActivityMonsterProbDataEntryMgr::GetProbByActivity(UInt32 activityId, UInt32 tag)
{
	class ActivityMonsterProbVistor : public Avalon::DataEntryVisitor<ActivityMonsterProbDataEntry>
	{
	public:
		ActivityMonsterProbVistor(UInt32 activityId, UInt32 tag) : activityId_(activityId), tag_(tag), data(NULL) {}
		bool operator()(ActivityMonsterProbDataEntry* dataEntry)
		{
			if (dataEntry->activityId == activityId_ && dataEntry->tags.test(tag_))
			{
				data = dataEntry;
				return false;
			}
			return true;
		}

		UInt32 activityId_;
		UInt32 tag_;
		ActivityMonsterProbDataEntry* data;
	};

	ActivityMonsterProbVistor vistor(activityId, tag);
	Visit(vistor);

	return vistor.data == NULL ? 0 : vistor.data->prob;
}