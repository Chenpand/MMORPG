#ifndef _WS_ACTIVITY_MGR_H_
#define _WS_ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <AvalonNetConnection.h>
#include <AvalonSimpleTimer.h>
#include <CLGameDefine.h>
#include <CLGlobalActivityServerDefine.h>
#include "CLItemDefine.h"
#include "CLActivityDefine.h"

class WSPlayer;
class CLRecordCallback;

/**
*@brief 活动页签信息
*/
struct ActivityTabInfo
{
	ActivityTabInfo() :id(0){}

	// 页签id
	UInt32 id;
	// 页签名
	std::string name;
	// 页签下的活动id
	std::vector<UInt32> actIds;
};

class WSActivityMgr : public Avalon::Singleton<WSActivityMgr>
{
	/**
	 *@brief 活动注册信息
	 */
	struct ActivityRegInfo
	{
		ActivityRegInfo(): type(0), level(0), startTime(0), preTime(0), lastTime(0), gamestartTime(0), gamestartFlag(0), cricleDays(0){}
		ActivityRegInfo(UInt8 _type, UInt16 _level, UInt32 _startTime, UInt32 _preTime, UInt32 _lastTime, UInt32 _cricleDays)
			: type(_type), level(_level), startTime(_startTime), preTime(_preTime), lastTime(_lastTime), gamestartTime(0), gamestartFlag(0), cricleDays(_cricleDays){}

		UInt32		id;
		//类型，0 每日活动，1 一次性活动
		UInt8		type;
		// 活动名
		std::string	name;
		//需要等级
		UInt16		level;
		//开始时间
		UInt32		startTime;
		//准备时间
		UInt32		preTime;
		//持续时间
		UInt32		lastTime;
		//开服时间要求
		UInt32		gamestartTime;
		//开服时间要求标记，0 规定时间之前开服才有活动，1 规定时间之后开服才有活动
		UInt8		gamestartFlag;
		//循环周期天数
		UInt32		cricleDays;
	};
	
	typedef std::vector<ActivityRegInfo>	ActivityRegVec;
	typedef std::map<UInt32, ActivityRegInfo>		ActivityRegMap;
	typedef std::set<ObjID_t>  RightPlayersSet;

	

public:
	WSActivityMgr();
	~WSActivityMgr();

	bool Init();

public:

	/**
	*@brief 开服时间的活动
	*/
	void OpenServerTimeActivity(UInt32 firstOpenServerTime);

	/**
	*@brief 加载页签表
	*/
	void LoadActivityTabTable();

	/**
	*@brief 添加活动页签信息
	*/
	void AddActivityTabInfo(ActivityTabInfo* tabInfo);

	/**
	*@brief 访问活动页签信息
	*/
	void VisitActivityTabInfo(const std::function<bool(ActivityTabInfo*)>& F);

	/**
	*@brief 清空页签数据
	*/
	void ClearActivityTabInfo();

	/**
	*@brief 添加活动页签
	*/
	bool AddActivityToTab(UInt32 actId, UInt32 tabId);

	/**
	*@brief 更新活动页签
	*/
	bool UpdateActivityTab();

	//-------------------------运营活动start------------------------------------------
public:
	//载入运营活动配置数据
	bool LoadOpActivityData();

	//加载表格配置的运营活动
	bool LoadOpActivityFromDataTable(bool isReload = false);

	//重载表格
	void ReloadOpActivityFromDataTable();

	//创建运营活动配置
	void CreateOpActivity(CLRecordCallback* cb);

	// 添加运营活动
	void AddOpActivity(OpActivityRegInfo& opAct);

	//查询运营活动配置
	void QueryOpActData(std::map<UInt32,OpActivityRegInfo>& datas);
	//更新运营活动配置
	void UpdateOpActData(OpActivityRegInfo& info);
	//删除运营活动配置
	bool DeleteOpActData(UInt32 dataId);

	// 访问活动
	void VisitOpActivity(const std::function<bool(const OpActivityRegInfo&)>& F);

	// 检测运营活动加载完毕
	void OnCheckGmOpActLoadOver(UInt32 totalNum);

	// 是否加载完毕gm运营活动
	bool IsGmDataLoadOver() { return m_GmDataLoadOver; }

	// 自定义设置活动数据
	void CustomSetOpActData(UInt32 id, UInt32 startTime, UInt32 endTime);

