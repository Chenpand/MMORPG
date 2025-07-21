#include "AbnormalTransactionMgr.h"

#include <CLAuctionProtocol.h>
#include <CLMailProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <CLRecordClient.h>
#include <TimeUtil.h>
#include <CLAuctionSysValueDataEntry.h>

#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "SSApplication.h"


class QueryWaitFreezeAbnormalTransactionCallback : public CLRecordCallback
{
public:
	QueryWaitFreezeAbnormalTransactionCallback(ObjID_t playerId) : m_OwnerId(playerId) {}

	virtual void OnFinished()
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_OwnerId);

		if (player != NULL)
			player->GetAbnormalTransactionMgr().OnQueryWaitFreezeAbnormalTransactionRecordRet(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Player(" << m_OwnerId << ") select t_abnormal_transaction failed!" << LogStream::eos;
	}

private:
	ObjID_t m_OwnerId;
};

AbnormalTransactionMgr::AbnormalTransactionMgr()
{
	m_Owner = NULL;
	m_TailDeadline = 0;
	m_PlayerOnlineLoadDBCount = 0;
}

AbnormalTransactionMgr::~AbnormalTransactionMgr()
{
	for (auto elem : m_AbnormalTransactionRecords)
	{
		delete elem;
	}
	m_AbnormalTransactionRecords.clear();
}

bool AbnormalTransactionMgr::AddAbnormalRecord(AbnormalTransactionRecord* record)
{
	if (!record) return false;
	m_AbnormalTransactionRecords.push_back(record);
	return true;
}

AbnormalTransactionRecord* AbnormalTransactionMgr::FindAbnormalRecord(ObjID_t id)
{
	for (auto elem : m_AbnormalTransactionRecords)
	{
		if (!elem) continue;

		if (elem->GetID() == id) return elem;
	}
	return NULL;
}

void AbnormalTransactionMgr::VisitAbnormalRecord(const std::function<bool(AbnormalTransactionRecord*)>& F)
{
	for (auto elem : m_AbnormalTransactionRecords)
	{
		if (!elem) continue;

		if (!F(elem)) return;
	}
}

void AbnormalTransactionMgr::OnOnline()
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	_OnCheckNotifyAbnormalTransaction();
	QueryWaitFreezeAbnormalTrans();
}

void AbnormalTransactionMgr::OnCharge(UInt32 chargeNum)
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	if (chargeNum == 0) return;

	AbnormalTransactionRecord* frozenTailRecord = NULL;

	VisitAbnormalRecord([&frozenTailRecord](AbnormalTransactionRecord* record)->bool{
		if (record != NULL && record->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			if (record->CanUnfreezeByCharge())
			{
				frozenTailRecord = record;
				return false;
			}
		}
		return true;
	});

	if (!frozenTailRecord)
	{
		return;
	}

	UInt32 rate = 0;
	if (frozenTailRecord->IsOnlyUnfreezeByCharge())
	{
		rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHARGE_UN_FREEZE_MONEY_RATE_TWO);
	}
	else
	{
		rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHARGE_UN_FREEZE_MONEY_RATE);
	}

	if (rate == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error rate!" << LogStream::eos;
		return;
	}

	UInt32 backMoney = chargeNum * rate;

	// 非法
	if (frozenTailRecord->GetFrozeAmount() < frozenTailRecord->GetBackAmount())
	{
		frozenTailRecord->SetHandleStatus(ATHS_INVALID);
		ErrorLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "Invalid amount: froze amount(" << frozenTailRecord->GetFrozeAmount() 
			<< ") is more than back amount(" << frozenTailRecord->GetBackAmount() << ")!" << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "Start unfreeze money(" << backMoney << "), charge " << chargeNum << LogStream::eos;

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ABNORMAL_TRANS_CHARGE_BACK_MONEY, frozenTailRecord->GetID());

	UInt32 unfreezeMoney = backMoney;

	UInt32 restBackAmount = frozenTailRecord->GetFrozeAmount() - frozenTailRecord->GetBackAmount();
	if (restBackAmount <= backMoney)
	{
		// 全部返还了

		unfreezeMoney = restBackAmount;

		frozenTailRecord->IncBackAmount(restBackAmount);
		frozenTailRecord->SetHandleStatus(ATHS_END_HANDLE);

		// 全部返还了需要终止相关数据
		_TerminateAbnormalTransData();

		if (frozenTailRecord->GetMoneyType() == MONEY_GOLD)
		{
			GetOwner()->AddGold(reason.c_str(), restBackAmount);
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "All money back, back money(type=" << frozenTailRecord->GetMoneyType() 
			<< ", num=" << restBackAmount << ")." << LogStream::eos;
	}
	else
	{
		frozenTailRecord->IncBackAmount(backMoney);

		if (frozenTailRecord->GetMoneyType() == MONEY_GOLD)
		{
			GetOwner()->AddGold(reason.c_str(), backMoney);
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "Back money(type=" << frozenTailRecord->GetMoneyType() 
			<< ", num=" << backMoney << ")." << LogStream::eos;
	}

	// 发送邮件

	{
		UInt32 restBackMoney = 0;
		if (frozenTailRecord->GetFrozeAmount() < frozenTailRecord->GetBackAmount())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "'s froze amount(" << frozenTailRecord->GetFrozeAmount()
				<< ") is more than back amount(" << frozenTailRecord->GetBackAmount() << ")!" << LogStream::eos;
		}
		else
		{
			restBackMoney = frozenTailRecord->GetFrozeAmount() - frozenTailRecord->GetBackAmount();
		}

		_SendNotifyMailWhileUnFreeze(unfreezeMoney, restBackMoney);
	}
}

