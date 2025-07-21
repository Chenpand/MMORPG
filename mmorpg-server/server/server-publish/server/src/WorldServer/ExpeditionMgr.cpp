#include "ExpeditionMgr.h"

#include <CLRecordClient.h>
#include <CLObjectProperty.h>
#include <CLAdventureTeamProtocol.h>
#include <CLAdventureTeamDefine.h>
#include <CLExpeditionMapDataEntry.h>
#include <CLExpeditionMapRewardDataEntry.h>
#include "WSPlayerMgr.h"
#include "ExpeditionMap.h"
#include "ExpeditionMember.h"


class PlayerQueryExpeditionRolesCallback : public CLRecordCallback
{
public:
	PlayerQueryExpeditionRolesCallback(ObjID_t roleid):m_RoleId(roleid) {}

	virtual void OnFinished()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		auto account = player->GetAccountInfo();
		if (!account) return;

		CLProtocol::WorldQueryOptionalExpeditionRolesRes res;
		res.resCode = ErrorCode::SUCCESS;

		while (NextRow())
		{
			CLProtocol::ExpeditionMemberInfo roleInfo;

			roleInfo.roleid = GetKey();
			roleInfo.name = GetData("name").GetString();
			roleInfo.level = GetData("level");
			roleInfo.occu = GetData("occu");
			
			ObjPlayerAvatar	avatardb;
			avatardb.SetDBValue(GetData("avatar"));
			roleInfo.avatar = avatardb;

			auto expeditionMember = account->GetExpeditionMgr().FindExpeditionMember(roleInfo.roleid);
			if (expeditionMember)
			{
				roleInfo.expeditionMapId = expeditionMember->GetExpeditionMapId();
			}

			res.roles.push_back(roleInfo);
		}

		player->SendProtocol(res);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		CLProtocol::WorldQueryOptionalExpeditionRolesRes res;
		res.resCode = ErrorCode::RECORD_ERROR;
		player->SendProtocol(res);

		ErrorLogStream << PLAYERINFO(player) << " PlayerQueryExpeditionRolesCallback: select t_player_info failed!" << LogStream::eos;
	}

private:
	ObjID_t m_RoleId;
};

class PlayerCheckExpeditionRolesCallback : public CLRecordCallback
{
public:
	PlayerCheckExpeditionRolesCallback(ObjID_t roleid, UInt8 mapId, const std::vector<ObjID_t>& members, UInt32 duration) 
		:m_RoleId(roleid), m_MapId(mapId), m_Members(members), m_Duration(duration) 
	{}

	virtual void OnFinished()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		auto account = player->GetAccountInfo();
		if (!account) return;

		CLProtocol::WorldDispatchExpeditionTeamRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.mapId = m_MapId;

		auto expeditionMapData = ExpeditionMapDataEntryMgr::Instance()->FindEntry(m_MapId);
		if (!expeditionMapData)
		{
			ErrorLogStream << PLAYERINFO(player) << " can't find expedition map data(" << m_MapId << ")!" << LogStream::eos;
			res.resCode = ErrorCode::EXPEDITION_MAP_ERROR;
			player->SendProtocol(res);
			return;
		}

		if (expeditionMapData->CheckAdventureTeamLevelLimit(player->GetAdventureTeamLevel()))
		{
			ErrorLogStream << PLAYERINFO(player) << " adventure team level(" << player->GetAdventureTeamLevel() << ") reach map(" << m_MapId << ")'s limit!" << LogStream::eos;
			res.resCode = ErrorCode::ADVENTURE_TEAM_LEVEL_LIMIT;
			player->SendProtocol(res);
			return;
		}

		if (!expeditionMapData->CheckExpiditonMemberValid(m_Members.size()))
		{
			ErrorLogStream << PLAYERINFO(player) << " invalid expedition map(" << m_MapId << ") member(" << (UInt32)m_Members.size() << ")!" << LogStream::eos;
			res.resCode = ErrorCode::EXPEDITION_MEMBER_NUM_ERR;
			player->SendProtocol(res);
			return;
		}

