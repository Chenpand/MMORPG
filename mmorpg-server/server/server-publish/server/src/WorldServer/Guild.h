#ifndef __GUILD_H__
#define __GUILD_H__

#include <AvalonSimpleTimer.h>
#include <CLSyncObject.h>
#include <CLGuildDefine.h>
#include <CLSceneObject.h>
#include <CLObjectIndex.h>
#include <CLObjectProperty.h>
#include <CLChatDefine.h>
#include <CLItemDefine.h>
#include "WorldSysNotify.h"
#include "GuildMember.h"
#include "GuildBuilding.h"
#include "GuildDonate.h"
#include "GuildTable.h"
#include "GuildStorage.h"
#include "GuildEventMgr.h"
#include "GuildHonorMgr.h"

struct GuildDataEntry;
struct GuildEntry;
struct GuildSkillDataEntry;
struct RedPacketDataEntry;

class GuildBattleTerritory;
class GuildEmblemTransaction;

enum GUILD_INIT_TYPE
{
	GUILD_INIT_TYPE_CREATE = 0,
	GUILD_INIT_TYPE_LOAD = 1,
};


class GuildComparer
{
public:
	bool operator()(Guild* a, Guild* b);
};

class Guild : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Guild, Avalon::NullMutex)
public:
	// ���������б��������
	const UInt32 GUILD_REQUEST_QUEEU_MAX_SIZE = 50;

	Guild();
	~Guild();

	bool Init(GUILD_INIT_TYPE type);

	void OnTick(const Avalon::Time& now);

public:
	// �����¼�

	/**
	*@brief ��ɢ���
	*/
	void OnDismiss(UInt8 dismissType);

	/**
	*@brief �����˻᳤
	*/
	void OnChangeLeader(GuildMember* newLeader);

	/**
	*@brief ����һ��
	*/
	void OnDayChanged();

	/**
	*@brief ����һ��
	*/
	void OnWeekChanged();

	/**
	*@brief ���᳤�Զ�ת��
	*/
	void CheckAutoChangeLeader();

