#include "OpenJarMgr.h"
#include "CLRecordCommand.h"
#include "CLRecordClient.h"
#include "CLItemDataEntry.h"
#include "Player.h"
#include "CLSystemValueDataEntry.h"
#include "CLErrorCode.h"
#include "JarItem.h"
#include "CLDungeonProtocol.h"
#include "AsyncMsgCallback.h"
#include "AsyncMsgCallbackMgr.h"
#include "SSRouter.h"
#include "CLChatProtocol.h"
#include "SceneSysNotify.h"
#include "SSServiceSwitchMgr.h"
#include "CLMsgParser.h"
#include "CLFunctionDataEntry.h"
#include "ActivityMgr.h"
#include "CLOpActiveProtocol.h"
#include <array>
#include "SSApplication.h"

using namespace ErrorCode;
std::array<UInt32, 5> ARTIFACT_JAR_ID = { 601, 602, 603, 605, 606 };

OpenJarMgr::OpenJarMgr()
{
	m_inited = false;
	m_opening = false;
}


OpenJarMgr::~OpenJarMgr()
{
	DeletAllMap();
	ClearHitItems();
}

bool OpenJarMgr::LoadJarItem(JarItem* jarItem)
{
	if (!jarItem)
		return false;

	JarBonusTrpKey tpKey(jarItem->GetType(), jarItem->GetOccus(), jarItem->GetItemId());
	JarItemPoolData* data = JarItemPoolMgr::Instance()->GetPoolData(tpKey);

	if (!data)
	{
		//CL_SAFE_DELETE(jarItem);
		return false;
	}
	
	jarItem->SetDBState(SDS_UPDATE);
	jarItem->SetData(data);
	jarItem->SetOwner(GetOwner()->GetID());

	CreateMap(jarItem);

	return true;
}

//(废弃)
void OpenJarMgr::Init()
{
	if (m_inited)
	{
		return;
	}

	m_inited = true;

	DebugLogStream << "player(" << GetOwner()->GetID() << ") occu(" << GetOwner()->GetOccu() << ") start to init jar pool." << LogStream::eos;

	JarItemPoolMgr::BonusVec datas;
	BonusVec myDatas;
	JarItemPoolMgr::Instance()->GetOccuToDatas(GetOwner()->GetOccu(), datas);
	GetOccuDatas(myDatas);

	if (bonusPool.empty())
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") occu(" << GetOwner()->GetOccu() << ") init jar pool failed." << LogStream::eos;
	}

	//配表data里有新的数据就更新内存
	for (size_t i = 0; i < datas.size(); ++i)
	{
		for (size_t j = 0; j < myDatas.size(); ++j)
		{
			if (datas[i]->id == myDatas[j]->GetData()->id &&
				datas[i]->type == myDatas[j]->GetData()->type)
			{
				myDatas[j]->SetData(datas[i]);
				break;
			}
		}
	}

 	DebugLogStream << PLAYERINFO(GetOwner()) << "table data num = " << (UInt32)datas.size() << LogStream::eos;
 	DebugLogStream << PLAYERINFO(GetOwner()) << "bonusPool num = " << (UInt32)bonusPool.size() << LogStream::eos;
 	DebugLogStream << PLAYERINFO(GetOwner()) << "bonusPoolColor num = " << (UInt32)bonusPoolColor.size() << LogStream::eos;
 	DebugLogStream << PLAYERINFO(GetOwner()) << "occuToDatas num = " << (UInt32)occuToDatas.size() << LogStream::eos;
}

UInt32 OpenJarMgr::OpenJar(UInt32 id, UInt8 combo, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit, UInt32& baseItemId, UInt32& baseItemNum, UInt32 opActId)
{
	ClearHitItems();

	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(id);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "jarbonus data is null, key = " << SVT_ONEKEY_DECOMPOSE_LV << LogStream::eos;
		GetOwner()->SendNotify("jarbonus data is null! key = {0}", id);
		return ITEM_DATA_INVALID;
	}

	//是否是装备回收罐子
	if (IsEquipRewardJar((JarType)data->type))
	{
		UInt32 checkRet = GetOwner()->GetItemMgr().checkEqRecOpenJar(id);
		if (checkRet != SUCCESS)
		{
			return checkRet;
		}
	}

	OpActivityRegInfo* opAct = NULL;

	// 检测运营活动
	if (opActId > 0)
	{
		opAct = ActivityMgr::Instance()->GetOpActData(opActId);
		if (!opAct)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " OpActivity(" << opActId << ")'s data is not exist!" << LogStream::eos;
			return ITEM_ARTIFACT_JAR_ACTIVITY_CLOSE;
		}

		if (!ActivityMgr::Instance()->IsInOpActivity(opAct, GetOwner()))
		{
			return ITEM_ARTIFACT_JAR_ACTIVITY_CLOSE;
		}
	}
	

	//首次开魔罐
	bool isFirstOpenMagJar = false;
	if ((JarType)data->type == JT_MAGIC && GetOwner()->GetCount(FIRST_OPEN_MAGJAR) == 0 && 1 == combo)
	{
		isFirstOpenMagJar = true;
	}

	
	UInt32 costMoney = 0;
	
	//获取连抽次数
	UInt32 comboBuyNum = 1;
	if (combo > 1)
		comboBuyNum = data->comboBuyNum;

	//获取免费次数,只有1抽才能享受免费次数
	UInt32 freeNum = 0;
	if (1 == comboBuyNum)
	{
		freeNum = CheckFreeCD(data);
	}

	//不使用开罐凭证判断钱是否够用
	if (freeNum == 0 && ((data->needExItem > 0 && GetOwner()->GetItemNum(data->exItemId) < data->exItemNum * comboBuyNum) ||
		data->needExItem == 0))
	{
		UInt32 checkRet = CheckOpenJar(data, comboBuyNum, costMoney, opAct);
		if (checkRet != SUCCESS)
		{
			return checkRet;
		}
	}

	//by huchenhui 2017.08.07 拆分单抽和十连抽.
	LogSourceType reasonType = LogSourceType::SOURCE_TYPE_ITEM_JAR;
	if (comboBuyNum >= 10)
		reasonType = LogSourceType::SOURCE_TYPE_ITEM_JAR_TEN;

	std::string reasonStr = GetReason(reasonType, data->id, comboBuyNum);

	// 扣钱或者凭证道具
	if (costMoney > 0)
	{
		// 安全锁开启，不允许消耗点券
		if (data->moneyType == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "jarbonus security lock not allow." << LogStream::eos;
			return ErrorCode::SECURITY_LOCK_FORBID_OP;
		}

		//花钱
		if (!GetOwner()->RemoveItem(reasonStr.c_str(), data->moneyType, costMoney))
		{
			return ITEM_NUM_INVALID;
		}

		GetOwner()->IncCounter(data->countDayLimite.c_str(), 1);

		//增加活动罐子单抽折扣次数
		if (IsActivityJar((JarType)data->type) && opAct == NULL)
		{
// 			UInt32 disNum = data->discountMaxNum;
// 			std::ostringstream os;
// 			os << ACTJAR_DIS_NUM_PRE << data->id;
// 			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
// 			if (!counterConfig)
// 			{
// 				CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
// 			}
// 			else
// 			{
// 				disNum = GetOwner()->GetCount(os.str().c_str());
// 			}

			std::ostringstream os;
			os << ACTJAR_DIS_NUM_PRE << data->id;
			UInt32 disNum = GetOwner()->GetCount(os.str().c_str());

			if (disNum < comboBuyNum)
			{
				GetOwner()->SetCounter(os.str().c_str(), 0);
			}
			else
			{
				GetOwner()->SetCounter(os.str().c_str(), disNum - comboBuyNum);
			}
		}

	}
	else if (freeNum > 0)
	{
		//使用免费次数
		GetOwner()->SetCounter(data->freeNumCounterKey.c_str(), freeNum - 1);
	}
	else
	{
		//使用开罐凭证道具
		if (data->exItemId > 0 && data->exItemNum > 0 && !GetOwner()->RemoveItem(reasonStr.c_str(), data->exItemId, data->exItemNum * comboBuyNum))
		{
			return ITEM_NUM_INVALID;
		}
	}

	// 得道具
	if (isFirstOpenMagJar)
	{
		RewardGroup* grp = GetOwner()->GetRewardGroup();

		//获得基本道具
		for (UInt32 i = 0; i < data->getItems.size(); ++i)
		{
			UInt32 num = data->getItems[i].minNum;
			if (data->getItems[i].minNum != data->getItems[i].maxNum)
			{
				num = Avalon::Random::RandBetween(data->getItems[i].minNum, data->getItems[i].maxNum);
			}
			num *= comboBuyNum;

			grp->AddReward(data->getItems[i].itemId, num);
			baseItemId = data->getItems[i].itemId;
			baseItemNum = num;
			break;
		}
		
		UInt32 fakeJarItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_JAR_FIRST_OPEN_GET_ITEM);
		JarItemPoolData* jarItemData = JarItemPoolMgr::Instance()->FindEntry(fakeJarItemId);
		if (!jarItemData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " fake item data is null! id:" << fakeJarItemId << LogStream::eos;
			return ITEM_DATA_INVALID;
		}
		grp->AddReward(jarItemData->itemId, jarItemData->num);
		m_hitJarItems.push_back(fakeJarItemId);
		GetOwner()->AddJarRewards(reasonStr.c_str(), grp);

		GetOwner()->SetCounter(FIRST_OPEN_MAGJAR, 1);

		SendUdpLog(data, comboBuyNum, grp->GetRewards());
	}
	else
	{
		GiveJarRewards(reasonStr, data, comboBuyNum, false, baseItemId, baseItemNum);
	}
	

	//抽到特定品质道具记录下来
	OpenSpecifyItemRecord(id, data->name, (JarType)data->type);
	
	GetOwner()->OnOpenMagicJar((JarType)data->type, comboBuyNum);

	//获得相应开罐积分和暴击积分
	OnGetPoint(reasonStr, data, comboBuyNum, getPointId, getPoint, getPointCrit);
	
	//是否是装备回收罐子
	if (IsEquipRewardJar((JarType)data->type))
	{
		GetOwner()->GetItemMgr().handleEqRecOpenJar(id);
	}
	GetOwner()->SyncProperty();

	// 神器罐活动
	ArtifactJarActivity(id, comboBuyNum);

	return SUCCESS;
}

