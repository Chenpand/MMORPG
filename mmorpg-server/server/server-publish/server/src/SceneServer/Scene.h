#ifndef _SCENE_H_
#define _SCENE_H_

#include <AvalonProtocol.h>
#include <AvalonIDGenerator.h>
#include <AvalonTime.h>
#include <AvalonAStar.h>
#include <AvalonSimpleTimer.h>
#include <set>
#include <vector>
#include <functional>
#include <CLScene.h>
#include <CLObjectIndex.h>
#include <CLSceneDefine.h>
#include <CLSceneDataEntry.h>
#include <CLItemProperty.h>
#include <CLSysNotify.h>
#include <CLChatDefine.h>

#include "SceneObject.h"

class Player;
class Npc;
class Creature;
class Vehicle;
class Trap;
class CityMonster;

/**
 *@brief 场景状态
 */
enum SceneStatus
{
	SCENE_INIT,			//初始化
	SCENE_NORMAL,		//通常
	SCENE_QUIT,			//退出状态
};

/**
 *@brief 场景参数名最大长度
 */
#define MAX_SCENEPARAM_NAME_LEN 16

/**
 *@brief 场景定时事件                                                                     
 */
struct SceneTimerEvent
{
	SceneTimerEvent():nextTime(0), interval(0), times(1){}

	std::string	handler;
	UInt64		nextTime;
	UInt64		interval;
	UInt32		times;
};

/**
*@brief 传送门信息
*/
struct TransportDoor
{
    UInt32      id;
	ScenePos	birthScenePos;
    CLPosition  birthPos;

    UInt32      targetSceneId;
    UInt32      targetDoorId;
};

/**
 *@brief 场景
 */
class Scene : public CLScene
{
	AVALON_DEFINE_OBJECTPOOL(Scene, Avalon::NullMutex)

	typedef std::map<std::string, UInt64>	SceneParamMap;
	typedef std::map<UInt64, UInt64>	IdTimeMap;
	typedef std::map<Creature*, UInt64>	CreatureTimeMap;
	typedef std::vector<Scene*>			SceneVec;
	typedef std::vector<SceneTimerEvent>	TimerEventVec;

	//最大空位查询范围
	const static Int32	NONEGIRD_SEARCH_RANGE = 128;
	//切场景超时时间
	const static UInt32 CHANGESCENE_TIMEOUT = 10;
	//分流的玩家等级
	const static UInt32	MAX_GHOSTSCENE_NUM = 5;

public:
	Scene();
	Scene(const Scene& scene);
	~Scene();

public:
	/**
	 *@brief 加载卸载
	 */
	virtual bool Load(UInt32 mapid,const std::string& config);
	virtual void UnLoad(){}

	/**
	 *@brief 获取场景资源id
	 */
	UInt32 GetMapID() const{ return m_MapID;}

	/**
	 *@brief 获取类型
	 */
	virtual UInt8 GetType() const{ return m_pDataEntry->type; }

	/**
	 *@brief 设置名字
	 */
	void ScriptSetName(const char* name);

	/**
	 *@brief 获取配置项
	 */
	SceneDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief 获取主城
	 */
	UInt32 GetMajorCity() const;

    /**
    *@brief 有没有这个门
    */
    bool HasThisDoor(UInt32 doorId);

    /**
    *@brief 获取门的位置
    */
    CLPosition GetDoorBirthPos(UInt32 doorId);
	ScenePos GetDoorBirthScenePos(UInt32 doorId);

	const std::vector<TransportDoor>& GetDoors() const { return m_doors; }

	/**
	 *@brief 获取玩家数
	 */
	UInt32 GetPlayerNum(){ return m_PlayerNum; }
	UInt32 GetWaitPlayerNum();
	UInt32 GetMaxPlayerNum() const{ return m_pDataEntry->maxPlayerNum; }

	/**
	 *@brief 地图广播
	 */
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);
	//by huchenhui
	//void BroadcastNotifyById(UInt32 id, ...);

	template <typename... Args>
	void BroadcastNotifyById(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		BroadcastNotify(type, color, buffer);
	}

	/**
	 *@brief 同步人数到worldserver
	 */
	virtual void SyncPlayerNumToWorldServer();