public:
	/**
	*@brief ����
	*/
	inline std::string GetName() { return m_Name; }
	inline void SetName(std::string name) { m_Name = name; }

	/**
	*@brief �ȼ�
	*/
	inline UInt8 GetLevel() { return m_Level; }
	void SetLevel(UInt8 level);

	/**
	*@brief �᳤
	*/
	inline GuildMember* GetLeader() const { return m_Leader; }
	inline void SetLeader(GuildMember* leader) { m_Leader = leader; }
	inline void SetLeaderName(const std::string& name) { m_LeaderName = name; }

	/**
	*@brief ����
	*/
	inline const std::string& GetDeclaration() const { return m_Declaration; }
	inline void SetDeclaration(std::string declaration) { m_Declaration = declaration; }

	/**
	*@brief ����ʱ��
	*/
	inline void SetUpTime(UInt32 time) { m_SetupTime = time; }
	inline UInt32 GetSetUpTime() const { return m_SetupTime; }

	/**
	*@brief ��ɢʱ��
	*/
	inline UInt32 GetDismissTime() const { return m_DismissTime; }
	inline void SetDismissTime(UInt32 time) { m_DismissTime = time; }

	/**
	*@brief �ʲ�
	*/
	inline UInt32 GetFund() const { return m_Fund; }
	inline void AddFund(UInt32 fund) { m_Fund = m_Fund + fund; m_WeekAddedFund = m_WeekAddedFund + fund; }

	/**
	*@brief ��Ա����
	*/
	bool AddMember(GuildMember* member);
	GuildMember* FindMember(ObjID_t id);
	void DelMember(GuildMember* member);
	void GetGuildMemberList(std::vector<GuildMemberEntry>& memberList);
	void GetGuildMemberListByPost(std::vector<GuildMember*>& memberList, GuildPost post);
	void GetGuildMembers(std::vector<GuildMember*>& memberList);
	UInt32 GetMemberNum() { return m_Members.size(); }

	/**
	*@brief ��ȡ������Ϣ
	*/
	void GetGuildEntry(WSPlayer* player, GuildEntry& entry);

	/**
	*@brief �����Ƿ�����
	*/
	bool IsGuildFull() const;

	/**
	*@brief ���Ὠ��
	*/
	inline UInt8 GetBuildingLevel(GuildBuildingType type) const { return m_BuildingMgr.GetLevel(type); };

	/**
	*@brief �����Ĺ������
	*/
	inline UInt8 GetEnrollTerrID() const { return m_EnrollTerrId; }
	inline void SetEnrollTerrID(UInt8 id) { m_EnrollTerrId = id; }

	/**
	*@brief ռ��Ĺ������
	*/
	inline UInt8 GetOccupyTerrID() const { return m_OccupyTerrId; }
	inline void SetOccupyTerrID(UInt8 id) { m_OccupyTerrId = id; }

	/**
	*@brief ռ��Ĺ��������
	*/
	inline UInt8 GetOccupyCrossTerrID() const { return m_OccupyCrossTerrId; }
	inline void SetOccupyCrossTerrID(UInt8 id) { m_OccupyCrossTerrId = id; }

	/**
	*@brief ����ս����
	*/
	inline UInt32 GetGuildBattleScore() const { return m_GuildBattleScore; }
	inline void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief ����ս����
	*/
	inline UInt8 GetInspire() const { return m_Inspire; }

	/**
	*@brief ����ս��Աս������
	*/
	inline UInt32 GetGuildMemberBattleCount() const { return m_GuildMemberBattleCount; }

	/**
	*@brief ����ս���輼��ID
	*/
	inline UInt16 GetInspireSkillId() const { return m_InspireSkillId; }

	/**
	*@brief ��ʷռ�����
	*/
	inline UInt8 GetHistoryTerrId() const { return (UInt8)m_HistoryTerrId; }
	inline void SetHistoryTerrId(UInt8 terrId) { m_HistoryTerrId = (UInt32)terrId; };

	/**
	*@brief ����սʤ���齱����
	*/
	inline UInt8 GetWinProbability() const { return m_WinProbability; }

	/**
	*@brief ����սʧ�ܳ齱����
	*/
	inline UInt8 GetLoseProbability() const { return m_LoseProbability; }

	/**
	*@brief ����ֿ����Ȩ��
	*/
	inline UInt8 GetStorageAddPost() const { return m_StorageAddPost; }

	/**
	*@brief ����ֿ����Ȩ��
	*/
	inline UInt8 GetStorageDelPost() const { return m_StorageDelPost; }

	/**
	*@brief �������ȼ�
	*/
	inline UInt32 GetJoinLevel() const { return m_JoinLevel; }
	inline void SetJoinLevel(UInt32 level) { m_JoinLevel = level; }

	/**
	*@brief ���ù�����³�boss�Ѷ�
	*/
	inline UInt32 GetGuildDungeonType() const { return m_GuildDungeonType ? m_GuildDungeonType : 1; }
	inline void SetGuildDungeonType(UInt32 bossType) { m_GuildDungeonType = bossType; }

	/**
	 *@brief ���Ὠ��������ʱ��
	 */
	inline UInt32 GetLastUpdateGuildBuildingTime() {return m_LastUpdateGuildBuildingTime;}
	inline void SetLastUpdateGuildBuildingTime(UInt32 LastTime) { m_LastUpdateGuildBuildingTime = LastTime; }
public:
	/* ������빫����� */

	/**
	*@brief �����б��Ƿ�����
	*/
	bool IsRequestQueueFull() const;

	/**
	*@brief �Ƿ��Ѿ�������
	*/
	bool IsRequesting(ObjID_t id) const;

	/**
	*@brief ɾ��������
	*/
	void DelRequester(ObjID_t id);

	/**
	*@brief ֪ͨ����Ѿ�û����������
	*/
	void NotifyRequesterIsEmpty();

	/**
	*@brief ������빫��
	*/
	UInt32 RequestJoinGuild(WSPlayer* player);

	/**
	*@brief �ظ���������
	*/
	void ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree);

	/**
	*@brief ��������б�
	*/
	void RequestClearRequestList(WSPlayer* player);

	/**
	*@brief ��ȡ�������б�
	*/
	std::vector<GuildRequesterInfo> GetRequesterList();