struct OpenJarStatistic
{
	OpenJarStatistic()
	{
		memset(this, 0, sizeof(*this));
	}

	UInt32 itemId;
	// 生成数量
	UInt32 num;
	// 生成次数
	UInt32 times;
};

void OpenJarMgr::OpenJarSample(UInt32 type, UInt32 combo, UInt32 num)
{
// 	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(type);
// 	if (!data)
// 	{
// 		ErrorLogStream << PLAYERINFO(GetOwner()) << "jarbonus data is null, key = " << SVT_ONEKEY_DECOMPOSE_LV << LogStream::eos;
// 		GetOwner()->SendNotify("jarbonus data is null! key = {0}", type);
// 		return ITEM_DATA_INVALID;
// 	}
// 
// 	UInt32 costMoney = 0;
// 
// 	//获取连抽次数
// 	UInt32 comboBuyNum = combo;
// 
// 	RewardGroup* grp = GetOwner()->GetRewardGroup();
// 
// 	UInt32 randNum = 0;
// 	for (UInt32 n = 0; n < comboBuyNum; ++n)
// 	{
// 		//获得奖励道具池
// 		BonusVec pool;
// 		GetBonusPool(data->id, data, pool);
// 
// 		//抽奖次数累计奖励
// 		randNum++;
// 
// 		//先处理特定次数必掉掉落
// 		for (UInt32 i = 0; i < data->countBonus.size(); ++i)
// 		{
// 			if (data->countBonus[i].count == 0)
// 				continue;
// 
// 			if (randNum % data->countBonus[i].count == 0)
// 			{
// 				std::vector<JarItem*> exBonus;
// 				JarBonusColorKey key(data->id, data->countBonus[i].color);
// 				GetBonusPool(key, data, exBonus);
// 
// 				FakeWeightVec fakePool;
// 				GetFakeWeightBase(exBonus, fakePool);
// 
// 				FakeWeightBase* hiter = WeightRander::RandWeight(fakePool);
// 				if (!hiter)
// 				{
// 					DebugLogStream << PLAYERINFO(GetOwner()) << "jaritem randweight err hiter is null!" << LogStream::eos;
// 					continue;
// 				}
// 
// 				JarItem* hitJarItem = (JarItem*)(hiter->GetOwner());
// 				hitJarItem->IncGetNum();
// 				grp->AddReward(hitJarItem->GetItemId(), hitJarItem->GetData()->num);
// 			}
// 		}
// 
// 		//普通抽奖
// 		FakeWeightVec fakePool;
// 		GetFakeWeightBase(pool, fakePool);
// 
// 		FakeWeightBase* hiter = WeightRander::RandWeight(fakePool);
// 		if (!hiter)
// 		{
// 			DebugLogStream << PLAYERINFO(GetOwner()) << "jaritem randweight err hiter is null!" << LogStream::eos;
// 			continue;
// 		}
// 
// 		JarItem* hitJarItem = (JarItem*)(hiter->GetOwner());
// 		hitJarItem->IncGetNum();
// 		grp->AddReward(hitJarItem->GetItemId(), hitJarItem->GetData()->num);
// 	}
// 
// 	UInt32 colorNum[ITEM_QUALITY_MAX];
// 	memset(colorNum, 0, sizeof(UInt32)*ITEM_QUALITY_MAX);
// 	for (size_t i = 0; i < grp->GetRewards().size(); ++i)
// 	{
// 		const ItemRewardVec& grpVec = grp->GetRewards();
// 		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(grpVec[i].id);
// 		if (!data)	continue;
// 		
// 		colorNum[data->color]++;
// 	}
// 
// 	DebugLogStream << PLAYERINFO(GetOwner()) 
// 	<< "whiteNum = " << colorNum[ITEM_QUALITY_WHITE]
// 	<< "blueNum = " << colorNum[ITEM_QUALITY_BLUE]
// 	<< "purpleNum = " << colorNum[ITEM_QUALITY_PURPLE]
// 	<< "greenNum = " << colorNum[ITEM_QUALITY_GREEN]
// 	<< "pinkNum = " << colorNum[ITEM_QUALITY_PINK]
// 	<< "yellowNum = " << colorNum[ITEM_QUALITY_YELLOW]
// 	<< LogStream::eos;
// 
// 	char output[1024];
// 	sprintf(output, "whiteNum = %u, blueNum = %u, purpleNum = %u, greenNum = %u, pinkNum = %u, yellowNum = %u",
// 		colorNum[ITEM_QUALITY_WHITE], colorNum[ITEM_QUALITY_BLUE], colorNum[ITEM_QUALITY_PURPLE],
// 		colorNum[ITEM_QUALITY_GREEN], colorNum[ITEM_QUALITY_PINK], colorNum[ITEM_QUALITY_YELLOW]);
// 
// 	CLProtocol::SceneSyncChat protocol;
// 	protocol.channel = CHAT_CHANNEL_SYSTEM;
// 	protocol.objname = "";
// 	protocol.word = std::string(output);
// 	protocol.bLink = true;
// 	protocol.isGm = 0;
// 	protocol.voiceKey = "";
// 	protocol.voiceDuration = 0;
// 	GetOwner()->SendProtocol(protocol);

	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(type);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "jarbonus data is null, key = " << SVT_ONEKEY_DECOMPOSE_LV << LogStream::eos;
		GetOwner()->SendNotify("jarbonus data is null! key = {0}", type);
		return;
	}
	//获取连抽次数
	UInt32 comboBuyNum = 1;
	if (combo > 1)
	comboBuyNum = data->comboBuyNum;

	LogSourceType reasonType = LogSourceType::SOURCE_TYPE_ITEM_JAR;
	if (comboBuyNum >= 10)
	reasonType = LogSourceType::SOURCE_TYPE_ITEM_JAR_TEN;

	std::string reasonStr = GetReason(reasonType, data->id, comboBuyNum);

	bool isMagicBox = false;
	ReasonInfo ri;
	if (ParseReason(reasonStr.c_str(), ri))
	{
		if (ri.type == SOURCE_TYPE_OPEN_MAG_BOX)
		{
			isMagicBox = true;
		}
	}

	RewardGroup* grp = GetOwner()->GetRewardGroup();

	for (UInt32 j = 0; j < num; j++)
	{
		//获得基本道具
		for (UInt32 i = 0; i < data->getItems.size(); ++i)
		{
			UInt32 num = data->getItems[i].minNum;
			if (data->getItems[i].minNum != data->getItems[i].maxNum)
			{
				num = Avalon::Random::RandBetween(data->getItems[i].minNum, data->getItems[i].maxNum);
			}

			if (!isMagicBox)
			{
				num *= comboBuyNum;
			}

			grp->AddReward(data->getItems[i].itemId, num);
		}

		for (UInt32 n = 0; n < comboBuyNum; ++n)
		{
			RandJarItem(data, isMagicBox);
		}
	}

	std::map<UInt32, OpenJarStatistic> openJarStatics;
	const ItemRewardVec& rewards = grp->GetRewards();
	for (UInt32 i = 0; i < rewards.size(); i++)
	{
		const ItemReward& re = rewards[i];
		auto& sta = openJarStatics[re.id];
		sta.num += re.num;
		sta.times++;
	}
	std::string rootpath = SSApplication::Instance()->GetJarCountPath();
	std::string accidStr = Avalon::StringUtil::ConvertToString(GetOwner()->GetAccID());
	std::string typeStr = Avalon::StringUtil::ConvertToString(type);
	std::string comboStr = Avalon::StringUtil::ConvertToString(combo);
	std::string numStr = Avalon::StringUtil::ConvertToString(num);

	std::string postfix = accidStr + "_" + typeStr + "_" + comboStr + "_" + numStr + ".csv";
	std::string filename = rootpath + "OpenJarSampleCount_" + postfix;

	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
		return;
	}
	std::string header = "itemid,name,times,num\n";
	ofs.write(header.c_str(), header.length());
	for (auto itr : openJarStatics)
	{
		auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first);
		if (!data)
		{
			continue;
		}

		std::ostringstream oss;
		oss << itr.first << ", " << data->name << ", " << itr.second.times << ", " << itr.second.num << "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}

	ofs.close();

	grp->ClearRewards();
	return;
}

