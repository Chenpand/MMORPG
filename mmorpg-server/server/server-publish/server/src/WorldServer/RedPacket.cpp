#include "RedPacket.h"
#include <udplog_typedef.h>
#include <CLRedPacketProtocol.h>
#include <CLRecordClient.h>
#include <CLRedPacketDataEntry.h>
#include <CLSystemValueDataEntry.h>

#include "WSPlayerMgr.h"
#include "AnnouncementMgr.h"
#include "GuildMgr.h"
#include "CLSysNotify.h"
#include "CLMsgParser.h"
#include "WSActivityMgr.h"
#include "WSApplication.h"
#include "PlayerJoins.h"
#include "RedPacketMgr.h"

RedPacket::RedPacket()
{
	m_OwnerId = 0;
	m_OwnerLevel = 0;
	m_OwnerOccu = 0;
	m_Type = 0;
	m_Status = 0;
	m_Reason = 0;
	m_TotalMoney = 0;
	m_RemainMoney = 0;
	m_Num = 0;
	m_StatusTime = 0;
	m_Cond1 = 0;
	m_SaveTimer.SetLastTime(CURRENT_TIME.MSec());
	m_SaveTimer.SetInterval(1000);
	m_Cond2 = 0;
	m_GenMoneyVec = false;
}

RedPacket::RedPacket(WSPlayer* player, RedPacketType type, UInt16 reason, UInt32 money)
{
	m_OwnerId = player->GetID();
	m_OwnerName = player->GetName();
	m_OwnerLevel = player->GetLevel();
	m_OwnerOccu = player->GetOccu();
	m_Type = type;
	m_Status = RED_PACKET_STATUS_INIT;
	m_Reason = reason;
	m_TotalMoney = m_RemainMoney = money;
	m_Num = 0;
	m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	m_Cond1 = 0;
	m_SaveTimer.SetLastTime(CURRENT_TIME.MSec());
	m_SaveTimer.SetInterval(1000);
	m_Cond2 = 0;
	m_GenMoneyVec = false;
}

RedPacket::~RedPacket()
{
	for (auto receiver : m_Receivers)
	{
		if (receiver)
		{
			delete receiver;
		}
	}

	m_Receivers.clear();
	m_MoneyVec.clear();
}

bool RedPacket::OnTick(const Avalon::Time& now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		if (GetStatus() == RED_PACKET_STATUS_WAIT_RECEIVE && m_Receivers.size() >= m_Num)
		{
			ChangeStatus(RED_PACKET_STATUS_EMPTY);
		}

		SaveToDB();
	}

	return true;
}

void RedPacket::ChangeStatus(RedPacketStatus status)
{
	if (GetStatus() == status)
	{
		return;
	}

	m_Status = status;
	m_StatusTime = (UInt32)CURRENT_TIME.Sec();

	auto player = WSPlayerMgr::Instance()->FindPlayer(m_OwnerId);
	if (player)
	{
		CLProtocol::WorldNotifySyncRedPacketStatus notify;
		notify.id = GetID();
		notify.status = status;
		player->SendProtocol(notify);
	}
	if (m_Status == RED_PACKET_STATUS_EMPTY)
	{
		RedPacketReceiver* receiver = GetBestReceiver();
		if (receiver)
		{
			receiver->SetBset();
			auto receive_player = WSPlayerMgr::Instance()->FindNormalPlayer(receiver->GetReceiverID());
			if (receive_player)
			{
				CLProtocol::WorldSyncRedPacketRecord syncReceiveRecords;
				syncReceiveRecords.updates.push_back(receiver->ToClient());
				receive_player->SendProtocol(syncReceiveRecords);
}
		}
	}
}

RedPacketStatus RedPacket::GetStatus(ObjID_t playerId, UInt32 accid)
{
	if (m_Type == RED_PACKET_TYPE_GUILD)
	{
		return GetGuildRedPacketStatus(playerId, accid);
	}

	return (RedPacketStatus)(UInt8)m_Status;
}

