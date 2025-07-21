#include "TCPlayerMonitorMgr.h"
#include "TCPlayerMgr.h"
#include "TCPlayer.h"
#include <AvalonStringUtil.h>

void TCPlayerMonitorMgr::LoadFromString(const std::string& str)
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

void TCPlayerMonitorMgr::OnPlayerAddToMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") add to monitor list..." << LogStream::eos;
	auto player = TCPlayerMgr::Instance()->find_player(accid);
	if (player)
	{
		player->SetMonitored(true);
	}
}

void TCPlayerMonitorMgr::OnPlayerDelFromMonitor(UInt32 accid)
{
	DebugLogStream << "account(" << accid << ") remove from monitor list..." << LogStream::eos;
	auto player = TCPlayerMgr::Instance()->find_player(accid);
	if (player)
	{
		player->SetMonitored(false);
	}
}