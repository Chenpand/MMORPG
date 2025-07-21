#include "RedPacketMgr.h"
#include <CLRedPacketProtocol.h>
#include <CLRedPacketDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLGuildBattleDataEntry.h>
#include <CLGuildDungeonTimeDataEntry.h>

#include "WSPlayerMgr.h"
#include "CLRecordClient.h"
#include "GuildMgr.h"
#include "WSSelectInitDataCallback.h"
#include "WSPlayer.h"
#include "WSItemTransaction.h"
#include "WSActivityMgr.h"
#include "WSApplication.h"
#include "GameParamMgr.h"
#include "PlayerJoins.h"
#include "CLPlayerJoinEventDefine.h"


/**
*@brief 加载红包返回
*/
class SelectRedPacketCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return RedPacketMgr::Instance()->OnSelectRedPacketRet(this);
	}
};

/**
*@brief 加载红包领取者返回
*/
class SelectRedPacketReceiverCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return RedPacketMgr::Instance()->OnSelectRedPacketReceiverRet(this);
	}
};

/**
*@brief 自费红包
*/
class CustomRedPacketTransaction : public ItemTransaction
{
public:
	CustomRedPacketTransaction(WSPlayer *player, UInt16 reason, std::string name, UInt8 num)
		: ItemTransaction(player), m_Reason(reason), m_Name(name), m_Num(num) {}
	~CustomRedPacketTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto result = RedPacketMgr::Instance()->OnCheckCustomRedPacketRet(player, m_Reason, m_Name, m_Num);
		if (result == ErrorCode::SUCCESS)
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_RED_PACKET_CUSTOM, m_Reason, m_Num));
		}
		else
		{
			if (player)
			{
				player->OnSendRedPacketFailure(result);
			}
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		if (player)
		{
			player->OnSendRedPacketFailure(GetErrorCode());
		}
		UnLock();
	}

private:
	UInt16 m_Reason;
	std::string m_Name;
	UInt8 m_Num;
};

RedPacketMgr::RedPacketMgr()
{
	m_DailyTimer.SetTime(6);
}

RedPacketMgr::~RedPacketMgr()
{

}

bool RedPacketMgr::Init()
{
	CLSelectCommand* guild_command = CLRecordClient::Instance()->PrepareSelectCommand("t_red_packet");
	CLRecordClient::Instance()->SendCommand(guild_command, new SelectRedPacketCallback());

	return true;
}

bool RedPacketMgr::OnTick(const Avalon::Time& now)
{
	// 每天6点定时删除已经空的红包
	bool forceDelEmptyRedPacket = m_DailyTimer.IsTimeout(now);

	std::set<RedPacket*> needDestoryList;
	for (auto itr : m_RedPackets)
	{
		auto redPacket = itr.second;
		if (!redPacket)
		{
			continue;
		}

		redPacket->OnTick(now);

		if (redPacket->GetStatus() == RED_PACKET_STATUS_INIT &&
			now.Sec() > redPacket->GetStatusTime() + RED_PACKET_INIT_TIME_OUT)
		{
			// 超过24小时，自动发红包
			if (OnPlayerSendRedPacket(redPacket->GetOwner(), redPacket->GetID(), 0) != ErrorCode::SUCCESS)
			{
				// 发送红包失败，直接删除红包
				needDestoryList.insert(redPacket);
			}
		}
		else if (redPacket->GetStatus() == RED_PACKET_STATUS_WAIT_RECEIVE || 
			redPacket->GetStatus() == RED_PACKET_STATUS_EMPTY)
		{
			if (now.Sec() > redPacket->GetStatusTime() + RED_PACKET_WAIT_RECEIVE_TIME ||
				(redPacket->GetStatus() == RED_PACKET_STATUS_EMPTY && forceDelEmptyRedPacket))
			{
				needDestoryList.insert(redPacket);
			}
		}
		else if (redPacket->GetStatus() == RED_PACKET_STATUS_DESTORY)
		{
			needDestoryList.insert(redPacket);
		}
	}

	CLProtocol::WorldNotifyDelRedPacket notify;
	for (auto redPacket : needDestoryList)
	{
		// 如果是在领取状态被删除的红包，通知客户端删除该红包
		if (redPacket->GetStatus() == RED_PACKET_STATUS_WAIT_RECEIVE || redPacket->GetStatus() == RED_PACKET_STATUS_EMPTY)
		{
			notify.redPacketList.push_back(redPacket->GetID());
		}
		
		_DelRedPacket(redPacket);
	}

	if (!notify.redPacketList.empty())
	{
		Router::BroadcastToPlayer(notify);
	}

	return true;
}