UInt32 OpenJarMgr::GiveJarRewards(const std::string& reason, UInt32 type)
{
	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(type);
	if (!data)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 baseItemId = 0;
	UInt32 baseItemNum = 0;
	GiveJarRewards(reason, data, data->comboBuyNum, false, baseItemId, baseItemNum);
	return ErrorCode::SUCCESS;
}

void OpenJarMgr::GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum)
{
	bool isMagicBox = false;
	ReasonInfo ri;
	if (ParseReason(reason.c_str(), ri))
	{
		if (ri.type == SOURCE_TYPE_OPEN_MAG_BOX)
		{
			isMagicBox = true;
		}
	}

	RewardGroup* grp = GetOwner()->GetRewardGroup();

	//获得基本道具
	for (UInt32 i = 0; i < data->getItems.size(); ++i)
	{
		UInt32 num = data->getItems[i].minNum;
		if (data->getItems[i].minNum != data->getItems[i].maxNum)
		{
			num = Avalon::Random::RandBetween(data->getItems[i].minNum, data->getItems[i].maxNum);
		}

		if (!isMagicBox)
		{
			num *= comboBuyNum;
		}
		
		grp->AddReward(data->getItems[i].itemId, num);
		baseItemId = data->getItems[i].itemId;
		baseItemNum = num;
	}

	for (UInt32 n = 0; n < comboBuyNum; ++n)
	{
		RandJarItem(data, isMagicBox);
	}

	GetOwner()->AddJarRewards(reason.c_str(), grp, bNotify, 1);

	SendUdpLog(data, comboBuyNum, grp->GetRewards());

}

void OpenJarMgr::GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum, RewardGroup& rewardGroup)
{
	bool isMagicBox = false;
	ReasonInfo ri;
	if (ParseReason(reason.c_str(), ri))
	{
		if (ri.type == SOURCE_TYPE_OPEN_MAG_BOX)
		{
			isMagicBox = true;
		}
	}

	RewardGroup* grp = GetOwner()->GetRewardGroup();

	//获得基本道具
	for (UInt32 i = 0; i < data->getItems.size(); ++i)
	{
		UInt32 num = data->getItems[i].minNum;
		if (data->getItems[i].minNum != data->getItems[i].maxNum)
		{
			num = Avalon::Random::RandBetween(data->getItems[i].minNum, data->getItems[i].maxNum);
		}

		if (!isMagicBox)
		{
			num *= comboBuyNum;
		}

		grp->AddReward(data->getItems[i].itemId, num);
		//rewardGroup.AddReward(data->getItems[i].itemId, num);
		baseItemId = data->getItems[i].itemId;
		baseItemNum = num;
	}

	for (UInt32 n = 0; n < comboBuyNum; ++n)
	{
		RandJarItem(data, isMagicBox);
	}
	if (grp->GetRewards().size() > 0)
	{
		rewardGroup.AddRewards(grp->GetRewards());
	}

	RewardGroup realRewardGroup;

	for (auto reward : grp->GetRewards())
	{
		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(reward.id);
		if (!itemData) continue;

		if (itemData->subType == ST_RETURN_TOKEN)
		{
			auto activitySystemConfigData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
			if (!activitySystemConfigData) continue;

			if (ActivityMgr::Instance()->IsInOpActivity(activitySystemConfigData->returnActId, GetOwner()))
			{
				std::string tokenKey = Avalon::StringUtil::ConvertToString<UInt32>(activitySystemConfigData->returnActId) + OPACT_FATIGUE_TOKENS;
				UInt32 totalNum = GetOwner()->IncCounter(tokenKey.c_str(), reward.num);
				InfoLogStream << PLAYERINFO(GetOwner()) << "Add fatigue token(" << reward.num << ") by use item(" << reward.id << ") and JarBonus(" << data->id << "), total num=" << totalNum << LogStream::eos;
			}
		}
		else
		{
			realRewardGroup.AddReward(reward.id, reward.num, reward.qualityLv, reward.strenth);
		}
	}

	GetOwner()->AddRewards(reason.c_str(), &realRewardGroup, bNotify, 1);

	SendUdpLog(data, comboBuyNum, grp->GetRewards());
}

void OpenJarMgr::ComRandJarItem(UInt32 type, std::string condCt)
{
	GetOwner()->ClearRewardGroup();

	//获得奖励道具池
	BonusVec pool;
	GetBonusPool(type, condCt, pool);

	//普通抽奖
	HitProc(pool);

	SyncPoolToDB(pool);
}

bool OpenJarMgr::IsActivityJar(JarType type)
{
	if (type == JT_WING || type == JT_FASHION || type == JT_EQUIP)
	{
		return true;
	}

	return false;
}

void OpenJarMgr::OnOnline()
{
	RegistGameEvent();

	if (GetOwner()->GetLastGoldJarPtRstTime() == 0)
	{
		GetOwner()->SetLastGoldJarPtRstTime((UInt32)CURRENT_TIME.Sec());
	}
	if (GetOwner()->GetLastMagJarPtRstTime() == 0)
	{
		GetOwner()->SetLastMagJarPtRstTime((UInt32)CURRENT_TIME.Sec());
	}

	ResetPoint(true);

	InitMagJarCounter();

	SendArtifactJarDataToGlobal();
}

void OpenJarMgr::OnOffline()
{
	UnRegistGameEvent();
}

void OpenJarMgr::RegistGameEvent()
{
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_NOTIFY_NEWITEM, this, &OpenJarMgr::OnGetNewItemEvent);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_NOTIFY_UPDATEITEM, this, &OpenJarMgr::OnUpdateItemEvent);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_LEVEL_UP, this, &OpenJarMgr::OnLevelUp);
	
	
}

void OpenJarMgr::UnRegistGameEvent()
{
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_NOTIFY_NEWITEM, this, &OpenJarMgr::OnGetNewItemEvent);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_NOTIFY_UPDATEITEM, this, &OpenJarMgr::OnUpdateItemEvent);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), OpenJarMgr, ET_LEVEL_UP, this, &OpenJarMgr::OnLevelUp);
}

