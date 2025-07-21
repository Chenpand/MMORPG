#include "TeamCopyMgr.h"

#include <CLTeamCopyProtocol.h>
#include <CLMoveProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLRecordClient.h>
#include <CLFunctionDataEntry.h>
#include "Player.h"
#include "SSRouter.h"
#include "Scene.h"
#include "SSServiceSwitchMgr.h"
#include "ActivityMgr.h"
#include "TCSMgr.h"

void TeamCopySelfData::ChangeDay(const Avalon::Time& now)
{
	dayTime = now.MSec();
	dayNumStr = "";
	dayQuitTeamNum = 0;
	dayFlopStr = "";
}

void TeamCopySelfData::ChangeWeek(const Avalon::Time& now)
{
	ChangeDay(now);
	weekTime = now.MSec();
	weekNumStr = "";
	weekQuitTeamNum = 0;
	weekFlopStr = "";
	extraNumStr = "";
	rollNumStr = "";
}

std::string TeamCopySelfData::LogStr()
{
	std::stringstream str;
	str << "teamType:" << teamType
		<< "dayNumStr:" << dayNumStr.c_str()
		<< "dayQuitTeamNum:" << dayQuitTeamNum
		<< "weekNumStr:" << weekNumStr.c_str()
		<< "weekQuitTeamNum:" << weekQuitTeamNum
		<< "modelPassStr:" << modelPassStr.c_str()
		<< "dayFlopStr:" << dayFlopStr.c_str()
		<< "weekFlopStr:" << weekFlopStr.c_str()
		<< "extraNumStr:" << extraNumStr.c_str()
		<< "rollNumStr:" << rollNumStr.c_str();

	return str.str();
}

void TeamCopySelfData::UpdateToDB(bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_team_copy", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void TeamCopySelfData::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_team_copy", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}


TeamCopyMgr::TeamCopyMgr()
{
	m_IsTeamCopyFlag = false;
}

TeamCopyMgr::~TeamCopyMgr()
{

}

void TeamCopyMgr::OnInit(const Avalon::Time& now)
{
	for (UInt32 k = TC_TYPE_ONE;k < TC_TYPE_MAX;++k)
	{
		if (m_SelfDataMap.count(k))
			continue;

		TeamCopySelfData data;
		data.SetID(CLRecordClient::Instance()->GenGuid());
		data.owner = GetOwner()->GetID();
		data.teamType = k;
		data.ChangeWeek(now);
		data.modelPassStr = "";
		data.InsertToDB();
		m_SelfDataMap[k] = data;
	}
}

void TeamCopyMgr::OnTick(const Avalon::Time& now)
{
	// 多个团本只要开启了一个就可以tick
	if ( !_IsOpen())
	{
		return;
	}

	if (m_SelfDataMap.size() != TC_TYPE_MAX - 1)
	{
		OnInit(now);
	}

	for (auto& itr : m_SelfDataMap)
	{
		TeamCopySelfData& data = itr.second;

		// 周几6点刷新
		if ( !isSameWeek(data.weekTime, now.MSec(), GetTCRefreshWeek(itr.first), 6))
		{
			data.ChangeWeek(now);
			data.UpdateToDB(true);

			// 刷新团本通行证使用次数
			_RefreshTeamCopyItem(itr.first);
			PlayerInfoToTeamCopy(itr.first);
		}
		else if ( !CheckInSameLogicDay(UInt32(data.dayTime / 1000), (UInt32)now.Sec()))
		{
			data.ChangeDay(now);
			data.UpdateToDB(true);
			PlayerInfoToTeamCopy(itr.first);
		}
	}
}

void TeamCopyMgr::LoadData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		TeamCopySelfData data;
		data.SetID(callback->GetKey());
		data.SetDBData(callback);
		m_SelfDataMap[data.teamType] = data;
	}
}

UInt32 TeamCopyMgr::GetWeekNum(UInt32 teamType, UInt32 grade)
{
	// TC_TYPE_MAX 代表所有类型的团本
	if (teamType != TC_TYPE_MAX)
	{
		auto iter = m_SelfDataMap.find(teamType);
		if (iter == m_SelfDataMap.end())
		{
			return 0;
		}

		return PassGradeNum(grade, iter->second.weekNumStr.c_str());
	}

	UInt32 num = 0;
	for (auto& iter : m_SelfDataMap)
	{
		num += PassGradeNum(grade, iter.second.weekNumStr.c_str());
	}

	return num;
}

