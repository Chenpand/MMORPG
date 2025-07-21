#include "CLChannelNameSwitchDataEntry.h"


bool ChannelNameSwitchDataEntry::Read(Avalon::DataRow& row)
{
	uid = row.ReadUInt32();
	EnglishName = row.ReadString();
	ChineseName = row.ReadString();

	return true;
}

bool ChannelNameSwitchDataEntryMgr::AddEntry(ChannelNameSwitchDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<ChannelNameSwitchDataEntry>::AddEntry(dataEntry)) return false;

	m_EN2CNChannels[dataEntry->EnglishName] = dataEntry->ChineseName;

	return true;
}

std::string ChannelNameSwitchDataEntryMgr::SwitchToChinese(const std::string& channel)
{
	auto itr = m_EN2CNChannels.find(channel);
	if (itr != m_EN2CNChannels.end())
	{
		return itr->second;
	}
	return "";
}