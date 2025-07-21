#pragma once
#include <CLDefine.h>
#include "BSPlayer.h"
#include "CLErrorCode.h"

struct RacePlayerInfo;
class BSBaseRace;
class BSPlayer;

class BSBaseBattle
{
public:
	BSBaseBattle();
	virtual ~BSBaseBattle();

	/**
	*@brief 计算比赛ID
	*/
	static UInt64 GenRaceID();

	/**
	*@brief 设置获取id
	*/
	void SetId(UInt32 id) { m_ID = id; }
	UInt32 GetId() const { return m_ID; }

	inline UInt8 GetState() { return m_state; }
	inline void SetState(UInt8 state) { m_state = state; }

	/**
	*@brief 设置获取场景服id
	*/
	void SetSceneNodeID(UInt32 id) { m_nodeID = id; }
	UInt32 GetSceneNodeID() const { return m_nodeID; }

	void SetSceneId(UInt32 id) { m_sceneId = id; }
	UInt32 GetSceneId() { return m_sceneId; }

	Avalon::Time GetBeginTime() { return m_beginTime; }

	BSPlayer* FindPlayer(UInt64 playerId);

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	virtual bool IsTimeToDestory(const Avalon::Time& now) { return false; }

	virtual void OnTick(const Avalon::Time& now) {};

	virtual void NotifyCreateSceneBattle() {};
	virtual void NotifyCloseSceneBattle() {};

	virtual bool OnPlayerBeginEnterBattle(UInt32 accId, ObjID_t roleId, UInt32 sourceSceneNodeId,
		UInt32 dstSceneNodeId, RacePlayerInfo& racePlayerInfo)
	{
		return true;
	}

	virtual void OnPlayerEnterBattle(BSPlayer* player) {};

	virtual void OnPlayerLeaveBattle(BSPlayer* player) {};

	virtual UInt32 PkPlayer(UInt64 roleId, BSPlayer* attacker, BSPlayer* attackeder, UInt32 dungeonID) { return ErrorCode::SUCCESS; }

	void SendToBattleScene(Avalon::Protocol& protocol);
protected:
	//战场ID
	UInt32 m_ID;
	//场景服ID
	UInt32 m_nodeID;
	//SceneId(表id)
	UInt32 m_sceneId;
	//开始的时间戳
	Avalon::Time m_beginTime;
	//状态			
	UInt8	m_state;
	// 战场被摧毁时间
	Avalon::Time   m_destoryTime;
};