#include "LostDungeon.h"
#include <CLLostDungeonDataEntry.h>
#include <CLErrorCode.h>
#include <CLSystemValueDataEntry.h>
#include <CLLostDungeonTreasureChestDataEntry.h>
#include <CLLostDungeonProtocol.h>

#include "Player.h"
#include "ActivityMgr.h"
#include "SSRouter.h"

using namespace ErrorCode;
LostDungeon::LostDungeon()
{
	m_floorDataDirty = false;
}

LostDungeon::~LostDungeon()
{

}

void LostDungeon::OnOnline()
{
	if (!ActivityMgr::Instance()->IsInActivity(ACTIVITY_LOST_DUNGEON))
	{
		Reset();
		return;
	}

	UInt32 startTime = 0;
	UInt32 endTime = 0;
	std::string actName = ACTIVITY_LOST_DUNGEON;
	ActivityMgr::Instance()->GetOpenActivityTime(actName, startTime, endTime);

	//检查创建时间
	if (GetCreateTime() < startTime)
	{
		Reset();
	}
}

void LostDungeon::OnActOpen(ActivityInfo* info)
{
	if (!info)	return;
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_CLOSE);
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_REMAIN_NUM, 1);
	SetLostDungeonScore(0);
}

void LostDungeon::OnActClose()
{
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_CLOSE);
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_REMAIN_NUM, 0);
}

void	LostDungeon::LoadDataFromDb(CLRecordCallback* pCallback)
{
	if (NULL == pCallback) return;
	while (pCallback->NextRow())
	{
		m_guid = pCallback->GetKey();
		m_createTime = pCallback->GetData("create_time");
		m_lostDungeonScore = pCallback->GetData("score");
		m_LostDungeonHp = pCallback->GetData("hp");
		m_LostDungeonMp = pCallback->GetData("mp");
		std::string floorsStr = pCallback->GetData("floors").GetString();
		this->LoadFloorDataFromDB(floorsStr);
	}
}

UInt32 LostDungeon::OnOpen()
{
	if (!GenerateFloors())
	{
		return ErrorCode::LOSTDUNGEON_GENER_FLOOR_FAIL;
	}
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_PROGRESSING);
	//减少次数
	UInt32 curTime = GetOwner()->GetCounter(COUNTER_LOSTDUNG_REMAIN_NUM);
	if (curTime > 0)
	{
		GetOwner()->SetCounter(COUNTER_LOSTDUNG_REMAIN_NUM, curTime - 1);
	}

	SetLostDungeonHp(HP_MP_FULL_VALUE);
	SetLostDungeonMp(HP_MP_FULL_VALUE);

	SetLostDungeonScore(0);

	//清除神器
	GetOwner()->GetItemMgr().ClearPackage(PACK_LOSTDUNGEON, SOURCE_LOSTDUNGEON_REWARD);

	SaveDirtyData();

	return ErrorCode::SUCCESS;
}

