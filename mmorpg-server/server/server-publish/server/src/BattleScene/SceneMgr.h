#ifndef _SCENE_MGR_H_
#define _SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonIDGenerator.h>
#include <AvalonPropertySet.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include "Scene.h"
#include "DungeonEntrySceneDynamic.h"

class BattleScene;
class LostBattleScene;

/**
 *@brief ����������
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
	 *@brief ���һ������
	 */
	bool AddScene(Scene* scene);

	/**
	 *@brief �Ƴ�һ������ 
	 */
	void RemoveScene(Scene* scene);

	/**
	 *@brief �ͷ�һ����������
	 */
	void FreeScene(Scene* scene);

	/**
	 *@brief ����һ������
	 */
	Scene* FindScene(UInt32 id);
	Scene* FindNormalScene(UInt32 id);

	/**
	 *@brief ����һ��ָ����ͼ�ĳ���
	 */
	bool LoadScene(UInt32 mapid);

    /**
    *@brief ���ݳ������ʹ�����������
    */
    Scene* CreateSceneByType(SceneType type);

    /**
    *@brief ����һ�����³���ڵ�ͼ�ĳ���
    */
    bool LoadDungeonEntryScene(UInt32 mapid);

	/**
	*@brief ����һ�����³���ڵ�ͼ�ĳ���
	*/
	Scene*  LoadDynamicDungeonEntryScene(UInt32 mapid);

	/**
	*@brief ������̬���³���ڳ���
	*/
	Scene* CreateDungeonEntrySceneDynamic(UInt32 ownSceneId, UInt32 curDoorId, UInt32 mapid);

	/**
	*@brief ���Ҷ�̬���³���ڳ���
	*/
	Scene* GetDungeonEntrySceneDynamic(UInt32 ownSceneId, UInt32 curDoorId, UInt32 mapid);

	/**
	*@brief ͨ������IDɾ����̬����
	*/
	void  DelDungeonEntrySceneDynamic(UInt32 ownSceneId, UInt32 curDoorId, UInt32 mapid);

	/**
	 *@brief �������г���
	 */
	void VisitScenes(SceneVisitor& visitor);

	/**
	*@brief ��ȡ�����ID
	*/
	UInt32 GetActivitySceneID() const { return m_activitySceneId; }

public:// ��̬�������

	/**
	*@brief ���ض�̬����
	*/
	Scene* LoadDynamicScene(UInt32 mapid);

	/**
	*@brief ������̬ս������
	*/
	Scene* CreateDynamicScene(UInt32 battleID, UInt32 mapid);

	/**
	*@brief ͨ������ID���Ҷ�̬����
	*/
	Scene* GetDynamicScene(UInt32 battleID);

	/**
	*@brief ͨ������IDɾ����̬����
	*/
	void DelDynamicScene(UInt32 battleID);

public:
	/**
	*@brief �����ˢ��
	*/
	void GenActivityCityMonster();

	/**
	*@brief ������еĻ����
	*/
	void ClearActivityCityMonster();

	/**
	*@brief ��ȡ���������Ϣ
	*/
	std::vector<SceneNpcInfo> GetSceneNpcInfo(Player* player);

	/**
	*@brief ��ȡ�������
	*/
	CityMonster* FindCityMonster(ObjID_t id);

	/**
	*@brief �����ˢ��
	*/
	void RemoveTaskCityMonster(ObjID_t owner, ObjID_t id);

	/**
	*@brief �����ˢ��
	*/
	void RemovePersionalCityMonsters(ObjID_t owner, bool notify);

public://�¼�
	/**
	 *@brief ����������
	 */
	void OnGateConnected(UInt32 id);

	/**
	*@brief ���������
	*/
	void OnCrossConnected(UInt32 id);

	/**
	*@brief ս��������
	*/
	void OnBattleConnected(UInt32 id);

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
	//id-scene����
	CLIDObjIndex<Scene>	m_Scenes;
	//��̬����id������
	Avalon::IDGenerator m_IdGenerator;

	//��̬����ģ��
	SceneMap	m_SceneTemplates;

	//ж���б�
	SceneSet	m_FreeList;

	// �����ID
	UInt32		m_activitySceneId;

	//��̬����ս������
	std::map<UInt32, Scene*> m_DynamicSceneMap;
	
	//��̬���³���ڳ���
	std::map<DungeonEntrySceneDynamicKey, Scene*> m_DungeonEntrySceneDynamics;
};

#endif