RedPacketStatus RedPacket::GetGuildRedPacketStatus(ObjID_t playerId, UInt32 accid)
{
	RedPacketStatus state = (RedPacketStatus)(UInt8)m_Status;

	if (m_Type != RED_PACKET_TYPE_GUILD)
	{
		return state;
	}

	if (state != RED_PACKET_STATUS_WAIT_RECEIVE)
	{
		return state;
	}

	//先判断是否已抢完
	if (m_Receivers.size() >= m_Num)
	{
		ChangeStatus(RED_PACKET_STATUS_EMPTY);
		return RED_PACKET_STATUS_EMPTY;
	}

	//判断是否有抢红包资格
	bool isSatisfy = RedPacketMgr::Instance()->IsCanReciveGuildRedPacket(*this, playerId);
	
	//未达成领取资格
	if (!isSatisfy)
	{
		return RED_PACKET_STATUS_UNSATISFY;
	}

	//达成领取资格,判断是否已经抢过
	if (IsPlayerAlreadyOpened(playerId) || IsAccountAlreadyOpend(accid))
	{
		return RED_PACKET_STATUS_RECEIVED;
	}
	
	return state;
}

bool RedPacket::IsEmpty() const
{
	if (m_Receivers.size() >= m_Num || _GetRemainMoney() == 0)
	{
		return true;
	}
	return false;
}

void RedPacket::AddReceiver(RedPacketReceiver* receiver)
{
	m_Receivers.push_back(receiver);
}

RedPacketReceiver* RedPacket::FindReceiver(ObjID_t playerId)
{
	for (auto receiver : m_Receivers)
	{
		if (receiver->GetReceiverID() == playerId)
		{
			return receiver;
		}
	}
	return NULL;
}

RedPacketBaseEntry RedPacket::GetBaseEntry(ObjID_t roleId, UInt32 accid)
{
	RedPacketBaseEntry entry;
	entry.id = GetID();
	entry.name = GetName();
	entry.status = GetStatus(roleId, accid);
	entry.ownerId = m_OwnerId;
	entry.ownerName = m_OwnerName;
	entry.opened = IsPlayerAlreadyOpened(roleId) ? 1 : 0;
	if (m_Type == RED_PACKET_TYPE_GUILD && IsAccountAlreadyOpend(accid))
	{
		entry.opened = 1;
	}
	entry.type = m_Type;
	entry.reason = m_Reason;
	entry.totalMoney = m_TotalMoney;
	entry.totalNum = m_Num;
	entry.remainNum = GetRemainNum();
	entry.guildTimeStamp = GetCond2();
	return entry;
}

RedPacketDetail RedPacket::GetDetailInfo(ObjID_t roleId, UInt32 accid)
{
	RedPacketDetail detail;
	detail.baseEntry = GetBaseEntry(roleId, accid);
	detail.ownerIcon.id = m_OwnerId;
	detail.ownerIcon.name = m_OwnerName;
	detail.ownerIcon.level = m_OwnerLevel;
	detail.ownerIcon.occu = m_OwnerOccu;
	
	for (auto receiver : m_Receivers)
	{
		if (!receiver)
		{
			continue;
		}

		detail.receivers.push_back(receiver->GetEntry());
	}

	return detail;
}

UInt8 RedPacket::GetRemainNum()
{
	return m_Num - m_Receivers.size();
}

