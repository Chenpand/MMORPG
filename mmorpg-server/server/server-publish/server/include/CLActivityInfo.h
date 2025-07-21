#ifndef _CL_ACTIVITY_INFO_H_
#define _CL_ACTIVITY_INFO_H_

#include "CLActivityDefine.h"
#include "CLOpActiveProtocol.h"
#include "CLRoutineProtocol.h"
#include "ActivityUtil.h"
#include "TimeUtil.h"


#define TPLAYERINFO(T) "playerid(" << T->GetID() << ")"

template<typename PlayerT>
class PlayerActivityInfo
{
public:
	PlayerActivityInfo();
	~PlayerActivityInfo();

public:
	void SetOwner(PlayerT* player) { m_Player = player; }
	PlayerT* GetOwner() { return m_Player; }

public:// 活动

	/**
	*@brief 添加
	*/
	void AddActivity(const ActivityInfo& info);

	/**
	*@brief 查找
	*/
	ActivityInfo* FindActivity(UInt32 id);

	/**
	*@brief 获取
	*/
	std::vector<ActivityInfo*>& GetActivities() { return m_Activities; }
	void GetOpenActivities(std::vector<ActivityInfo>& infos);

	/**
	*@brief 同步
	*/
	void SyncActivity(const ActivityInfo* act);

public:// 运营活动

	/**
	*@brief 添加
	*/
	void AddOpActivity(const OpActivityRegInfo& info);

	/**
	*@brief 查找
	*/
	OpActivityRegInfo* FindOpActivity(UInt32 id);
	void FindOpActivity(OpActivityType opActType, std::vector<OpActivityRegInfo*>& opActivities);

	/**
	*@brief 获取
	*/
	std::vector<OpActivityRegInfo*>& GetOpActivities() { return m_OpActivities; }

	/**
	*@brief 是否开启
	*/
	bool OpActivitiesIsOpen(OpActivityType opActType);

	/**
	*@brief 同步
	*/
	void SyncOpActivity(const OpActivityRegInfo* opAct);

public:// 事件
	void OnTick(const Avalon::Time& now, int tickType);

	void OnLevelUp();

	void OnSystemDataReload(const ActivitySystemConfigDataEntry* data);

	void SyncDatas();

public:

	/**
	*@brief 注册新人周签到运营活动
	*/
	void PlayerOpActivityTimeHandle(OpActivityRegInfo& opAct, UInt32 opActId, UInt32 timePoint);

	/**
	*@brief 注册老兵回归运营活动
	*/
	void RegisterVeteransReturnOpActivity();

	/**
	*@brief 注册新人周签到运营活动
	*/
	void RegisterNewPlayerWeekSignOpActivity();

private:
	/**
	*@brief 活动状态改变
	*/
	void _OnActivityStateChange(ActivityInfo* act, ActivityState state);

private:
	/**
	*@brief 运营活动状态改变
	*/
	void _OnOpActivityStateChange(OpActivityRegInfo* opAct, ActivityState state);

private:
	//活动
	std::vector<ActivityInfo*> m_Activities;
	//运营活动
	std::vector<OpActivityRegInfo*>	m_OpActivities;

	//关闭的活动
	std::vector<UInt32> m_CloseActivities;
	//关闭的运营活动
	std::vector<UInt32> m_CloseOpActivities;

	PlayerT* m_Player;
};

template<typename PlayerT>
PlayerActivityInfo<PlayerT>::PlayerActivityInfo()
{
	m_Player = NULL;
}