void AbnormalTransactionMgr::OnIncDailyTaskScore(UInt32 totalScore)
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	if (GetTailDeadline() == 0 || totalScore == 0) return;

	AbnormalTransactionRecord* frozenTailRecord = NULL;

	VisitAbnormalRecord([&frozenTailRecord](AbnormalTransactionRecord* record)->bool{
		if (record != NULL && record->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			if (record->CanUnFreezeByDailyTaskScore())
			{
				frozenTailRecord = record;
				return false;
			}
		}
		return true;
	});

	if (frozenTailRecord == NULL)
	{
		return;
	}

	UInt32 dailyTailDays = GetOwner()->GetCount(ABNORMAL_TRANS_DAILY_TAIL_DAYS);

	UInt32 unFreezeRequireScore = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DAILY_TASK_SCORE_UN_FREEZE_REQUIREMENT);

	if (unFreezeRequireScore == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction unfreeze requrie score are zero!" << LogStream::eos;
		return;
	}

	if (dailyTailDays == 0 && totalScore >= unFreezeRequireScore)
	{
		// 日常任务活跃度达到一定值，可以解冻若干天
		UInt32 unFreezeDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DAILL_TASK_SCORE_UN_FREEZE_DAYS);

		if (unFreezeDays == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction unfreeze days are zero!" << LogStream::eos;
			return;
		}

		GetOwner()->IncCounter(ABNORMAL_TRANS_DAILY_TAIL_DAYS, unFreezeDays);
		GetOwner()->IncCounter(ABNORMAL_TRANS_TOTAL_TAIL_DAYS, unFreezeDays);

		DecTailDeadline(unFreezeDays * DAY_TO_SEC);

		InfoLogStream << PLAYERINFO(GetOwner()) << "Unfreeze tail days(" << unFreezeDays << "), tail deadline=" << GetTailDeadline() << "), the daily task score(" 
			<< totalScore << ") meet the requirement(" << unFreezeRequireScore << ")." << LogStream::eos;
	}
}

void AbnormalTransactionMgr::OnHeartbeat(const Avalon::Time& now, int tickType)
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	if (tickType == LOGICTICK_SECOND)
	{
		if (m_TailDeadline != 0 && m_TailDeadline <= (UInt32)now.Sec())
		{
			_OnTailDeadlineExpire();
		}

		for (auto elem : m_AbnormalTransactionRecords)
		{
			if (!elem) continue;

			elem->SaveToDB();
		}
	}
	else if (tickType == LOGICTICK_HALFMIN)
	{
		QueryWaitFreezeAbnormalTrans();
	}
}

void AbnormalTransactionMgr::OnSceneChanged()
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	_OnCheckNotifyAbnormalTransaction();
}

void AbnormalTransactionMgr::OnQueryFrozenAndNeverBackAbnormalTransactionRecordRet()
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	if (m_PlayerOnlineLoadDBCount != 2)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " player online load db count(" << (UInt32)m_PlayerOnlineLoadDBCount << ") is error!" << LogStream::eos;
		return;
	}

	if (m_AbnormalTransactionRecords.empty())
	{
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "Query froze and never back record return..." << LogStream::eos;

	// 配置的条件
	UInt32 freezeBaseDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_BASE_DAYS);
	UInt32 freezeMaxDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_MAX_DAYS);
	UInt32 freezeAccumulateDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_ACCUMULATE_DAYS);

	if (freezeBaseDays == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail base days are zero!" << LogStream::eos;
		return;
	}

	if (freezeBaseDays >= freezeMaxDays)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail base days is more than max days!" << LogStream::eos;
		return;
	}

	if (freezeAccumulateDays == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail accumulate days are zero!" << LogStream::eos;
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	// 累加值
	UInt32 accumulation = freezeBaseDays;
	// 上一个冻结期记录
	AbnormalTransactionRecord* preFrozenTailReocrd = NULL;

	//第一次加载的是冻结期和不返还期的记录
	for (auto abnormalRecord : m_AbnormalTransactionRecords)
	{
		if (!abnormalRecord) continue;

		// 因为时间是从小到大排列的，所以这里的时间计算为
		// 第一个处于冻结期或者不返还期的创建时间+配置的保释期
		if (GetTailDeadline() == 0)
		{
			if (abnormalRecord->IsFrozenPermanent())
			{
				SetTailDeadline(_GetMaxFrozenTime());
			}
			else
			{
				UInt32 deadline = abnormalRecord->GetCreateTime() + freezeBaseDays * DAY_TO_SEC;
				SetTailDeadline(deadline);

				if (GetTailDeadline() > 0 && GetTailDeadline() > curTime)
				{
					accumulation = (GetTailDeadline() - curTime) / DAY_TO_SEC;
				}
			}

			InfoLogStream << PLAYERINFO(GetOwner()) << "Recalculate deadline, " << abnormalRecord->LogStr() << ", deadline=" << GetTailDeadline() << LogStream::eos;
			continue;
		}

		if (abnormalRecord->IsFrozenPermanent())
		{
			SetTailDeadline(_GetMaxFrozenTime());
		}

		if (GetTailDeadline() != _GetMaxFrozenTime())
		{
			accumulation += freezeAccumulateDays;

			if (accumulation <= freezeMaxDays)
			{
				IncTailDeadline(freezeAccumulateDays * DAY_TO_SEC);

				InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << "Increase days(" << freezeAccumulateDays << "), deadline=" << GetTailDeadline() << LogStream::eos;
			}
		}

		if (abnormalRecord->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			if (preFrozenTailReocrd != NULL)
			{
				UInt32 openNevenBackHandleTime = GetOwner()->GetCount(ABNORMAL_TRANS_OPEN_NEVEN_BACK_HANDLE_TIME);
				if (openNevenBackHandleTime != 0 && openNevenBackHandleTime >= abnormalRecord->GetCreateTime())
				{
					_MergeAbnormalTransaction(preFrozenTailReocrd, abnormalRecord, freezeBaseDays);

					InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " merge frozen amount, because prefrozen record: " << preFrozenTailReocrd->LogStr() << ", and open time("
						<< openNevenBackHandleTime << "), deadline=" << GetTailDeadline() << LogStream::eos;

					continue;
				}

				preFrozenTailReocrd->SetHandleStatus(ATHS_NEVER_BACK_MONEY);

				InfoLogStream << PLAYERINFO(GetOwner()) << preFrozenTailReocrd->LogStr() << " never back money, because another frozen record: " << abnormalRecord->LogStr() << LogStream::eos;
			}

			preFrozenTailReocrd = abnormalRecord;
		}
	}

	UInt32 totalTailDays = GetOwner()->GetCount(ABNORMAL_TRANS_TOTAL_TAIL_DAYS);
	UInt32 preDeadline = GetTailDeadline();
	DecTailDeadline(totalTailDays * DAY_TO_SEC);

	InfoLogStream << PLAYERINFO(GetOwner()) << "Decrease tail deadline(" << preDeadline << "), total tail days=" << totalTailDays << ", now deadline=" << GetTailDeadline() << LogStream::eos;
}

