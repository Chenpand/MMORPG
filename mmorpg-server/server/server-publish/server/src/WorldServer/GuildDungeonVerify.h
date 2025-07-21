#ifndef _GUILD_DUNGEON_VERIFY_H_
#define _GUILD_DUNGEON_VERIFY_H_

#include <CLDefine.h>

class GuildDungeonVerify
{
public:
	GuildDungeonVerify() : m_id(0), m_playerId(0), m_occu(0), m_guildId(0), 
		m_dungeonId(0), m_damageVal(0), m_startVerifyTime(0), m_fightTime(0){}

	~GuildDungeonVerify() {};

	UInt64 GetID() const { return m_id; }
	void SetID(UInt64 id) { m_id = id; }

	UInt64 GetPlayerId() const { return m_playerId; }
	void SetPlayerId(UInt64 id) { m_playerId = id; }

	const std::string& GetPlayerName() const { return m_playerName; }
	void SetPlayerName(const std::string& name) { m_playerName = name; }

	UInt32 GetPlayerOccu() const { return m_occu; }
	void SetPlayerOccu(UInt32 occu) { m_occu = occu; }

	UInt64 GetGuildId() const { return m_guildId; }
	void SetGuildId(UInt64 id) { m_guildId = id; }

	UInt32 GetDungeonId() const { return m_dungeonId; }
	void SetDungeonId(UInt32 id) { m_dungeonId = id; }

	UInt64 GetDamageVal() const { return m_damageVal; }
	void SetDamageVal(UInt64 val) { m_damageVal = val; }

	UInt64 GetStartVerifyTime() const { return m_startVerifyTime; }
	void SetStartVerifyTime(UInt64 time) { m_startVerifyTime = time; }

	UInt32 GetFightTime() const { return m_fightTime; }
	void SetFightTime(UInt32 time) { m_fightTime = time; }

	std::string SerializeVerifyInfo() 
	{
		std::stringstream ss;
		ss << "raceId:"<<m_id;
		ss << ", playerId:" << m_playerId;
		ss << ", playerName:" << m_playerName;
		ss << ", occu:" << m_occu;
		ss << ", guildId:" << m_guildId;
		ss << ", dungeonId:" << m_dungeonId;
		ss << ", damageVal:" << m_damageVal;
		ss << ", startVerifyTime:" << m_startVerifyTime;
		ss << ", fightTime:" << m_fightTime;
		return ss.str();
	};

private:

	// 对局Id
	UInt64 m_id;
	// 玩家Id
	UInt64 m_playerId;
	// 玩家名字
	std::string m_playerName;
	// 玩家职业
	UInt32 m_occu;
	// 公会Id
	UInt64 m_guildId;
	// 地下城Id
	UInt32 m_dungeonId;
	// 伤害值
	UInt64 m_damageVal;
	// 开始验证时间
	UInt64 m_startVerifyTime;
	// 战斗用时
	UInt32 m_fightTime;
};

#endif
