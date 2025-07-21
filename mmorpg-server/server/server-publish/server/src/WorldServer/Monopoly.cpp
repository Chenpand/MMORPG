#include "Monopoly.h"

#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLItemProtocol.h>
#include <CLFunctionDataEntry.h>
#include <CLSettingProtocol.h>
#include <CLWorldMonopolyProtocol.h>


#include "AccountMgr.h"
#include "WSPlayerMgr.h"
#include "WSPlayer.h"
#include "WSItemTransaction.h"
#include "WSActivityMgr.h"
#include "AccountLoadDataCallback.h"
#include "AccountInfo.h"


MonopolyMapGrid::~MonopolyMapGrid()
{
	for (auto& event : m_Event)
	{
		CL_SAFE_DELETE(event);
	}
	m_Event.clear();
}

MonopolyMapGrid* MonopolyMapGrid::GenGrid(UInt32 gridType)
{
	//根据随机表配置生成具体格子
	auto data = MonopolyRandomDataEntryMgr::Instance()->GetGridData(gridType);
	if (data == nullptr)
	{
		ErrorLogStream << "gen grid faild,grid type is " << gridType << LogStream::eos;
		return nullptr;
	}
	MonopolyMapGrid* grid = nullptr;
	if (data->randomType == MRT_ALL)
	{
		grid = new AllMonopolyMapGrid;
	}
	else if (data->randomType == MRT_RANDOM_ONE)
	{
		grid = new RandomMonopolyMapGrid;
	}

	//添加事件
	for (auto& eventData : data->events)
	{
		auto event = MonopolyGridEvent::GenEvent(&eventData);
		if (event == nullptr)
		{
			ErrorLogStream << "gen event faild,event type is " << eventData.eventType << LogStream::eos;
			continue;
		}
		grid->m_Event.push_back(event);
		grid->m_TotalWeight += event->GetWeight();
	}
	
	return grid;
}


MonopolyInfo::MonopolyInfo()
{
	m_SaveTimer.SetInterval(1000);
	m_MissionTimer.SetInterval(30 * Avalon::Time::MSECS_OF_SEC);
	m_Turn = 1;
	m_Vsersion = 0;
}


MonopolyInfo::~MonopolyInfo()
{
	for (auto grid : m_Map)
	{
		CL_SAFE_DELETE(grid);
	}
	m_Map.clear();
}

void MonopolyInfo::Reset()
{
	InfoLogStream << MONOPOLY_INFO(this) << "acitve closed,Reset MonopolyInfo" << LogStream::eos;
	SetTurn(1);
	ResetMap();
	SetCoin(0);
	SetBuff(0);
	SetRemainRoll(0);
	SetRolledTimes(0);
	m_CardsMap.clear();
	m_Cards = "";
	m_CardExchangeMap.clear();
	m_Exchanged = "";
}

void MonopolyInfo::InitMap()
{
	for (auto grid : m_Map)
	{
		CL_SAFE_DELETE(grid);
	}
	m_Map.clear();

	auto data = MonopolyMapDataEntryMgr::Instance()->FindEntry(GetFakeTurn());
	if (data == nullptr)
	{
		ErrorLogStream << GetOwner()->GetAccID() << "can't find map entry,turn is " << GetFakeTurn() << LogStream::eos;
		return;
	}
	for (auto gridType : data->content)
	{
		auto gird = MonopolyMapGrid::GenGrid(gridType);
		if (gird == nullptr)
		{
			ErrorLogStream  << MONOPOLY_INFO(this) << "gen grid faild,grid type is " << gridType << LogStream::eos;
			continue;
		}
		m_Map.push_back(gird);
	}
}

void MonopolyInfo::OnTick(const Avalon::Time &now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		SaveToDB();
	}
	if (m_MissionTimer.IsTimeout(now) && m_MissionID != 0)
	{
		m_MissionID = 0;
	}
}

void MonopolyInfo::OnWeekChange()
{

}

void MonopolyInfo::OnDayChanged()
{

}

void MonopolyInfo::ResetMap()
{
	//从起点重新开始下一轮
	SetCurrnetStep(0);
	//初始化地图
	InitMap();
}

void MonopolyInfo::LoadData()
{
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_monopoly");
	DevLogStream << "accid is " << m_Owner->GetAccID() << LogStream::eos;
	selectCmd->PutCond("accid", m_Owner->GetAccID());
	CLRecordClient::Instance()->SendCommand(selectCmd, new LoadMonopolyCallback(m_Owner));
}