UInt32 RedPacket::OnPlayerOpen(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	bool first = false;
	CLProtocol::WorldOpenRedPacketRes res;
	do 
	{
		if (!WSApplication::Instance()->IsOpenRedPacket())
		{
			res.result = ErrorCode::RED_PACKET_NOT_OPEN;
			break;
		}

		if (GetType() == RED_PACKET_TYPE_NEW_YEAR)
		{
			UInt32 activityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_NEW_YEAR_RED_PACKET_ACTIVITY_ID);
			if (WSActivityMgr::Instance()->GetActivityNeedLevel(activityId) > player->GetLevel())
			{
				res.result = ErrorCode::RED_PACKET_NOT_OPEN;
				break;
			}
		}

		if (IsPlayerAlreadyOpened(player->GetID()))
		{
			res.result = ErrorCode::SUCCESS;
			res.detail = GetDetailInfo(player->GetID(), player->GetAccId());
			break;
		}

		if (GetType() == RED_PACKET_TYPE_GUILD
			&& IsAccountOpenRedPacketReachMax(player))
		{
			//res.result = ErrorCode::RED_PACKET_ACCOUNT_GUILD_OPENED_MAX;
			res.result = ErrorCode::SUCCESS;
			res.detail = GetDetailInfo(player->GetID(), player->GetAccId());
			break;
		}

		if (GetType() == RED_PACKET_TYPE_GUILD
			&& IsAccountAlreadyOpend(player->GetAccId()))
		{
			//res.result = ErrorCode::RED_PACKET_ACCOUNT_OPENED;
			res.result = ErrorCode::SUCCESS;
			res.detail = GetDetailInfo(player->GetID(), player->GetAccId());
			break;
		}

		auto data = RedPacketDataEntryMgr::Instance()->FindEntry(GetReason());
		if (!data)
		{
			ErrorLogStream << "player(" << player->GetID() << ") open red packet(" << GetID() << ") can't find data(" << GetReason() << ")" << LogStream::eos;
			res.result = ErrorCode::RED_PACKET_SYS_ERROR;
			break;
		}

		if (!IsMatchCond(player))
		{
			res.result = ErrorCode::RED_PACKET_CANT_OPEN;
			break;
		}

		if (IsEmpty())
		{
			res.result = ErrorCode::SUCCESS;
			res.detail = GetDetailInfo(player->GetID(), player->GetAccId());
			break;
		}

		UInt32 money = _GenMoney();
		auto receiver = new RedPacketReceiver();
		if (!receiver->Init(player, GetID(), money, *this))
		{
			res.result = ErrorCode::RED_PACKET_SYS_ERROR;
			delete receiver;
			break;
		}

		auto moneyItemData = ItemDataEntryMgr::Instance()->FindEntry(data->getMoneyId);
		if (!moneyItemData)
		{
			delete receiver;

			// 金钱道具找不到
			ErrorLogStream << "player(" << player->GetID() << ") open red packet(" << GetID() << ") invalid money item(" << data->getMoneyId << ")" << LogStream::eos;
			res.result = ErrorCode::RED_PACKET_SYS_ERROR;
			break;
		}

		m_RemainMoney = (m_RemainMoney > money) ? (m_RemainMoney - money) : 0;
		m_Receivers.push_back(receiver);

		InfoLogStream << "red packet(" << GetID() << ") player(" << player->GetID() << ") got money(" << money << ") remain(" << _GetRemainMoney() << ")." << LogStream::eos;

		// 添加玩家抽到的货币
		player->AddMoneyItem(::GetReason(SOURCE_TYPE_RED_PACKET).c_str(), moneyItemData->subType, money);

		// 保存数据库
		receiver->InsertToDB();

		res.result = ErrorCode::SUCCESS;
		res.detail = GetDetailInfo(player->GetID(), player->GetAccId());

		CLProtocol::WorldSyncRedPacketRecord notify;
		notify.adds.push_back(receiver->ToClient());
		player->SendProtocol(notify);
		first = true;

		if (GetType() == RED_PACKET_TYPE_GUILD)
		{
			UInt64 newNum = player->GetAccountInfo()->GetAccountCounterNum(ACC_GUILD_REDPACKET_DAILY_MAX) + 1;
			player->GetAccountInfo()->SetAccountCounterNum(player, ACC_GUILD_REDPACKET_DAILY_MAX, newNum);
			player->SendUdpLog("log_guild_readpacket", LOG_GUILD_REDPACKET, GetID(), GUILD_REDPACKET_RECV, data->thrType, 
				GetTotalMoney(), GetNum(), IsEmpty() ? 1 : 0, this->GetOwner());
		}
	} while (0);

	player->SendProtocol(res);

	
	if (IsEmpty())
	{
		// 已经领完
		_OnEmpty(first);
	}

	return res.result;
}