UInt32 TeamCopyMgr::PassGradeNum(UInt32 grade, const std::string& numStr)
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(numStr, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");

		if (param_2.size() != 2)
			continue;

		UInt32 key = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		if (grade != key)
			continue;

		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		if (val != 0)
		{
			return val;
		}
	}

	return 0;
}

void TeamCopyMgr::UseTeamCopyItem(UInt32 teamType, UInt32 grade, UInt32 num)
{
	auto iter = m_SelfDataMap.find(teamType);
	if (iter == m_SelfDataMap.end())
	{
		return;
	}

	TeamCopySelfData& data = iter->second;

	std::map<UInt32, UInt32> tempMap;
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(data.extraNumStr, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");

		if (param_2.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "parse team copy item extra error!" << data.extraNumStr << LogStream::eos;
			return;
		}

		UInt32 key = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		tempMap[key] = val;
	}

	tempMap[grade] += num;

	std::string tmpStr;
	for (auto& it : tempMap)
	{
		tmpStr.append(std::to_string(it.first)).append(",")
			.append(std::to_string(it.second)).append("|");
	}
	
	if ( !tmpStr.empty())
	{
		tmpStr.pop_back();
		data.extraNumStr = tmpStr;
		data.UpdateToDB(true);
		PlayerInfoToTeamCopy(teamType);
	}
}

void TeamCopyMgr::HandlerTeamCopyPlayerInfo(CLProtocol::TeamCopyPlayerInfoSync& sync)
{
	for (auto& iter : sync.tcTypeDataMap)
	{
		TeamCopySelfData& data = m_SelfDataMap[iter.first];
		data.dayNumStr = iter.second.dayNumStr;
		data.dayQuitTeamNum = iter.second.dayQuitTeamNum;
		data.weekNumStr = iter.second.weekNumStr;
		data.weekQuitTeamNum = iter.second.weekQuitTeamNum;
		data.modelPassStr = iter.second.modelPassStr;
		data.dayFlopStr = iter.second.dayFlopStr;
		data.weekFlopStr = iter.second.weekFlopStr;
		data.rollNumStr = iter.second.rollNumStr;
		data.UpdateToDB(true);

		InfoLogStream << PLAYERINFO(GetOwner()) << data.LogStr() << LogStream::eos;
	}
}

bool TeamCopyMgr::ChangeTeamCopyScene(UInt32 destSceneId)
{
	if ( !_IsOpen(GetTCType(destSceneId)))
	{
		_SceneFailed();
		return false;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::TeamCopyEnterSceneReq::__ID);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & NODE_ID & Router::GetWorldNodeID() &  GetOwner()->GetAccID() &  GetOwner()->GetID() & 
			GetOwner()->GetName() &  GetOwner()->GetOccu() &  GetOwner()->GetLevel() & GetOwner()->GetVipLvl() &
			GetOwner()->GetGuildId() & GetOwner()->GetAwaken();
		stream & ObjID_t(0);
	}
	catch (const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		_SceneFailed();
		return false;
	}

	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));

	Router::SendToTeamCopy(packet);
	Avalon::Packet::Destroy(packet);

	InfoLogStream << "player(" << GetOwner()->GetID() << ") request enter team copy prepare scene(" 
		<< destSceneId << ")." << LogStream::eos;

	// 团本数据发送
	PlayerInfoToTeamCopy(0);
	// 门票数据
	_TicketToTeamCopy();

	return true;
}

void TeamCopyMgr::QuitTeamCopyScene()
{
	CLProtocol::TeamCopyExitSceneReq req;
	req.roleId = GetOwner()->GetID();
	Router::SendToTeamCopy(req);
}

bool TeamCopyMgr::IsTeamCopyScene()
{
	if (NULL == GetOwner()->GetScene())
		return false;

	return GetOwner()->GetScene()->GetDataEntry()->IsTeamCopyScene();
}

void TeamCopyMgr::ReconnectTeamCopy()
{
	CLProtocol::TeamCopyPlayerReconnect req;
	req.roleId = GetOwner()->GetID();
	if (IsTeamCopy())
		req.isSendData = 1;
	else
		req.isSendData = 0;

	Router::SendToTeamCopy(req);
}

void TeamCopyMgr::NotifyTeamCopySceneEnter()
{
	if ( !IsTeamCopy())
	{
		return;
	}

	Avalon::Packet* packet = GetOwner()->MakeSyncObjectPacket(true);
	if (packet)
	{
		Router::SendToTeamCopy(packet);
		Avalon::Packet::Destroy(packet);
	}
}

