#ifndef __GUILD_MEMBER_H__
#define __GUILD_MEMBER_H__

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLGuildDefine.h>
#include <CLObjectDefine.h>
#include <CLMaskProperty.h>

class WSPlayer;
struct RaceSkillInfo;
class GuildMember : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GuildMember, Avalon::NullMutex)
public:
	GuildMember();
	~GuildMember();

	bool Init(WSPlayer* player);

public:
	/*
		Get/Set接口
		*/

	/**
	*@brief 公会ID
	*/
	inline ObjID_t GetGuildID() const { return m_GuildId; }
	inline void SetGuildID(ObjID_t id) { m_GuildId = id; }

	/**
	*@brief 职务
	*/
	inline GuildPost GetPost() const { return (GuildPost)(UInt8)m_Post; }
	void SetPost(GuildPost post, bool updateTitle = true);

	/**
	*@brief 名字
	*/
	inline std::string GetName() { return m_Name; }
	inline void SetName(std::string name) { m_Name = name; }

	/**
	*@brief 职业
	*/
	inline UInt8 GetOccu() const { return m_Occu; }
	inline void SetOccu(UInt8 occu) { m_Occu = occu; }

	/**
	*@brief 等级
	*/
	inline UInt16 GetLevel() const { return m_Level; }
	inline void SetLevel(UInt16 level) { m_Level = level; }

	/**
	*@brief VIP等级
	*/
	inline UInt8 GetVipLevel() const { return m_VipLevel; }
	inline void SetVipLevel(UInt8 level) { m_VipLevel = level; }

	/**
	*@brief 累计贡献度
	*/
	inline UInt32 GetTotalContribution() const { return m_TotalContri; }
	void AddTotalContribution(UInt32 num);
	void SetTotalContribution(UInt32 contri) { m_TotalContri = contri; }

	/**
	*@brief 加入时间
	*/
	inline UInt32 GetEnterTime() const { return m_EnterTime; }
	inline void SetEnterTime(UInt32 time) { m_EnterTime = time; }

	/**
	*@brief 离线时间
	*/
	inline UInt32 GetOfflineTime() const { return m_OfflineTime; }
	inline void SetOfflineTime(UInt32 time) { m_OfflineTime = time; }

	/**
	*@brief 可加入公会时间
	*/
	inline UInt32 GetLeaveTime() const { return m_LeaveTime; }
	inline void SetLeaveTime(UInt32 time) { m_LeaveTime = time; }

	/**
	*@brief 玩家对象
	*/
	inline WSPlayer* GetPlayer() const { return m_Player; }
	inline void SetPlayer(WSPlayer* player) { m_Player = player; }

	/**
	*@brief 外观
	*/
	inline PlayerAvatar GetAvatar() const { return m_Avatar; }
	inline void SetAvatar(const PlayerAvatar& avatar) { m_Avatar = avatar; }

	/**
	*@brief 设置获取玩家标签信息
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info);

	/**
	*@brief 公会战连胜
	*/
	inline UInt32 GetGuildBattleWinStreak() const { return m_GuildBattleWinStreak; }
	inline void SetGuildBattleWinStreak(UInt32 winStreak) { m_GuildBattleWinStreak = winStreak; }
	inline void IncGuildBattleWinStreak() { m_GuildBattleWinStreak = m_GuildBattleWinStreak + 1; }

	/**
	*@brief 公会战积分
	*/
	inline UInt32 GetGuildBattleScore() const { return m_GuildBattleScore; }
	inline void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief 公会战次数
	*/
	inline UInt32 GetGuildBattleNumber() const { return m_GuildBattleNumber; }
	inline void SetGuildBattleNumber(UInt32 number) { m_GuildBattleNumber = number; }

	/**
	*@brief 公会战奖励情况
	*/
	inline std::string GetRewardMask() { return m_GuildBattleRewardMask.GetDBValue(); }

	/**
	*@brief 是否有权限
	*/
	inline bool HasPower(GuildPowerFlag power) const { return HasGuildPower(GetPost(), power); }

	/**
	*@brief 设置积分奖励
	*/
	inline bool CheckRewardMask(UInt32 boxId) const { return m_GuildBattleRewardMask.CheckMask(boxId); }
	inline void SetRewardMask(UInt32 boxId){ m_GuildBattleRewardMask.SetMask(boxId); }

	/**
	*@brief 设置公会战HP
	*/
	inline UInt32 GetGuildBattleHP() const { return m_GuildBattleHP; }
	inline void SetGuildBattleHP(UInt32 hp) { m_GuildBattleHP = hp; }

	/**
	*@brief 设置公会战MP
	*/
	inline UInt32 GetGuildBattleMP() const { return m_GuildBattleMP; }
	inline void SetGuildBattleMP(UInt32 mp) { m_GuildBattleMP = mp; }

	/**
	*@brief 设置公会战鼓舞
	*/
	inline UInt32 GetGuildBattleInspire() const { return m_GuildBattleInspire; }
	inline void IncGuildBattleInspire() { m_GuildBattleInspire = m_GuildBattleInspire + 1; }

	/**
	*@brief 设置公会战状态
	*/
	inline GuildBattleMemberStatus GetBattleStatus() const { return m_BattleStatus; }
	inline void SetGuildBattleStatus(GuildBattleMemberStatus status) { m_BattleStatus = status; }

	/**
	*@brief 设置公会成员抽奖状态
	*/
	inline UInt8 GetGuildBattleLotteryStatus() const{ return m_GuildBattleLotteryStatus; }
	inline void SetGuildBattleLotteryStatus(GuildBattleLotteryStatus status) { m_GuildBattleLotteryStatus = (UInt8)status; }

	/**
	*@brief 设置公会战领地每日奖励领取
	*/
	inline UInt32 GetGuildBattleTerrDayReward() const { return m_GuildTerrDayReward; }
	inline void SetGuildBattleTerrDayReward(UInt32 flag) { m_GuildTerrDayReward = flag; }

	/**
	*@brief 活跃度
	*/
	inline UInt32 GetActiveDegree() const { return m_ActiveDegree; }
	inline void SetActiveDegree(UInt32 value)
	{
		if (m_ActiveDegree == value) return;
		m_ActiveDegree = value;
		m_ActiveUpdateTime = (UInt32)CURRENT_TIME.Sec();
	}
	inline UInt32 GetActiveDegreeUpdateTime() const { return m_ActiveUpdateTime; }

	/**
	*@brief 设置段位
	*/
	inline UInt32 GetSeasonLevel() const { return m_SeasonLevel; }
	inline void SetSeasonLevel(UInt32 level) { m_SeasonLevel = level; }

	/**
	*@brief 设置徽记
	*/
	inline UInt32 GetEmblemLevel() const { return m_EmblemLevel; }
	inline void SetEmblemLevel(UInt32 level) { m_EmblemLevel = level; }

	/**
	*@brief 成员信息
	*/
	GuildMemberEntry GetEntry();

	/**
	*@brief 圆桌成员信息
	*/
	GuildTableMember GetTableMember(UInt8 seat, UInt8 type);

	/**
	*@brief 同步事件
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1);

	/**
	*@brief 是否在公会中
	*/
	bool IsInGuild() const;

	/**
	*@brief 是否能抽奖
	*/
	bool IsCanLottery() const;