bool RedPacket::IsMatchCond(WSPlayer* player)
{
	if (!player)
	{
		return false;
	}

	if (GetStatus() == RED_PACKET_STATUS_INIT)
	{
		return false;
	}

	if (GetType() == RED_PACKET_TYPE_GUILD)
	{
		if (GetCond1() != player->GetGuildID())
		{
			return false;
		}

		return RedPacketMgr::Instance()->IsCanReciveGuildRedPacket(*this, player->GetID());
	}
	else if (GetType() == RED_PACKET_TYPE_NEW_YEAR)
	{
		return true;
	}

	return false;
}

bool RedPacket::IsPlayerAlreadyOpened(ObjID_t id) const
{
	for (auto receiver : m_Receivers)
	{
		if (!receiver)
		{
			continue;
		}

		if (receiver->GetReceiverID() == id)
		{
			return true;
		}
	}

	return false;
}

bool RedPacket::IsAccountAlreadyOpend(UInt32 accid) const
{
	if (accid == 0)
	{
		return false;
	}
	for (auto receiver : m_Receivers)
	{
		if (!receiver)
		{
			continue;
		}

		if (receiver->GetReceiverAccID() == accid)
		{
			return true;
		}
	}

	return false;
}

bool RedPacket::IsAccountOpenRedPacketReachMax(WSPlayer* player)
{
	if (GetType() != RED_PACKET_TYPE_GUILD)
	{
		return false;
	}
	if (!player || !player->GetAccountInfo())
	{
		return true;
	}
	UInt32 Uplimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_REDPACKET_ACC_RECV_DAY_MAX);
	if (player->GetAccountInfo()->GetAccountCounterNum(ACC_GUILD_REDPACKET_DAILY_MAX) >= Uplimit)
	{
		return true;
	}
	
	return false;
}

UInt32 RedPacket::_GenMoney()
{
	if (!m_GenMoneyVec)
	{
		_GenMoneyVec();
		m_GenMoneyVec = true;
	}

	if (m_Receivers.size() >= m_Num)
	{
		return 0;
	}

	UInt8 remainNum = m_Num - (UInt8)m_Receivers.size();
	UInt8 vecSize = (UInt8)m_MoneyVec.size();
	if (remainNum > vecSize || remainNum == 0)
	{
		return 0;
	}

	return m_MoneyVec[vecSize - remainNum];
}

UInt32 RedPacket::_GetRemainMoney() const
{
	UInt32 money = 0;
	for (auto receiver : m_Receivers)
	{
		if (receiver)
		{
			money += receiver->GetMoney();
		}
	}

	return m_TotalMoney >= money ? m_TotalMoney - money : 0;
}

void RedPacket::_OnEmpty(bool first)
{
	ChangeStatus(RED_PACKET_STATUS_EMPTY);

	auto best = GetBestReceiver();
	if (best == NULL) return;

	if (GetType() == RED_PACKET_TYPE_GUILD && first)
	{
		char buffer[CL_MAX_CHAT_LEN + 1];

		std::string ownerPlayer;
		if (!MsgPackage::GetPlayerMsgTag(ownerPlayer, this->m_OwnerId, this->m_OwnerName, this->m_OwnerOccu, this->m_OwnerLevel)) return;
		std::string receiverPlayer;
		if (!MsgPackage::GetPlayerMsgTag(receiverPlayer, best->GetReceiverID(), best->GetName(), best->GetReceiverOccu(), (UInt16)best->GetReceiverLevel())) return;
		if (!SysNotifyMgr::Instance()->MakeNotify(3021, buffer, CL_MAX_CHAT_LEN, ownerPlayer.c_str(), receiverPlayer.c_str(), best->GetMoney(), best->GetRedPacketID())) return;

		Guild* guild = GuildMgr::Instance()->FindGuildByID(GetCond1());
		if (!guild)
		{
			ErrorLogStream << " RedPacket::_OnEmpty guild is null! id = " << GetCond1() << LogStream::eos;
			return;
		}

		guild->Chat(std::string(buffer), true);
	}
	else if (GetType() == RED_PACKET_TYPE_NEW_YEAR)
	{
		// 新年红包被领完后，通知所有客户端把红包删掉
		CLProtocol::WorldNotifyDelRedPacket notify;
		notify.redPacketList.push_back(GetID());
		Router::BroadcastToPlayer(notify);
	}
}

