/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLStrenTicketFuseMaterialDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

StrenTicketFuseMaterialDataEntry::StrenTicketFuseMaterialDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

StrenTicketFuseMaterialDataEntry::~StrenTicketFuseMaterialDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 StrenTicketFuseMaterialDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool StrenTicketFuseMaterialDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	material = row.ReadString();
	pickCondOfStrenLv = row.ReadString();

// __CUSTOM_READ_BEGIN__
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(material, strs, "|");

		for (auto& elem : strs)
		{
			std::vector<std::string> paramStr;
			Avalon::StringUtil::Split(elem, paramStr, "_");

			if (paramStr.size() != 2)
			{
				ErrorLogStream << "Error reward's format(" << material << ")!" << LogStream::eos;
				return false;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());

			costMaterials.push_back(reward);
		}
	}

	{
		std::vector<std::string> paramStr;
		Avalon::StringUtil::Split(pickCondOfStrenLv, paramStr, "_");

		if (paramStr.size() != 2)
		{
			ErrorLogStream << "Error reward's format(" << pickCondOfStrenLv << ")!" << LogStream::eos;
			return false;
		}

		lowerStrenLevelRange = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
		upperStrenLevelRange = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool StrenTicketFuseMaterialDataEntryMgr::AddEntry(StrenTicketFuseMaterialDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<StrenTicketFuseMaterialDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
ItemRewardVec StrenTicketFuseMaterialDataEntryMgr::GetFuseMaterialsByStrenLevel(UInt32 strenLevel)
{
	class DataVisitor : public Avalon::DataEntryVisitor<StrenTicketFuseMaterialDataEntry>
	{
	public:
		DataVisitor(UInt32 lv) : m_StrenLevel(lv) {}

		bool operator()(StrenTicketFuseMaterialDataEntry* data)
		{
			if (!data) return true;

			if (data->lowerStrenLevelRange <= m_StrenLevel && m_StrenLevel <= data->upperStrenLevelRange)
			{
				costMaterials = data->costMaterials;
				return false;
			}

			return true;
		}

		ItemRewardVec costMaterials;

	private:
		UInt32 m_StrenLevel;
	};

	DataVisitor visitor(strenLevel);
	Visit(visitor);

	return visitor.costMaterials;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

