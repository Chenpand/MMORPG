#include "FigureStatueMgr.h"
#include "WSPlayerMgr.h"
#include "CLRecordClient.h"
#include "WSRouter.h"
#include "WSPlayer.h"
#include "WSSelectInitDataCallback.h"
#include "WSNetwork.h"

#include <CLSyncObjectProtocol.h>


class SelectInitFigureStatueCb : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		if (!NextRow())
		{
			ErrorLogStream << "DB table t_figure_statue is empty!" << LogStream::eos;
			return false;
		}

		do
		{
			FigureStatue* info = FigureStatue::Create();
			info->SetID(GetKey());
			info->SetDBData(this);

			if (info->GetStatueType() >= FST_GUILD_DUNGEON_FIRST && info->GetStatueType() <= FST_GUILD_DUNGEON_THIRD)
			{
				FigureStatueMgr::Instance()->AddGuildDungeonFigureStatue(info);
			}
			else
			{
				FigureStatueMgr::Instance()->AddFigureStatue(info);
			}

		} while (NextRow());

		return true;
	}
};

class SelectFigureStatueRoleInfoCb : public CLRecordCallback
{
public:
	SelectFigureStatueRoleInfoCb(UInt64 roleId, UInt8 statueType, const std::string& guildName, UInt64 guildId = 0)
		: m_RoleID(roleId), m_StatueType(statueType), m_GuildName(guildName),m_GuildId(guildId) {}
	~SelectFigureStatueRoleInfoCb() {}

public:
	void OnFinished()
	{
		if (!NextRow())
		{
			ErrorLogStream << "Failed to select player(" << m_RoleID << ") information!" << LogStream::eos;
			return;
		}

		do
		{
			UInt32 accId = GetData("accid");
			std::string name = GetData("name").GetString();
			UInt8 occu = GetData("occu");
			std::string avatarStr = GetData("avatar").GetString();
			PlayerAvatar avatar = FigureStatueMgr::Instance()->ConvertToPlayerAvatar(avatarStr);

			if (m_StatueType >= FST_GUILD_DUNGEON_FIRST && m_StatueType <= FST_GUILD_DUNGEON_THIRD)
			{
				FigureStatue* statue = FigureStatue::Create();
				statue->SetAccID(accId);
				statue->SetRoleID(m_RoleID);
				statue->SetRoleName(name);
				statue->SetOccu(occu);
				statue->SetAvatar(avatar);
				statue->SetStatueType(m_StatueType);
				statue->SetGuildName(m_GuildName);
				statue->SetGuildId(m_GuildId);

				FigureStatueMgr::Instance()->AddGuildDungeonFigureStatue(statue, true);
			}
			else
			{
				FigureStatueInfo info;
				info.roleId = m_RoleID;
				info.accId = accId;
				info.name = name;
				info.occu = occu;
				info.avatar = avatar;
				info.statueType = m_StatueType;
				info.guildName = m_GuildName;
				FigureStatueMgr::Instance()->OnCreateFigureStatue(info);

				std::string infoStr = FigureStatueMgr::Instance()->GetFigureStatueInfoStream(info);
				InfoLogStream << "Success to create a figure statue(" << m_StatueType << ") from DB." << LogStream::eos;
			}

		} while (NextRow());
	}

	void OnFailed(UInt32 errcode)
	{
		ErrorLogStream << "Failed to select player(" << m_RoleID << ") information! Error code(" << errcode << ")" << LogStream::eos;
	}

private:
	UInt64	m_RoleID;
	UInt8	m_StatueType;
	std::string m_GuildName;
	UInt64 m_GuildId;
};


FigureStatueMgr::FigureStatueMgr()
{	

}

FigureStatueMgr::~FigureStatueMgr()
{
	auto iter = m_IdxFigureStatues.begin();
	while (iter != m_IdxFigureStatues.end())
	{
		FigureStatue* ptr = iter->second;
		if (ptr)
		{
			FigureStatue::Destroy(ptr);
		}

		iter = m_IdxFigureStatues.erase(iter);
	}

	for (auto& iter : m_GuildDungeonFigureStatue)
	{
		for (auto& it : iter.second)
		{
			delete it.second;
			it.second = NULL;
		}

		iter.second.clear();
	}

	m_GuildDungeonFigureStatue.clear();
}

void FigureStatueMgr::Init()
{
	CLSelectCommand* figureCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_figure_statue");
	CLRecordClient::Instance()->SendCommand(figureCmd, new SelectInitFigureStatueCb());
}

