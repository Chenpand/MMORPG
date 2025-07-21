#ifndef _GUILD_DUNGEON_MGR_H_
#define _GUILD_DUNGEON_MGR_H_

#include <CLDefine.h>
#include <CLGuildDefine.h>
#include <AvalonSingleton.h>

class WSPlayer;
class GuildDungeon;
class CLRecordCallback;
class Guild;
namespace CLProtocol{
	struct GuildDungeonInfo;
}

/**
 *@brief ����ĵ��³ǹ���
 */
class GuildDungeonMgr : public Avalon::Singleton<GuildDungeonMgr>
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::map<UInt64, GuildDungeon*> GuildDungeonMap;

public:
	GuildDungeonMgr();
	~GuildDungeonMgr();

public:
	void Init();

	void OnTick(const Avalon::Time &now);

	void Final();

	bool IsGuildDungeon();

	bool IsStartDungeon();

	bool IsJoinOrLeaveGuild();

	bool SwitchIsOpen();

	void NotifyGuildDungeonActivity(WSPlayer* player);

	// �����ɢ
	void GuildDismiss(UInt64 guildId);
	// ��������³���Ϣ����
	void HandleGuildDungeonInfoReq(WSPlayer* player);
	// ��������³��˺���������
	void HandleGuildDungeonDamageRankReq(WSPlayer* player);
	// ��������³Ǹ�����Ϣ����
	void HandleGuildDungeonCopyReq(WSPlayer* player);
	// ��������³ǳ齱����
	void HandleGuildDungeonLotteryReq(WSPlayer* player);
	// ��������³ǵ�������
	void HandleGuildDungeonStatueReq(WSPlayer* player);
	// ֪ͨ�ͻ��˵��³�״̬�޸�
	void SyncGuildDungeonStatus();

	// ����DB����
	void LoadData();
	// ���ص��³�״̬
	bool LoadGuildDungeonStatus(CLRecordCallback* callback);
	// ���ع�����³�����
	bool LoadGuildDungeonData(CLRecordCallback* callback);
	// ���ع�����³�ս������
	bool LoadGuildDungeonBattle(CLRecordCallback* callback);
	// ���ع�����³�����˺���¼
	bool LoadGuildDungeonDamage(CLRecordCallback* callback);
	// ���ع�����³�����콱�ص�
	bool LoadGuildDungeonReward(CLRecordCallback* callback);
	// ���ع�����³���Ҳ���ص�
	bool LoadGuildDungeonJoin(CLRecordCallback* callback);
	// ���ع�����³����
	bool LoadGuildDungeonRand(CLRecordCallback* callback);

	// ��ȡ������³���Ϣ
	UInt32 StartGuildDungeon(UInt64 guildId, UInt32 dungeonId, UInt64 dungeonRaceId, CLProtocol::GuildDungeonInfo& guildDungeonInfo);
	// ��¼���³��˺�
	void RecordDungeonDamage(UInt64 guildId, UInt32 dungeonId, UInt64 playerId, UInt32 occu, UInt64 damageVal, UInt64 dungeonRaceId, const std::string& playerName, UInt32 fightTime = 0);
	// ���ӵ��³ǵ�ս������
	void AddGuildDungeonBattleCnt(UInt64 guildId, UInt32 dungeonId);
	// ���Ӳ������
	void AddJoinPlayer(UInt64 guildId, UInt64 playerId);
	// ������������Ƿ���
	void GuildLevelUp(Guild* guildPtr);
	// ��ȡboss��ʣ��Ѫ��
	UInt64 GetBossOddBlood(UInt64 guildId, UInt32 dungeonId);
	// ���������ֳ�
	void GuildAuctionBonus(UInt64 guildId, UInt32 rewardNum);
	// ��ȡ����ؿ�
	void GetRandDungeon(UInt32 dungeonType, std::vector<UInt32>& randVec);
	// GM����״̬
	void GMUpdateStatus(UInt32 status);
	// GM���Сboss
	void ClearSmallBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, const std::string& playerName);
	// GM�����boss
	void ClearBigBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, UInt64 damage, const std::string& playerName);
	// GM��buff�ʹ���
	void GMAddBuffCnt(UInt64 guildId, UInt32 buffItemId, UInt32 cnt);


	UInt32 GetGuildDungeonStatus() { return m_DungeonStatus; }