public:
	/**
	*@brief �����޸Ĺ�����
	*/
	void RequestChangeGuildName(WSPlayer* player, const std::string& name, ObjID_t itemId, UInt32 itemTypeId);

	/**
	*@brief �����޸Ĺ��ṫ��
	*/
	void RequestChangeAnnouncement(WSPlayer* player, const std::string& content);

	/**
	*@brief �����޸Ĺ�������
	*/
	void RequestChangeDeclaration(WSPlayer* player, const std::string& content);

	/**
	*@brief ���͹����ʼ�
	*/
	void SendGuildMail(WSPlayer* player, const std::string& content);

	/**
	*@brief �޸�ְ��
	*/
	void RequestChangePost(WSPlayer* player, ObjID_t targetId, ObjID_t replaceId, GuildPost post);

	/**
	*@brief ����
	*/
	void RequestKickPlayer(WSPlayer* player, ObjID_t targetId);

	/**
	*@brief �뿪����
	*/
	void RequestLeave(WSPlayer* player);

	/**
	*@brief ��������
	*/
	void RequestUpgradeBuilding(WSPlayer* player, GuildBuildingType type);

	/**
	*@brief �����Ϊ�᳤
	*/
	UInt32 RequestBecomeLeader(WSPlayer* player);

	/**
	*@brief �����ɢ
	*/
	void RequestDismiss(WSPlayer* player);

	/**
	*@brief ����ȡ��
	*/
	void RequestCancelDismiss(WSPlayer* player);

	/**
	*@brief ȡ����ɢ֪ͨ
	*/
	void CancelDismissNotify();

	/**
	*@brief �������ԷѺ��
	*/
	UInt32 OnRequestPayRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

	/**
	*@brief ��鹫���ԷѺ���Ƿ���Է���
	*/
	UInt32 CheckGuildRedPacketCanBuy(WSPlayer* player, RedPacketDataEntry& dataEntry);

	/**
	*@brief �������ü���ȼ�
	*/
	void RequestSetJoinLevel(WSPlayer* player, UInt32 joinLevel);

public:
	/**
	*@brief �޸Ĺ��������񷵻�
	*/
	bool OnCheckChangeGuildNameRet(WSPlayer* player, const std::string& name);

	/**
	*brief
	*/

	/**
	*@brief ���͹����ʼ����񷵻�
	*/
	bool OnCheckMailCounterRet(WSPlayer* player, const std::string& content);

	/**
	*@brief ��ɢ�������񷵻�
	*/
	bool OnCheckDismissRet(WSPlayer* player);

	/**
	*@brief �����ԷѺ�����񷵻�
	*/
	UInt32 OnCheckPayRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

public:
	// ���ḣ����

	/**
	*@brief �������
	*/
	void RequestDonate(WSPlayer* player, GuildDonateType type, UInt8 num);

	/**
	*@brief �������񷵻�
	*/
	bool OnCheckDonateRet(WSPlayer* player, GuildDonateType type, UInt8 num);

	/**
	*@brief ��ȡ������־
	*/
	std::vector<GuildDonateLog> GetDonateLogs();

public:
	// ����ս����

	/**
	*@brief ������������
	*/
	void RequestUpgradeSkill(WSPlayer* player, UInt16 skillId);

	/**
	*@brief ������������
	*/
	bool OnCheckUpgradeSkillRet(WSPlayer* player, UInt16 skillId, UInt8 level);

public:
	// �����̵�

	/**
	*@brief ˢ���̵�
	*/
	void RefreshShop();

	/**
	*@brief ˢ���̵��¼�
	*/
	void OnRefreshShop();

public:
	// ����

	/**
	*@brief ��ȡ�᳤��Ϣ
	*/
	void RequestLeaderInfo(WSPlayer* player);

	/**
	*@brief ����Ĥ��
	*/
	void RequestOrz(WSPlayer* player, GuildOrzType type);

	/**
	*@brief ���Ĥ�ݷ���
	*/
	bool CheckOrzRet(WSPlayer* player, GuildOrzType type);

public:
	// Բ������

	/**
	*@brief �μ�Բ������
	*/
	void RequestJoinTable(WSPlayer* player, UInt8 seat, UInt8 type);

	/**
	*@brief �μ�Բ������
	*/
	bool CheckJoinTable(WSPlayer* player, UInt8 seat, UInt8 type);

	/**
	*@brief Բ���������
	*/
	void OnTableFull();