void RedPacketMgr::Final()
{
	for (auto itr : m_RedPackets)
	{
		auto redPacket = itr.second;
		if (!redPacket)
		{
			continue;
		}

		redPacket->SaveToDB();
	}
}

RedPacket* RedPacketMgr::FindRedPacketByID(ObjID_t id) const
{
	auto itr = m_RedPackets.find(id);
	return itr == m_RedPackets.end() ? NULL : itr->second;
}

void RedPacketMgr::DelRedPacketByID(ObjID_t id)
{
	auto redPacket = FindRedPacketByID(id);
	_DelRedPacket(redPacket);
}

void RedPacketMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldSyncRedPacket sync;
	CLProtocol::WorldSyncRedPacketRecord syncReceiveRecords;
	for (auto itr : m_RedPackets)
	{
		auto redPacket = itr.second;
		if (_IsPlayerSubscribe(player, redPacket))
		{
			sync.entrys.push_back(redPacket->GetBaseEntry(player->GetID(), player->GetAccId()));
			RedPacketReceiver* receiver = redPacket->FindReceiver(player->GetID());
			if (receiver)
			{
				syncReceiveRecords.adds.push_back(receiver->ToClient());
		}
	}
	}

	player->SendProtocol(sync);

	player->SendProtocol(syncReceiveRecords);

	DebugLogStream << "player(" << player->GetID() << ") has red packet(" << (UInt8)sync.entrys.size() << ")." << LogStream::eos;
}

void RedPacketMgr::OnPlayerChangeGuild(ObjID_t roleId, ObjID_t oldGuildId, ObjID_t newGuildId)
{
	/*
	1. 已经发到原公会的红包还是在原公会
	2. 原公会别人发的红包需要通知玩家删除
	3. 新公会别人发的红包需要通知玩家获得
	*/

	// 原公会别人发的红包，需要通知自己删除
	CLProtocol::WorldNotifyDelRedPacket notifyDel;
	// 新公会已有的红包，需要发给自己
	CLProtocol::WorldNotifyNewRedPacket notifyNew;

	UInt32 accid = 0;
	auto player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (player)
	{
		accid = player->GetAccId();
	}

	for (auto itr : m_RedPackets)
	{
		auto redPacket = itr.second;
		if (!redPacket)
		{
			continue;
		}

		if (redPacket->GetType() != RED_PACKET_TYPE_GUILD || redPacket->GetOwner() != roleId)
		{
			continue;
		}

		if (redPacket->GetStatus() != RED_PACKET_STATUS_INIT &&
			redPacket->GetCond1() == oldGuildId)
		{
			// 通知把原来的公会红包删掉
			notifyDel.redPacketList.push_back(redPacket->GetID());
		}
		else if (redPacket->GetStatus() != RED_PACKET_STATUS_INIT &&
			redPacket->GetCond1() == newGuildId)
		{
			// 通知把新公会的红包
			notifyNew.entrys.push_back(redPacket->GetBaseEntry(roleId, accid));
		}
	}

	if (player)
	{
		if (!notifyDel.redPacketList.empty())
		{
			player->SendProtocol(notifyDel);
		}

		if (!notifyNew.entrys.empty())
		{
			player->SendProtocol(notifyNew);
		}
	}
}

