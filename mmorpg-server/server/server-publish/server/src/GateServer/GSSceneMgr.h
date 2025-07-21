#ifndef _GS_SCENE_MGR_H_
#define _GS_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include "GSScene.h"

/**
 *@brief 网关场景管理器
 */
class GSSceneMgr : public Avalon::Singleton<GSSceneMgr>
{
public:
	GSSceneMgr();
	~GSSceneMgr();

public:
	void Init();
	void Final();

	/**
	 *@brief 添加一个场景
	 */
	bool AddScene(GSScene* scene);

	/**
	 *@brief 删除一个场景
	 */
	void RemoveScene(GSScene* scene);

	/**
	 *@brief 查找一个场景
	 */
	GSScene* FindScene(ObjID_t id);

public://事件
	/**
	 *@brief 场景连接断开
	 */
	void OnSceneDisconnected(UInt32 id);

private:
	//id-scene索引
	CLIDObjIndex<GSScene> m_Scenes;
};

#endif
