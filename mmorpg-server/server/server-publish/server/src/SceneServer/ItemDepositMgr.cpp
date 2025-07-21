#include "ItemDepositMgr.h"
#include "Player.h"
#include "SceneSysNotify.h"

#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLItemProtocol.h>
#include <udplog_typedef.h>

ItemDepositMgr::ItemDepositMgr()
{
	m_IsTipsExpire = false;
}

ItemDepositMgr::~ItemDepositMgr()
{

}

void ItemDepositMgr::OnTick(const Avalon::Time& now)
{
	UInt32 expireTime = GetOwner()->GetCounter(MONTH_CARD_DEPOSIT);
	if (expireTime == 0)
		return;

	UInt32 tipsTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_DEPOSIT_TIPS_TIME);
	if ( !m_IsTipsExpire && expireTime - now.Sec() <= tipsTime * 60)
	{
		m_IsTipsExpire = true;
		_ExpireTimeNotify(tipsTime * 60);
	}

	if (now.Sec() < expireTime)
		return;

	// 玩家是否有月卡
	if (GetOwner()->HasMonthCard())
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "item deposit expire has month card!" << LogStream::eos;

		// 所有物品发邮件
		std::string reason = GetReason(SOURCE_TYPE_MONTH_CARD_DEPOSIT_EXPIRE);
		std::string sender = SysNotifyMgr::Instance()->MakeString(10033);
		std::string title = SysNotifyMgr::Instance()->MakeString(10034);
		std::string content = SysNotifyMgr::Instance()->MakeString(10035);

		UInt32 k = 0;
		UInt32 num = 0;
		std::vector<ItemReward> rewardVec;
		for (auto& iter : m_DepositItemMap)
		{
			++k;
			++num;

			const CLProtocol::depositItem& itemRf = iter.second;
			rewardVec.push_back(itemRf.itemReward);

			_UdpItemDeposit(UDP_ITEM_DEPOSIT_EXPIRE, itemRf.itemReward, itemRf.dungeonId);

			InfoLogStream << PLAYERINFO(GetOwner()) << "item deposit delete guid:"<< itemRf.guid << " itemId:" << itemRf.itemReward.id << " itemNum:" << itemRf.itemReward.num << LogStream::eos;

			if (k == 4 || num == m_DepositItemMap.size()) // 每封邮件最多四个道具
			{
				RewardGroup rewardGroup(rewardVec);
				if (Sys::SendSysMail(GetOwner()->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), &rewardGroup) != ErrorCode::SUCCESS)
				{
					ErrorLogStream << "Expire Mail Error! " << LogStream::eos;
				}

				rewardVec.clear();
				k = 0;
			}
		}
	}
	else
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "item deposit expire not has month card!" << LogStream::eos;

		for (auto& iter : m_DepositItemMap)
		{
			const CLProtocol::depositItem& itemRf = iter.second;
			_UdpItemDeposit(UDP_ITEM_DEPOSIT_EXPIRE, itemRf.itemReward, itemRf.dungeonId);
		}
	}

	m_IsTipsExpire = false;
	GetOwner()->SetCounter(MONTH_CARD_DEPOSIT, 0);

	// 删除所有物品
	_DelExpireItem();
	m_DepositItemMap.clear();

	// 到期通知
	_ExpireTimeNotify(0);

	_DepositItemRes();
}

void ItemDepositMgr::HandleDepositItem()
{
	_DepositItemRes();
}

void ItemDepositMgr::HandleGetDepositItem(UInt64 guid)
{
	CLProtocol::SceneItemDepositGetRes res;

	do 
	{
		if (!GetOwner()->HasMonthCard())
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			ErrorLogStream << PLAYERINFO(GetOwner()) << " Not Has MonthCard! guid:"<< guid << LogStream::eos;
			break;
		}

		auto iter = m_DepositItemMap.find(guid);
		if (iter == m_DepositItemMap.end())
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			ErrorLogStream << PLAYERINFO(GetOwner()) << " Not Find Item! guid:"<< guid << LogStream::eos;
			break;
		}

		// 放入背包
		const CLProtocol::depositItem& itemRf = iter->second;

		ItemRewardVec rewardVec = { itemRf.itemReward };
		RewardGroup rewardGroup(rewardVec);

		std::string reason = GetReason(SOURCE_TYPE_MONTH_CARD_DEPOSIT);
		GetOwner()->AddRewards(reason.c_str(), &rewardGroup, true);

		_UdpItemDeposit(UDP_ITEM_DEPOSIT_GET, itemRf.itemReward, itemRf.dungeonId);

		// 删除之
		_DelDB(guid);
		m_DepositItemMap.erase(iter);

		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	GetOwner()->SendProtocol(res);
}

void ItemDepositMgr::LoadItemDeposit(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		CLProtocol::depositItem deitem;
		deitem.guid = callback->GetKey();
		deitem.createTime = callback->GetData("create_time");
		deitem.dungeonId = callback->GetData("dungeon_id");

		deitem.itemReward.id = callback->GetData("data_id");
		deitem.itemReward.num = callback->GetData("num");
		deitem.itemReward.strenth = callback->GetData("strengthen");
		deitem.itemReward.qualityLv = callback->GetData("quality_lv");
		deitem.itemReward.equipType = callback->GetData("equip_type");

		m_DepositItemMap[deitem.guid] = deitem;
	}
}

