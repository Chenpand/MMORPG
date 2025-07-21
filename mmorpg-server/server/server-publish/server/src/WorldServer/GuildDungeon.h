#ifndef _GUILD_DUNGEON_H_
#define _GUILD_DUNGEON_H_

#include <set>
#include <CLDefine.h>
#include <CLGuildDefine.h>

class Guild;

/**
 *@brief ����ĵ��³�
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

	// ����ռ����³���Ϣ
	GuildDungeonBattleRecord* GetFinalGuildDungeonBattleRecord();
	// ��õ��³���Ϣ
	GuildDungeonBattleRecord* GetGuildDungeonBattleRecord(UInt32 dungeonId);

	// �������boss�Ƿ񶼴���
	bool CheckAllBossIsDead();
	// ������ռ�boss������˺�����
	UInt32 GetFinalBossBloodRate();
	// ����ս����¼
	void AddBattleRecord(UInt32 dungeonId, UInt32 battleCnt, UInt64 bossOddBlood, const std::string& dropBuff);
	// �������˺�
	void UpdateTotalDamageDB();
	// ���ӵ��³ǵĵ���buff
	void AddDropbuff(UInt32 dungeonId, UInt32 buffItemId);
	// ���µ���buff��DB
	void UpdateDropBuffDB(UInt32 dungeonId);
	// �۳�bossѪ��
	void CostBossBlood(UInt32 dungeonId, UInt64 bossDamage);
	// ���ӵ��³ǵ�ս������
	void AddBattleCnt(UInt32 dungeonId, UInt32 addCnt = 1);
	// ���bossʣ��Ѫ��
	UInt64 GetBossOddBlood(UInt32 dungeonId);
	// Сboss�Ƿ�����
	bool SmallBossIsAllDead();
	// Сboss��������
	UInt32 SmallBossDeadNum();
	// ��������˺�
	void AddPlayerDamage(UInt64 playerId, UInt64 damagePoint, const std::string& playerName, bool isSave = false);
	// ���Ӳ�������
	void AddJoinPlayer(UInt64 playerId, bool isSave = false);

	// ���ӵ��±���
	void AddDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId);
	// �������³Ǳ���
	void EndDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId);
	// ֪ͨ���ڽ��еı���bossʣ��Ѫ��
	void NotifyRaceBossOddBlood(UInt32 dungeonId, UInt64 dungeonRaceId);

private:

	// ����ָ��
	Guild* m_Guild;
	// ���³�����
	UInt32 m_DungeonType;
	// ��������˺�,ֻ�����ռ���boss
	UInt64 m_TotalDamage;
	// key->���³�id
	GuildDungeonBattleMap m_BattleRecordMap;
	// ��Ҵ�����˺�
	GuildDungeonDamageVec m_DamageVec;
	// ����ʤ���������
	UInt32 m_VictoryPubCnt; 
	// ʤ������ʱ���
	UInt64 m_VictoryPubStamp;

	// �����˻����ң����븱������
	std::set<UInt64> m_JoinPlayer;

	// ս���е����(key->dungeonId)
	std::map<UInt32, std::set<UInt64>> m_DungeonRaceMap;
};



#endif // !_GUILD_DUNGEON_H_