void MonopolyInfo::SetData(CLRecordCallback * callback)
{
	if (callback == nullptr)
	{
		return;
	}
	DevLogStream << "key is " << callback->GetKey() << LogStream::eos;
	SetID(callback->GetKey());
	SetDBData(callback);
}

void MonopolyInfo::InsertToDB()
{
	m_Accid = GetOwner()->GetAccID();
	m_Turn = 1;
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_monopoly", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
	InitMap();
}

void MonopolyInfo::OnOnline()
{
	UInt32 acitvityId = 0;
	WSActivityMgr::Instance()->VisitOpActivity([&] (const OpActivityRegInfo& info)->bool {
		if (info.tmpType == OAT_MONOPOLY && info.state == AS_IN)
		{
			acitvityId = info.dataId;
			return false;
		}
		return true;
	});

	if (acitvityId != GetVsersion())
	{
		InfoLogStream << GetOwner()->GetAccID() << "OnOnline:Reset" << GetVsersion() << LogStream::eos;
		Reset();
		SetVsersion(acitvityId);
	}
	DebugLogStream << GetOwner()->GetAccID() << "OnOnline:" << acitvityId << LogStream::eos;
}

void MonopolyInfo::OnOffline()
{
	DebugLogStream << GetOwner()->GetAccID() << "OnOffline" << LogStream::eos;
	SaveToDB();
}

void MonopolyInfo::SendStatus()
{
	CLProtocol::WorldMonopolyStatusRes res;
	res.turn = GetFakeTurn();
	res.currentGrid = GetCurrnetStep();
	res.rollTimes = GetRemainRoll();
	res.buff = GetBuff();
	GetOwner()->SendProtocol(res);
}

void MonopolyInfo::SendCardList()
{
	CLProtocol::WorldMonopolyCardListRes res;
	auto& cards = GetCards();
	for (auto& cardIter : cards)
	{
		CLProtocol::MonpolyCard mc;
		mc.id = cardIter.first;
		mc.num = cardIter.second;
		res.cardList.push_back(mc);
	}
	GetOwner()->SendProtocol(res);

}

void MonopolyInfo::SendCoin()
{
	CLProtocol::WorldMonopolyCoinRes res;
	res.num = GetCoin();
	GetOwner()->SendProtocol(res);
}

UInt32 MonopolyInfo::ForwardToMap(UInt32 cnt)
{
	UInt32 reslut = 0;
	UInt32 nextGridID = (GetCurrnetStep() + cnt >= GetMaxGridID()) ? GetMaxGridID() : (GetCurrnetStep() + cnt);
	reslut = nextGridID - GetCurrnetStep();
	SetCurrnetStep(nextGridID);

	if (nextGridID >= m_Map.size())
	{
		ErrorLogStream << MONOPOLY_INFO(this) << "can't move to this grid:" << nextGridID << LogStream::eos;
		return 0;
	}
	
	if (GetPlayer() == nullptr)
	{
		ErrorLogStream << MONOPOLY_INFO(this) << "player not online" << nextGridID << LogStream::eos;
		return 0;
	}

	//触发格子事件
	m_Map[GetCurrnetStep()]->OnTriged(GetPlayer());

	//检查是否走完了，走完了则重置
	if (nextGridID >= GetMaxGridID())
	{
		//轮数加1
		SetTurn(GetTurn() + 1);
		ResetMap();
	}
	return reslut;
}