bool LostDungeon::GenerateFloors()
{
	m_genFloorDatas.clear();
	UInt32 maxFloor = LostDungeonFloorDataEntryMgr::Instance()->GetMaxFloor();
	//是否生成了npc
	bool isGenedNpc = 0;
	//每一组生成的困难关卡数量
	std::map<UInt32, UInt32> groupHardNodes;
	//每一组生成的宝箱数量
	std::map<UInt32, UInt32> groupTreasNums;
	for (UInt32 floor = maxFloor; floor > 0; --floor)
	{
		LostDungeonFloorDataEntry* dataEntry = LostDungeonFloorDataEntryMgr::Instance()->GetDataEntry(floor);
		if (!dataEntry)
		{
			continue;
		}
		LostDungeonFloorData* floorData = GetFloorData(dataEntry->floor);
		if (floorData == NULL)
		{
			LostDungeonFloorData fd;
			fd.floor = dataEntry->floor;
			fd.state = LDFS_LOCK;
			m_genFloorDatas[dataEntry->floor] = fd;
			floorData = GetFloorData(dataEntry->floor);
			if (!floorData)
			{
				continue;
			}
		}

		if (floorData->nodes.size() >= dataEntry->nodeNum)
		{
			continue;
		}

		//生成战场关卡节点，每层最少要有一个战场入口（困难普通都可以）
		UInt32 groupHardNodeNum = groupHardNodes[dataEntry->hardBattleGenInfo.groupId];
		if (groupHardNodeNum < dataEntry->hardBattleGenInfo.groupMaxNum)
		{
			UInt32 randWeight = Avalon::Random::RandBetween(1, 100);
			if (dataEntry->hardBattleGenInfo.pro >= randWeight)
			{
				//生成困难关卡节点
				groupHardNodes[dataEntry->hardBattleGenInfo.groupId] = ++groupHardNodeNum;
				LostDungeonNode node(LDET_HARD_BATTLE);
				floorData->nodes.push_back(node);
			}
			else
			{
				//生成普通关卡节点
				LostDungeonNode node(LDET_NORMAL_BATTLE);
				floorData->nodes.push_back(node);
			}
		}
		else {
			//生成普通关卡节点
			LostDungeonNode node(LDET_NORMAL_BATTLE);
			floorData->nodes.push_back(node);
		}

		//生成NPC
		if (!isGenedNpc && dataEntry->existMysticShop)
		{
			UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DL_MYSTERIOUS_SHOP_PROBABILITY);
			rate = (rate == 0) ? 5 : rate;
			UInt32 randWeight = Avalon::Random::RandBetween(1, 100);
			if (rate >= randWeight)
			{
				LostDungeonNode node(LDET_NPC);
				floorData->nodes.push_back(node);
				isGenedNpc = true;
			}
		}
		
		if (floorData->nodes.size() >= dataEntry->nodeNum)
		{
			continue;
		}
		//生成宝箱
		UInt32 groupTreasNum = groupTreasNums[dataEntry->treasChestGenInfo.groupId];
		if (dataEntry->treasChestGenInfo.num > 0
			&& groupTreasNum < dataEntry->treasChestGenInfo.groupMaxNum)
		{
			UInt32 randWeight = Avalon::Random::RandBetween(1, 100);
			if (dataEntry->treasChestGenInfo.pro >= randWeight)
			{
				LostDungeonNode node(LDET_TREAS_CHEST);
				node.state = LDBXS_UNOPENED;
				groupTreasNums[dataEntry->treasChestGenInfo.groupId] = ++groupTreasNum;
				//随机宝箱id
				UInt32 totalWeight = 0;
				for (auto it : dataEntry->treasChestWt)
				{
					totalWeight += it.weight;
				}
				UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
				for (auto it : dataEntry->treasChestWt)
				{
					if (it.weight >= randWeight)
					{
						node.treasChestId = it.dataId;
						break;
					}
					else
					{
						randWeight -= it.weight;
					}
				}
				floorData->nodes.push_back(node);
			}
		}

		//节点不满，则生成普通关卡
		if (floorData->nodes.size() < dataEntry->nodeNum)
		{
			LostDungeonNode node(LDET_NORMAL_BATTLE);
			floorData->nodes.push_back(node);
		}
	}
	
	//修改第一层楼层节点状态
	LostDungeonFloorData* firstFloor = GetFloorData(1);
	if (firstFloor)
	{
		firstFloor->state = LDFS_UNLOCK_UNPASS;
		for (auto& node : firstFloor->nodes)
		{
			if (node.type != LDET_NORMAL_BATTLE && node.type != LDET_HARD_BATTLE)
			{
				continue;
			}
			node.state = LDNS_OPEN;
		}
	}
	//GetOwner()->SetCounter(COUNTER_LOSTDUNG_REMAIN_NUM, 1);
	m_createTime = (UInt32)CURRENT_TIME.Sec();
	SaveData();
	return true;
}

LostDungeonFloorData* LostDungeon::GetFloorData(UInt32 floor)
{
	auto it = m_genFloorDatas.find(floor);
	if (it == m_genFloorDatas.end())
	{
		return NULL;
	}
	return &it->second;
}