void FigureStatueMgr::AddFigureStatue(FigureStatue* statue)
{
	if (!statue)
	{
		ErrorLogStream << "FigureStatue ptr is null!" << LogStream::eos;
		return;
	}

	UInt8 statueType = statue->GetStatueType();

	auto iter = m_IdxFigureStatues.find(statueType);
	if (iter != m_IdxFigureStatues.end())
	{
		ErrorLogStream << "The same figure statue(" << statueType << ")!" << LogStream::eos;
		return;
	}

	m_IdxFigureStatues[statueType] = statue;
}

void FigureStatueMgr::OnCreateFigureStatue(UInt64 roleId, UInt8 statueType, const std::string& guildName)
{
	InfoLogStream << "Request to create a figure statue(roleId: " << roleId << ", statue type: " << statueType << ", guild name: " << guildName << ")." << LogStream::eos;

	if (roleId == 0)
	{
		auto statue = _FindFigureStatueByStatueType(statueType);
		if (!statue)
		{
			ErrorLogStream << "Can not find statue type(roleId: " << roleId << ", statue type: " << statueType << ")!" << LogStream::eos;
			return;
		}

		statue->SetRoleID(roleId);
		statue->UpdateToDB();
		_BroadCastFigureStatues();

		InfoLogStream << "Figure statue(statueType: " << statueType << ") is destroyed." << LogStream::eos;
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
	if (!player)
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutCond("guid", roleId);
		cmd->PutColumn("accid");
		cmd->PutColumn("name");
		cmd->PutColumn("occu");
		cmd->PutColumn("avatar");
		CLRecordClient::Instance()->SendCommand(cmd, new SelectFigureStatueRoleInfoCb(roleId, statueType, guildName));

		return;
	}

	UInt32 accId = player->GetAccId();
	const std::string& name = player->GetName();
	UInt8 occu = player->GetOccu();
	PlayerAvatar avatar = player->GetAvatar();

	FigureStatueInfo info;
	info.roleId = roleId;
	info.accId = accId;
	info.name = name;
	info.occu = occu;
	info.avatar = avatar;
	info.statueType = statueType;
	info.guildName = guildName;
	FigureStatueMgr::Instance()->OnCreateFigureStatue(info);

	std::string infoStr = GetFigureStatueInfoStream(info);
	InfoLogStream << "Success to create a figure statue(" << infoStr << ")." << LogStream::eos;
}

void FigureStatueMgr::OnCreateFigureStatue(const FigureStatueInfo& info)
{
	FigureStatue* statue = NULL;
	bool isNewType = false;

	auto iter = m_IdxFigureStatues.find(info.statueType);
	if (iter != m_IdxFigureStatues.end())
	{
		statue = iter->second;
	}
	else
	{
		statue = FigureStatue::Create();
		statue->SetID(CLRecordClient::Instance()->GenGuid());
		isNewType = true;
	}

	statue->SetAccID(info.accId);
	statue->SetRoleID(info.roleId);
	statue->SetRoleName(info.name);
	statue->SetOccu(info.occu);
	statue->SetAvatar(info.avatar);
	statue->SetStatueType(info.statueType);
	statue->SetGuildName(info.guildName);

	if (isNewType)
	{
		AddFigureStatue(statue);
		statue->InsertToDB();
	}
	else
	{
		statue->UpdateToDB();
	}

	_BroadCastFigureStatues();
}

void FigureStatueMgr::SyncFigureStatues(WSPlayer* player)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::WorldFigureStatueSync protocol;
	_GetFigureStatueInfos(protocol.figureStatue);
	player->SendProtocol(protocol);

	for (auto info : protocol.figureStatue)
	{
		std::string str = GetFigureStatueInfoStream(info);
		DevLogStream << "Sync figure statue(" << str << ")!" << LogStream::eos;
	}
}

void FigureStatueMgr::OnSceneConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL) return;

	CLProtocol::WorldFigureStatueSync protocol;
	_GetFigureStatueInfos(protocol.figureStatue);
	CL_SEND_PROTOCOL(conn, protocol);
}

