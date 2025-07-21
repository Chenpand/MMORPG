#ifndef __DUNGEON_RACE_H__
#define __DUNGEON_RACE_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <CLDungeonDataEntry.h>
#include <CLDungeonDefine.h>
#include <CLDungeonProtocol.h>
#include "DungeonArea.h"
#include "DungeonPlayer.h"
#include "DungeonChestTask.h"
#include <CLItemDefine.h>
#include <CLDungeonPrefixLimitDataEntry.h>
#include <CLSortListDefine.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <CLCheatDefine.h>
#include <CLGameSessionProtocol.h>

enum DungeonRaceStatus
{
    DRS_INIT,           // ��ʼ״̬
	DRS_CREATING,		// ������
    DRS_RACING,         // ������
    DRS_RACE_END,       // ������
    DRS_WAIT_CLOSE,     // �ȴ��ر�
};

enum DungeonRaceTransactionType
{
	DUNGEON_RACE_OPEN_PAY_CHEST,
	DUNGEON_RACE_MULTI_RACE_END_DROP,
	DUNGEON_RACE_REVIVE,
};

struct DungeonMonsterGenerateContext;


class DungeonRace
{
    AVALON_DEFINE_OBJECTPOOL(DungeonRace, Avalon::NullMutex)
public:
    DungeonRace();
    ~DungeonRace();

    void Tick(const Avalon::Time& now);

public:
	/**
	*@brief ����Get/Set����
	*/

	inline void SetID(UInt64 id) { m_id = id; }
	inline UInt64 GetID() const { return m_id; }

	inline void SetSessionId(ObjID_t session) { m_sessionId = session; }
	inline ObjID_t GetSessionId() { return m_sessionId; }

	inline void SetServiceId(UInt32 serviceId) { m_serviceId = serviceId; };
	inline UInt32 GetServiceId() { return m_serviceId; }

	inline void SetTeamID(UInt32 id) { m_teamId = id; }
	inline UInt32 GetTeamID() const { return m_teamId; }

	inline void SetStatus(DungeonRaceStatus status) { m_status = status; }
	inline DungeonRaceStatus GetStatus() const { return m_status; }

	inline void SetDungeonId(UInt32 dungeonRaceId) { m_dungeonId = dungeonRaceId; }
	inline UInt32 GetDungeonID() const { return m_dungeonId; }

	inline void SetDungeonData(DungeonDataEntry* entry) { m_dungeonData = entry; }
	inline DungeonDataEntry* GetDungeonData() const { return m_dungeonData; }

	inline void SetPassedAreaIndex(UInt32 index) { m_passedAreaIndex = index; }
	UInt32 GetPassedAreaIndex() { return m_passedAreaIndex; }

	inline UInt32 GetStartAreaID() const { return m_startAreaId; }

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	inline ObjID_t GetSession() const { return m_sessionId; }

	void SetRelayServerID(UInt32 id) { m_relayServerId = id; }
	inline UInt32 GetRelayServerID() const { return m_relayServerId; }

	void SetIsSettled(bool isSettled) { m_isSettled = isSettled; };
	bool IsSettled() { return m_isSettled; }

	void SetClearedDungeonIds(std::vector<UInt32>& clearedDungeonIds) { m_clearedDungeonIds = clearedDungeonIds; };
	std::vector<UInt32>& GetClearedDungeonIds() { return m_clearedDungeonIds; };

	void OnDestory();

	inline UInt32 GetLastFrame() const { return m_lastFrame; }

	void SetPlayerInfos(std::vector<DungeonRacePlayerInfo>& playerInfos) { m_playerInfoes = playerInfos; }
	std::vector<DungeonRacePlayerInfo>& GetDungeonRacePlayerInfo() { return m_playerInfoes; }

	void SetFighters(std::vector<CLProtocol::DungeonFighter>& fighters) { m_fighters = fighters; }
	const std::vector<CLProtocol::DungeonFighter>& GetFighters() const { return m_fighters; }
	bool GetFighter(ObjID_t roleId, CLProtocol::DungeonFighter& fighter);

	inline void SetAddTeamReviveCount(UInt32 cnt) { m_AddTeamReviveCount = cnt; }
	inline UInt32 GetAddTeamReviveCount() { return m_AddTeamReviveCount; }

	inline void IncTeamReviveCount() { m_teamReviveCount++; }
	inline UInt32 GetTeamReviveCount() { return m_teamReviveCount; }