UInt32 RedPacketMgr::OnPlayerSendCustomRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num)
{
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to send custom packet, reason(" << reason << ") num(" << num << ")." << LogStream::eos;

	if (!WSApplication::Instance()->IsOpenRedPacket())
	{
		return ErrorCode::RED_PACKET_NOT_OPEN;
	}

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (!dataEntry || dataEntry->subType != RPST_BUY ||
		(dataEntry->type != RED_PACKET_TYPE_NEW_YEAR && dataEntry->type != RED_PACKET_TYPE_GUILD))
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (dataEntry->type == RED_PACKET_TYPE_GUILD)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (!guild)
		{
			return ErrorCode::RED_PACKET_NOT_IN_GUILD;
		}

		return guild->OnRequestPayRedPacket(player, reason, name, num);
	}
	else if (dataEntry->type != RED_PACKET_TYPE_NEW_YEAR)
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (!dataEntry->IsValidNum(num))
	{
		return ErrorCode::RED_PACKET_INVALID_NUM;
	}

	if (dataEntry->type == RED_PACKET_TYPE_NEW_YEAR)
	{
		UInt32 activityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_NEW_YEAR_RED_PACKET_ACTIVITY_ID);
		if (!WSActivityMgr::Instance()->IsInActivity(player->GetLevel(), activityId))
		{
			return ErrorCode::RED_PACKET_NOT_OPEN;
		}
	}

	auto trans = new CustomRedPacketTransaction(player, reason, name, num);
	trans->SetItem(dataEntry->costMoneyId, dataEntry->totalMoney);
	if (!player->BeginItemTransaction(trans))
	{
		return ErrorCode::GUILD_SYS_ERROR;
	}

	return ErrorCode::SUCCESS;
}

UInt32 RedPacketMgr::OnPlayerAddRedPacket(WSPlayer* player, UInt16 reason, std::string name, ObjID_t& redPacketId)
{
	redPacketId = 0;
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	DebugLogStream << "player(" << player->GetID() << ") watnt to add red pacekt reason(" << reason << ") name(" << name << ")." << LogStream::eos;

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (!reason)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to add unexist red packet(" << reason << ")." << LogStream::eos;
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (name.empty())
	{
		name = dataEntry->desc;
	}
	else
	{
		// 检测红包名是否合法
		if (!GuildMgr::Instance()->IsStrValid(name, RED_PACKET_NAME_MAX_WIDTH))
		{
			return ErrorCode::RED_PACKET_INVALID_NAME;
		}
	}

	UInt32 money = dataEntry->totalMoney;
	auto redPacket = new RedPacket(player, (RedPacketType)dataEntry->type, reason, money);
	if (!redPacket)
	{
		ErrorLogStream << "player(" << player->GetID() << ") create RedPacket instance failed." << LogStream::eos;
		return ErrorCode::RED_PACKET_INVALID;
	}
	redPacket->SetID(CLRecordClient::Instance()->GenGuid());
	redPacket->SetName(name);
	if (dataEntry->type == RED_PACKET_TYPE_GUILD)
	{
		redPacket->SetCond1(player->GetGuildID());
		if (dataEntry->thrType >= RPT_THR_TYPE_GUILD_BATTLE && dataEntry->thrType <= RPT_THR_TYPE_GUILD_DUNGEON)
		{
			UInt64 battle_time = GetGuildBattleLastTime(dataEntry->thrType);
			redPacket->SetCond2(battle_time);
	}
	}

	// 保存到数据库
	redPacket->InsertToDB();

	// 通知客户端有新的红包
	//CLProtocol::WorldNotifyGotNewRedPacket notify;
	//notify.entry = redPacket->GetBaseEntry(player->GetID());
	//player->SendProtocol(notify);

	m_RedPackets[redPacket->GetID()] = redPacket;

	redPacketId = redPacket->GetID();
	return ErrorCode::SUCCESS;
}

