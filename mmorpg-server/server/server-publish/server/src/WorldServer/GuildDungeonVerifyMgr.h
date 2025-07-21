#ifndef _GUILD_DUNGEON_VERIFY_MGR_H_
#define _GUILD_DUNGEON_VERIFY_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <set>

class GuildDungeonVerify;

class GuildDungeonVerifyMgr : public Avalon::Singleton<GuildDungeonVerifyMgr>
{
public:
	GuildDungeonVerifyMgr();
	~GuildDungeonVerifyMgr();

	void Init();

	void OnTick(const Avalon::Time &now);

	// 查找
	const GuildDungeonVerify* GetGuildDungeonVerify(UInt64 id, UInt64 playerId);
	// 增加伤害验证数据
	void AddVerifyInfo(UInt64 id, UInt64 playerId, UInt64 damage, UInt64 guildId, UInt32 dungeonId, UInt32 useTime, const std::string& name, UInt32 occu);
	// 删除伤害验证数据
	void DelVerifyInfo(UInt64 id, UInt64 playerId);
	// 收到验证服的返回
	void VerifyServerRes(UInt64 id, UInt64 playerId, UInt32 dungeonId, UInt64 realDamage, UInt32 fightTime);
	// 获取需要验证的伤害
	UInt64 GetVerifyDamage(UInt64 guildId, UInt32 dungeonId);

	// 是否验证超时
	bool IsVerifyTimeOut(UInt64 id, UInt64 playerId);
	// 加入验证超时
	void AddVerifyTimeOut(UInt64 id, UInt64 playerId);
	// 是否已经验证
	bool IsVerifyFinish(UInt64 id, UInt64 playerId);
	// 加入验证
	void AddVerify(UInt64 id, UInt64 playerId);

private:

	typedef std::list<GuildDungeonVerify*> GuildDungeonVerifyList;
	typedef std::map<UInt64, GuildDungeonVerifyList> GuildDungeonVerifyMap;

	// 验证超时时间(秒)
	UInt32 m_verifyTime;

	// 验证列表 key->raceId
	GuildDungeonVerifyMap m_VerifyMap;

	// 验证超时列表key->raceId，val->玩家Id列表
	std::map<UInt64, std::set<UInt64>> m_VerifyTimeOutMap;

	// 已验证的列表key->raceId，val->玩家Id列表
	std::map<UInt64, std::set<UInt64>> m_VerifyFinishMap;
};

#endif