void TeamCopyMgr::NotifyTeamCopySceneLeave()
{
	if ( !IsTeamCopy())
	{
		return;
	}

	Avalon::Packet* packet = GetOwner()->MakeDelObjectPacket();
	if (packet)
	{
		Router::SendToTeamCopy(packet);
		Avalon::Packet::Destroy(packet);
	}
}

void TeamCopyMgr::TeamCopyDungeonRevive(UInt64 targetId, UInt32 reviveId, UInt32 reviveCoinNum)
{
	CLProtocol::TeamCopyDungeonRevive revive;
	revive.playerId = GetOwner()->GetID();
	revive.targetId = targetId;
	revive.reviveId = reviveId;
	revive.reviveCoinNum = reviveCoinNum;
	Router::SendToTeamCopy(revive);
}

void TeamCopyMgr::PlayerInfoToTeamCopy(UInt32 teamType)
{
	if (!IsTeamCopy())
	{
		return;
	}

	CLProtocol::TeamCopyPlayerInfoSync sync;
	sync.roleId = GetOwner()->GetID();
	sync.equipScore = GetOwner()->GetTotalEquipScore();

	if (teamType == 0)
	{
		for (auto& data : m_SelfDataMap)
		{
			CLProtocol::TCTypeData typeData;
			typeData.dayNumStr = data.second.dayNumStr;
			typeData.dayQuitTeamNum = data.second.dayQuitTeamNum;
			typeData.weekNumStr = data.second.weekNumStr;
			typeData.weekQuitTeamNum = data.second.weekQuitTeamNum;
			typeData.modelPassStr = data.second.modelPassStr;
			typeData.dayFlopStr = data.second.dayFlopStr;
			typeData.weekFlopStr = data.second.weekFlopStr;
			typeData.extraNumStr = data.second.extraNumStr;
			typeData.rollNumStr = data.second.rollNumStr;

			sync.tcTypeDataMap[data.first] = typeData;
		}
	}
	else
	{
		auto iter = m_SelfDataMap.find(teamType);
		if (iter == m_SelfDataMap.end())
			return;

		CLProtocol::TCTypeData typeData;
		typeData.dayNumStr = iter->second.dayNumStr;
		typeData.dayQuitTeamNum = iter->second.dayQuitTeamNum;
		typeData.weekNumStr = iter->second.weekNumStr;
		typeData.weekQuitTeamNum = iter->second.weekQuitTeamNum;
		typeData.modelPassStr = iter->second.modelPassStr;
		typeData.dayFlopStr = iter->second.dayFlopStr;
		typeData.weekFlopStr = iter->second.weekFlopStr;
		typeData.extraNumStr = iter->second.extraNumStr;
		typeData.rollNumStr = iter->second.rollNumStr;

		sync.tcTypeDataMap[iter->first] = typeData;
	}

	Router::SendToTeamCopy(sync);
}

void TeamCopyMgr::TicketAlterToTeamCopy(UInt32 itemId)
{
	if ( !IsTeamCopy())
	{
		return;
	}
	
	bool isTickItem = false;
	do 
	{
		UInt32 ticketId_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_ID_1);
		UInt32 ticketId_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_ID_2);
		if (itemId == ticketId_1 || itemId == ticketId_2)
		{
			isTickItem = true;
			break;
		}

		ticketId_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_ID_1);
		ticketId_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_ID_2);
		if (itemId == ticketId_1 || itemId == ticketId_2)
		{
			isTickItem = true;
			break;
		}

	} while (0);

	if (isTickItem)
	{
		_TicketToTeamCopy();
	}
}

void TeamCopyMgr::EquipScoreToTeamCopy()
{
	CLProtocol::TeamCopyPlayerEquipScoreSync sync;
	sync.roleId = GetOwner()->GetID();
	sync.equipScore = GetOwner()->GetTotalEquipScore();
	Router::SendToTeamCopy(sync);
}

void TeamCopyMgr::NotifyTeamCopyPlayerLoading(UInt32 isLoading)
{
// 	if ( !IsTeamCopy())
// 	{
// 		return;
// 	}

	CLProtocol::TeamCopyPlayerLoadingSync sync;
	sync.roleId = GetOwner()->GetID();
	sync.isLoading = isLoading;
	Router::SendToTeamCopy(sync);
}