UInt32 MonopolyInfo::OnOwnerRollOnce(UInt32& rolledNum)
{
	InfoLogStream << MONOPOLY_INFO(this) << "request roll" << LogStream::eos;
	//各种条件判断
	if (GetPlayer() == nullptr)
	{
		ErrorLogStream << MONOPOLY_INFO(this) << " owner is null" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (GetRemainRoll() <= 0)
	{
		WarnLogStream << MONOPOLY_INFO(this) << " not have remain roll" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//调整相应变量
	SetRemainRoll(GetRemainRoll() - 1);
	SetRolledTimes(GetRolledTimes() + 1);

	//算出前进步数
	UInt8 num = GetForwardNum();

	//行动
	rolledNum = ForwardToMap(num);

	InfoLogStream << MONOPOLY_INFO(this) << "rolled num is " << rolledNum << LogStream::eos;

	CLProtocol::SceneMonopolyPlayerRollNotify notify;
	notify.roleId = GetPlayer()->GetID();
	GetPlayer()->SendToScene(notify);

	return ErrorCode::SUCCESS;
}

UInt32 MonopolyInfo::GetForwardNum()
{
	const UInt8 MAX_FORWARD = 6;
	return Avalon::Random::RandBetween(1, MAX_FORWARD);
}

UInt32 MonopolyInfo::AddCard(UInt32 id, UInt32 num)
{
	if (m_CardsMap.count(id) == 0)
	{
		m_CardsMap[id] = 0;
	}
	if (GetPlayer() != nullptr)
	{
		GetPlayer()->SendItemUdpLog(GetReason(SOURCE_MONOPOLY).c_str(), id, id, ItemType::ITEM_MONOPOLY_CARD, ST_MONOPOLY_CARD, "", 0, num, m_CardsMap[id]);
	}
	m_CardsMap[id] += num;
	SaveCards();
	return num;
}

UInt32 MonopolyInfo::RemoveCard(UInt32 id, UInt32 num)
{
	auto iter = m_CardsMap.find(id);
	if (iter == m_CardsMap.end())
	{
		return 0;
	}
	if (iter->second < num)
	{
		return 0;
	}

	if (GetPlayer() != nullptr)
	{
		GetPlayer()->SendItemUdpLog(GetReason(SOURCE_MONOPOLY).c_str(), id, id, ItemType::ITEM_MONOPOLY_CARD, ST_MONOPOLY_CARD, "", 0, -num, iter->second);
	}

	iter->second -= num;
	SaveCards();
	return num;
}

UInt32 MonopolyInfo::GetCardNum(UInt32 id)
{
	if (m_CardsMap.count(id) != 0)
	{
		return m_CardsMap[id];
	}
	return 0;
}

UInt32 MonopolyInfo::AddCardExchange(UInt32 id, UInt32 num)
{
	if (m_CardExchangeMap.count(id) == 0)
	{
		m_CardExchangeMap[id] = 0;
	}
	m_CardExchangeMap[id] += num;
	SaveCardExchange();
	return num;
}

UInt32 MonopolyInfo::RemoveCardExchange(UInt32 id, UInt32 num)
{
	auto iter = m_CardExchangeMap.find(id);
	if (iter == m_CardExchangeMap.end())
	{
		return 0;
	}
	if (iter->second < num)
	{
		return 0;
	}
	iter->second -= num;
	SaveCardExchange();
	return num;
}

UInt32 MonopolyInfo::GetCardExchange(UInt32 id)
{
	if (m_CardExchangeMap.count(id) != 0)
	{
		return m_CardExchangeMap[id];
	}
	return 0;
}

UInt8 MonopolyInfo::GetMaxGridID()
{
	return m_Map.size() - 1;
}

void MonopolyInfo::AddMissionLimit(UInt32 id)
{
	m_MissionID = id;
	m_MissionTimer.SetLastTime(CURRENT_TIME.MSec());
}

bool MonopolyInfo::CanJoinMission(UInt32 id)
{
	if (m_MissionID == id)
	{
		InfoLogStream << MONOPOLY_INFO(this) << "id(" << m_MissionID << ") is ok" << LogStream::eos;
		m_MissionID = 0;
		return true;
	}
	InfoLogStream << MONOPOLY_INFO(this) << "id(" << id << ") is fake,true id is " << m_MissionID << LogStream::eos;
	return false;
}

std::map<UInt32, UInt32>& MonopolyInfo::GetCards()
{
	return m_CardsMap;
}

std::map<UInt32, UInt32>& MonopolyInfo::GetCardExchangeList()
{
	return m_CardExchangeMap;
}

void MonopolyInfo::SaveCards()
{
	std::ostringstream os;
	bool first = true;
	for (auto &i : m_CardsMap)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			os << '|';
		}
		os << i.first << ':' << i.second;
	}
	if (static_cast<std::string>(m_Cards) != os.str())
	{
		m_Cards = os.str();
	}
}

void MonopolyInfo::LoadCards()
{
	if (static_cast<std::string>(m_Cards).size() == 0)
	{
		return;
	}
	std::string splitCard = "|";
	std::string splitContent = ":";
	std::vector<std::string> cardVec;
	Avalon::StringUtil::Split(m_Cards, cardVec, splitCard);
	for (UInt32 i = 0; i < cardVec.size(); ++i)
	{
		std::vector<std::string> contentVec;
		Avalon::StringUtil::Split(cardVec[i], contentVec, splitContent);
		m_CardsMap[Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[0])] = Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[1]);
	}
}

