#include "TCSPlayer.h"

#include <CLItemProtocol.h>
#include <CLTeamCopyProtocol.h>
#include <CLNetworkPerformance.h>
#include <CLTeamCopyRollDataEntry.h>
#include "GameZoneMgr.h"
#include "TCSApplication.h"
#include "TCSTeamMgr.h"

TCSPlayer::TCSPlayer()
{
	m_TeamType = 0;
	m_ZoneId = 0;
	m_AccId = 0;
	m_Occu = 0;
	m_Level = 0;
	m_VipLvl = 0;
	m_EquipScore = 0;
	m_ConnId = 0;
	m_CopyId = 0;
	m_SceneType = 0;
	m_SceneSubType = 0;
	m_SceneNodeId = 0;
	m_WorldNodeId = 0;
	m_GuildId = 0;
	m_Awaken = 0;
	m_IsLoading = false;
}

TCSPlayer::~TCSPlayer()
{

}

UInt32 TCSPlayer::GetDayQuitTeamNum(UInt32 teamType)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	return playerData.dayQuitTeamNum;
}

UInt32 TCSPlayer::GetWeekQuitTeamNum(UInt32 teamType)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	return playerData.weekQuitTeamNum;
}

void TCSPlayer::AddQuitTeamNum(UInt32 teamType)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	++playerData.dayQuitTeamNum;
	++playerData.weekQuitTeamNum;

	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

bool TCSPlayer::IsOddFreeQuitTeamNum(UInt32 teamType)
{
	UInt32 dayCfgNum = 0;
	UInt32 weekCfgNum = 0;
	if (teamType == TC_TYPE_ONE)
	{
		dayCfgNum = GetTeamCopyValue(TC_VALUE_DAY_FREE_QUIT_NUM);
		weekCfgNum = GetTeamCopyValue(TC_VALUE_WEEK_FREE_QUIT_NUM);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		dayCfgNum = GetTeamCopyValue(TC_VALUE_2_DAY_FREE_QUIT_NUM);
		weekCfgNum = GetTeamCopyValue(TC_VALUE_2_WEEK_FREE_QUIT_NUM);
	}

	if (GetWeekQuitTeamNum(teamType) >= weekCfgNum)
	{
		return false;
	}

	if (GetDayQuitTeamNum(teamType) >= dayCfgNum)
	{
		return false;
	}

	return true;
}

