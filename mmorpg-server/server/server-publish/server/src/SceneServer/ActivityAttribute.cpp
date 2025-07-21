#include "ActivityAttribute.h"

#include <CLRecordClient.h>
#include <CLOpActiveProtocol.h>

#include "Player.h"


Int64 ActivityVarMap::GetVar(const std::string& key) const
{
	if (key.empty())
	{
		ErrorLogStream << "Key is empty!" << LogStream::eos;
		return 0;
	}

	auto itr = m_StringVars.find(key);
	if (itr == m_StringVars.end())
	{
		return 0;
	}

	return Avalon::StringUtil::ConvertToValue<Int64>(itr->second);
}

void ActivityVarMap::SetVar(const std::string& key, const std::string& value)
{
	if (key.empty())
	{
		ErrorLogStream << "Key is empty!" << LogStream::eos;
		return;
	}

	auto itr = m_StringVars.find(key);
	if (itr != m_StringVars.end())
	{
		if (itr->second == value)
		{
			return;
		}
		itr->second = value;
		SetDirty();
	}
	else
	{
		m_StringVars.insert(std::make_pair(key, value));
		SetDirty();
	}
}

void ActivityVarMap::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_StringVars;
}

void ActivityVarMap::Decode(Avalon::NetInputStream& stream)
{
	stream & m_StringVars;
}

void ActivityVarMap::DecodeString(std::istringstream& is)
{
	if (is.str().empty())
	{
		return;
	}

	std::vector<std::string> tmpVar;
	Avalon::StringUtil::Split(is.str(), tmpVar, ",");
	for (const auto& elem : tmpVar)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(elem, pair, "=");
		if (pair.size() < 2) continue;

		m_StringVars.insert(std::make_pair(pair[0], pair[1]));
	}
}

void ActivityVarMap::EncodeString(std::ostringstream& os)
{
	for (const auto& elem : m_StringVars)
	{
		if (!os.str().empty())
		{
			os << ",";
		}
		os << elem.first << "=" << elem.second;

		if (os.str().size() > 1024)
		{
			ErrorLogStream << "Activity var map size(" << os.str() << ") is reach max(512)!" << LogStream::eos;

			std::string overStr = os.str().substr(0, 1024);
			auto pos = overStr.find_last_of(",");
			std::string correctStr = os.str().substr(0, pos + 1);
			os.str(correctStr);
			break;
		}
	}
}

const std::string ActivityVarMap::LogStr()
{
	std::ostringstream oss;
	for (const auto& elem : m_StringVars)
	{
		if (!oss.str().empty())
		{
			oss << ",";
		}
		oss << elem.first << "=" << elem.second;
	}
	return oss.str();
}


OpActivityAttribute::OpActivityAttribute()
{
	m_OpActId = 0;
	m_Owner = 0;
	m_CreateTime = 0;
	m_RefreshTime = 0;
}

void OpActivityAttribute::SetVar(Player* player, const std::string& key, Int64 value)
{
	std::string valueStr = Avalon::StringUtil::ConvertToString<Int64>(value);
	m_VarMap.SetVar(key, valueStr);
	SyncVar(player, key, valueStr);
	SaveToDB();
}

void OpActivityAttribute::IncVar(Player* player, const std::string& key, Int64 incValue)
{
	if (incValue == 0) return;

	Int64 oldValue = GetVar(key);
	Int64 newValue = oldValue + incValue;
	SetVar(player, key, newValue);
}

void OpActivityAttribute::IncVarNoSync(const std::string& key, Int64 incValue)
{
	if (incValue == 0) return;

	Int64 oldValue = GetVar(key);
	Int64 newValue = oldValue + incValue;
	SetVarNoSync(key, newValue);
}

void OpActivityAttribute::SetVarNoSync(const std::string& key, Int64 value)
{
	std::string valueStr = Avalon::StringUtil::ConvertToString<Int64>(value);
	m_VarMap.SetVar(key, valueStr);
	SaveToDB();
}

Int64 OpActivityAttribute::GetVar(const std::string& key) const
{
	if (key.empty())
	{
		ErrorLogStream << "Key is empty!" << LogStream::eos;
		return 0;
	}

	return m_VarMap.GetVar(key);
}

void OpActivityAttribute::SyncVar(Player* player, const std::string key, const std::string& value)
{
	if (!player) return;

	CLProtocol::SceneOpActivityVarSync protocol;
	protocol.opActId = GetOpActId();
	protocol.key = key;
	protocol.value = value;
	player->SendProtocol(protocol);
}

bool OpActivityAttribute::TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate)
{
	if (refreshType == TimeUtil::REFRESH_TYPE_NONE)
	{
		return false;
	}

	UInt64 curTimeMsec = CURRENT_TIME.MSec();

	if (GetRefreshTime() > curTimeMsec)
	{
		return false;
	}

	UInt64 nextRefreshTimestamp = 0;
	if (!TimeUtil::CalculateNextRefreshTimestamp(refreshType, refreshDate, nextRefreshTimestamp))
	{
		ErrorLogStream << "Calc next refresh timestamp failed by refresh type(" << (UInt32)refreshType << ") and ("
			<< refreshDate.LogStr() << ") for " << LogStr() << LogStream::eos;
		return false;
	}

	SetRefreshTime(nextRefreshTimestamp);
	SaveToDB();
	return true;
}

const std::string OpActivityAttribute::LogStr()
{
	std::ostringstream oss;
	oss << "opActivity(" << GetOpActId() << ")'s attribute(" << GetID() << ") refresh time:"
		<< GetRefreshTime() << ", vars:" << m_VarMap.LogStr();
	return oss.str();
}

void OpActivityAttribute::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_activity_op_attribute", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_activity_op_attribute", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}