UInt32 OpenJarMgr::CheckFreeCD(JarBonusDataEntry* data)
{
	if (!data)	return 0;
	if (0 == data->maxFreeNum || 0 == data->freeCd)	return 0;

	std::string freeNumCounterName = data->freeNumCounterKey;
	{
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(freeNumCounterName);
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(freeNumCounterName.c_str(), 0);
		}
	}
	
	std::string nextFreeTimeCounterName = data->nextFreeTimeCounterKey;
	{
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(nextFreeTimeCounterName);
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(nextFreeTimeCounterName.c_str(), 0);
		}
	}


	UInt32 freeNum = GetOwner()->GetCount(freeNumCounterName.c_str());
	UInt32 nextFreeTime = GetOwner()->GetCount(nextFreeTimeCounterName.c_str());

	UInt32 deltaTime = 0;
	if ((UInt32)CURRENT_TIME.Sec() >= nextFreeTime)
	{
		deltaTime = (UInt32)CURRENT_TIME.Sec() - nextFreeTime;
	}
	else
	{
		return freeNum;
	}

	UInt32 overLayNum = deltaTime / data->freeCd + 1;

	UInt32 totNum = freeNum + overLayNum;
	totNum = (totNum > data->maxFreeNum) ? data->maxFreeNum : totNum;
	
	if (totNum == data->maxFreeNum)
	{
		GetOwner()->SetCounter(nextFreeTimeCounterName.c_str(), (UInt32)CURRENT_TIME.Sec() + data->freeCd);
	}
	else
	{
		//计算下一次cd到得时间
		nextFreeTime = nextFreeTime + overLayNum * data->freeCd;
		GetOwner()->SetCounter(nextFreeTimeCounterName.c_str(), nextFreeTime);
	}
	GetOwner()->SetCounter(freeNumCounterName.c_str(), totNum);

	return totNum;
}

void OpenJarMgr::OpenSpecifyItemRecord(UInt32 jarId, std::string jarName, JarType jarType)
{
	//std::vector<UInt32> announceItemIds;

	for (size_t i = 0; i < m_hitJarItems.size(); ++i)
	{
		UInt32 jarItemId = m_hitJarItems[i];
		
		JarItemPoolData* jarPoolData = JarItemPoolMgr::Instance()->FindEntry(jarItemId);
		if (!jarPoolData)
		{
			continue;
		}

		if (jarPoolData->needAnouce == 0)
		{
			continue;
		}

		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(jarPoolData->itemId);
		if (!data)	continue;
		
		//record & announce it
		CLProtocol::WorldAddJarRecordReq req;
		req.playerUId = GetOwner()->GetID();
		req.jarId = jarId;
		req.name = GetOwner()->GetName();
		req.itemId = data->id;
		req.num = jarPoolData->num;
		req.jarName = jarName;
		req.isActive = IsActivityJar(jarType);
		req.isEqReco = IsEquipRewardJar(jarType);
		Router::SendToWorld(req);

		//announceItemIds.push_back(data->id);
		DebugLogStream << " record jarItem id:" << data->id << " dataId:" << jarPoolData->itemId << " name:" << data->name << LogStream::eos;
	}

// 	for (size_t j = 0; j < m_hitItems.size(); ++j)
// 	{
// 		Item* item = m_hitItems[j];
// 		if (!item)	continue;
// 
// 		for (size_t i = 0; i < announceItemIds.size(); ++i)
// 		{
// 			if (announceItemIds[i] == item->GetDataID())
// 			{
// 				
// 				break;
// 			}
// 		}
// 	}
	
}

void OpenJarMgr::RandJarItem(JarBonusDataEntry* data, bool isMagicBox)
{
	//获得奖励道具池
	BonusVec pool;
	GetBonusPool(data, pool);

	const CounterConfig* counterConfigFix = CounterCfg::Instance()->GetCycleConfig(data->counterKey);
	if (!counterConfigFix)
	{
		CounterCfg::Instance()->RegCounter(data->counterKey.c_str(), 0);
	}

	//抽奖次数累计奖励
	GetOwner()->IncCounter(data->counterKey.c_str(), 1);

	UInt32 openJarCt = GetOwner()->GetCounter(data->counterKey.c_str());
	//DebugLogStream << PLAYERINFO(GetOwner()) << "open_jar_count=" << openJarCt << LogStream::eos;

	UInt32 ct = 0;
	if (isMagicBox)
	{
		if (openJarCt % data->comboBuyNum != 0)
		{
			ct = ceil((float)openJarCt / (float)data->comboBuyNum);
		}
		else
		{
			ct = openJarCt / data->comboBuyNum;
		}
	}
	else
	{
		ct = openJarCt;
	}

	//先处理特定次数必掉掉落
	for (UInt32 i = 0; i < data->countBonus.size(); ++i)
	{
		if (data->countBonus[i].count == 0)
			continue;

		if (ct % data->countBonus[i].count == 0)
		{
			//DebugLogStream << PLAYERINFO(GetOwner()) << "reach ex jar count, count=" << data->countBonus[i].count << LogStream::eos;

			UInt32 exJarId = data->countBonus[i].jarId;
			JarBonusDataEntry* exJarData = JarBonusDataMgr::Instance()->FindEntry(exJarId);
			if (!exJarData)
			{
				continue;
			}

			BonusVec exPool;
			GetBonusPool(exJarData, exPool);
			HitProc(exPool);
			SyncPoolToDB(exPool);

			return;
		}
	}

	//礼包罐子
	if (data->type == JT_GIFT && data->jarGiftMustOutNum > 0 && openJarCt == data->jarGiftMustOutNum)
	{
		const CounterConfig* counterKey = CounterCfg::Instance()->GetCycleConfig(data->curCycleIsOutTreasKey);
		if (!counterKey)
		{
			CounterCfg::Instance()->RegCounter(data->curCycleIsOutTreasKey.c_str(), 0);
		}

		//前面是否抽出过珍惜物品
		if (GetOwner()->GetCounter(data->curCycleIsOutTreasKey.c_str()) == 0)
		{
			UInt32 exJarId = data->jarGiftOutJarId;
			JarBonusDataEntry* exJarData = JarBonusDataMgr::Instance()->FindEntry(exJarId);
			if (exJarData)
			{
				BonusVec exPool;
				GetBonusPool(exJarData, exPool);
				HitProc(exPool);
				SyncPoolToDB(exPool);
				return;
			}
		}	
	}

	//普通抽奖
	JarItem* jarItem = HitProc(pool);

	//礼包罐子
	if (jarItem && data->type == JT_GIFT && data->jarGiftMustOutNum > 0 && openJarCt < data->jarGiftMustOutNum)
	{
		const CounterConfig* counterKey = CounterCfg::Instance()->GetCycleConfig(data->curCycleIsOutTreasKey);
		if (!counterKey)
		{
			CounterCfg::Instance()->RegCounter(data->curCycleIsOutTreasKey.c_str(), 0);
		}

		if (jarItem->GetData()->isTreasItem == 1
			&& GetOwner()->GetCounter(data->curCycleIsOutTreasKey.c_str()) == 0)
		{		
			GetOwner()->SetCounter(data->curCycleIsOutTreasKey.c_str(), 1);
		}
		
	}

	SyncPoolToDB(pool);
}

JarItem* OpenJarMgr::HitProc(BonusVec& pool)
{
	RewardGroup* grp = GetOwner()->GetRewardGroupNoClear();

	FakeWeightVec fakePool;
	GetFakeWeightBase(pool, fakePool);

	FakeWeightBase* hiter = WeightRander::RandWeight(fakePool);
	if (!hiter)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "jaritem randweight err hiter is null!" << LogStream::eos;
		return NULL;
	}

	JarItem* hitJarItem = (JarItem*)(hiter->GetOwner());
	hitJarItem->IncGetNum();

	//获取随机强化等级
	UInt8 strength = GetRandStrength(hitJarItem->GetData());
	grp->AddReward(hitJarItem->GetData()->itemId, hitJarItem->GetData()->num, 0, strength);

	m_hitJarItems.push_back(hitJarItem->GetData()->id);

	//DebugLogStream << PLAYERINFO(GetOwner()) << "get jaritem! id = " << hitJarItem->GetData()->itemId << LogStream::eos;

	return hitJarItem;
}

void OpenJarMgr::CreateMap(JarItem* jarItem)
{
	ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(jarItem->GetData()->itemId);
	if (!itemdata)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "add jaritem itemdata is null! itemid = " << jarItem->GetData()->itemId << LogStream::eos;
		CL_SAFE_DELETE(jarItem);
		return;
	}

	JarBonusColorKey clKey(jarItem->GetData()->type, itemdata->color);

	//建立品质二级索引
	BonusVec& datas1 = bonusPoolColor[clKey];
	datas1.push_back(jarItem);

	//建立key to data一级索引
	BonusVec& datas2 = bonusPool[jarItem->GetData()->type];
	datas2.push_back(jarItem);

	//建立data集合
	occuToDatas.push_back(jarItem);
	
}