PlayerAvatar FigureStatueMgr::ConvertToPlayerAvatar(const std::string& avatarStr)
{
	PlayerAvatar avatar;
	std::vector<std::string> avatarInfoes;
	Avalon::StringUtil::Split(avatarStr, avatarInfoes, ",");
	if (avatarInfoes.size() >= 1)
	{
		std::vector<std::string> avatarEquips;
		Avalon::StringUtil::Split(avatarInfoes[0], avatarEquips, "|");
		for (auto& str : avatarEquips)
		{
			avatar.equipItemIDs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
	if (avatarInfoes.size() >= 2)
	{
		avatar.weaponStrengthen = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[1]);
	}

	if (avatarInfoes.size() >= 3)
	{
		avatar.isShoWeapon = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[2]);
	}

	return avatar;
}

std::string FigureStatueMgr::ConverToPlayerAvatarStr(const PlayerAvatar& avatar)
{
	std::stringstream ss;
	bool first = true;
	for (auto id : avatar.equipItemIDs)
	{
		if (first)
		{
			ss << id;
			first = false;
		}
		else
		{
			ss << "|" << id;
		}
	}
	ss << "," << (UInt32)avatar.weaponStrengthen;
	ss << "," << (UInt32)avatar.isShoWeapon;
	std::string avatarStr = ss.str();

	return avatarStr;
}

std::string FigureStatueMgr::GetFigureStatueInfoStream(const FigureStatueInfo& info)
{
	std::ostringstream oss;
	oss << "accId=" << info.accId << ", roleId=" << info.roleId;

	std::string occuStr = Avalon::StringUtil::ConvertToString<UInt8>(info.occu);
	std::string typeStr = Avalon::StringUtil::ConvertToString<UInt8>(info.statueType);
	std::string avatarStr = ConverToPlayerAvatarStr(info.avatar);

	std::string str = oss.str() + ", occu=" + occuStr + ", avatarStr=" + avatarStr + ", statueType=" + typeStr + ", guildName=" + info.guildName;

	return str;
}

void FigureStatueMgr::_GetFigureStatueInfos(std::vector<FigureStatueInfo>& infos)
{
	for (auto idxStatue : m_IdxFigureStatues)
	{
		FigureStatue* statue = idxStatue.second;
		if (!statue)
		{
			ErrorLogStream << "FigureStatue ptr is null!" << LogStream::eos;
			continue;
		}

		if (statue->GetRoleID() == 0)
		{
			continue;
		}

		FigureStatueInfo info;
		info.accId = statue->GetAccID();
		info.roleId = statue->GetRoleID();
		info.name = statue->GetRoleName();
		info.occu = statue->GetOccu();
		info.avatar = statue->GetAvatar();
		info.statueType = statue->GetStatueType();
		info.guildName = statue->GetGuildName();

		infos.push_back(info);
	}
}

FigureStatue* FigureStatueMgr::_FindFigureStatueByStatueType(UInt8 statueType)
{
	for (auto idxStatue : m_IdxFigureStatues)
	{
		FigureStatue* statue = idxStatue.second;
		if (!statue)
		{
			ErrorLogStream << "FigureStatue ptr is null!" << LogStream::eos;
			continue;
		}

		if (statue->GetStatueType() == statueType)
		{
			return statue;
		}
	}

	return NULL;
}

void FigureStatueMgr::_BroadCastFigureStatues()
{
	CLProtocol::WorldFigureStatueSync protocol;
	_GetFigureStatueInfos(protocol.figureStatue);
	Router::BroadcastToPlayer(protocol);
	Router::BroadcastToScene(protocol);

	for (auto info : protocol.figureStatue)
	{
		std::string str = GetFigureStatueInfoStream(info);
		InfoLogStream << "Sync figure statue(" << str << ")!" << LogStream::eos;
	}
}

void FigureStatueMgr::AddGuildDungeonFigureStatue(FigureStatue* statue, bool isSave)
{
	if (!statue)
	{
		ErrorLogStream << "AddGuildDungeonFigureStatue ptr is null!" << LogStream::eos;
		return;
	}

	InfoLogStream <<" AddGuildDungeonFigureStatue statue:"<< statue->GetStatueType()<< LogStream::eos;

	auto iter = m_GuildDungeonFigureStatue.find(statue->GetGuildId());
	if (iter == m_GuildDungeonFigureStatue.end())
	{
		FigureStatueMap figure_statue_map;
		figure_statue_map[statue->GetStatueType()] = statue;

		m_GuildDungeonFigureStatue[statue->GetGuildId()] = figure_statue_map;

		if (isSave)
		{
			statue->SetID(CLRecordClient::Instance()->GenGuid());
			statue->InsertToDB();
		}
	}
	else
	{
		FigureStatueMap& statueMap = iter->second;
		auto it = statueMap.find(statue->GetStatueType());
		if (it != statueMap.end())
		{
			CL_SAFE_DELETE(it->second);
			it->second = statue;

			if (isSave)
				statue->UpdateToDB();
		}
		else
		{
			statueMap[statue->GetStatueType()] = statue;

			if (isSave)
			{
				statue->SetID(CLRecordClient::Instance()->GenGuid());
				statue->InsertToDB();
			}
		}
	}
}

