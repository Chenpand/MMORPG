#ifndef _GS_SCENE_MGR_H_
#define _GS_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include "GSScene.h"

/**
 *@brief ���س���������
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
	 *@brief ���һ������
	 */
	bool AddScene(GSScene* scene);

	/**
	 *@brief ɾ��һ������
	 */
	void RemoveScene(GSScene* scene);

	/**
	 *@brief ����һ������
	 */
	GSScene* FindScene(ObjID_t id);

public://�¼�
	/**
	 *@brief �������ӶϿ�
	 */
	void OnSceneDisconnected(UInt32 id);

private:
	//id-scene����
	CLIDObjIndex<GSScene> m_Scenes;
};

#endif