UInt32 RedPacketMgr::OnPlayerSendRedPacket(ObjID_t roleId, ObjID_t redPacketId, UInt8 num)
{
	auto redPacket = FindRedPacketByID(redPacketId);
	if (!redPacket)
	{
		return ErrorCode::RED_PACKET_NOT_EXIST;
	}

	if (redPacket->GetOwner() != roleId)
	{
		return ErrorCode::RED_PACKET_NOT_OWNER;
	}

	if (redPacket->GetStatus() != RED_PACKET_STATUS_INIT)
	{
		return ErrorCode::RED_PACKET_ALREADY_SEND;
	}

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(redPacket->GetReason());
	if (!dataEntry || dataEntry->nums.empty())
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (num == 0)
	{
		// 如果是0，就用第一个数量
		num = dataEntry->nums[0];
	}
	else if (std::find(dataEntry->nums.begin(), dataEntry->nums.end(), num) == dataEntry->nums.end())
	{
		ErrorLogStream << "player(" << roleId << ") want to add red packet(" << redPacket->GetReason() << ") invalid num(" << num << ")." << LogStream::eos;
		return ErrorCode::RED_PACKET_INVALID_NUM;
	}

	if (redPacket->GetType() == RED_PACKET_TYPE_GUILD)
	{
		ObjID_t guildId = redPacket->GetCond1();
		auto guild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (!guild)
		{
			InfoLogStream << "player(" << roleId << ") send guild redpacket(" << redPacketId << ") but not in guild." << LogStream::eos;
			return ErrorCode::RED_PACKET_NOT_IN_GUILD;
		}

		redPacket->SetNum(num);
		redPacket->ChangeStatus(RED_PACKET_STATUS_WAIT_RECEIVE);



		// 广播新红包给公会成员
		CLProtocol::WorldNotifyNewRedPacket notify;
		notify.entrys.push_back(redPacket->GetBaseEntry(0, 0));
		//guild->Broadcast(notify);
		guild->Broadcast(notify, 
			[redPacket, this](WSPlayer* player, Avalon::Protocol* protocol){
			if (!player || !protocol)	return;
			CLProtocol::WorldNotifyNewRedPacket* proto = dynamic_cast<CLProtocol::WorldNotifyNewRedPacket*> (protocol);
			if (!proto || proto->entrys.size() < 1)
			{
				return;
	}
			proto->entrys[0].status = redPacket->GetGuildRedPacketStatus(player->GetID(), player->GetAccId());
			return;
		});
	}
	else if (redPacket->GetType() == RED_PACKET_TYPE_NEW_YEAR)
	{
		// 全服红包
		redPacket->SetNum(num);
		redPacket->ChangeStatus(RED_PACKET_STATUS_WAIT_RECEIVE);

		// 广播新红包所有玩家
		CLProtocol::WorldNotifyNewRedPacket notify;
		notify.entrys.push_back(redPacket->GetBaseEntry(0, 0));
		Router::BroadcastToPlayer(notify);
	}
	else
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	
	return ErrorCode::SUCCESS;
}

bool RedPacketMgr::OnPlayerOpenRedPacket(WSPlayer* player, ObjID_t redPacketId)
{
	if (!player)
	{
		return false;
	}

	auto redPacket = FindRedPacketByID(redPacketId);
	if (!redPacket)
	{
		CLProtocol::WorldOpenRedPacketRes res;
		res.result = ErrorCode::RED_PACKET_NOT_EXIST;
		player->SendProtocol(res);
		return false;
	}

	if (player && redPacket->OnPlayerOpen(player) == ErrorCode::RED_PACKET_SYS_ERROR)
	{
		ErrorLogStream << "player(" << player->GetID() << ") redpacket(" << redPacket->GetID() << ") system error, delete it." << LogStream::eos;

		CLProtocol::WorldNotifyDelRedPacket notify;
		notify.redPacketList.push_back(redPacket->GetID());
		Router::BroadcastToPlayer(notify);

		// 系统错误,把这个红包删了
		_DelRedPacket(redPacket);
	}
	return true;
}

