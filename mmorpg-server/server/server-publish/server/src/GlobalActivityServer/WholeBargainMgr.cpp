#include "WholeBargainMgr.h"

#include <CLGlobalActivityProtocol.h>
#include <CLWholeBargainDiscountDataEntry.h>
#include <CLRecordClient.h>

#include "GASPlayer.h"
#include "GASGameZoneMgr.h"
#include "GASSelectInitDataCallBack.h"

struct SelectWholeBargainCallBack : public SelectInitDataCallback
{
	SelectWholeBargainCallBack() {}

	bool OnLoadFinished()
	{
		return WholeBargainMgr::Instance()->LoadWholeBargain(this);
	}
};

struct SelectWholeBargainPlayerJoinCallBack : public SelectInitDataCallback
{
	SelectWholeBargainPlayerJoinCallBack() {}

	bool OnLoadFinished()
	{
		return WholeBargainMgr::Instance()->LoadWholeBargainPlayerJoin(this);
	}
};

WholeBargainMgr::WholeBargainMgr()
{
	m_CurActStatus = AS_END;
	m_CurActId = 0;
}

WholeBargainMgr::~WholeBargainMgr()
{

}

void WholeBargainMgr::OnInit()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_whole_bargain");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectWholeBargainCallBack());

	CLSelectCommand* cmd_player = CLRecordClient::Instance()->PrepareSelectCommand("t_whole_bargain_player_join");
	CLRecordClient::Instance()->SendCommand(cmd_player, new SelectWholeBargainPlayerJoinCallBack());
}

bool WholeBargainMgr::LoadWholeBargain(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		UInt32 actId = callback->GetData("cur_act_id");
		UInt32 joinNum = callback->GetData("join_num");

		m_HistoryActJoinNumMap[actId] = joinNum;
	}

	return true;
}

bool WholeBargainMgr::LoadWholeBargainPlayerJoin(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		std::string uid = callback->GetData("player_id").GetString();
		UInt32 joinNum = callback->GetData("join_num");

		m_PlayerJoinMap[uid] = joinNum;
	}

	return true;
}

void WholeBargainMgr::OnActivityStart(UInt32 actId)
{
	m_CurActStatus = AS_IN;
	m_CurActId = actId;

	auto iter = m_HistoryActJoinNumMap.find(actId);
	if (iter == m_HistoryActJoinNumMap.end())
	{
		// 首次开活动
		InfoLogStream << "whole bargain start actId:" << actId << LogStream::eos;

		_InsertDB(actId, 0);
		_ClearPlayerJoinNum();
	}
}

void WholeBargainMgr::OnActivityEnd()
{
	m_CurActStatus = AS_END;
	UInt32 discount = GetDiscount(m_CurActId);

	InfoLogStream << "whole bargain end id" << m_CurActId << " join num:" << _GetActJoinNum(m_CurActId) 
		<< " discount:" << discount << LogStream::eos;

	// 活动结束通知各区服折扣
	_DiscountSync(discount);
}

void WholeBargainMgr::HandleAddJoinNum(const std::string& uid, UInt32 num)
{
	if (m_CurActStatus == AS_END)
	{
		ErrorLogStream << "whole bargain end playerId:" << uid << " num:" << num << LogStream::eos;
		return;
	}

	m_HistoryActJoinNumMap[m_CurActId] += num;

	bool insert = false;
	auto iter = m_PlayerJoinMap.find(uid);
	if (iter == m_PlayerJoinMap.end())
		insert = true;

	m_PlayerJoinMap[uid] += num;
	_UpdatePlayerJoinNumDB(uid, m_PlayerJoinMap[uid], insert);

	UInt32 actJoinNum = m_HistoryActJoinNumMap[m_CurActId];

	InfoLogStream << "whole bargain playerId:" << uid << " num:" << num << " joinNum:" << actJoinNum << LogStream::eos;

	_UpdateDB(m_CurActId, actJoinNum);
}

void WholeBargainMgr::HandleWholeBargainReq(GASPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::GASWholeBargainRes res;
	res.playerJoinNum = _GetPlayerJoinNum(player->GetUId());
	res.joinNum = _GetActJoinNum(m_CurActId);
	res.maxNum = WholeBargainDiscountDataEntryMgr::Instance()->GetMaxJoinNum();
	res.discount = GetDiscount(m_CurActId);
	player->SendProtocol(res);
}

UInt32 WholeBargainMgr::GetDiscount(UInt32 actId)
{
	return WholeBargainDiscountDataEntryMgr::Instance()->GetDiscount(_GetActJoinNum(actId));
}

void WholeBargainMgr::ZoneQueryDiscount(UInt32 zoneId, UInt32 actId)
{
	auto zone = GASGameZoneMgr::Instance()->FindGASGameZone(zoneId);
	if (NULL == zone)
	{
		ErrorLogStream << "whole bargain not find game zone id:" << zoneId << LogStream::eos;
		return;
	}

	CLProtocol::GASWholeBargainDiscountSync res;
	res.discount = GetDiscount(actId);
	zone->BroadcastToWorld(res);
}

UInt32 WholeBargainMgr::_GetActJoinNum(UInt32 actId)
{
	auto iter = m_HistoryActJoinNumMap.find(actId);
	return iter != m_HistoryActJoinNumMap.end() ? iter->second : 0;
}

void WholeBargainMgr::_DiscountSync(UInt32 discount)
{
	CLProtocol::GASWholeBargainDiscountSync sync;
	sync.discount = discount;
	GASGameZoneMgr::Instance()->BroadcastToWorld(sync);
}

UInt32 WholeBargainMgr::_GetPlayerJoinNum(const std::string& uid)
{
	auto iter = m_PlayerJoinMap.find(uid);
	return iter != m_PlayerJoinMap.end() ? iter->second : 0;
}

void WholeBargainMgr::_ClearPlayerJoinNum()
{
	m_PlayerJoinMap.clear();

	std::ostringstream cond;
	cond << "1=1";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_whole_bargain_player_join", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WholeBargainMgr::_UpdatePlayerJoinNumDB(const std::string& uid, UInt32 num, bool isInsert)
{
	if (isInsert)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_whole_bargain_player_join", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("player_id", uid);
		cmd->PutData("join_num", num);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		std::ostringstream cond;
		cond << "player_id='" << uid <<"'";
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_whole_bargain_player_join", cond.str());
		cmd->PutData("join_num", num);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void WholeBargainMgr::_InsertDB(UInt32 actId, UInt32 joinNum)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_whole_bargain", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("cur_act_id", actId);
	cmd->PutData("join_num", joinNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WholeBargainMgr::_UpdateDB(UInt32 actId, UInt32 joinNum)
{
	std::string cond;
	cond.append("cur_act_id=").append(std::to_string(actId));
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_whole_bargain", cond);
	cmd->PutData("join_num", joinNum);
	CLRecordClient::Instance()->SendCommand(cmd); 
}