	//GM接口编解码
	bool GMDecodeRewardItems(std::vector<ItemReward>& items, std::string strItems);
	std::string GMEncodRewardItems(std::vector<ItemReward>& items);

	//是否处于活动中
	bool IsInOpActivity(UInt32 activityId);

	//获取运营活动by activityId
	bool GetOpActivityById(UInt32 id, OpActivityRegInfo& opAct);

	//获取活动掉落加成
	void GetActivityDropAddition(UInt32 dungeonId, DungeonAddition& addition);

	//更新运营活动状态
	void SyncOpActStateChange(OpActivityRegInfo& info);

	// 活动服同步活动
	void GlobalActivitySync(const OpActivityInfoObjectVec& opActs);
	
	
private:

	//遍历运营活动配置
	void TraversalOpRegActs(const Avalon::Time& now);

	//运营活动状态改变事件
	void OnOpActStateChange(OpActivityRegInfo& info, ActivityState state);

	void OnTickOpAct(const Avalon::Time& now);

	//运营活动开启处理
	void OnOpActivityChangeOpen(const OpActivityRegInfo& info);
	void OnChangeOpenNewServerFighting(const OpActivityRegInfo& info);

	//运营活动关闭处理
	void OnOpActivityChangeClose(const OpActivityRegInfo& info);

	//-------------------------运营活动end------------------------------------------

public:
	/**
	*@brief 清除注册信息
	*/
	void ClearRegInfos();

	/**
	*@brief 注册每日活动
	*/
	void RegisterDailyActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief 更新每日活动
	*/
	void UpdateDailyActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief 注册一次性活动
	*/
	void RegisterOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOnceActivity(const std::string &name, UInt16 level, UInt32 startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief 注册开服一次性活动(可配置开服时间)
	*/
	void RegisterOpenServerOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 lastTime, UInt32 openServerTime);

	/**
	*@biref 注册每周活动
	*/
	void RegisterWeekActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);
	/**
	*@brief 更新每周活动
	*/
	void UpdateWeekActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@biref 注册每月活动(每月1号0点开始，持续整个月)
	*/
	void RegisterMonthActivity(UInt32 id, const std::string &name, UInt16 level);

	/**
	*@brief 注册开服活动
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt32 lastTime);

	/**
	*@brief 注册开服活动
	*@param name:活动名称 level:玩家等级 endTime:结束时间（格式：天数|小时:分钟:秒）
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& endTime);

	/**
	*@brief 注册开服活动
	*@param name:活动名称 level:玩家等级 fromTime:开服第几天某个时间点开始 toTime:开服第几天某个时间点结束 （格式：天数|小时:分钟:秒）
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& fromTime, const std::string& toTime);

	/**
	*@brief 注册开服活动
	*@param name:活动名称 level:玩家等级 fromDay:开服第几天开服时间点开始 toDay:开服第几天开服时间点结束
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt16 fromDay, UInt16 toDay);

	/**
	*@brief 注册永久活动
	*/
	void RegisterConsistActivity(const std::string &name, UInt16 level);
	void RegisterConsistActivity(UInt32 id, const std::string &name, UInt16 level);

	/**
	*@brief 注册自定义定时活动
	*/
	void RegisterCustomCircleActivity(UInt32 id, const std::string &name, UInt16 level, const std::string startTime, UInt32 preTime, const UInt32 lastTime);

	/**
	*@brief 设置活动的开服时间要求
	*/
	void SetGameStartLimit(UInt32 id, const std::string &gamestartTime, UInt8 gamestartFlag);

	/**
	*@brief 添加跨服活动信息
	*/
	void AddGlobalActivity(const char *szName);

	/**
	*@brief 访问活动
	*/
	void VisitActivity(const std::function<bool(const ActivityInfo&)>& F);
	

	/**
	*@brief 查找活动
	*/
	bool FindActivity(UInt32 actId, ActivityInfo& actInfo);

	/**
	 *@brief 开启一个活动广播全服玩家
	 */
	bool OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime);

	// 修改活动对一个玩家
	void NotifyActivityStateChange(WSPlayer* player, Int32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime);
	/**
	 *@brief 关闭一个活动
	 */
	void CloseActivity(const std::string& name);

	/**
	 *@brief 是否处于活动中
	 */
	bool IsInActivity(const std::string& name) const;
	bool IsInActivity(WSPlayer* player, const std::string& name) const;
	bool IsPreActivity(const std::string& name) const;
	bool IsPreActivity(WSPlayer* player, const std::string& name) const;

	/**
	*@brief 是否处于活动中
	*/
	bool IsEndActivity(const std::string& name) const;

	/**
	*@brief 获取活动开始到期时间
	*/
	UInt32 GetActivityPreTime(const std::string& name) const;
	UInt32 GetActivityStartTime(const std::string& name) const;
	UInt32 GetActivityDueTime(const std::string& name) const;

	//注册系统触发计数事件
	void RegSysNotifyCount();

	//获取当前开放的活动
	void GetOpenActivity(WSPlayer* player, std::vector<ActivityInfo>& infos);

	//升级检查开放得活动是否满足条件
	void OnPlayerLevelUp(WSPlayer* player, UInt16 oldLv);

	//商城系统接口
	UInt32 GetMallSale(OpActivityType type);

	/**
	*@brief 获取当前开放的活动
	*/
	ActivityVec& GetOpenActivitys() { return m_Activities; }

	/**
	*@brief 活动是否开放
	*/
	bool IsInActivity(UInt16 playerLevel, UInt32 activityId);

	/**
	*@brief 获取活动开放等级
	*/
	UInt16 GetActivityNeedLevel(UInt32 activityId);

	/**
	*@brief 是否是账号的活动
	*/
	bool IsAccountActivity(UInt32 activityId);

	/**
	*@brief 获取角色绑定的活动
	*/
	void GetRoleActivity(std::vector<ActivityInfo>& activities);

	/**
	*@brief 获取账号绑定的活动
	*/
	void GetAccountActivity(std::vector<ActivityInfo>& activities, WSPlayer* player);

	ActivityRegInfo*  FindActivityRegInfo(UInt32 actId);
	ActivityInfo* FindActivityInfo(std::string& actName);
	ActivityRegInfo*  FindNearestActivityRegInfo(std::string& actName);