	inline const SceneNpc& GetMonster() const { return m_monster; }
	void SetMonster(const SceneNpc& monster) { m_monster = monster; }

	inline UInt64 GetBattleTempRaceId() const { return m_battleTempRaceId; }
	void SetBattleTempRaceId(UInt64 tempRaceId) { m_battleTempRaceId = tempRaceId; }
	
	void SetSourceSceneNodeId(UInt32 nodeId) { m_sourceSceneNodeId = nodeId; };
	UInt32	GetSourceSceneNodeId() { return m_sourceSceneNodeId; }
	/**
	*@brief ������
	*/
	inline const MapDungeonPlayer& GetPlayerList() const { return m_players; }
	void AddPlayer(const DungeonRacePlayerInfo& info);
	DungeonPlayer* FindDungeonPlayer(UInt64 roleId);

	/**
	*@brief �Ƿ����ģʽ�����ڵȼ�2����ң�
	*/
	bool IsTeamMode() const { return m_players.size() > 0; }

	/**
	*@brief ����뿪����
	*/
	void OnPlayerLeave(UInt64 roleId, bool isDisconnect);

	/**
	*@brief �ǲ���������Ҷ��뿪������
	*/
	bool IsAllPlayerLeave();

	/**
	*@brief ������־
	*/
	void SendDungeonLogWhenAllPlayerLeave();

	/**
	*@brief �Ƿ�����Ҷ���
	*/
	bool HasPlayerDisconnect();

	/**
	*@brief ��ȡArea��Ϣ
	*/
	void GetAreaInfo(UInt64 roleId, std::vector<CLProtocol::SceneDungeonArea>& areas);

	/**
	*@brief ��ȡ�����������Ϣ
	*/
	void GetAreaDropInfo(UInt64 roleId, UInt32 areaId, CLProtocol::SceneDungeonEnterNextAreaRes& res);

	/**
	*@brief ��ȡ��Ԩ��Ϣ
	*/
	void GetHellInfo(UInt64 roleId, CLProtocol::DungeonHellInfo& info);

	/**
	*@brief �Ƿ���Ҫ���ݻ�
	*/
	bool IsTimeToDestory(const Avalon::Time& now);

	/**
	*@brief ����Ƿ��Ѿ������
	*/
	bool IsPlayerRaceEnd(ObjID_t id);

	/**
	*@brief ��ȡ���³Ƿ�������
	*/
	static UInt32 GetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief ���õ��³Ƿ�����������λ�洢������������ʼΪ1
	*/
	static UInt32 SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief ���õ��³Ƿ�������
	*/
	static UInt32 ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief �Ƿ����õ��³Ƿ�������
	*/
	static bool IsSetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

public:
	/* �����ز��� */
	
	/**
	*@brief ��ʼս�����������򣬹�����ؿͻ���...��
	*/
	UInt32 StartRace(ObjID_t sessionId, const Avalon::SockAddr& addr, UInt32 relayServerId);

	/*
	*@brief ֪ͨ��ҿ�ʼս������
	*/
	void NotifyPlayerStartDunegonRes();

	/**
	*@brief ��ҽ�����Ϸ
	*/
	bool OnPlayerEnterRace(DungeonPlayer* dungeonPlayer);

	/**
	*@brief ����
	*/
	bool OnPlayerRaceEnd(DungeonPlayer* player, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame = 0, UInt32 maxDamage = 0, UInt32 skillId = 0, UInt32 param = 0, UInt64 totalDamage = 0, UInt64 bossDamage = 0);

	/**
	*@brief �������
	*/
	bool OnRaceEnd(UInt8 reason, DungeonRaceEndInfo& raceEndInfo);