public: //动态地图相关
	/**
	 *@brief 设置、获取副本等级
	 */
	virtual void SetLevel(UInt16 level){}
	virtual UInt16 GetLevel() const{ return 0; }

	/**
	 *@brief 检查能否进入
	 */
	virtual bool CheckPlayerEnter(ObjID_t id, UInt16 level){ return true; }

    /**
    *@brief 获取出生点
    */
    virtual CLPosition GetBirthPoint();

    /**
    *@brief 获取格子高度
    */
    virtual float GetGridHeight();

    /**
    *@brief 获取格子宽度
    */
    virtual float GetGridWidth();

public:
	/**
	 *@brief 添加移除查找一个object
	 */
	bool AddObject(SceneObject* obj);
	void RemoveObject(SceneObject* obj);
	SceneObject* FindObject(ObjID_t id);

	/**
	 *@brief 查找相关场景对象
	 */
	Creature* FindCreature(ObjID_t id);
	Player* FindPlayer(ObjID_t id);
	
	/**
	 *@brief 添加移除查找一个等待切场景的obj
	 */
	bool AddWaitObject(SceneObject* obj);
	UInt32 GetWaitObjectNum() const;
	void RemoveWaitObject(SceneObject* obj);
	void MoveOutWaitList(SceneObject* obj);
	SceneObject* FindWaitObject(ObjID_t id);

	/**
	 *@brief 移除物件
	 */
	void ClearItems();

	/**
	 *@brief 设置获取场景状态
	 */
	void SetStatus(SceneStatus status){ m_Status = status; }
	SceneStatus GetStatus() const { return m_Status; }

	/**
	 *@brief 踢出玩家
	 */
	virtual void KickPlayersOut(){}
	virtual void KickPlayerOut(Player* player){}

public://城镇怪物相关

	/**
	*@brief 指定具体怪物信息刷怪
	*/
	CityMonster* SummonCityMonster(ObjID_t guid, ObjID_t owner, const std::string& name,
		UInt8 type, UInt32 dataId, NpcPos pos, UInt32 dungeonId, UInt32 remainTimes, bool notify);

	/**
	*@brief 获取怪物信息
	*/
	SceneNpcInfo GetNpcInfo(Player* player);

	/**
	*@brief 添加怪物
	*/
	void AddCityMonster(CityMonster* monster, bool notify);

	/**
	*@brief 删除怪物
	*/
	void DelCityMonster(CityMonster* monster, bool notify = true);

	/**
	*@brief 删除玩家私有的怪物
	*/
	void DelPersonalCityMonsters(ObjID_t owner, bool notify);

	/**
	*@brief 查找怪物,owner=0说明是普通怪物，否则就是玩家私有的怪物
	*/
	CityMonster* FindCityMonster(ObjID_t owner, ObjID_t id);

	/**
	*@brief 查找怪物
	*/
	CityMonster* FindCityMonster(ObjID_t id);

	/**
	*@brief 访问所有的通用怪物
	*/
	void VisitCommonCityMonsters(const std::function<bool(CityMonster*)>& F);