public:
	//����ս

	/**
	*@brief ����һ������ս
	*/
	void OnGuildBattleRaceEnd(UInt32 terrId, UInt32 score);

	/**
	*@brief ����ս����
	*/
	void ClearEnroll();

	/**
	*@brief ����
	*/
	UInt32 Inspire();

	/**
	*@brief ��ȡ������Ϣ
	*/
	UInt32 GetGuildBattleInspireInfo(std::vector<GuildBattleInspireInfo>& infos);

	/**
	*@brief ���ù���ֿ�Ȩ�޺ͼ���
	*/
	UInt32 SetGuildStorageSetting(GuildStorageSetting type, UInt32 value);

	/**
	*@brief ���Ž���
	*/
	void SendGuildBattleReward(const std::vector<ItemReward>& leaderReward, const std::vector<ItemReward>& memberReward);

	/**
	*@brief ����ʧ�ܽ���
	*/
	void SendGuildBattleLoseReward(std::vector<ItemReward> itemReward);

	/**
	*@brief ���Ż᳤����
	*/
	void SendGuildLeaderReward(std::vector<ItemReward> itemReward);

	/**
	*@brief ���ų�Ա����
	*/
	void SendGuildMemberReward(std::vector<ItemReward> itemReward);

	/**
	*@brief ����ս����
	*/
	void SetGuildBattleFigureStatue();


	/**
	*@brief ��ȡ�ֿ���Ʒ
	*/
	void GetStorageItems(std::vector<GuildStorageItemInfo>& items);

	/**
	*@brief ��ȡ�ֿ��¼
	*/
	void GetStorageRecords(std::vector<GuildStorageOpRecord>& record);

	/**
	*@brief ɾ���ֿ���Ʒ
	*/
	UInt32 DelStorageItems(std::vector<GuildStorageDelItemInfo>& delItems, std::vector<GuildStorageItemInfo>& itemInfos);

	/**
	*@brief ����ֿ��¼
	*/
	void InsertStorageItemRecords(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);

	/**
	*@brief ��ȡ�ֿ��������
	*/
	UInt32 GetStorageMaxSize();

	/**
	*@brief ������ս����
	*/
	void ResultChallengeEnroll(std::vector<ItemReward> itemReward, std::string guildName, UInt32 price);

	/**
	*@brief ���ñ������id
	*/
	void SetEnrollPlayer(UInt64 playerId);

	/**
	*@brief ��ȡ�������id
	*/
	UInt64 GetEnrollPlayer();

	/**
	*@brief �齱
	*/
	UInt32 Lottery(WSPlayer* player, bool isWin, bool isCheck = true);

	/**
	*@brief ���Ź��ṱ��
	*/
	UInt32 GiveContribution(WSPlayer* player);

	/**
	*@brief �����Ա��ӹ���
	*/
	void MemberAddContri(GuildMember* member, UInt32 contri, std::string reason);

	/**
	*@brief ����ռ�����
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief ����ս����֪ͨ
	*/
	void OnGuildBattleEnrollMail(UInt8 terrId, GuildMember* member = NULL);

	/**
	*@brief ����ս�������ֽ���
	*/
	void OnGuildBattleScoreRankReward(UInt32 titleId, UInt32 expireTime);

public:
	/* ����ֿ� */
	void AddStorageItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb);
	
	void AddStorageItemCb(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items);

	void GetStorageItemCb(UInt64 playerUid, WSItem* item, UInt64 oldUid);

	WSItem* QueryStorageItem(UInt64 uid);

	bool CheckStorageCapacity(WSItem* item);

	bool ShopBuyAddStorage(WSPlayer* player, WSItem* item);

