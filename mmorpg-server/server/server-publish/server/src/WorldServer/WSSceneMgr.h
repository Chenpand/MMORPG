#ifndef _WS_SCENE_MGR_H_
#define _WS_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include <CLSceneDefine.h>
#include <set>
#include "WSScene.h"

/**
 *@brief ����������
 */
class WSSceneMgr : public Avalon::Singleton<WSSceneMgr>
{
	typedef std::vector<UInt32>							MapList;
	typedef std::map<ObjID_t, std::vector<WSScene*> >	DynSceneMap;
	typedef std::map<UInt32, std::vector<NpcPosInfo> >	NpcMap;
	typedef std::vector<WSScene*>	PublicDynSceneVec;
	typedef std::map<UInt32, ObjID_t>	IdSeedMap;

public:
	WSSceneMgr();
	~WSSceneMgr();

public:
	/**
	 *@brief ���� 
	 */
	bool Init();
	void Final();

public:
	/**
	 *@brief ��ӡ��Ƴ������ҳ���
	 */
	bool AddScene(WSScene* scene);
	void RemoveScene(WSScene* scene);
	WSScene* FindScene(UInt32 id);

	/**
	*@brief ������������
	*/
	void CreatePublicDynScene(UInt32 uMapId, UInt16 uDifficulty, UInt64 uParam, UInt32 uLastTime);

	/**
	 *@brief ѡ��һ��������̬�����ķ�����
	 */
	UInt32 SelectDynSceneServer(UInt32 nodeId);
	/**
	 *@brief ѡ��һ������ս���ķ�����
	 */
	UInt32 SelectBattleSceneServer();
	/**
	 *@brief ���ѡ��һ������������
	 */
	UInt32 SelectRandSceneServer();

	/**
	 *@brief �������ͺ�ӵ���߻�ȡ��̬��ͼ
	 */
	WSScene* GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId, UInt16 level);
	WSScene* GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId);

	/**
	*@brief ��ù���������ͼ
	*/
	WSScene* GetPublicDynScene(UInt32 uMapId, UInt16 level);

	/**
	 *@brief ����id����
	 */
	void SetIdSeed(UInt32 nodeid, ObjID_t idseed);

	/**
	 *@brief ��������
	 */
	void VisitScenes(WSSceneVisitor& visitor);

	/**
	*@brief �㲥��Ϣ������������
	*/
	void BroadcastToSceneServer(Avalon::Packet *pPacket);

public://npcλ�����
	/**
	 *@brief ע��һ��npcλ����Ϣ
	 */
	void RegisterNpc(UInt32 npcId, UInt32 mapId, const CLPosition& pos);

	/**
	 *@brief ����һ��npc
	 */
	NpcPosInfo* FindNpc(UInt32 mapId, UInt32 npcId);

public://�¼�
	/**
	 *@brief ������������������
	 */
	void OnSceneConnected(UInt32 id);

	/**
	 *@brief �����������Ͽ�����
	 */
	void OnSceneDisconnected(UInt32 id);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

private:
	//id-scene ����
	CLUIntObjIndex<WSScene>		m_Scenes;
	//��ͼ��������  noted by aprilliu, ÿ��scene server�����ص�map�б� ��scene server idΪkey
	std::map<UInt32,MapList>	m_SceneCfg;
	//����������������  noted by aprilliu, ÿ��scene server�����صĹ��������б���scene server idΪkey
	std::map<UInt32,std::map<UInt32, UInt32> >	m_PublicCopyCfg;
    //���³���ڲ�������  noted by zjy, ÿ��scene server�����صĵ��³���ڣ� ��scene server idΪkey
    std::map<UInt32, MapList>	m_DungeonEntries;

	//��ǰ��ͼ�������б�  Noted by aprilliu ��ǰ���ӵ�world server��scene server�б� server id ��
	std::set<UInt32>		m_SceneServers;
	//id�����б�
	IdSeedMap				m_IdSeeds;
	//�ϴδ���ս���ĳ���������
	UInt32					m_BattleSceneNode;

	//��̬��ͼ
	DynSceneMap				m_DynScenes[SCENE_TYPE_MAX];

	// ��������
	PublicDynSceneVec		m_PublicDynScenes;	

	//npcλ����Ϣ
	NpcMap					m_Npcs;
};

#endif
