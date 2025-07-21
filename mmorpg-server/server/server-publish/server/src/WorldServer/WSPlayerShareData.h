#ifndef _WS_PLAYER_SHARE_DATA_H_
#define _WS_PLAYER_SHARE_DATA_H_

/************************************************************************/
/* 玩家线下数据类                                                                     */
/************************************************************************/

#include "CLTaskDefine.h"
#include "CLObject.h"
#include <CLPlayerDefine.h>
#include <CLRecordCallback.h>

class PlayerShareData;
class WSPlayer;
class WSShareDataLoadCallback;
/**
*@brief 离线数据类状态
*/
enum PlayerShareDataStatus
{

	PSDS_INIT	 = 1,	//创建完成
	PSDS_LOADING = 2,	//加载数据中
	PSDS_LOADED	 = 3,   //数据加载完成
};

/**
*@brief 数据加载完成状态
*/
enum PlayerShareDataLoadedStatus
{
	PSDLS_WORLDDATA_LOADED = 1 << 0,		//World数据已经加载
	PSDLS_SCENEDATA_LOADED = 2 << 1,     //Scene数据已经加载
};

//拉取玩家离线数据请求类型
enum LoadShareDataReqType
{
	LDSDT_NONE = 0,
	LDSDT_GET_MASTERTASK = 1, //师傅查看徒弟师门任务
};


class WSPlayerShareData
{
public:
	WSPlayerShareData();
	~WSPlayerShareData();

public:
	/**
	*@brief 获取玩家角色id
	*/
	UInt64 GetID() { return m_Guid; }
	void SetID(ObjID_t id) { m_Guid = id; }
	/**
	*@brief 获取账号id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() const{ return m_AccId; }

	/**
	*@brief 设置获取玩家下线时间
	*/
	UInt64 GetOfflineTime() { return m_OfflineTime; }
	UInt32 GetOfflineTimeSec() { return (UInt32)(m_OfflineTime / 1000); }
	void SetOfflineTime(UInt64 time) { m_OfflineTime = time; }

	/**
	*@brief 等级
	*/
	UInt16 GetLevel() { return m_level; }
	void SetLevel(UInt16 level) { m_level = level; }

	/**
	*@brief 出师徒弟数量
	*/
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }
	void SetFinSchDiscipleSum(UInt32 sum) { m_FinSchDiscipleSum = sum; };
	void AddFinSchDiscipleSum(UInt32 addValue);
public:
	/**
	*@brief 同步数据 Scene离线数据
	*/
	void SyncDataFromScene(const PlayerSenceShareData& shareData);
	
	/**
	*@brief 同步world数据
	*/
	void SyncDataFromWorld(WSPlayer* player);

	/**
	*@brief 同步师门任务数据
	*/
	void SyncMasterTaskDataFromScene(const MasterTaskShareData& masterTaskShareData);

	/**
	*@brief 获取师门任务数据
	*/
	void  GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData);