UInt32 LostDungeon::OnPlayerChallengeReq(UInt32 floor, UInt8 hardType)
{
	if (!ActivityMgr::Instance()->IsInActivity(ACTIVITY_LOST_DUNGEON))
	{
		return ErrorCode::LOSTDUNGEON_FAIL_ACT_CLOSE;
	}

	LostDungeonFloorData* floorData = GetFloorData(floor);
	if (!floorData)
	{
		return ITEM_DATA_INVALID;
	}

	if (floorData->state != LDFS_UNLOCK_UNPASS)
	{
		return ITEM_DATA_INVALID;
	}

	CLProtocol::LostDungeonMathBattleReq mathReq;
	mathReq.playerId = GetOwner()->GetID();
	mathReq.floor = floor;
	mathReq.hardType = hardType;

	Router::SendToBattle(mathReq);
	return SUCCESS;
}

void LostDungeon::GetFloorDatas(std::vector<LostDungeonFloorData>& floorDatas)
{
	for (auto it : m_genFloorDatas)
	{
		floorDatas.push_back(it.second);
	}
	return;
}

UInt32 LostDungeon::OnPlayerOpenBox(UInt32 floor, UInt32 boxId, std::vector<ItemReward>& itemVec)
{
	if (!ActivityMgr::Instance()->IsInActivity(ACTIVITY_LOST_DUNGEON))
	{
		return LOSTDUNGEON_FAIL_ACT_CLOSE;
	}

	auto dataEntry = LostDungeonTreasureDataEntryMgr::Instance()->FindEntry(boxId);
	if (!dataEntry)	return ErrorCode::ITEM_DATA_INVALID;

	LostDungeonFloorData* lostFloor = GetFloorData(floor);
	if (!lostFloor)	return ErrorCode::ITEM_DATA_INVALID;

	if (lostFloor->state == LDFS_LOCK)
	{
		return LOSTDUNGEON_OPENBOX_FAIL_LOCK;
	}

	//判断是否开过
	LostDungeonNode* pNode = NULL;
	for (auto& node : lostFloor->nodes)
	{
		if (node.treasChestId == boxId)
		{
			pNode = &node;
			break;
		}
	}
	if (!pNode)	return ErrorCode::ITEM_DATA_INVALID;

	if (pNode->state == LDBXS_OPENED)
	{
		return LOSTDUNGEON_OPENBOX_FAIL_OPENED;
	}

	if (dataEntry->type == LDTCT_GIFT)	//发礼包
	{
		//随机出一个礼包
		UInt32 giftId = 0;
		UInt32 totalWeight = 0;
		for (UInt32 i = 0; i < dataEntry->gifts.size(); ++i)
		{
			totalWeight += dataEntry->gifts[i].weight;
		}
		UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
		for (UInt32 i = 0; i < dataEntry->gifts.size(); ++i)
		{
			if (dataEntry->gifts[i].weight >= randWeight)
			{
				giftId = dataEntry->gifts[i].dataId;
				break;
			}
			else
			{
				randWeight -= dataEntry->gifts[i].weight;
			}
		}
		if (giftId == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner())  << " Open box fail, gift item id = 0, boxId = " << boxId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		GiftPackData* gpData = GiftPackDataMgr::Instance()->FindEntry(giftId);
		if (!gpData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftPackData is null! id = " << giftId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		std::vector<ItemReward> tempVec;
		for (size_t i = 0; i < gpData->giftItems.size(); ++i)
		{
			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			if (giData->occus.size() == 0)
			{
				ItemReward item(giData->itemId, giData->num, 0, giData->strengthen, 0, giData->equipType, 0);
				tempVec.push_back(item);
			}
			else
			{
				for (size_t j = 0; j < giData->occus.size(); ++j)
				{
					if ((giData->occus[j] == GetOwner()->GetOccu() || giData->occus[j] == 0) &&
						giData->levels[0] <= GetOwner()->GetLevel() &&
						GetOwner()->GetLevel() <= giData->levels[1])
					{
						ItemReward item(giData->itemId, giData->num, 0, giData->strengthen, 0, giData->equipType, 0);
						tempVec.push_back(item);
						break;
					}
				}
			}
		}
		if (!tempVec.empty())
		{
			GetOwner()->SetLostDungeonBoxItemVec(tempVec);
			GetOwner()->SetLostDungeonBoxFloor(floor);
			itemVec = tempVec;
		}
	}
	else if (dataEntry->type == LDTCT_EVENT_CHANGEBATTLE) //事件（修改战场难度）
	{
		bool notify = false;
		for (auto& node : lostFloor->nodes)
		{
			if (node.type == LDET_NORMAL_BATTLE)
			{
				node.type = LDET_HARD_BATTLE;
				node.id = GetFloorElementId(LDET_HARD_BATTLE);
				notify = true;
			}
			else if (node.type == LDET_HARD_BATTLE)
			{
				node.type = LDET_NORMAL_BATTLE;
				node.id = GetFloorElementId(LDET_NORMAL_BATTLE);
				notify = true;
			}
		}
		//标志已经打开过
		pNode->state = LDBXS_OPENED;
		//保存数据库
		m_floorDataDirty = true;
		SaveDirtyData();
		
		if (notify)
		{
			CLProtocol::LostDungeonSyncFloor protocl;
			protocl.floorDatas.push_back(*lostFloor);
			GetOwner()->SendProtocol(protocl);
		}
		
	}

	return SUCCESS;
}

UInt32	LostDungeon::OnPlayerChoiceBoxItem(UInt32 itemId, ItemReward& item)
{
	if (!ActivityMgr::Instance()->IsInActivity(ACTIVITY_LOST_DUNGEON))
	{
		return LOSTDUNGEON_FAIL_ACT_CLOSE;
	}

	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemData)
	{
		return ITEM_DATA_INVALID;
	}
	LostDungeonFloorData* floordata = GetFloorData(GetOwner()->GetLostDungeonBoxFloor());
	if (!floordata)
	{
		return ITEM_DATA_INVALID;
	}
	LostDungeonNode* pNode = NULL;
	for (auto& node : floordata->nodes)
	{
		if (node.type == LDET_TREAS_CHEST)
		{
			pNode = &node;
			break;
		}
	}
	if (!pNode)
	{
		return ITEM_DATA_INVALID;
	}
	if (pNode->state == LDBXS_OPENED)
	{
		return LOSTDUNGEON_OPENBOX_FAIL_OPENED;
	}

	std::vector<ItemReward> itemVec = GetOwner()->GetLostDungeonBoxItemVec();
	bool isFind = false;
	RewardGroup* rgp = GetOwner()->GetRewardGroup();
	if (!rgp)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " RewardGroup is null!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	ItemRewardVec temp;
	for (auto& iter : itemVec)
	{
		if (itemId == iter.id)
		{
			isFind = true;
			item = iter;
			temp.push_back(iter);
			break;
		}
	}
	if (!isFind)
		return ErrorCode::ITEM_DATA_INVALID;

	if (itemData->subType == ST_LOST_DUNGEON_INTEGRAL)
	{
		UInt32 score = GetLostDungeonScore();
		SetLostDungeonScore(score + itemData->useLimiteValue);
	}
	else if (itemData->subType == ST_LOST_DUNGEON_RECOVERY)
	{
		UInt32 hp = GetLostDungeonHp();
		UInt32 mp = GetLostDungeonMp();
		SetLostDungeonHp(hp + itemData->useLimiteValue);
		SetLostDungeonMp(mp + itemData->useLimiteValue);
		SaveDirtyData();
	}
	else
	{
		rgp->AddRewards(temp);
		GetOwner()->AddRewards(GetReason(LogSourceType::SOURCE_LOSTDUNGEON_OPENBOX).c_str(), rgp, true);
	}
	

	pNode->state = LDBXS_OPENED;
	//保存数据库
	m_floorDataDirty = true;
	SaveDirtyData();

	CLProtocol::LostDungeonSyncFloor protocl;
	protocl.floorDatas.push_back(*floordata);
	GetOwner()->SendProtocol(protocl);
	return ErrorCode::SUCCESS;
}