void AbnormalTransactionMgr::OnQueryWaitFreezeAbnormalTransactionRecordRet(CLRecordCallback* callback)
{
	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	//InfoLogStream << PLAYERINFO(GetOwner()) << "Query wait freeze recor..." << LogStream::eos;

	if (!LoadDataFromDBCallback(callback, true)) return;

	// 配置的条件
	UInt32 freezeBaseDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_BASE_DAYS);
	UInt32 freezeMaxDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_MAX_DAYS);
	UInt32 freezeAccumulateDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_ACCUMULATE_DAYS);
	UInt32 checkInterval = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_VERIFY_INTERVAL_TIAME);

	if (freezeBaseDays == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail base days are zero!" << LogStream::eos;
		return;
	}

	if (freezeBaseDays >= freezeMaxDays)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail base days is more than max days!" << LogStream::eos;
		return;
	}

	if (freezeAccumulateDays == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction freeze tail accumulate days are zero!" << LogStream::eos;
		return;
	}

	if (checkInterval == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " abnormal transaction check interval is zero!" << LogStream::eos;
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	// 累加值
	UInt32 accumulation = freezeBaseDays;
	if (GetTailDeadline() > 0 && GetTailDeadline() > curTime)
	{
		accumulation = (GetTailDeadline() - curTime) / DAY_TO_SEC;
	}
	// 上一个冻结期记录
	AbnormalTransactionRecord* preFrozenTailReocrd = NULL;
	// 当前冻结期记录
	AbnormalTransactionRecord* curFrozenTailRecord = NULL;
	// 需要去邮件扣道具
	bool needRmMailItems = false;

	// 每次只从数据库拉一条待冻结记录
	for (auto abnormalRecord : m_AbnormalTransactionRecords)
	{
		if (!abnormalRecord) continue;

		// 新检测出来的记录是等待状态，需要处理
		if (abnormalRecord->GetHandleStatus() == ATHS_WAIT_FREEZE)
		{
			// 每隔一段较长时间去数据库拉一条待冻结记录，所以这里正常情况只有一条
			curFrozenTailRecord = abnormalRecord;

			_OnHandleWaiteHandleAbnormalTransaction(abnormalRecord);

			if (abnormalRecord->GetFrozeAmount() < abnormalRecord->GetTransationAmount())
			{
				needRmMailItems = true;
			}
		}

		if (abnormalRecord->GetHandleStatus() != ATHS_FROZEN_TAIL &&
			abnormalRecord->GetHandleStatus() != ATHS_NEVER_BACK_MONEY)
		{
			continue;
		}

		// 因为时间是从小到大排列的，所以这里的时间计算为
		// 第一个处于冻结期或者不返还期的创建时间+配置的保释期
		if (GetTailDeadline() == 0)
		{
			if (abnormalRecord->IsFrozenPermanent())
			{
				SetTailDeadline(_GetMaxFrozenTime());
			}
			else
			{
				UInt32 deadline = abnormalRecord->GetCreateTime() + freezeBaseDays * DAY_TO_SEC;
				SetTailDeadline(deadline);
				if (GetTailDeadline() > 0 && GetTailDeadline() > curTime)
				{
					accumulation = (GetTailDeadline() - curTime) / DAY_TO_SEC;
				}
			}

			if (abnormalRecord->IsTreasure())
			{
				GetOwner()->SetCounter(ABNORMAL_TRANS_OPEN_NEVEN_BACK_HANDLE_TIME, (abnormalRecord->GetCreateTime() + checkInterval * MIN_TO_SEC));
				InfoLogStream << PLAYERINFO(GetOwner()) << " set next o[en never back handle time(" << TimeUtil::TimestampToStandardFormat(GetOwner()->GetCount(ABNORMAL_TRANS_OPEN_NEVEN_BACK_HANDLE_TIME)) << ")." << LogStream::eos;
			}

			if (!needRmMailItems)
			{
				_SendNotifyMailWhileFrozen(abnormalRecord, NULL);
			}

			InfoLogStream << PLAYERINFO(GetOwner()) << "Recalculate deadline, " << abnormalRecord->LogStr() << ", deadline=" << GetTailDeadline() << LogStream::eos;
			continue;
		}

		if (abnormalRecord->IsFrozenPermanent())
		{
			SetTailDeadline(_GetMaxFrozenTime());
		}

		// 冻结期
		if (abnormalRecord->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			if (preFrozenTailReocrd != NULL)
			{
				// 在审核期内再犯的的交易,冻结金合并,时间重置
				UInt32 openNevenBackHandleTime = GetOwner()->GetCount(ABNORMAL_TRANS_OPEN_NEVEN_BACK_HANDLE_TIME);
				if (openNevenBackHandleTime != 0 && openNevenBackHandleTime >= abnormalRecord->GetCreateTime())
				{
					_MergeAbnormalTransaction(preFrozenTailReocrd, abnormalRecord, freezeBaseDays);

					if (!needRmMailItems)
					{
						_SendNotifyMailWhileFrozen(abnormalRecord, NULL);
					}

					InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " merge frozen amount, because prefrozen record: " << preFrozenTailReocrd->LogStr() << ", and open time("
						<< openNevenBackHandleTime << "), deadline=" << GetTailDeadline() << LogStream::eos;

					continue;
				}

				if (GetTailDeadline() != _GetMaxFrozenTime())
				{
					accumulation += freezeAccumulateDays;

					if (accumulation <= freezeMaxDays)
					{
						IncTailDeadline(freezeAccumulateDays * DAY_TO_SEC);

						InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << "Increase days(" << freezeAccumulateDays << "), deadline=" << GetTailDeadline() << LogStream::eos;
					}
				}

				preFrozenTailReocrd->SetHandleStatus(ATHS_NEVER_BACK_MONEY);

				if (!needRmMailItems)
				{
					_SendNotifyMailWhileFrozen(abnormalRecord, preFrozenTailReocrd);
				}

				InfoLogStream << PLAYERINFO(GetOwner()) << preFrozenTailReocrd->LogStr() << " never back money, because another frozen record: " << abnormalRecord->LogStr() << LogStream::eos;
			}

			preFrozenTailReocrd = abnormalRecord;
		}
	}

	if (!curFrozenTailRecord || curFrozenTailRecord->GetHandleStatus() != ATHS_FROZEN_TAIL) return;

	if (GetTailDeadline() == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << curFrozenTailRecord->LogStr() << " freeze tail deadline is zero!" << LogStream::eos;
		return;
	}

	if (!needRmMailItems)
	{
		_NotifyClientAbnormalTransaction(true);

		InfoLogStream << PLAYERINFO(GetOwner()) << "Notify abnormal tansaction: " << curFrozenTailRecord->LogStr() << LogStream::eos;
		return;
	}

	// 玩家身上的金额不够，扣除邮件中的钱
	CLProtocol::SceneAbnormalTransRemoveMailItemReq rmMailItemReq;
	rmMailItemReq.abnormalTransId = curFrozenTailRecord->GetID();
	rmMailItemReq.preAbnormalTransId = preFrozenTailReocrd ? preFrozenTailReocrd->GetID() : 0;
	rmMailItemReq.owner = curFrozenTailRecord->GetSeller();
	rmMailItemReq.itemDataId = 0;
	rmMailItemReq.needRmNum = 0;

	if (curFrozenTailRecord->GetMoneyType() == MONEY_GOLD)
	{
		rmMailItemReq.itemDataId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLD_ITEM_DATA_ID);
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << curFrozenTailRecord->LogStr() << "Unkonwn money type(" << curFrozenTailRecord->GetMoneyType() << ")!" << LogStream::eos;
	}

	Int32 needRmNum = (Int32)curFrozenTailRecord->GetTransationAmount() - (Int32)curFrozenTailRecord->GetFrozeAmount();
	if (needRmNum < 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << curFrozenTailRecord->LogStr() << "transaction amount(" << curFrozenTailRecord->GetTransationAmount() 
			<< ") is more than froze amount(" << curFrozenTailRecord->GetFrozeAmount() << ")!" << LogStream::eos;
	}
	else
	{
		rmMailItemReq.needRmNum = (UInt32)needRmNum;
	}

	Router::SendToWorld(rmMailItemReq);

	InfoLogStream << PLAYERINFO(GetOwner()) << curFrozenTailRecord->LogStr() << " need remove money(type=" << curFrozenTailRecord->GetMoneyType() << ", num=" << needRmNum << ") from mail." << LogStream::eos;
}

