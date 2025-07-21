#ifndef _WS_MASTER_SYSTEM_H
#define _WS_MASTER_SYSTEM_H

#include <AvalonSimpleTimer.h>
#include "CLRelationDefine.h"
#include "MasterSectRelation.h"
#include "CLMasterSystemDefine.h"
#include "CLRelationProtocol.h"

class WSPlayer;
class MasterSectRelation;
class WSPlayerShareData;

/*
*@brief 师徒问卷调查
*/
class  MsaterDiscipleQuestionnaire
{
public:
	MsaterDiscipleQuestionnaire()
		: guid(0), owner(0), activeTimeType(0), masterType(0), regionId(0), declaration("")
	{}
	~MsaterDiscipleQuestionnaire() {}
public:
	bool SetDataFromClient(CLProtocol::WorldSetDiscipleQuestionnaireReq& req);
	bool SetDataFromClient(CLProtocol::WorldSetMasterQuestionnaireReq& req);
	void LoadFromDB(CLRecordCallback* callback);
	void SynToClient(WSPlayer* player);
	void SynToDB(WSPlayer* player);

public:
	ObjID_t		guid;			//主键
	ObjID_t		owner;			//玩家id
	UInt8		activeTimeType; //在线时间类型
	UInt8		masterType;		//师傅类型
	UInt8		regionId;		//地区id
	std::string	declaration;    //宣言
};

class WSMasterSystem
{
	typedef std::map<ObjID_t, MasterSectRelation*>	MasterSectRelationMap;
	typedef std::vector<ObjID_t> IdVec;
	typedef std::map<ObjID_t, UInt64> TimeStampMap;

public:
	WSMasterSystem();
	~WSMasterSystem();

public:
	/**
	*@breif 设置获取拥有者
	*/
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }
	
	/*
	*@brief 检查刷同门关系
	*/
	void CheckAndMakeRelations();

	/**
	*@brief 加载所有同门
	*/
	void LoadRelations();

	/**
	*@brief 加载数据返回
	*/
	void OnLoadDataRet(CLRecordCallback* callback);

	/*
	*#brief 加载玩家数据
	*/
	void OnLoadPlayerDataRet(CLRecordCallback* callback);

	/**
	*@brief 心跳
	*/
	void OnHeartbeat(const Avalon::Time& now);

	/*
	*@brief 更新关系
	*/
	void UpdateRelation();

	/**
	*@brief 查找、移除、增加关系
	*/
	bool AddRelation(MasterSectRelation* relation);

	MasterSectRelation* GetRelation(ObjID_t id);

	MasterSectRelation* GetRelation(UInt8 type, ObjID_t id);

	bool HasRelation(UInt8 type);

	bool HasRelation(ObjID_t id);

	bool RemoveRelation(UInt8 type, ObjID_t id);

	void ClearRelations();

	void ClearRelations(UInt8 type);

	
	/**
	*@brief 遍历关系
	*/
	void VisitRelations(MasterSectRelationVisitor& visitor);

	void VisitRelations(UInt8 type, MasterSectRelationVisitor& visitor);