		if (!expeditionMapData->CheckExpeditionDurationValid(m_Duration))
		{
			ErrorLogStream << PLAYERINFO(player) << " invalid expedition map(" << m_MapId << ") duration(" << m_Duration << ")!" << LogStream::eos;
			res.resCode = ErrorCode::EXPEDITION_DURATION_ERROR;
			player->SendProtocol(res);
			return;
		}

		for (auto memberId : m_Members)
		{
			ExpeditionMember* member = account->GetExpeditionMgr().FindExpeditionMember(memberId);
			if (member)
			{
				ErrorLogStream << PLAYERINFO(player) << " dispatch expedition to map(" << m_MapId << "),  member (" << memberId << ") is already in  other expedition" << LogStream::eos;
				res.resCode = ErrorCode::EXPEDITION_TEAM_DISPATCH_FAILED;
				player->SendProtocol(res);
				return;
			}
		}

		while (NextRow())
		{
			ObjID_t roleid = GetKey();
			UInt32 accid = GetData("accid");
			if (accid != player->GetAccId())
			{
				ErrorLogStream << PLAYERINFO(player) << " not had the role(" << accid << "," << roleid << ") while dispatch to map(" << m_MapId << ")!" << LogStream::eos;
				res.resCode = ErrorCode::EXPEDITION_TEAM_MEMBER_INEXISTENCE;
				player->SendProtocol(res);
				return;
			}

			UInt16 level = GetData("level");
			if (expeditionMapData->CheckPlayerLevelLimit(level))
			{
				ErrorLogStream << PLAYERINFO(player) << " role(" << roleid << ")'s level(" << level << ") reach map(" << m_MapId << ")'s limit!" << LogStream::eos;
				res.resCode = ErrorCode::EXPEDITION_ROLE_LEVEL_ERR;
				player->SendProtocol(res);
				return;
			}

			CLProtocol::ExpeditionMemberInfo memberInfo;
			memberInfo.roleid = roleid;
			memberInfo.name = GetData("name").GetString();
			memberInfo.level = level;
			memberInfo.occu = GetData("occu");

			ObjPlayerAvatar	avatardb;
			avatardb.SetDBValue(GetData("avatar"));
			memberInfo.avatar = avatardb;

			memberInfo.expeditionMapId = m_MapId;

			res.members.push_back(memberInfo);
		}

		auto expeditionMap = account->GetExpeditionMgr().FindExpeditionMap(m_MapId);
		if (!expeditionMap)
		{
			expeditionMap = new ExpeditionMap();
			account->GetExpeditionMgr().AddExpeditionMap(expeditionMap);
		}

		account->GetExpeditionMgr().ClearExpeditionMember(m_MapId);

		for (const auto& memberInfo : res.members)
		{
			auto expeditionMember = new ExpeditionMember();
			expeditionMember->SetAccGUID(account->GetID());
			expeditionMember->SetAccId(player->GetAccId());
			expeditionMember->SetRoleId(memberInfo.roleid);
			expeditionMember->SetExpeditionMapId(memberInfo.expeditionMapId);
			expeditionMember->SetRoleName(memberInfo.name);
			expeditionMember->SetRoleLevel(memberInfo.level);
			expeditionMember->SetOccu(memberInfo.occu);
			expeditionMember->SetAvatar(memberInfo.avatar);
			expeditionMember->SaveToDB();
			account->GetExpeditionMgr().AddExpeditionMember(expeditionMember);
		}

		expeditionMap->SetAccGUID(account->GetID());
		expeditionMap->SetAccId(player->GetAccId());
		expeditionMap->SetExpeditionMapId(m_MapId);
		expeditionMap->SetExpeditionStartTime((UInt32)CURRENT_TIME.Sec());
		expeditionMap->SetExpeditionDuration(m_Duration);
		expeditionMap->SetExpeditionStatus(EXPEDITION_STATUS_IN);
		expeditionMap->SaveToDB();