void LostDungeon::SaveData()
{
	if (m_guid == 0)
	{
		return InsertData();
	}
	CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_lost_dungeon", m_guid, false);
	pCmd->PutData("create_time", m_createTime);
	pCmd->PutData("hp", m_LostDungeonHp);
	pCmd->PutData("mp", m_LostDungeonMp);
	pCmd->PutData("score", m_lostDungeonScore);
	pCmd->PutData("floors", GetFloorDataStr());
	CLRecordClient::Instance()->SendCommand(pCmd);
}

void LostDungeon::SaveDirtyData()
{
	CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_lost_dungeon", m_guid, false);
	bool save = false;
	
	if (m_LostDungeonHp.IsDirty(DB_SYNC_TYPE))
	{
		pCmd->PutData("hp", m_LostDungeonHp);
		m_LostDungeonHp.ClearDirty(DB_SYNC_TYPE);
		save = true;
	}
	if (m_LostDungeonMp.IsDirty(DB_SYNC_TYPE))
	{
		pCmd->PutData("mp", m_LostDungeonMp);
		m_LostDungeonMp.ClearDirty(DB_SYNC_TYPE);
		save = true;
	}
	if (m_lostDungeonScore.IsDirty(DB_SYNC_TYPE))
	{
		pCmd->PutData("score", m_lostDungeonScore);
		m_lostDungeonScore.ClearDirty(DB_SYNC_TYPE);
		save = true;
	}
	if (m_floorDataDirty)
	{
		pCmd->PutData("floors", GetFloorDataStr());
		m_floorDataDirty = false;
		save = true;
	}
	if (save)
	{
		CLRecordClient::Instance()->SendCommand(pCmd);
	}
}