void TCSPlayer::GMClearChallengeNum()
{
	UInt32 teamType = GetTeamType();
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.dayNumMap.clear();
	playerData.weekNumMap.clear();
	playerData.rollNumMap.clear();
	playerData.dayFlopMap.clear();
	playerData.weekFlopMap.clear();

	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

void TCSPlayer::GMOpenGoldTeam()
{
	UInt32 teamType = GetTeamType();
	UInt32 cond = 0;
	if (teamType == TC_TYPE_ONE)
	{
		cond = GetTeamCopyValue(TC_VALUE_GOLD_MODEL_COND_1);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		cond = GetTeamCopyValue(TC_VALUE_2_GOLD_MODEL_COND_1);
	}

	SetModelNum(teamType, TEAM_COPY_TEAM_MODEL_CHALLENGE, cond);
	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

void TCSPlayer::GMOpenDiffGrade()
{
	UInt32 teamType = GetTeamType();
	UInt32 cond = 0;
	if (teamType == TC_TYPE_ONE)
	{
		cond = GetTeamCopyValue(TC_VALUE_DIFF_UNLOCK_COND);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		cond = GetTeamCopyValue(TC_VALUE_2_DIFF_UNLOCK_COND);
	}

	SetModelNum(teamType, TEAM_COPY_TEAM_MODEL_CHALLENGE, cond);
	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

void TCSPlayer::SetTicketIsEnough(UInt32 teamType, UInt32 flag)
{
	m_TicketIsEnoughMap[teamType] = flag;
}

UInt32 TCSPlayer::GetTicketIsEnough(UInt32 teamType)
{
	return m_TicketIsEnoughMap[teamType];
}

void TCSPlayer::AddModelNum(UInt32 teamType, UInt32 model, UInt32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.modelNumMap[model] += num;

	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

void TCSPlayer::SetModelNum(UInt32 teamType, UInt32 model, UInt32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.modelNumMap[model] = num;
}

UInt32 TCSPlayer::GetModelNum(UInt32 teamType, UInt32 model)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	return playerData.modelNumMap[model];
}

UInt32 TCSPlayer::GetAllModelNum(UInt32 teamType)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	UInt32 num = 0;
	for (auto& iter : playerData.modelNumMap)
	{
		num += iter.second;
	}

	return num;
}

void TCSPlayer::AddDayNum(UInt32 teamType, UInt32 grade, Int32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	if (num > 0)
	{
		playerData.dayNumMap[grade] += num;
	}
	else
	{
		auto iter = playerData.dayNumMap.find(grade);
		if (iter == playerData.dayNumMap.end())
			return;

		if (iter->second <= UInt32(abs(num)))
			iter->second = 0;
		else
			iter->second -= UInt32(abs(num));
	}
}

void TCSPlayer::SetDayNum(UInt32 teamType, UInt32 grade, UInt32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.dayNumMap[grade] = num;
}

UInt32 TCSPlayer::GetDayNum(UInt32 teamType, UInt32 grade)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	auto iter = playerData.dayNumMap.find(grade);
	return iter != playerData.dayNumMap.end() ? iter->second : 0;
}

void TCSPlayer::AddWeekNum(UInt32 teamType, UInt32 grade, Int32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	if (num > 0)
	{
		playerData.weekNumMap[grade] += num;
	}
	else
	{
		auto iter = playerData.weekNumMap.find(grade);
		if (iter == playerData.weekNumMap.end())
			return;

		if (iter->second <= UInt32(abs(num)))
			iter->second = 0;
		else
			iter->second -= UInt32(abs(num));
	}
}

void TCSPlayer::SetWeekNum(UInt32 teamType, UInt32 grade, UInt32 num)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.weekNumMap[grade] = num;
}

UInt32 TCSPlayer::GetWeekNum(UInt32 teamType, UInt32 grade)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	auto iter = playerData.weekNumMap.find(grade);
	return iter != playerData.weekNumMap.end() ? iter->second : 0;
}

UInt32 TCSPlayer::GetCfgTotalNum(UInt32 teamType, UInt32 teamGrade, bool isDay)
{
	UInt32 num = 0;
	switch (teamGrade)
	{
	case TEAM_COPY_TEAM_GRADE_COMMON:
	{
		UInt32 day = 0;
		UInt32 week = 0;
		if (teamType == TC_TYPE_ONE)
		{
			day = GetTeamCopyValue(TC_VALUE_DAY_NUM);
			week = GetTeamCopyValue(TC_VALUE_WEEK_NUM);
		}
		else if (teamType == TC_TYPE_TWO)
		{
			day = GetTeamCopyValue(TC_VALUE_2_DAY_NUM);
			week = GetTeamCopyValue(TC_VALUE_2_WEEK_NUM);
		}

		num = isDay ? day : week;
		num += GetExtraNum(teamType, teamGrade);
		break;
	}
	case TEAM_COPY_TEAM_GRADE_DIFF:
	{
		UInt32 day = 0;
		UInt32 week = 0;
		if (teamType == TC_TYPE_ONE)
		{
			day = GetTeamCopyValue(TC_VALUE_DIFF_DAY_NUM);
			week = GetTeamCopyValue(TC_VALUE_DIFF_WEEK_NUM);
		}
		else if(teamType == TC_TYPE_TWO)
		{
			day = GetTeamCopyValue(TC_VALUE_2_DIFF_DAY_NUM);
			week = GetTeamCopyValue(TC_VALUE_2_DIFF_WEEK_NUM);
		}

		num = isDay ? day : week;
		num += GetExtraNum(teamType, teamGrade);
		break;
	}
	default: break;
	}

	return num;
}

bool TCSPlayer::IsHasChallengeNum(UInt32 teamType, UInt32 teamGrade)
{
	if (GetWeekNum(teamType, teamGrade) >= GetCfgTotalNum(teamType, teamGrade, false))
	{
		return false;
	}

	if (GetDayNum(teamType, teamGrade) >= GetCfgTotalNum(teamType, teamGrade, true))
	{
		return false;
	}

	return true;
}