public://路径阻挡相关
	/**
	 *@brief 设置检查阻挡
	 */
	void SetBlock(const CLPosition& pos, UInt8 flag);
	void UnsetBlock(const CLPosition& pos, UInt8 flag);
	bool CheckBlock(const CLPosition& pos, UInt8 flag);
	/**
	 *@brief 判断从一点到另一点的直线距离有没有某些类型的阻挡
	 */
	bool CheckBlock(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag);
	/**
	 *@brief 在一个坐标点的周围获取一个坐标
	 */
	bool FindPosNearby(const CLPosition& pos, CLPosition& tarpos, UInt8 flag);
	/**
	 *@brief 查找线段内能直线到达的最远的点
	 */
	bool FindPosReachable(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag, CLPosition& tarpos);
	/**
	 *@brief 在范围内随机找一点
	 *@return true 找到了  false 没找到
	 */
	bool FindRandPos(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief 在地图上随机找一个点
	 */
	bool FindRandPos(CLPosition& pos, UInt8 blockFlag, UInt32 edgeWidth = 3);
	/**
	 *@brief 在一个圈上随机找一个点
	 */
	bool FindRandPosOnCircle(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	*@brief 在一个圈上随机找指定阻挡类型的一个点
	*/
	bool FindRandBlockPosOnCircle(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief 在一个圈的指定方向上开始找个点
	 */
	bool FindPosInDirection(const CLPosition& center, Int32 radius, Int32 dir, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief 查找一条路径
	 */
	bool FindPath(const CLPosition& startPos, const CLPosition& endPos, std::vector<CLPosition>& path, UInt8 blockFlag);

public://事件
	/**
	 *@brief 场景初始化
	 */
	virtual void OnSceneInit();

	/**
	 *@brief 场景退出
	 */
	virtual void OnSceneQuit();

	/**
	 *@brief 场景开始卸载
	 */
	virtual void OnSceneUnload();

	/**
	 *@brief 玩家即将进入
	 */
	virtual void OnBeforePlayerEnter(Player* player);

	/**
	 *@brief npc即将进入                                                                     
	 */
	virtual void OnBeforeNpcEnter(Npc* npc);

	/**
	 *@brief 玩家进入
	 */
	virtual void OnPlayerEnter(Player* player);

	/**
	 *@brief 玩家离开
	 */
	virtual void OnPlayerLeave(Player* player);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

public://访问接口

	/**
	 *@brief 访问objects
	 */
	virtual void VisitSceneObjects(SceneObjectVisitor& visitor);
	
	/**
	 *@brief 脚本遍历所有玩家
	 */
	void ScriptVisitPlayers(const char* func);

	/**
	 *@brief 访问周围的玩家
	 */
    virtual void VisitPlayersAround(const CLPosition& pos, CLVisitor<Player>& visitor);

	/**
	 *@brief 访问周围的npc
	 */
	void VisitNpcsAround(const CLPosition& pos, CLVisitor<Npc>& visitor);

	/** 
	 *@brief 访问周围除玩家和npc外的其他物件
	 */
	void VisitOthersAround(const CLPosition& pos, SceneObjectVisitor& visitor);

	/**
	 *@brief 访问周围的object
	 */
    virtual void VisitObjectsAround(const CLPosition& pos, SceneObjectVisitor& visitor);

	/**
	 *@brief 访问except
	 */
	void VisitPlayersNewZone(const SceneObject* obj,  CLVisitor<Player>& visitor);
	void VisitPlayersOldZone(const SceneObject* obj,  CLVisitor<Player>& visitor);

	/**
	 *@brief 访问except
	 */
	virtual void VisitObjectsNewZone(const SceneObject* obj, SceneObjectVisitor& visitor);
	virtual void VisitObjectsOldZone(const SceneObject* obj, SceneObjectVisitor& visitor);

    /**
    *@brief 能否看到其它玩家
    */
    virtual bool CanSeeOthers() { return true; }

	/**
	 *@brief 判断是否在周围
	 */
	bool IsInBroadcastZone(const SceneObject* obj1, const SceneObject* obj2) const;

	/**
	 *@brief 获取位置pos同步域中的玩家数
	 */
	UInt32 GetPlayerNumAround(const CLPosition& pos) const;

	/**
	 *@brief 遍历场景上所有玩家
	 */
	void VisitPlayers(CLVisitor<Player>& visitor);

public://消息相关
	/**
	 *@brief 广播到场景所有玩家
	 */
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);

private:
	//id-obj索引
	CLIDObjIndex<SceneObject>	m_Objects;
	//等待进入场景的列表
	CLIDObjIndex<SceneObject>	m_WaitObjects;

	//待消失列表
	IdTimeMap		m_WaitDisappearMap;
	//待复活列表
	CreatureTimeMap	m_WaitReviveMap;

	//a*寻路
	Avalon::AStar	m_AStar;

	//地图id
	UInt32			m_MapID;
	//场景状态
	SceneStatus		m_Status;
	//踢人定时器
	Avalon::SimpleTimer	m_KickTimer;
	//秒定时器
	Avalon::SimpleTimer	m_SecTimer;

	//当前玩家数
	UInt32			m_PlayerNum;
	//原玩家数
	UInt32			m_OldPlayerNum;

	//地图配置
	SceneDataEntry*	m_pDataEntry;

    //出生点位置
    CLPosition      m_BirthPos;

    //格子高度
    float           m_GridHeight;
    //格子宽度
    float           m_GridWidth;

    std::vector<TransportDoor>   m_doors;

	// 所有怪物
	std::map<ObjID_t, CityMonster*>					m_AllMonsters;

	// 通用怪物(所有人都看得见)
	std::map<ObjID_t, CityMonster*>					m_CommonMonsters;

	// 玩家私有的怪(只有自己看得见)
	std::map<ObjID_t, std::vector<CityMonster*> >	m_PlayerMonsters;
};

typedef CLVisitor<Scene> SceneVisitor;

#endif