void OpenJarMgr::DeleteMap(JarItem* jarItem)
{
	if (!jarItem)
	{
		return;
	}

	ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(jarItem->GetData()->itemId);
	if (!itemdata)
	{
		return;
	}

	JarBonusColorKey clKey(jarItem->GetData()->type, itemdata->color);

	auto itr1 = bonusPoolColor.find(clKey);
	if (itr1 != bonusPoolColor.end())
	{
		BonusVec& datas = itr1->second;
		for (BonusVec::iterator vecIter = datas.begin();
			vecIter != datas.end(); ++vecIter)
		{
			if (*vecIter == jarItem)
			{
				datas.erase(vecIter);
				break;
			}
		}
	}

	auto itr2 = bonusPool.find(jarItem->GetData()->type);
	if (itr2 != bonusPool.end())
	{
		BonusVec& datas = itr2->second;
		for (BonusVec::iterator vecIter = datas.begin();
			vecIter != datas.end(); ++vecIter)
		{
			if (*vecIter == jarItem)
			{
				datas.erase(vecIter);
				break;
			}
		}
	}

	for (BonusVec::iterator vecIter = occuToDatas.begin();
		vecIter != occuToDatas.end(); ++vecIter)
	{
		if (*vecIter == jarItem)
		{
			occuToDatas.erase(vecIter);
			break;
		}
	}

	CL_SAFE_DELETE(jarItem);
}

void OpenJarMgr::DeletAllMap()
{
	BonusVec::iterator itr = occuToDatas.begin();
	for (; itr != occuToDatas.end(); ++itr)
	{
		JarItem* jarItem = *itr;
		CL_SAFE_DELETE(jarItem);
	}
	occuToDatas.clear();
	bonusPoolColor.clear();
	bonusPool.clear();
}

void OpenJarMgr::AddJarItem(JarItem* jarItem, UInt8 color)
{
	
}

bool OpenJarMgr::GetBonusPool(UInt32 key, JarBonusDataEntry* data, BonusVec& datas)
{
	BonusPoolMap::iterator itr = bonusPool.find(key);
	if (itr == bonusPool.end())
	{
		return false;
	}
	else
	{
		RejectJarItem(data, itr->second, datas);
		return true;
	}
}

bool OpenJarMgr::GetBonusPool(JarBonusColorKey key, JarBonusDataEntry* data, BonusVec& datas)
{
	BonusPoolColorMap::iterator itr = bonusPoolColor.find(key);
	if (itr == bonusPoolColor.end())
	{
		return false;
	}
	else
	{
		RejectJarItem(data, itr->second, datas);
		return true;
	}
}

bool OpenJarMgr::GetBonusPool(UInt32 key, std::string cond, BonusVec& datas)
{
	BonusPoolMap::iterator itr = bonusPool.find(key);
	if (itr == bonusPool.end())
	{
		return false;
	}
	else
	{
		RejectJarItem(cond, itr->second, datas);
		return true;
	}
}

bool OpenJarMgr::GetBonusPool(JarBonusDataEntry* data, BonusVec& datas)
{
	if (!data)
	{
		return false;
	}

	JarItemPoolMgr::BonusVec tableDatas;
	JarItemPoolMgr::Instance()->GetTypeAndOccuToDatas(data->id, GetOwner()->GetOccu(), tableDatas);

	for (size_t i = 0; i < tableDatas.size(); ++i)
	{
		bool find = false;
		for (size_t j = 0; j < occuToDatas.size(); ++j)
		{
			if (tableDatas[i]->id == occuToDatas[j]->GetData()->id)
			{
				find = true;
				break;
			}
		}

		if (!find)
		{
			JarItem* jarItem = new JarItem();
			jarItem->SetID(CLRecordClient::Instance()->GenGuid());
			jarItem->SetData(tableDatas[i]);
			jarItem->SetDBState(SDS_INSERT);
			jarItem->SetOwner(GetOwner()->GetID());
			CreateMap(jarItem);
		}
	}

	auto findIter = bonusPool.find(data->id);
	if (findIter == bonusPool.end())
	{
		return false;
	}

	RejectJarItem(data, findIter->second, datas);
	return true;
}

void OpenJarMgr::RejectJarItem(JarBonusDataEntry* data, BonusVec& inDatas, BonusVec& outDatas)
{
	for (size_t i = 0; i < inDatas.size(); ++i)
	{
		JarItem* jarItem = inDatas[i];
		if (!jarItem)
			continue;

		//限量
		if (!jarItem->HasRestNum())
			continue;

		//开罐次数
		if (jarItem->GetData()->openCond > 0)
		{
			const CounterConfig* counterConfigFix = CounterCfg::Instance()->GetCycleConfig(data->counterKey.c_str());
			if (!counterConfigFix)
			{
				CounterCfg::Instance()->RegCounter(data->counterKey.c_str(), 0);
			}
			UInt32 openJarCt = GetOwner()->GetCounter(data->counterKey.c_str());
			if (openJarCt < jarItem->GetData()->openCond)
				continue;
		}
		
		//充值,角色充值量
		if (jarItem->GetData()->chargeCond > GetOwner()->GetTotChargeNum())
		{
			continue;
		}
		//...

		//全服稀有控制

		outDatas.push_back(jarItem);
	}
}


void OpenJarMgr::RejectJarItem(std::string cond, BonusVec& inDatas, BonusVec& outDatas)
{
	for (size_t i = 0; i < inDatas.size(); ++i)
	{
		JarItem* jarItem = inDatas[i];
		if (!jarItem)
			continue;

		//限量
		if (!jarItem->HasRestNum())
			continue;

		//开罐次数
		if (jarItem->GetData()->openCond > 0)
		{
			UInt32 openJarCt = GetOwner()->GetCounter(cond.c_str());
			if (openJarCt < jarItem->GetData()->openCond)
				continue;
		}

		//充值
		//...

		//全服稀有控制

		outDatas.push_back(jarItem);
	}
}

class ReqRareItemCallback : public AsyncMsgCallback
{
public:
	ReqRareItemCallback(OpenJarMgr* mgr, OpenJarMgr::BonusVec& oldPool, JarBonusDataEntry* data) :
		AsyncMsgCallback(new CLProtocol::SceneRareItemRes())
	{
		m_data = data;
		m_mgr = mgr;
		m_oldPool = oldPool;
	}

	virtual void OnSuccess(Avalon::Protocol* msg)
	{
		CLProtocol::SceneRareItemRes* res = (CLProtocol::SceneRareItemRes*)msg;
		for (size_t i = 0; i < res->items.size(); ++i)
		{
			if (res->items[i].num == 0)
			{
				continue;
			}

			for (size_t j = 0; j < m_oldPool.size(); ++j)
			{
				if (m_oldPool[j]->GetItemId() == res->items[i].id)
				{
					m_newPool.push_back(m_oldPool[j]);
				}
			}
		}
	}

	virtual void OnFailed(MsgCallbackError errorCode){}

	OpenJarMgr* m_mgr;
	JarBonusDataEntry* m_data;
	OpenJarMgr::BonusVec m_newPool;
	OpenJarMgr::BonusVec m_oldPool;
};

void OpenJarMgr::RareDropItemReq(BonusVec& inDatas)
{
// 	CLProtocol::SceneRareItemReq req;
// 	req.roleId = GetOwner()->GetID();
// 
// 	for (size_t i = 0; i < inDatas.size(); ++i)
// 	{
// 		ItemReward reward;
// 		reward.id = inDatas[i]->GetItemId();
// 		reward.num = inDatas[i]->GetData()->num;
// 		req.items.push_back(reward);
// 	}
// 
// 	ReqRareItemCallback* cb = new ReqRareItemCallback(inDatas);
// 	AsyncMsgCallbackMgr::Instance()->Add(cb);
// 
// 	SEND_ASYNC_MSG(Router::SendToWorld, cb->GetRequestPacket(req));
}

