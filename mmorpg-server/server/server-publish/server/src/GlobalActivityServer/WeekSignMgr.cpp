#include "WeekSignMgr.h"
#include "GASPlayer.h"
#include "GASSelectInitDataCallBack.h"
#include "GASAnnouncementMgr.h"

#include <CLRecordClient.h>
#include <CLOpActiveProtocol.h>
#include <CLWeekSignDataEntry.h>
#include <CLMsgParser.h>


struct SelectActCallBack : public SelectInitDataCallback
{
	SelectActCallBack() {}

	bool OnLoadFinished()
	{
		return WeekSignMgr::Instance()->LoadActData(this);
	}
};

struct SelectServerActItemCallBack : public SelectInitDataCallback
{
	SelectServerActItemCallBack() {}

	bool OnLoadFinished()
	{
		return WeekSignMgr::Instance()->LoadServerActItemData(this);
	}
};

struct SelectPlayerActItemCallBack : public SelectInitDataCallback
{
	SelectPlayerActItemCallBack() {}

	bool OnLoadFinished()
	{
		return WeekSignMgr::Instance()->LoadPlayerActItemData(this);
	}
};

struct SelectWeekSignRecordCallBack : public SelectInitDataCallback
{
	SelectWeekSignRecordCallBack() {}

	bool OnLoadFinished()
	{
		return WeekSignMgr::Instance()->LoadWeekSignRecord(this);
	}
};


WeekSignMgr::WeekSignMgr()
{

}

WeekSignMgr::~WeekSignMgr()
{

}

void WeekSignMgr::OnInit()
{
	CLSelectCommand* act = CLRecordClient::Instance()->PrepareSelectCommand("t_week_sign_act");
	CLRecordClient::Instance()->SendCommand(act, new SelectActCallBack());

	CLSelectCommand* serverItem = CLRecordClient::Instance()->PrepareSelectCommand("t_week_sign_item_server");
	CLRecordClient::Instance()->SendCommand(serverItem, new SelectServerActItemCallBack());

	CLSelectCommand* playerItem = CLRecordClient::Instance()->PrepareSelectCommand("t_week_sign_item_player");
	CLRecordClient::Instance()->SendCommand(playerItem, new SelectPlayerActItemCallBack());

	CLSelectCommand* record = CLRecordClient::Instance()->PrepareSelectCommand("t_week_sign_record");
	CLRecordClient::Instance()->SendCommand(record, new SelectWeekSignRecordCallBack());
}

void WeekSignMgr::OnTick(const Avalon::Time& now)
{
	for (auto& it : m_ActMap)
	{
		bool isRefresh = false;
		// 跨月
		if (!isSameMonth(CURRENT_TIME.MSec(), it.second.refreshTime))
		{
			isRefresh = true;
			_RefreshItemCnt(it.first, WEEK_MONTH);
		}

		// 跨周
		if (!isSameWeek(CURRENT_TIME.MSec(), it.second.refreshTime))
		{
			isRefresh = true;
			_RefreshItemCnt(it.first, WEEK_REFRESH);
		}

		if (isRefresh)
		{
			it.second.refreshTime = CURRENT_TIME.MSec();
			_SaveActId(it.first, it.second);
			WeekSignDataEntryMgr::Instance()->ResetSortWeekSignCfg(it.first);
		}
	}
}

void WeekSignMgr::SetActivityStatus(UInt32 actType, UInt32 actId, UInt32 actStatus)
{
	if (actStatus != AS_IN)
		return;
	
	UInt32 curActId = m_ActMap[actType].actId;
	if (actId != curActId)
	{
		// 更新活动id
		m_ActMap[actType] = ActRefresh(actId, 0);
		_SaveActId(actType, m_ActMap[actType], curActId == 0);

		// 清除上次活动的数据
		_DelItemData(actType, "t_week_sign_item_player");
		_DelItemData(actType, "t_week_sign_item_server");
		_DelDBData(actType, "t_week_sign_record");
	}
}

bool WeekSignMgr::LoadActData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt32 actType = callback->GetData("act_type");
		UInt32 actId = callback->GetData("act_id");
		UInt64 refreshTime = callback->GetData("refresh_time");

		ActRefresh actFr(actId, refreshTime);
		m_ActMap[actType] = actFr;
	}

	return true;
}

