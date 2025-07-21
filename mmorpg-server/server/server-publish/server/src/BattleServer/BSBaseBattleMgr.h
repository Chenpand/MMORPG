#pragma once
#include "CLDefine.h"

#include <AvalonSimpleTimer.h>

class BSBaseBattle;

class BSBaseBattleMgr
{
public:
	typedef std::vector<UInt32>			SceneServerVec;

public:
	BSBaseBattleMgr();
	~BSBaseBattleMgr();

public:
	/**
	*@brief 获取battle id
	*/
	static UInt32 GenBattleID();

	/**
	*@brief 场景服务器连接建立
	*/
	void OnSceneServerConnected(UInt32 nodeId);

	/**
	 *@brief 场景服务器链接断开
	 */
	void OnSceneServerDisconnected(UInt32 nodeId);

	/**
	*@brief 获得一个可用的场景服务器
	*/
	UInt32 SelectSceneServer();

	//战场
	virtual bool AddBattle(BSBaseBattle* battle);
	virtual void DelBattle(BSBaseBattle* battle);

	BSBaseBattle* FindBattle(UInt32 battleId);

	void DestoryBattle(BSBaseBattle* battle);

	void OnTick(const Avalon::Time& now);

	virtual void OnTickImp(const Avalon::Time& now) {};

	virtual void DestoryBattleImp(BSBaseBattle* battle) {};

	template<class T>
	T* FindBattleWithType(ObjID_t id)
	{
		auto race = FindBattle(id);
		if (!race)
		{
			return NULL;
		}

		return dynamic_cast<T*>(race);
	}
	
	virtual void SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime);
protected:
	//所有战场<battleId, BSBaseBattle*>
	std::map<UInt32, BSBaseBattle*>	m_allBattles;

	//当前可以分配的ID
	static UInt32 m_CurrentID;

	//场景服务器列表
	SceneServerVec				m_SceneServers;
	//上一个场景服务器
	UInt32						m_SceneServer;

	// 准备时间
	UInt32 m_PreTime;
	// 开始时间
	UInt32 m_StartTime;
	// 结束时间
	UInt32 m_EndTime;
	// 派奖时间
	UInt32 m_AwardTime;

	// 定时器，0.5s
	Avalon::SimpleTimer	m_Timer;
};