UInt32 OpenJarMgr::CheckOpenJar(JarBonusDataEntry* data, UInt32 comboBuyNum, UInt32& cost, OpActivityRegInfo* opAct)
{
	//判断每日金钱开罐次数是否满足
	if (data->dayBuyLimite > 0 && data->dayBuyLimite <= GetOwner()->GetCounter(data->countDayLimite.c_str()))
	{
		return ITEM_OPEN_JAR_DAYCOUNT;
	}

	float discount = 1.0;
	UInt32 discountPrice = 0;
	UInt32 originalNum = 0;
	UInt32 discountNum = 0;

	if (opAct == NULL)
	{
		//活动罐子判断折扣
		if (IsActivityJar((JarType)data->type))
		{
			std::ostringstream os;
			os << ACTJAR_DIS_NUM_PRE << data->id;
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
				discountNum = data->discountMaxNum;
			}
			else
			{
				discountNum = GetOwner()->GetCount(os.str().c_str());
			}
		

			if (discountNum > 0)
			{
				discount = ((float)data->singleBuyDiscount / 100);
			}

			if (comboBuyNum > discountNum)
			{
				originalNum = comboBuyNum - discountNum;
			}
			else
			{
				discountNum = comboBuyNum;
			}
		}
		else
		{
			//非折扣罐子
			discount = ((float)data->singleBuyDiscount / 100);
			discountNum = comboBuyNum;
		}

		discountPrice = (UInt32)(discount * (float)data->moneyValue);
		cost = data->moneyValue * originalNum + discountNum * discountPrice;

		// 连抽带上连抽折扣
		if (comboBuyNum > 1)
		{
			cost = (UInt32)(cost * (float)data->comboBuyDiscount / 100);
		}
	}
	else
	{
		cost = data->moneyValue * comboBuyNum;

		// 连抽带上连抽折扣
		if (comboBuyNum > 1)
		{
			cost = (UInt32)(cost * (float)data->comboBuyDiscount / 100);
		}

		// 运营活动打折
		InfoLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " discount jar, combo buy num(" << comboBuyNum << "), cost money(" << cost << ")." << LogStream::eos;
		GetOwner()->GetActivityAttributeMgr().OnOpenJar(opAct, comboBuyNum, cost);
	}

	//判断钱
	ItemDataEntry* moneydata = ItemDataEntryMgr::Instance()->FindEntry(data->moneyType);
	if (!moneydata)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "moneydata is null! key = " << data->moneyType << LogStream::eos;
		GetOwner()->SendNotify("moneydata is null! key = {0}", data->moneyType);
		return ITEM_DATA_INVALID;
	}

	if (!GetOwner()->CheckRmMoney(moneydata, cost))
	{
		if (moneydata->subType == ST_ITEM_GOLD || moneydata->subType == ST_BIND_GOLD)
			return ITEM_NOT_ENOUGH_GOLD;
		if (moneydata->subType == ST_ITEM_POINT || moneydata->subType == ST_BIND_POINT)
			return ITEM_NOT_ENOUGH_POINT;
		else
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "moneydata subtype is err! key = " << data->moneyType << LogStream::eos;
			GetOwner()->SendNotify("moneydata subtype is err! key = {0}", data->moneyType);
			return ITEM_DATA_INVALID;
		}
	}

	return SUCCESS;
}

void OpenJarMgr::GetOccuDatas(BonusVec& datas)
{
	datas = occuToDatas;
}

void OpenJarMgr::SyncPoolToDB(BonusVec& pool)
{
	for (size_t i = 0; i < pool.size(); ++i)
	{
		if (!pool[i]->SyncDB(GetOwner()))
		{
			DeleteMap(pool[i]);
		}
	}
}

void OpenJarMgr::GetFakeWeightBase(BonusVec& pool, FakeWeightVec& fwPool)
{
	for (size_t i = 0; i < pool.size(); ++i)
	{
		fwPool.push_back(pool[i]->GetFakeWeight());
	}
}

void OpenJarMgr::OnGetPoint(std::string reason, JarBonusDataEntry* data, UInt32 comboNum, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit)
{
	if (!data)
	{
		return;
	}

	if (!IsGoldJarPointOpen() && ((JarType)data->type == JT_GOLD))
	{
		return;
	}

	if (!IsMagicJarPointOpen() && ((JarType)data->type == JT_MAGIC))
	{
		return;
	}

	getPointId = GetPointId(data);
	getPoint = GetPointNum(data) * comboNum;
	getPointCrit = GetPointCrit(data);

	UInt32 addPoint = getPoint * getPointCrit;
	//DebugLogStream << " GetOpenJarPoint id:" << getPointId << " BasePoint:" << getPoint << " CritPoint:" << addPoint - getPoint << " TotlePoint:" << addPoint << LogStream::eos;
	if (addPoint > 0)
	{
		GetOwner()->AddItem(reason.c_str(), getPointId, addPoint, 0, 0, false);
	}
}

UInt32 OpenJarMgr::GetPointId(JarBonusDataEntry* data)
{
	return data->getPointId;
}

UInt32 OpenJarMgr::GetPointNum(JarBonusDataEntry* data)
{
	return data->getPointNum;
}

UInt32 OpenJarMgr::GetPointCrit(JarBonusDataEntry* data)
{
	Int32 idx = WeightRander::RandWeight(data->getPointCritRate);
	if (idx >= (Int32)data->getPointCritTime.size() || idx < 0)
	{
		DebugLogStream << " GetPointCrit error! idx:" << idx << LogStream::eos;
		return 0;
	}

	return data->getPointCritTime[idx];
}

bool OpenJarMgr::IsResetPoint(bool isGold)
{
	UInt32 resetDay = isGold ? SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLDJARPOINT_RESET_DAY) : SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAGJARPOINT_RESET_DAY);
	UInt32 intervalType = isGold ? SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLDJARPOINT_RESET_INTERVAL_TYPE) : SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAGJARPOINT_RESET_INTERVAL_TYPE);

	UInt32 endTimeSec = 0;

	UInt64 lastTimeMSec = 0;
	if (isGold)
	{
		lastTimeMSec = (UInt64)GetOwner()->GetLastGoldJarPtRstTime() * 1000;
	}
	else
	{
		lastTimeMSec = (UInt64)GetOwner()->GetLastMagJarPtRstTime() * 1000;
	}

	Avalon::Time lastTime(lastTimeMSec);
	Avalon::Date lastDate(lastTime);

	if (intervalType == 1)
	{
		//月
		endTimeSec = (UInt32)lastTime.Sec() + (UInt32)lastDate.GetDaysInMonth() * DAY_TO_SEC;
		UInt64 endTimeMSec = (UInt64)endTimeSec * 1000;
		Avalon::Time endTime(endTimeMSec);
		Avalon::Date endTimeDate(endTime);

		endTimeDate.Day(resetDay);
		endTimeDate.Hour(0);
		endTimeDate.Min(0);
		endTimeDate.Sec(0);

		return CURRENT_TIME.Sec() >= endTimeDate.ToTime().Sec();
	}
	else if (intervalType == 2)
	{
		//周
		endTimeSec = (UInt32)lastTime.Sec() + 7 * DAY_TO_SEC + (resetDay - lastDate.WDay()) * DAY_TO_SEC;
	
		return CURRENT_TIME.Sec() >= endTimeSec;
	}
	else
	{
		return false;
	}
}

void OpenJarMgr::OnFirstOpenMagJar()
{

}

UInt8 OpenJarMgr::GetRandStrength(JarItemPoolData* data)
{
	if (!data)
	{
		return 0;
	}

	if (data->strWts.size() == 0)
	{
		return 0;
	}

	std::vector<UInt32> wts;
	for (size_t i = 0; i < data->strWts.size(); ++i)
	{
		wts.push_back(data->strWts[i].weight);
	}

	Int32 idx = WeightRander::RandWeight(wts);
	if (idx < 0 || idx >= (Int32)data->strWts.size())
	{
		return 0;
	}

	return data->strWts[idx].strength;
}

void OpenJarMgr::InitMagJarCounter()
{
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_JAR);
	if (!functionData)
	{
		return;
	}
	
	bool needInit = false;
	if (GetOwner()->GetLevel() >= functionData->endLevel)
	{
		needInit = true;
	}

	if (needInit)
	{
		//魔罐
		JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(801);
		if (data)
		{
			if (GetOwner()->GetCount(data->nextFreeTimeCounterKey.c_str()) == 0)
			{
				GetOwner()->SetCounter(data->nextFreeTimeCounterKey.c_str(), (UInt32)CURRENT_TIME.Sec() + data->freeCd);
				GetOwner()->SetCounter(data->freeNumCounterKey.c_str(), 1);
			}
		}
	}
}

