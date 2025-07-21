#ifndef _TEAM_COPY_MGR_H_
#define _TEAM_COPY_MGR_H_

#include <unordered_map>
#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLTeamCopyDefine.h>
#include <CLObjectProperty.h>

class Player;
class CLRecordCallback;

namespace CLProtocol{
	class TeamCopyPlayerInfoSync;
};

struct TeamCopySelfData : public CLSyncObject
{
	TeamCopySelfData() {}

	ObjUInt64 owner;
	// 类型
	ObjUInt32 teamType;
	// 日挑战次数
	ObjString dayNumStr;
	// 日退队次数
	ObjUInt32 dayQuitTeamNum;
	// 日时间
	ObjUInt64 dayTime;
	// 周挑战次数
	ObjString weekNumStr;
	// 周退队次数
	ObjUInt32 weekQuitTeamNum;
	// 周时间
	ObjUInt64 weekTime;
	// 模式通关次数
	ObjString modelPassStr;
	// 日翻牌记录
	ObjString dayFlopStr;
	// 周翻牌记录
	ObjString weekFlopStr;
	// 使用团本道具次数
	ObjString extraNumStr;
	// roll奖励次数
	ObjString rollNumStr;

	void ChangeDay(const Avalon::Time& now);
	void ChangeWeek(const Avalon::Time& now);
	std::string LogStr();

	//数据库相关
	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", owner)
	CL_DBPROPERTY("team_type", teamType)
	CL_DBPROPERTY("day_num", dayNumStr)
	CL_DBPROPERTY("day_quit_team_num", dayQuitTeamNum)
	CL_DBPROPERTY("day_time", dayTime)
	CL_DBPROPERTY("week_num", weekNumStr)
	CL_DBPROPERTY("week_quit_team_num", weekQuitTeamNum)
	CL_DBPROPERTY("week_time", weekTime)
	CL_DBPROPERTY("model_pass", modelPassStr)
	CL_DBPROPERTY("day_flop", dayFlopStr)
	CL_DBPROPERTY("week_flop", weekFlopStr)
	CL_DBPROPERTY("extra_num", extraNumStr)
	CL_DBPROPERTY("roll_num", rollNumStr)
	CL_DBPROPERTY_END()
};

/**
*brief 团本数据
*/
class TeamCopyMgr
{
public:
	TeamCopyMgr();
	~TeamCopyMgr();

	void SetOwner(Player* player) { m_owner = player; }
	Player* GetOwner() { return m_owner; }

	/**
	*brief 是否在团本
	*/
	inline bool IsTeamCopy() { return m_IsTeamCopyFlag; }
	inline void SetTeamCopy(bool flag) { m_IsTeamCopyFlag = flag; }

	/**
	*brief init
	*/
	void OnInit(const Avalon::Time& now);

	/**
	*brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*brief 加载
	*/
	void LoadData(CLRecordCallback* callback);

	/**
	*brief 周挑战次数
	*/
	UInt32 GetWeekNum(UInt32 teamType, UInt32 grade);

	/**
	*brief 难度通关次数
	*/
	UInt32 PassGradeNum(UInt32 grade, const std::string& numStr);

	/**
	*brief 使用团本道具
	*/
	void UseTeamCopyItem(UInt32 teamType, UInt32 grade, UInt32 num);

	/**
	*brief 团本数据发回
	*/
	void HandlerTeamCopyPlayerInfo(CLProtocol::TeamCopyPlayerInfoSync& sync);

	/**
	*brief 切换团本场景
	*/
	bool ChangeTeamCopyScene(UInt32 destSceneId);

	/**
	*brief 退出团本场景
	*/
	void QuitTeamCopyScene();

	/**
	*brief 是否在团本场景
	*/
	bool IsTeamCopyScene();

	/**
	*brief 重连团本场景
	*/
	void ReconnectTeamCopy();

	/**
	*brief 团本场景进入、离开通知
	*/
	void NotifyTeamCopySceneEnter();
	void NotifyTeamCopySceneLeave();

	/**
	*brief 团本地下城复活
	*/
	void TeamCopyDungeonRevive(UInt64 targetId, UInt32 reviveId, UInt32 reviveCoinNum);

	/**
	*brief 团本信息同步
	*/
	void PlayerInfoToTeamCopy(UInt32 teamType);

	/**
	*brief 门票同步
	*/
	void TicketAlterToTeamCopy(UInt32 itemId);

	/**
	*brief 装备评分同步
	*/
	void EquipScoreToTeamCopy();

	/**
	*brief Loading
	*/
	void NotifyTeamCopyPlayerLoading(UInt32 isLoading);

	/**
	*brief 通关地下城事件
	*return param1:团本难度，param2:通关消耗时间，param3:团本类型
	*/
	void ClearDungeonEvent(ObjID_t param1, ObjID_t param2, ObjID_t param3);

private:

	/**
	*brief 场景失败
	*/
	void _SceneFailed();

	/**
	*brief 开关
	*/
	bool _IsOpen();
	bool _IsOpen(UInt32 teamType);

	/**
	*brief 门票同步
	*/
	void _TicketToTeamCopy();

	/**
	*brief 门票是否足够
	*/
	bool _TicketIsEnough(UInt32 ticket1, UInt32 ticketNum1, UInt32 ticket2, UInt32 ticketNum2);

	/**
	*brief 刷新团本道具使用次数
	*/
	void _RefreshTeamCopyItem(UInt32 teamType);

private:

	Player* m_owner;
	// 是否在团本
	bool m_IsTeamCopyFlag;
	// 团本数据
	std::unordered_map<UInt32, TeamCopySelfData> m_SelfDataMap;
};

#endif // !_TEAM_COPY_MGR_H_
