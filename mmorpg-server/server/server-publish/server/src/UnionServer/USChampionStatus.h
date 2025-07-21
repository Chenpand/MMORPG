#ifndef __US_CHAMPION_RACE_H__
#define __US_CHAMPION_RACE_H__


#include "ChampionPlayer.h"
#include <CLGameSessionDefine.h>
#include "USPlayerMgr.h"
#include "USceneMgr.h"
#include "ChampionService.h"

/**
*@brief �ھ�������Ϣ����
*/
class ChampionGroupInfoMgr : public Avalon::Singleton<ChampionGroupInfoMgr>
{
public:
	/**
	*@brief ��ʼ��
	*/
	bool Init();
	/**
	*@brief �����ڲ���Ϣ����㲥����Ϣ
	*/
	void Clear();
	/**
	*@brief ѹ��һ����Ϣ���㲥����
	*/
	void Push(ChampionGroupStatus& groupInfo);
	/**
	*@brief �ı�ĳ��״̬���㲥����
	*/
	void ChangeStatus(UInt32 groupID, UInt8 status);
	/**
	*@brief ��ȷָı�ʱ�����㲥������Ϣ
	*/
	void OnChanged(UInt32 groupID, ChampionPlayer* roleA, ChampionPlayer* roleB);
	/**
	*@brief ͬ�����������Ϣ
	*/
	void SyncAllGroupInfo();
	/**
	*@brief ͬ��һ����Ϣ
	*/
	void SyncOneGroupInfo(UInt32 groupID);
	/**
	*@brief ��ȡһ����Ϣ
	*/
	ChampionGroupStatus& GetGroupInfo(UInt32 groupID);
private:
	//��id->����Ϣ�������ͻ�����
	std::map<UInt32, ChampionGroupStatus> m_GroupStatus;
};

class ChampionGroup
{
public:
	/**
	*@brief ��ȡ������id
	*/
	inline const UInt32 GetID() const { return m_GroupID; }
	inline void SetID(UInt32 id) { m_GroupID = id; }
	/**
	*@brief �Ѹý�ɫѹ�뱾��
	*/
	void Push(ChampionPlayer* player);
	/**
	*@brief ѹ���µı���ս����¼�������ڴ��ﲻ����
	*/
	void PushRecord(ChampionScoreBattleRecord& record);
	/**
	*@brief ���ұ���ĳ��ɫ
	*/
	ChampionPlayer* Find(UInt64 roleId);
	/**
	*@brief ��ȡ��������ǰtop�Ľ�ɫ
	*/
	std::vector<ChampionPlayer*> GetTopScorePlayer(UInt32 top);
	/**
	*@brief �޸ı������н�ɫ����δʵ��
	*/
	void ModifyAllPlayer(std::function<void(ChampionPlayer*)> fun);
	/**
	*@brief ���鿪ʼս��
	*/
	void StartBattle(ChampionStatus status);
	/**
	*@brief ��ȡ�������н�ɫ
	*/
	std::vector<ChampionPlayer*>& GetPlayerVec() { return m_PlayerVec; }
	/**
	*@brief ��ȡ����ý�ɫ�Ķ��֣���δʹ��
	*/
	ChampionPlayer* GetEnemyPlayer(UInt64 roleId);
	/**
	*@brief �����ɫ�����ı�
	*/
	void OnPlayerScoreChanged(ChampionPlayer* player);
	/**
	*@brief ��ȡ���鵱ǰ��������
	*/
	const std::list<ChampionPlayer*>& GetScoreRankList() const;
	/**
	*@brief ��ȡ���鵱ǰ������ս����¼
	*/
	const std::vector<ChampionScoreBattleRecord>& GetScoreRecord() const;
	/**
	*@brief ���ñ���ս��Ϊ����״̬
	*/
	void SetEnd() { m_IsEnd = true; }
	/**
	*@brief �����ɫ�㲥��Ϣ
	*/
	void BroadCast(Avalon::Protocol& protocol);
private:
	// ��id
	UInt32 m_GroupID;
	// �ڱ�������
	std::vector<ChampionPlayer*> m_PlayerVec;
	//����ڼ�����Ŀǰֻ��8ǿ����֮����Ч����������һ������
	UInt32 m_Order = 0;
	//���������а�
	std::list<ChampionPlayer*> m_RankList;
	//������ս����¼
	std::vector<ChampionScoreBattleRecord> m_ScoreRecords;
	// ����ս���Ƿ��ѽ���
	bool m_IsEnd = false;
};