		res.expeditionStatus = expeditionMap->GetExpeditionStatus();
		res.durationOfExpedition = expeditionMap->GetExpeditionDuration();
		res.endTimeOfExpedition = expeditionMap->GetExpeditionEndTime();
		player->SendProtocol(res);

		std::ostringstream ossLog;
		for (auto elem : m_Members)
		{
			if (!ossLog.str().empty())
			{
				ossLog << ",";
			}
			ossLog << elem;
		}
		InfoLogStream << PLAYERINFO(player) << " success dispatch expedition(" << expeditionMap->LogStr() << "), members(" << ossLog.str() << ")." << LogStream::eos;

		player->SendDispatchExpeditionTeamUdpLog(res.mapId, res.durationOfExpedition, (UInt32)res.members.size());
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		CLProtocol::WorldDispatchExpeditionTeamRes res;
		res.resCode = ErrorCode::RECORD_ERROR;
		player->SendProtocol(res);

		ErrorLogStream << PLAYERINFO(player) << " PlayerCheckExpeditionRolesCallback: select t_player_info failed!" << LogStream::eos;
	}

private:
	ObjID_t m_RoleId;
	UInt8 m_MapId;
	std::vector<ObjID_t> m_Members;
	UInt32 m_Duration;
};


ExpeditionMgr::ExpeditionMgr()
{

}

ExpeditionMgr::~ExpeditionMgr()
{
	for (auto elem : m_ExpeditionMaps)
	{
		ExpeditionMap::DeleteExpeditionMap(elem);
	}
	m_ExpeditionMaps.clear();

	for (auto elem : m_ExpeditionMembers)
	{
		ExpeditionMember::DeleteExpeditionMember(elem);
	}
	m_ExpeditionMembers.clear();
}

void ExpeditionMgr::LoadExpeditionMapCallback(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		UInt8 mapid = callback->GetData("mapid");

		ExpeditionMap* expeditionMap = FindExpeditionMap(mapid);
		if (!expeditionMap)
		{
			expeditionMap = new ExpeditionMap();
			expeditionMap->SetID(callback->GetKey());
			expeditionMap->SetDBData(callback);
			AddExpeditionMap(expeditionMap);
		}
	}
}

ExpeditionMap* ExpeditionMgr::FindExpeditionMap(UInt8 mapId)
{
	for (auto elem : m_ExpeditionMaps)
	{
		if (!elem) continue;

		if (elem->GetExpeditionMapId() == mapId)
		{
			return elem;
		}
	}
	return NULL;
}

void ExpeditionMgr::AddExpeditionMap(ExpeditionMap* map)
{
	m_ExpeditionMaps.push_back(map);
}

bool ExpeditionMgr::RemoveExpeditionMap(UInt8 mapId)
{
	for (auto itr = m_ExpeditionMaps.begin(); itr != m_ExpeditionMaps.end(); ++itr)
	{
		auto elem = *itr;
		if (!elem) continue;

		if (elem->GetExpeditionMapId() != mapId)
		{
			continue;
		}

		elem->DeleteFromDB();
		ExpeditionMap::DeleteExpeditionMap(elem);
		m_ExpeditionMaps.erase(itr);

		ClearExpeditionMember(mapId);

		return true;
	}

	return false;
}

void ExpeditionMgr::VisitExpeditionMaps(const std::function<bool(ExpeditionMap*)>& F)
{
	for (auto elem : m_ExpeditionMaps)
	{
		if (!elem) continue;

		if (!F(elem))
		{
			break;
		}
	}
}

void ExpeditionMgr::LoadExpeditionMemberCallback(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		ObjID_t roleid = callback->GetData("roleid");

		ExpeditionMember* member = FindExpeditionMember(roleid);
		if (!member)
		{
			member = new ExpeditionMember();
			member->SetID(callback->GetKey());
			member->SetDBData(callback);
			AddExpeditionMember(member);
		}
	}
}

