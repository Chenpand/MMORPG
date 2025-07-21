#include "RoleHistoryHonor.h"

#include <CLRecordClient.h>
#include <CLMatchDefine.h>
#include <CLHonorProtocol.h>

#include "Player.h"
#include "HonorMgr.h"

RoleHistoryHonor::RoleHistoryHonor()
{
	m_player = NULL;
}

RoleHistoryHonor::~RoleHistoryHonor()
{

}

void RoleHistoryHonor::LoadData(CLRecordCallback* pCallback)
{
	if (NULL == pCallback) return;
	while (pCallback->NextRow())
	{
		HistoryHonor hHonor;
		hHonor.guid = pCallback->GetKey();
		hHonor.totalHonor = pCallback->GetData("total_honor");

		std::string pvpStr = pCallback->GetData("pvp_info").GetString();
		_EncodePvpInfoStr(hHonor.pvpCntMap, pvpStr);

		UInt32 date = pCallback->GetData("date_type");
		m_HistoryMap[date] = hHonor;
	}
}

void RoleHistoryHonor::OnOnline(bool isDayChange)
{
	if (isDayChange)
	{
		OnLogicDayChanged();
	}

	HandlerHonor();
}

void RoleHistoryHonor::HandlerHonor()
{
	CLProtocol::SceneHonorRes res;

	const RoleHonor* roleHonor = HonorMgr::Instance()->FindRoleHonor(GetOwner()->GetID());
	if (roleHonor != NULL)
	{
		res.honorLvl = roleHonor->GetHonorLvl();
		res.honorExp = roleHonor->GetHonorExp();
		res.lastWeekRank = roleHonor->GetLastWeekRank();
		res.historyRank = roleHonor->GetHistoryRank();
		res.highestHonorLvl = roleHonor->GetHighestHonorLvl();
		res.isUseCard = roleHonor->GetUseGuildCard();
	}

	res.rankTime = HonorMgr::Instance()->NextRankTime();

	for (auto& iter : m_HistoryMap)
	{
		CLProtocol::HistoryHonorInfo honorInfo;
		honorInfo.dateType = iter.first;
		honorInfo.totalHonor = iter.second.totalHonor;

		for (auto& it : iter.second.pvpCntMap)
		{
			CLProtocol::PvpStatistics pvpSta;
			pvpSta.pvpType = it.first;
			pvpSta.pvpCnt = it.second;
			honorInfo.pvpStatisticsVec.emplace_back(pvpSta);
		}

		res.historyHonorInfoVec.emplace_back(honorInfo);
	}

	GetOwner()->SendProtocol(res);
}

void RoleHistoryHonor::OnLogicDayChanged()
{
	// 先删掉昨日的
	auto yesterday = m_HistoryMap.find(HONOR_DATE_TYPE_YESTERDAY);
	if (yesterday != m_HistoryMap.end())
	{
		_DelDB(yesterday->second.guid);
		m_HistoryMap.erase(yesterday);
	}

	// 跨天今日的赋给昨日
	auto today = m_HistoryMap.find(HONOR_DATE_TYPE_TODAY);
	if (today == m_HistoryMap.end())
	{
		return;
	}

	m_HistoryMap[HONOR_DATE_TYPE_YESTERDAY] = today->second;
	_UpdateDateType(today->second.guid, HONOR_DATE_TYPE_YESTERDAY);

	// 今日数据清空
	m_HistoryMap.erase(today);
} 

void RoleHistoryHonor::OnLogicWeekChanged(UInt32 changeWeekNum)
{
	// 先把上周的删掉
	auto lastWeek = m_HistoryMap.find(HONOR_DATE_TYPE_LAST_WEEK);
	if (lastWeek != m_HistoryMap.end())
	{
		_DelDB(lastWeek->second.guid);
		m_HistoryMap.erase(lastWeek);
	}

	// 跨周本周的赋给上周
	auto thisWeek = m_HistoryMap.find(HONOR_DATE_TYPE_THIS_WEEK);
	if (thisWeek == m_HistoryMap.end())
	{
		return;
	}

	// 跨一周以上，清除本周跟上周的
	if (changeWeekNum > 1)
	{
		_DelDB(thisWeek->second.guid);
	}
	else
	{
		m_HistoryMap[HONOR_DATE_TYPE_LAST_WEEK] = thisWeek->second;
		_UpdateDateType(thisWeek->second.guid, HONOR_DATE_TYPE_LAST_WEEK);
	}

	// 本周数据清空
	m_HistoryMap.erase(thisWeek);
}