void AbnormalTransactionMgr::OnRemoveMailItemRes(const CLProtocol::SceneAbnormalTransRemoveMailItemRes& res)
{
	auto abnormalRecord = FindAbnormalRecord(res.abnormalTransId);
	if (!abnormalRecord)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find abnormal transaction record(" << res.abnormalTransId << ")!" << LogStream::eos;
		return;
	}

	auto preAbnormalRecord = FindAbnormalRecord(res.preAbnormalTransId);

	if (res.itemDataId == SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLD_ITEM_DATA_ID))
	{
		abnormalRecord->IncFrozeAmount(res.successRmNum);
		abnormalRecord->SaveToDB();

		// 发送通知
		_SendNotifyMailWhileFrozen(abnormalRecord, preAbnormalRecord);
		_NotifyClientAbnormalTransaction(true);

		InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " freeze gold amount(" << res.successRmNum << ")." << LogStream::eos;
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << "Unkonwn money type(" << res.itemDataId << ")!" << LogStream::eos;
	}
}

void AbnormalTransactionMgr::OnQueryAbnormalTransactionRecordReq()
{
	CLProtocol::SceneAbnormalTransactionRecordRes res;

	if (SSApplication::Instance()->IsCloseAbnormalTransHandle())
	{
		GetOwner()->SendProtocol(res);
		return;
	}

	AbnormalTransactionRecord* frozenTailRecord = NULL;

	VisitAbnormalRecord([&frozenTailRecord](AbnormalTransactionRecord* record)->bool{
		if (record != NULL && record->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			frozenTailRecord = record;
			return false;
		}
		return true;
	});

	if (frozenTailRecord != NULL)
	{
		res.frozenMoneyType = frozenTailRecord->GetMoneyType();
		res.frozenAmount = frozenTailRecord->GetFrozeAmount();
		res.abnormalTransactionTime = frozenTailRecord->GetCreateTime();
		res.backDeadline = GetTailDeadline();
		res.backAmount = frozenTailRecord->GetBackAmount();
		res.backDays = GetOwner()->GetCount(ABNORMAL_TRANS_TOTAL_TAIL_DAYS);
		DebugLogStream << PLAYERINFO(GetOwner()) << " frozen tail record(" << (UInt32)frozenTailRecord->GetHandleStatus() << "): " << frozenTailRecord->LogStr() << LogStream::eos;
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find frozen tail record!" << LogStream::eos;
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << " query record(amount:" << res.frozenAmount << ",transtime:" << TimeUtil::TimestampToStandardFormat(res.abnormalTransactionTime)
		<< ",deadline:" << TimeUtil::TimestampToStandardFormat(res.backDeadline) << ",back amount:" << res.backAmount << ",back days:" << res.backDays << ")" << LogStream::eos;
	GetOwner()->SendProtocol(res);
}

void AbnormalTransactionMgr::OnAbnormalTransUnfreezeReq()
{
	InfoLogStream << PLAYERINFO(GetOwner()) << " start to unfreeze abnormal transaction..." << LogStream::eos;

	// 实际解冻金额
	UInt32 unfreezeMoney = 0;

	VisitAbnormalRecord([&](AbnormalTransactionRecord* frozenTailRecord)->bool{
		if (frozenTailRecord != NULL && frozenTailRecord->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			UInt32 restBackAmount = _BackRestFrozenMoneyToPlayer(frozenTailRecord);
			unfreezeMoney += restBackAmount;

			frozenTailRecord->SetHandleStatus(ATHS_MANUAL_END_HANDLE);

			InfoLogStream << PLAYERINFO(GetOwner()) << " OnAbnormalTransUnfreezeReq: " << frozenTailRecord->LogStr() << ", back money(type=" << frozenTailRecord->GetMoneyType()
				<< ", num=" << restBackAmount << ")." << LogStream::eos;
		}
		return true;
	});

	_TerminateAbnormalTransData();

	// 发送邮件
	_SendNotifyMailWhileUnFreeze(unfreezeMoney);

	InfoLogStream << PLAYERINFO(GetOwner()) << " unfreeze money(" << unfreezeMoney << ") manual." << LogStream::eos;
}

void AbnormalTransactionMgr::QueryWaitFreezeAbnormalTrans()
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

	if (SSApplication::Instance()->IsCloseAbnormalTransHandle()) return;

	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_abnormal_transaction");
		selectCmd->PutCond("handle_status", (UInt8)ATHS_WAIT_FREEZE);
		selectCmd->PutCond("seller", GetOwner()->GetID());
		selectCmd->PutCond("freeze_object", (UInt8)AT_FREEZE_OBJECT_SELLER);
		std::string limitStr = "ORDER BY guid";
		selectCmd->SetLimit(limitStr);
		CLRecordClient::Instance()->SendCommand(selectCmd, new QueryWaitFreezeAbnormalTransactionCallback(GetOwner()->GetID()));
	}
	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_abnormal_transaction");
		selectCmd->PutCond("handle_status", (UInt8)ATHS_WAIT_FREEZE);
		selectCmd->PutCond("buyer", GetOwner()->GetID());
		selectCmd->PutCond("freeze_object", (UInt8)AT_FREEZE_OBJECT_BUYER);
		std::string limitStr = "ORDER BY guid";
		selectCmd->SetLimit(limitStr);
		CLRecordClient::Instance()->SendCommand(selectCmd, new QueryWaitFreezeAbnormalTransactionCallback(GetOwner()->GetID()));
	}
}

