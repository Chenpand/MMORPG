#include "WSActivityTask.h"

#include <CLTaskProtocol.h>
#include <AvalonStringUtil.h>
#include <CLRecordClient.h>
#include <CLActiveEventDataEntry.h>

#include "WSPlayer.h"

WSActivityTask::WSActivityTask()
{
	m_Uid = 0;
	m_AccId = 0;
	m_ActivityId = 0;
	m_TaskId = 0;
	m_Status = 0;
	m_AcceptTime = 0;
	m_UpdateTime = 0;
	m_DayChangeTime = 0;
	m_DBFlag = df_none;
}

WSActivityTask::~WSActivityTask()
{

}

void WSActivityTask::SetStatus(WSPlayer* player, UInt8 status)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	if (m_Status != status)
	{
		m_Status = status;

		SyncStatus(player);

		if (m_DBFlag == df_none) m_DBFlag = df_update;
	}
}

void WSActivityTask::SetStatusNoSync(UInt8 status)
{
	m_Status = status;

	if (m_DBFlag == df_none) m_DBFlag = df_update;
}

void WSActivityTask::SyncStatus(WSPlayer* player)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::SceneNotifyActiveTaskStatus protocol;
	protocol.taskId = GetTaskId();
	protocol.status = GetStatus();
	player->SendProtocol(protocol);

	DebugLogStream << PLAYERINFO(player) << "task(" << protocol.taskId << ") status(" << protocol.status << ")" << LogStream::eos;
}

void WSActivityTask::SetVar(WSPlayer* player, const std::string& key, Int64 val)
{
	if (!player || key.empty())
	{
		ErrorLogStream << "WSPlayer ptr is null or key is empty!" << LogStream::eos;
		return;
	}

	m_Vars[key] = Avalon::StringUtil::ConvertToString(val);

	SyncVar(player, key, val);

	if (m_DBFlag == df_none) m_DBFlag = df_update;
}

void WSActivityTask::SetVarNoSync(const std::string& key, Int64 val)
{
	if (key.empty())
	{
		ErrorLogStream << "key is empty!" << LogStream::eos;
		return;
	}

	m_Vars[key] = Avalon::StringUtil::ConvertToString(val);

	if (m_DBFlag == df_none) m_DBFlag = df_update;
}

Int64 WSActivityTask::GetVar(const std::string& key) const
{
	if (key.empty())
	{
		ErrorLogStream << "key is empty!" << LogStream::eos;
		return 0;
	}

	auto iter = m_Vars.find(key);
	if (iter == m_Vars.end())
	{
		return 0;
	}

	return Avalon::StringUtil::ConvertToValue<Int64>(iter->second);
}

void WSActivityTask::SyncVar(WSPlayer* player, const std::string& key, Int64 val)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::SceneNotifyActiveTaskVar protocol;
	protocol.taskId = GetTaskId();
	protocol.key = key;
	protocol.value = Avalon::StringUtil::ConvertToString(val);

	player->SendProtocol(protocol);

	InfoLogStream << PLAYERINFO(player) << "task(" << protocol.taskId << ") key(" << protocol.key << ", value(" << protocol.value << ")" << LogStream::eos;
}

void WSActivityTask::SetDayChangeTime(UInt32 time)
{
	if (m_DayChangeTime != time)
	{
		m_DayChangeTime = time;
		if (m_DBFlag == df_none) m_DBFlag = df_update;
	}
}

const std::string WSActivityTask::LogStr() const
{
	std::ostringstream oss;
	oss << "Account(" << GetAccId() << ") task(" << GetUID() << "," << GetTaskId() << ") status(" << (UInt32)GetStatus() << ")" << LogStream::eos;
	return oss.str();
}

void WSActivityTask::Refresh(WSPlayer* player)
{
	if (!player) return;

	for (auto elem : m_Vars)
	{
		SetVar(player, elem.first, 0);
	}
	SetStatus(player, TASK_UNFINISH);
	SaveToDB();
	DebugLogStream << PLAYERINFO(player) << " refresh activity(" << GetActivityId() << ")'s " << LogStr() << LogStream::eos;
}

void WSActivityTask::RefreshNoSync()
{
	for (auto elem : m_Vars)
	{
		SetVarNoSync(elem.first, 0);
	}
	SetStatusNoSync(TASK_UNFINISH);
	SaveToDB();
	InfoLogStream << "Player(" << GetAccId() << ") refresh activity(" << GetActivityId() << ")'s  " << LogStr() << LogStream::eos;
}

void WSActivityTask::DecodeVar(const std::string& str)
{
	std::vector<std::string> valuePairs;
	Avalon::StringUtil::Split(str, valuePairs, ",");

	for (auto elem : valuePairs)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(elem, pair, "=");
		if (pair.size() < 2)
		{
			continue;
		}

		if (m_Vars.find(pair[0]) != m_Vars.end())
		{
			ErrorLogStream << "The same var key(" << pair[0] << ")!" << LogStream::eos;
			continue;
		}

		m_Vars[pair[0]] = pair[1];
	}
}

std::string WSActivityTask::EncodeVar()
{
	std::string result;
	for (auto var : m_Vars)
	{
		result = result + var.first + "=" + var.second + ',';
	}
	return result;
}

void WSActivityTask::InsertToDB()
{
	SetUID(CLRecordClient::Instance()->GenGuid());

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_activity_acc_task", GetUID());
	cmd->PutData("accid", GetAccId());
	cmd->PutData("activity_id", GetActivityId());
	cmd->PutData("task_id", GetTaskId());
	cmd->PutData("status", GetStatus());
	cmd->PutData("params", EncodeVar());
	cmd->PutData("accept_time", GetAcceptTime());
	cmd->PutData("update_time", GetUpdateTime());
	cmd->PutData("day_change_time", GetDayChangeTime());
	CLRecordClient::Instance()->SendCommand(cmd);

	InfoLogStream << "account(" << GetAccId() << ")  insert activity(" << GetActivityId() << ", " << GetTaskId() << ", " << GetUID() << ") to db." << LogStream::eos;
}

void WSActivityTask::UpdateToDB()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_activity_acc_task", GetUID(), false);
	cmd->PutData("status", GetStatus());
	cmd->PutData("params", EncodeVar());
	cmd->PutData("update_time", GetUpdateTime());
	cmd->PutData("day_change_time", GetDayChangeTime());
	CLRecordClient::Instance()->SendCommand(cmd);

	InfoLogStream << "account(" << GetAccId() << ")  update activity(" << GetActivityId() << ", " << GetTaskId() << ", " << GetUID() << ") to db." << LogStream::eos;
}

void WSActivityTask::SaveToDB(bool canUpdateTime)
{
	if (canUpdateTime && m_DBFlag != df_none)
	{
		SetUpdateTime((UInt32)CURRENT_TIME.Sec());
	}

	switch (m_DBFlag)
	{
	case df_insert:
		InsertToDB(); break;

	case df_update:
		UpdateToDB(); break;

	default:
		break;
	}

	m_DBFlag = df_none;
}