void TCSPlayer::AddChallengeNum(UInt32 teamType, UInt32 teamGrade)
{
	AddDayNum(teamType, teamGrade, 1);
	AddWeekNum(teamType, teamGrade, 1);

	InfoLogStream << PLAYERINFO(this) << "teamType:" << teamType << " grade:" << teamGrade <<
		" dayNum:" << GetDayNum(teamType, teamGrade) <<
		" weekNum:" << GetWeekNum(teamType, teamGrade) << LogStream::eos;

	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

void TCSPlayer::ReturnChallengeNum(UInt32 teamType, UInt32 teamGrade)
{
	if (GetDayNum(teamType, teamGrade) > 0)
	{
		AddDayNum(teamType, teamGrade, -1);
	}

	if (GetWeekNum(teamType, teamGrade) > 0)
	{
		AddWeekNum(teamType, teamGrade, -1);
	}

	InfoLogStream << PLAYERINFO(this) << "teamType:" << teamType << " grade:" << teamGrade << 
		" dayNum:" << GetDayNum(teamType, teamGrade) <<
		" weekNum:" << GetWeekNum(teamType, teamGrade) << LogStream::eos;

	PlayerInfoToScene(teamType);
	PlayerInfoToClient(teamType);
}

UInt32 TCSPlayer::GetExtraNum(UInt32 teamType, UInt32 teamGrade)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	auto iter = playerData.extraNumMap.find(teamGrade);
	return iter != playerData.extraNumMap.end() ? iter->second : 0;
}

void TCSPlayer::AddRollNum(UInt32 teamType, UInt32 teamGrade)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	playerData.rollNumMap[teamGrade] += 1;
}

UInt32 TCSPlayer::GetRollNum(UInt32 teamType, UInt32 teamGrade)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	auto iter = playerData.rollNumMap.find(teamGrade);
	return iter != playerData.rollNumMap.end() ? iter->second : 0;
}

bool TCSPlayer::IsCreateGoldTeam(UInt32 teamType)
{
	if (TCSApplication::Instance()->GetIsOpenGoldTeam())
	{
		return false;
	}

	UInt32 cond1 = 0;
	UInt32 cond2 = 0;
	if (teamType == TC_TYPE_ONE)
	{
		cond1 = GetTeamCopyValue(TC_VALUE_GOLD_MODEL_COND_1);
		cond2 = GetTeamCopyValue(TC_VALUE_GOLD_MODEL_COND_2);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		cond1 = GetTeamCopyValue(TC_VALUE_2_GOLD_MODEL_COND_1);
		cond2 = GetTeamCopyValue(TC_VALUE_2_GOLD_MODEL_COND_2);
	}

	// 两个条件控制是否可以创建金团
	if (GetModelNum(teamType, TEAM_COPY_TEAM_MODEL_CHALLENGE) < cond1
		|| GetEquipScore() < cond2)
	{
		return false;
	}

	return true;
}

void TCSPlayer::AddFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage, UInt32 crossDateType)
{
	FlopKey flopKey(teamGrade, stage);
	TCSPlayerData& playerData = m_TCDataMap[teamType];

	// 跨周局, 当前翻牌算上周的，不记录
	if (crossDateType == TEAM_COPY_CROSS_WEEK)
	{
		return;
	}
	else if (crossDateType == TEAM_COPY_CROSS_DAY)
	{
		// 跨天局，当局翻牌算昨天的，只记录周翻牌
		playerData.weekFlopMap[flopKey] += 1;
	}
	else
	{
		playerData.dayFlopMap[flopKey] += 1;
		playerData.weekFlopMap[flopKey] += 1;
	}
	
	PlayerInfoToScene(teamType);
}

UInt32 TCSPlayer::GetDayFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage)
{
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	FlopKey flopKey(teamGrade, stage);
	auto iter = playerData.dayFlopMap.find(flopKey);
	return iter != playerData.dayFlopMap.end() ? iter->second : 0;
}