bool WeekSignMgr::LoadServerActItemData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		ActItem actItem;
		actItem.actType = callback->GetData("act_type");
		actItem.itemUid = callback->GetData("item_id");

		UInt32 actNum = callback->GetData("act_num");

		_AddServerItemData(actItem, actNum);

		WeekSignDataEntryMgr::Instance()->SetLimitWeekSignItem(actItem.actType, actItem.itemUid, actNum);
	}

	return true;
}

bool WeekSignMgr::LoadPlayerActItemData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		ActItem actItem;
		actItem.actType = callback->GetData("act_type");
		actItem.itemUid = callback->GetData("item_id");

		UInt32 actNum = callback->GetData("act_num");
		std::string playerId = callback->GetData("player_id").GetString();

		_AddPlayerItemData(playerId, actItem, actNum);
	}

	return true;
}

bool WeekSignMgr::LoadWeekSignRecord(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		CLProtocol::WeekSignRecord record;
		record.guid = callback->GetKey();
		record.serverName = callback->GetData("server_name").GetString();
		record.playerName = callback->GetData("player_name").GetString();
		record.playerId = callback->GetData("player_id").GetString();
		record.itemId = callback->GetData("item_id");
		record.itemNum = callback->GetData("item_num");
		record.createTime = callback->GetData("create_time");

		UInt32 actType = callback->GetData("act_type");

		_AddWeekSignRecord(actType, record);
	}

	return true;
}

void WeekSignMgr::HandleWeekSignReward(GASPlayer* player, UInt32 actType, UInt32 actId, UInt32 vipLvl)
{
	if (NULL == player) return;

	CLProtocol::GASWeekSignLotteryRes res;
	res.roleId = player->GetID();
	res.actType = actType;
	res.actId = actId;

	const WeekSignItem* signItem = WeekSignDataEntryMgr::Instance()->GetWeekSignItem(actType, vipLvl);
	if (NULL == signItem)
	{
		InfoLogStream << "GetWeekSignItem Null! PlayerId:"<< player->GetID() << " actId:"<< actId << LogStream::eos;

		// 没有抽到给个垃圾物品
		_LotteryTrashItem(player, actType, res.rewardVec);
	}
	else
	{
		// 个人数量是否满足
		if (signItem->roleLimit != 0 && _GetPlayerItemCnt(player->GetUId(), actType, signItem->rewardUid) >= signItem->roleLimit)
		{
			InfoLogStream << PLAYERINFO(player) << "Player Limit Item ActType:" << actType << " itemUid:" << signItem->rewardUid << LogStream::eos;
			_LotteryTrashItem(player, actType, res.rewardVec);
			player->SendToScene(res);
			return;
		}

		// 全服数量是否满足
		if (signItem->serverLimit != 0 && _GetServerItemCnt(actType, signItem->rewardUid) >= signItem->serverLimit)
		{
			InfoLogStream << PLAYERINFO(player) << "Server Limit Item ActType:" << actType << " itemUid:" << signItem->rewardUid << LogStream::eos;
			_LotteryTrashItem(player, actType, res.rewardVec);
			player->SendToScene(res);
			return;
		}

		// 记录个人获得
		if (signItem->roleLimit != 0)
		{
			ActItem actItem(actType, signItem->rewardUid);
			_AddPlayerItemData(player->GetUId(), actItem, 1, true);
		}

		// 记录全服获得
		if (signItem->serverLimit != 0)
		{
			ActItem actItem(actType, signItem->rewardUid);
			_AddServerItemData(actItem, 1, true);
			WeekSignDataEntryMgr::Instance()->SetLimitWeekSignItem(actItem.actType, actItem.itemUid, _GetServerItemCnt(actItem.actType, actItem.itemUid));
		}

		if (signItem->isBigReward != 0)
			_RecordWeekSign(player, actType, *signItem);

		if (signItem->notifyId != 0)
			_PublicWeekSign(actType, signItem->notifyId, signItem->superLink, player->GetServerName(), player->GetPlayerName(), signItem->itemEntry->name);

		ItemReward itemReward(signItem->rewardId, signItem->rewardNum, 0, 0);
		res.rewardVec.push_back(itemReward);
	}

	player->SendToScene(res);
}

void WeekSignMgr::HandleWeekSignRecord(GASPlayer* player, UInt32 actType)
{
	if (NULL == player) return;

	CLProtocol::GASWeekSignRecordRes res;
	res.opActType = actType;
	auto iter = m_WeekSignRecordMap.find(actType);
	if (iter != m_WeekSignRecordMap.end())
	{
		res.record = iter->second;
	}

	player->SendProtocol(res);
}