/**
*@brief �ھ���״̬����
*/
class USChampionStatus
{
public:
	USChampionStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : m_Service(service),m_PlayerMgr(playerMgr), m_SceneMgr(sceneMgr) {}
	virtual ~USChampionStatus();
	/**
	*@brief Tick
	*/
	virtual void OnTick(const Avalon::Time& now);
	/**
	*@brief ��׼���׶ο�ʼ
	*/
	virtual void OnPrepareStart();
	/**
	*@brief ��׼���׶ν���
	*/
	virtual void OnPrepareEnd();
	/**
	*@brief ��ս���׶ο�ʼ
	*/
	virtual void OnBattleStart();
	/**
	*@brief ��ս���׶ν���
	*/
	virtual void OnBattleEnd();
	/**
	*@brief ����ɫ��������
	*/
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr) = 0;
	/**
	*@brief ����ɫ���뱾״̬�ĳ���
	*/
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
	/**
	*@brief ��ȡ��״̬��׼���׶�
	*/
	virtual const ChampionStatus GetPrepareStatus() const = 0;
	/**
	*@brief ��ȡ��״̬��ս���׶�
	*/
	virtual const ChampionStatus GetBattleStatus() const = 0;
	/**
	*@brief �Ƿ��ǻ���״̬
	*/
	bool IsScoreStatus() const;
	/**
	*@brief ����ɫ���ָı�
	*/
	void OnPlayerScoreChanged(ChampionPlayer* player);
	/**
	*@brief ��ĳ�����ȫ������
	*/
	void OnGroupBattleEnd(UInt32 groupID);
	/**
	*@brief ��ĳ��ս���ı�
	*/
	void OnGroupBattleChanged(UInt32 groupID, ChampionPlayer* winner, ChampionPlayer* loser);
	/**
	*@brief ������ս����ʼ
	*/
	void OnAllGroupBattleStart();
	/**
	*@brief ��һ�ȡ�Լ���Ļ���������
	*/
	const std::list<ChampionPlayer*>& OnPlayerGetGroupScoreRank(ChampionPlayer* player);
	/**
	*@brief ��һ�ȡ�Լ���Ļ�����ս����¼
	*/
	const std::vector<ChampionScoreBattleRecord>& OnPlayerGetGroupScoreRecord(ChampionPlayer* player);
	//����ͬ��
	/**
	*@brief ͬ��ս����ʼ����ʱ
	*/
	void SyncBattleCountdown(ChampionPlayer* player);
	/**
	*@brief ͬ������������ǰ��
	*/
	void SyncScoreRankTop(ChampionPlayer* player);
	/**
	*@brief ͬ����id
	*/
	void SyncGroupID(ChampionPlayer* player);
	/**
	*@brief ͬ��bo5�ȷ�
	*/
	void SyncKnockoutScore(ChampionPlayer* player);
protected:
	/**
	*@brief ��øý׶ε����
	*/
	USPlayerVec GetStatusPlayer(ChampionStatus status);
	/**
	*@brief ���ý׶���ҷ���ô����
	*/
	void GroupPlayer(ChampionStatus status, UInt32 groupNum);
	/**
	*@brief ���ݻ��ֽ��ý׶���ҷ���ô����
	*/
	void GroupPlayerByScore(ChampionStatus status, UInt32 groupNum);
	/**
	*@brief ������һ�η�����飬8ǿ����֮�����
	*/
	void GroupPlayerByLastGroup(ChampionStatus status);
	/**
	*@brief �����������ý׶���ҷ���
	*/
	void GroupPlayerByRank(ChampionStatus status);
	/**
	*@brief ���ý׶ν�ɫ�����Լ�����
	*/
	void GroupPlayerByStatus(ChampionStatus status);
	/**
	*@brief ��ɫ�������
	*/
	void PlayerBelongToGroup(ChampionPlayer* player, UInt32 groupId);
	/**
	*@brief ���ɳ���
	*/
	void GenScene(UInt32 num);
	/**
	*@brief ����״̬���ѹ���Ӧ����
	*/
	void PushAllGroupPlayerToScene(ChampionStatus status);
	/**
	*@brief ����״̬���ѹ��һ������
	*/
	void PushAllGroupPlayerToOneScene(ChampionStatus status);
	/**
	*@brief ����������ս��
	*/
	UInt32 OnPlayerBattleEndScore(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result);
	/**
	*@brief ��̭������ս��
	*/
	UInt32 OnPlayerBattleEndCount(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result);
	/**
	*@brief BO5����ս��
	*/
	UInt32 OnPlayerBattleEnd3Win(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info);
	/**
	*@brief ���������ڳ����е����
	*/
	void LockOnlinePlayerForBattle();
	/**
	*@brief ���״̬�������������ʤ��ս������
	*/
	void ClearPlayerWinCount();
	/**
	*@brief ���������
	*/
	void ClearGroup();
protected:
	//�����еķ���
	ChampionService& m_Service;
	//������е�PlayerMgr
	USPlayerMgr& m_PlayerMgr;
	//������е�Scene
	USceneMgr& m_SceneMgr;
	//��id-����
	std::map<UInt32, ChampionGroup> m_GroupMap;

private:
	
	// ƥ�䶨ʱ��ÿ6minһ��ս��
	Avalon::SimpleTimer							m_BattleTimer;
	// ����ǰ������ͬ����ʱ��
	Avalon::SimpleTimer							m_ScoreTopSyncTimer;
};


class SeaStatus : public USChampionStatus
{
public:
	SeaStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_SEA_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_SEA_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
private:
	UInt32 m_CurrSceneId = 1;
};

class ReSeaStatus : public USChampionStatus
{
public:
	ReSeaStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_RE_SEA_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_RE_SEA_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
private:

};

class Select64Status : public USChampionStatus
{
public:
	Select64Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_64_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_64_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select16Status : public USChampionStatus
{
public:
	Select16Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_16_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_16_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select8Status : public USChampionStatus
{
	
public:
	Select8Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_8_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_8_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select4Status : public USChampionStatus
{
public:
	Select4Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_4_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_4_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select2Status : public USChampionStatus
{
public:
	Select2Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_2_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_2_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select1Status : public USChampionStatus
{
public:
	Select1Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_1_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_1_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};





#endif



