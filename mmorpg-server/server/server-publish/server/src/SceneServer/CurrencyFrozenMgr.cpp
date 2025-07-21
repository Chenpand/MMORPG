#include "CurrencyFrozenMgr.h"

#include <CLRecordClient.h>
#include "PlayerMgr.h"
#include "SceneSysNotify.h"

/**
*@brief 加载资金冻结
*/
class SelectCurrencyFrozenCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		CurrencyFrozenMgr::Instance()->LoadCurrencyFrozen(this);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "load t_currency_frozen error! code:" << errorcode << LogStream::eos;
	}
};

CurrencyFrozenMgr::CurrencyFrozenMgr()
{
}

CurrencyFrozenMgr::~CurrencyFrozenMgr()
{
}

void CurrencyFrozenMgr::Init()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_currency_frozen");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectCurrencyFrozenCallback());
}

UInt64 CurrencyFrozenMgr::ApplyFrozen(UInt64 roleId, UInt32 itemId, UInt32 num, const std::string& reason)
{
	CurrencyFrozen frozen;
	frozen.guid = CLRecordClient::Instance()->GenGuid();
	frozen.roleId = roleId;
	frozen.itemId = itemId;
	frozen.num = num;
	frozen.frozenTime = CURRENT_TIME.Sec();
	frozen.reason = reason;

	m_FrozenMap[frozen.guid] = frozen;

	_InsertFrozen(frozen);
	_AddRoleOrder(roleId, frozen.guid);

	InfoLogStream << frozen.SerializeToString() << LogStream::eos;

	return frozen.guid;
}

UInt32 CurrencyFrozenMgr::RemoveFrozen(UInt64 guid)
{
	auto iter = m_FrozenMap.find(guid);
	if (iter == m_FrozenMap.end())
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	CurrencyFrozen& frozen = iter->second;

	InfoLogStream << frozen.SerializeToString() << LogStream::eos;

	// 解除冻结,需要返还玩家
	std::string sender = SysNotifyMgr::Instance()->MakeString(10045);
	std::string title = SysNotifyMgr::Instance()->MakeString(10046);
	std::string content = SysNotifyMgr::Instance()->MakeString(10047);

	std::vector<ItemReward> rewardVec;
	ItemReward reward(frozen.itemId, frozen.num, 0, 0);
	rewardVec.push_back(reward);
	RewardGroup rewardGroup(rewardVec);

	if (Sys::SendSysMail(iter->second.roleId, sender.c_str(), title.c_str(), content.c_str(), iter->second.reason.c_str(), &rewardGroup) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "CurrencyFrozenMgr Mail Error! " << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	_DelRoleOrder(iter->second.roleId, guid);

	m_FrozenMap.erase(iter);
	_DelFrozen(guid);

	return ErrorCode::SUCCESS;
}

UInt32 CurrencyFrozenMgr::DeductFrozen(UInt64 guid)
{
	auto iter = m_FrozenMap.find(guid);
	if (iter == m_FrozenMap.end())
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	InfoLogStream << iter->second.SerializeToString() << LogStream::eos;

	_DelRoleOrder(iter->second.roleId, guid);

	m_FrozenMap.erase(iter);
	_DelFrozen(guid);

	return ErrorCode::SUCCESS;
}

void CurrencyFrozenMgr::GetRoleFrozenOrder(UInt64 roleId, std::vector<CurrencyFrozen>& orderVec)
{
	auto iter = m_RoleOrderMap.find(roleId);
	if (iter == m_RoleOrderMap.end())
	{
		return;
	}

	for (auto& it : iter->second)
	{
		auto tt = m_FrozenMap.find(it);
		if (tt == m_FrozenMap.end())
		{
			continue;
		}

		orderVec.push_back(tt->second);
	}
}

void CurrencyFrozenMgr::LoadCurrencyFrozen(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		CurrencyFrozen frozen;
		frozen.guid = callback->GetKey();
		frozen.roleId = callback->GetData("owner_id");
		frozen.itemId = callback->GetData("item_id");
		frozen.num = callback->GetData("num");
		frozen.frozenTime = callback->GetData("frozen_time");
		frozen.reason = callback->GetData("reason");

		m_FrozenMap[frozen.guid] = frozen;

		_AddRoleOrder(frozen.roleId, frozen.guid);
	}
}

void CurrencyFrozenMgr::_AddRoleOrder(UInt64 roleId, UInt64 orderId)
{
	auto iter = m_RoleOrderMap.find(roleId);
	if (iter == m_RoleOrderMap.end())
	{
		std::set<UInt64> tempSet;
		tempSet.insert(orderId);
		m_RoleOrderMap[roleId] = tempSet;
	}
	else
	{
		iter->second.insert(orderId);
	}
}

void CurrencyFrozenMgr::_DelRoleOrder(UInt64 roleId, UInt64 orderId)
{
	auto iter = m_RoleOrderMap.find(roleId);
	if (iter == m_RoleOrderMap.end())
	{
		return;
	}

	iter->second.erase(orderId);
}

void CurrencyFrozenMgr::_InsertFrozen(const CurrencyFrozen& frozen)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_currency_frozen", frozen.guid);
	cmd->PutData("owner_id", frozen.roleId);
	cmd->PutData("item_id", frozen.itemId);
	cmd->PutData("num", frozen.num);
	cmd->PutData("frozen_time", frozen.frozenTime);
	cmd->PutData("reason", frozen.reason);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void CurrencyFrozenMgr::_DelFrozen(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_currency_frozen", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}