bool AbnormalTransactionMgr::LoadDataFromDBCallback(CLRecordCallback* callback, bool isLoadWaitRecord)
{
	if (!callback) return false;

	if (!isLoadWaitRecord)
	{
		m_PlayerOnlineLoadDBCount++;
	}

	if (!callback->NextRow()) return false;

	AbnormalTransactionRecordVec waitRecords;

	do{
		AbnormalTransactionRecord* record = new AbnormalTransactionRecord();
		record->SetID(callback->GetKey());
		record->SetDBData(callback);

		if (FindAbnormalRecord(record->GetID()) != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Repeat record(" << record->LogStr() << ")!" << LogStream::eos;
			delete record;
		}
		else
		{
			if (isLoadWaitRecord)
			{
				waitRecords.push_back(record);
				continue;
			}

			if (!AddAbnormalRecord(record))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Failed add record(" << record->LogStr() << ")!" << LogStream::eos;
				delete record;
			}
		}

	} while (callback->NextRow());

	// 将创建时间按规则排序
	struct SortFunc
	{
		bool operator()(const AbnormalTransactionRecord* lhs, const AbnormalTransactionRecord* rhs)
		{
			if (!lhs || !rhs) return true;

			if (lhs->GetCreateTime() != rhs->GetCreateTime())
				return lhs->GetCreateTime() < rhs->GetCreateTime();
			else
				return lhs->GetID() < rhs->GetID();
		}
	};

	if (isLoadWaitRecord)
	{
		if (waitRecords.empty())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Wait records are empty!" << LogStream::eos;
			return true;
		}

		std::sort(waitRecords.begin(), waitRecords.end(), SortFunc());

		// 每次都从等待记录里取排序中最前的一个,每次只处理一个
		AddAbnormalRecord(waitRecords.front());

		waitRecords.erase(waitRecords.begin());

		for (auto record : waitRecords)
		{
			delete record;
		}
		waitRecords.clear();
	}
	else
	{
		std::sort(m_AbnormalTransactionRecords.begin(), m_AbnormalTransactionRecords.end(), SortFunc());
	}

	return true;
}

