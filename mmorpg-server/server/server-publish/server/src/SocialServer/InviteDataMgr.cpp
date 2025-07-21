#include "InviteDataMgr.h"

#include <AvalonObjectPool.h>
#include <CLSysProtocol.h>
#ifdef AVALON_OS_WINDOWS
#include "FakeMemcachedApi.h"
#else
#include <libmemcached/memcached.h>
#endif
#include "OnlineStateMgr.h"
#include "OSNetwork.h"


//openid最大长度
#define OPENID_MAX_LENGTH 34
//被邀请者最大列表
#define MAX_INVITEE_NUM	100

/**
 *@brief 获取一天开始时间
 */
inline UInt32 GetDayBeginTime(UInt32 secs)
{
	Avalon::Date date(Avalon::Time(Avalon::UInt64(secs) * 1000));
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);
	return UInt32(date.ToTime().Sec());
}


#pragma pack(1)

#define PLAYER_BASEDATA_SIZE ((char*)(&((PlayerData*)1)->inviteeNum) - (char*)1)

/**
 *@brief 玩家数据
 */
struct PlayerData
{
	PlayerData()
	{
		uid = 0;
		memset(openid, 0, sizeof(openid));
		memset(kl_openid, 0, sizeof(kl_openid));
		offlineTime		= 0;
		lastOnlineDay	= 0;
		level = 1;
		goldReturn = 0;
		onlineAwardTime = 0;
		memset(data, 0, sizeof(data));
		inviteeNum = 0;
	}

	bool IsInvitee(UInt32 accid) const
	{
		for(UInt32 i = 0; i < inviteeNum; ++i)
		{
			if(invitees[i] == accid) return true;
		}
		return false;
	}

	//玩家id
	UInt32	uid;
	//openid
	char	openid[OPENID_MAX_LENGTH];
	//恐龙openid
	char	kl_openid[OPENID_MAX_LENGTH];
	//离线时间
	UInt32	offlineTime;
	//昨天在线
	UInt32	lastOnlineDay;
	//等级UInt16 
	UInt16	level;
	//金贝
	UInt32	goldReturn;
	//登陆奖励领取时间
	UInt32	onlineAwardTime;
	//保留字段
	UInt8	data[32];
	//被邀请者人数, 放在最后
	UInt32	inviteeNum;
	//被邀请者列表
	UInt32	invitees[MAX_INVITEE_NUM];

	size_t GetDataLength() const{
		return PLAYER_BASEDATA_SIZE + sizeof(UInt32) * inviteeNum + sizeof(inviteeNum);
	}
};

#pragma pack()

InviteDataMgr::InviteDataMgr()
	:m_pMemc(NULL)
{
}

InviteDataMgr::~InviteDataMgr()
{
}

bool InviteDataMgr::Init(Avalon::PropertySet* properties)
{
	if(properties == NULL) return false;

	std::string db_ip1 = properties->GetProperty("Database", "invitedb_ip1").GetString();
	UInt32		db_port1 = properties->GetProperty("Database", "invitedb_port1");

	std::string db_ip2 = properties->GetProperty("Database", "invitedb_ip2").GetString();
	UInt32		db_port2 = properties->GetProperty("Database", "invitedb_port2");

	m_pMemc = memcached_create(NULL);
	if(m_pMemc == NULL)
	{
		ErrorLogStream << "InviteDataMgr::Init() create memcached_st failed!" << LogStream::eos;
		return false;
	}

	memcached_behavior_set(m_pMemc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);

	memcached_server_list_st servers;
	memcached_return_t result = MEMCACHED_SUCCESS;
	servers = memcached_server_list_append(NULL, db_ip1.c_str(), db_port1, &result);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "InviteDataMgr::Init() append server1 failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}

	servers = memcached_server_list_append(servers, db_ip2.c_str(), db_port2, &result);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "InviteDataMgr::Init() append server2 failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		memcached_server_list_free(servers);
		return false;
	}

	result = memcached_server_push(m_pMemc, servers);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "InviteDataMgr::Init() push servers failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		memcached_server_list_free(servers);
		return false;
	}

	memcached_server_list_free(servers);

	return true;
}

void InviteDataMgr::Final()
{
	memcached_free(m_pMemc);
	m_pMemc = NULL;
}