UInt32 RedPacketMgr::OnCheckCustomRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num)
{
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (!dataEntry)
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (!dataEntry->IsValidNum(num))
	{
		return ErrorCode::RED_PACKET_INVALID_NUM;
	}

	// 添加红包
	ObjID_t redPacketId = 0;
	auto result = RedPacketMgr::Instance()->OnPlayerAddRedPacket(player, reason, name, redPacketId);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	// 发送红包
	result = RedPacketMgr::Instance()->OnPlayerSendRedPacket(player->GetID(), redPacketId, num);
	if (result != ErrorCode::SUCCESS)
	{
		RedPacketMgr::Instance()->DelRedPacketByID(redPacketId);
		return result;
	}

	InfoLogStream << "player(" << player->GetID() << ") send custom red packet, reason(" << reason << ") num(" << num << ")." << LogStream::eos;

	player->OnSendRedPacketSuccess(redPacketId, reason, num, name);
	return ErrorCode::SUCCESS;
}

bool RedPacketMgr::OnSelectRedPacketRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		auto redPacket = new RedPacket();
		redPacket->SetID(callback->GetKey());
		redPacket->SetDBData(callback);
		redPacket->SetName(callback->GetData("name").GetString());

		auto data = RedPacketDataEntryMgr::Instance()->FindEntry(redPacket->GetReason());
		if (!data)
		{
			delete redPacket;
			continue;
		}

		_AddRedPacket(redPacket);
	}

	// 加载成员信息
	CLSelectCommand* member_command = CLRecordClient::Instance()->PrepareSelectCommand("t_red_packet_receiver");
	CLRecordClient::Instance()->SendCommand(member_command, new SelectRedPacketReceiverCallback());

	return true;
}

bool RedPacketMgr::OnSelectRedPacketReceiverRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		auto receiver = new RedPacketReceiver();
		receiver->SetID(callback->GetKey());
		receiver->SetDBData(callback);
		receiver->SetName(callback->GetData("name").GetString());

		auto redPacket = FindRedPacketByID(receiver->GetRedPacketID());
		if (!redPacket)
		{
			ErrorLogStream << "red packet receiver(" << receiver->GetID() << ") can't find red packet(" << receiver->GetRedPacketID() << ")." << LogStream::eos;
			CL_SAFE_DELETE(receiver);
			continue;
		}

		redPacket->AddReceiver(receiver);
	}

	return true;
}

bool RedPacketMgr::_IsPlayerSubscribe(WSPlayer* player, RedPacket* redPacket)
{
	if (!player || !redPacket)
	{
		return false;
	}

	if (redPacket->GetType() == RED_PACKET_TYPE_NEW_YEAR)
	{
		// 广播红包只有当红包是自己可以领的情况下需要发送
		if (redPacket->GetStatus() == RED_PACKET_STATUS_WAIT_RECEIVE && !redPacket->IsPlayerAlreadyOpened(player->GetID()))
		{
			return true;
		}
		return false;
	}
	else if (redPacket->GetType() == RED_PACKET_TYPE_GUILD)
	{
		//if (redPacket->IsAccountAlreadyOpend(player->GetAccId()))
		//{
		//	return false;
		//}

		// 属于自己的未发红包
		if (redPacket->GetStatus() == RED_PACKET_STATUS_INIT && redPacket->GetOwner() == player->GetID())
		{
			return true;
		}
		else if (redPacket->GetStatus() != RED_PACKET_STATUS_INIT && 
				player->GetGuildID() == redPacket->GetCond1())
		{
			return true;
		}
	} 
	else if (redPacket->GetOwner() == player->GetID())
	{
		return true;
	}

	return false;
}

