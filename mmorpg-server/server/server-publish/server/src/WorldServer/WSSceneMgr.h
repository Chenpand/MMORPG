#ifndef _WS_SCENE_MGR_H_
#define _WS_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include <CLSceneDefine.h>
#include <set>
#include "WSScene.h"

/**
 *@brief 场景管理器
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
	 *@brief 加载 
	 */
	bool Init();
	void Final();

public:
	/**
	 *@brief 添加、移除、查找场景
	 */
	bool AddScene(WSScene* scene);
	void RemoveScene(WSScene* scene);
	WSScene* FindScene(UInt32 id);

	/**
	*@brief 创建公共副本
	*/
	void CreatePublicDynScene(UInt32 uMapId, UInt16 uDifficulty, UInt64 uParam, UInt32 uLastTime);

	/**
	 *@brief 选择一个创建动态场景的服务器
	 */
	UInt32 SelectDynSceneServer(UInt32 nodeId);
	/**
	 *@brief 选择一个创建战场的服务器
	 */
	UInt32 SelectBattleSceneServer();
	/**
	 *@brief 随机选择一个场景服务器
	 */
	UInt32 SelectRandSceneServer();

	/**
	 *@brief 根据类型和拥有者获取动态地图
	 */
	WSScene* GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId, UInt16 level);
	WSScene* GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId);

	/**
	*@brief 获得公共副本地图
	*/
	WSScene* GetPublicDynScene(UInt32 uMapId, UInt16 level);

	/**
	 *@brief 设置id种子
	 */
	void SetIdSeed(UInt32 nodeid, ObjID_t idseed);

	/**
	 *@brief 遍历场景
	 */
	void VisitScenes(WSSceneVisitor& visitor);

	/**
	*@brief 广播消息到场景服务器
	*/
	void BroadcastToSceneServer(Avalon::Packet *pPacket);

public://npc位置相关
	/**
	 *@brief 注册一个npc位置信息
	 */
	void RegisterNpc(UInt32 npcId, UInt32 mapId, const CLPosition& pos);

	/**
	 *@brief 查找一个npc
	 */
	NpcPosInfo* FindNpc(UInt32 mapId, UInt32 npcId);

public://事件
	/**
	 *@brief 场景服务器建立连接
	 */
	void OnSceneConnected(UInt32 id);

	/**
	 *@brief 场景服务器断开连接
	 */
	void OnSceneDisconnected(UInt32 id);

	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

private:
	//id-scene 索引
	CLUIntObjIndex<WSScene>		m_Scenes;
	//地图部署配置  noted by aprilliu, 每个scene server所加载的map列表， 以scene server id为key
	std::map<UInt32,MapList>	m_SceneCfg;
	//公共副本部署配置  noted by aprilliu, 每个scene server所加载的公共副本列表，以scene server id为key
	std::map<UInt32,std::map<UInt32, UInt32> >	m_PublicCopyCfg;
    //地下城入口部署配置  noted by zjy, 每个scene server所加载的地下城入口， 以scene server id为key
    std::map<UInt32, MapList>	m_DungeonEntries;

	//当前地图服务器列表  Noted by aprilliu 当前连接到world server的scene server列表（ server id ）
	std::set<UInt32>		m_SceneServers;
	//id种子列表
	IdSeedMap				m_IdSeeds;
	//上次创建战场的场景服务器
	UInt32					m_BattleSceneNode;

	//动态地图
	DynSceneMap				m_DynScenes[SCENE_TYPE_MAX];

	// 公共副本
	PublicDynSceneVec		m_PublicDynScenes;	

	//npc位置信息
	NpcMap					m_Npcs;
};

#endif