bool InviteDataMgr::GetInviteeList(UInt32 inviter, std::vector<CLProtocol::InviteeInfo>& list, UInt32& totalGold)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(inviter,pData)) return false;

	std::vector<PlayerData*> invitees;
	GetPlayerDataByUIDs(pData.invitees, pData.inviteeNum, invitees);

	UInt32 dayBeginTime = GetDayBeginTime(UInt32(CURRENT_TIME.Sec()));
	totalGold = 0;

	for(std::vector<PlayerData*>::iterator iter = invitees.begin();
		iter != invitees.end(); ++iter)
	{
		PlayerData* pInviteeData = *iter;
		CLProtocol::InviteeInfo info;
		GetInviteeData(*pInviteeData, dayBeginTime, info);
		list.push_back(info);

		totalGold += pInviteeData->goldReturn;
	}

	FreePlayerDatas(invitees);
	return true;
}

bool InviteDataMgr::CheckGiftbagCond(UInt32 accId, UInt8 type, UInt16 minLevel, UInt16 maxLevel, UInt32 playerNum)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(accId, pData)) return false;

	UInt32 num = 0;

	std::vector<PlayerData*> invitees;
	GetPlayerDataByUIDs(pData.invitees, pData.inviteeNum, invitees);

	if(type < 10)
	{
		UInt32 dayBeginTime = GetDayBeginTime(UInt32(CURRENT_TIME.Sec()));

		for(std::vector<PlayerData*>::iterator iter = invitees.begin();
			iter != invitees.end(); ++iter)
		{	
			PlayerData* pInviteeData = *iter;
			if(pInviteeData->level < minLevel || pInviteeData->level > maxLevel) continue;

			UInt32 onlineTime = OnlineStateMgr::Instance()->GetOnlineTime(pInviteeData->uid);
			if(pInviteeData->lastOnlineDay == dayBeginTime - DAY_TO_SEC 
				|| (onlineTime != 0 && onlineTime < dayBeginTime)
				|| (pData.offlineTime < dayBeginTime && pData.offlineTime >= dayBeginTime - DAY_TO_SEC))
			{
				++num;
			}
		}
	}
	else
	{
		for(std::vector<PlayerData*>::iterator iter = invitees.begin();
			iter != invitees.end(); ++iter)
		{	
			PlayerData* pInviteeData = *iter;
			if(pInviteeData->level < minLevel || pInviteeData->level > maxLevel) continue;
			++num;
		}
	}

	FreePlayerDatas(invitees);

	return num >= playerNum;
}

bool InviteDataMgr::CheckRecallAwardCond(UInt32 accId, UInt32 inviteeId, std::vector<CLProtocol::InviteeInfo>& list)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(accId, pData)) return false;

	if(inviteeId != 0)
	{
		if(!pData.IsInvitee(inviteeId)) return false;

		PlayerData inviteeData;
		if(!GetPlayerDataByUID(inviteeId, inviteeData)) return false;

		UInt32 dayBeginTime = GetDayBeginTime(UInt32(CURRENT_TIME.Sec()));
		if(inviteeData.onlineAwardTime >= dayBeginTime) return false; //已领过

		UInt32 onlineTime = OnlineStateMgr::Instance()->GetOnlineTime(inviteeData.uid);
		if(onlineTime == 0 && inviteeData.offlineTime < dayBeginTime) return false;

		inviteeData.onlineAwardTime = UInt32(CURRENT_TIME.Sec());
		SetPlayerDataByUID(inviteeId, inviteeData);

		CLProtocol::InviteeInfo info;
		GetInviteeData(inviteeData, dayBeginTime, info);
		list.push_back(info);
	}
	else
	{
		std::vector<PlayerData*> invitees;
		GetPlayerDataByUIDs(pData.invitees, pData.inviteeNum, invitees);

		UInt32 dayBeginTime = GetDayBeginTime(UInt32(CURRENT_TIME.Sec()));
		for(std::vector<PlayerData*>::iterator iter = invitees.begin();
			iter != invitees.end(); ++iter)
		{	
			PlayerData* pInviteeData = *iter;
			if(pInviteeData->onlineAwardTime >= dayBeginTime) continue;

			UInt32 onlineTime = OnlineStateMgr::Instance()->GetOnlineTime(pInviteeData->uid);
			if(onlineTime == 0 && pInviteeData->offlineTime < dayBeginTime) continue;

			pInviteeData->onlineAwardTime = UInt32(CURRENT_TIME.Sec());
			SetPlayerDataByUID(pInviteeData->uid, *pInviteeData);

			CLProtocol::InviteeInfo info;
			GetInviteeData(*pInviteeData, dayBeginTime, info);
			list.push_back(info);
		}

		FreePlayerDatas(invitees);
	}

	return true;
}