void MonopolyInfo::SaveCardExchange()
{
	std::ostringstream os;
	bool first = true;
	for (auto &i : m_CardExchangeMap)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			os << '|';
		}
		os << i.first << ':' << i.second;
	}
	if (static_cast<std::string>(m_Exchanged) != os.str())
	{
		m_Exchanged = os.str();
	}
}

void MonopolyInfo::LoadCardExchange()
{
	if (static_cast<std::string>(m_Exchanged).size() == 0)
	{
		return;
	}
	std::string splitCard = "|";
	std::string splitContent = ":";
	std::vector<std::string> cardVec;
	Avalon::StringUtil::Split(m_Exchanged, cardVec, splitCard);
	for (UInt32 i = 0; i < cardVec.size(); ++i)
	{
		std::vector<std::string> contentVec;
		Avalon::StringUtil::Split(cardVec[i], contentVec, splitContent);
		m_CardExchangeMap[Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[0])] = Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[1]);
	}
}

void MonopolyInfo::SetOwner(AccountInfo* info)
{
	if (info == nullptr)
	{
		return;
	}
	m_Owner = info;
}

AccountInfo* MonopolyInfo::GetOwner()
{
	return m_Owner;
}

WSPlayer * MonopolyInfo::GetPlayer()
{
	if (GetOwner() == nullptr)
	{
		return nullptr;
	}
	return GetOwner()->GetCurrentRole();
}


void MonopolyInfo::SetRemainRoll(UInt32 times, std::string reason)
{
	if (reason == "")
	{
		reason = GetReason(SOURCE_MONOPOLY);
	}
	InfoLogStream << GetOwner()->GetAccID() << "SetRemainRoll:" << times << LogStream::eos;

	if (GetPlayer() != nullptr)
	{
		GetPlayer()->SendAssetUdpLog(AssetType::ASSET_MONOPOLY_ROLL, reason.c_str(), (UInt64)m_RemainRoll, (Int64)times - (Int64)m_RemainRoll, (Int64)times - (Int64)m_RemainRoll, (UInt64)times);
	}
	m_RemainRoll = times;
}

void MonopolyInfo::SetCoin(UInt32 num, std::string reason)
{
	if (reason == "")
	{
		reason = GetReason(SOURCE_MONOPOLY);
	}

	InfoLogStream << GetOwner()->GetAccID() << "SetCoin:" << num << LogStream::eos;

	if (GetPlayer() != nullptr)
	{
		GetPlayer()->SendAssetUdpLog(AssetType::ASSET_MONOPOLY_COIN, reason.c_str(), (UInt64)m_Coin, (Int64)num - (Int64)m_Coin, (Int64)num - (Int64)m_Coin, (UInt64)num);
	}
	
	m_Coin = num;
	SendCoin();
}

void MonopolyInfo::SetBuff(UInt32 num)
{
	m_Buff = num;
	InfoLogStream << GetOwner()->GetAccID() << "SetBuff:" << num << LogStream::eos;
	SendStatus();
}

UInt32 MonopolyInfo::GetFakeTurn()
{
	if (MonopolyMapDataEntryMgr::Instance()->GetMaxTurn() == 0)
	{
		ErrorLogStream << MONOPOLY_INFO(this) << "max turn is 0" << LogStream::eos;
		return 0;
	}

	UInt32 fakeTurn = GetTurn() % MonopolyMapDataEntryMgr::Instance()->GetMaxTurn();
	if (fakeTurn == 0)
	{
		fakeTurn = MonopolyMapDataEntryMgr::Instance()->GetMaxTurn();
	}
	return fakeTurn;
}

void MonopolyInfo::SaveToDB()
{
	DevLogStream << GetOwner()->GetAccID() << "save monopoly data begin" << LogStream::eos;
	if (GetID() == 0)
	{
		InsertToDB();
		return;
	}


	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_monopoly", GetID(), false);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
		DevLogStream << GetOwner()->GetAccID() << "save monopoly data success" << LogStream::eos;
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}

