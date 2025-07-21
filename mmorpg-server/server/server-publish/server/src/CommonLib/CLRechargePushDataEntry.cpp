/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLRechargePushDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

RechargePushDataEntry::RechargePushDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	daysBeforMin = 0;
	daysBeforeMax = 0;
	pushItemDataID = 0;
	pushItemCount = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

RechargePushDataEntry::~RechargePushDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 RechargePushDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool RechargePushDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	lvLower = row.ReadUInt32();
	lvToplimit = row.ReadUInt32();
	vipLvLower = row.ReadUInt32();
	judgePropID = row.ReadString();
	parameterType = row.ReadUInt32();
	daysBefore = row.ReadString();
	parameter1 = row.ReadUInt32();
	recent = row.ReadUInt32();
	parameter2 = row.ReadUInt32();
	pushID = row.ReadString();
	buyTimes = row.ReadUInt32();
	pushPrice = row.ReadUInt32();
	pushBeforePrice = row.ReadUInt32();
	pushCD = row.ReadUInt32();
	coolCD = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
	std::vector<std::string> judgePropIDStrs;
	Avalon::StringUtil::Split(judgePropID, judgePropIDStrs, "|");
	for (auto iter : judgePropIDStrs)
	{
		UInt32 tmpId = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
		judgePropIDs.push_back(tmpId);
	}

	std::vector<std::string> daysBeforeStrs;
	Avalon::StringUtil::Split(daysBefore, daysBeforeStrs, "|");
	if (daysBeforeStrs.size() == 2)
	{
		daysBeforMin = Avalon::StringUtil::ConvertToValue<UInt32>(daysBeforeStrs[0]);
		daysBeforeMax = Avalon::StringUtil::ConvertToValue<UInt32>(daysBeforeStrs[1]);
	}

	std::vector<std::string> pushIDStrs;
	Avalon::StringUtil::Split(pushID, pushIDStrs, "_");
	if (pushIDStrs.size() == 2)
	{
		pushItemDataID = Avalon::StringUtil::ConvertToValue<UInt32>(pushIDStrs[0]);
		pushItemCount = Avalon::StringUtil::ConvertToValue<UInt32>(pushIDStrs[1]);
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool RechargePushDataEntryMgr::AddEntry(RechargePushDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<RechargePushDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
const RechargePushDataEntry* RechargePushDataEntryMgr::GetDataEntryByPushId(UInt32 pushId)
{
	class RechargePushVisitor : public Avalon::DataEntryVisitor<RechargePushDataEntry>
	{
	public:
		RechargePushVisitor(UInt32 pushId) :m_pushId(pushId), m_dataEntry(NULL){}

		bool operator()(RechargePushDataEntry* dataEntry)
		{
			if (dataEntry->type == m_pushId)
			{
				m_dataEntry = dataEntry;
				return false;
			}
			
			return true;
		}

	public:
		UInt32 m_pushId;
		RechargePushDataEntry* m_dataEntry;
	};
	
	RechargePushVisitor visitor(pushId);
	RechargePushDataEntryMgr::Instance()->Visit(visitor);

	return visitor.m_dataEntry;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

