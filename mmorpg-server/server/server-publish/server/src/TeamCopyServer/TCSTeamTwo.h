#ifndef _TCS_TEAM_TWO_H_
#define _TCS_TEAM_TWO_H_

#include <CLTeamCopyDefine.h>
#include <AvalonSimpleTimer.h>
#include "TCSTeam.h"

class TCGridObj;
class TCSquadObj;
class TCMonsterObj;
class TCMonsterTransferObj;
class TCLaboratoryObj;

/**
 *@brief 团本2 队伍
 */
class TCSTeamTwo : public TCSTeam
{
public:
	TCSTeamTwo();
	~TCSTeamTwo();

	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief 队伍是否满
	*/
	virtual bool TeamIsFull();

	/**
	*@brief 获取门票
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec);

	/**
	*@brief 金主限制数量
	*/
	virtual UInt32 GetGoldMaxNum();
	virtual UInt32 GetGoldNumErrorCode();

	/**
	*@brief 攻坚数据通知
	*/
	virtual void AttackDataNotify(UInt32 stage);
	virtual void AttackDataNotify(TCSPlayer* player);

	/**
	*@brief 条件检查完成
	*/
	virtual void CheckCondFinish();

	/**
	*@brief 移动格子
	*/
	virtual void MoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId);

	/**
	*@brief 查询玩家信息超时
	*/
	virtual void QueryRacePlayerInfoTimeOut(UInt32 squadId);

	/**
	*@brief 开始发起比赛
	*/
	virtual UInt32 StartRace(UInt32 squadId, UInt32 fieldId);

	/**
	*@brief 创建比赛返回
	*/
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief 比赛结束
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime);

	/**
	*@brief 上报阶段boss信息
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief 游戏结束
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief Gm通关
	*/
	virtual void GMClearAllField(UInt32 squadId);

	/**
	*@brief 设置小队属性
	*/
	virtual void SetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd);

	/**
	*@brief 清除格子对象
	*/
	virtual void GMClearGridObj(UInt64 playerId, UInt32 gridType);

	/**
	*@brief 是否允许换座位
	*/
	virtual UInt32 IsAllowChangeSeat(UInt32 srcSquadId, UInt32 destSquadId);

	/**
	*@brief 换位完成
	*/
	virtual void ChangeSeatFinish(UInt32 srcSquadId, bool srcIsDec, UInt32 destSquadId, bool destIsInc);

	/**
	*@brief 小队攻坚数据通知
	*/
	virtual void SquadAttackDataNotify();

	/**
	*@brief 成员离线
	*/
	virtual void MemberExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief 成员数量空
	*/
	virtual void SquadMemberNull(UInt32 squadId);

	/**
	*@brief 阶段Udp日志
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result);

	/**
	*@brief 查找格子对象
	*/
	TCGridObj* FindGridObjByGridId(UInt32 gridId);
	TCGridObj* FindGridObjByObjId(UInt32 objId);

	/**
	*@brief 小队重新寻路
	*/
	void SquadReSearchWay(UInt32 squadId, UInt32 gridId, UInt32 objId);

	/**
	*@brief 本小队的信息通知到其他小队
	*/
	void SendCurSquadDataToOtherSquad(TCSquadObj* squadObj);

	/**
	*@brief 激活传送门
	*/
	void TransferFieldActive();

	/**
	*@brief 小队发起战斗
	*/
	UInt32 SquadBattle(TCGridObj* battleObj, TCSquadObj* squadObj);

	/**
	*@brief 小队数据同步
	*/
	void SendSquadObjData();
	void SendSquadObjData(TCSquadObj* squadObj, bool isSendToOtherSquad = true);

	/**
	*@brief 同步据点数据
	*/
	void SendGridObjData(TCGridObj* fieldObj);

	/**
	*@brief 同步怪物数据
	*/
	void SendMonsterObjData(TCMonsterObj* monsterObj);

	/**
	*@brief 同步怪物传送门
	*/
	void SendMonsterTransferObjData(TCMonsterTransferObj* mTransferObj);

	/**
	*@brief 小队返回出生点
	*/
	void SquadReturnBorn(TCSquadObj* squadObj);

	/**
	*@brief 对象被击破
	*/
	void GridObjDestory(TCGridObj* destoryObj, bool isKill = true);

	/**
	*@brief 实验室之间联动
	*/
	void LaboratoryLink(TCLaboratoryObj* laboratory);

	/**
	*@brief 制造怪物
	*/
	void MakeMonster(UInt32 monsterType, const std::vector<UInt32>& gridVec);

	/**
	*@brief 制造怪物传送门
	*/
	void MakeMonsterTransfer(UInt32 liveTime, const std::vector<UInt32>& gridVec);

	/**
	*@brief 选出一个空格子
	*/
	UInt32 SelectNullGrid(const std::vector<UInt32>& gridVec);

	/**
	*@brief 增加怪物
	*/
	void AddMonster(TCGridObj* newObj);

	/**
	*@brief 增加复活次数
	*/
	void AddReviveCnt(UInt32 reviveCnt);

	/**
	*@brief 修改据点的cd
	*/
	void UpdateGridObjCd(UInt32 squadRecudeCd, UInt32 monsterAddCd, UInt32 makeMonsterAddCd);

	/**
	*@brief 打开祭坛
	*/
	void OpenAltar();

	/**
	*@brief 结束祭坛中的比赛
	*/
	void EndAltarRace(bool isPhaseEnd);

	/**
	*@brief 祭坛被摧毁
	*/
	void AltarDestory(TCGridObj* altarObj);

	/**
	*@brief 据点重生
	*/
	void FieldReborn(UInt32 objType, UInt32 objStatus);

	/**
	*@brief boss联动
	*/
	void BossLink();

	/**
	*@brief 通知据点的比赛退出
	*/
	void FieldRaceEnd(TCGridObj* gridObj, UInt64 raceId, bool isVictory);

	/**
	*@brief 通知所有比赛退出
	*/
	void AllRaceEnd();

	/**
	*@brief 增加死亡据点
	*/
	void AddDeadField(TCGridObj* deadObj);

	/**
	*@brief 生成对象id
	*/
	UInt32 GenObjId();

	/**
	*@brief 阶段boss结束
	*/
	void PhaseBossEnd();
	UInt32 GetBossPhase() { return m_BossPhase; }

	/**
	*@brief 获取寻路路径
	*/
	std::list<UInt32> GetSearchPath(UInt32 srcGridId, UInt32 destGridId);

	/**
	*@brief boss关卡是否有战斗
	*/
	bool BossObjIsBattle();

	/**
	*@brief 主城位置
	*/
	UInt32 GetMainCityGridId() { return m_MainCityGridId; }

	/**
	*@brief 是否有小队在打这只怪物
	*/
	bool IsSquadAttackMonster(UInt32 monsterObjId);

	/**
	*@brief 设置翻牌类型
	*/
	void SetPassType(UInt32 type);

