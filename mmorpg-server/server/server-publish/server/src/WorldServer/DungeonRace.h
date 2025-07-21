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

#include "DungeonRoll.h"

enum DungeonRaceStatus
{
    DRS_INIT,           // ��ʼ״̬
    DRS_WAIT,           // �ȴ�������ʼ
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

enum BattleFlagType
{
	Eff_Devilddom_hidden_room = 7,
	// �Ƿ��ǵ���
	Eff_Single_race = 32
};

class WSPlayer;
struct DungeonMonsterGenerateContext;

// ����ͳ��key
struct DropKey
{
	DropKey(UInt32 itemId = 0, UInt8 equipType_ = 0) : itemTypeId(itemId), equipType(equipType_) {}

	bool operator<(const DropKey& other) const
	{
		if (itemTypeId != other.itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}

		return equipType < other.equipType;
	}
	// ����ID
	UInt32	itemTypeId;
	// װ������
	UInt8	equipType;
};

struct DropCount
{
	UInt64 times;
	UInt64 num;
};

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

	inline void SetTeamID(UInt32 id) { m_teamId = id; }
	inline UInt32 GetTeamID() const { return m_teamId; }

	inline void SetStatus(DungeonRaceStatus status) { m_status = status; }
	inline DungeonRaceStatus GetStatus() const { return m_status; }

	inline void SetDungeonId(UInt32 dungeonRaceId) { m_dungeonId = dungeonRaceId; }
	inline UInt32 GetDungeonID() const { return m_dungeonId; }

	inline void SetHellDungeonEntryID(UInt32 id) { m_hellDungeonEntryId = id; }

	inline void SetDungeonData(DungeonDataEntry* entry) { m_dungeonData = entry; }
	inline DungeonDataEntry* GetDungeonData() const { return m_dungeonData; }

	inline void SetPassedAreaIndex(UInt32 index) { m_passedAreaIndex = index; }

	inline bool IsHell() const { return m_isHellMode; }
	inline void SetHell() { m_isHellMode = true; }

	inline DungeonHellMode GetHellMode() const { return m_hellHardMode; }

	inline UInt32 GetStartAreaID() const { return m_startAreaId; }

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	inline ObjID_t GetSession() const { return m_sessionId; }

	inline UInt32 GetRelayServerID() const { return m_relayServerId; }

	inline UInt32 GetLastFrame() const { return m_lastFrame; }

	inline std::map<DropKey, DropCount>& GetDropSta() { return m_DropCount; };

	inline const SceneNpc& GetCityMonster() const { return m_CityMonster; }
	void SetCityMonster(const SceneNpc& cityMonster) { m_CityMonster = cityMonster; }

	void SetZjslDungeonInfo(const CLProtocol::ZjslDungeonInfo& info) { m_ZjslDungeonInfo = info; }

	/**
	*@brief ������
	*/
	inline const MapDungeonPlayer& GetPlayerList() const { return m_players; }
	void AddPlayer(const DungeonRacePlayerInfo& info);
	DungeonPlayer* FindDungeonPlayer(UInt64 roleId);

	/**
	*@brief �Ƿ����ģʽ�����ڵȼ�2����ң�
	*/
	bool IsTeamMode() const { return m_players.size() > 1; }

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
	UInt32 SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief ���õ��³Ƿ�������
	*/
	static UInt32 ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief �Ƿ����õ��³Ƿ�������
	*/
	static bool IsSetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief ����������³�
	*/
	void EndGuildDungeon();

	/**
	*@brief ����������³�
	*/
	void NotifyGuildDungeonBossOddBlood(UInt64 oddBlood, UInt64 totalBlood);

public:
	/* �����ز��� */
	
	/**
	*@brief ��ʼս�����������򣬹�����ؿͻ���...��
	*/
	UInt32 StartRace(ObjID_t sessionId, const Avalon::SockAddr& addr, UInt32 relayServerId);

