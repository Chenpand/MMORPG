#ifndef _TCS_PLAYER_H_
#define _TCS_PLAYER_H_

#include <CLObject.h>
#include <CLProtocolDefine.h>
#include <CLItemDefine.h>
#include <kingnet_analyzer.h>
#include <udplog_typedef.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#define PLAYERINFO(T) \
	"accid(" << T->GetAccID() << ") playerid(" << T->GetID() << ") name(" << T->GetName() << ") zoneid(" << T->GetZoneID() << ")"

struct TeamCopyInviteInfo;

// 翻牌key
struct FlopKey
{
	FlopKey() : grade(0), stage(0) {}
	FlopKey(UInt32 _grade, UInt32 _stage) : grade(_grade), stage(_stage) {}
	UInt32 grade;
	UInt32 stage;

	bool operator < (const FlopKey& key) const
	{
		if (grade != key.grade)
		{
			return grade < key.grade;
		}
		else
		{
			return stage < key.stage;
		}

		return true;
	}
};

struct TCSPlayerData
{
	TCSPlayerData() : dayQuitTeamNum(0), weekQuitTeamNum(0) {}

	// 日退队次数
	UInt32 dayQuitTeamNum;
	// 周退队次数
	UInt32 weekQuitTeamNum;

	// 日次数(key->难度， val->次数)
	std::map<UInt32, UInt32> dayNumMap;
	// 周次数(key->难度， val->次数)
	std::map<UInt32, UInt32> weekNumMap;

	// 模式次数(key->model, val->次数)
	std::map<UInt32, UInt32> modelNumMap;

	// 额外次数(key->model, val->次数)
	std::map<UInt32, UInt32> extraNumMap;

	// roll次数(key->model, val->次数)
	std::map<UInt32, UInt32> rollNumMap;

	// 日翻牌(key->阶段，val->翻牌次数)
	std::map<FlopKey, UInt32> dayFlopMap;
	// 周翻牌(key->阶段，val->翻牌次数)
	std::map<FlopKey, UInt32> weekFlopMap;
};

namespace CLProtocol {
	class TeamCopyPlayerInfoSync;
	class TeamCopyPlayerInfoNotify;
};

