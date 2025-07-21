#ifndef _BATTLE_SCENE_MGR_H_
#define _BATTLE_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>

#include "GameZoneMgr.h"

class BattleScene;

/**
 *@brief 战场场景管理器                                                                     
 */
class BattleSceneMgr : public Avalon::Singleton<BattleSceneMgr>
{
public:
	//一轮的时长
	const static UInt32	ROUND_LASTTIME = 3 * 60;
	//报名时长
	const static UInt32 ROUND_SIGNIN_TIME = 60;

	typedef std::vector<BattleScene*>	SceneVec;
	typedef std::vector<UInt32>			SceneServerVec;	

public:
	BattleSceneMgr();
	~BattleSceneMgr();

public:
	bool Init();
	void Final();

	BattleScene* FindBattleScene(UInt32 nodeId);

public:
	/**
	 *@brief 事件                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 场景创建                                                                     
	 */
	void OnSceneCreated(UInt32 nodeId, UInt32 sceneId);

	/**
	 *@brief 场景销毁                                                                     
	 */
	void OnSceneDestroyed(UInt32 sceneId);

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

	/**
	*@brief 获得吃鸡准备场景服务器
	*/
	UInt32 GetChiJiSceneServer();

private:
	//场景列表
	SceneVec					m_Scenes;

	//场景服务器列表
	SceneServerVec				m_SceneServers;

	//创建场景定时器
	Avalon::SimpleTimer			m_CreateSceneTimer;

	//上一个场景服务器
	UInt32						m_SceneServer;
};

#endif