void WeekSignMgr::_AddServerItemData(ActItem& actItem, UInt32 num, bool _isSave)
{
	auto iter = m_ServerItemMap.find(actItem);
	if (iter == m_ServerItemMap.end())
	{
		m_ServerItemMap[actItem] = num;

		if (_isSave)
			_InsertServerItem(actItem, num);
	}
	else
	{
		iter->second += num;

		if (_isSave)
			_UpdateServerItem(actItem, iter->second);
	}
}

void WeekSignMgr::_AddPlayerItemData(const std::string& playerId, ActItem& actItem, UInt32 num, bool _isSave)
{
	auto iter = m_PlayerItemMap.find(playerId);
	if (iter == m_PlayerItemMap.end())
	{
		ItemNumMap tempMap;
		tempMap[actItem] = num;
		m_PlayerItemMap[playerId] = tempMap;

		if (_isSave)
			_InsertPlayerItem(playerId, actItem, num);
	}
	else
	{
		auto it = iter->second.find(actItem);
		if (it == iter->second.end())
		{
			iter->second[actItem] = num;

			if (_isSave)
				_InsertPlayerItem(playerId, actItem, num);
		}
		else
		{
			it->second += num;

			if (_isSave)
				_UpdatePlayerItem(playerId, actItem, it->second);
		}
	}
}

bool MaxSort(const CLProtocol::WeekSignRecord& a, const CLProtocol::WeekSignRecord& b)
{
	return a.createTime > b.createTime;
}

void WeekSignMgr::_AddWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record)
{
	auto iter = m_WeekSignRecordMap.find(actType);
	if (iter == m_WeekSignRecordMap.end())
	{
		WeekSignRecordVec tempVec;
		tempVec.push_back(record);
		m_WeekSignRecordMap[actType] = tempVec;
	}
	else
	{
		// 如果有50条了，就删掉最早的
		if (iter->second.size() == 50)
		{
			CLProtocol::WeekSignRecord& backRecord = iter->second.back();
			_DelWeekSignRecord(backRecord.guid);
			iter->second.pop_back();
		}

		iter->second.push_back(record);

		std::sort(iter->second.begin(), iter->second.end(), MaxSort);
	}
}

void WeekSignMgr::_LotteryTrashItem(GASPlayer* player, UInt32 actType, ItemRewardVec& rewardVec)
{
	const WeekSignItem* signItem = WeekSignDataEntryMgr::Instance()->GetWeekSignTrashItem(actType);
	if (NULL == signItem)
	{
		ErrorLogStream << "GetWeekSignTrashItem Failed! ActType:"<< actType << LogStream::eos;
		return;
	}

	if (signItem->notifyId != 0)
		_PublicWeekSign(actType, signItem->notifyId, signItem->superLink, player->GetServerName(), player->GetPlayerName(), signItem->itemEntry->name);

	if (signItem->isBigReward != 0)
		_RecordWeekSign(player, actType, *signItem);

	ItemReward itemReward(signItem->rewardId, signItem->rewardNum, 0, 0);
	rewardVec.push_back(itemReward);
}

UInt32 WeekSignMgr::_GetServerItemCnt(UInt32 actType, UInt32 itemUid)
{
	ActItem actItem(actType, itemUid);
	auto iter = m_ServerItemMap.find(actItem);
	if (iter == m_ServerItemMap.end())
		return 0;

	return iter->second;
}

UInt32 WeekSignMgr::_GetPlayerItemCnt(const std::string& playerId, UInt32 actType, UInt32 itemUid)
{
	auto iter = m_PlayerItemMap.find(playerId);
	if (iter == m_PlayerItemMap.end())
		return 0;

	ActItem actItem(actType, itemUid);
	auto it = iter->second.find(actItem);
	if (it == iter->second.end())
		return 0;

	return it->second;
}

void WeekSignMgr::_PublicWeekSign(UInt32 actType, UInt32 notifyId, UInt32 superLink, const std::string& serverName, const std::string& playerName, const std::string& itemName)
{
	std::string systemLinkStr;
	MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, superLink, 0);
	GASAnnouncementMgr::Instance()->PublicAnnouncement(false, notifyId, UInt32(CURRENT_TIME.Sec()), serverName, playerName, itemName, systemLinkStr);
}