/**
*@brief 团本玩家
*/
class TCSPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(TCSPlayer, Avalon::NullMutex)
public:

	TCSPlayer();
	~TCSPlayer();

	/**
	*@brief 团本类型
	*/
	inline void SetTeamType(UInt32 type) { m_TeamType = type; }
	inline UInt32 GetTeamType() { return m_TeamType; }

	/**
	*@brief zoneId
	*/
	inline void SetZoneID(UInt32 id) { m_ZoneId = id; }
	inline UInt32 GetZoneID() { return m_ZoneId; }
	
	/**
	*@brief 账号
	*/
	inline void SetAccID(UInt32 id) { m_AccId = id; }
	inline UInt32 GetAccID() { return m_AccId; }

	/**
	*@brief 职业
	*/
	inline void SetOccu(UInt32 occu) { m_Occu = occu; }
	inline UInt32 GetOccu() { return m_Occu; }

	/**
	*@brief 等级
	*/
	inline void SetLevel(UInt32 lvl) { m_Level = lvl; }
	inline UInt32 GetLevel() { return m_Level; }

	/**
	*@brief vip等级
	*/
	inline void SetVipLevel(UInt32 lvl) { m_VipLvl = lvl; }
	inline UInt32 GetVipLevel() { return m_VipLvl; }

	/**
	*@brief 装备评分
	*/
	inline void SetEquipScore(UInt32 score) { m_EquipScore = score; }
	inline UInt32 GetEquipScore() { return m_EquipScore; }

	/**
	*@brief 连接Id
	*/
	inline void SetConnId(UInt32 id) { m_ConnId = id; }
	inline UInt32 GetConnId() { return m_ConnId; }

	/**
	*@brief 团本Id
	*/
	inline void SetCopyId(UInt32 id) { m_CopyId = id; }
	inline UInt32 GetCopyId() { return m_CopyId; }

	/**
	*@brief 当前场景类型
	*/
	inline void SetSceneType(UInt32 type) { m_SceneType = type; }
	inline UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief 当前场景子类型
	*/
	inline void SetSceneSubType(UInt32 type) { m_SceneSubType = type; }
	inline UInt32 GetSceneSubType() { return m_SceneSubType; }

	/**
	*@brief 原scene
	*/
	inline void SetSceneNodeId(UInt32 id) { m_SceneNodeId = id; }
	inline UInt32 GetSceneNodeId() { return m_SceneNodeId; }

	/**
	*@brief 原world
	*/
	inline void SetWorldNodeId(UInt32 id) { m_WorldNodeId = id; }
	inline UInt32 GetWorldNodeId() { return m_WorldNodeId; }

	/**
	*@brief 公会id
	*/
	inline void SetGuildId(UInt64 id) { m_GuildId = id; }
	inline UInt64 GetGuildId() { return m_GuildId; }

	/**
	*@brief 觉醒
	*/
	inline void SetAwaken(UInt32 awaken) { m_Awaken = awaken; }
	inline UInt32 GetAwaken() { return m_Awaken; }
	
	/**
	*@brief 退队次数
	*/
	UInt32 GetDayQuitTeamNum(UInt32 teamType);
	UInt32 GetWeekQuitTeamNum(UInt32 teamType);
	void AddQuitTeamNum(UInt32 teamType);

	/**
	*@brief 是否剩余免费退队次数
	*/
	bool IsOddFreeQuitTeamNum(UInt32 teamType);

	/**
	*@brief 清除挑战次数
	*/
	void GMClearChallengeNum();

	/**
	*@brief GM开启金团资格
	*/
	void GMOpenGoldTeam();

	/**
	*@brief GM开启噩梦资格
	*/
	void GMOpenDiffGrade();

	/**
	*@brief 门票是否足够
	*/
	void SetTicketIsEnough(UInt32 teamType, UInt32 flag);
	UInt32 GetTicketIsEnough(UInt32 teamType);

	/**
	*@brief 是否loading
	*/
	inline void SetIsLoading(bool flag) { m_IsLoading = flag; }
	inline bool GetIsLoading() { return m_IsLoading; }

	/**
	*@brief 模式次数
	*/
	void AddModelNum(UInt32 teamType, UInt32 model, UInt32 num);
	void SetModelNum(UInt32 teamType, UInt32 model, UInt32 num);
	UInt32 GetModelNum(UInt32 teamType, UInt32 model);
	UInt32 GetAllModelNum(UInt32 teamType);

	/**
	*@brief 难度日次数
	*/
	void AddDayNum(UInt32 teamType, UInt32 grade, Int32 num);
	void SetDayNum(UInt32 teamType, UInt32 grade, UInt32 num);
	UInt32 GetDayNum(UInt32 teamType, UInt32 grade);

	/**
	*@brief 难度周次数
	*/
	void AddWeekNum(UInt32 teamType, UInt32 grade, Int32 num);
	void SetWeekNum(UInt32 teamType, UInt32 grade, UInt32 num);
	UInt32 GetWeekNum(UInt32 teamType, UInt32 grade);

	/**
	*@brief 获取总次数
	*/
	UInt32 GetCfgTotalNum(UInt32 teamType, UInt32 teamGrade, bool isDay);

	/**
	*@brief 是否有挑战次数
	*/
	bool IsHasChallengeNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 增加挑战次数
	*/
	void AddChallengeNum(UInt32 teamType, UInt32 teamGrade);
	void ReturnChallengeNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 获取额外次数
	*/
	UInt32 GetExtraNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 获取roll次数
	*/
	void AddRollNum(UInt32 teamType, UInt32 teamGrade);
	UInt32 GetRollNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 是否可以创建金团
	*/
	bool IsCreateGoldTeam(UInt32 teamType);

	/**
	*@brief 翻牌次数
	*/
	void AddFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage, UInt32 crossDateType);
	UInt32 GetDayFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage);
	UInt32 GetWeekFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage);

	/**
	*@brief 开局计算玩家不可翻牌的阶段
	*/
	void CalNotFlopStage(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 是否可以翻牌
	*/
	UInt32 IsCanFlop(UInt32 stage);

	/**
	*@brief 计算不可翻牌阶段
	*/
	bool IsUnlockGrade(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief 同步数据到Scene
	*/
	void PlayerInfoToScene(UInt32 teamType);

	/**
	*@brief 下发玩家团本数据
	*/
	void PlayerInfoToClient(UInt32 teamType);

	/**
	*@brief 下发玩家团本1数据
	*/
	void GetTeamCopyOneInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify);

	/**
	*@brief 下发玩家团本2数据
	*/
	void GetTeamCopyTwoInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify);

	/**
	*@brief 加奖励
	*/
	void AddRewards(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify = true);

	/**
	*@brief 加入邀请
	*/
	void AddInvite(const TeamCopyInviteInfo& inviteInfo);

	/**
	*@brief 删除邀请
	*/
	bool DelInviteInfo(UInt32 teamId);

	/**
	*@brief 下发邀请列表
	*/
	void SendInviteList();

	/**
	*@brief 发送到客户端
	*/
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief 发送到Scene
	*/
	void SendToScene(Avalon::Packet* packet);
	void SendToScene(Avalon::Protocol& protocol);

	/**
	*@brief 设置数据
	*/
	void SetPlayerData(CLProtocol::TeamCopyPlayerInfoSync& protocol);

	/**
	*@brief 发送通知
	*/
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		SendProtocol(notify);
	}

	/**
*@brief 发送udplog
*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_TEAM_COPY_UID);
		if (NULL == pLogger) return;

		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#endif

		pLogger->SetUserIP("");
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);
	}

private:

	/**
	*brief 字符串操作
	*/
	void _EncodeString(std::map<UInt32, UInt32>& strMap, const std::string& str);
	std::string _DecodeString(const std::map<UInt32, UInt32>& strMap);

	/**
	*brief 翻牌字符串操作
	*/
	void _EncodeFlopString(std::map<FlopKey, UInt32>& strMap, const std::string& str);
	std::string _DecodeFlopString(const std::map<FlopKey, UInt32>& strMap);