public:
	/* ����¼� */

	/**
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player, bool dayChanged);

	/**
	*@brief �������
	*/
	void OnPlayerOffline(WSPlayer* player);

	/**
	*@brief �¼��빫��
	*/
	void OnMemberEnter(WSPlayer* replyPlayer, GuildMember* member);

	/**
	*@brief �����scene�ϻ�ù���
	*/
	void OnScenePlayerAddContri(ObjID_t roleId, UInt32 num);

	/**
	*@brief �˺�������ɾ��
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

public:
	/* ��Ϣ��� */

	/**
	*@brief �㲥��Ϣ
	*/
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);
	void Broadcast(Avalon::Protocol& protocol, UInt32 playerLevelLimit);

	void Broadcast(Avalon::Protocol& protocol, const std::function<void(WSPlayer*, Avalon::Protocol*)>& callback);
	
	/**
	*@brief �㲥�������Ȩ�޵�����Ϣ
	*/
	void BroadcastByPower(Avalon::Protocol& protocol, GuildPowerFlag power);
	void BroadcastByPower(Avalon::Packet* packet, GuildPowerFlag power);

	/**
	*@brief ͬ�����������Ϣ
	*/
	void SyncBaseInfo(GuildMember* member);

	/**
	*@brief ͬ�����������Ϣ
	*/
	void BroadcastInfo();

	/**
	*@brief ͬ����Ա��Ϣ��scene
	*/
	void SyncMemberInfoToScene(GuildMember* member, bool leaveGuild = false);

	/**
	*@brief ͬ���ռǵ�scene
	*/
	void SyncEmblemToScene(GuildMember* member);

	/**
	*@brief ͬ�������Ϣ��Scene
	*/
	void SyncCrossOccupyToScene();

	/**
	*@brief ͬ�������Ϣ��Scene
	*/
	void SyncOccupyToScene();

	/**
	*@brief �㲥��ʾ
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
		Broadcast(notify);
	}

	/**
	*@brief ���͹�������
	*/
	void Chat(const std::string& content, bool bLink = false);

	/**
	*@brief ���¹���ȼ����а�
	*/
	void UpdateGuildLevelSortList();

	/**
	*@brief �����ռ�
	*/
	void HandleEmblemUpgrade(WSPlayer* player);

	/**
	*@brief �ռ����񷵻�
	*/
	void EmblemUpgradeTransaction(WSPlayer* player, UInt32 emblemLvl, GuildEmblemTransaction* tran);

	/**
	*@brief ���ḱ���Ѷ�����
	*/
	void HandleGuildDungeonType(WSPlayer* player, UInt32 dType);

	/**
	*@brief �������ÿ�ս�����ȡ
	*/
	UInt32 HandleGuildTerrDayReward(WSPlayer* player);

	/**
	*@brief ÿ����������Ա���ÿ�ս�����ȡ���
	*/
	void ClearMemberTerrDayReward(bool isClear);

	/**
	*@brief ���¹���սÿ�ܻ������а�
	*/
	void UpdateGuildBattleWeekScoreRank(UInt32 addScore);

public:
	/* ���ݿ���� */
	void SaveToDB();
	void DelFromDB();
	void InsertToDB();


	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("setuptime", m_SetupTime);
	CL_DBPROPERTY("declaration", m_Declaration)
	CL_DBPROPERTY("fund", m_Fund)
	CL_DBPROPERTY("announcement", m_Announcement)
	CL_DBPROPERTY("buildings", m_BuildingMgr)
	CL_DBPROPERTY("donate_logs", m_DonateLogMgr)
	CL_DBPROPERTY("shopid", m_ShopId)
	CL_DBPROPERTY("dismiss_time", m_DismissTime)
	CL_DBPROPERTY("popularoty", m_LeaderPopularoty)
	CL_DBPROPERTY("table", m_Table)
	CL_DBPROPERTY("enroll_terrid", m_EnrollTerrId)
	CL_DBPROPERTY("battle_score", m_GuildBattleScore)
	CL_DBPROPERTY("occupy_terrid", m_OccupyTerrId)
	CL_DBPROPERTY("inspire", m_Inspire)
	CL_DBPROPERTY("win_probability", m_WinProbability)
	CL_DBPROPERTY("lose_probability", m_LoseProbability)
	CL_DBPROPERTY("storage_add_post", m_StorageAddPost)
	CL_DBPROPERTY("storage_del_post", m_StorageDelPost)
	CL_DBPROPERTY("enroll_player", m_EnrollPlayerId)
	CL_DBPROPERTY("occupy_cross_terrid", m_OccupyCrossTerrId)
	CL_DBPROPERTY("history_terrid", m_HistoryTerrId)
	CL_DBPROPERTY("join_level", m_JoinLevel)
	CL_DBPROPERTY("dungeon_type", m_GuildDungeonType);
	CL_DBPROPERTY("prosperity_status", m_ProsperityStatus);
	CL_DBPROPERTY("week_added_fund", m_WeekAddedFund);
	CL_DBPROPERTY("merger_request_status", m_MergerRequsetStatus);
	CL_DBPROPERTY("goal_guild_id", m_MergerGoalGuildID);
	CL_DBPROPERTY("merger_request_time", m_MergerRequestTime);
	CL_DBPROPERTY("merger_accept_time", m_MergerRequestAccpetTime);
	CL_DBPROPERTY("merger_other_time", m_MergerOtherGuildTime);
	CL_DBPROPERTY("last_week_added_fund", m_LastWeekAddedFund);
	CL_DBPROPERTY("last_update_guild_building_time", m_LastUpdateGuildBuildingTime);
	CL_DBPROPERTY("honor_log", m_GuildHonorMgr);
	CL_DBPROPERTY_END()