void RedPacketMgr::_AddRedPacket(RedPacket* redPacket)
{
	if (!redPacket)
	{
		return;
	}

	m_RedPackets[redPacket->GetID()] = redPacket;
}

UInt32 RedPacketMgr::GetGuildBattleLastTime(UInt8 type)
{
	if (type == RPT_THR_TYPE_GUILD_BATTLE)
	{
		return GuildBattleTimeDataEntryMgr::Instance()->GetLastGuildBattleTim(CURRENT_TIME, GBT_NORMAL);
	}
	else if (type == RPT_THR_TYPE_GUILD_CROSS_BATTLE)
	{
		return GuildBattleTimeDataEntryMgr::Instance()->GetLastGuildBattleTim(CURRENT_TIME, GBT_CROSS);
	}
	else if (type == RPT_THR_TYPE_GUILD_DUNGEON)
	{
		return GuildDungeonTimeDataEntryMgr::Instance()->GetLastBattleTim(CURRENT_TIME);
	}
	else
	{
		return 0;
	}
}

UInt8 RedPacketMgr::RedPackThreTpCoverToPlayerJoinTp(RedPacketThreeType type)
{
	if (type == RPT_THR_TYPE_GUILD_BATTLE)
	{
		return PJET_THR_TYPE_GUILD_BATTLE;
	}
	else if (type == RPT_THR_TYPE_GUILD_CROSS_BATTLE)
	{
		return PJET_THR_TYPE_GUILD_CROSS_BATTLE;
	}
	else if (type == RPT_THR_TYPE_GUILD_DUNGEON)
	{
		return PJET_THR_TYPE_GUILD_DUNGEON;
	}
	else
	{
		return PJET_TYPE_GUILD_NONE;
	}
}

UInt32 RedPacketMgr::GetGuildRedPacketInfo(WSPlayer* player, std::vector<GuildRedPacketSpecInfo>& infos)
{
	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		return ErrorCode::RED_PACKET_NOT_IN_GUILD;
	}
	GuildRedPacketSpecInfo info;
	info.type = RPT_THR_TYPE_GUILD_ALL;
	info.lastTime = 0;
	info.joinNum = guild->GetMemberNum();
	infos.push_back(info);

	for (UInt8 i = RPT_THR_TYPE_GUILD_BATTLE; i < RPT_THR_TYPE_GUIL_MAX; ++i)
	{
		GuildRedPacketSpecInfo info;
		info.type = i;
		UInt8 joinType = RedPackThreTpCoverToPlayerJoinTp((RedPacketThreeType)i);
		UInt32 lastTime = GetGuildBattleLastTime((RedPacketThreeType)i);
		info.lastTime = lastTime;
		info.joinNum = PlayerJoinMgr::Instance()->GetPlayerJoinNum(joinType, info.lastTime, player->GetGuildID());
		infos.push_back(info);
	}

	return ErrorCode::SUCCESS;
}

void RedPacketMgr::_DelRedPacket(RedPacket* redPacket)
{
	if (!redPacket)
	{
		return;
	}

	m_RedPackets.erase(redPacket->GetID());

	redPacket->DelFromDB();
	delete redPacket;
}

bool  RedPacketMgr::IsCanReciveGuildRedPacket(RedPacket& redPacket, ObjID_t playerId)
{
	

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(redPacket.GetReason());
	if (!dataEntry)
	{
		return false;
	}
	if (dataEntry->thrType == RPT_THR_TYPE_GUILD_ALL)
	{
		return true;
	}
	UInt8 type = RedPackThreTpCoverToPlayerJoinTp((RedPacketThreeType)dataEntry->thrType);
	UInt64	guildId = redPacket.GetCond1();
	UInt64	battleTm = redPacket.GetCond2();

	return PlayerJoinMgr::Instance()->IsPlayerJoinEvent(playerId, type, battleTm, guildId);
}