private:

	// 所在的团本
	UInt32 m_TeamType;
	// zoneId
	UInt32 m_ZoneId;
	// 账号Id
	UInt32 m_AccId;
	// 职业
	UInt32 m_Occu;
	// 等级
	UInt32 m_Level;
	// vip
	UInt32 m_VipLvl;
	// 装备评分
	UInt32 m_EquipScore;
	// 连接Id
	UInt32 m_ConnId;
	// 团本id
	UInt32 m_CopyId;
	// 当前场景类型
	UInt32 m_SceneType;
	// 当前场景子类型
	UInt32 m_SceneSubType;
	// 原scene
	UInt32 m_SceneNodeId;
	// 原world
	UInt32 m_WorldNodeId;
	// 公会id
	UInt64 m_GuildId;
	// 觉醒
	UInt32 m_Awaken;
	// 是否loading
	bool m_IsLoading;

	// 门票是否足够
	std::map<UInt32, UInt32> m_TicketIsEnoughMap;

	std::map<UInt32, TCSPlayerData> m_TCDataMap;

	// 不可翻牌阶段(进战斗之前算好)(key->阶段, val->不可翻牌原因)
	std::map<UInt32, UInt32> m_NotFlopMap;

	// 邀请信息
	std::map<UInt32, TeamCopyInviteInfo> m_InviteMap;
};

#endif
