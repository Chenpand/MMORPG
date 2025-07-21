#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <CLGameDefine.h>
#include <map>
#include <vector>
#include <string>
#include "CLActivityDefine.h"

class Player;
class DungeonDataEntry;
struct Buff;

#define SIGN_IN_REFRESH_NUM		30			//签到刷新天数


/**
 *@brief 活动
 */
class ActivityMgr : public Avalon::Singleton<ActivityMgr>
{
	typedef std::vector<ActivityInfo>	ActivityVec;
	typedef std::map<ObjID_t, ActivityVec>	TribeActivityMap;

public:
	ActivityMgr();
	~ActivityMgr();

public:
	/**
	 *@brief 开启关闭活动
	 */
	void OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime, UInt8 state);
	void CloseActivity(UInt32 id);
	
	/**
	 *@brief 开启关闭部落活动
	 */
	void OpenTribeActivity( ObjID_t uTribeId, const std::string& name, UInt16 level, UInt32 dueTime);
	void CloseTribeActivity(ObjID_t uTribeId, const std::string& name);

	/**
	 *@brief 设置活动
	 */
	void SetActivities(const ActivityVec& activities, const TribeActivityMap &tribeactivities);

	/**
	 *@brief 是否处于活动中
	 */
	bool IsInActivity(const std::string& name) const;
	bool IsInActivity(Player* player, const std::string& name) const;
	bool IsInActivity(UInt32 activeId) const;
	bool IsInActivity(UInt32 activeId, UInt32 time) const;		//给定时间判断是否在活动期间
	bool IsInActivity(UInt32 activeId, Player* player) const;

	// 是否碰上活动开始
	bool IsJustInActivity(UInt32 activeId, Player* player) const;

	//获得活动信息
	bool GetActivityInfo(UInt32 activeId, ActivityInfo& info);

	/**
	*@brief 访问开放的活动
	*/
	void VisitOpenActivities(Player* player, const std::function<bool(const ActivityInfo&)>& F);

	/**
	*@brief 获取活动时间
	*/
	bool GetActivityTime(UInt32 activeId, UInt32& startTime, UInt32& endTime);

	/**
	 *@brief 是否处于部落活动中
	 */
	bool IsInTribeActivity(ObjID_t uTribeId, const std::string& name) const;

	/**
	 *@brief 获取活动开始时间                                                                     
	 */
	UInt32 GetActivityStartTime(const std::string& name) const;
	
	//获取进行中的活动
	void GetOpenActivity(Player* player, std::vector<ActivityInfo>& activities);

	//通知玩家接受活动任务
	void NotifyPlayerAcceptActiveTask(UInt32 id);

	//通知玩家删除活动任务
	void NotifyPlayerRemoveActiveTask(UInt32 id);

	//通知玩家活动结束
	void NotifyPlayerActiveClosed(UInt32 id);

	void OnTick(const Avalon::Time &now);
	
	/**
	*@brief 运营活动
	*/

	//加载运营活动配置
	void OnLoadOpActData(OpActRegMap& activeData);

	//加载当前运营活动信息
	//void OnLoadOpActInfo(OpActivityVec& activeInfo);

	//同步当前运营活动状态
	void OnOpActStateChange(OpActivityRegInfo& info);

	//活动开启
	void OnOpActOpen(OpActivityRegInfo* info);

	//活动结束(从进行中到结束)
	void OnOpActClosed(OpActivityRegInfo* info);

	//获得运营活动状态BY运营活动id
	//ActivityState	GetOpActState(UInt32 id);

	//获得所有开放的运营活动信息
	void GetOpenOpActs(std::vector<OpActivityRegInfo*>& opActs);

	//获得所有运营活动信息
	void GetOpActs(std::vector<OpActivityRegInfo*>& opActs);

	//获得运营活动配置任务BY任务dataid
	OpActTaskData* GetOpActTaskData(UInt32 dataId);

	//获得所有运营活动任务配置BY模板类别
	bool GetOpActTaskDataVec(UInt32 tmpType, std::vector<OpActTaskData*>& datas);

	//获得运营活动配置BY dataId
	OpActivityRegInfo* GetOpActData(UInt32 dataId);

	//获得运营活动配置BY tmpId
	void GetOpActDataByTmpType(OpActivityType type, std::vector<OpActivityRegInfo*>& activities);
	//获得运营活动配置BY tmpId
	std::vector<OpActivityRegInfo*> GetOpActDataByTmpTypes(UInt32 tmpType, bool mustInActivity = true);

	//通知玩家运营活动状态改变
	void NotifyPlayerOpActChange(OpActivityRegInfo& info);

	//是否处于运营活动
	bool IsInOpActivity(UInt32 activityId);
	bool IsInOpActivity(UInt32 activityId, Player* player);
	bool IsInOpActivity(const OpActivityRegInfo* opAct, Player* player);

	//是否处于运营活动
	bool IsInOpActivityByTmpType(UInt32 tmpId, UInt16 playerLevel);

	//获取运营活动加成系数
	UInt32 GetOpActivityAddition(UInt32 tmpId, Player* player);

	void GetVeteranReturnOpActBuffs(Player* player, std::vector<Buff*>& veteranBuffs, const DungeonDataEntry* dungeonData);

	//获取进行中的活动id
	UInt32 GetOpenActivityIdByTmplateType(UInt32 actTmpType);
	OpActivityRegInfo* GetOpenActivityRegInfoByTmplateType(Player* player, OpActivityType actTmpType);

	// 检查运营活动数据合法
	bool CheckOpActivityDataValid(Player* player, UInt32 opActId, OpActivityType opActType);

	// 加载活动账号数据
	void LoadActivityAccRecord(UInt32 accid, ActivityAccRecord* record);
	// 查找账号活动次数
	ActivityAccRecord* FindAccActivityRecord(UInt32 accid, UInt32 actId);
	// 增加账号活动次数
	void AddAccActivityRecord(UInt32 accid, ActivityAccRecord* record);
	// 更新账号活动次数
	void UpdateAccActivityRecord(UInt32 accid, const ActivityAccRecord* record, bool isInsert = false);
	// 删除过期的账号活动记录
	void DelExpireAccActivityRecord(UInt32 accid);

private:
	void _AddOpActData(OpActivityRegInfo& info);

public:
	//活动
	ActivityVec m_Activities;

	//部落活动
	TribeActivityMap	m_TribeActivities;

	//运营活动配置,tmptype为key	
	std::map<UInt32, std::vector<OpActivityRegInfo*>>	m_tmpTypeToOpActDatas;
	//运营活动配置,dataId为key	
	std::map<UInt32, OpActivityRegInfo*>				m_dataIdToOpActData;
	//运营活动任务,dataId为key
	std::map<UInt32, OpActTaskData*>					m_dataIdToTaskDatas;
	//运营活动任务,tmptype为key
	std::map<UInt32, std::vector<OpActTaskData*>>		m_tmpTypeToTaskDatas;

	// 账号运营活动记录，key->账号(key->活动id)
	std::map<UInt32, std::map<UInt32, ActivityAccRecord*>>	m_accActivityRecordMap;
};

#endif