UInt32 TCSPlayer::GetWeekFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage)
{ 
	TCSPlayerData& playerData = m_TCDataMap[teamType];
	FlopKey flopKey(teamGrade, stage);
	auto iter = playerData.weekFlopMap.find(flopKey);
	return iter != playerData.weekFlopMap.end() ? iter->second : 0;
}

void TCSPlayer::CalNotFlopStage(UInt32 teamType, UInt32 teamGrade)
{
	m_NotFlopMap.clear();

	for (UInt32 k = TEAM_COPY_STAGE_ONE; k <= TEAM_COPY_STAGE_FINAL; ++k)
	{
		if (GetWeekFlopNum(teamType, teamGrade, k) >= GetCfgTotalNum(teamType, teamGrade, false))
		{
			m_NotFlopMap[k] = TEAM_COPY_FLOP_LIMIT_WEEK;
			continue;
		}

		if (GetDayFlopNum(teamType, teamGrade, k) >= GetCfgTotalNum(teamType, teamGrade, true))
		{
			m_NotFlopMap[k] = TEAM_COPY_FLOP_LIMIT_DAY;
			continue;
		}
	}
}

UInt32 TCSPlayer::IsCanFlop(UInt32 stage)
{
	auto iter = m_NotFlopMap.find(stage);
	return iter != m_NotFlopMap.end() ? iter->second : TEAM_COPY_FLOP_LIMIT_NULL;
}

bool TCSPlayer::IsUnlockGrade(UInt32 teamType, UInt32 teamGrade)
{
	UInt32 cond = 0;
	if (teamType == TC_TYPE_ONE)
	{
		cond = GetTeamCopyValue(TC_VALUE_DIFF_UNLOCK_COND);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		cond = GetTeamCopyValue(TC_VALUE_2_DIFF_UNLOCK_COND);
	}

	switch (teamGrade)
	{
	case TEAM_COPY_TEAM_GRADE_COMMON: 
		return true;
	case TEAM_COPY_TEAM_GRADE_DIFF: 
		return GetAllModelNum(teamType) >= cond;
	default: break;
	}

	return false;
}

void TCSPlayer::PlayerInfoToScene(UInt32 teamType)
{
	CLProtocol::TeamCopyPlayerInfoSync sync;
	sync.roleId = GetID();

	TCSPlayerData& playerData = m_TCDataMap[teamType];

	CLProtocol::TCTypeData typeData;
	typeData.dayNumStr = _DecodeString(playerData.dayNumMap);
	typeData.dayQuitTeamNum = playerData.dayQuitTeamNum;
	typeData.weekNumStr = _DecodeString(playerData.weekNumMap);
	typeData.weekQuitTeamNum = playerData.weekQuitTeamNum;
	typeData.modelPassStr = _DecodeString(playerData.modelNumMap);
	typeData.rollNumStr = _DecodeString(playerData.rollNumMap);
	typeData.dayFlopStr = _DecodeFlopString(playerData.dayFlopMap);
	typeData.weekFlopStr = _DecodeFlopString(playerData.weekFlopMap);
	sync.tcTypeDataMap[teamType] = typeData;
	SendToScene(sync);
}

void TCSPlayer::PlayerInfoToClient(UInt32 teamType)
{
	CLProtocol::TeamCopyPlayerInfoNotify notify;
	if (teamType == TC_TYPE_ONE)
	{
		GetTeamCopyOneInfo(teamType, notify);
	}
	else if (teamType == TC_TYPE_TWO)
	{
		GetTeamCopyTwoInfo(teamType, notify);
	}

	notify.dayNum = GetDayNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.weekNum = GetWeekNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.diffWeekNum = GetWeekNum(teamType, TEAM_COPY_TEAM_GRADE_DIFF);
	notify.isCreateGold = IsCreateGoldTeam(teamType) ? 1 : 0;
	notify.ticketIsEnough = GetTicketIsEnough(teamType);
	notify.commonGradePassNum = GetAllModelNum(teamType);
	notify.rollRewardNum = GetRollNum(teamType, TEAM_COPY_TEAM_GRADE_DIFF);
	notify.rollRewardTotalNum = TeamCopyRollDataEntryMgr::Instance()->GetRollRewardNum(teamType, TEAM_COPY_TEAM_GRADE_DIFF);

	for (UInt32 grade = TEAM_COPY_TEAM_GRADE_COMMON; grade < TEAM_COPY_TEAM_GRADE_NUM; ++grade)
	{
		if (IsUnlockGrade(teamType, grade))
		{
			notify.yetOpenGradeVec.push_back(grade);
		}
	}

	SendProtocol(notify);
}

