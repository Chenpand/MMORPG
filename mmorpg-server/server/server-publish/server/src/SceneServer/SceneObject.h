#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#include <CLSceneObject.h>
#include <AvalonTime.h>
#include <AvalonPacket.h>
#include <set>

class Scene;

/**
 *@brief 场景物体
 */
class SceneObject : public CLSceneObject
{
	typedef std::set<SceneObject*> SceneObjectSet;

public:
	SceneObject();
	~SceneObject();
	
	/**
	 *@brief 获取id
	 */
	ObjID_t	GetID() const{ return CLSceneObject::GetID(); }

	/**
	 *@brief 设置获取名字
	 */
	void SetName(const std::string& name){ m_Name = name; }
	const std::string& GetName() const { return m_Name; }

	void SetDoorID(UInt32 id) { m_doorId = id; }
	UInt32 GetDoorID() const { return m_doorId; }

	/**
	 *@brief 脚本设置、获取名字
	 */
	const char* ScriptGetName() const{ return m_Name.c_str(); }
	void ScriptSetName(const char *szName) { SetName(szName); }

public:
	/**
	 *@brief 设置获取场景对象
	 */
	void SetScene(Scene* scene){ m_pScene = scene; }
	Scene* GetScene() const{ return m_pScene; }
	UInt32	GetSceneID() const{ return CLSceneObject::GetSceneID(); }

	/**
	 *@brief 设置位置
	 */
	void SetPos(const CLPosition& pos);

	/**
	 *@brief 设置获取网关id
	 */
	virtual bool SetGateID(UInt32 nodeid){ return true; }
	virtual UInt32 GetGateID() const { return 0; } 

	/**
	 *@brief 设置账号id
	 */
	virtual void SetAccID(UInt32 accid){}
	virtual UInt32 GetAccID() const { return 0; }

	/**
	 *@brief 是否能看见我
	 */
	virtual bool IsCanSeeMe(SceneObject* obj);

	/**
	 *@brief 是否隐身的
	 */
	virtual bool IsHidden(){ return false; }

	/**
	 *@brief 初始设置
	 */
	virtual bool SetupBase() = 0;

	/**
	 *@brief 创建object
	 */
	static SceneObject* CreateObject(int type);
	static void FreeObject(SceneObject* obj);
	static void DestroyObjects();
	static ObjID_t GenObjID();
	static void SetIDSeed(ObjID_t id);

	/**
	 *@brief 同步属性变化
	 */
	virtual void SyncProperty();

    void SetPosChanged(bool changed) { m_bPosChanged = changed; }

public://场景相关
	/**
	 *@brief 切换场景
	 *@param sceneid 目标场景id
	 *@param pos 目标位置
	 *@param dir 目标方向
	 *@param bTransZone 是否是正常跳转点切场景
	 */
	virtual bool ChangeScene(UInt32 sceneid, const CLPosition& pos, UInt8 dir, bool bTransZone = false, UInt32 targetDoorId = 0);

	/**
	*@brief 是否可以进入场景
	*/
	virtual bool CanEnterMap(UInt32 sceneid) { return true; }

public://事件
	/**
	 *@brief 心跳
	 */
	virtual void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief 即将进入场景事件，此接口通常用于确定位置
	 */
	virtual void OnBeforeEnterScene();

	/**
	 *@brief 进入场景事件
	 */
	virtual void OnEnterScene();

	/**
	 *@brief 离开场景事件
	 */
	virtual void OnLeaveScene();

	/**
	 *@brief 切换场景
	 */
	virtual void OnSceneChanged(){}

	/**
	 *@brief 同步域格子发生变化
	 */
	virtual void OnGridChanged();

	/**
	 *@brief 移动事件
	 */
	virtual void OnMoved(const CLPosition& oldpos);

	/** 
	 *@brief 进入第二天
	 */
	virtual void OnDayChanged();

	//检查是否跨天了
	virtual bool CheckDayChange(const Avalon::Time& now);

public:
	/**
	 *@brief 广播到周围能看见我的
	 */
	virtual void Broadcast(Avalon::Protocol& protocol);
	virtual void Broadcast(Avalon::Packet* packet);
	virtual void BroadcastExceptMe(Avalon::Protocol& protocol);
	virtual void BroadcastExceptMe(Avalon::Packet* packet);

	/**
	 *@brief 广播到新区域
	 */
	virtual void BroadcastNewZone(Avalon::Packet* packet);
	virtual void BroadcastOldZone(Avalon::Packet* packet);

public:
	/**
	 *@brief 构建同步object的消息包
	 */
	Avalon::Packet* MakeSyncObjectPacket(bool bEnterScene);

	/**
	 *@brief 构建同步属性的消息包
	 */
	Avalon::Packet* MakeSyncPropertyPacket(UInt32 type, bool bDirty);
	Avalon::Packet* MakeSyncPropertyPacket(UInt32 type);

	/**
	 *@brief 构建删除obj的消息包
	 */
	Avalon::Packet* MakeDelObjectPacket();

	/**
	 *@brief 构建切场景的消息包
	 */
	Avalon::Packet* MakeChangeSceneDataPacket(ChangeSceneInfo& info);

public:
	CL_SYNC_PROPERTY_BEGIN(CLSceneObject)
	CL_SYNC_PROPERTY(m_Name, SOA_NAME, SST_SELF | SST_AROUND | SST_TEAM | SST_WATCH | SST_SCENE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSceneObject)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY_END()

	DEFINE_CHANGESCENE_SERIALIZATION(CLSceneObject)
	{
		stream & m_bDayChanged;
	}

	virtual void ClearInvalidData(){}


public:
	//是否触发日期变化
	UInt8		m_bDayChanged;

private:
	//id生成种子
	static UInt64	m_IdSeed;
	//待回收列表
	static SceneObjectSet	m_FreeObjects;

	//场景
	Scene*		m_pScene;
	//原位置
	CLPosition	m_OldPos;
	//位置是否变化
	bool		m_bPosChanged;

	//名字
	OwnerObjString	m_Name;

	//门ID
	UInt32		m_doorId;
};

/**
 *@brief 玩家对象访问器
 */
typedef CLVisitor<SceneObject> SceneObjectVisitor;

#endif