bool InviteDataMgr::CheckConsumeAward(UInt32 accId, UInt32 maxGold, UInt32& gold, UInt32& totalGold)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(accId, pData)) return false;

	std::vector<PlayerData*> invitees;
	GetPlayerDataByUIDs(pData.invitees, pData.inviteeNum, invitees);

	gold = 0;
	totalGold = 0;
	for(std::vector<PlayerData*>::iterator iter = invitees.begin();
		iter != invitees.end(); ++iter)
	{
		PlayerData* inviteeData = *iter;
		if(inviteeData->goldReturn == 0) continue;

		if(maxGold > inviteeData->goldReturn)
		{
			maxGold -= inviteeData->goldReturn;
			gold += inviteeData->goldReturn;
			inviteeData->goldReturn = 0;
			SetPlayerDataByUID(inviteeData->uid, *inviteeData);
		}
		else if(maxGold > 0)
		{
			gold += maxGold;
			inviteeData->goldReturn -= maxGold;
			maxGold = 0;
			SetPlayerDataByUID(inviteeData->uid, *inviteeData);
		}
		totalGold += inviteeData->goldReturn;
	}

	FreePlayerDatas(invitees);

	return true;
}

bool InviteDataMgr::BindKLPlayer(UInt32 accId, const std::string& openid, UInt16 level, const std::string& klopenid)
{
	PlayerData pData;
	if(GetPlayerDataByUID(accId, pData))
	{
		avalon_strncpy(pData.kl_openid, klopenid.c_str(), OPENID_MAX_LENGTH - 1);
		pData.level = level;
		SetPlayerDataByUID(accId, pData);
	}
	else
	{
		pData.uid = accId;
		avalon_strncpy(pData.openid, openid.c_str(), OPENID_MAX_LENGTH - 1);
		avalon_strncpy(pData.kl_openid, klopenid.c_str(), OPENID_MAX_LENGTH - 1);
		pData.offlineTime = 0;
		pData.level = level;
		AddPlayerDataByUID(accId, pData);
	}
	return true;
}

void InviteDataMgr::CheckKLActivity(UInt32 nodeId, UInt32 serverId, UInt32 accId, ObjID_t playerId)
{
	if(CURRENT_TIME.Sec() > 1355936400) return;

	PlayerData pData;
	if(!GetPlayerDataByUID(accId, pData)) return;

	if(pData.kl_openid[0] == 0) return;

	UInt8 flag = 0;
	if(pData.level >= 20) flag |= 1;
	if(pData.level >= 45) flag |= 2;
	if(pData.inviteeNum >= 2) flag |= 4;
	
	if(flag != 0)
	{
		CLProtocol::SocialInviteCheckKLActivityRet ret;
		ret.playerId = playerId;
		ret.finishFlag = flag;
		ret.klopenid = pData.kl_openid;
		OSNetwork::Instance()->TransmitProtocol(nodeId, serverId, ret);
	}
}

bool InviteDataMgr::GetPlayerDataByUID(UInt32 uid, PlayerData& data)
{
	size_t value_length = 0;
	memcached_return_t result;
	char* buf = memcached_get(m_pMemc, (const char*)&uid, sizeof(uid), &value_length, 0, &result);
	if(buf == NULL) return false;
	if(value_length > sizeof(PlayerData)) value_length = sizeof(PlayerData);

	memmove(&data, buf, value_length);
	free(buf);
	if(data.GetDataLength() != value_length)
	{
		ErrorLogStream << "error data length for player " << uid << " :" << UInt32(value_length) << LogStream::eos;
	}

	return true;
}

bool InviteDataMgr::GetPlayerDataByUIDs(const UInt32* ids, UInt32 idNum, std::vector<PlayerData*>& datas)
{
	if(idNum >= MAX_INVITEE_NUM) return false;
	if(idNum == 0) return false;
	const char* keys[MAX_INVITEE_NUM];
	size_t keylens[MAX_INVITEE_NUM];
	for(UInt32 i = 0; i < idNum; ++i)
	{
		keys[i] = (const char*)&ids[i];
		keylens[i] = sizeof(UInt32);
	}
	memcached_return_t result = memcached_mget(m_pMemc, keys, keylens, idNum);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "get players data failed: " << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}

	memcached_result_st* pResultsObj;
	while((pResultsObj = memcached_fetch_result(m_pMemc, NULL, &result)) != NULL)
	{
		const char* pData = memcached_result_value(pResultsObj);
		if(pData != NULL)
		{
			PlayerData* pPlayerData = new PlayerData(*(const PlayerData*)(pData));
			datas.push_back(pPlayerData);
		}
		memcached_result_free(pResultsObj);
	}

	return true;
}

