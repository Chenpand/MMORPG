#ifndef _TCS_GRID_OBJ_H_
#define _TCS_GRID_OBJ_H_

#include <memory>
#include <CLTeamCopyDefine.h>

struct TCSGridCfg;
class TCSTeamTwo;

/**
 *@brief 团本格子对象(小队，怪物，各种据点)
 */
class TCGridObj
{
public:
	TCGridObj();
	virtual ~TCGridObj() {}

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2) { return true; }

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now) {}

	/**
	 *@brief 是否能攻打
	 */
	virtual bool IsCanAttack();

	/**
	 *@brief 准备战斗
	 */
	virtual void PreBattle();

	/**
	 *@brief 战斗
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 对象信息
	 */
	virtual std::string GetObjStr();

	/**
	 *@brief 是否死亡
	 */
	virtual bool IsDead();

	/**
	 *@brief 对象的id
	 */
	UInt32 GetObjId() { return m_ObjId; }
	void SetObjId(UInt32 id) { m_ObjId = id; }

	/**
	 *@brief 对象所在格子的id
	 */
	UInt32 GetGridId() { return m_GridId; }
	void SetGridId(UInt32 id) { m_GridId = id; }

	/**
	 *@brief 据点id
	 */
	UInt32 GetFieldId() { return m_FieldId; }
	void SetFieldId(UInt32 id) { m_FieldId = id; }

	/**
	 *@brief 格子里对象的类型
	 */
	TCGridObjType GetObjType() { return m_ObjType; }

	/**
	 *@brief 格子里对象的状态
	 */
	TCGridObjStatus GetObjStatus() { return m_ObjStatus; }
	void SetObjStatus(TCGridObjStatus status) { m_ObjStatus = status; }

	/**
	 *@brief 死亡原因
	 */
	TCGridObjDeadReason GetObjDeadReason() { return m_ObjDeadReason; }
	void SetObjDeadReason(TCGridObjDeadReason reason) { m_ObjDeadReason = reason; }

	/**
	 *@brief 格子所属队伍
	 */
	TCSTeamTwo* GetTeam() { return m_Team; }
	void SetTeam(TCSTeamTwo* team) { m_Team = team; }

protected:

	// 对象id(不会变的)
	UInt32 m_ObjId;

	// 所在格子id(对象发生移动，格子id会变化)
	UInt32 m_GridId;

	// 对应据点
	UInt32 m_FieldId;

	// 格子中对象的类型
	TCGridObjType m_ObjType;

	// 格子中对象的状态
	TCGridObjStatus m_ObjStatus;

	// 死亡原因
	TCGridObjDeadReason m_ObjDeadReason;

	// 所属队伍
	TCSTeamTwo* m_Team;
};

/**
 *@brief 小队
 */
class TCSquadObj : public TCGridObj
{
public:
	TCSquadObj();
	~TCSquadObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief 是否能攻打
	 */
	virtual bool IsCanAttack();

	/**
	 *@brief 准备战斗
	 */
	virtual void PreBattle();

	/**
	 *@brief 战斗
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 移动
	 */
	bool Move(UInt32 targetGridId, UInt32 targetObjId, std::list<UInt32>& pathList);

	/**
	 *@brief 停止移动
	 */
	void StopMove();

	/**
	 *@brief 设置一次位置
	 */
	void SetPos(UInt32 gridId);

	/**
	 *@brief 目标死亡
	 */
	void TargetDead(bool isBattleObj);

	/**
	 *@brief 减少移动cd
	 */
	void ReduceMoveCD(UInt32 cd);

	/**
	 *@brief 小队id
	 */
	void SetSquadId(UInt32 id) { m_SquadId = id; }
	UInt32 GetSquadId() { return m_SquadId; }

	/**
	 *@brief 小队出生点
	 */
	void SetBornGridId(UInt32 id) { m_BornGrid = id; }
	UInt32 GetBornGridId() { return m_BornGrid; }
	
	/**
	 *@brief 目标格子的id
	 */
	void SetTargetGridId(UInt32 id) { m_TargetGridId = id; }
	UInt32 GetTargetGridId() { return m_TargetGridId; }