public://ͬ�����	
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(m_Name, GA_NAME, SST_WORLD)
		CL_SYNC_PROPERTY(m_Level, GA_LEVEL, SST_WORLD)
		CL_SYNC_PROPERTY(m_Declaration, GA_DECLARATION, SST_WORLD)
		CL_SYNC_PROPERTY(m_Fund, GA_FUND, SST_WORLD)
		CL_SYNC_PROPERTY(m_Announcement, GA_ANNOUNCEMENT, SST_WORLD)
		CL_SYNC_PROPERTY(m_BuildingMgr, GA_BUILDING, SST_WORLD)
		CL_SYNC_PROPERTY(m_DismissTime, GA_DISMISS_TIME, SST_WORLD)
		CL_SYNC_PROPERTY(m_MemberNum, GA_MEMBER_NUM, SST_WORLD)
		CL_SYNC_PROPERTY(m_LeaderName, GA_LEADER_NAME, SST_WORLD)
		CL_SYNC_PROPERTY(m_EnrollTerrId, GA_ENROLL_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_GuildBattleScore, GA_BATTLE_SCORE, SST_WORLD)
		CL_SYNC_PROPERTY(m_OccupyTerrId, GA_OCCUPY_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_Inspire, GA_INSPIRE, SST_WORLD)
		CL_SYNC_PROPERTY(m_WinProbability, GA_WIN_PROBABILITY, SST_WORLD)
		CL_SYNC_PROPERTY(m_LoseProbability, GA_LOSE_PROBABILITY, SST_WORLD)
		CL_SYNC_PROPERTY(m_StorageAddPost, GA_STORAGE_ADD_POST, SST_WORLD)
		CL_SYNC_PROPERTY(m_StorageDelPost, GA_STORAGE_DEL_POST, SST_WORLD)
		CL_SYNC_PROPERTY(m_OccupyCrossTerrId, GA_OCCUPY_CROSS_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_HistoryTerrId, GA_HISTORY_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_JoinLevel, GA_JOIN_LEVEL, SST_WORLD)
		CL_SYNC_PROPERTY(m_GuildDungeonType, GA_DUNGEON_TYPE, SST_WORLD)
		CL_SYNC_PROPERTY(m_LastWeekAddedFund, GA_LAST_WEEK_ADD_FUND, SST_WORLD)
		CL_SYNC_PROPERTY(m_WeekAddedFund, GA_THIS_WEEK_ADD_FUND, SST_WORLD)

	CL_SYNC_PROPERTY_END()

private:
	/* �ڲ��ӿ� */

	/**
	*@brief ��ȡ��ְͬ������������-1��������
	*/
	UInt32 _GetMaxNumByPost(GuildPost post);

	/**
	*@brief ��ȡ��ְͬ�������
	*/
	UInt32 _GetMemberNumByPost(GuildPost post);

	/**
	*@brief ��ǰְ���ܷ�����Ŀ��ְ��
	*/
	bool _CanSetPost(GuildPost post, GuildPost targetPost);

	/**
	*@brief ��ȡ��������
	*/
	UInt32 _GetDonateCost(GuildDonateType type);

	/**
	*@brief ��ȡ��������
	*/
	UInt8 _GetDonateMaxCount(WSPlayer* player, GuildDonateType type);

	/**
	*@brief ��ȡcounter����
	*/
	const char* _GetDonateCounter(GuildDonateType type);

	/**
	*@brief ��ȡ���׻�õĹ��ṱ��
	*/
	UInt32 _GetDonateContribution(GuildDonateType type, UInt8 num);

	/**
	*@brief ��ȡ�̵�ID
	*/
	UInt8 _GetShopIDByLevel();

	/**
	*@brief ���͹����ʼ�
	*/
	void _BroadcastMail(const std::string& title, const std::string& sender, const std::string& content, const std::string& reason);

	/**
	*@brief ���͹��ά���ʼ�
	*/
	UInt32 _GetOrzMaxTime();

	/**
	*@brief ��ȡԲ�������������
	*/
	std::string _GetTableCounterName(UInt8 help);

	/**
	*@brief ��ȡԲ������������
	*/
	UInt32 _GetTableMaxTime(UInt8 help);

	/**
	*@brief ����һ���µĹ���᳤
	*/
	GuildMember* _GenNewLeader();

	/**
	*@brief ���¹����Ա��
	*/
	void UpdateMemberNum();