public:
	//数据库相关
	void StartLoadData(WSShareDataLoadCallback* cb);

	void LoadGlobalInfo(UInt32 loadReqId);
	void LoadTaskInfo(UInt32 loadReqId);

	/**
	*@brief 加载玩家全局信息回调
	*/
	void OnLoadGlobalInfo(CLRecordCallback *callback);

	/**
	*@brief 加载玩家任务信息回调
	*/
	void OnLoadTaskInfo(CLRecordCallback *callback);

	/**
	*@brief 增加加载项个数，返回本次加载序号
	*/
	UInt8 IncLoadDataCount();

	/**
	*@brief 重置加载状态
	*/
	void ResetLoadDataStatus();

	/**
	*@brief 检查加载项
	*/
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

	/**
	*@brief 加载数据完成
	*/
	void LoadDataFinished();

	/**
	*@brief 加载数据完成
	*/
	void LoadingFailed();

	/**
	*@brief 设置获取状态
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() { return m_Status; }

	bool IsInLoading() { return m_Status == PSDS_LOADING; }

	void SetDataLoadedStatus(UInt8 status);
	bool CheckDataLoadedStatus(UInt8 status);

	bool IsWorldDataLoaded();
	bool IsSceneDataLoaded();

	/**
	*@brief 自增数据加载请求id
	*/
	UInt32 IncLoadReqId();
	
	/**
	*@brief 获取数据加载请求id
	*/
	UInt32 GetLoadReqId() { return m_LoadDataReqId; }

	/**
	*@brief 添加加载数据回调
	*/
	void AddLoadingCallback(WSShareDataLoadCallback* cb);

	/**
	*@brief 离线加载完成回调
	*/
	void LoadFinishedCallback();

	/**
	*#brief 良师值
	*/
	void SetMasterSysGoodTeacherValue(UInt32 value, bool syncDb = false);
	UInt32 GetMasterSysGoodTeacherValue() { return m_SceneshareData.m_MasterTaskShareData.m_GoodTeachValue; }

	/*
	*@brief 设置徒弟学业总成长值
	*/
	void SetDiscipleGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_AcademicTotalGrowth = num; }
	UInt32 GetDiscipleGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_AcademicTotalGrowth; }

	/*
	*@brief 设置徒弟日常任务成长总值
	*/
	void SetMasterDailyTaskGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterDailyTaskGrowth = num; }
	UInt32 GetMasterDailyTaskGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterDailyTaskGrowth; }

	/*
	*@brief 设置徒弟师门学业任务成长总值
	*/
	void SetMasterAcademicTaskGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterAcademicTaskGrowth = num; }
	UInt32 GetMasterAcademicTaskGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterAcademicTaskGrowth; }

	/*
	*@brief 设置徒弟师门赠送装备成长总值
	*/
	void SetMasterGiveEquipGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterGiveEquipGrowth = num; }
	UInt32 GetMasterGiveEquipGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterGiveEquipGrowth; }

	/*
	*@brief 设置徒弟师门升级成长总值
	*/
	void SetMasterUplevelGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterUplevelGrowth = num; }
	UInt32 GetMasterUplevelGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterUplevelGrowth; }

	/*
	*@brief 设置徒弟师门赠送礼包成长总值
	*/
	void SetMasterGiveGiftGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterGiveGiftGrowth = num; }
	UInt32 GetMasterGiveGiftGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterGiveGiftGrowth; }

	/*
	*@brief 师徒组队通过地下城成长值
	*/
	void SetMasterTeamClearDungeonGrowth(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterTeamClearDungeonGrowth = num; }
	UInt32 GetMasterTeamClearDungeonGrowth() { return m_SceneshareData.m_MasterTaskShareData.m_MasterTeamClearDungeonGrowth; }

	/**
	*@brief 添加师门任务数据
	*/
	void AnalysisTaskBriefInfo(TaskBriefInfo& taskinfo, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finishedTime, UInt8 submitCount);
	bool AddMasterDailyTask(const TaskBriefInfo& taskinfo);
	bool AddMasterAcademicTask(const TaskBriefInfo& taskinfo);

	/**
	*@brief 清空师门任务数据
	*/
	void ClearMasterTaskData();
private:
	//玩家guid
	ObjID_t			m_Guid;
	
	//玩家账号id
	UInt32			m_AccId;
	
	//下线时间,单位毫秒
	UInt64			m_OfflineTime;

	//玩家等级
	UInt16			m_level;

	//玩家出师徒弟数量
	UInt32			m_FinSchDiscipleSum;
private:
	//数据加载请求序列id
	UInt32          m_LoadDataReqId;

	//加载项个数
	UInt8			m_LoadDataCount;

	//加载进度
	UInt64			m_LoadDataSerial;

	//对象状态
	UInt8			m_Status;

	//数据加载完成状态
	UInt8			m_DataLoadedStatus;

	// 拉取离线玩家数据请求类型 LoadType
	UInt32			m_LoadPlayerType;

	// 请求离线玩家的人
	ObjID_t			m_RequestTarget;

	// 加载shardata完成回调
	std::vector<WSShareDataLoadCallback*>    m_LoadingCallback;;

private:
	//玩家Sence离线数据
	PlayerSenceShareData m_SceneshareData;
};

/**
*@brief 玩家访问器
*/
typedef CLVisitor<WSPlayerShareData> PlayerShareDataVisitor;

/**
*@brief 查询玩家离线数据返回
*/
class SelectRoleShareDataCallback : public CLRecordCallback
{
public:
	SelectRoleShareDataCallback(WSPlayerShareData* shareData, UInt32 loadReqId)
		:m_PlayerId(shareData->GetID()), m_LoadSerial(shareData->IncLoadDataCount()), m_LoadReqId(loadReqId){}
	~SelectRoleShareDataCallback(){}

public:
	virtual void OnFinished(WSPlayerShareData* shareData) = 0;
	virtual void OnFailed(WSPlayerShareData* shareData) = 0;

	UInt32 GetLoadDataReqId() { return m_LoadReqId; }

private:
	void OnFinished();

	void OnLoadFailed();

private:
	//玩家id
	ObjID_t m_PlayerId;
	//加载序号
	UInt8	m_LoadSerial;
	//加载请求序列id
	UInt32  m_LoadReqId;
};

#endif