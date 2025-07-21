#ifndef _CL_CHANNEL_NAME_SWITCH_DATA_ENTRY_
#define _CL_CHANNEL_NAME_SWITCH_DATA_ENTRY_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
*@brief 渠道名称切换表
*/
class ChannelNameSwitchDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return uid; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 3; };

public:
	// uid
	UInt32 uid;
	// 渠道英文名
	std::string EnglishName;
	// 渠道中文名
	std::string ChineseName;
};

class ChannelNameSwitchDataEntryMgr : public Avalon::DataEntryMgrBase<ChannelNameSwitchDataEntry>
	, public Avalon::Singleton<ChannelNameSwitchDataEntryMgr>
{
public:
	bool AddEntry(ChannelNameSwitchDataEntry* dataEntry);
	std::string SwitchToChinese(const std::string& channel);

private:
	std::map<std::string, std::string> m_EN2CNChannels;
};

#endif