public:
	static GuildSkillDataEntry* FindSkillEntry(UInt16 skillId, UInt8 level);

public: //�����¼����

	/**
	 *@brief  ��ʼ�������¼��������Ĺ���ID
	 */
	inline void SetGuildEventGuildID() { m_GuildEventMgr.SetGuildID(GetID()); };

	/**
	 *@brief  ���ع����¼�
	 */
	inline void LoadGuildEvent(GuildEvent* guildEvent) { m_GuildEventMgr.LoadGuildEvent(guildEvent); }

	/**
	 *@brief  ���һ�������¼�
	 */
	inline bool AddGuildEvent(std::string data) { Chat(data); return m_GuildEventMgr.addGuildEvent(data); }

	/**
	 *@brief  ɾ�����й����¼�
	 */
	inline void DelGuildEvent() { m_GuildEventMgr.DelDBData(); }

	/**
	 *@brief  ��ȡ�����¼���������
	 */
	inline UInt16 GetGuildEventSize() { return m_GuildEventMgr.GetGuildEventSize(); }

	/**
	 *@brief  ��ȡ�����¼�
	 */
	inline void GetGuildEventPage(std::vector<GuildEvent>& guildEvents, UInt32 upTime) { m_GuildEventMgr.GetGuildEvent(guildEvents, upTime); };

private:
	// �����
	GuildDataEntry*				m_DataEntry;

	// ����
	ObjString					m_Name;

	// ����ȼ�
	ObjUInt8					m_Level;

	// �᳤
	GuildMember*				m_Leader;

	// �᳤����
	ObjString					m_LeaderName;

	// ��Ա�б�
	HashMap<ObjID_t, GuildMember*>	m_Members;

	// ��Ա����
	ObjUInt16					m_MemberNum;

	//����ʱ��
	ObjUInt32					m_SetupTime;

	// ����
	ObjString					m_Declaration;

	// ����
	ObjString					m_Announcement;

	// �ʲ�
	ObjUInt32					m_Fund;

	// ���Ὠ��
	GuildBuildingMgr			m_BuildingMgr;

	// ������־
	GuildDonateLogMgr			m_DonateLogMgr;

	// �����̵�ID
	ObjUInt8					m_ShopId;

	// �����̵�ˢ��ʱ��
	UInt32						m_ShopRefreshTime;

	// ������빫�����
	MapGuildRequesterInfo		m_Requesters;

	// ��ɢʱ��
	ObjUInt32					m_DismissTime;

	// �᳤����
	ObjUInt32					m_LeaderPopularoty;

	//���涨ʱ��
	Avalon::SimpleTimer			m_SaveTimer;

	//ͬ����Ϣ��ʱ��
	Avalon::SimpleTimer			m_SyncTimer;

	// Բ������
	GuildTable					m_Table;

	// ����ս���
	// ���������ID
	ObjUInt8					m_EnrollTerrId;
	// ����ս����
	ObjUInt32					m_GuildBattleScore;
	// �Ѿ�ռ������ID
	ObjUInt8					m_OccupyTerrId;
	// �Ѿ�ռ��Ŀ�����ID
	ObjUInt8					m_OccupyCrossTerrId;
	//�������
	ObjUInt8					m_Inspire;
	//����ս��Աս������
	UInt32						m_GuildMemberBattleCount;
	//����ս���輼��ID
	UInt16						m_InspireSkillId;

	//��ʷ���
	ObjUInt32					m_HistoryTerrId;

	//����սʤ���齱����
	ObjUInt8					m_WinProbability;
	//����սʧ�ܳ齱����
	ObjUInt8					m_LoseProbability;

	//�ֿ������ƷȨ��
	ObjUInt8					m_StorageAddPost;
	//�ֿ�ɾ����ƷȨ��
	ObjUInt8					m_StorageDelPost;

	//����ս�������id
	ObjUInt64					m_EnrollPlayerId;

	// ����ֿ�
	GuildStorage				m_guildStorage;
	// ���ȼ�
	ObjUInt32					m_JoinLevel;
	//����boss�Ѷ�
	ObjUInt32					m_GuildDungeonType;
	//���ᷱ�ٶ�״̬
	ObjUInt8                    m_ProsperityStatus;
	//���᱾�����ӵķ��ٶ�
	ObjUInt32                   m_WeekAddedFund;
	//�����������ӵķ��ٶ�
	ObjUInt32                   m_LastWeekAddedFund;
	//�沢��Ĺ��������״̬
	ObjUInt8                    m_MergerRequsetStatus;
	//�沢Ŀ��
	ObjUInt64                   m_MergerGoalGuildID;
	//�沢����ʱ��
	ObjUInt64                   m_MergerRequestTime;
	//�沢���󱻽��ܵ�
	ObjUInt64                   m_MergerRequestAccpetTime;
	//�ϴμ沢��Ĺ����ʱ��
	ObjUInt64                   m_MergerOtherGuildTime;
	//�յ��ı�Ĺ���ļ沢�����б�
	std::set<ObjID_t>           m_HavedMergerRequest;
	//�����¼�������
	GuildEventMgr				m_GuildEventMgr;

	// ��������
	GuildHonorMgr              m_GuildHonorMgr;

	//���Ὠ������޸�ʱ��
	ObjUInt32					m_LastUpdateGuildBuildingTime;