void ItemDepositMgr::AddItemDeposit(const ItemReward& itemReward, UInt32 dungeonId)
{
	if (m_DepositItemMap.empty())
	{
		UInt32 cfgTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_DEPOSIT_TIME);
		UInt32 expireTime = UInt32(CURRENT_TIME.Sec()) + cfgTime * 60;
		GetOwner()->SetCounter(MONTH_CARD_DEPOSIT, expireTime);
	}

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemReward.id);
	if (!itemData)
	{
		ErrorLogStream << "can't find item " << itemReward.id << LogStream::eos;
		return;
	}

	CLProtocol::depositItem ditem;
	ditem.guid = CLRecordClient::Instance()->GenGuid();
	ditem.createTime = UInt32(CURRENT_TIME.Sec());
	ditem.dungeonId = dungeonId;
	ditem.itemReward = itemReward;

	if (itemData->type == ITEM_EQUIP) 	//随机品级
	{
		Int32 ql = Avalon::Random::RandBetween(1, 99);
		ditem.itemReward.qualityLv = UInt8(ql);
	}

	bool isAdd = true;
	UInt32 girdNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_DEPOSIT_GIRD);
	if (m_DepositItemMap.size() >= girdNum)
	{
		// 找到品质最低,放入时间最早的道具
		UInt64 guid = 0;
		UInt32 color = ITEM_QUALITY_MAX;
		UInt32 createTime = 0xffffffff;

		for (auto& iter : m_DepositItemMap)
		{
			const CLProtocol::depositItem& itemRf = iter.second;
			ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemRf.itemReward.id);
			if (NULL == itemEntry)
			{
				ErrorLogStream << "not find item entry id: " << itemRf.itemReward.id << LogStream::eos;
				continue;
			}

			if (itemEntry->color < color)
			{
				guid = iter.first;
				color = itemEntry->color;
				createTime = itemRf.createTime;
			}
			else if (itemEntry->color == color)
			{
				if (itemRf.createTime < createTime)
				{
					guid = iter.first;
					color = itemEntry->color;
					createTime = itemRf.createTime;
				}
			}
		}

		// 新道具品质大于 品质最低的道具
		if (itemData->color > color)
		{
			auto tt = m_DepositItemMap.find(guid);
			if (tt != m_DepositItemMap.end())
			{
				_UdpItemDeposit(UDP_ITEM_DEPOSIT_REPLACE, tt->second.itemReward, tt->second.dungeonId);

				//InfoLogStream << PLAYERINFO(GetOwner()) << " Replace ItemDeposit " << tt->second.SerializeToString() << LogStream::eos;
				m_DepositItemMap.erase(guid);
				_DelDB(guid);
			}
		}
		else
		{
			isAdd = false;
		}
	}

	if (isAdd)
	{
		m_DepositItemMap[ditem.guid] = ditem;
		_InsertDB(ditem);
		
		_UdpItemDeposit(UDP_ITEM_DEPOSIT_OBTAIN, itemReward, dungeonId);

		//InfoLogStream << PLAYERINFO(GetOwner()) << " Add ItemDeposit " << ditem.SerializeToString() << LogStream::eos;
	}
}

void ItemDepositMgr::_ExpireTimeNotify(UInt32 oddTime)
{
	CLProtocol::SceneItemDepositExpire notify;
	notify.oddExpireTime = oddTime;
	GetOwner()->SendProtocol(notify);
}

void ItemDepositMgr::_UdpItemDeposit(UInt32 opType, const ItemReward& itemReward, UInt32 dungeonId)
{
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemReward.id);
	if (NULL == itemEntry)
	{
		ErrorLogStream << "not find item entry id:" << itemReward.id << LogStream::eos;
		return;
	}

	GetOwner()->SendUdpLog("item_deposit", LOG_ITEM_DEPOSIT, opType, itemReward.id, itemReward.num, itemEntry->color, dungeonId);
}

void ItemDepositMgr::_DepositItemRes()
{
	CLProtocol::SceneItemDepositRes res;
	res.expireTime = GetOwner()->GetCounter(MONTH_CARD_DEPOSIT);

	for (auto& iter : m_DepositItemMap)
	{
		res.depositItemVec.push_back(iter.second);
	}

	GetOwner()->SendProtocol(res);
}

void ItemDepositMgr::_InsertDB(const CLProtocol::depositItem& dpitem)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_item_deposit", dpitem.guid);
	cmd->PutData("owner_id", GetOwner()->GetID());
	cmd->PutData("data_id", dpitem.itemReward.id);
	cmd->PutData("num", dpitem.itemReward.num);
	cmd->PutData("strengthen", dpitem.itemReward.strenth);
	cmd->PutData("quality_lv", dpitem.itemReward.qualityLv);
	cmd->PutData("equip_type", dpitem.itemReward.equipType);
	cmd->PutData("create_time", dpitem.createTime);
	cmd->PutData("dungeon_id", dpitem.dungeonId);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ItemDepositMgr::_DelDB(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_item_deposit", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ItemDepositMgr::_DelExpireItem()
{
	std::ostringstream cond;
	cond << "owner_id=" << GetOwner()->GetID();
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_item_deposit", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}