void TeamCopyMgr::ClearDungeonEvent(ObjID_t param1, ObjID_t param2, ObjID_t param3)
{
	// 通关团本的活动
	GetOwner()->GetActiveTaskMgr().TeamCopyClearDungeon(param1, param3);

	// 团本成就任务
	GetOwner()->GetAchievementTaskMgr().OnClearTeamCopy(param3, param1, param2);
}

void TeamCopyMgr::_SceneFailed()
{
	CLProtocol::SceneNotifyEnterScene notify;
	notify.result = ErrorCode::TEAM_COPY_FUN_LOCK;
	GetOwner()->SendProtocol(notify);

	CLProtocol::SceneSyncSceneObject a;
	GetOwner()->SendProtocol(a);
}

bool TeamCopyMgr::_IsOpen()
{
	for (UInt32 k = TC_TYPE_ONE; k < TC_TYPE_MAX; ++k)
	{
		if (_IsOpen(k))
		{
			return true;
		}
	}

	return false;
}

bool TeamCopyMgr::_IsOpen(UInt32 teamType)
{
	switch (teamType)
	{
	case TC_TYPE_ONE :
	{
		if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_TEAM_COPY))
			return false;

		if (!GetOwner()->CheckFuncFlag(FUNC_TEAM_COPY))
			return false;

		return true;
	}
	case TC_TYPE_TWO :
	{
		if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_TEAM_COPY_TWO))
			return false;

		if (!GetOwner()->CheckFuncFlag(FUNC_TEAM_COPY_TWO))
			return false;

		return true;
	}
	default:break;
	}

	return false;
}

void TeamCopyMgr::_TicketToTeamCopy()
{
	CLProtocol::TeamCopyPlayerTicketSync sync;
	sync.roleId = GetOwner()->GetID();

	{
		UInt32 ticketId_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_ID_1);
		UInt32 ticketNum_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_NUM_1);
		UInt32 ticketId_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_ID_2);
		UInt32 ticketNum_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_TICKET_NUM_2);

		bool isEnough = _TicketIsEnough(ticketId_1, ticketNum_1, ticketId_2, ticketNum_2);
		sync.ticketMap[TC_TYPE_ONE] = isEnough;
	}

	{
		UInt32 ticketId_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_ID_1);
		UInt32 ticketNum_1 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_NUM_1);
		UInt32 ticketId_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_ID_2);
		UInt32 ticketNum_2 = TCSMgr::Instance()->GetCfgval(TC_VALUE_2_TICKET_NUM_2);
		bool isEnough = _TicketIsEnough(ticketId_1, ticketNum_1, ticketId_2, ticketNum_2);
		sync.ticketMap[TC_TYPE_TWO] = isEnough;
	}

	Router::SendToTeamCopy(sync);
}

bool TeamCopyMgr::_TicketIsEnough(UInt32 ticket1, UInt32 ticketNum1, UInt32 ticket2, UInt32 ticketNum2)
{
	// 检查是否参与团本扶持活动，有门票消耗减少
	std::vector<UInt32> discountVec;
	std::vector<OpActivityRegInfo*> actVec = ActivityMgr::Instance()->GetOpActDataByTmpTypes(OAT_TEAM_COPY_SUPPORT);
	if (actVec.size() == 1)
	{
		discountVec = (*actVec.begin())->parm3;
	}

	if (ticket1 != 0 && ticketNum1 != 0)
	{
		UInt32 dis = 0;
		if (discountVec.size() >= 1)
			dis = discountVec[0];

		if (GetOwner()->GetItemNum(ticket1) < (ticketNum1 > dis ? ticketNum1 - dis : 0))
		{
			return false;
		}
	}

	if (ticket2 != 0 && ticketNum2 != 0)
	{
		UInt32 dis = 0;
		if (discountVec.size() >= 2)
			dis = discountVec[1];

		if (GetOwner()->GetItemNum(ticket2) < (ticketNum2 > dis ? ticketNum2 - dis : 0))
		{
			return false;
		}
	}

	return true;
}

void TeamCopyMgr::_RefreshTeamCopyItem(UInt32 teamType)
{
	const std::vector<UInt32>& teamCopyItems = ItemDataEntryMgr::Instance()->GetTeamCopyItem();
	for (auto& iter : teamCopyItems)
	{
		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(iter);
		if (itemData->thirdType != teamType)
		{
			continue;
		}

		std::string counterName = COUNTER_TEAM_COPY_ITEM_USE_CNT + Avalon::StringUtil::ConvertToString(iter);
		if (GetOwner()->GetCount(counterName.c_str()) != 0)
		{
			GetOwner()->SetCounter(counterName.c_str(), 0);
		}
	}
}