bool AbnormalTransactionMgr::LoadBuyerDataFromDB(CLRecordCallback* callback)
{
	if (!callback) return false;

	m_PlayerOnlineLoadDBCount++;

	if (!callback->NextRow()) return false;

	do{
		AbnormalTransactionRecord* record = new AbnormalTransactionRecord();
		record->SetID(callback->GetKey());
		record->SetDBData(callback);

		if (FindAbnormalRecord(record->GetID()) != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Repeat record(" << record->LogStr() << ")!" << LogStream::eos;
			delete record;
		}
		else
		{
			if (!AddAbnormalRecord(record))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Failed add record(" << record->LogStr() << ")!" << LogStream::eos;
				delete record;
			}
		}

	} while (callback->NextRow());

	// 将创建时间按规则排序
	struct SortFunc
	{
		bool operator()(const AbnormalTransactionRecord* lhs, const AbnormalTransactionRecord* rhs)
		{
			if (!lhs || !rhs) return true;

			if (lhs->GetCreateTime() != rhs->GetCreateTime())
				return lhs->GetCreateTime() < rhs->GetCreateTime();
			else
				return lhs->GetID() < rhs->GetID();
		}
	};

	std::sort(m_AbnormalTransactionRecords.begin(), m_AbnormalTransactionRecords.end(), SortFunc());

	return true;
}

void AbnormalTransactionMgr::_OnHandleWaiteHandleAbnormalTransaction(AbnormalTransactionRecord* abnormalRecord)
{
	if (!abnormalRecord || abnormalRecord->GetHandleStatus() != ATHS_WAIT_FREEZE) return;

	InfoLogStream << PLAYERINFO(GetOwner()) << "On handle wait handle " << abnormalRecord->LogStr() << LogStream::eos;

	UInt32 timeOut = abnormalRecord->GetCreateTime() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FREEZE_TAIL_BASE_DAYS) * DAY_TO_SEC;

	// 超时了
	if ((UInt32)CURRENT_TIME.Sec() >= timeOut)
	{
		abnormalRecord->SetHandleStatus(ATHS_TIME_OUT);
		ErrorLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << ") is time out(" << timeOut << ")." << LogStream::eos;
		return;
	}

	// 交易金为金币
	if (abnormalRecord->GetMoneyType() == MONEY_GOLD)
	{
		// 未冻结金币，立即返还

		UInt32 unFrozenAmount = abnormalRecord->GetUnfrozenAmount();

		if (unFrozenAmount > 0)
		{
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MONEY, abnormalRecord->GetID());
			GetOwner()->AddGold(reason.c_str(), unFrozenAmount);
			InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " add unfrozen money(" << unFrozenAmount << ")." << LogStream::eos;
		}

		// 异常交易，冻结交易金

		UInt32 removeNum = 0;

		if (abnormalRecord->NeedRemovePlayerTransMoney())
		{
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MONEY, abnormalRecord->GetID());
			removeNum = GetOwner()->RemoveGold(reason.c_str(), abnormalRecord->GetTransationAmount());

			if (removeNum > abnormalRecord->GetTransationAmount())
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << "Remove gold abnormal!!! Remove success num=" << removeNum
					<< ", but transaction amount=" << abnormalRecord->GetTransationAmount() << LogStream::eos;
			}
		}
		else
		{
			removeNum = abnormalRecord->GetTransationAmount();
			InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " freeze money(" << removeNum << ") but not remove gold from player package." << LogStream::eos;
		}

		if (removeNum > 0)
		{
			// 设置冻结金额
			abnormalRecord->SetFrozeAmount(removeNum);

			InfoLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << " freeze gold amount(" << removeNum << ")!" << LogStream::eos;
		}

		abnormalRecord->SetHandleStatus(ATHS_FROZEN_TAIL);
		abnormalRecord->SaveToDB();
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << abnormalRecord->LogStr() << "Unkonwn money type(" << abnormalRecord->GetMoneyType() << ")!" << LogStream::eos;
	}
}