void RoleHistoryHonor::OnPvP(UInt32 pvpType, UInt32 obtainHonor)
{
	if (!_CheckPvpType(pvpType)) return;

	InfoLogStream << PLAYERINFO(GetOwner()) << "pvpType:" << pvpType << " add honor:" << obtainHonor << LogStream::eos;

	// 需要分别更总计，今日，本周计数
	_UpdatePkCntAndHonor(HONOR_DATE_TYPE_TOTAL, pvpType, obtainHonor);
	_UpdatePkCntAndHonor(HONOR_DATE_TYPE_TODAY, pvpType, obtainHonor);
	_UpdatePkCntAndHonor(HONOR_DATE_TYPE_THIS_WEEK, pvpType, obtainHonor);
}

void RoleHistoryHonor::AddHonor(UInt32 honor)
{
	_AddHonor(HONOR_DATE_TYPE_TOTAL, honor);
	_AddHonor(HONOR_DATE_TYPE_TODAY, honor);
	_AddHonor(HONOR_DATE_TYPE_THIS_WEEK, honor);
}

void RoleHistoryHonor::HonorRedPoint()
{
	CLProtocol::SceneHonorRedPoint point;
	GetOwner()->SendProtocol(point);
}

HistoryHonor* RoleHistoryHonor::_GetHistoryHonor(UInt32 dateType)
{
	auto iter = m_HistoryMap.find(dateType);
	return iter != m_HistoryMap.end() ? &(iter->second) : NULL;
}

bool RoleHistoryHonor::_CheckPvpType(UInt32 pvpType)
{
	switch (pvpType)
	{
	case PK_SEASON_1V1:break;
	case PK_WUDAODAHUI:break;
	case PK_PREMIUM_LEAGUE_PRELIMINAY: break;
	case PK_PREMIUM_LEAGUE_BATTLE: break;
	case PK_3V3_CHIJI: break;
	case PK_3V3_ACTIVITY: break;
	case PK_2V2_ACTIVITY: break;
	default:return false;
	}

	return true;
}

void RoleHistoryHonor::_UpdatePkCntAndHonor(UInt32 dateType, UInt32 pvpType, UInt32 obtainHonor)
{
	bool isNew = false;
	HistoryHonor* hHonor = _GetHistoryHonor(dateType);
	if (NULL == hHonor)
	{
		hHonor = &m_HistoryMap[dateType];
		hHonor->guid = CLRecordClient::Instance()->GenGuid();
		isNew = true;

		if (dateType == HONOR_DATE_TYPE_TOTAL)
		{
			HonorRedPoint();
		}
	}

	hHonor->totalHonor += obtainHonor;
	hHonor->pvpCntMap[pvpType] += 1;
	_UpdateDB(dateType, isNew);
}

void RoleHistoryHonor::_UpdateDB(UInt32 date, bool isInsert)
{
	HistoryHonor& honor = m_HistoryMap[date];
	if (isInsert)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_honor_history", honor.guid);
		cmd->PutData("owner", GetOwner()->GetID());
		cmd->PutData("date_type", date);
		cmd->PutData("total_honor", honor.totalHonor);
		std::string str;
		_DecodePvpInfoStr(honor.pvpCntMap, str);
		cmd->PutData("pvp_info", str);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_honor_history", honor.guid, false);
		cmd->PutData("total_honor", honor.totalHonor);
		std::string str;
		_DecodePvpInfoStr(honor.pvpCntMap, str);
		cmd->PutData("pvp_info", str);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void RoleHistoryHonor::_DelDB(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_honor_history", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void RoleHistoryHonor::_UpdateDateType(UInt64 guid, UInt32 date)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_honor_history", guid, false);
	cmd->PutData("date_type", date);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void RoleHistoryHonor::_AddHonor(UInt32 dateType, UInt32 honor)
{
	bool isNew = false;
	HistoryHonor* hHonor = _GetHistoryHonor(dateType);
	if (NULL == hHonor)
	{
		hHonor = &m_HistoryMap[dateType];
		hHonor->guid = CLRecordClient::Instance()->GenGuid();
		isNew = true;
	}

	hHonor->totalHonor += honor;
	_UpdateDB(dateType, isNew);
}

void RoleHistoryHonor::_EncodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, const std::string& pvpStr)
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(pvpStr, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");

		if (param_2.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "_EncodePvpInfoStr Error!" << pvpStr << LogStream::eos;
			continue;
		}

		UInt32 pvpType = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 cnt = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		pvpMap[pvpType] = cnt;
	}
}

void RoleHistoryHonor::_DecodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, std::string& str)
{
	str.clear();
	for (auto& it : pvpMap)
	{
		str.append(std::to_string(it.first)).append(",")
			.append(std::to_string(it.second)).append("|");
	}

	if (!str.empty())
	{
		str.pop_back();
	}
}
