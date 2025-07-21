#ifndef _CITY_MONSTER_H_
#define _CITY_MONSTER_H_

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLObject.h>
#include <CLCityMonsterDataEntry.h>
#include <CLObjectDefine.h>

class Player;
class CityMonster : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(CityMonster, Avalon::NullMutex);
public:
	CityMonster();
	~CityMonster();

public:
	// 事件

	/**
	*@brief 创建成功
	*/
	virtual void OnCreated(bool notify = true);

	/**
	*@brief 被玩家杀死（组队时同一局会被调用多次）
	*/
	virtual void OnKilled(ObjID_t raceId);

	/**
	*@brief 怪物被删除
	*/
	virtual void OnRemoved(bool notify = true);

public:

	/**
	*@brief 转换成字符串，方便打印日志
	*/
	std::string ToString();

	/**
	*@brief 设置获取拥有者ID
	*/
	void SetOwnerID(ObjID_t id) { m_Owner = id; }
	ObjID_t GetOwnerID() const { return m_Owner; }

	/**
	*@brief 设置获取场景ID
	*/
	void SetSceneID(UInt32 id) { m_SceneId = id; }
	UInt32 GetSceneID() const { return m_SceneId; }

	/**
	*@brief 怪物类型
	*/
	void SetMonsterType(CityMonsterType type) { m_Type = type; }
	CityMonsterType GetMonsterType() const { return m_Type; }
	

	/**
	*@brief 设置获取类型ID
	*/
	void SetDataID(UInt32 id) { m_DataId = id; }
	UInt32 GetDataID() const { return m_DataId; }

	/**
	*@brief 设置获取位置
	*/
	void SetPos(NpcPos pos) { m_Pos = pos; }
	NpcPos GetPos() const { return m_Pos; }

	/**
	*@brief 设置对应地下城ID
	*/
	void SetDungeonID(UInt32 dungeonId) { m_DungeonId = dungeonId; }
	UInt32 GetDungeonID() const { return m_DungeonId; }

	/**
	*@brief 总次数
	*/
	void SetTotalTimes(UInt32 times) { m_TotalTimes = times; }
	UInt32 GetTotalTimes() const { return m_TotalTimes; }

	/**
	*@brief 剩余可打次数
	*/
	void SetRemainTimes(UInt32 times) { m_RemainTimes = times; }
	UInt32 GetRemainTimes() const { return m_RemainTimes; }
	void DecRemainTimes();

	/**
	*@brief 能否打
	*/
	UInt32 CanFight();

	/**
	*@brief 正在打的队伍列表
	*/
	void AddRacingRace(ObjID_t raceId);
	void DelRacingRace(ObjID_t raceId);
	UInt32 GetRacingRaceNum() { return m_RacingRaceList.size(); }

	/**
	*@brief npc信息
	*/
	SceneNpc GetInfo();

	/**
	*@死亡复活时间间隔
	*/
	inline void SetReviveTimeSpace(UInt32 timeSpace) {	m_reviveTimeSpace = timeSpace;}
	inline UInt32 GetReviveTimeSpace() { return m_reviveTimeSpace; }

	inline void SetReviveTimeStamp(UInt32 timeStamp) { m_reviveTimeStamp = timeStamp; }
	inline UInt32 GetReviveTimeStamp() { return m_reviveTimeStamp; }

	//战场id
	inline void SetBattleId(UInt32 battleId) { m_battleId = battleId; }
	inline UInt32 GetBattleId() { return m_battleId; }

	//战场动态场景id
	inline void SetBattleDyncSceneId(UInt32 battleDyncSceneId) { m_battleDyncSceneId = battleDyncSceneId; }
	inline UInt32 GetBattleDyncSceneId() { return m_battleDyncSceneId; }

	inline UInt8 GetInBattle() { return m_inBattle; }
private:
	// 拥有者(如果有拥有者就说明是属于某个玩家的)
	ObjID_t		m_Owner;
	// 场景ID
	UInt32		m_SceneId;
	// 怪物类型
	CityMonsterType	m_Type;
	// 类型ID
	UInt32		m_DataId;
	// 位置
	NpcPos		m_Pos;
	// 地下城ID
	UInt32		m_DungeonId;
	// 剩余可打次数
	UInt32		m_RemainTimes;
	// 总次数
	UInt32		m_TotalTimes;
	// 已经打过的比赛列表
	std::set<ObjID_t>	m_RaceIds;
	// 正在打的战斗列表
	std::set<ObjID_t>	m_RacingRaceList;
	// 所在战场id
	UInt32	m_battleId;
	// 所在战场动态场景id
	UInt32	m_battleDyncSceneId;
	// 死亡间隔时间
	UInt32	m_reviveTimeSpace;
	// 复活时间戳
	UInt32	m_reviveTimeStamp;
	// 是否在战斗中
	UInt8	m_inBattle;
};

#endif