UInt32 OpenJarMgr::CheckOpenMagBox(Item* item, JarBonusDataEntry* data, UInt8 isBatch, UInt32& costBoxNum, UInt32& costKeyNum)
{
	if (!item || !data)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//魔盒
	if (item->GetDataEntry()->subType == ItemSubType::ST_MAGIC_BOX)
	{
		if (isBatch > 0)
		{
			costBoxNum = item->GetNum();
		}
		else
		{
			costBoxNum = 1;
		}

		costKeyNum = data->moneyValue * costBoxNum;

		//有没有对应消耗的魔锤
		if (GetOwner()->GetItemNum(data->moneyType) < costKeyNum)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}

		return ErrorCode::SUCCESS;
	}
	else if (item->GetDataEntry()->subType == ItemSubType::ST_MAGIC_HAMMER)	//魔锤
	{
		if (GetOwner()->GetItemNum(item->GetDataID()) < data->moneyValue)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}

		if (isBatch > 0)
		{
			costKeyNum = item->GetNum() / data->moneyValue * data->moneyValue;
		}
		else
		{
			costKeyNum = data->moneyValue;
		}

		costBoxNum = costKeyNum / data->moneyValue;

		//有没有对应消耗的魔盒
		if (GetOwner()->GetItemNum(data->exItemId) < costBoxNum)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}

		return ErrorCode::SUCCESS;
	}
	else
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
}

bool OpenJarMgr::IsFirstOpenMagBox()
{
	if (GetOwner()->GetCount(FIRST_OPEN_MAGBOX) == 0 &&
		MAGIC_BOX_GUILD_ID == GetOwner()->GetNewBoot())
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

void OpenJarMgr::SendArtifactJarDataToGlobal()
{
	if (!ActivityMgr::Instance()->IsInOpActivityByTmpType(OpActivityType::OAT_JAR_DRAW_LOTTERY, GetOwner()->GetLevel()))
		return;

	CLProtocol::GASArtifactJarLotterySignReq req;

	for (UInt32 i = 0; i < ARTIFACT_JAR_ID.size(); ++i)
	{
		if (GetOwner()->GetCount(std::to_string(ARTIFACT_JAR_ID[i]).c_str()) < GetActifactJarLotteryNeedCnt(ARTIFACT_JAR_ID[i]))
			continue;

		req.jarIdVec.push_back(ARTIFACT_JAR_ID[i]);
	}

	if (!req.jarIdVec.empty())
		GetOwner()->SendToGlobalActivity(req);
}

void OpenJarMgr::OpenActifactJarData()
{
	CLProtocol::SceneArtifactJarBuyCntRes res;

	for (UInt32 i = 0; i < ARTIFACT_JAR_ID.size(); ++i)
	{
		std::string jarId_str(ARTIFACT_JAR_BUY);
		jarId_str.append(std::to_string(ARTIFACT_JAR_ID[i]));

		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarId_str);
		if (NULL == counterConfig)
		{
			CounterCfg::Instance()->RegCounter(jarId_str.c_str(), COUNTER_CYCLE_DAY);
		}

		CLProtocol::ArtifactJarBuy artifactJar;
		artifactJar.jarId = ARTIFACT_JAR_ID[i];
		artifactJar.buyCnt = GetOwner()->GetCount(jarId_str.c_str());
		artifactJar.totalCnt = GetActifactJarLotteryNeedCnt(artifactJar.jarId);
		res.artifactJarBuyVec.push_back(artifactJar);
	}

	GetOwner()->SendProtocol(res);
}

UInt32 OpenJarMgr::GetActifactJarLotteryNeedCnt(UInt32 jarId)
{
	UInt32 val = 0;
	switch (jarId)
	{
	case 601: val = SVT_ARTIFACT_JAR_COUNT_20; break;
	case 602: val = SVT_ARTIFACT_JAR_COUNT_30; break;
	case 603: val = SVT_ARTIFACT_JAR_COUNT_40; break;
	case 605: val = SVT_ARTIFACT_JAR_COUNT_50; break;
	case 606: val = SVT_ARTIFACT_JAR_COUNT_60; break;
	default:
		return 10000000;
	}

	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType(val));
}

bool OpenJarMgr::IsActivityArtifactJar(UInt32 jarId)
{
	for (UInt32 i = 0; i < ARTIFACT_JAR_ID.size(); ++i)
	{
		if (ARTIFACT_JAR_ID[i] == jarId)
			return true;
	}

	return false;
}

void OpenJarMgr::ArtifactJarActivity(UInt32 jarId, UInt32 cnt)
{
	if ( !IsActivityArtifactJar(jarId))
		return;

	if ( !ActivityMgr::Instance()->IsInOpActivityByTmpType(OpActivityType::OAT_JAR_DRAW_LOTTERY, GetOwner()->GetLevel()))
		return;

	std::string jarId_str(ARTIFACT_JAR_BUY);
	jarId_str.append(std::to_string(jarId));
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarId_str);
	if (NULL == counterConfig)
	{
		CounterCfg::Instance()->RegCounter(jarId_str.c_str(), COUNTER_CYCLE_DAY);
	}

	GetOwner()->GetCounterMgr().IncCounter(jarId_str.c_str(), cnt);

	// 发到GlobalActivity参加抽奖活动
	UInt32 totalCnt = GetOwner()->GetCounterMgr().GetCounter(jarId_str.c_str());
	if (totalCnt >= GetActifactJarLotteryNeedCnt(jarId))
	{
		CLProtocol::GASArtifactJarLotterySignReq req;
		req.jarIdVec.push_back(jarId);
		GetOwner()->SendToGlobalActivity(req);

		InfoLogStream << "Obtain Artifact Jar Lottery Qualifications! JarId:"<<jarId<<" playerId:"<<GetOwner()->GetID()<<" playerName:" <<GetOwner()->GetName()<<" totalCnt:"<<totalCnt<<" addCnt:"<<cnt<< LogStream::eos;
	}
}

bool OpenJarMgr::IsGoldJarPointOpen()
{
	return SSServiceSwitchMgr::Instance()->IsOpen(GOLD_JAR_POINT);
}

bool OpenJarMgr::IsMagicJarPointOpen()
{
	return SSServiceSwitchMgr::Instance()->IsOpen(MAGIC_JAR_POINT);
}

void OpenJarMgr::SendAnnounce(Item* item, Int32 num, std::string jarName)
{
	if (!item)
	{
		return;
	}

	std::string linkStr;

	std::string playerLinkStr;
	if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
	{
		playerLinkStr = GetOwner()->GetName();
	}

	if (MsgPackage::GetItemMsgTag(linkStr, 0, item->GetDataID(), (UInt32)item->GetStrengthen()))
	{
		GetOwner()->WorldCacheItemData(item);
		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_JAR, 0);
		Sys::SendAnnouncementByStartTime(34, (UInt32)CURRENT_TIME.Sec() + 10, playerLinkStr.c_str(), jarName, linkStr.c_str(), systemLinkStr.c_str());
	}
}

void OpenJarMgr::SendAnnounce(UInt32 itemId, Int32 num, std::string jarName)
{
	std::string linkStr;

	std::string playerLinkStr;
	if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
	{
		playerLinkStr = GetOwner()->GetName();
	}

	if (MsgPackage::GetItemMsgTag(linkStr, 0, itemId, 0))
	{
		//GetOwner()->WorldCacheItemData(item);
		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_TIME_JAR, 0);
		Sys::SendAnnouncementByStartTime(34, (UInt32)CURRENT_TIME.Sec() + 10, playerLinkStr.c_str(), jarName, linkStr.c_str(), systemLinkStr.c_str());
	}
}

void OpenJarMgr::SendUdpLog(JarBonusDataEntry* data, UInt32 combo, ItemRewardVec itemRewards)
{
	if (data == NULL) return;
	if (itemRewards.size() - data->getItems.size() != combo) return;
	for (UInt32 i = 0; i < itemRewards.size(); ++i)
	{
		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemRewards[i].id);
		if (itemData == NULL) continue;
		bool isContinue = false;
		for (UInt32 j = 0; j < data->getItems.size(); ++j)
		{
			if (data->getItems[j].itemId == itemData->id)
			{
				isContinue = true;
				break;
			}
		}
		if (isContinue) continue;
		GetOwner()->SendOpenJarUdpLog(data->id, combo, itemData, itemRewards[i].num);
	}
}

void OpenJarMgr::OnGetNewItemEvent(IEventParam* param)
{
	GetNewItemParam* myParam = (GetNewItemParam*)param;
	if (!myParam)	return;
	if (!IsOpenJar()) return;
	if (!myParam->_newItem)	return;

	// 如果不在身上，就拷贝一份
	Item* item = (Item*)myParam->_newItem;
	if (item->GetPos().pack == PACK_INVALID)
	{
		if (!myParam->openJar)
		{
			return;
		}
		item = new Item(*item);
	}
	m_hitItems.push_back(item);
}