private:
	//ͬ���沢��Ϣ���������ٶ�״̬���Լ�������״̬
	void SyncMergerInfo();
	void SendMergerUDPLog(UInt32 result);
public:
	//��ȡ���ٶ�״̬
	inline UInt8 GetProsperity(){ return m_ProsperityStatus; }
	//���÷��ٶ�״̬
	inline void SetProsperity(UInt8 properity){ m_ProsperityStatus = properity; }
	//��ȡ�������ӵ��ʽ�����
	inline UInt32 GetWeekAddedFund(){ return m_WeekAddedFund; }
	//���ñ������ӵ��ʽ�����
	inline void SetWeekAddedFund(UInt32 fund){ m_WeekAddedFund = fund; }
	//�Ӽ沢�Լ����б����Ƴ���������
	void RemoveMergerRequestGuild(Guild * guild);
	//����沢��������
	UInt32 RequestMerger(Guild * guild);
	//ȡ���Լ�������
	UInt32 CancelMerger(Guild * guild);
	//��ȡ���Լ�����沢���б�
	std::set<ObjID_t>& GetMergerRequestedGuildSet();
	//���ܲ�������ļ沢����
	UInt32 AcceptMergerRequest(Guild * guild);
	//�ܾ���������ļ沢����
	UInt32 RefuseMergerRequest(Guild * guild);
	//ȡ���ѽ��ܵĲ�������ļ沢����
	UInt32 CancelMergerRequest(Guild * guild);
	//������м沢�Լ������󣬳����Ѿ����ܵ�
	UInt32 CleanAllMergerRequest(bool isSendMail = true);
	//��ȡ�沢Ŀ�깫��id
	inline UInt64 GetMergerGoalGuildID(){ return m_MergerGoalGuildID; }
	//��ȡ�Լ�������״̬
	inline UInt8 GetMergerRequestStatus(){ return m_MergerRequsetStatus; }
	//�ж��Լ��Ƿ���Լ沢
	bool CanMerger();
	//�ж��Լ���û���ѽ��ܵļ沢����
	bool ByAcceptedMerger();
	//�沢
	void OnMerger(Guild *byGuild);
	//����Լ������沢����
	void CleanSelfMergerRequest();
	//�Ѳ���������ӵ�����沢�Լ����б���
	void AddMergerRequestGuild(Guild * guild);
	//��ȡ�������ӵķ��ٶ�
	inline UInt32 GetLastWeekAddFund(){ return m_LastWeekAddedFund; }
	
	//������������
	void AddGuildHonorCnt(GuildHonorChallenge type,std::vector<UInt32>& conds){m_GuildHonorMgr.AddGuildHonorCnt(type,conds);}
	void GetGuildHonor(CLProtocol::WorldGuildHonorInfoRes& res, std::vector<UInt32>& groupIds){m_GuildHonorMgr.GetGuildHonor(res,groupIds); }
};


#endif //__GUILD_MEMBER_H__