void LostDungeon::InsertData()
{
	m_guid = CLRecordClient::Instance()->GenGuid();
	CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_lost_dungeon", m_guid);
	pCmd->PutData("owner", m_owner->GetID());
	pCmd->PutData("create_time", m_createTime);
	pCmd->PutData("hp", m_LostDungeonHp);
	pCmd->PutData("mp", m_LostDungeonMp);
	pCmd->PutData("score", m_lostDungeonScore);
	pCmd->PutData("floors", GetFloorDataStr());
	CLRecordClient::Instance()->SendCommand(pCmd);
}

std::string LostDungeon::GetFloorDataStr()
{
	std::stringstream ss;
	for (auto it : m_genFloorDatas)
	{
		LostDungeonFloorData& floorData = it.second;
		ss << floorData.GetStrForDB() << "|";
	}
	return ss.str();
}

void LostDungeon::LoadFloorDataFromDB(std::string str)
{
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	for (auto s : strs)
	{
		LostDungeonFloorData floorData;
		if (floorData.LoadFromDB(s))
		{
			m_genFloorDatas[floorData.floor] = floorData;
		}
	}
}

void LostDungeon::SettleFloor(UInt32 floor, UInt8 result, UInt32 score, UInt8 btState)
{
	InfoLogStream << "settle floor player id : " << GetOwner()->GetID()
		<< ", floor : " << floor << ", result : " << (UInt32)result << ", score : " << score << ", btState : " << (UInt32)btState <<  LogStream::eos;

	bool syncFloors = false;

	if (result == LDBR_ABN_EXIT)
	{
		if (floor == 0)
		{
			floor = GetChallengeFloor();
		}
		if (btState > 0)
		{
			result = LDBR_FAIL;
			syncFloors = true;
		}
		else
		{
			return;
		}
	}

	UInt32 maxFloor = LostDungeonFloorDataEntryMgr::Instance()->GetMaxFloor();
	if (floor > maxFloor)
	{
		ErrorLogStream << "floor > maxFloor floor : " << floor << ", maxFloor : " << maxFloor 
			<< ", player id : " << GetOwner()->GetID() << LogStream::eos;
		return;
	}

	LostDungeonFloorData* floorData = GetFloorData(floor);
	if (!floorData)
	{
		ErrorLogStream << "not find floor data floor : " << floor << ", player id : " << GetOwner()->GetID() << LogStream::eos;
		return;
	}

	CLProtocol::SceneLostDungeonSettleFlooorNotify notify;
	notify.floor = floor;
	notify.battleResult = result;
	notify.addScore = score;
	notify.againItemId = 0;
	notify.decScore = 0;

	CLProtocol::LostDungeonSyncFloor syns;

	bool save = false;

	if (result == LDBR_PASS) //通关
	{
		//加积分
		UInt32 oldScore = GetOwner()->GetLostDungeonScore();
		GetOwner()->SetLostDungeonScore(oldScore + score);
		save = true;
		//修改楼层状态
		floorData->state = LDFS_UNLOCK_PASS;
		m_floorDataDirty = true;
		UpdateOneFloor(floorData);
		syns.floorDatas.push_back(*floorData);
		//解锁下一层
		if (floor < maxFloor)
		{
			LostDungeonFloorData* nextFloor = GetFloorData(floor + 1);
			if (nextFloor)
			{
				nextFloor->state = LDFS_UNLOCK_UNPASS;
				UpdateOneFloor(nextFloor);
				syns.floorDatas.push_back(*nextFloor);
			}
			else
			{
				ErrorLogStream << "not find next floor data floor : " << (floor + 1) << ", player id : " << GetOwner()->GetID() << LogStream::eos;
			}
		}
		else if (floor == maxFloor) //最终层完成，结束本次爬塔
		{
			GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_END_CAN_REWARD);
		}
		else
		{
		}
	}
	else if(result == LDBR_FAIL) //失败
	{
		auto floorDataEntry = LostDungeonFloorDataEntryMgr::Instance()->GetDataEntry(floorData->floor);
		if (!floorDataEntry)
		{
			ErrorLogStream << "not find floor data entry floor : " << floorData->floor << ", player id : " << GetOwner()->GetID() << LogStream::eos;
			return;
		}
		if (floorDataEntry->failHandlerId == LDFH_NONE) //无处理
		{
			notify.score = GetOwner()->GetLostDungeonScore();
		}
		else if (floorDataEntry->failHandlerId == LDFH_FALL)	//掉层扣积分
		{
			if (GetOwner()->GetItemNum(floorDataEntry->notFallItemId) == 0) //没有道具
			{
				save = true;
				m_floorDataDirty = true;
				//扣积分
				UInt32 curSocre = GetOwner()->GetLostDungeonScore();
				if (curSocre > floorDataEntry->failDecScore)
				{
					GetOwner()->SetLostDungeonScore(curSocre - floorDataEntry->failDecScore);
				}
				else
				{
					GetOwner()->SetLostDungeonScore(0);
				}

				notify.decScore = floorDataEntry->failDecScore;

				floorData->state = LDFS_LOCK;
				UpdateOneFloor(floorData);
				syns.floorDatas.push_back(*floorData);

				if (floorData->floor > 1)
				{
					UInt32 upperFloor = floorData->floor - 1;
					LostDungeonFloorData* upperFloorData = GetFloorData(upperFloor);
					if (upperFloorData)
					{
						upperFloorData->state = LDFS_UNLOCK_UNPASS;
						UpdateOneFloor(upperFloorData);
						syns.floorDatas.push_back(*upperFloorData);
					}
					else
					{
						ErrorLogStream << "not find upper floor data floor : " << upperFloor << ", player id : " << GetOwner()->GetID() << LogStream::eos;
					}
				}
			}
			else //扣除一个道具
			{
				GetOwner()->RemoveItem(GetReason(SOURCE_LOSTDUNGEON_FAIL, floor).c_str(), floorDataEntry->notFallItemId, 1);
				notify.againItemId = floorDataEntry->notFallItemId;
			}
		}
		else if (floorDataEntry->failHandlerId == LDFH_SETTLE) //失败并结算
		{
			if (GetOwner()->GetItemNum(floorDataEntry->againItemId) == 0) //没有道具，结束本次爬塔
			{
				GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_END_CAN_REWARD);
			}
			else //扣除一个道具
			{
				notify.againItemId = floorDataEntry->againItemId;
				GetOwner()->RemoveItem(GetReason(SOURCE_LOSTDUNGEON_FAIL, floor).c_str(), floorDataEntry->againItemId, 1);
			}
		}

	}
	else if (result == LDBR_OVER) //活动结束
	{
	}
	else if (result == LDBR_ABN_EXIT)
	{
	}
	else
	{
		ErrorLogStream << "result is err,  floor : " << floor  << ", player id : " << GetOwner()->GetID() << ", result : " << result << LogStream::eos;
	}

	notify.score = GetOwner()->GetLostDungeonScore();

	if (syns.floorDatas.size() > 0)
	{
		GetOwner()->SendProtocol(syns);
	}

	if (save)
	{
		SaveDirtyData();
	}

	if (syncFloors)
	{
		CLProtocol::LostDungeonSyncFloor syns1;
		GetFloorDatas(syns1.floorDatas);
		GetOwner()->SendProtocol(syns1);
	}
	GetOwner()->SendProtocol(notify);
}