void OpenJarMgr::OnUpdateItemEvent(IEventParam* param)
{
	UpdateItemParam* myParam = (UpdateItemParam*)param;
	if (!myParam)	return;
	if (!IsOpenJar()) return;
	if (!myParam->_updateItem)	return;

	// 如果不在身上，就拷贝一份
	Item* item = (Item*)myParam->_updateItem;
	if (item->GetPos().pack == PACK_INVALID)
	{
		item = new Item(*item);
	}
	m_hitItems.push_back(item);
}

void OpenJarMgr::OnLevelUp(IEventParam* param)
{
	InitMagJarCounter();
}

void OpenJarMgr::ResetPoint(bool online)
{
	if (IsGoldJarPointOpen())
	{
		if ((online || GetOwner()->GetGoldJarPoint() > 0) && IsResetPoint(true))
		{
			std::string reason = GetReason(SOURCE_GOLDJAR_RESET_POINT);
			GetOwner()->RemoveGoldJarPoint(reason.c_str(), GetOwner()->GetGoldJarPoint());
			GetOwner()->SetLastGoldJarPtRstTime((UInt32)CURRENT_TIME.Sec());
			GetOwner()->SyncProperty();
		
			//发邮件
			std::string strSenderName;
			std::string	strTitle;
			std::string	strContent;
			GetOwner()->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);
			Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), NULL, MAIL_VALID_DAY);
		}
	}

	if (IsMagicJarPointOpen())
	{
		if ((online || GetOwner()->GetMagJarPoint() > 0) && IsResetPoint(false))
		{
			std::string reason = GetReason(SOURCE_MAGJAR_RESET_POINT);
			GetOwner()->RemoveMagJarPoint(reason.c_str(), GetOwner()->GetMagJarPoint());
			GetOwner()->SetLastMagJarPtRstTime((UInt32)CURRENT_TIME.Sec());
			GetOwner()->SyncProperty();

			//发邮件
			std::string strSenderName;
			std::string	strTitle;
			std::string	strContent;
			GetOwner()->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);
			Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), NULL, MAIL_VALID_DAY);

			//通知客户端
			CLProtocol::SCUpdateNotifyList update;
			update.notify.type = NT_MAGJAR_RESET_POINT;
			GetOwner()->SendProtocol(update);
		}
	}
}

void OpenJarMgr::GetOpenResult(std::vector<Item*>& results, std::vector<OpenJarResult>& jarResult)
{
	results = m_hitItems;

	std::vector<Item*>::iterator itr = m_hitItems.begin();
	for (size_t i = 0; i < m_hitJarItems.size(); ++i)
	{
		OpenJarResult result;
		
		UInt32 jarItemId = m_hitJarItems[i];

		JarItemPoolData* jarPoolData = JarItemPoolMgr::Instance()->FindEntry(jarItemId);
		if (!jarPoolData)
		{
			continue;
		}

		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(jarPoolData->itemId);
		if (!itemData)
		{
			continue;
		}

		result._JarItemId = jarPoolData->id;

		//不可叠加
		if (itemData->maxNum == 1)
		{
			if (itr != m_hitItems.end())
			{
				result._ItemUId = (*itr)->GetID();
				++itr;
			}
		}

		jarResult.push_back(result);
	}
}

bool OpenJarMgr::IsActiveJarSaleReset(UInt32 jarId)
{
// 	JarBonusDataEntry* jarData = JarBonusDataMgr::Instance()->FindEntry(jarId);
// 	if (!jarData)
// 	{
// 		DebugLogStream << PLAYERINFO(GetOwner()) << " JarBonusDataEntry is null! id:" << jarId << LogStream::eos;
// 		return false;
// 	}
// 
// 	if (!jarData->discountReset)
// 	{
// 		return false;
// 	}
// 
// 	std::ostringstream os;
// 	os << ACTJAR_DIS_NUM_PRE << jarId;
// 	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
// 	if (!counterConfig)
// 	{
// 		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_WEEK_SPECIFY, 5);
// 		return true;
// 	}
// 	else
// 	{
// 		UInt32 count = GetOwner()->GetCounter(os.str().c_str());
// 		if (0 == count)
// 		{
// 			return true;
// 		}
// 	}
	
	return false;
}

UInt32 OpenJarMgr::OpenMagicBox(UInt64 uid, UInt8 isBatch)
{
	ClearHitItems();

	UInt32 needLevel = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_OPEN_MAGIC_BOX_NEED_LV);
	if (GetOwner()->GetLevel() < needLevel)
	{
		return ErrorCode::ITEM_USE_LEVEL_LIMIT;
	}

	Item* item = GetOwner()->FindItem(uid);
	if (!item)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(item->GetDataEntry()->giftId);
	if (!data)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 costBoxNum = 0;
	UInt32 costKeyNum = 0;
	UInt32 retCode = CheckOpenMagBox(item, data, isBatch, costBoxNum, costKeyNum);
	if (ErrorCode::SUCCESS != retCode)
	{
		return retCode;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_OPEN_MAG_BOX);

	// 扣材料
	if (!GetOwner()->RemoveItem(reason.c_str(), data->moneyType, costKeyNum) ||
		!GetOwner()->RemoveItem(reason.c_str(), data->exItemId, costBoxNum))
	{
		return ErrorCode::ITEM_NOT_ENOUGH_MAT;
	}

	// 首次开魔盒
	if (IsFirstOpenMagBox())
	{
		UInt8 index = 2;
		RewardGroup* grp = GetOwner()->GetRewardGroup();

		//获得基本道具
		for (UInt32 i = 0; i < data->getItems.size(); ++i)
		{
			UInt32 num = data->getItems[i].minNum;
			if (data->getItems[i].minNum != data->getItems[i].maxNum)
			{
				num = Avalon::Random::RandBetween(data->getItems[i].minNum, data->getItems[i].maxNum);
			}

			grp->AddReward(data->getItems[i].itemId, num);
			break;
		}

		
		//给首开道具
		if (data->countBonus.size() < index)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}

		JarCountBonus& exBonus = data->countBonus[index - 1];
		DebugLogStream << PLAYERINFO(GetOwner()) << "reach first magic box exbonus, pool id = " << exBonus.jarId << LogStream::eos;

		JarBonusDataEntry* exJarData = JarBonusDataMgr::Instance()->FindEntry(exBonus.jarId);
		if (!exJarData)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}

		for (UInt32 n = 0; n < exJarData->comboBuyNum; ++n)
		{
			BonusVec exPool;
			GetBonusPool(exJarData, exPool);
			HitProc(exPool);
			SyncPoolToDB(exPool);
		}

		GetOwner()->AddJarRewards(reason.c_str(), grp, false, 1);

		SendUdpLog(data, data->comboBuyNum, grp->GetRewards());

		GetOwner()->IncCounter(FIRST_OPEN_MAGBOX, 1);
	}
	else
	{
		UInt32 baseItemId = 0;
		UInt32 baseItemNum = 0;
		GiveJarRewards(reason, data, data->comboBuyNum, false, baseItemId, baseItemNum);
	}
	
	return ErrorCode::SUCCESS;
}

UInt32 OpenJarMgr::OpenJarGift(UInt32 jarid)
{
	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(jarid);
	if (!data)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, jarid);

	UInt32 baseItemId = 0;
	UInt32 baseItemNum = 0;
	GiveJarRewards(reason, data, data->comboBuyNum, true, baseItemId, baseItemNum);
	
	return ErrorCode::SUCCESS;
}

UInt32 OpenJarMgr::OpenJarGift(UInt32 jarid, RewardGroup& rewardGroup)
{
	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(jarid);
	if (!data)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, jarid);

	UInt32 baseItemId = 0;
	UInt32 baseItemNum = 0;
	GiveJarRewards(reason, data, data->comboBuyNum, false, baseItemId, baseItemNum, rewardGroup);

	return ErrorCode::SUCCESS;
}

bool OpenJarMgr::IsEquipRewardJar(JarType type)
{
	if (type == JT_EQRECO)
	{
		return true;
	}

	return false;
}

void OpenJarMgr::ClearHitItems()
{
	m_hitJarItems.clear();
	for (auto item : m_hitItems)
	{
		if (item->GetPos().pack == PACK_INVALID)
		{
			CL_SAFE_DELETE(item);
		}
	}
	m_hitItems.clear();
}