	/**
	*@brief ��ҽ�����Ϸ
	*/
	bool OnPlayerEnterRace(WSPlayer* player);

	/**
	*@brief ����
	*/
	bool OnPlayerRaceEnd(WSPlayer* player, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame = 0, 
							UInt32 maxDamage = 0, UInt32 skillId = 0, UInt32 param = 0, UInt64 totalDamage = 0, UInt64 bossDamage = 0, 
							UInt32 playerRemainHp = 0, UInt32 playerRemainMp = 0, UInt32 boss1ID = 0, UInt32 boss2ID = 0, UInt32 boss1RemainHp = 0, 
							UInt32 boss2RemainHp = 0, UInt32 lastChecksum = 0);

	/**
	*@brief ���ɱ������
	*/
	void OnPlayerKilledMonster(WSPlayer* player, std::vector<UInt32>& monsterIds);

	/**
	*@brief ���ɱ�굱ǰ��������й���
	*/
	void OnPlayerClearAreaMonsters(WSPlayer* player, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame);

	/**
	*@brief ��ҽ����µ�����
	*/
	UInt32 OnPlayerEnterNextArea(WSPlayer* player, UInt32 areaId, UInt32 lastFrame, const DungeonStaticValue& staticVal);

	/**
	*@brief ���ʰȡ����
	*/
	void OnPlayerPickUpDropItems(WSPlayer* player, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief ������󿪱���
	*/
	bool OnPlayerOpenChest(WSPlayer* player, UInt8 pos);

	/**
	*@brief �������������
	*/
	bool OnPlayerRequestRaceEndDrop(WSPlayer* player, UInt8 multiple);

	/**
	*@brief ������󸴻���ң����Ը�����ˣ�
	*/
	UInt32 OnPlayerReviveRequest(WSPlayer* player, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum);

public:
	/**
	*@brief ����ϱ�֡�������У��
	*/
	UInt32 OnPlayerReportFrame(WSPlayer* player, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums);

public:
	/* ������� */

	/**
	*@brief ����򿪸��ѱ��������
	*/
	bool OnCheckOpenPayChestRet(WSPlayer* player, UInt8 pos);

	/**
	*@brief ����ӱ�������������
	*/
	bool OnCheckMultiRaceEndDropRet(WSPlayer* player, UInt8 multi);

	/**
	*@brief ����ӱ�������������
	*/
	UInt32 OnCheckReviveRet(WSPlayer* player, UInt32 reviveId, ObjID_t targetId);

public:
	/* ͳ�Ƶ���������� */

	/**
	*@brief ͳ�Ƶ���
	*/
	void GenDropStatistic(UInt32 times);

	/**
	*@brief ���õ���ͳ��״̬��������ر�ͳ�ƣ�
	*/
	void SetDropCountState(bool state) { m_DropCountState = state; }

	/**
	*@brief ��������д���ļ�
	*/
	void RecordDropCountToFile(WSPlayer* player, UInt32 dungenId, UInt8 isHell, UInt32 times);

public:
	/* ��֤��������� */

	/**
	*@brief ������֤���񷵻�
	*/
	void OnCreateVerifyReturn(UInt32 nodeid);

	/**
	*@brief ��֤�ɹ�
	*/
	void OnVerifySuccess(ObjID_t roleId);

private:
	/**
	*@brief �Ƿ�������֤
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief �Ƿ���Ҫ��֤
	*/
	bool _IsNeedVerify();

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
	*@brief ������Ԩ��
	*/
	bool _GenHellMonsters(DungeonMonsterGenerateContext& context);

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
	*@brief ���ɽ��㱦��
	*/
	void _GenChest(WSPlayer* player);

	/**
	*@brief ������Ԩ���ӷ���ID
	*/
	UInt32 _GenHellAreaID();

	/**
	*@brief ��ӽ���
	*/
	ItemReward _GenTeamReward(WSPlayer* player, LevelScore score);

	/**
	*@brief �ϱ��ع���佱��
	*/
	ItemReward _GenVeteranReturnDropReward(WSPlayer* player, LevelScore score);

	/**
	*@brief ������Ѿ���ӳ�ϵ��
	*/
	void _GenFriendExpRate();

	/**
	*@brief �����Ƿ񴥷����ط���
	*/
	void _GenTriggerHiddenRoom();
private:
	/* �ڲ�ʹ�õ�һЩ���� */

	/**
	*@brief ��ȡս����ʼ������
	*/
	void _GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res);