public: 
	/*
	*@brief 获取问卷调查
	*/
	MsaterDiscipleQuestionnaire& GetQuestionnaire() { return m_Questionnaire; }

	/*
	*@brief 获取玩家最近被推荐师傅时间戳
	*/
	UInt64 GetRecommanMasterTimeStamp(ObjID_t masterId);

	/*
	*@brief 获取玩家最近被推荐徒弟时间戳
	*/
	UInt64 GetRecommanDiscipleTimeStamp(ObjID_t discipleId);

	/*
	*@brief 设置玩家最近被推荐师傅时间戳
	*/
	void SetRecommanMasterTimeStamp(ObjID_t masterId, UInt64 timestamp);

	/*
	*@brief 设置玩家最近被推荐徒弟时间戳
	*/
	void SetRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 timestamp);

	/*
	*@brief 检查玩家最近被推荐师傅时间戳
	*/
	bool CheckRecommanMasterTimeStamp(ObjID_t masterId, UInt64 curr_timestamp);

	/*
	*@brief 检查玩家最近被推荐徒弟时间戳
	*/
	bool CheckRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 curr_timestamp);

	/*
	*@brief 检查调查问卷匹配数目
	*/
	static UInt8 CalcQuestionnaireMatchNum(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief 处理师门任务发布成功
	*/
	static void HandlePublishMasterTaskSuccess(ObjID_t masterId, ObjID_t discipleId);

	/**
	*@brief 拜师惩罚时间戳
	*/
	void SetApprenticMasterPunishTime(UInt64 timestamp, bool saveDb = true);
	UInt64 GetApprenticMasterPunishTime() { return m_ApprenticMasterPunishTime; }
	bool CheckApprenticMasterPunishTime();
	
	/**
	*@brief 收徒惩罚时间戳
	*/
	void SetRecruitDisciplePunishTime(UInt64 timestamp, bool saveDb = true);
	UInt64 GetRecruitDisciplePunishTime() { return m_RecruitDisciplePunishTime; }
	bool CheckRecruitDisciplePunishTime();

	/**
	*@brief 良师值
	*/
	void SetGoodTeacherValue(UInt32 value, bool saveDb = false);
	UInt32 GetGoodTeacherValue() { return m_GoodTeacherValue; }

	/**
	*@brief 强制解除师傅设置惩罚时间戳
	*/
	void SetApprenticMasterPunishTimeOnRemoveMaster(UInt64 currTime, UInt64 masterOfflineTime);

	/**
	*@brief 强制解除徒弟设置惩罚时间戳
	*/
	void SetRecruitDisciplePunishTimeOnRemoveDisciple(UInt64 currTime, UInt64 discipleOfflineTime);

	/**
	*@brief 同步惩罚时间戳
	*/
	void SyncPunishTimeToClient();

	/**
	*@brief 徒弟学业总成长值
	*/
	void SetAcademicTotalGrowth(UInt32 value) { m_AcademicTotalGrowth = value; }
	UInt32 GetAcademicTotalGrowth() { return m_AcademicTotalGrowth; }

	/**
	*@brief 师门日常任务成长总值
	*/	
	void SetMasterDailyTaskGrowth(UInt32 value) { m_MasterDailyTaskGrowth = value; }
	UInt32 GetMasterDailyTaskGrowth() { return m_MasterDailyTaskGrowth; }

	/**
	*@brief 师门学业任务成长总值
	*/
	void SetMasterAcademicTaskGrowth(UInt32 value) { m_MasterAcademicTaskGrowth = value; }
	UInt32 GetMasterAcademicTaskGrowth() { return m_MasterAcademicTaskGrowth; }

	/**
	*@brief 师门赠送装备成长总值
	*/
	void SetMasterGiveEquipGrowth(UInt32 value) { m_MasterGiveEquipGrowth = value; }
	UInt32 GetMasterGiveEquipGrowth() { return m_MasterGiveEquipGrowth; }

	/**
	*@brief 师门升级成长总值
	*/
	void SetMasterUplevelGrowth(UInt32 value) { m_MasterUplevelGrowth = value; }
	UInt32 GetMasterUplevelGrowth() { return m_MasterUplevelGrowth; }
	
	/**
	*@brief 师门赠送礼包成长总值
	*/
	void SetMasterGiveGiftGrowth(UInt32 value) { m_MasterGiveGiftGrowth = value; }
	UInt32 GetMasterGiveGiftGrowth() { return m_MasterGiveGiftGrowth; }

	/**
	*@brief 出师徒弟数量
	*/
	void SetFinSchDiscipleSum(UInt32 value) { m_FinSchDiscipleSum = value; }
	void AddFinSchDiscipleSum(UInt32 addValue);
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }

	/**
	*brief 世界频道公告禁止时间戳
	*/
	UInt32 GetAnnounceProhibitTimeStamp() { return m_AnnounceProhibitTimeStamp; }
	void SetAnnounceProhibitTimeStamp(UInt32 timeStamp) { m_AnnounceProhibitTimeStamp = timeStamp; }
