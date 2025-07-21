#ifndef _WS_SCENE_H_
#define _WS_SCENE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <set>
#include <CLSceneDataEntry.h>

class WSPlayer;

/**
 *@brief World场景类
 */
class WSScene
{
public:
	WSScene();
	~WSScene();

public:
	/**
	 *@brief 初始化
	 */
	bool Init(UInt32 id, UInt32 mapid, const std::string& name, UInt32 nodeid);

	/**
	 *@brief 获取场景id
	 */
	UInt32 GetID() const{ return m_Id; }

	/** 
	 *@brief 获取场景名
	 */
	const char* ScriptGetName() const{ return m_Name.c_str(); }

	/**
	 *@brief 获取mapid
	 */
	UInt32 GetMapID() const { return m_MapID; }

	/**
	 *@brief 获取所在服务器id
	 */
	UInt32 GetNodeID() const{ return m_Conn->GetID(); }

	/**
	 *@brief 设置获取类型
	 */
	void SetType(UInt8 type){ m_Type = type; }
	UInt8 GetType() const{ return m_Type; }

	/**
	 *@brief 获取最大人数
	 */
	UInt32 GetMaxPlayerNum() const{ return m_pDataEntry->maxPlayerNum; }

	/**
	*@brief 获取表格数据
	*/
	SceneDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief 获取创建时间
	 */
	UInt32 GetCreateTime() const{ return m_CreateTime; }

	/**
	 *@brief 设置场景变量
	 */
	void SetParam(const char* name, UInt64 value);

	/** 
	 *@brief 设置获取场景玩家数
	 */
	void SetPlayerNum(UInt32 num){ m_PlayerNum = num; }
	UInt32 GetPlayerNum() const{ return m_PlayerNum; } 

public://副本相关
	/**
	 *@brief 设置获取副本等级
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief 设置获取boss击杀列表
	 */
	void SetBossList(const std::map<UInt32, UInt8>& bosses){ m_Bosses = bosses; }
	const std::map<UInt32, UInt8>& GetBossList() const{ return m_Bosses; }

	/**
	 *@brief 检查是否第一次进入副本
	 */
	bool IsFirstEnter(ObjID_t playerId);

	/**
	 *@brief 设置获取归属者
	 */
	void SetOwner(ObjID_t owner){ m_Owner = owner; }
	ObjID_t GetOwner() const{ return m_Owner; }

	/**
	 *@brief 设置复活点
	 */
	void SetRevivePos(const CLPosition& pos){ m_RevivePos = pos; }
	const CLPosition& GetRevivePos() const{ return m_RevivePos; }

	/**
	 *@brief 拉人
	 */
	void PullPlayer(WSPlayer* player);
	void PullPlayerToPos(WSPlayer* player, const CLPosition& pos);

public://事件
	/**
	 *@brief 场景创建完
	 */
	void OnCreated(WSPlayer* creater);

	void OnDestroyed();

public://网络
	/**
	 *@brief 发送到场景服务器
	 */
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

private:
	//id
	UInt32	m_Id;
	//名字
	std::string m_Name;

	//场景服务器连接
	Avalon::NetConnectionPtr m_Conn;
	//map id
	UInt32 m_MapID;
	//类型
	UInt8	m_Type;
	//场景数据项
	SceneDataEntry*	m_pDataEntry;

	//创建时间
	UInt32	m_CreateTime;

	//副本等级
	UInt16	m_Level;
	//副本boss
	std::map<UInt32, UInt8> m_Bosses;
	//副本进入过的玩家列表
	std::set<ObjID_t>	m_EnterPlayers;

	//所属者
	ObjID_t	m_Owner;
	//复活点
	CLPosition	m_RevivePos;

	//玩家人数
	UInt32	m_PlayerNum;
};

typedef CLVisitor<WSScene> WSSceneVisitor;

#endif 
