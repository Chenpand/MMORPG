#ifndef _CL_MASTER_SYSTEM_DEFINE_H_
#define _CL_MASTER_SYSTEM_DEFINE_H_

/**
*@brief 同门关系类型
*/
enum MasterSectRelationType
{
	MSRELATION_MASTER = 1,		//师傅
	MSRELATION_BROTHER = 2,		//师兄弟
	MSRELATION_DISCIPLE = 3,	//徒弟

	MSRELATION_MAX,
};

/**
*@brief 同门关系同步类型
*/
enum MasterSectRelationSyncType
{
	MSRST_MASTER = 1 << MSRELATION_MASTER,
	MSRST_BROTHER = 1 << MSRELATION_BROTHER,
	MSRST_DISCIPLE = 1 << MSRELATION_DISCIPLE,
};

/**
*@brief 同门关系属性
*/
enum MasterSectRelationAttr
{
	MSRA_INVALID,
	MSRA_NAME,		//对方名字(std::string)
	MSRA_LEVEL,		//对方等级(UInt16)
	MSRA_OCCU,		//对方职业(UInt8)
	MSRA_TYPE,		//关系(UInt8)
	MSRA_VIPLV,		//vip等级(UInt8)
	MSRA_STATUS,	//状态(UInt8)
	MSRA_ISFINSCH,  //是否出师
};

/**
*@brief 拜师收徒问卷玩家在线类型
*/
enum QuestionnaireActiveTimeType
{
	QAT_INVALID = 0,
	QAT_ALLDAY = 1, //24小时活跃
	QAT_DAY = 2,    //白天
	QAT_NIGHT = 3,  //晚上
	QAT_MAX = QAT_NIGHT,
};

/**
*@brief 拜师收徒问卷师傅类型
*/
enum QuestionnaireMasterType
{
	QMT_INVALID = 0,
	QMT_STRENGTH = 1,	 //实力强悍型
	QMT_RESPONSIBLE = 2, //认真负责型
	QMT_CHITCHAT = 3,	 //聊天社交型
};

/**
*@brief 师门日常任务状态
*/
enum MasterDailyTaskState
{
	MDTST_UNPUBLISHED = 0,  //未发布
	MDTST_UNCOMPLETE = 1,   //已发布，未完成
	MDTST_UNRECEIVED = 2,   //已完成，可领取
	MDTST_RECEIVEED = 3,	//已完成，已领取
	MDTST_UNPUB_UNRECE = 4, //未发布，可领取 师傅才可能有这个状态
};
//enum MasterDailyTaskState
//{
//	MDTST_UNPUBLISHED = 0,  //未发布
//	MDTST_UNREPORTED = 1,   //已发布，未汇报
//	MDTST_UNRECEIVED = 2,   //已汇报，可领取
//	MDTST_RECEIVEED = 3,	//已汇报，已领取
//	MDTST_UNPUB_UNRECE = 4, //未发布，可领取 师傅才可能有这个状态
//};

/**
*@brief 师门每日任务完成状态
*/
enum MasterDailyTaskCompleteState
{
	MDTCS_NOTCOMPLTETEED = 0, //未完成
	MDTCS_COMPLTETEED = 1,    //完成
};

/**
*@brief 师门系统礼包类型
*/
enum MasterSysGiftType
{
	MSGT_MASTER_DAILYTASK = 1,     //师傅日常师门任务礼包
	MSGT_DISCIPLE_DAILYTASK = 2,   //徒弟日常师门任务礼包
	MSGT_DISCIPLE_ACADEMIC = 3,    //徒弟成长礼包
	MSGT_FISNISHCL_MASTER = 4,     //师傅出师礼包
	MSGT_FISNISHCL_DISCIPLE = 5,   //徒弟出师礼包
};

/**
*@brief 领取师门日常任务奖励类型
*/
enum MasterSysReceiveDailyTaskRewardType
{
	MSRDTR_MASTER = 1,    //师傅领取
	MARDTR_DISCIPLE = 2,  //徒弟领取
};

/**
*@brief 师门成长奖励领取状态
*/
enum MasterAcademicRewardRecvState
{
	MARRS_NOTRECVED = 0,  //没有领取
	MARRS_RECVED = 1,     //已经领取
};

/**
*@brief 自动出师发起类型
*/
enum MasterSysAutoFinSchType
{
	MSAFST_MASTER = 1, //师傅发起
	MSAFST_DISCIPLE = 2, //徒弟发起
	MSAFST_NORMAL = 3, //正常出师
};

/**
*@brief 师徒公告类型
*/
enum  RelationAnnounceType
{
	RAT_MASTER = 1, //师傅发公告
	RAT_DISCIPLE = 2, //徒弟发公告
};

#endif