void WeekSignMgr::_RecordWeekSign(GASPlayer* player, UInt32 actType, const WeekSignItem& signItem)
{
	CLProtocol::WeekSignRecord record;
	record.guid = CLRecordClient::Instance()->GenGuid();
	record.serverName = player->GetServerName();
	record.playerName = player->GetPlayerName();
	record.itemId = signItem.rewardId;
	record.itemNum = signItem.rewardNum;
	record.playerId = player->GetUId();
	record.createTime = UInt32(CURRENT_TIME.Sec());
	_AddWeekSignRecord(actType, record);
	_InsertWeekSignRecord(actType, record);
}

void WeekSignMgr::_RefreshItemCnt(UInt32 actType, UInt32 refreshType)
{
	for (auto& it : m_PlayerItemMap)
	{
		for (auto & tt : it.second)
		{
			const ActItem& actItem = tt.first;
			if (actItem.actType != actType)
				continue;

			const WeekSignItem* signItem = WeekSignDataEntryMgr::Instance()->FindWeekSignItem(actItem.actType, actItem.itemUid);
			if (NULL == signItem)
			{
				ErrorLogStream << "Not Find WeekSignItem ActType:" << actType << " itemUid:" << actItem.itemUid << LogStream::eos;
				continue;
			}

			if (refreshType != signItem->refreshType)
				continue;

			tt.second = 0;
			_UpdatePlayerItem(it.first, actItem, tt.second);
		}
	}

	for (auto& mt : m_ServerItemMap)
	{
		const ActItem& actItem = mt.first;
		if (actItem.actType != actType)
			continue;

		const WeekSignItem* signItem = WeekSignDataEntryMgr::Instance()->FindWeekSignItem(actItem.actType, actItem.itemUid);
		if (NULL == signItem)
		{
			ErrorLogStream << "Not Find WeekSignItem ActType:" << actType << " itemUid:" << actItem.itemUid << LogStream::eos;
			continue;
		}

		if (refreshType != signItem->refreshType)
			continue;

		mt.second = 0;
		_UpdateServerItem(mt.first, mt.second);
	}
}

void WeekSignMgr::_InsertWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_week_sign_record", record.guid);
	cmd->PutData("act_type", actType);
	cmd->PutData("player_id", record.playerId);
	cmd->PutData("player_name", record.playerName);
	cmd->PutData("server_name", record.serverName);
	cmd->PutData("item_id", record.itemId);
	cmd->PutData("item_num", record.itemNum);
	cmd->PutData("create_time", record.createTime);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_InsertServerItem(ActItem& actItem, UInt32 actNum)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_week_sign_item_server", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("act_type", actItem.actType);
	cmd->PutData("item_id", actItem.itemUid);
	cmd->PutData("act_num", actNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_InsertPlayerItem(const std::string& playerId, ActItem& actItem, UInt32 actNum)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_week_sign_item_player", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("act_type", actItem.actType);
	cmd->PutData("player_id", playerId);
	cmd->PutData("item_id", actItem.itemUid);
	cmd->PutData("act_num", actNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_UpdateServerItem(const ActItem& actItem, UInt32 actNum)
{
	std::ostringstream cond;
	cond << "act_type=" << actItem.actType << " and item_id=" << actItem.itemUid;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_week_sign_item_server", cond.str());
	cmd->PutData("act_num", actNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_UpdatePlayerItem(const std::string& playerId, const ActItem& actItem, UInt32 actNum)
{
	std::ostringstream cond;
	cond << "act_type=" << actItem.actType << " and player_id='" << playerId << "' and item_id=" << actItem.itemUid;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_week_sign_item_player", cond.str());
	cmd->PutData("act_num", actNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_DelItemData(UInt32 actType, const std::string& tableName)
{
	std::ostringstream cond;
	cond << "act_type=" << actType;
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(tableName, cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_DelDBData(UInt32 actType, const std::string& tableName)
{
	std::ostringstream cond;
	cond << "act_type=" << actType;
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(tableName, cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_DelWeekSignRecord(UInt64 guid)
{
	std::ostringstream cond;
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_week_sign_record", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WeekSignMgr::_SaveActId(UInt32 actType, ActRefresh& actFr, bool _isInsert)
{
	if (_isInsert)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_week_sign_act", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("act_type", actType);
		cmd->PutData("act_id", actFr.actId);
		cmd->PutData("refresh_time", actFr.refreshTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		std::ostringstream cond;
		cond << "act_type=" << actType;
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_week_sign_act", cond.str());
		cmd->PutData("act_id", actFr.actId);
		cmd->PutData("refresh_time", actFr.refreshTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}