	/**
	*@brief ���ɱ������
	*/
	void OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<UInt32>& monsterIds);
	void OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<DungeonKilledMonsterInfo>& monsters);

	/**
	*@brief ���ɱ�굱ǰ��������й���
	*/
	void OnPlayerClearAreaMonsters(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame);

	/**
	*@brief ��ҽ����µ�����
	*/
	UInt32 OnPlayerEnterNextArea(DungeonPlayer* dungeonPlayer, UInt32 areaId);

	/**
	*@brief ���ʰȡ����
	*/
	void OnPlayerPickUpDropItems(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief �������������
	*/
	bool OnPlayerRequestRaceEndDrop(DungeonPlayer* dungeonPlayer, UInt8 multiple);

	/**
	*@brief ������󸴻���ң����Ը�����ˣ�
	*/
	UInt32 OnPlayerReviveRequest(DungeonPlayer* dungeonPlayer, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum,
		UInt32  reviveItem, UInt32 reviveItemNum, UInt32 reviveHp, UInt32 reviveMp);

public:
	/**
	*@brief ����ϱ�֡�������У��
	*/
	UInt32 OnPlayerReportFrame(DungeonPlayer* dungeonPlayer, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums);

public:
	/* ������� */

	/**
	*@brief ����
	*/
	UInt32 OnCheckReviveRet(DungeonPlayer* player, UInt32 reviveId, ObjID_t targetId, UInt32 hp, UInt32 mp);

public:
	/* ��֤��������� */

	/**
	*@brief ������֤���񷵻�
	*/
	void OnCreateVerifyReturn(UInt32 nodeid);

private:
	/**
	*@brief �Ƿ�������֤
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief ��ʼ��֤
	*/
	void _StartVerify();

	/**
	*@brief ������Ϣ����֤������
	*/
	void _SendToVerifyServer(Avalon::Protocol& protocol);

private:
	/* �ؿ������������ */

	/**
	*@brief �����������򣨹������ȣ�
	*/
	bool _GenAreas();

	/**
	*@brief ������ʼ����
	*/
	bool _GenStartArea();

	/**
	*@brief ���㵥�ֹ���
	*/
	void _GenMonsters(DungeonMonsterGenerateContext& context, DungeonArea& area);

	/**
	*@brief ����BOSS����
	*/
	bool _GenBossDropItems();

	/**
	*@brief ��������׺��Ϣ
	*/
	void _GenMonsterPrefix();

	/**
	*@brief ���������
	*/
	void _GenDropItems(DungeonMonster& monster);

	/**
	*@brief ���������
	*/
	std::vector<PlayerDropItem> _GenDropItemsByDropSet(DungeonPlayer* player, std::vector<UInt32> dropSetIds, UInt32 monsterId, UInt32 monsterTypeId);

	/**
	*@brief ����ͨ�õ��䷽��ID
	*/
	UInt32 _GetCommonDropSetID(MonsterType type);

	/**
	*@brief ���ɵ���ID
	*/
	UInt32 _GenDropItemID();

	/**
	*@brief ��ӽ���
	*/
	ItemReward _GenTeamReward(DungeonPlayer* player, LevelScore score);

private:
	/* �ڲ�ʹ�õ�һЩ���� */

	/**
	*@brief ��ȡս����ʼ������
	*/
	void _GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res);

	/**
	*@brief ɱ��
	*/
	bool _OnPlayerKillMonster(DungeonPlayer* dungeonPlayer, DungeonMonster* monster);

	/**
	*@brief ���ҵ�����
	*/
	PlayerDropItem* _FindDropItem(UInt32 itemId, DungeonPlayer* player = NULL, bool mustEnteredArea = 0);

	/**
	*@brief �����Ʒ��ǿ���ȼ�
	*/
	UInt8 _RandomItemStrenth(UInt32 itemId);

	/**
	*@brief ����Ʒ�ŵ�ItemReward�����У�������֮ǰ����Ʒ����
	*/
	void _PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt16 num, UInt8 strenth = 0, UInt8 equipType = 0);

	/**
	*@brief ��ȡ���㾭�齱��
	*/
	UInt32 _GetRaceEndExpReward(DungeonPlayer* player, LevelScore score, DungeonAdditionInfo& addition);

	/**
	*@brief ��ȡ���³ǵĽ��㾭��
	*/
	UInt32 _GetDungeonRaceEndExp(UInt32 dungeonId, UInt16 level);

	/**
	*@brief ��������ȼ���0������Ҫ���䣩
	*/
	UInt16 _GetAdapterLevel();

	/**
	*@brief ����areaId��ȡarea
	*/
	DungeonArea* _FindArea(UInt32 areaId);

	/**
	*@brief ���ҹ���
	*/
	DungeonMonster* _FindMonster(UInt32 id);

	/**
	*@brief ����ؿ��Ĵ�׺��������Ϣ
	*/
	DungeonPrefixLimitDataEntry* _GetPrefixMonsterLimit(DungeonHardType type, UInt32 level);

	/**
	*@brief ���㵥������Ĵ�׺���������
	*/
	UInt8 _GetPerAreaPrefixMonsterMaxNum(UInt32 totalNum);

	/**
	*@brief �������ӱ����ģ��������UINT32_MAX�������ı���
	*/
	UInt32 _MultiRaceEndDropCost(UInt8 multi);

	/**
	*@brief ��ȡ��������
	*/
	UInt8 _GetRaceEndDropBaseMulti(DungeonPlayer* dungeonPlayer);

	/**
	*@brief �㲥��Ϣ
	*/
	void _BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ��ȡ���³Ƿ������
	*/
	UInt32 _GetIndexByAreaID(UInt32 areaId);

	/**
	*@brief ͨ�����³�ID��ȡ��ͨ�Ѷȵĵ��³�ID
	*/
	static UInt32 _GetNormalHardDungeonID(UInt32 dungeonId);

	/**
	*@brief �����ɱ�˹֣�����û�з�ʰȡ�ĵ��߷������
	*/
	void _GiveOnPickDropItem(DungeonPlayer* dungeonPlayer);