ExpeditionMember* ExpeditionMgr::FindExpeditionMember(ObjID_t roleid)
{
	for (auto elem : m_ExpeditionMembers)
	{
		if (!elem) continue;

		if (elem->GetRoleId() == roleid)
		{
			return elem;
		}
	}
	return NULL;
}

void ExpeditionMgr::AddExpeditionMember(ExpeditionMember* member)
{
	m_ExpeditionMembers.push_back(member);
}

void ExpeditionMgr::VisitExpeditionMember(UInt8 mapid, const std::function<bool(ExpeditionMember*)>& F)
{
	for (auto elem : m_ExpeditionMembers)
	{
		if (!elem) continue;
		
		if (elem->GetExpeditionMapId() != mapid)
		{
			continue;
		}

		if (!F(elem))
		{
			break;
		}
	}
}

void ExpeditionMgr::ClearExpeditionMember(UInt8 mapid)
{
	for (auto itr = m_ExpeditionMembers.begin(); itr != m_ExpeditionMembers.end();)
	{
		auto elem = *itr;
		if (!elem)
		{
			++itr;
			continue;
		}

		if (elem->GetExpeditionMapId() != mapid)
		{
			++itr;
			continue;
		}

		elem->DeleteFromDB();
		ExpeditionMember::DeleteExpeditionMember(elem);

		itr = m_ExpeditionMembers.erase(itr);
	}
}

UInt16 ExpeditionMgr::GetExpeditionMemberNum(UInt8 mapid)
{
	UInt16 sum = 0;
	VisitExpeditionMember(mapid, [&sum](ExpeditionMember* member){
		if (member)
		{
			sum += 1;
		}
		return true;
	});
	return sum;
}

void ExpeditionMgr::OnTick(const Avalon::Time& now)
{
	for (auto expeditionMap : m_ExpeditionMaps)
	{
		if (!expeditionMap) continue;

		if (expeditionMap->GetExpeditionStatus() != EXPEDITION_STATUS_IN)
		{
			continue;
		}

		if ((UInt32)now.Sec() > expeditionMap->GetExpeditionEndTime())
		{
			expeditionMap->FinishExpedition();
			_SyncAllExpeditionMaps();
		}
	}
}

void ExpeditionMgr::OnPlayerOnline(WSPlayer* player)
{
}

void ExpeditionMgr::OnPlayerQueryExpeditionMap(WSPlayer* player, UInt8 mapId)
{
	if (!player) return;

	CLProtocol::WorldQueryExpeditionMapRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.mapId = mapId;

	auto expeditionMapData = ExpeditionMapDataEntryMgr::Instance()->FindEntry(mapId);
	if (!expeditionMapData)
	{
		res.resCode = ErrorCode::EXPEDITION_MAP_ERROR;
		player->SendProtocol(res);
		return;
	}

	auto expeditionMap = FindExpeditionMap(mapId);
	if (!expeditionMap)
	{
		res.expeditionStatus = EXPEDITION_STATUS_PREPARE;
		res.durationOfExpedition = expeditionMapData->expeditionDurationList.empty() ? 0 : expeditionMapData->expeditionDurationList.front();
		res.endTimeOfExpedition = 0;
	}
	else
	{
		res.expeditionStatus = expeditionMap->GetExpeditionStatus();
		res.durationOfExpedition = expeditionMap->GetExpeditionDuration();
		res.endTimeOfExpedition = expeditionMap->GetExpeditionEndTime();

		VisitExpeditionMember(mapId, [&](ExpeditionMember* member){
			if (!member) return true;

			CLProtocol::ExpeditionMemberInfo memberInfo;
			memberInfo.roleid = member->GetRoleId();
			memberInfo.occu = member->GetOccu();
			memberInfo.avatar = member->GetAvatar();
			res.members.push_back(memberInfo);

			return true;
		});
	}

	player->SendProtocol(res);
}