void TCSPlayer::GetTeamCopyOneInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify)
{
	UInt32 dayFreeQuit = GetTeamCopyValue(TC_VALUE_DAY_FREE_QUIT_NUM);
	UInt32 dayQuitNum = GetDayQuitTeamNum(teamType);
	notify.dayFreeQuitNum = 0;
	if (dayFreeQuit > dayQuitNum)
		notify.dayFreeQuitNum = dayFreeQuit - dayQuitNum;

	UInt32 weekFreeQuit = GetTeamCopyValue(TC_VALUE_WEEK_FREE_QUIT_NUM);
	UInt32 weekQuitNum = GetWeekQuitTeamNum(teamType);
	notify.weekFreeQuitNum = 0;
	if (weekFreeQuit > weekQuitNum)
		notify.weekFreeQuitNum = weekFreeQuit - weekQuitNum;

	notify.dayTotalNum = GetTeamCopyValue(TC_VALUE_DAY_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.weekTotalNum = GetTeamCopyValue(TC_VALUE_WEEK_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.diffWeekTotalNum = GetTeamCopyValue(TC_VALUE_DIFF_WEEK_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_DIFF);
	notify.unlockDiffGradeCommonNum = GetTeamCopyValue(TC_VALUE_DIFF_UNLOCK_COND);
}

void TCSPlayer::GetTeamCopyTwoInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify)
{
	UInt32 dayFreeQuit = GetTeamCopyValue(TC_VALUE_2_DAY_FREE_QUIT_NUM);
	UInt32 dayQuitNum = GetDayQuitTeamNum(teamType);
	notify.dayFreeQuitNum = 0;
	if (dayFreeQuit > dayQuitNum)
		notify.dayFreeQuitNum = dayFreeQuit - dayQuitNum;

	UInt32 weekFreeQuit = GetTeamCopyValue(TC_VALUE_2_WEEK_FREE_QUIT_NUM);
	UInt32 weekQuitNum = GetWeekQuitTeamNum(teamType);
	notify.weekFreeQuitNum = 0;
	if (weekFreeQuit > weekQuitNum)
		notify.weekFreeQuitNum = weekFreeQuit - weekQuitNum;

	notify.dayTotalNum = GetTeamCopyValue(TC_VALUE_2_DAY_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.weekTotalNum = GetTeamCopyValue(TC_VALUE_2_WEEK_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_COMMON);
	notify.diffWeekTotalNum = GetTeamCopyValue(TC_VALUE_2_DIFF_WEEK_NUM) + GetExtraNum(teamType, TEAM_COPY_TEAM_GRADE_DIFF);
	notify.unlockDiffGradeCommonNum = GetTeamCopyValue(TC_VALUE_2_DIFF_UNLOCK_COND);
}

void TCSPlayer::AddRewards(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify)
{
	CLProtocol::TeamCopyGetItem reward;
	reward.roleId = GetID();
	reward.reason = reason;
	reward.rewards = itemVec;
	reward.notice = isNotify ? 1 : 0;
	SendToScene(reward);
}

void TCSPlayer::AddInvite(const TeamCopyInviteInfo& inviteInfo)
{
	auto iter = m_InviteMap.find(inviteInfo.teamId);
	if (iter != m_InviteMap.end())
	{
		return;
	}

	m_InviteMap[inviteInfo.teamId] = inviteInfo;
}

bool TCSPlayer::DelInviteInfo(UInt32 teamId)
{
	auto iter = m_InviteMap.find(teamId);
	if (iter == m_InviteMap.end())
		return false;

	m_InviteMap.erase(teamId);
	return true;
}

void TCSPlayer::SendInviteList()
{
	CLProtocol::TeamCopyInviteListRes res;

	std::map<UInt32, TeamCopyInviteInfo>::iterator iter = m_InviteMap.begin();
	for (;iter != m_InviteMap.end();)
	{
		if ( !TCSTeamMgr::Instance()->IsExistTeam(iter->first))
		{
			iter = m_InviteMap.erase(iter);
			continue;
		}

		if (iter->second.teamType != GetTeamType())
		{
			++iter;
			continue;
		}

		res.inviteList.push_back(iter->second);
		++iter;
	}

	SendProtocol(res);
}

void TCSPlayer::SendPacket(Avalon::Packet* packet)
{
	auto zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->SendToPlayer(GetID(), packet);
		CLNetworkPerfMgr::Instance()->OnSendPacket(GetAccID(), packet->GetID(), packet->GetSize());
	}
	else
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find zone(" << m_ConnId << ") when send protocol(" << packet->GetID() << ")." << LogStream::eos;
	}
}

