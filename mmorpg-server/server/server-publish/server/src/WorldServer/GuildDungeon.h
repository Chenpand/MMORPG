#ifndef _GUILD_DUNGEON_H_
#define _GUILD_DUNGEON_H_

#include <set>
#include <CLDefine.h>
#include <CLGuildDefine.h>

class Guild;

/**
 *@brief 公会的地下城
 */
class GuildDungeon
{
public:
	GuildDungeon();
	~GuildDungeon();

public:

	bool OnInit(Guild* guildPtr);
	void OnTick(const Avalon::Time& now);
	void Final();

	void SetGuildPtr(Guild* guildPtr) { m_Guild = guildPtr; }
	Guild* GetGuildPtr() { return m_Guild; }

	void SetDungeonType(UInt32 dungeonType) { m_DungeonType = dungeonType; }
	UInt32 GetDungeonType() { return m_DungeonType; }

	void AddTotalDamage(UInt64 damage) { m_TotalDamage += damage; }
	UInt64 GetTotalDamage() { return m_TotalDamage; }

	void AddVictoryPubCnt() { m_VictoryPubCnt++; }
	UInt32 GetVictoryPubCnt() { return m_VictoryPubCnt; }

	void SetVictoryPubTime() { m_VictoryPubStamp = CURRENT_TIME.Sec(); }
	UInt64 GetVictoryPubTime() { return m_VictoryPubStamp; }

	const GuildDungeonBattleMap& GetBattleRecordMap() { return m_BattleRecordMap; }

	const GuildDungeonDamageVec& GetDamageVec() { return m_DamageVec; }

	const std::set<UInt64>& GetJoinSet() { return m_JoinPlayer; }

	// 获得终极地下城信息
	GuildDungeonBattleRecord* GetFinalGuildDungeonBattleRecord();
	// 获得地下城信息
	GuildDungeonBattleRecord* GetGuildDungeonBattleRecord(UInt32 dungeonId);

	// 检查所有boss是否都打死
	bool CheckAllBossIsDead();
	// 计算对终极boss打出的伤害比例
	UInt32 GetFinalBossBloodRate();
	// 增加战斗记录
	void AddBattleRecord(UInt32 dungeonId, UInt32 battleCnt, UInt64 bossOddBlood, const std::string& dropBuff);
	// 更新总伤害
	void UpdateTotalDamageDB();
	// 增加地下城的掉落buff
	void AddDropbuff(UInt32 dungeonId, UInt32 buffItemId);
	// 更新掉落buff的DB
	void UpdateDropBuffDB(UInt32 dungeonId);
	// 扣除boss血量
	void CostBossBlood(UInt32 dungeonId, UInt64 bossDamage);
	// 增加地下城的战斗次数
	void AddBattleCnt(UInt32 dungeonId, UInt32 addCnt = 1);
	// 获得boss剩余血量
	UInt64 GetBossOddBlood(UInt32 dungeonId);
	// 小boss是否都死了
	bool SmallBossIsAllDead();
	// 小boss死亡数量
	UInt32 SmallBossDeadNum();
	// 增加玩家伤害
	void AddPlayerDamage(UInt64 playerId, UInt64 damagePoint, const std::string& playerName, bool isSave = false);
	// 增加参与的玩家
	void AddJoinPlayer(UInt64 playerId, bool isSave = false);

	// 增加地下比赛
	void AddDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId);
	// 结束地下城比赛
	void EndDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId);
	// 通知正在进行的比赛boss剩余血量
	void NotifyRaceBossOddBlood(UInt32 dungeonId, UInt64 dungeonRaceId);

private:

	// 公会指针
	Guild* m_Guild;
	// 地下城类型
	UInt32 m_DungeonType;
	// 打出的总伤害,只计算终极大boss
	UInt64 m_TotalDamage;
	// key->地下城id
	GuildDungeonBattleMap m_BattleRecordMap;
	// 玩家打出的伤害
	GuildDungeonDamageVec m_DamageVec;
	// 播放胜利公告次数
	UInt32 m_VictoryPubCnt; 
	// 胜利公告时间戳
	UInt64 m_VictoryPubStamp;

	// 参与了活动的玩家，进入副本就算
	std::set<UInt64> m_JoinPlayer;

	// 战斗中的玩家(key->dungeonId)
	std::map<UInt32, std::set<UInt64>> m_DungeonRaceMap;
};



#endif // !_GUILD_DUNGEON_H_