void AbnormalTransactionMgr::_NotifyClientAbnormalTransaction(bool bNotify)
{
	CLProtocol::SceneNotifyAbnormalTransaction notifyAbnormal;
	notifyAbnormal.bNotify = bNotify ? 1 : 0;
	GetOwner()->SendProtocol(notifyAbnormal);
}

void AbnormalTransactionMgr::_OnCheckNotifyAbnormalTransaction()
{
	AbnormalTransactionRecord* frozenTailRecord = NULL;

	VisitAbnormalRecord([&frozenTailRecord](AbnormalTransactionRecord* record)->bool{
		if (!record) return true;

		//DebugLogStream << "Player(" << record->GetSeller() << "'s abnormal trans(" << record->GetID() << ", " << (UInt32)record->GetHandleStatus() << ")." << LogStream::eos;

		if (record != NULL && record->GetHandleStatus() == ATHS_FROZEN_TAIL)
		{
			frozenTailRecord = record;
			return false;
		}
		return true;
	});

	if (frozenTailRecord != NULL)
	{
		_NotifyClientAbnormalTransaction(true);
	}
}

void AbnormalTransactionMgr::_SendNotifyMailWhileFrozen(const AbnormalTransactionRecord* curFrozenTailRecord, const AbnormalTransactionRecord* preFrozenTailRecord)
{
	if (!curFrozenTailRecord) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	// 发送通知邮件

	std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
	std::string title = SysNotifyMgr::Instance()->MakeString(9852);

	UInt32 restDays = 0;
	if (GetTailDeadline() >= curTime)
	{
		restDays = (GetTailDeadline() - curTime) / DAY_TO_SEC;
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Freeze tail deadline(" << GetTailDeadline() << ") is more than current time(" << curTime << ")!" << LogStream::eos;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MAIL, curFrozenTailRecord->GetID(), preFrozenTailRecord == NULL ? 0 : 1);

	if (preFrozenTailRecord == NULL)
	{
		// 初犯

		switch (curFrozenTailRecord->GetMailNotifyType())
		{
		case 0:
		{
			std::string content = SysNotifyMgr::Instance()->MakeString(9853, TimeUtil::TimestampToStandarFormatNoSpace(curFrozenTailRecord->GetTansactionTime()), curFrozenTailRecord->GetFrozeAmount(), curFrozenTailRecord->GetUnfrozenAmount(), restDays);
			Sys::SendSysMail(GetOwner()->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), 30, NULL);

			break;
		}

		case 1:
		{
			std::string content2 = SysNotifyMgr::Instance()->MakeString(9956, TimeUtil::TimestampToStandarFormatNoSpace(curFrozenTailRecord->GetTansactionTime()), curFrozenTailRecord->GetFrozeAmount(), curFrozenTailRecord->GetUnfrozenAmount(), restDays);
			Sys::SendSysMail(curFrozenTailRecord->GetBuyer(), sender.c_str(), title.c_str(), content2.c_str(), reason.c_str(), 30, NULL);

			break;
		}

		default:
			ErrorLogStream << PLAYERINFO(GetOwner()) << curFrozenTailRecord->LogStr() << " error mail notify type(" << (UInt32)curFrozenTailRecord->GetMailNotifyType() << ")!" << LogStream::eos;
			break;
		}
	}
	else
	{
		// 再犯

		UInt32 neverBackMoney = 0;

		if (preFrozenTailRecord->GetFrozeAmount() < preFrozenTailRecord->GetBackAmount())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << preFrozenTailRecord->LogStr() << "'s froze amount(" << preFrozenTailRecord->GetFrozeAmount()
				<< ") is more than back amount(" << preFrozenTailRecord->GetBackAmount() << ")!" << LogStream::eos;
		}
		else
		{
			neverBackMoney += (preFrozenTailRecord->GetFrozeAmount() - preFrozenTailRecord->GetBackAmount());
		}

		std::string content = SysNotifyMgr::Instance()->MakeString(9854, TimeUtil::TimestampToStandarFormatNoSpace(curFrozenTailRecord->GetTansactionTime()), curFrozenTailRecord->GetFrozeAmount(), curFrozenTailRecord->GetUnfrozenAmount(), restDays, neverBackMoney);
		Sys::SendSysMail(GetOwner()->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), 30, NULL);
	}
}