RedPacketReceiver* RedPacket::GetBestReceiver()
{
	RedPacketReceiver* best = NULL;
	for (auto receiver : m_Receivers)
	{
		if (!receiver)
		{
			continue;
		}

		if (best == NULL || receiver->GetMoney() > best->GetMoney())
		{
			best = receiver;
		}
	}

	return best;
}

void RedPacket::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_red_packet", GetID());
	GetDBData(insertCmd, false);
	insertCmd->PutData("name", GetName());
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}

void RedPacket::SaveToDB()
{
	// 保存自身信息
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_red_packet", GetID(), true);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}

	ClearDirty();
}

void RedPacket::DelFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_red_packet", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);

	for (auto receiver : m_Receivers)
	{
		if (!receiver)
		{
			continue;
		}

		receiver->DelFromDB();
	}
}

void RedPacket::_GenMoneyVec()
{
	m_MoneyVec.clear();

	if (m_Receivers.size() >= m_Num)
	{
		return;
	}

	UInt8 remainNum = m_Num - (UInt8)m_Receivers.size();
	auto remainMoney = _GetRemainMoney();
	if (remainNum == 1)
	{
		m_MoneyVec.push_back(remainMoney);
		return;
	}

	UInt32 minMoney = 0;
	UInt32 maxMoney = 0;
	// 根据表格中的数据调整
	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(m_Reason);
	if (dataEntry)
	{
		if (minMoney < dataEntry->minMoney)
		{
			minMoney = dataEntry->minMoney;
		}

		if (maxMoney < dataEntry->maxMoney)
		{
			maxMoney = dataEntry->maxMoney;
		}
	}

	if (minMoney == 0 || maxMoney == 0 || minMoney >= maxMoney)
	{
		ErrorLogStream << " RedPacket minMoney maxMoney Error! minMoney = " << minMoney << ", maxMoney = " << maxMoney << LogStream::eos;
		return;
	}

	UInt32 needMoney = remainNum * minMoney;
	UInt32 randMoney = 0;
	for (UInt32 i = 0; i < remainNum; ++i)
	{
		if (remainMoney > needMoney)
		{
			if ((remainMoney - needMoney) > (maxMoney - minMoney))
			{
				randMoney = Avalon::Random::RandBetween(minMoney, maxMoney);
			}
			else
			{
				randMoney = Avalon::Random::RandBetween(minMoney, (remainMoney - needMoney + minMoney));
			}

			m_MoneyVec.push_back(randMoney);
			remainMoney -= randMoney;
		}
		else if (remainMoney == needMoney)
		{
			m_MoneyVec.push_back(minMoney);
			remainMoney -= minMoney;
		}
		else
		{
			ErrorLogStream << " RedPacket remainMoney needMoney Error! remainMoney = " << remainMoney << ", needMoney = " << needMoney << LogStream::eos;
			return;
		}

		needMoney -= minMoney;
	}

	if (remainMoney > 0)
	{
		m_MoneyVec[remainNum - 1] += remainMoney;
	}

	std::random_shuffle(m_MoneyVec.begin(), m_MoneyVec.end());
}