void TCSPlayer::SendProtocol(Avalon::Protocol& protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->SendToPlayer(GetID(), protocol);
	}
	else
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find zone(" << m_ConnId << ") when send protocol(" << protocol.GetProtocolID() << ")." << LogStream::eos;
	}
}

void TCSPlayer::SendToScene(Avalon::Packet* packet)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << " send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToScene(packet);
}

void TCSPlayer::SendToScene(Avalon::Protocol& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << "player send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToScene(protocol);
}

void TCSPlayer::SetPlayerData(CLProtocol::TeamCopyPlayerInfoSync& protocol)
{
	SetEquipScore(protocol.equipScore);

	for (auto& iter : protocol.tcTypeDataMap)
	{
		TCSPlayerData& playerData = m_TCDataMap[iter.first];
		playerData.dayQuitTeamNum = iter.second.dayQuitTeamNum;
		playerData.weekQuitTeamNum = iter.second.weekQuitTeamNum;

		_EncodeString(playerData.dayNumMap, iter.second.dayNumStr);
		_EncodeString(playerData.weekNumMap, iter.second.weekNumStr);
		_EncodeString(playerData.modelNumMap, iter.second.modelPassStr);
		_EncodeString(playerData.extraNumMap, iter.second.extraNumStr);
		_EncodeString(playerData.rollNumMap, iter.second.rollNumStr);
		_EncodeFlopString(playerData.dayFlopMap, iter.second.dayFlopStr);
		_EncodeFlopString(playerData.weekFlopMap, iter.second.weekFlopStr);
	}
}

void TCSPlayer::_EncodeString(std::map<UInt32, UInt32>& strMap, const std::string& str)
{
	if (str.empty())
	{
		strMap.clear();
		return;
	}

	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(str, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");

		if (param_2.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(this) << "_EncodeString Error!" << str << LogStream::eos;
			continue;
		}

		UInt32 key = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		strMap[key] = val;
	}
}

std::string TCSPlayer::_DecodeString(const std::map<UInt32, UInt32>& strMap)
{
	std::string str;
	for (auto& it : strMap)
	{
		str.append(std::to_string(it.first)).append(",")
			.append(std::to_string(it.second)).append("|");
	}

	if (!str.empty())
	{
		str.pop_back();
	}

	return str;
}

void TCSPlayer::_EncodeFlopString(std::map<FlopKey, UInt32>& strMap, const std::string& str)
{
	if (str.empty())
	{
		strMap.clear();
		return;
	}

	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(str, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");

		if (param_2.size() != 3)
		{
			ErrorLogStream << PLAYERINFO(this) << "_EncodeFlopString Error!" << str << LogStream::eos;
			continue;
		}

		UInt32 grade = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 stage = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[2]);
		FlopKey flopKey(grade, stage);
		strMap[flopKey] = num;
	}
}

std::string TCSPlayer::_DecodeFlopString(const std::map<FlopKey, UInt32>& strMap)
{
	std::string str;
	for (auto& it : strMap)
	{
		str.append(std::to_string(it.first.grade)).append(",")
			.append(std::to_string(it.first.stage)).append(",")
			.append(std::to_string(it.second)).append("|");
	}

	if (!str.empty())
	{
		str.pop_back();
	}

	return str;
}