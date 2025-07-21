#include "GSPlayerMonitorMgr.h"
#include "GSPlayerMgr.h"

void GSPlayerMonitorMgr::LoadFromString(const std::string& str)
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

void GSPlayerMonitorMgr::OnPlayerAddToMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") add to monitor list..." << LogStream::eos;
	auto player = GSPlayerMgr::Instance()->FindPlayerByAccID(accid);
	if (player)
	{
		player->SetMonitored(true);
	}
}

void GSPlayerMonitorMgr::OnPlayerDelFromMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") remove from monitor list..." << LogStream::eos;
	auto player = GSPlayerMgr::Instance()->FindPlayerByAccID(accid);
	if (player)
	{
		player->SetMonitored(false);
	}
}