public:
	/*
	*@brief 处理新的师徒关系成立
	*/
	static void OnMakeMasterDisciple(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief 处理移除徒弟
	*/
	static void OnRemoveDisciple(WSPlayer* master, ObjID_t discipleId);

	/*
	*@brief 处理移除师傅
	*/
	static void OnRemoveMaster(WSPlayer* disciple, ObjID_t masterId);

	/*
	*@brief 徒弟移除后的解除关系
	*/
	static void ClearRelationsOnRemoveDisciple(WSPlayer* disciple);

	/*
	*@brief 处理拜师成功后添加新的师兄同门关系
	*/
	static void MakeBrotherRelationByMasterMaked(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief 师傅数据离线加载后添加师傅,师兄关系
	*/
	static void MakeMasterAndBrotherRelationByMasterDataLoaded(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief 建立所有徒弟的同门关系
	*/
	static void MakeAllDiscipleRelations(WSPlayer* master);

	/*
	*@brief 建立师兄同门关系
	*/
	static void MakeBrotherRelation(WSPlayer* older, WSPlayer* younger);

	/*
	*@brief 建立师傅同门关系
	*/
	static void MakeMasterRelation(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief 建立徒弟同门关系
	*/
	static void MakeDiscipleRelation(WSPlayer* master, WSPlayer* disciple);

	/**
	*@brief 处理领取师门日常任务奖励
	*/
	UInt32 OnReceiveMasterDailyTaskReward(UInt8 type, ObjID_t discipeleId);

	/*
	*@brief 处理领取师门成长奖励
	*/
	UInt32 OnReceiveMasterAcademicReward(UInt32 masterSysGiftId);

	/*
	*@brief 处理出师
	*/
	static void  HandleDiscipleFinishSchool(WSPlayer* master, WSPlayer* disciple);

	/**
	*@brief 处理自动出师
	*/
	void OnAutomaticFinishSchool(ObjID_t targetId);

	/**
	*@brief 师傅自动出师
	*/
	void OnMasterAutomaticFinishSchool(ObjID_t discipleId, WSPlayerShareData* discipleShareData);

	/**
	*@brief 徒弟自动出师
	*/
	void OnDiscipleAutomaticFinishSchool(ObjID_t masterId, WSPlayerShareData* masterShareData);

	/**
	*@brief 自己是徒弟出师修改同门数据
	*/
	void UpdateMasterSectRelationOnSelfFinSch(ObjID_t masterId);

	/**
	*@brief 自己是师傅强制自动出师某个徒弟修改同门数据
	*/
	void UpdateMatserSectRelationOnAutoFinSchOneDiscple(ObjID_t discipleId);

	/**
	*@brief 处理领取师门日常奖励完成
	*/
	void OnRceiveMasterDailyTaskRewardSucc(UInt8 type, ObjID_t discipeleId);

	/**
	*@brief 处理领取师门成长奖励完成
	*/
	void OnRceiveMasterAcademicRewardSucc(UInt32 masterSysGiftId);

	/**
	*@brief 处理师傅领取出师礼包完成
	*/
	void OnReceiveMasterFinishSchoolRewardSucc(UInt32 giftId, ObjID_t discipleId);

	/**
	*@brief 处理徒弟领取出师礼包完成
	*/
	void OnReceiveDiscipleFinishSchoolRewardSucc(UInt32 giftId, ObjID_t masterId);

	/**
	*@brief 处理徒弟师门日常任务完成
	*/
	void OnMasterDailyTaskComplete();

	/**
	*@brief 发送出师公告
	*/
	static void SendFinishSchoolAnnounce(const std::string& masterName, const std::string& discipleName);

	/**
	*@brief  处理角色永久删除
	*/
	static void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief 广播出师事件
	*/
	static void BroadcastFinschEvent(const std::string& masterName, const std::string& discipleName);
public: //玩家事件
	/**
	*@brief 玩家上线
	*/
	void OnOnline();

	/**
	*@brief 玩家下线
	*/
	void OnOffline();

	/**
	*@brief 逻辑日更
	*/
	void OnLogicDayChange();

	/**
	*@brief 等级改变
	*/
	void OnLevelChanged();

	/**
	*@brief vip等级改变
	*/
	void OnVipLvlChanged();

	/**
	*@brief 职业改变
	*/
	void OnOccuChanged();

	/**
	*@brief 状态改变
	*/
	void OnStatusChanged(UInt8 status);

	/**
	*@brief 名字改变
	*/
	static void OnNameChanged(ObjID_t playerId, const std::string& name);
public: //网络
	/*
	*@brief 通知玩家删除关系
	*/
	void SendPlayerDelRelation(MasterSectRelationType type, ObjID_t tarId);

public:
	/**
	*@brief 同步到数据库
	*/
	void SyncToDB();

	/**
	*@brief 同步等级到数据库
	*/
	static void SyncLevelToDB(ObjID_t playerId, UInt16 level);

	/**
	*@brief 同步vip等级到数据库
	*/
	static void SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl);

	/**
	*@brief 同步职业到数据库
	*/
	static void SyncOccuToDB(ObjID_t playerId, UInt8 occu);

	/**
	*@brief 同步名字到数据库
	*/
	static void SyncNameToDB(ObjID_t playerId, const std::string& name);

	/**
	*@brief 同步师门任务状态
	*/
	static void SyncMasterDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state);
	static void SyncDiscipleDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state);

	/**
	*@brief 同步出师标示到数据库
	*/
	static void SyncFinSchMarkToDB(ObjID_t playerId);
public:
	/**
	*@brief 师门日常任务师傅奖励领取状态
	*/
	void SetRecvingMasterDailyTaskRewardState(UInt64 timesec);
	void ResetRecvingMasterDailyTaskRewardState();

	/**
	*@brief 师门日常任务徒弟奖励领取状态
	*/
	void SetRecvingDiscipleDailyTaskRewardState(UInt64 timesec);
	void ResetRecvingDiscipleDailyTaskRewardState();

	/**
	*@brief 检查领奖状态标识
	*/
	void CheckRecvRewardState(const Avalon::Time& now);
private:
	//拥有者
	WSPlayer*					m_pOwner;
	
	//同门关系列表
	MasterSectRelationMap		m_Relations[MSRELATION_MAX];
	
	//简单定时器
	Avalon::SimpleTimer			m_SyncDBTimer;
	
	//师徒问卷调查
	MsaterDiscipleQuestionnaire m_Questionnaire;
	
	//查找师傅被推荐时间戳
	TimeStampMap				m_MasterRecommanTimestamp;
	
	//查找徒弟被推荐时间戳
	TimeStampMap				m_DisipleRecommanTimestamp;
	
	//查找师傅徒弟冷却时间
	UInt32						m_RecommanCoolTime;
	
	//拜师惩罚时间戳
	UInt64						m_ApprenticMasterPunishTime;
	
	//招收徒弟惩罚时间戳
	UInt64						m_RecruitDisciplePunishTime;
	
	//良师值
	UInt32						m_GoodTeacherValue;

	//徒弟学业总成长值
	UInt32						m_AcademicTotalGrowth;

	//师门日常任务成长总值
	UInt32					m_MasterDailyTaskGrowth;

	//师门学业任务成长总值
	UInt32					m_MasterAcademicTaskGrowth;

	//师门赠送装备成长总值
	UInt32					m_MasterGiveEquipGrowth;

	//师门升级成长总值
	UInt32					m_MasterUplevelGrowth;

	//师门赠送礼包成长总值
	UInt32					m_MasterGiveGiftGrowth;

	//出师徒弟数量
	UInt32					m_FinSchDiscipleSum;

	//师徒世界频道公告禁止时间戳
	UInt32					m_AnnounceProhibitTimeStamp;

	//是否正在领取师门师傅日常任务奖励
	bool					m_IsRecvingMasterDailyTaskReward;

	//师门师傅日常任务奖励状态重置时间戳
	UInt64					m_RecvingMasterDailyTaskRewardTime;

	//是否正在领取师门徒弟日常任务奖励
	bool					m_IsRecvingDiscipleDailyTaskReward;

	//师门徒弟日常任务奖励状态重置时间戳
	UInt64					m_RecvingDiscipleDailyTaskRewardTime;
};

#endif