void ExpeditionMgr::OnPlayerQueryOptionalExpeditionRoles(WSPlayer* player)
{
	if (!player) return;

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutCond("accid", player->GetAccId());
	cmd->PutColumn("name");
	cmd->PutColumn("level");
	cmd->PutColumn("occu");
	cmd->PutColumn("avatar");
	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0)";
	cmd->SetLimit(limitoss.str());
	CLRecordClient::Instance()->SendCommand(cmd, new PlayerQueryExpeditionRolesCallback(player->GetID()));
}

void ExpeditionMgr::OnPlayerDispatchExpeditionTeam(WSPlayer* player, UInt8 mapId, const std::vector<ObjID_t>& members, UInt32 duration)
{
	if (!player) return;

	CLProtocol::WorldDispatchExpeditionTeamRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.mapId = mapId;

	auto expeditionMapData = ExpeditionMapDataEntryMgr::Instance()->FindEntry(mapId);
	if (!expeditionMapData)
	{
		ErrorLogStream << PLAYERINFO(player) << " can't find expedition map data(" << mapId << ")!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_MAP_ERROR;
		player->SendProtocol(res);
		return;
	}

	if (expeditionMapData->CheckAdventureTeamLevelLimit(player->GetAdventureTeamLevel()))
	{
		ErrorLogStream << PLAYERINFO(player) << " adventure team level(" << player->GetAdventureTeamLevel() << ") reach map(" << mapId << ")'s limit!" << LogStream::eos;
		res.resCode = ErrorCode::ADVENTURE_TEAM_LEVEL_LIMIT;
		player->SendProtocol(res);
		return;
	}
	
	if (!expeditionMapData->CheckExpiditonMemberValid(members.size()))
	{
		ErrorLogStream << PLAYERINFO(player) << " invalid expedition map(" << mapId << ") member(" << (UInt32)members.size() << ")!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_MEMBER_NUM_ERR;
		player->SendProtocol(res);
		return;
	}

	if (!expeditionMapData->CheckExpeditionDurationValid(duration))
	{
		ErrorLogStream << PLAYERINFO(player) << " invalid expedition map(" << mapId << ") duration(" << duration << ")!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_DURATION_ERROR;
		player->SendProtocol(res);
		return;
	}

	auto expeditionMap = FindExpeditionMap(mapId);
	if (expeditionMap && expeditionMap->GetExpeditionStatus() != EXPEDITION_STATUS_PREPARE)
	{
		res.resCode = ErrorCode::EXPEDITION_TEAM_DISPATCH_FAILED;
		res.expeditionStatus = expeditionMap->GetExpeditionStatus();
		res.durationOfExpedition = expeditionMap->GetExpeditionDuration();
		res.endTimeOfExpedition = expeditionMap->GetExpeditionEndTime();

		VisitExpeditionMember(mapId, [&](ExpeditionMember* member){
			if (!member) return true;

			CLProtocol::ExpeditionMemberInfo memberInfo;
			memberInfo.roleid = member->GetRoleId();
			memberInfo.name = member->GetRoleName();
			memberInfo.level = member->GetRoleLevel();
			memberInfo.occu = member->GetOccu();
			memberInfo.avatar = member->GetAvatar();
			memberInfo.expeditionMapId = member->GetExpeditionMapId();
			res.members.push_back(memberInfo);

			return true;
		});

		ErrorLogStream << PLAYERINFO(player) << " dispatch expedition to map(" << mapId << ") again, map status(" << (UInt32)expeditionMap->GetExpeditionStatus() << ")." << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	for (auto memberId : members)
	{
		ExpeditionMember* member = FindExpeditionMember(memberId);
		if (member)
		{
			ErrorLogStream << PLAYERINFO(player) << " dispatch expedition to map(" << mapId << "),  member (" << memberId << ") is already in  other expedition" << LogStream::eos;
			res.resCode = ErrorCode::EXPEDITION_TEAM_DISPATCH_FAILED;
			player->SendProtocol(res);
			return;
		}
	}

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutColumn("accid");
	cmd->PutColumn("name");
	cmd->PutColumn("level");
	cmd->PutColumn("occu");
	cmd->PutColumn("avatar");
	std::string limitStr = "WHERE `guid` IN(";
	std::string roleidStr;
	for (auto roleid : members)
	{
		if (!roleidStr.empty())
		{
			roleidStr += ",";
		}
		roleidStr += Avalon::StringUtil::ConvertToString<ObjID_t>(roleid);
	}
	limitStr.append(roleidStr).append(")");
	cmd->SetLimit(limitStr);
	CLRecordClient::Instance()->SendCommand(cmd, new PlayerCheckExpeditionRolesCallback(player->GetID(), mapId, members, duration));

	InfoLogStream << PLAYERINFO(player) << " start check expedition(" << mapId << ") roles..." << LogStream::eos;
}

void ExpeditionMgr::OnPlayerCancelExpedition(WSPlayer* player, UInt8 mapId)
{
	if (!player) return;

	CLProtocol::WorldCancelExpeditionRes res;
	res.mapId = mapId;
	res.resCode = ErrorCode::SUCCESS;

	if (!RemoveExpeditionMap(mapId))
	{
		ErrorLogStream << PLAYERINFO(player) << " remove expedition map(" << mapId << ") failed!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_CANCEL_FAILED;
		player->SendProtocol(res);
		return;
	}
	
	InfoLogStream << PLAYERINFO(player) << " success cancel expedition from map(" << mapId << ")!" << LogStream::eos;
	player->SendProtocol(res);

	player->SendCancelExpeditionTeamUdpLog(mapId);
}

void ExpeditionMgr::OnPlayerGetExpeditionRewards(WSPlayer* player, UInt8 mapId)
{
	if (!player) return;

	CLProtocol::WorldGetExpeditionRewardsRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.mapId = mapId;

	auto expeditionMapData = ExpeditionMapDataEntryMgr::Instance()->FindEntry(mapId);
	if (!expeditionMapData)
	{
		ErrorLogStream << PLAYERINFO(player) << " can't find expedition map data(" << mapId << ")!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_MAP_ERROR;
		player->SendProtocol(res);
		return;
	}
	
	auto expeditionMap = FindExpeditionMap(mapId);
	if (!expeditionMap)
	{
		ErrorLogStream << PLAYERINFO(player) << " can't find expedition map(" << mapId << ")!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_INEXISTENCE;
		player->SendProtocol(res);
		return;
	}

	if (expeditionMap->GetExpeditionStatus() != EXPEDITION_STATUS_OVER)
	{
		ErrorLogStream << PLAYERINFO(player) << expeditionMap->LogStr() << " status error!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_TAKE_REWARD_FAILED;
		res.expeditionStatus = expeditionMap->GetExpeditionStatus();
		player->SendProtocol(res);
		return;
	}

	std::vector<UInt8> occus;
	VisitExpeditionMember(mapId, [&](ExpeditionMember* member){
		if (!member) return true;
		occus.push_back(member->GetOccu());
		return true;
	});

	UInt32 hoursOfExpedition = expeditionMap->GetExpeditionDuration();
	if (hoursOfExpedition == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << expeditionMap->LogStr() << " duration is zero!" << LogStream::eos;
		RemoveExpeditionMap(mapId);
		res.resCode = ErrorCode::EXPEDITION_DURATION_ERROR;
		res.expeditionStatus = EXPEDITION_STATUS_PREPARE;
		player->SendProtocol(res);
		return;
	}

	if (!RemoveExpeditionMap(mapId))
	{
		ErrorLogStream << PLAYERINFO(player) << " remove expedition map(" << mapId << ") failed!" << LogStream::eos;
		res.resCode = ErrorCode::EXPEDITION_TAKE_REWARD_FAILED;
		res.expeditionStatus = EXPEDITION_STATUS_PREPARE;
		player->SendProtocol(res);
		return;
	}

	ItemRewardVec rewards;
	std::vector<UInt32> rewardsDataIds;
	ExpeditionMapRewardDataEntryMgr::Instance()->GetExpeditionRewards(mapId, occus, rewards, rewardsDataIds);
	std::string reason = GetReason(SOURCE_TYPE_ADVENTURE_TEAM_EXPEDITION, mapId);
	std::ostringstream rewardsOss;
	std::string rewardsStr;
	for (auto itr = rewards.begin(); itr != rewards.end();)
	{
		UInt32 itemId = itr->id;
		UInt32 itemNum = itr->num;

		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
		if (!itemData)
		{
			ErrorLogStream << PLAYERINFO(player) << " can't find item data(" << itemId << ")!" << LogStream::eos;
			++itr;
			continue;
		}

		itemNum *= hoursOfExpedition;
		itr->num = itemNum;
		itr->EncodeString(rewardsOss);

		if (itemData->subType == ST_BOUNTY)
		{
			{
				// 这里只是用来通知
				ItemRewardVec notifyRewards;
				notifyRewards.push_back(*itr);
				player->NotifyClientAddItem(reason, notifyRewards, 0);
			}

			player->IncAccountMoney(reason, (ItemSubType)itemData->subType, itr->num);
			itr = rewards.erase(itr);
			continue;
		}

		++itr;
	}
	if (!rewards.empty())
	{
		player->AddItem(reason, rewards);
	}
	rewardsStr = rewardsOss.str();

	res.expeditionStatus = EXPEDITION_STATUS_PREPARE;
	player->SendProtocol(res);

	InfoLogStream << PLAYERINFO(player) << " get rewards(" << rewardsStr << ") from expedition map(" << mapId << ")." << LogStream::eos;
	player->SendGetExpeditionTeamRewardsUdpLog(mapId, hoursOfExpedition, rewardsDataIds);
}

void ExpeditionMgr::OnPlayerQueryAllExpeditionMaps(WSPlayer* player, const std::vector<UInt8>& mapIds)
{
	if (!player) return;
	
	CLProtocol::WorldQueryAllExpeditionMapsRes res;
	res.resCode = ErrorCode::SUCCESS;

	for (auto mapId : mapIds)
	{
		auto expeditionMap = FindExpeditionMap(mapId);

		CLProtocol::ExpeditionMapBaseInfo mapBaseInfo;
		mapBaseInfo.mapId = mapId;
		mapBaseInfo.expeditionStatus = expeditionMap ? expeditionMap->GetExpeditionStatus() : EXPEDITION_STATUS_PREPARE;
		mapBaseInfo.memberNum = GetExpeditionMemberNum(mapId);
		res.mapBaseInfos.push_back(mapBaseInfo);
	}

	player->SendProtocol(res);
}

void ExpeditionMgr::_SyncAllExpeditionMaps()
{
	UInt32 accid = 0;
	CLProtocol::WorldAllExpeditionMapsSync protocol;
	VisitExpeditionMaps([&](ExpeditionMap* expeditionMap){
		if (expeditionMap)
		{
			accid = expeditionMap->GetAccId();
			CLProtocol::ExpeditionMapBaseInfo mapBaseInfo;
			mapBaseInfo.mapId = expeditionMap->GetExpeditionMapId();
			mapBaseInfo.expeditionStatus = expeditionMap->GetExpeditionStatus();
			mapBaseInfo.memberNum = GetExpeditionMemberNum(mapBaseInfo.mapId);
			protocol.mapBaseInfos.push_back(mapBaseInfo);
		}
		return true;
	});

	if (protocol.mapBaseInfos.empty())
	{
		return;
	}

	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
	if (player)
	{
		player->SendProtocol(protocol);
	}
}