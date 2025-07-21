/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiPointDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiPointDataEntry::ChiJiPointDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiPointDataEntry::~ChiJiPointDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiPointDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiPointDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				packIDs.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiPointDataEntryMgr::AddEntry(ChiJiPointDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiPointDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
std::vector<ChiJiPointDataEntry*> ChiJiPointDataEntryMgr::GetPositionVecByPackID(UInt32 packID)
{
	class ChijiPointDataVisitor : public Avalon::DataEntryVisitor<ChiJiPointDataEntry>
	{
	public:
		ChijiPointDataVisitor(UInt32 packID) : m_packID(packID) {}

		bool operator()(ChiJiPointDataEntry* dataEntry)
		{
			//for (auto id : dataEntry->packIDs)
			//{
			//	if (id == m_packID)
			//	{
					m_pointVec.push_back(dataEntry);
			//	}
			//}
			
			return true;
		}

	public:
		UInt32 m_packID;
		std::vector<ChiJiPointDataEntry*> m_pointVec;
	};

	ChijiPointDataVisitor visitor(packID);
	ChiJiPointDataEntryMgr::Instance()->Visit(visitor);

	return visitor.m_pointVec;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