	/**
	 *@brief 目标对象的id
	 */
	void SetTargetObjId(UInt32 id) { m_TargetObjId = id; }
	UInt32 GetTargetObjId() { return m_TargetObjId; }

	/**
	 *@brief 小队在格子上的状态
	 */
	void SetSquadObjStatus(TCGridSquadStatus status) { m_Status = status; }
	TCGridSquadStatus GetSquadObjStatus() { return m_Status; }

	/**
	 *@brief 设置攻打据点
	 */
	void SetBattleObj(TCGridObj* objPtr) { m_BattleObj = objPtr; }
	TCGridObj* GetBattleObj() { return m_BattleObj; }

	std::list<UInt32>& GetPathList() { return m_PathList; }
	UInt32 GetMoveCnt() { return m_MoveCnt; }

private:

	/**
	 *@brief 移动到下一个格子
	 */
	void _MoveNextGrid();

private:

	// 小队id
	UInt32 m_SquadId;

	// 小队在格子上的状态
	TCGridSquadStatus m_Status;

	// 移动cd
	UInt32 m_MoveCd;

	// 移动cd结束时间戳
	UInt32 m_MoveEndStamp;

	// 出生点格子
	UInt32 m_BornGrid;

	// 目标格子
	UInt32 m_TargetGridId;

	// 目标对象id
	UInt32 m_TargetObjId;

	// 寻路路径
	std::list<UInt32> m_PathList;
	
	// 移动次数
	UInt32 m_MoveCnt;

	// 正在攻打的据点
	TCGridObj* m_BattleObj;
};

/**
 *@brief 怪物
 */
class TCMonsterObj : public TCGridObj
{
public:
	TCMonsterObj();
	~TCMonsterObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief 战斗
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 成为小队的目标
	 */
	void AddSquadTarget(UInt32 squadId);

	/**
	 *@brief 删除小队目标
	 */
	void RemoveSquadTarget(UInt32 squadId);

	/**
	 *@brief 增加移动cd
	 */
	void AddMoveCD(UInt32 addCd) { m_MoveCd += addCd; }

	const std::list<UInt32>& GetMovePath() { return m_MovePathList; }

private:

	/**
	 *@brief 移动一格
	 */
	void _MoveNextGrid();

private:

	// 移动cd
	UInt32 m_MoveCd;

	// 移动结束时间
	UInt32 m_MoveEndStamp;

	// 属性参数id
	UInt32 m_ProId;

	// 是否初始路线
	bool m_IsInitPath;

	// 移动路线(配置固定路线)
	std::list<UInt32> m_MovePathList;

	// 是哪些小队目标
	std::set<UInt32> m_SquadTargetSet;
};

/**
 *@brief 传送门
 */
class TCTransferObj : public TCGridObj
{
public:
	TCTransferObj();
	~TCTransferObj();

	/**
	*@brief 初始化数据
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief 传送小队
	*/
	void TransferSquad(TCSquadObj* squadObj);

private:

	std::vector<UInt32> m_TransferGridVec;
};

/**
 *@brief 前哨站
 */
class TCFixPointObj : public TCGridObj
{
public:
	TCFixPointObj();
	~TCFixPointObj();

	/**
	*@brief 初始化数据
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief 获取数据
	*/
	virtual void GetObjData(TCGridObjData& data);

	/**
	*@brief 比赛结束
	*/
	virtual void BattleEnd(bool isDestory);

private:

	// 血量
	UInt32 m_Blood;
};

/**
 *@brief 能量站（击破后的状态是可以传送的）
 */
class TCEnergyPointObj : public TCGridObj
{
public:
	TCEnergyPointObj();
	~TCEnergyPointObj();

	/**
	*@brief 初始化数据
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief 获取数据
	*/
	virtual void GetObjData(TCGridObjData& data);

	/**
	*@brief tick
	*/
	virtual void OnTick(const Avalon::Time& now);

	/**
	*@brief 比赛结束
	*/
	virtual void BattleEnd(bool isDestory);