public: //协议	
	void SendProtocol(Avalon::Protocol& protocol);

public:	//公会战相关

	//一场公会战结束
	void OnGuildBattleRaceEnd(GuildBattleMember& guildBattleMember, bool isWin, UInt32 terrId, UInt32 hp = HP_MP_FULL_VALUE, UInt32 mp = HP_MP_FULL_VALUE);

	//公会战开始
	void OnEnrollBegin();

	//设置鼓舞技能
	void SetInspireSkill(std::vector<RaceSkillInfo>& skills);

	//鼓舞
	UInt32 Inspire();

public:
	/* 数据库相关 */
	void SaveToDB();
	void InsertToDB();
	void DelFromDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("guildid", m_GuildId)
	CL_DBPROPERTY("post", m_Post)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY("occu", m_Occu)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("vip_level", m_VipLevel)
	CL_DBPROPERTY("totalcontri", m_TotalContri)
	CL_DBPROPERTY("entertime", m_EnterTime)
	CL_DBPROPERTY("offlinetime", m_OfflineTime)
	CL_DBPROPERTY("leavetime", m_LeaveTime)
	CL_DBPROPERTY("avatar", m_Avatar)
	CL_DBPROPERTY("battle_number", m_GuildBattleNumber)
	CL_DBPROPERTY("battle_score", m_GuildBattleScore)
	CL_DBPROPERTY("battle_winstreak", m_GuildBattleWinStreak)
	CL_DBPROPERTY("battle_reward_mask", m_GuildBattleRewardMask)
	CL_DBPROPERTY("battle_hp", m_GuildBattleHP)
	CL_DBPROPERTY("battle_mp", m_GuildBattleMP)
	CL_DBPROPERTY("battle_inspire", m_GuildBattleInspire)
	CL_DBPROPERTY("battle_lottery_status", m_GuildBattleLotteryStatus)
	CL_DBPROPERTY("active_degree", m_ActiveDegree);
	CL_DBPROPERTY("season_level", m_SeasonLevel);
	CL_DBPROPERTY("emblem_level", m_EmblemLevel);
	CL_DBPROPERTY("battle_terr_day_reward", m_GuildTerrDayReward);
	CL_DBPROPERTY_END()

private:
	//公会ID
	ObjUInt64	m_GuildId;
	//职位(对应枚举GuildPost)
	ObjUInt8	m_Post;
	//名字
	ObjString	m_Name;
	//职业
	ObjUInt8	m_Occu;
	//等级
	ObjUInt16	m_Level;
	//vip等级
	ObjUInt8	m_VipLevel;
	//累计贡献度
	ObjUInt32	m_TotalContri;
	//加入时间
	ObjUInt32	m_EnterTime;
	//离线时间
	ObjUInt32	m_OfflineTime;
	//离开公会时间
	ObjUInt32	m_LeaveTime;
	//活跃度
	ObjUInt32	m_ActiveDegree;
	//活跃度更新时间
	UInt32		m_ActiveUpdateTime;
	//外观
	ObjPlayerAvatar	m_Avatar;
	//玩家对象
	WSPlayer*	m_Player;

	//公会战相关
	//公会战次数
	ObjUInt32	m_GuildBattleNumber;
	//公会战积分
	ObjUInt32	m_GuildBattleScore;
	//公会战连胜
	ObjUInt32	m_GuildBattleWinStreak;
	//奖励领取情况
	CLMaskProperty<GUILD_BATTLE_REWARD_SIZE> m_GuildBattleRewardMask;
	//公会战剩余HP
	ObjUInt32	m_GuildBattleHP;
	//公会战剩余MP
	ObjUInt32	m_GuildBattleMP;
	//公会战鼓舞次数
	ObjUInt32	m_GuildBattleInspire;
	//公会战抽奖状态
	ObjUInt8	m_GuildBattleLotteryStatus;
	//公会战状态
	GuildBattleMemberStatus	m_BattleStatus;
	//段位
	ObjUInt32	m_SeasonLevel;
	//徽记
	ObjUInt32	m_EmblemLevel;
	//公会战领地每日奖励领取
	ObjUInt32	m_GuildTerrDayReward;
};


#endif //__GUILD_MEMBER_H__