void LostDungeon::UpdateOneFloor(LostDungeonFloorData* floor)
{
	if (!floor) return;

	UInt32 maxFloor = LostDungeonFloorDataEntryMgr::Instance()->GetMaxFloor();

	for (auto& node : floor->nodes)
	{
		if (node.type != LDET_NORMAL_BATTLE && node.type != LDET_HARD_BATTLE)
		{
			continue;
		}

		if (floor->state == LDFS_LOCK)	//封锁
		{
			node.state = LDNS_LOCK;
		}
		else if (floor->state == LDFS_UNLOCK_UNPASS)	//解锁未通关
		{
			node.state = LDNS_OPEN;
		}
		else if (floor->state == LDFS_UNLOCK_PASS)	//已通关
		{
			node.state = LDNS_CLOSE;
			if (floor->floor < maxFloor)
			{
				auto nextFloorData = LostDungeonFloorDataEntryMgr::Instance()->GetDataEntry(floor->floor + 1);
				if (nextFloorData && nextFloorData->failHandlerId == LDFH_FALL)
				{
					node.state = LDNS_HALF_OPEN;
				}
			}
		}
		else
		{
			ErrorLogStream << "player : " << GetOwner()->GetID() << ", floor : " << floor->floor
				<< "state is error, state : " << floor->state << LogStream::eos;
		}
	}
}