	/**
	*@brief 传送小队
	*/
	void TransferSquad(TCSquadObj* squadObj);

	/**
	*@brief 清除怪物传送门信息
	*/
	void ClearMonsterTransferData();

private:

	// 怪物传送门时间戳
	UInt32 m_MakeMonsterTransferStamp;
	// 怪物传送门的cd
	UInt32 m_MakeMonsterTransferCd;

	// 怪物传送门的生存时间
	UInt32 m_MonsterTransferLiveTime;
	
	// 怪物传送门的点位
	std::vector<UInt32> m_MonsterGridVec;
	// 传送点
	std::vector<UInt32> m_TransferGridVec;
};

/**
 *@brief 灵魂祭坛
 */
class TCAltarObj : public TCGridObj
{
public:
	TCAltarObj();
	~TCAltarObj();

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 是否死亡
	 */
	virtual bool IsDead();
};

/**
 *@brief 恐怖的灵魂祭坛
 */
class TCTerrorAltarObj : public TCGridObj
{
public:
	TCTerrorAltarObj();
	~TCTerrorAltarObj();

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 是否死亡
	 */
	virtual bool IsDead();
};

/**
 *@brief boss
 */
class TCBossObj : public TCGridObj
{
public:
	TCBossObj();
	~TCBossObj();

	/**
	 *@brief 战斗
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);
};

/**
 *@brief 主城
 */
class TCMainCityObj : public TCGridObj
{
public:
	TCMainCityObj();
	~TCMainCityObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 掉血
	 */
	void CostBlood();
	UInt32 GetBlood() { return m_Blood; }

private:

	// 血量
	UInt32 m_Blood;
};

/**
 *@brief 物资补给
 */
class TCReviveCntObj : public TCGridObj
{
public:
	TCReviveCntObj();
	~TCReviveCntObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 是否死亡
	 */
	virtual bool IsDead();

	/**
	 *@brief 复活次数
	 */
	UInt32 GetReviveCnt() { return m_ReviveCnt; }

private:
	
	// 复活次数
	UInt32 m_ReviveCnt;
};

/**
 *@brief 机动补给
 */
class TCCdDownObj : public TCGridObj
{
public:
	TCCdDownObj();
	~TCCdDownObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 是否死亡
	 */
	virtual bool IsDead();

private:

	// 小队缩减的cd
	UInt32 m_SquadReduceCd;
	// 怪物增加的cd
	UInt32 m_MonsterAddCd;
	// 兵营增加的制造怪物cd
	UInt32 m_MakeMonsterAddCd;
};

/**
 *@brief 制造怪物
 */
class TCLaboratoryObj : public TCGridObj
{
public:
	TCLaboratoryObj();
	~TCLaboratoryObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief 比赛结束
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief 增加复活cd
	 */
	void AddReviveCd();

	/**
	 *@brief 增加怪物制造cd
	 */
	void AddMakeMonsterCd(UInt32 cd) { m_MakeMonsterCD += cd; }

	/**
	 *@brief 是否正在复活
	 */
	bool IsInRevive() { return m_ReviveEndStamp != 0; }

private:

	// 复活cd
	UInt32 m_ReviceCD;
	// 制造怪物cd
	UInt32 m_MakeMonsterCD;
	// 增加的复活时间
	UInt32 m_AddReviveCD;
	// 复活结束时间
	UInt32 m_ReviveEndStamp;
	// 制造怪物结束时间
	UInt32 m_MakeMonsterEndStamp;
	// 怪物出生点
	std::vector<UInt32> m_MonsterBornGridVec;
};

/**
 *@brief 怪物传送门
 */
class TCMonsterTransferObj : public TCGridObj
{
public:
	TCMonsterTransferObj();
	~TCMonsterTransferObj();

	/**
	 *@brief 初始化数据
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief 获取数据
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

private:

	// 怪物传送门的生存时间戳
	UInt32 m_MonsterTransferLiveStamp;
	// 怪物传送门的生存时间
	UInt32 m_MonsterTransferLiveTime;
};

#endif // !_TCS_GRID_OBJ_H_

