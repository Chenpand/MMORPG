#ifndef _CL_ABNORMAL_TRANS_DEFINE_H_
#define _CL_ABNORMAL_TRANS_DEFINE_H_

#include "CLItemTypeDefine.h"

/*
*@brief 异常交易处理状态
*/
enum AbnormalTransHandleStatus
{
	// 非法
	ATHS_INVALID = 0,
	// 等待冻结
	ATHS_WAIT_FREEZE,
	// 冻结保释期
	ATHS_FROZEN_TAIL,
	// 不再返还冻结金
	ATHS_NEVER_BACK_MONEY,
	// 超时
	ATHS_TIME_OUT,
	// 处理结束
	ATHS_END_HANDLE,
	// 已合并
	ATHS_MERGE_OVER,
	// 手动结束
	ATHS_MANUAL_END_HANDLE,
};

/*
*@brief 异常交易冻结金返还途径
*/
enum AbnormalTransFrozenMoneyBackWay
{
	ATFROZEN_MONEY_BACK_WAY_NONE = 0,
	// 通过充值返还
	ATFROZEN_MONEY_BACK_WAY_CHARGE = 1,
	// 通过每日活跃度返还
	ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE = 1 << 1,
};

/*
*@brief 异常交易冻结对象
*/
enum AbnormalTransFreezeObject
{
	// 冻结卖家
	AT_FREEZE_OBJECT_SELLER = 0,
	// 冻结买家
	AT_FREEZE_OBJECT_BUYER,
};

/*
*@brief 异常交易数据库存储信息
*/
struct AbnormalTransDBInfo
{
	AbnormalTransDBInfo()
	{
		buyer = 0;
		seller = 0;
		transTime = 0;
		transId = 0;
		itemUId = 0;
		itemDataId = 0;
		itemNum = 0;
		moneyType = MONEY_INVALID;
		transAmount = 0;
		freezeObj = AT_FREEZE_OBJECT_SELLER;
		backWay = ATFROZEN_MONEY_BACK_WAY_NONE;
		isFrozenPermanet = false;
		isTreasure = true;
		mailNotifyType = 0;
		unfrozenAmount = 0;
		isNeedRemovePlayerTransMoney = false;
	}

	const std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "Abnormal trans DB info(" << transId << ")";
		return oss.str();
	}

	// 买家
	ObjID_t buyer;
	// 卖家
	ObjID_t seller;
	// 交易时间
	UInt32 transTime;
	// 交易id
	ObjID_t transId;
	// 交易道具uid
	ObjID_t itemUId;
	// 交易道具表id
	UInt32 itemDataId;
	// 交易道具数量
	UInt32 itemNum;
	// 交易金类型
	MoneyType moneyType;
	// 交易金额
	UInt32 transAmount;
	// 惩罚冻结对象
	AbnormalTransFreezeObject freezeObj;
	// 冻结金返还途径
	UInt8 backWay;
	// 是否永久冻结
	bool isFrozenPermanet;
	// 是否珍品
	bool isTreasure;
	// 邮件通知类型(0:旧通知,1:新通知,目前只有交易取消的才用1)
	UInt8 mailNotifyType;
	// 未冻结金额
	UInt32 unfrozenAmount;
	// 是否需要追缴扣除玩家交易金
	bool isNeedRemovePlayerTransMoney;
};

#endif