UInt32 LostDungeon::GetReward()
{
	if (GetOwner()->GetCounter(COUNTER_LOSTDUNG_STATE) != LDS_END_CAN_REWARD)
	{
		return LOSTDUNGEON_REWARD_FAIL_ST;
	}
	//发放奖励，设置爬塔状态
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_CLOSE);
	//清除神器
	GetOwner()->GetItemMgr().ClearPackage(PACK_LOSTDUNGEON, SOURCE_LOSTDUNGEON_REWARD);
	SetLostDungeonScore(0);
	return SUCCESS;
}

UInt32 LostDungeon::OnPlayerGiveUp()
{
	if (GetOwner()->GetCounter(COUNTER_LOSTDUNG_STATE) != LDS_PROGRESSING)
	{
		return LOSTDUNGEON_OPFAIL_ST;
	}
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_END_CAN_REWARD);
	return SUCCESS;
}

void LostDungeon::SetLostDungeonHp(Int32 hp)
{
	m_LostDungeonHp = hp;

	if (m_LostDungeonHp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(GetOwner()) << ") invalid hp(" << m_LostDungeonHp << ")." << LogStream::eos;
		m_LostDungeonHp = HP_MP_FULL_VALUE;
	}

}

void LostDungeon::SetLostDungeonMp(Int32 mp)
{
	m_LostDungeonMp = mp;

	if (m_LostDungeonMp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(GetOwner()) << ") invalid mp(" << m_LostDungeonMp << ")." << LogStream::eos;
		m_LostDungeonMp = HP_MP_FULL_VALUE;
	}
}

bool LostDungeon::OnPlayerDeadInBattle(UInt32 floor)
{
	bool revive = false;
	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_REVIVE_STONE_ITEM);
	//判断是否有复活石头
	if (GetOwner()->GetItemNum(itemId) > 0)
	{
		GetOwner()->RemoveItem(GetReason(SOURCE_LOSTDUNGEON_FAIL, floor).c_str(), itemId, 1);
		
		revive = true;
	}
	return revive;
}

void LostDungeon::SetCreateTime(UInt32 time)
{ 
	m_createTime = time;

}

void LostDungeon::Reset()
{
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_STATE, LDS_CLOSE);
	GetOwner()->SetCounter(COUNTER_LOSTDUNG_REMAIN_NUM, 1);
}

UInt8  LostDungeon::GetChallengeFloor()
{
	for (auto it : m_genFloorDatas)
	{
		auto floorData = it.second;
		if (floorData.state == LDFS_UNLOCK_UNPASS)
		{
			return floorData.floor;
		}
	}
	return 0;
}