void AllMonopolyMapGrid::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "AllMonopolyMapGrid player is null " << LogStream::eos;
		return;
	}
	for (auto& event : m_Event)
	{
		if (event == nullptr)
		{
			continue;
		}
		event->OnTriged(player);
	}
}

void RandomMonopolyMapGrid::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "RandomMonopolyMapGrid player is null" << LogStream::eos;
		return;
	}
	UInt32 rnd = Avalon::Random::RandBetween(1, m_TotalWeight);
	for (auto& event : m_Event)
	{
		if (event == nullptr)
		{
			ErrorLogStream << "event is null " << LogStream::eos;
			continue;
		}
		if (rnd < event->GetWeight())
		{
			event->OnTriged(player);
			return;
		}
		rnd -= event->GetWeight();
	}
}

MonopolyGridEvent* MonopolyGridEvent::GenEvent(MonopolyGridEventDataEntry* data)
{
	MonopolyGridEvent* event = nullptr;
	switch (data->eventType)
	{
	case MGET_ADD_ITEM:
		event = new AddItemMonopolyEvent;
		break;
	case MGET_ADD_COIN:
		event = new AddCoinMonopolyEvent;
		break;
	case MGET_BUFF:
		event = new AddBuffMonopolyEvent;
		break;
	case MGET_MISSION:
		event = new JoinMissionMonopolyEvent;
		break;
	case MGET_RANDOM_CARD:
		event = new RandomCardMonopolyEvent;
		break;
	case MGET_ADD_ROLL:
		event = new AddRollMonopolyEvent;
		break;
	case MGET_NULL:
		event = new NullMonopolyEvent;
		break;
	default:
		break;
	}
	if (event != nullptr)
	{
		event->m_Id = data->id;
		event->m_Param = data->param;
		event->m_Weight = data->weight;
		event->m_Num = data->num;
	}
	return event;
}

void MonopolyGridEvent::SendResult(WSPlayer * player, UInt32 result)
{
	if (player == nullptr)
	{
		ErrorLogStream << "SendResult player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	InfoLogStream << PLAYERINFO(player) << "Monopoly roll once reslut is(" << m_Id << "," << result << ")" << LogStream::eos;
	CLProtocol::WorldMonopolyNotifyResult notify;
	notify.resultId = m_Id;
	notify.param = result;
	player->SendProtocol(notify);
}

void AddItemMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "AddItemMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	ItemRewardVec vec;
	vec.emplace_back(m_Param, m_Num, 0, 0);
	player->AddItem(GetReason(LogSourceType::SOURCE_MONOPOLY), vec, false);
	SendResult(player);
}

void AddCoinMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "AddCoinMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	auto& info = player->GetMonopolyInfo();
	UInt32 coin = m_Num;
	if (info.GetBuff() > 0)
	{
		coin += (coin * info.GetBuff()) / 100;
	}
	info.SetBuff(0);
	info.SetCoin(info.GetCoin() + coin);
	SendResult(player);
}

void AddBuffMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "AddBuffMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	player->GetMonopolyInfo().SetBuff(player->GetMonopolyInfo().GetBuff() + m_Num);
	SendResult(player);
}

void JoinMissionMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "JoinMissionMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	player->GetMonopolyInfo().AddMissionLimit(m_Param);
	SendResult(player);
}

void RandomCardMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "RandomCardMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	auto& vec = MonopolyCardDataEntryMgr::Instance()->cardIDVec;
	UInt32 sit = Avalon::Random::RandBetween(0, vec.size() - 1);
	player->GetMonopolyInfo().AddCard(vec[sit], m_Num);
	SendResult(player, vec[sit]);
}

void RandomCardMonopolyEvent::SendResult(WSPlayer * player, UInt32 result)
{
	if (player == nullptr)
	{
		ErrorLogStream << "RandomCardMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	InfoLogStream << PLAYERINFO(player) << "Monopoly roll once reslut is(" << m_Id << "," << result << ")" << LogStream::eos;
	CLProtocol::WorldMonopolyNotifyResult notify;
	notify.resultId = m_Id;
	notify.param = result;
	player->SendProtocol(notify);
}

void AddRollMonopolyEvent::OnTriged(WSPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "AddRollMonopolyEvent player is null,event id is" << m_Id << LogStream::eos;
		return;
	}
	player->GetMonopolyInfo().SetRemainRoll(player->GetMonopolyInfo().GetRemainRoll() + m_Num);
	SendResult(player);
}
