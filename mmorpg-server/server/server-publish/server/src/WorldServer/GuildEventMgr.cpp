#include "GuildEventMgr.h"

#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>

GuildEventMgr::GuildEventMgr()
{

}

GuildEventMgr::~GuildEventMgr()
{
	eventVec.clear();
}

void GuildEventMgr::LoadGuildEvent(GuildEvent* guildEvent)
{
	eventVec.push_back(guildEvent);
}

bool GuildEventMgr::addGuildEvent(std::string data)
{
	if (data.empty())
	{
		return false;
	}

	GuildEvent* guildEvent = new GuildEvent();
	if (NULL == guildEvent)
	{
		return false;
	}

	guildEvent->eventInfo = data;
	guildEvent->addTime = (UInt32)CURRENT_TIME.Sec();
	eventVec.push_front(guildEvent);
	InsertDBData(guildEvent);

	UInt32 showMax = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EVENT_SHOW_MAX);
	if ((UInt32)eventVec.size() > showMax)
	{
		GuildEvent* delGuildEvent = eventVec.back();
		if (delGuildEvent)
		{
			DelOneDBData(delGuildEvent->guid);
		}
		eventVec.pop_back();
		CL_SAFE_DELETE(delGuildEvent);
	}

	return true;
}

void GuildEventMgr::GetGuildEvent(std::vector<GuildEvent>& guildEvents, UInt32 upTime)
{
	UInt32 index = 0;
	UInt32 showMax = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EVENT_SHOW_MAX);

	for (auto iter = eventVec.begin(); iter != eventVec.end(); ++iter)
	{
		if (upTime != 0 && (*iter)->addTime < upTime)
		{
			break;
		}

		GuildEvent guildEvent;
		guildEvent.eventInfo = (*iter)->eventInfo;
		guildEvent.addTime = (*iter)->addTime;
		guildEvents.push_back(guildEvent);

		if (++index >= showMax)
		{
			break;
		}
	}
}

void GuildEventMgr::InsertDBData(GuildEvent* guildEvent)
{
	if (NULL == guildEvent)
	{
		return;
	}
	if (GetGuildID() == 0)
	{
		ErrorLogStream << "GuildEventMgr::InsertDBData() GetGuildID = 0" << LogStream::eos;
		return;
	}

	if (guildEvent->guid == 0)
	{
		guildEvent->guid = CLRecordClient::Instance()->GenGuid();
	}

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_event", guildEvent->guid);
	cmd->PutData("guild_id", GetGuildID());
	cmd->PutData("add_time", guildEvent->addTime);
	cmd->PutData("event_info", guildEvent->eventInfo);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildEventMgr::DelDBData()
{
	std::stringstream str;
	str << " `guild_id`='" << GetGuildID() << "'";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_event", str.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildEventMgr::DelOneDBData(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_event", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}