	/**
	*@brief ɱ��
	*/
	bool _OnPlayerKillMonster(WSPlayer* player, DungeonPlayer* dungeonPlayer, DungeonMonster* monster);

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
	void _PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt32 num, UInt8 strenth = 0, UInt8 equipType = 0);

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
	*@brief ���³ǲ�ͬ�Ѷ�����Ԩģʽ����������ӳ�
	*/
	UInt16 _HellModeDropAddition();

	/**
	*@brief �������ӱ����ģ��������UINT32_MAX�������ı���
	*/
	UInt32 _MultiRaceEndDropCost(UInt8 multi);

	/**
	*@brief ��ȡ��������
	*/
	UInt8 _GetRaceEndDropBaseMulti(DungeonPlayer* dungeonPlayer);

	/**
	*@brief ��ȡ��Ԩ����
	*/
	const std::vector<UInt32>&  _GetHellDropID(DungeonHellMode mode);

	/**
	*@brief �㲥��Ϣ
	*/
	void _BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ��ȡ����֮������
	*/
	UInt32 _GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId);

	/**
	*@brief ��ȡ���³Ƿ������
	*/
	UInt32 _GetIndexByAreaID(UInt32 areaId);

	/**
	*@brief ͨ�����³�ID��ȡ��ͨ�Ѷȵĵ��³�ID
	*/
	static UInt32 _GetNormalHardDungeonID(UInt32 dungeonId);

	/**
	*@brief ֪ͨrelay�����¼�
	*/
	void _NotifyRelayPlayerEvent(ObjID_t roleId, UInt8 type, ObjID_t param1, ObjID_t param2);

private:
	// �����ж�

	/**
	*@brief �����³�ʹ��ʱ��
	*/
	bool _CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime);

	/**
	*@brief ������׵Ĵ���
	*/
	void _OnPlayerCheat(WSPlayer* player, PlayerCheatReason cheatReason);

	/**
	*@brief ������˼��ٹ�
	*/
	void _OnPlayerSpeedUpCheat(WSPlayer* player);

private:
	/* ���㱦����� */

	/**
	*@brief ���ҽ��㱦����Ϣ
	*/
	DungeonChestTask* _FindDungeonChestTask(ObjID_t roleId);

	/**
	*@brief ����һ��û�д򿪵ı���λ��
	*/
	UInt8 _GenUnopenedChestPos();

	/**
	*@brief ����λ�û�ȡ��������
	*/
	DungeonChestType _GetChestTypeByPos(UInt8 pos);

	/**
	*@brief �򿪱���
	*/
	bool _OpenChest(WSPlayer* player, DungeonChestTask* task, DungeonChestType type, UInt8 pos);

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

	/**
	*@brief �����������
	*/
	void _RandMysticalMerchant();

	/**
	*@brief ����������������ܶ�
	*/
	void  checkAndAddIntimacy(UInt32 areaId);
	
	/**
	*@brief ����ʦͽ����log
	*/
	void SendMasterDiscipleInteractionLog();

	/**
	*@brief ������ط�����
	*/
	void _ClearPlayerHiddenRoomFlag();

	/**
	*@brief ����roll����
	*/
	void _GenRollDrops();

	/**
	*@brief roll���ɵ������(���ľ�����Ҹ���)
	*/
	UInt32 _GenRollDropNum();

	/**
	*@brief �Ƿ�Ӣ���³�
	*/
	bool  _IsEliteDungeon();

	/**
	*@brief ����rollitem
	*/
	DungeonRollItem* _FindRollItem(UInt8 index);

	/**
	*@brief ����roll����
	*/
	void _SettleRollDrops();

	/**
	*@brief ����һ��rool����
	*/
	void _SettleOneRollDrop(DungeonRollItem& rollItem);

	/**
	*@brief ��ȡΨһ����ս���
	*/
	bool _GetOnlyNotAssistPlayer(ObjID_t& playerId);

	/**
	*@brief ����ģʽ����roll����
	*/
	void _SettleRollSingleMode();

	/**
	*@brief ����Ƿ���Խ���roll
	*/
	bool _CheckCanSettleRollDrop();
