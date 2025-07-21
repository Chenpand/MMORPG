#ifndef _SCENE_MGR_H_
#define _SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonIDGenerator.h>
#include <AvalonPropertySet.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include "Scene.h"

/**
 *@brief 场景管理器
 */
class SceneMgr : public Avalon::Singleton<SceneMgr>
{
	typedef std::map<UInt32, Scene*>	SceneMap;
	typedef std::set<Scene*>			SceneSet;

public:
	SceneMgr();
	~SceneMgr();

public:
	bool Init(const Avalon::PropertySet* properties);
	void Final();

	/**
	 *@brief 添加一个场景
	 */
	bool AddScene(Scene* scene);

	/**
	 *@brief 移除一个场景 
	 */
	void RemoveScene(Scene* scene);

	/**
	 *@brief 释放一个场景对象
	 */
	void FreeScene(Scene* scene);

	/**
	 *@brief 查找一个场景
	 */
	Scene* FindScene(UInt32 id);
	Scene* FindNormalScene(UInt32 id);

	/**
	 *@brief 加载一个指定地图的场景
	 */
	bool LoadScene(UInt32 mapid);

    /**
    *@brief 根据场景类型创建场景对象
    */
    Scene* CreateSceneByType(SceneType type);

    /**
    *@brief 加载一个地下城入口地图的场景
    */
    bool LoadDungeonEntryScene(UInt32 mapid);

	/**
	 *@brief 访问所有场景
	 */
	void VisitScenes(SceneVisitor& visitor);

	/**
	*@brief 获取活动场景ID
	*/
	UInt32 GetActivitySceneID() const { return m_activitySceneId; }

public:
	/**
	*@brief 活动城镇刷怪
	*/
	void GenActivityCityMonster();

	/**
	*@brief 清除所有的活动怪物
	*/
	void ClearActivityCityMonster();

	/**
	*@brief 获取城镇怪物信息
	*/
	std::vector<SceneNpcInfo> GetSceneNpcInfo(Player* player);

	/**
	*@brief 获取城镇怪物
	*/
	CityMonster* FindCityMonster(ObjID_t id);

	/**
	*@brief 活动城镇刷怪
	*/
	void RemoveTaskCityMonster(ObjID_t owner, ObjID_t id);

	/**
	*@brief 活动城镇刷怪
	*/
	void RemovePersionalCityMonsters(ObjID_t owner, bool notify);

public://动态战场刷怪
	

public://事件
	/**
	 *@brief 网关连上来
	 */
	void OnGateConnected(UInt32 id);

	/**
	*@brief 跨服连上来
	*/
	void OnCrossConnected(UInt32 id);

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

private:
	//id-scene索引
	CLIDObjIndex<Scene>	m_Scenes;
	//动态场景id生成器
	Avalon::IDGenerator m_IdGenerator;

	//动态场景模板
	SceneMap	m_SceneTemplates;

	//卸载列表
	SceneSet	m_FreeList;

	// 活动场景ID
	UInt32		m_activitySceneId;

private:
	Avalon::SimpleTimer PrintPerformanceCountTimer;
};

#endif
