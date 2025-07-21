#ifndef __BATTLE_SCENE_H__
#define __BATTLE_SCENE_H__

#include "Scene.h"
#include <CLSyncObjectProtocol.h>


struct BattleNpcInfo
{
	BattleNpcInfo() : npcGuid(0), npcType(0), npcId(0), beginTime(0), pos(NULL) {}
	UInt64 npcGuid;
	UInt32 npcType;
	UInt32 npcId;
	UInt32 beginTime;
	const ScenePos* pos;
};

class Player;

class BattlePlayer
{
public:
	BattlePlayer();
	~BattlePlayer();

	/**
	*@brief ���û�ȡ״̬
	*/
	void SetStatus(BattlePlayerState status) { m_playerStatus = status; }
	BattlePlayerState GetStatus(void) { return m_playerStatus; }

public:
	UInt64 m_playerID;
	UInt64 m_killerID;
	std::vector<DetailItem> m_items;
	UInt32 m_protectTimestamp;
	//����״̬
	BattlePlayerState m_playerStatus;
	//���ڿ��ı���GUID
	UInt64 m_openingBoxGuid;
	UInt32 m_openingTimestamp;
	//��ʱʱ���������Լ���ʱ
	UInt32 m_enterTimeoutTimestamp;
};

class BattleInfo
{
public:
	BattleInfo();
	~BattleInfo();

public:
	UInt32 m_battleID;
	UInt32 m_waveID;
	//ˢ����ʼȦ
	ScenePos m_blueCenterPoint;
	UInt32 m_blueRadius;
	//ˢ��Ŀ��Ȧ
	ScenePos m_whiteCenterPoint;
	UInt32 m_whiteRadius;
	//��һ��ˢ��Ŀ��Ȧ
	ScenePos m_nextWhiteCenterPoint;
	UInt32 m_nextWhiteRadius;
	//ˢ����ʼʱ��ͳ־�ʱ��
	UInt32 m_poisonInterval;
	UInt32 m_poisonBegin;
	UInt32 m_deltaHPPerSec;
};

/**
*@brief ս������
*/
class BattleScene : public Scene
{
	AVALON_DEFINE_OBJECTPOOL(BattleScene, Avalon::NullMutex)

public:
	BattleScene();
	~BattleScene();

	/**
	*@brief �߳����
	*/
	virtual void KickPlayersOut();
	virtual void KickPlayerOut(Player* player);
	virtual void OnKickPlayerTimeout();

public:
	bool Load(UInt32 mapid, const std::string& config);

	UInt8 GetType() const { return SCENE_TYPE_BATTLE; }

public://�¼�
	/**
	*@brief tick
	*/
	virtual void OnTick(const Avalon::Time& now);

public:
	void ClearItems();

	void CreateItems(UInt32 battleID, UInt32 waveID);

	void DestroyBattle(UInt32 battleID);

	void InitBattle(UInt32 battleID, std::vector<UInt64>& playerIDs);

	void GetSceneItemList(UInt32 battleID, UInt64 playerID, CLProtocol::SceneItemList& itemList);

	SceneItem* GetSceneItemByGuid(UInt32 battleID, UInt64 playerID, UInt64 itemID);

	void DeleteSceneItemByGuid(UInt32 battleID, UInt64 itemID);

	bool BeInBattle(UInt32 battleID, UInt64 playerID);

	void KillSomeone(UInt32 battleID, UInt64 playerID, UInt64 killerID, Int32 winnerRemainHp, Int32 winnerRemainMp, UInt64 otherID);

	void OnWaveNotify(UInt32 battleID, UInt32 waveID);

	void OnShopWaveNotify(UInt32 battleID, UInt32 shopWave);

	BattlePlayer* GetBattlePlayer(UInt32 battleID, UInt64 playerID);

	const ScenePos* RandPositionByRegionID(UInt32 regionID);

	void NotifyMsgToAllPlayerByBattleID(UInt32 battleID, Avalon::Protocol& protocol, bool includeDead = false);

	void NotifyBattleSomeoneDead(UInt32 battleID, UInt64 playerID, UInt64 killerID = 0, UInt32 reason = 0);

	void CheckPlayerInPoisonRing(UInt32 battleID, UInt64 playerID, const ScenePos& pos);

	void SyncAllDataToPlayer(UInt32 battleID, UInt64 playerID);

	void LeaveBattle(UInt32 battleID, UInt64 playerID);

	void UpdatePlayerItemDataVector(UInt32 battleID, UInt64 playerID, std::vector<DetailItem>& itemDatas);

	void CreateNpc(UInt32 battleID, UInt32 waveID, UInt32 npcSubType);

	const BattleNpcInfo* GetBattleNpcInfo(UInt32 battleID, UInt64 npcGuid);

	void RemoveNpc(UInt32 battleID, UInt64 npcGuid, bool isNotify = false);

	void AddSceneItem(UInt32 battleID, SceneItem* item);

	void OnOpenBoxBeInterrupted(UInt32 battleID, UInt64 playerID);

private:

	// ��ð�Ȧ�ڵ���Դ��
	const ScenePos* GetWhiteCirclePoint(UInt32 battleID, bool isHasNpc = false);
	// ���Npc
	void AddNpc(UInt32 battleID, BattleNpcInfo& npcInfo);
	// ��λ���Ƿ���Npc
	bool PosIsHasNpc(const ScenePos* pos);


private:
	std::map<UInt32, std::vector<SceneItem*>>	m_AllItems;
	std::map<UInt32, std::vector<BattlePlayer>>	m_AllPlayers;
	std::map<UInt32, BattleInfo>				m_AllBattles;
	std::map<UInt32, std::vector<ScenePos>>		m_ResourceNodes;
	std::vector<ScenePos>						m_ResourceVec;

	std::map<UInt32, std::vector<ScenePos>>		m_TransferNodes;
	UInt64 m_GuidGen;

	// npc��Ϣ��key->battleID
	std::map<UInt32, std::list<BattleNpcInfo>>	m_NpcInfos;

	// Tick��ʱ����500ms
	Avalon::SimpleTimer							m_TickTimer500ms;
};


#endif