template<typename PlayerT>
PlayerActivityInfo<PlayerT>::~PlayerActivityInfo()
{
	for (auto elem : m_Activities)
	{
		delete elem;
	}
	m_Activities.clear();

	for (auto elem : m_OpActivities)
	{
		delete elem;
	}
	m_OpActivities.clear();
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::AddActivity(const ActivityInfo& info)
{
	ActivityInfo* pActInfo = FindActivity(info.id);
	if (!pActInfo)
	{
		pActInfo = new ActivityInfo(info);
		m_Activities.push_back(pActInfo);
	}
	else
	{
		*pActInfo = info;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	if (curTime >= pActInfo->preTime && curTime < pActInfo->startTime)
	{
		pActInfo->state = AS_PREPARE;
	}
	if (curTime >= pActInfo->startTime && curTime < pActInfo->dueTime)
	{
		pActInfo->state = AS_IN;
	}

	InfoLogStream << TPLAYERINFO(GetOwner()) << " add activity(" << pActInfo->id << "), state=" << pActInfo->state << ", st=" << TimeUtil::TimestampToStandardFormat(pActInfo->startTime)
		<< ", et=" << TimeUtil::TimestampToStandardFormat(pActInfo->dueTime) << LogStream::eos;
}

template<typename PlayerT>
ActivityInfo* PlayerActivityInfo<PlayerT>::FindActivity(UInt32 id)
{
	for (auto elem : m_Activities)
	{
		if (elem && elem->id == id)
		{
			return elem;
		}
	}
	return NULL;
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::GetOpenActivities(std::vector<ActivityInfo>& infos)
{
	for (auto elem : m_Activities)
	{
		if (!elem) continue;

		if (elem->state == AS_IN && elem->level <= GetOwner()->GetLevel())
		{
			infos.push_back(*elem);
		}
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::SyncActivity(const ActivityInfo* act)
{
	if (!act) return;

	CLProtocol::WorldNotifySceneActivity notify2;
	notify2.id = act->id;
	notify2.name = act->name;
	notify2.type = act->state;
	notify2.level = act->level;
	notify2.preTime = act->preTime;
	notify2.startTime = act->startTime;
	notify2.dueTime = act->dueTime;
	notify2.tribeId = 0;
	notify2.belongRole = GetOwner()->GetID();
	GetOwner()->SendToScene(notify2);
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::AddOpActivity(const OpActivityRegInfo& info)
{
	OpActivityRegInfo* pOpActInfo = FindOpActivity(info.dataId);
	if (!pOpActInfo)
	{
		pOpActInfo = new OpActivityRegInfo(info);
		m_OpActivities.push_back(pOpActInfo);
	}
	else
	{
		*pOpActInfo = info;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	if (curTime >= pOpActInfo->prepareTime && curTime < pOpActInfo->startTime)
	{
		pOpActInfo->state = AS_PREPARE;
	}
	if (curTime >= pOpActInfo->startTime && curTime < pOpActInfo->endTime)
	{
		pOpActInfo->state = AS_IN;
	}

	InfoLogStream << TPLAYERINFO(GetOwner()) << " add opActivity(" << pOpActInfo->dataId << "), state=" << pOpActInfo->state << ", st=" << TimeUtil::TimestampToStandardFormat(pOpActInfo->startTime)
		<< ", et=" << TimeUtil::TimestampToStandardFormat(pOpActInfo->endTime) << LogStream::eos;
}

template<typename PlayerT>
OpActivityRegInfo* PlayerActivityInfo<PlayerT>::FindOpActivity(UInt32 id)
{
	for (auto elem : m_OpActivities)
	{
		if (elem && elem->dataId == id)
		{
			return elem;
		}
	}
	return NULL;
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::FindOpActivity(OpActivityType opActType, std::vector<OpActivityRegInfo*>& opActivities)
{
	for (auto elem : m_OpActivities)
	{
		if (elem && elem->tmpType == (UInt32)opActType)
		{
			opActivities.push_back(elem);
		}
	}
}

template<typename PlayerT>
bool PlayerActivityInfo<PlayerT>::OpActivitiesIsOpen(OpActivityType opActType)
{
	for (auto elem : m_OpActivities)
	{
		if (elem && elem->tmpType == (UInt32)opActType)
		{
			return true;
		}
	}

	return false;
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::SyncOpActivity(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	CLProtocol::WSOpActivityChange protocol;
	protocol.opActRegInfo = *opAct;
	protocol.belongRole = GetOwner()->GetID();
	GetOwner()->SendToScene(protocol);
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::OnTick(const Avalon::Time& now, int tickType)
{
	if (tickType != LOGICTICK_SECOND) return;

	for (auto act : m_Activities)
	{
		if (!act) continue;

		if (act->state == AS_INVALID)
		{
			continue;
		}

		if (!m_CloseActivities.empty())
		{
			if (std::find(m_CloseActivities.begin(), m_CloseActivities.end(), act->id) != m_CloseActivities.end())
			{
				continue;
			}
		}

		if ((act->state == AS_IN || act->state == AS_PREPARE) && (UInt32)now.Sec() >= act->dueTime)
		{
			_OnActivityStateChange(act, AS_END);
		}
		else if ((act->state == AS_END || act->state == AS_PREPARE) && ((UInt32)now.Sec() >= act->startTime) && (now.Sec() < act->dueTime))
		{
			_OnActivityStateChange(act, AS_IN);
		}
		else if (act->state == AS_END && ((UInt32)now.Sec() >= act->preTime) && ((UInt32)now.Sec() < act->startTime))
		{
			_OnActivityStateChange(act, AS_PREPARE);
		}
	}

	for (auto opAct : m_OpActivities)
	{
		if (!opAct) continue;;

		if (opAct->state == AS_INVALID)
		{
			continue;
		}

		if (!m_CloseOpActivities.empty())
		{
			if (std::find(m_CloseOpActivities.begin(), m_CloseOpActivities.end(), opAct->dataId) != m_CloseOpActivities.end())
			{
				continue;
			}
		}

		if ((opAct->state == AS_IN || opAct->state == AS_PREPARE) && (UInt32)now.Sec() >= opAct->endTime)
		{
			_OnOpActivityStateChange(opAct, AS_END);
		}
		else if ((opAct->state == AS_END || opAct->state == AS_PREPARE) && ((UInt32)now.Sec() >= opAct->startTime) && (now.Sec() < opAct->endTime))
		{
			_OnOpActivityStateChange(opAct, AS_IN);
		}
		else if (opAct->state == AS_END && ((UInt32)now.Sec() >= opAct->prepareTime) && ((UInt32)now.Sec() < opAct->startTime))
		{
			_OnOpActivityStateChange(opAct, AS_PREPARE);
		}
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::OnLevelUp()
{
	for (auto act : m_Activities)
	{
		if (!act) continue;

		if (act->level <= GetOwner()->GetLevel() && act->state == AS_IN)
		{
			CLProtocol::WorldNotifyClientActivity notify;
			notify.id = act->id;
			notify.name = act->name;
			notify.type = act->state;
			notify.level = act->level;
			notify.startTime = act->startTime;
			notify.dueTime = act->dueTime;
			GetOwner()->SendProtocol(notify);
		}
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::OnSystemDataReload(const ActivitySystemConfigDataEntry* data)
{
	if (!data) return;

	if (!data->isClose)
	{
		for (auto itr = m_CloseActivities.begin(); itr != m_CloseActivities.end();)
		{
			UInt32 id = *itr;
			
			if (std::find(data->actList.begin(), data->actList.end(), id) != data->actList.end())
			{
				itr = m_CloseActivities.erase(itr);
				continue;
			}

			++itr;
		}
		for (auto itr = m_CloseOpActivities.begin(); itr != m_CloseOpActivities.end();)
		{
			UInt32 id = *itr;

			if (std::find(data->opActList.begin(), data->opActList.end(), id) != data->opActList.end())
			{
				itr = m_CloseOpActivities.erase(itr);
				continue;
			}

			++itr;
		}

		return;
	}

	m_CloseActivities.insert(m_CloseActivities.end(), data->actList.begin(), data->actList.end());
	m_CloseOpActivities.insert(m_CloseOpActivities.end(), data->opActList.begin(), data->opActList.end());

	for (auto elem : m_Activities)
	{
		if (!elem) continue;

		if (elem->state != AS_END)
		{
			elem->state = AS_END;
		}
	}

	for (auto elem : m_OpActivities)
	{
		if (!elem) continue;

		if (elem->state != AS_END)
		{
			elem->state = AS_END;
		}
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::SyncDatas()
{
	for (auto elem : m_Activities)
	{
		SyncActivity(elem);
	}

	for (auto elem : m_OpActivities)
	{
		SyncOpActivity(elem);
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::PlayerOpActivityTimeHandle(OpActivityRegInfo& opAct, UInt32 opActId, UInt32 timePoint)
{
	auto opActData = OpActivityDataEntryMgr::Instance()->FindEntry(opActId);
	if (!opActData)
	{
		ErrorLogStream << TPLAYERINFO(GetOwner()) << " can't find opActivity data(" << opActId << ")!" << LogStream::eos;
		return;
	}

	if (!ActivityUtil::InitOpActivityInfo(opAct, opActData))
	{
		return;
	}

	Int32 ret = TimeUtil::CalculateEndPointByStartPointAndInterval(timePoint, opActData->openInterval, opAct.startTime);
	if (ret != 0)
	{
		ErrorLogStream << TPLAYERINFO(GetOwner()) << " calculte time failed, open interval=" << opActData->openInterval << LogStream::eos;
		return;
	}

	ret = TimeUtil::CalculateEndPointByStartPointAndInterval(timePoint, opActData->closeInterval, opAct.endTime);
	if (ret != 0)
	{
		ErrorLogStream << TPLAYERINFO(GetOwner()) << " calculte time failed, close interval=" << opActData->closeInterval << LogStream::eos;
		return;
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::RegisterVeteransReturnOpActivity()
{
	auto actSystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_NEW_RETURN);
	if (!actSystemData)
	{
		ErrorLogStream << TPLAYERINFO(GetOwner()) << " can't find activity ststem config data(" << (UInt32)ACTEM_NEW_RETURN << ")!" << LogStream::eos;
		return;
	}

	for (auto opActId : actSystemData->opActList)
	{
		OpActivityRegInfo opAct;
		PlayerOpActivityTimeHandle(opAct, opActId, GetOwner()->GetRoleReturnTime());
		AddOpActivity(opAct);
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::RegisterNewPlayerWeekSignOpActivity()
{
	auto actSystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_WEEK_SIGN_NEW_PLAYER);
	if (!actSystemData)
	{
		ErrorLogStream << TPLAYERINFO(GetOwner()) << " can't find activity ststem config data(" << (UInt32)ACTEM_WEEK_SIGN_NEW_PLAYER << ")!" << LogStream::eos;
		return;
	}

	for (auto opActId : actSystemData->opActList)
	{
		OpActivityRegInfo opAct;
		PlayerOpActivityTimeHandle(opAct, opActId, GetOwner()->GetCreateTime());
		if (opAct.endTime > CURRENT_TIME.Sec())
		{
			AddOpActivity(opAct);
		}
	}
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::_OnActivityStateChange(ActivityInfo* act, ActivityState state)
{
	if (!act) return;

	if (state == act->state)
	{
		return;
	}

	act->state = state;

	SyncActivity(act);

	InfoLogStream << TPLAYERINFO(GetOwner()) << " activity(" << act->id << ") change state to " << act->state << ", st=" << TimeUtil::TimestampToStandardFormat(act->startTime)
		<< ", et=" << TimeUtil::TimestampToStandardFormat(act->dueTime) << LogStream::eos;
}

template<typename PlayerT>
void PlayerActivityInfo<PlayerT>::_OnOpActivityStateChange(OpActivityRegInfo* opAct, ActivityState state)
{
	if (!opAct) return;

	if (state == opAct->state)
	{
		return;
	}

	opAct->state = state;

	SyncOpActivity(opAct);

	InfoLogStream << TPLAYERINFO(GetOwner()) << " opActivity(" << opAct->dataId << ") change state to " << opAct->state << ", st=" << TimeUtil::TimestampToStandardFormat(opAct->startTime)
		<< ", et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
}

#endif