void AbnormalTransactionMgr::_SendNotifyMailWhileUnFreeze(UInt32 unfreezeAmount, UInt32 restBackAmount)
{
	std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
	std::string title = SysNotifyMgr::Instance()->MakeString(9852);
	std::string content = SysNotifyMgr::Instance()->MakeString(9855, unfreezeAmount, restBackAmount);
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MAIL);

	Sys::SendSysMail(GetOwner()->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), 30, NULL);
}

void AbnormalTransactionMgr::_OnTailDeadlineExpire()
{
	// 达到返还终止期，需要返还剩余全部冻结金

	InfoLogStream << PLAYERINFO(GetOwner()) << " reach tail deadline(" << TimeUtil::TimestampToStandardFormat(GetTailDeadline()) << ")." << LogStream::eos;

	UInt32 unfreezeAmount = 0;

	VisitAbnormalRecord([&](AbnormalTransactionRecord* frozenTailRecord)->bool{
		if (frozenTailRecord == NULL || frozenTailRecord->GetHandleStatus() != ATHS_FROZEN_TAIL)
		{
			return true;
		}

		frozenTailRecord->SetHandleStatus(ATHS_END_HANDLE);

		UInt32 restBackAmount = _BackRestFrozenMoneyToPlayer(frozenTailRecord);
		unfreezeAmount += restBackAmount;

		InfoLogStream << PLAYERINFO(GetOwner()) << frozenTailRecord->LogStr() << "All money back, back money(type=" << frozenTailRecord->GetMoneyType()
			<< ", num=" << restBackAmount << ")." << LogStream::eos;

		return true;
	});

	// 返还全部冻结金后需终止一些计数等数据
	_TerminateAbnormalTransData();

	// 发送邮件
	_SendNotifyMailWhileUnFreeze(unfreezeAmount);
}

void AbnormalTransactionMgr::_TerminateAbnormalTransData()
{
	_NotifyClientAbnormalTransaction(false);

	VisitAbnormalRecord([](AbnormalTransactionRecord* record)->bool{
		if (record != NULL && record->GetHandleStatus() == ATHS_NEVER_BACK_MONEY)
		{
			record->SetHandleStatus(ATHS_END_HANDLE);
		}
		return true;
	});

	SetTailDeadline(0);
	GetOwner()->SetCounter(ABNORMAL_TRANS_DAILY_TAIL_DAYS, 0);
	GetOwner()->SetCounter(ABNORMAL_TRANS_TOTAL_TAIL_DAYS, 0);

	InfoLogStream << PLAYERINFO(GetOwner()) << "Terminate abnormal transaction data!" << LogStream::eos;
}

UInt32 AbnormalTransactionMgr::_GetMaxFrozenTime()
{
	return (UInt32)INT32_MAX;
}

UInt32 AbnormalTransactionMgr::_BackRestFrozenMoneyToPlayer(AbnormalTransactionRecord* record)
{
	if (!record) return 0;

	if (record->GetFrozeAmount() < record->GetBackAmount())
	{
		record->SetHandleStatus(ATHS_INVALID);
		ErrorLogStream << PLAYERINFO(GetOwner()) << record->LogStr() << "Invalid amount: froze amount(" << record->GetFrozeAmount()
			<< ") is more than back amount(" << record->GetBackAmount() << ")!" << LogStream::eos;
		return 0;
	}

	UInt32 restBackAmount = record->GetFrozeAmount() - record->GetBackAmount();
	if (restBackAmount == 0)
	{
		return 0;
	}

	record->IncBackAmount(restBackAmount);

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_DAILY_TASK_SCORE_BACK_MONEY, record->GetID());
	if (record->GetMoneyType() == MONEY_GOLD)
	{
		GetOwner()->AddGold(reason.c_str(), restBackAmount);
	}

	return restBackAmount;
}

void AbnormalTransactionMgr::_MergeAbnormalTransaction(AbnormalTransactionRecord* destRecord, AbnormalTransactionRecord* srcRecord, UInt32 freezeBaseDays)
{
	if (!destRecord || !srcRecord) return;

	// 合并冻结金
	destRecord->SetFrozeAmount(destRecord->GetFrozeAmount() + srcRecord->GetFrozeAmount());

	// 取最大时间
	if (srcRecord->GetCreateTime() > destRecord->GetCreateTime())
	{
		destRecord->SetCreateTime(srcRecord->GetCreateTime());
	}

	// 设置为已合并
	srcRecord->SetHandleStatus(ATHS_MERGE_OVER);

	// 如果有永久冻结
	if (srcRecord->IsFrozenPermanent())
	{
		destRecord->SetFrozenPermanentFlag(true);
	}
	
	if (GetTailDeadline() != _GetMaxFrozenTime())
	{
		// 不是永久冻结,冻结时间重置
		UInt32 deadline = destRecord->GetCreateTime() + freezeBaseDays * DAY_TO_SEC;
		SetTailDeadline(deadline);
		GetOwner()->SetCounter(ABNORMAL_TRANS_DAILY_TAIL_DAYS, 0);
		GetOwner()->SetCounter(ABNORMAL_TRANS_TOTAL_TAIL_DAYS, 0);
	}
	else
	{
		// 如果有永久冻结的，只能通过充值返还
		destRecord->SetFrozenMoneyBackWay(ATFROZEN_MONEY_BACK_WAY_CHARGE);
	}
}