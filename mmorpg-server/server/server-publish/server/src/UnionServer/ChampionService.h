#ifndef __CL_CHAMPION_SERVICE_H__
#define __CL_CHAMPION_SERVICE_H__ 


#include "UnionService.h"
#include <functional>
#include <CLChampionDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLChampionProtocol.h>
#include <CLObserveProtocol.h>


class GameZone;
class ChampionPlayer;
class USChampionStatus;

bool IsLogicSameStatus(ChampionStatus a, ChampionStatus b);

struct ChampionActInfoBase
{

};

struct JoinRaceActInfo : public ChampionActInfoBase
{
	CLProtocol::UnionChampionJoinPrepareReq* msg;
};

struct BattleResultActInfo : public ChampionActInfoBase
{
	UInt64 winnerID = 0;
	UInt64 loseID = 0;
	_resultBitmask result = resultBitmask_invalid;
	UInt64 trueWinner = 0;
	UInt64 trueLoser = 0;
};

struct ObserveActInfo : public ChampionActInfoBase
{
	UInt32 connID = 0;
	CLProtocol::UnionChampionObserveReq* msg = nullptr;
};


using ChampionActFun = std::function<UInt32(UInt32, UInt64, ChampionActInfoBase*)>;


class ChampionService : public UnionService
{
public:
	ChampionService();
	~ChampionService();

public:
	void Clear();
	virtual USPlayer* CreatePlayer();
	virtual UScene* CreateScene();

	virtual bool Init();
	virtual void OnTick(const Avalon::Time& now);
	virtual void OnConnected(GameZone* zone);

	static UnionServerFunctionType GetFunctionType();
	/*
	**�����������
	*/
	virtual void OnAddNewPlayer(USPlayer* player);
	void OnLoadPlayerData(CLRecordCallback* callback);
	virtual void OnPlayerDisconnect(UInt64 roleId);
	virtual void OnPlayerMove(UInt64 roleID, Avalon::Packet* packet);
	virtual bool OnCheckAddNewPlayer(UInt64 roleId, UInt32 accid);
	/**
	*@brief �յ�Playerͬ��sceneobject
	*/
	void OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet* packet);
	/**
	*@brief �յ�Playerͬ��ɾ��sceneobject
	*/
	void OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �յ�Playerͬ��ObjectProperty
	*/
	void OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �յ�World�ı�ZoneID�ַ������е�Service
	*/
	virtual void OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID);

	/**
	*@brief �յ�SceneReady
	*/
	virtual void OnSceneReady(CLProtocol::UnionSceneIsReady & protocol);
	// �Ƿ�������
	inline const bool BoLoadFinish() const { return m_LoadFinish; }
	inline void SetLoadFinish(bool bo) { m_LoadFinish = bo; }

	void GMChangeStatus(const ChampionStatus status);
	void GMChangePlayerStatus(UInt64 roleID, std::function<void(ChampionPlayer*)> changer);

	void SetTop16Table(std::vector<ChampionPlayer*> vec);
	const std::vector<ChampionTop16Player> GetTop16Table();
	void Sync16Table(GameZone* zone);
	const UInt32 GetConnID(const UInt64 roleID) const;

	void GetPlayerAttributes(UInt64 roleID, std::function<void(ChampionPlayer*)> geter);
	ChampionPlayer* GetPlayer(UInt64 roleID);
	const ChampionStatus GetStatus() const { return m_Status; }
	const ChampionStatusInfo GetStatusInfo() const { return m_TimePoints[m_Status]; }
	void OnPlayerScoreChanged(ChampionPlayer* player);
	const std::list<ChampionPlayer*>& OnPlayerGetGroupScoreRank(ChampionPlayer* player);
	const std::vector<ChampionScoreBattleRecord>& OnPlayerGetGroupScoreRecord(ChampionPlayer* player);

	const bool IsGMControlStatus() const { return m_IsGMControlStatus; }
	bool IsEnroll(UInt32 accid);

	void SetDBStatus(ChampionStatus status);
public:
	//������Ϊ����
	UInt32 BeginAct(ChampionAct act, UInt32 accid, UInt64 roleId, ChampionActInfoBase* info = nullptr);

	UInt32 OnPlayerCheckEnroll(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);
	UInt32 OnPlayerJoinRace(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);
	UInt32 OnPlayerRaceEnd(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);
	UInt32 OnPlayerRelive(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);
	UInt32 OnPlayerCheckRelive(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);
	UInt32 OnPlayerObserve(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info);


public:
	void OnMatchRaceEnd(CLProtocol::MatchServerChampionRaceEnd& req);
	UInt32 OnMatchQueryPlayerInfo(CLProtocol::UnionMatchServerQueryPlayerInfoReq& req);
	void OnNotifyRaceStart(CLProtocol::WorldNotifyRaceStart& req);
	void OnMatchBattleRes(CLProtocol::MatchServerChampionBattleRes& req);
	void OnMatchOnPVPRes(CLProtocol::MatchServerRaceEnd& req);



private:
	void LoadTimePoint();
	void CalcStatus(const Avalon::Time& now);
	void ChangeToStatus(ChampionStatus status);
	void SetStatusRace(ChampionStatus status);
	void LoadPlayerData();
	void OnEnrollStart();
	void OnEnrollEnd();

private:
	
	// �Ƿ�������
	bool										m_LoadFinish = false;
	// ��ǰ״̬
	ChampionStatus							    m_Status;
	// ����ʱ���
	ChampionStatusInfo							m_TimePoints[CS_NUM];
	// �ѱ�����accid
	std::set<UInt32>							m_EnrollAccidSet;
	// ״̬��Ϊ����
	std::map<ChampionStatus, std::map<ChampionAct, ChampionActFun>> m_StatusActMap;
	// ��ǰ�׶ζ���
	USChampionStatus*                           m_StatusObj;

	//GM����״̬
	bool										m_IsGMControlStatus = false;

	//16ǿ��
	std::vector<ChampionPlayer*>                m_Top16TableVec;
	// userid -> connid ��ս��
	std::map<UInt64, UInt32>					m_UserIDConnIDMap;
	//���ݿ����״̬
	ChampionStatus								m_DBStatus;
	//��ǰ��������
	UInt32										m_InPlayerNum = 0;

	static std::vector<ChampionScoreBattleRecord> m_NullScoreBattleRecord;
	static std::list<ChampionPlayer*>             m_NullGroupScoreRank;
};


#endif // __CL_CHAMPION_SERVICE_H__



