#include "UCPlayerMonitorMgr.h"
#include "UCPlayerMgr.h"
#include "UCPlayer.h"
#include <AvalonStringUtil.h>

void UCPlayerMonitorMgr::LoadFromString(const std::string& str)
{
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	std::set<UInt32> accids;
	for (auto idStr : strs)
	{
		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(idStr);
		accids.insert(id);
	}

	if (accids == GetMonitorList())
	{
		return;
	}

	SetMonitorList(accids);
}

void UCPlayerMonitorMgr::OnPlayerAddToMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") add to monitor list..." << LogStream::eos;
	auto player = UCPlayerMgr::Instance()->find_player(accid);
	if (player)
	{
		player->SetMonitored(true);
	}
}

void UCPlayerMonitorMgr::OnPlayerDelFromMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") remove from monitor list..." << LogStream::eos;
	auto player = UCPlayerMgr::Instance()->find_player(accid);
	if (player)
	{
		player->SetMonitored(false);
	}
}