private:
	// �����ж�

	/**
	*@brief ���ɱ�ֵ��˺���Ϣ�Ƿ�Ϸ�
	*/
	bool _CheckMonsterDamage(DungeonPlayer* dungeonPlayer, DungeonMonster* monster, const DungeonKilledMonsterInfo& info);


	/**
	*@brief �����³�ʹ��ʱ��
	*/
	bool _CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime);

	/**
	*@brief ������׵Ĵ���
	*/
	void _OnPlayerCheat(DungeonPlayer* dungeonPlayer, PlayerCheatReason cheatReason);

private:
	/**
	*@brief ���ʰȡ���������md5
	*/
	bool _CheckDropRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, const std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief �����ͼ�����md5
	*/
	bool _CheckClearAreaRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16]);

	/**
	*@brief �����������md5
	*/
	bool _CheckRaceEndRequestMd5(DungeonPlayer* dungeonPlayer, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16]);

	/**
	*@brief ����һ�����ܵ���Կ
	*/
	void _GenEncryptKey(CLProtocol::SceneDungeonStartRes& res);

public:
	bool SendToService(Avalon::Protocol& protocol);

	bool SendToRelay(Avalon::Protocol& protocol);
private:
	// ID
	UInt64                  m_id;

	// ����ID
	UInt32					m_teamId;

	// ��ʼʱ��
	UInt64					m_startTime;

	// RelayServerID
	UInt32					m_relayServerId;

	// Session
	ObjID_t					m_sessionId;

	// ״̬
	DungeonRaceStatus       m_status;

	// ���³�����
	VDungeonArea            m_areas;

	// �������
	MapDungeonPlayer        m_players;

	// Ŀ����³�ID
	UInt32                  m_dungeonId;

	// Ŀ����³�����
	DungeonDataEntry*       m_dungeonData;

	// ��ʼ����
	UInt32                  m_startAreaId;

	// ���area֮ǰ��area�Ͳ���Ҫ�����ˣ��������area��
	UInt32					m_passedAreaIndex;

	// ������ID����
	UInt32					m_dropItemSeed;

	// �������ݻ�ʱ��
	Avalon::Time            m_destoryTime;

	// �Ѿ�ִ�й��ĸ�������
	std::set<UInt32>		m_reviveTasks;

	// ������Կ
	UInt32					m_encryptKey;

	// �յ������һ֡
	UInt32					m_lastFrame;

	// ����֡
	std::vector<CLProtocol::Frame>	m_frames;

	// ���������
	std::vector<CLProtocol::FrameChecksum>	m_checksums;

	// ��֤�������ڵ�
	UInt32					m_VSId;

	// �Ƿ��Ѿ�����ս��������Ϣ
	bool						m_HasSendRaceOver;
	
	// �Ƿ��Ѿ���¼ͨ�ش���(������ƴ���)
	bool						m_flagGuildDungeon;

	// �淨����������id
	UInt32						m_serviceId;

	// �����Ϣ
	std::vector<DungeonRacePlayerInfo>	m_playerInfoes;

	// �Ƿ�����
	bool						m_isSettled;

	// �Ѿ�ͨ�����³�id
	std::vector<UInt32>         m_clearedDungeonIds;

	// fighter����
	std::vector<CLProtocol::DungeonFighter>	m_fighters;

	// ���ӵĿɸ������
	UInt32					m_AddTeamReviveCount;

	// �������
	UInt32					m_teamReviveCount;	

	// ս��С��
	SceneNpc					m_monster;

	// ս�����ɵ���ʱraceId
	UInt64				m_battleTempRaceId;

	// Դscene node id
	UInt64				m_sourceSceneNodeId;
};

#endif