public:
	/*
	*@brief ����roll����
	*/
	UInt32 HandleOneRollReq(WSPlayer* player, UInt8  dropIndex, UInt8 opType, UInt32& rollPoint);

private:
	friend class DungeonRaceMgr;
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

	// ��Ԩ���ID
	UInt32					m_hellDungeonEntryId;

	// Ŀ����³�ID
	UInt32                  m_dungeonId;

	// Ŀ����³�����
	DungeonDataEntry*       m_dungeonData;

	// ��ʼ����
	UInt32                  m_startAreaId;

	// ���area֮ǰ��area�Ͳ���Ҫ�����ˣ��������area��
	UInt32					m_passedAreaIndex;

	// �Ƿ�����Ԩģʽ
	bool                    m_isHellMode;

	// ��Ԩģʽ�����Ѷ�
	DungeonHellMode         m_hellHardMode;

	// ������ID����
	UInt32					m_dropItemSeed;

	// ��������ͳ��״̬
	bool					m_DropCountState;

	// ��������ͳ��
	std::map<DropKey, DropCount> m_DropCount;

	// �������ݻ�ʱ��
	Avalon::Time            m_destoryTime;

	// �Ѿ����˵ı������루��һλ�ô���ڼ��������Ѿ����ˣ�
	UInt32                  m_openedChestMask;

	// ������Ϣ
	VDungeonChestTask       m_chestTasks;

	// �Ѿ�ִ�й��ĸ�������
	std::set<UInt32>		m_reviveTasks;

	// ��Ԩ�������ڵķ���ID
	UInt32					m_hellAreaId;

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

	// ���еĵ�����ID
	std::vector<UInt32>			m_allDropSet;

	// ��ʼ��������ʱ����������ڿ��ŵĻ�б�֮��Ͳ����¼����ˣ�Ϊ��Ч�ʣ�
	std::set<UInt32>			m_OpenedActivityList;

	// ��������id
	UInt32						m_MysticalMerchantId;

	// �������ID
	SceneNpc					m_CityMonster;

	// �Ƿ��Ѿ�����ս��������Ϣ
	bool						m_HasSendRaceOver;

	// �Ƿ��͹�ʦ�����log
	bool						m_bSendMasterDiscipleInteractionLog;
	
	// �Ƿ��Ѿ���¼ͨ�ش���(������ƴ���)
	bool						m_flagGuildDungeon;
	// �Ƿ񴥷�������ط���
	bool						m_TriggerHiddenRoom;

	// �ռ�������Ϣ
	CLProtocol::ZjslDungeonInfo	m_ZjslDungeonInfo;

	// �Ƿ����ɹ�������
	bool						m_isGeneratePublishDrops;

	// ��������
	std::map<UInt8, DungeonRollItem>	m_rollItems;

	// ���������Ƿ���ģʽ
	bool		m_isSingleRoll;

	// ���������Ƿ��Ѿ�����
	bool		m_isRollSettled;

	// �����������ʱ��
	UInt32           m_rollEndTime;
};

#endif