private:

	/**
	*@brief 创建格子对象
	*/
	TCGridObj* _CreateGridObj(TCGridObjType type);

	/**
	*@brief 查找小队对象
	*/
	TCSquadObj* _FindSquadObj(UInt32 squadId);

	/**
	*@brief 查找怪物对象
	*/
	TCMonsterObj* _FindMonsterObj(UInt32 monsterObjId);

	/**
	*@brief 查找死亡对象
	*/
	TCGridObj* _FindDeadObjByGridId(UInt32 gridId);
	TCGridObj* _FindDeadObjByObjId(UInt32 deadObjId);
	void _GetDeadObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList);

	/**
	*@brief 获取对象格子列表
	*/
	void _GetObjGridVec(std::set<UInt32>& gridSet);

	/**
	*@brief 操作权限
	*/
	bool _OperatorPower(TeamCopyMember* member);

	/**
	*@brief 小队传送到格子上
	*/
	void _SquadTransferToGrid(TCSquadObj* squadObj, UInt32 gridId);

	/**
	*@brief 获取对象列表
	*/
	void _GetObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList);

	/**
	*@brief 翻牌奖励
	*/
	void _FlopReward(UInt32 stage);

	/**
	*@brief 据点增加比赛
	*/
	void _AddFieldRace(UInt32 fieldObjId, UInt64 raceId);

	/**
	*@brief 获得据点比赛
	*/
	void _GetFieldRace(UInt32 fieldObjId, std::set<UInt64>& raceSet);

	/**
	*@brief 据点是否有比赛
	*/
	bool _IsHasFieldRace(UInt32 fieldObjId);

	/**
	*@brief 增加的复活次数发送到比赛
	*/
	void _SendReviveCntToRace(UInt64 raceId);

	/**
	*@brief 断线重连处理
	*/
	void _LeaveReconnect();

	/**
	*@brief 获取小队移动次数
	*/
	UInt32 _GetSquadMoveCnt(UInt32 squadId);

private:

	// 地图上对象编号种子
	UInt32 m_ObjSeed;

	// 增加的复活次数
	UInt32 m_AddReviveCnt;

	// 翻牌类型
	TC2PassType m_PassType;

	// boss关卡阶段
	UInt32 m_BossPhase;

	// 联动据点id
	UInt32 m_bossLinkFieldId;

	// 主城的格子id
	UInt32 m_MainCityGridId;

	// 1秒定时器
	Avalon::SimpleTimer m_SecTimer;

	// 据点管理(key->格子对象id)
	std::map<UInt32, TCGridObj*> m_GridObjMap;

	// 被摧毁的据点(key->格子对象id)
	std::map<UInt32, TCGridObj*> m_DeadObjMap;

	// 小队战场数据(key->小队id)
	std::map<UInt32, TCSquadObj*> m_SquadObjMap;

	// 怪物数据(key->怪物对象id)
	std::map<UInt32, TCMonsterObj*> m_MonsterObjMap;

	// 据点里的比赛(key->据点对象id，比赛id)
	std::map<UInt32, std::set<UInt64>> m_FieldRaceMap;
};

#endif // !_TCS_TEAM_TWO_H_