public:

	/**
	 *@brief 事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 玩家上线
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief 场景连上来
	 */
	void OnSceneConnected(UInt32 nodeId);

	/**
	*@brief Gate连上来
	*/
	void OnGateConnected(UInt32 nodeId);

	/**
	*@brief 活动开始
	*/
	void OnActivityStart(const std::string &name);

	/**
	 *@brief 活动结束
	 */
	void OnActivityEnd(const std::string& name);
	void OnActivityEnd(ActivityInfo &activity);
	/**
	*@brief 活动结束
	*/
	void OnActivityPrepare(const std::string& name);

	//跨天
	void OnDayChanged(WSPlayer* player);

public: //gm
	void GmOpenActive(UInt32 id, UInt8 state);

private:
	/**
	 *@brief 通知活动状态改变
	 */
	void NotifyActivityStateChange(ActivityInfo &activity, UInt8 state);

	//运营活动任务编解码
	bool OpTaskEncode(const std::vector<OpActTaskData>& tasks, std::string& taskStr, std::string& rewardStr);
	bool OpTaskDecode(std::vector<OpActTaskData>& tasks, const std::string& taskStr, const std::string& rewardStr);

	void _SendOpActivitiesToServer(Avalon::NetConnectionPtr conn);
	void _BrocastOpActivitiesToServers();

	/**
	*@brief 广播活动状态改变到玩家
	*/
	void _BrocastActivityStateChangeToPlayers(Int32 id, const std::string& name, UInt8 state, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime);

	//通知BattleServer活动时间
	void _NotifyBattleActivityTime(const std::string& actName);

	//通知UnionServer活动时间
	void _NotifyUnionActivityTime(UInt32 unionFunType, const std::string& actName);
public:
	//活动注册信息
	ActivityRegMap	m_mapRegInfos;
	//活动列表
	ActivityVec		m_Activities;

	//开服时间的活动注册信息
	ActivityRegMap	m_OpenServerTimeRegInfos;

	// 跨服活动名称表
	std::map<std::string, std::string>	m_mapGlobalActivity;

	//运营注册活动
	OpActRegMap	m_opRegInfos;
	//加载时临时活动
	OpActRegMap m_TmpOpRegInfos;
	
	// gm运营活动加载完毕
	bool m_GmDataLoadOver;

	// 活动页签
	std::vector<ActivityTabInfo*> m_ActivityTabsInfo;

	// Tick定时器，10s
	Avalon::SimpleTimer				m_TickTimer10s;
};

#endif