private:

	// ��ù�����³�
	GuildDungeon* FindGuildDungeon(UInt64 guildId);
	// ��������ĵ��³�
	GuildDungeon* CreateGuildDungeon(Guild* guildPtr);

	// �л�״̬
	void SwitchDungeonStatus(UInt32 status, UInt64 startStamp);
	// ׼��
	void DungeonPrepare();
	// ��ʼ
	void DungeonStart();
	// �콱
	void DungeonReward();
	// ����
	void DungeonEnd();
	// ��ʼ��TickMap
	void InitTickMap();
	// ִ��tick����
	void ExecuteTick(const Avalon::Time &now);
	// ״̬�Ƿ���Ч
	bool IsVaildStatus(UInt32 status);

	// �����콱���
	void AddObtainRewardPlayer(UInt64 playerId, bool isSave = false);
	// ����Ƿ��Ѿ���ȡ����
	bool PlayerIsObtainReward(UInt64 playerId);
	// ��ȡ��Ҵ�����˺�����(�ٷֱ�����ȡ��)
	UInt32 GetPlayerDamageRate(UInt64 guildId, UInt64 playerId);
	// ���Ͳ��뽱���ʼ�
	void SendAttendRewardMail(UInt64 guildId, GuildDungeon* dungeon);
	// ����
	void GuildDungeonStatue(UInt64 guildId, const GuildDungeonDamageVec& damageVec);
	// ����ͨ������
	void AddClearGateRank(UInt64 guildId);
	// ֪ͨboss����
	void BroadcastBossDead(GuildDungeon* guildDungeon, UInt32 dungeonId);
	// �������ͻ�ȡ�����б�
	void GetRewardByType(UInt32 rewardType, UInt32 dungeonType, ItemRewardVec& rewardVec);
	// �����������
	void GuildAuctionReward(UInt64 guildId, UInt32 dungeonType, UInt32 guildRank);
	// ������ݱ�
	void ClearDBTable(const std::string& _tableName);
	// ������³�����
	void ClearGuildDungeon();
	// ����������³�
	void AddRandDungeon(UInt32 dungeonType, UInt32 dungeonId);
	// ����������³�
	void RandDungeon(UInt32 guildLvl);
	// ��ȡ����ͨ��ʱ������
	UInt32 GetPassGateTimeRank(UInt64 guildId);

private:

	// ������³�״̬
	UInt32 m_DungeonStatus;
	// ״̬��ʼʱ���
	UInt64 m_StatusStartStamp;
	// ���³ǿ�ʼ��ʱ��
	UInt64 m_startStamp;
	// ���ݼ���
	UInt32 m_LoadFlag;
	// ������³�,key->����ID
	GuildDungeonMap m_GuildDungeonMap;
	// ��ȡ���������
	std::set<UInt64> m_ObtainRewardPlayer;
	// �ָ������ֳ�
	bool m_IsRecvCheck;
	// �ܼ�
	UInt32 m_Week;
	// �ָ��ֳ���tickmap
	std::map<UInt64, UInt32> m_TickMap;
	// ͨ����ʱ����
	std::vector<GuildDungeonGateTime> m_GateTimeVec;
	// �Ѷ����(key->�Ѷ�)
	std::map<UInt32, std::vector<UInt32>> m_RandDungeonMap;

	// �Ѷ�������³�����(key->�Ѷȣ�val->�������)
	std::map<UInt32, UInt32> m_RandDungenNumMap;
};

#endif // !_GUILD_DUNGEON_MGR_H_
