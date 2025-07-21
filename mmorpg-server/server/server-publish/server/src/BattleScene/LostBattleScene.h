#ifndef _LOST_BATTLE_SCENE_H_
#define _LOST_BATTLE_SCENE_H_

#include "Scene.h"
#include <functional>
#include "DungeonEntrySceneDynamic.h"

/**
*@brief ��ʧ����ս������
*/
class LostBattleScene : public Scene
{
	AVALON_DEFINE_OBJECTPOOL(LostBattleScene, Avalon::NullMutex)

public:
	LostBattleScene();
	~LostBattleScene();

public:
	bool Load(UInt32 mapid, const std::string& config);
	UInt8 GetType() const { return SCENE_TYPE_LOST_DUNGEON; }

	//ս��id
	inline void SetBattleId(UInt32 battleId) { m_battleId = battleId; }
	inline UInt32	GetBattleId() { return m_battleId; }
	inline void SetBattleDataId(UInt32 battleDataId) { m_battleDataId = battleDataId; }
	UInt32	GetBattleDataId() { return m_battleDataId; }
	inline void SetFloor(UInt32 floor) { m_floor = floor; }
	inline UInt32 GetFloor() { return m_floor; }

	void Init();

	void Destroy(UInt8 reason);

	virtual void KickPlayersOut();
	virtual void KickPlayerOut(Player* player);
	virtual void OnKickPlayerTimeout();

	UInt64 GenResourceId();

	void GenMonsters();

	void GenResources();

	void GenDugeonEntrySceneDynamics();

	void DelAllDungeonEntrySceneDynamics();

	bool VistResourceItems(const std::function<bool(SceneItem*)>& ff);

	SceneItem* FindResourceItem(UInt64 guid);

	void RemoveResourceItem(UInt64 guid);

	UInt32 OnPlayerPickUpItem(Player* player, UInt64 guid);

	const ScenePos* RandPosition();

	virtual void OnPlayerEnter(Player* player);

	virtual void OnTick(const Avalon::Time& now);

	virtual void  OnMonsterKilled(CityMonster* monster);

	/**
	 *@brief �����˳�
	 */
	virtual void OnSceneQuit();

	void BroadcastToDungeonEntrys(Avalon::Protocol& protocol);
public:	// �����������

	// ������ȡ����
	void AddTaskAcceptNum(UInt32 taskId);
	UInt32 GetTaskAcceptNum(UInt32 taskId);
	void RemoveTaskAcceptNum(UInt32 taskId);

	// ˢ�»�ɱָ������������
	void RefreshFixNumTask();

	// ��ȡ����ս������
	virtual UInt32 BattlePlayerNum() { return m_PlayerSet.size(); }
	// ������һ�����
	UInt64 RandGetPlayer(UInt64 exceptId);
	// �����ȡ������
	std::set<UInt64>& RandGetPlayerList();

	void OnPlayerBeforeEnterBattle(Player* player);
	//��ҽ���ս��
	void OnPlayerEnterBattle(Player* player);
	//����뿪ս��
	void OnPlayerLeaveBattle(Player* player, UInt8 result);
	//�Ƿ�ս������
	bool IsBattleExitDoor(UInt32 doorId);
	//���ս���ڸ���
	void OnPlayerRevive(ObjID_t playerId, ObjID_t targetId, UInt32 reviveId, UInt32 reviveItem, UInt32 reviveItemNum);
	//�����ӳٹر�
	void SetDelayClose(bool delayClose) { m_IsDelayClosing = delayClose; }
private:
	//ս��id(battleServer����)
	UInt32	m_battleId;
	//ս����id
	UInt32  m_battleDataId;

	UInt32	m_floor;

	//���͵�
	std::map<UInt32, std::vector<ScenePos>>		m_TransferNodes;
	
	//������ȡ����
	std::map<UInt32, UInt32>	m_TaskAcceptMap;

	//���
	std::set<UInt64>	m_PlayerSet;

	//��Դ������
	std::map<UInt32, std::vector<ScenePos>>		m_ResourceNodes;
	std::vector<ScenePos>						m_ResourceVec;

	std::vector<SceneItem*>						m_AllItems;

	UInt64 m_GuidGen;

	// �����ս�������Ӻ������ʱ��
	Avalon::SimpleTimer m_DestroyDelayTimer;
	// �����ս�����˱��׽�����ʱ��
	Avalon::SimpleTimer m_DestroyDelayGuardTimer;

	//�Ƿ��ӳٹر�
	bool  m_IsDelayClosing;
	bool  m_IsGuardDelayClosing;

	// �Ƿ���ѡɱָ������������
	bool m_IsRefreshKillFixNumTask;
	// ��ɱָ������������
	Avalon::SimpleTimer m_KillFixNumTimer;

	//�����ڴ����Ķ�̬���³���ڳ���
	std::set<DungeonEntrySceneDynamicKey> m_DungeonEntrySceneDynamics;

};

#endif
