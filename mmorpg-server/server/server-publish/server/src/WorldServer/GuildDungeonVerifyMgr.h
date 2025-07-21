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

	// ����
	const GuildDungeonVerify* GetGuildDungeonVerify(UInt64 id, UInt64 playerId);
	// �����˺���֤����
	void AddVerifyInfo(UInt64 id, UInt64 playerId, UInt64 damage, UInt64 guildId, UInt32 dungeonId, UInt32 useTime, const std::string& name, UInt32 occu);
	// ɾ���˺���֤����
	void DelVerifyInfo(UInt64 id, UInt64 playerId);
	// �յ���֤���ķ���
	void VerifyServerRes(UInt64 id, UInt64 playerId, UInt32 dungeonId, UInt64 realDamage, UInt32 fightTime);
	// ��ȡ��Ҫ��֤���˺�
	UInt64 GetVerifyDamage(UInt64 guildId, UInt32 dungeonId);

	// �Ƿ���֤��ʱ
	bool IsVerifyTimeOut(UInt64 id, UInt64 playerId);
	// ������֤��ʱ
	void AddVerifyTimeOut(UInt64 id, UInt64 playerId);
	// �Ƿ��Ѿ���֤
	bool IsVerifyFinish(UInt64 id, UInt64 playerId);
	// ������֤
	void AddVerify(UInt64 id, UInt64 playerId);

private:

	typedef std::list<GuildDungeonVerify*> GuildDungeonVerifyList;
	typedef std::map<UInt64, GuildDungeonVerifyList> GuildDungeonVerifyMap;

	// ��֤��ʱʱ��(��)
	UInt32 m_verifyTime;

	// ��֤�б� key->raceId
	GuildDungeonVerifyMap m_VerifyMap;

	// ��֤��ʱ�б�key->raceId��val->���Id�б�
	std::map<UInt64, std::set<UInt64>> m_VerifyTimeOutMap;

	// ����֤���б�key->raceId��val->���Id�б�
	std::map<UInt64, std::set<UInt64>> m_VerifyFinishMap;
};

#endif