bool InviteDataMgr::SetPlayerDataByUID(UInt32 uid, const PlayerData& pData)
{
	memcached_return_t result = memcached_set(m_pMemc, (const char*)&uid, sizeof(uid), (const char*)&pData, pData.GetDataLength(), 0, 0);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "set player " << uid << " data failed: " << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}
	return true;
}

bool InviteDataMgr::AddPlayerDataByUID(UInt32 uid, const PlayerData& pData)
{
	memcached_return_t result = memcached_add(m_pMemc, (const char*)&uid, sizeof(uid), (const char*)&pData, pData.GetDataLength(), 0, 0);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "add player " << uid << " data failed: " << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}
	return true;
}

void InviteDataMgr::FreePlayerDatas(std::vector<PlayerData*>& datas)
{
	for(std::vector<PlayerData*>::iterator iter = datas.begin();
		iter != datas.end(); ++iter)
	{
		delete *iter;
	}
	datas.clear();
}

void InviteDataMgr::GetInviteeData(const PlayerData& pData, UInt32 dayBeginTime, CLProtocol::InviteeInfo& info)
{
	info.uid = pData.uid;
	info.openid = pData.openid;
	UInt32 onlineTime = OnlineStateMgr::Instance()->GetOnlineTime(info.uid);
	info.bYesterdayOnline = 0;
	if(pData.lastOnlineDay == dayBeginTime - DAY_TO_SEC 
		|| (onlineTime != 0 && onlineTime < dayBeginTime)
		|| (pData.offlineTime < dayBeginTime && pData.offlineTime >= dayBeginTime - DAY_TO_SEC))
	{
		info.bYesterdayOnline = 1;
	}
	info.level = pData.level;
	info.offlineTime = onlineTime != 0 ? 0 : pData.offlineTime;
	info.onlineAwardTime = pData.onlineAwardTime;
}

void InviteDataMgr::SendProtocol(UInt32 connId, Avalon::Protocol& protocol)
{
	Avalon::NetConnectionPtr conn = OSNetwork::Instance()->FindConnection(connId);
	if( conn != NULL )
	{
		CL_SEND_PROTOCOL(conn, protocol);
	}
}

void InviteDataMgr::OnPlayerLevelup(UInt32 uid, UInt16 level)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(uid, pData)) return;
	
	if(level > pData.level)
	{
		pData.level = level;
		SetPlayerDataByUID(uid, pData);
	}
}

void InviteDataMgr::OnPlayerOnline(UInt32 uid)
{
}

void InviteDataMgr::OnPlayerOffline(UInt32 uid)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(uid, pData)) return;

	UInt32 dayBeginTime = GetDayBeginTime(UInt32(CURRENT_TIME.Sec()));
	UInt32 onlineTime = OnlineStateMgr::Instance()->GetOnlineTime(uid);
	if((onlineTime < dayBeginTime || 
		(pData.offlineTime < dayBeginTime && pData.offlineTime >= dayBeginTime - DAY_TO_SEC))
		&& pData.lastOnlineDay != dayBeginTime - DAY_TO_SEC)
	{
		pData.lastOnlineDay = dayBeginTime - DAY_TO_SEC;
	}
	pData.offlineTime = UInt32(CURRENT_TIME.Sec());

	SetPlayerDataByUID(uid, pData);
}

void InviteDataMgr::OnPlayerConsume(UInt32 uid, UInt32 gold)
{
	PlayerData pData;
	if(!GetPlayerDataByUID(uid, pData)) return;

	pData.goldReturn += gold;
	SetPlayerDataByUID(uid, pData);
}

void InviteDataMgr::OnInviteeEnterGame(UInt32 inviteeId, const std::string& openid, UInt32 inviterId)
{
	PlayerData pData;
	if(GetPlayerDataByUID(inviteeId, pData)) return; //已经存在

	PlayerData inviterData;
	bool bHasData = GetPlayerDataByUID(inviterId, inviterData);
	if(bHasData && inviterData.inviteeNum >= MAX_INVITEE_NUM)
	{
		return;
	}

	PlayerData inviteeData;
	inviteeData.uid = inviteeId;
	avalon_strncpy(inviteeData.openid, openid.c_str(), OPENID_MAX_LENGTH - 1);
	inviteeData.offlineTime = 0;
	inviteeData.level = 1;
	if(!AddPlayerDataByUID(inviteeId, inviteeData)) return;

	if(bHasData)
	{
		inviterData.invitees[inviterData.inviteeNum++] = inviteeId;
		SetPlayerDataByUID(inviterId, inviterData);
	}
	else
	{
		inviterData.uid = inviterId;
		inviterData.invitees[inviterData.inviteeNum++] = inviteeId;
		AddPlayerDataByUID(inviterId, inviterData);
	}
}

