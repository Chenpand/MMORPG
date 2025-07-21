#include "RLPlayerMonitorMgr.h"
#include "RLPlayerMgr.h"

void RLPlayerMonitorMgr::LoadFromString(const std::string& str)
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

void RLPlayerMonitorMgr::OnPlayerAddToMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") add to monitor list..." << LogStream::eos;
	auto player = RLPlayerMgr::Instance()->findPlayerByAccount(accid);
	if (player)
	{
		player->SetMonitored(true);
	}
}

void RLPlayerMonitorMgr::OnPlayerDelFromMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") remove from monitor list..." << LogStream::eos;
	auto player = RLPlayerMgr::Instance()->findPlayerByAccount(accid);
	if (player)
	{
		player->SetMonitored(false);
	}
}