void FigureStatueMgr::CreateGuildDungeonFigureStatue(UInt64 guildId, UInt64 roleId, UInt8 statueType)
{
	InfoLogStream << "create guild dungeon figure statue(roleId:" << roleId << " statue type:" << statueType << " guildid:" << guildId << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
	if (!player)
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutCond("guid", roleId);
		cmd->PutColumn("accid");
		cmd->PutColumn("name");
		cmd->PutColumn("occu");
		cmd->PutColumn("avatar");
		CLRecordClient::Instance()->SendCommand(cmd, new SelectFigureStatueRoleInfoCb(roleId, statueType, "", guildId));
		return;
	}

	FigureStatue* statue = FigureStatue::Create();
	statue->SetAccID(player->GetAccId());
	statue->SetRoleID(roleId);
	statue->SetRoleName(player->GetName());
	statue->SetOccu(player->GetOccu());
	statue->SetAvatar(player->GetAvatar());
	statue->SetStatueType(statueType);
	statue->SetGuildName("");
	statue->SetGuildId(guildId);

	AddGuildDungeonFigureStatue(statue, true);
}

void FigureStatueMgr::ClearGuildDungeonFigureStatue()
{
	for (auto& iter : m_GuildDungeonFigureStatue)
	{
		for (auto& it : iter.second)
		{
			delete it.second;
			it.second = NULL;
		}

		iter.second.clear();
	}

	m_GuildDungeonFigureStatue.clear();

	std::ostringstream cond;
	cond << "statuetype in(" << FST_GUILD_DUNGEON_FIRST << "," << FST_GUILD_DUNGEON_SECOND << "," << FST_GUILD_DUNGEON_THIRD <<")";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_figure_statue", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void FigureStatueMgr::SyncGuildDungeonFigureStatue(WSPlayer* player)
{
	if (NULL == player) return;
	if (player->GetGuildID() == 0) return;

	auto iter = m_GuildDungeonFigureStatue.find(player->GetGuildID());
	if (iter == m_GuildDungeonFigureStatue.end()) 
		return;

	CLProtocol::WorldFigureStatueSync protocol;
	for (auto& it : iter->second)
	{
		FigureStatue* statue = it.second;
		if (statue == NULL) 
			continue;

		if (statue->GetRoleID() == 0)
			continue;

		FigureStatueInfo info;
		info.accId = statue->GetAccID();
		info.roleId = statue->GetRoleID();
		info.name = statue->GetRoleName();
		info.occu = statue->GetOccu();
		info.avatar = statue->GetAvatar();
		info.statueType = statue->GetStatueType();
		info.guildName = statue->GetGuildName();

		protocol.figureStatue.push_back(info);
	}

	player->SendProtocol(protocol);
}

void FigureStatueMgr::GetGuildDungeonFigureStatue(UInt64 guildId, std::vector<FigureStatueInfo>& statueVec)
{
	auto iter = m_GuildDungeonFigureStatue.find(guildId);
	if (iter == m_GuildDungeonFigureStatue.end())
		return;

	for (auto& it : iter->second)
	{
		FigureStatue* statue = it.second;
		if (statue == NULL)
			continue;

		if (statue->GetRoleID() == 0)
			continue;

		FigureStatueInfo info;
		info.accId = statue->GetAccID();
		info.roleId = statue->GetRoleID();
		info.name = statue->GetRoleName();
		info.occu = statue->GetOccu();
		info.avatar = statue->GetAvatar();
		info.statueType = statue->GetStatueType();
		info.guildName = statue->GetGuildName();

		statueVec.push_back(info);
	}
}

bool FigureStatueMgr::IsExistGuildDungeonStatue(UInt64 guildId)
{
	auto iter = m_GuildDungeonFigureStatue.find(guildId);
	return iter != m_GuildDungeonFigureStatue.end();
}
