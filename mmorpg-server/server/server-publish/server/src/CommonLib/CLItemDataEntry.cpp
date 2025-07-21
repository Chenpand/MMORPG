#include "CLItemDataEntry.h"
#include <AvalonRandom.h>
#include <set>

ItemDataEntry::ItemDataEntry()
{
}

ItemDataEntry::~ItemDataEntry()
{
}

bool ItemDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	type = row.ReadUInt8();
	subType = row.ReadUInt32();
	thirdType = row.ReadUInt32();
	
	std::string occustr = row.ReadString();
	std::vector<std::string> occustrs;
	Avalon::StringUtil::Split(occustr, occustrs, "|");
	for (std::vector<std::string>::iterator iter = occustrs.begin();
		iter != occustrs.end(); ++iter)
	{
		Int8 oc = Avalon::StringUtil::ConvertToValue<Int8>(*iter);

		if (oc != 0)
		{
			occuFilter.insert(oc);
		}

		if (0 == oc)
		{
			occu.set();
			break;
		}
		else if (oc < 0)
		{
			oc = abs(oc);
			occu.set(oc);
		}
		else
		{
			occu.set(oc);
		}
	}


	//occu = row.ReadUInt8();
	color = row.ReadUInt8();
	if (0 == color)
		return false;
	color2 = row.ReadInt8();
	needLevel = row.ReadUInt16();
	maxLevel = row.ReadUInt16();
	baseAtkSpeed = row.ReadInt32();
	canUse = row.ReadUInt8();
	canTrade = row.ReadUInt8();
	owner = row.ReadUInt8();
	seal = row.ReadUInt8();
	sealMax = row.ReadUInt32();
	canDecompose = row.ReadUInt8();
	sellItemId = row.ReadUInt32();
	price = row.ReadUInt32();
	cdGroup = row.ReadUInt8();
	cdTime = row.ReadUInt32();
	time = row.ReadUInt32();
	maxNum = row.ReadUInt16();
	if (0 == maxNum)
	{
		ErrorLogStream << "item(" << id << ") maxNum(" << maxNum << ")." << LogStream::eos;
		return false;
	}
	onUseBuff = row.ReadUInt32();
	onGetBuff = row.ReadUInt32();
	canDungeonUse = row.ReadUInt8();
	canPkUse = row.ReadUInt8();

    recommendPrice = row.ReadUInt32();
	suitId = row.ReadUInt32();
	attrId = row.ReadUInt32();

	std::string strMutexBuffs = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(strMutexBuffs, strs, "|");
	for (std::vector<std::string>::iterator iter = strs.begin();
		iter != strs.end(); ++iter)
	{
		UInt32 buffID = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
		mutexBuffIDs.push_back(buffID);
	}
	canAnnounce = row.ReadUInt8();
	useLimiteType = row.ReadUInt32();
	useLimiteValue = row.ReadUInt32();
	isAbandon = row.ReadUInt8();
	giftId = row.ReadUInt32();
	isForbidAuctionCopy = row.ReadUInt8() != 0;

	std::string renewInfoStr = row.ReadString();
	std::vector<std::string> infoStrs;
	Avalon::StringUtil::Split(renewInfoStr, infoStrs, "|");
	ItemRenewInfo info;
	for (size_t i = 0; i < infoStrs.size(); ++i)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(infoStrs[i], pram, ",");

		if (pram.size() != 3)
			return false;

		info.durationDay = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
		info.moneyId = Avalon::StringUtil::ConvertToValue<UInt32>(pram[1].c_str());
		info.moneyNum = Avalon::StringUtil::ConvertToValue<UInt32>(pram[2].c_str());

		itemRenewInfos.push_back(info);
	}

	auctionMinPriceRate = row.ReadUInt32();
	auctionMaxPriceRate = row.ReadUInt32();
	canMasterGive = row.ReadUInt8();
	getLimitNum = row.ReadUInt32();

	jarGiftConsumeItemId = 0;
	jarGiftConsumeItemNum = 0;
	std::string strJarGiftConsumes = row.ReadString();
	std::vector<std::string> JarGiftConsStrs;
	Avalon::StringUtil::Split(strJarGiftConsumes, JarGiftConsStrs, "|");
	if (subType == ST_CONSUME_JAR_GIFT && JarGiftConsStrs.size() != 2)
	{
		ErrorLogStream << "itemid: " << id << " jarGiftConsumeItem is err!" << LogStream::eos;
		return false;
	}
	
	if (JarGiftConsStrs.size() == 2)
	{
		jarGiftConsumeItemId = Avalon::StringUtil::ConvertToValue<UInt32>(JarGiftConsStrs[0]);
		jarGiftConsumeItemNum = Avalon::StringUtil::ConvertToValue<UInt32>(JarGiftConsStrs[1]);
	}

	isTransparentFashion = row.ReadUInt8();

	std::string PreciousBeadHole1Str1 = row.ReadString();
	if (PreciousBeadHole1Str1.size() > 0)
	{
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(PreciousBeadHole1Str1, ssv, "|");
		for (auto ss : ssv)
		{
			std::vector<std::string> sv;
			Avalon::StringUtil::Split(ss, sv, ",");
			if (sv.size() == 2)
			{
				ProWeightItem weightItem;
				weightItem.value = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
				weightItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
				PreciousBeadHole1.push_back(weightItem);
			}
		}
	}

	std::string PreciousBeadHole1Str2 = row.ReadString();
	if (PreciousBeadHole1Str2.size() > 0)
	{
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(PreciousBeadHole1Str2, ssv, "|");
		for (auto ss : ssv)
		{
			std::vector<std::string> sv;
			Avalon::StringUtil::Split(ss, sv, ",");
			if (sv.size() == 2)
			{
				ProWeightItem weightItem;
				weightItem.value = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
				weightItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
				PreciousBeadHole2.push_back(weightItem);
			}
		}
	}

	strenTicketDataIndex = row.ReadUInt32();

	precBeadLev = row.ReadInt8();
	precBeadType = row.ReadInt8();

	isTreasure = row.ReadInt8();
	auctionTransCoolTimeMl = row.ReadInt32();
	auctionTransCoolTimeOther = row.ReadInt32();

	strTicketLvLimitMin = 0;
	strTicketLvLimitMax = 0;
	std::string strTicketLevelLimitStr = row.ReadString();
	std::vector<std::string> strTicketLevels;
	Avalon::StringUtil::Split(strTicketLevelLimitStr, strTicketLevels, "|");
	if (strTicketLevels.size() == 2)
	{
		strTicketLvLimitMin = Avalon::StringUtil::ConvertToValue<UInt32>(strTicketLevels[0]);
		strTicketLvLimitMax = Avalon::StringUtil::ConvertToValue<UInt32>(strTicketLevels[1]);
	}

	isRecordLog = (row.ReadUInt32() != 0);

	discountGiftCouponPro = row.ReadInt32();
	
	newTitleId = row.ReadInt32();

	expireTime = row.ReadUInt32();

	auctionTransNum = row.ReadInt32();

	std::string prohibitOpsStr = row.ReadString();
	std::vector<std::string> prohibitOpsv;
	Avalon::StringUtil::Split(prohibitOpsStr, prohibitOpsv, "|");
	for (auto prohibitOp : prohibitOpsv)
	{
		UInt8 op = Avalon::StringUtil::ConvertToValue<UInt8>(prohibitOp);
		prohibitOps.set(op);
	}
	
	useLinmit = row.ReadInt32();

	isSpeicMagic = row.ReadUInt8();

	std::istringstream stream2(row.ReadString());
	char split = 0;
	while (stream2)
	{
		ItemReward item;
		item.DecodeString(stream2);
		if (item.id == 0) break;
		presentedItems.push_back(item);
		stream2 >> split;
	}

	params = row.ReadString();
	sendedLimit = 0;
	sendLimit = 0;
	sendedTotleLimit = 0;
	if (subType == ST_FRIEND_PRESENT)
	{
		std::vector<std::string> paramv;
		Avalon::StringUtil::Split(params, paramv, "|");
		if (paramv.size() < 3)
		{
			ErrorLogStream << "itemid: " << id << " params is err!" << LogStream::eos;
			return false;
		}

		sendedLimit = Avalon::StringUtil::ConvertToValue<UInt8>(paramv[0]);
		sendLimit = Avalon::StringUtil::ConvertToValue<UInt8>(paramv[1]);
		sendedTotleLimit = Avalon::StringUtil::ConvertToValue<UInt8>(paramv[2]);
	}

	return true;
}

bool ItemDataEntry::IsMeetOccuRequirement(Occupation playerOccu) const
{
	if (playerOccu >= Occupation::OCCU_MAX)
	{
		return false;
	}

	// 空的就说明所有职业都可以使用
	if (occuFilter.empty())
	{
		return true;
	}

	// 如果有-1，说明只有转职后能使用
	if (occuFilter.find(-1) != occuFilter.end())
	{
		if (playerOccu == JobDataEntry::GetBaseOccu(playerOccu))
		{
			return false;
		}

		return true;
	}

	return occuFilter.find(playerOccu) != occuFilter.end();
}


ItemDataEntryMgr::ItemDataEntryMgr()
{
}

ItemDataEntryMgr::~ItemDataEntryMgr()
{
}

bool ItemDataEntryMgr::AddEntry(ItemDataEntry* dataEntry)
{
	if(!Avalon::DataEntryMgrBase<ItemDataEntry>::AddEntry(dataEntry)) return false;

	if (dataEntry->useLimiteType == ItemUseLimiteType::IULT_DAYLIMITE ||
		dataEntry->useLimiteType == ItemUseLimiteType::IULT_VIPLIMITE)
	{
		m_useCountItem.push_back(dataEntry->id);
	}

	if (dataEntry->useLimiteType == ItemUseLimiteType::IULT_TEAMCOPY)
	{
		m_TeamCopyItem.push_back(dataEntry->id);
	}

	if (dataEntry->useLimiteType == ItemUseLimiteType::IULT_WEEK6LIMITE)
	{
		m_useCountItemWeek6.push_back(dataEntry->id);
	}

// 	if(dataEntry->type == ITEM_EQUIP 
// 		&& dataEntry->subtype < EQUIP_TYPE_WEAPON_MAX
// 		&& dataEntry->needoccu < 5)
// 	{
// 		m_Equips[dataEntry->subtype][dataEntry->needoccu].insert(std::make_pair(dataEntry->needlevel, dataEntry->id));
// 		m_mapEquips[dataEntry->needoccu][dataEntry->needlevel].push_back(dataEntry->id);
// 	}
// 
// 	if(dataEntry->type == ITEM_PET
// 		&& dataEntry->subtype == PETITEM_EGG)
// 	{
// 		m_PetEggMap.insert(std::make_pair(dataEntry->effectParam[0], dataEntry->id));
// 	}

	if (dataEntry->subType == ST_COUPON && (dataEntry->strTicketLvLimitMin == 0 && dataEntry->strTicketLvLimitMax == 0))
	{
		m_StrenTicketMap[dataEntry->strenTicketDataIndex].push_back(dataEntry);
	}

	if (dataEntry->isRecordLog)
	{
		m_SendUdpItems.push_back(dataEntry);
	}

	if (dataEntry->useLinmit > 0)
	{
		m_flyHellGiftItem.push_back(dataEntry->id);
	}

	if (dataEntry->subType == ST_MAGICCARD)
	{
		auto itr = m_magicIdsByQuality.find(dataEntry->color);
		if (itr == m_magicIdsByQuality.end())
		{
			std::vector<UInt32> magicIds;
			magicIds.push_back(dataEntry->id);
			m_magicIdsByQuality[dataEntry->color] = magicIds;
		}
		else
		{
			itr->second.push_back(dataEntry->id);
		}
	}

	return true;
}

bool ItemDataEntryMgr::CheckCanAddGuildStorage(ItemDataEntry* dataEntry, bool isSealState)
{
	if (!dataEntry)
	{
		return false;
	}

	// 品质要小于等于紫色
	if (dataEntry->color > ITEM_QUALITY_PURPLE)
	{
		return false;
	}

	// 非绑定
	if (dataEntry->owner != 1 && !isSealState)
	{
		return false;
	}

	// 只有装备,附魔卡,材料和消耗品
	if (dataEntry->type != ITEM_EQUIP &&
		dataEntry->subType != ST_MAGICCARD &&
		dataEntry->type != ITEM_MATERIAL &&
		dataEntry->type != ITEM_EXPENDABLE)
	{
		return false;
	}

	return true;
}

bool ItemDataEntryMgr::GetItemDataByStrenTicketIndex(UInt32 index, ItemDataEntryVec& itemDatas)
{
	auto itr = m_StrenTicketMap.find(index);
	if (itr == m_StrenTicketMap.end())
	{
		return false;
	}

	itemDatas = itr->second;

	return true;
}

void ItemDataEntryMgr::GetSendUdpItems(ItemDataEntryVec& itemDatas)
{
	itemDatas = m_SendUdpItems;
}

// UInt32 ItemDataEntryMgr::GetNextEquip(UInt8 equipType, UInt8 occu, UInt16 level)
// {
// 	if(equipType >= EQUIP_TYPE_WEAPON_MAX) return 0;
// 	if(occu >= 5) return 0;
// 
// 	LevelEquipMap::iterator iter = m_Equips[equipType][occu].find(level);
// 	if(iter == m_Equips[equipType][occu].end() || ++iter == m_Equips[equipType][occu].end()) return 0;
// 
// 	return iter->second;
// }

// UInt32 ItemDataEntryMgr::GetSuitableEquip(UInt8 equipType, UInt8 occu, UInt16 level)
// {
// 	if(equipType >= EQUIP_TYPE_WEAPON_MAX) return 0;
// 	if(occu >= 5) return 0;
// 
// 	UInt32 equipId = 0;
// 	LevelEquipMap& levelEquips = m_Equips[equipType][occu];
// 	for(LevelEquipMap::iterator iter = levelEquips.begin();
// 		iter != levelEquips.end(); ++iter)
// 	{
// 		if(iter->first > level) break;
// 		
// 		equipId = iter->second;
// 	}
// 	return equipId;
// }

// UInt32 ItemDataEntryMgr::SelectRandEquip(UInt8 occu, UInt16 level)
// {
// 	if(occu >= 5)
// 		return 0;
// 
// 	std::map<UInt16, std::vector<UInt32> >::iterator	iter = m_mapEquips[occu].find(level);
// 	if(iter == m_mapEquips[occu].end())
// 		return 0;
// 
// 	std::random_shuffle(iter->second.begin(), iter->second.end());
// 
// 	return iter->second.front();
// }

// UInt32 ItemDataEntryMgr::GetPetEggId(UInt32 petId)
// {
// 	std::map<UInt32,UInt32>::iterator iter = m_PetEggMap.find(petId);
// 	if(iter != m_PetEggMap.end()) return iter->second;
// 	return 0;
// }


RandAttrDataEntry::RandAttrDataEntry()
{
}

RandAttrDataEntry::~RandAttrDataEntry()
{
}

bool RandAttrDataEntry::Read(Avalon::DataRow& row)
{
	type = (ItemRandProp)row.ReadUInt32();
	weight = row.ReadUInt32();

	if (0 == type || 0 == weight)
		return false;

	return true;
}

RandAttrNumDataEntry::RandAttrNumDataEntry()
{

}

RandAttrNumDataEntry::~RandAttrNumDataEntry()
{

}

bool RandAttrNumDataEntry::Read(Avalon::DataRow& row)
{
	color = (ItemQuality)row.ReadUInt32();

	std::string numstr = row.ReadString();
	std::vector<std::string> numstrs;
	Avalon::StringUtil::Split(numstr, numstrs, "|");
	for (std::vector<std::string>::iterator iter = numstrs.begin();
	iter != numstrs.end(); ++iter)
	{
		nums.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	std::string wtstr = row.ReadString();
	std::vector<std::string> wtstrs;
	Avalon::StringUtil::Split(wtstr, wtstrs, "|");
	for (std::vector<std::string>::iterator iter = wtstrs.begin();
	iter != wtstrs.end(); ++iter)
	{
		weights.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	if (0 == color || nums.size() == 0 || weights.size() == 0 || nums.size() != weights.size())
		return false;

	return true;
}

RandAttrValueDataEntry::RandAttrValueDataEntry()
{

}

RandAttrValueDataEntry::~RandAttrValueDataEntry()
{

}

bool RandAttrValueDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	color = (ItemQuality)row.ReadUInt32();
	lvMin = row.ReadUInt32();
	lvMax = row.ReadUInt32();

	std::string numstr = row.ReadString();
	std::vector<std::string> numstrs;
	Avalon::StringUtil::Split(numstr, numstrs, "|");
	for (std::vector<std::string>::iterator iter = numstrs.begin();
	iter != numstrs.end(); ++iter)
	{
		values.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	std::string wtstr = row.ReadString();
	std::vector<std::string> wtstrs;
	Avalon::StringUtil::Split(wtstr, wtstrs, "|");
	for (std::vector<std::string>::iterator iter = wtstrs.begin();
	iter != wtstrs.end(); ++iter)
	{
		weights.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	if (0 == id || 0 == type || 0 == color || 0 == lvMin || 0 == lvMax || values.size() == 0 ||
		weights.size() == 0 || values.size() != weights.size())
	{
		return false;
	}

	return true;
}

RandAttrValueDataEntryMgr::RandAttrValueDataEntryMgr()
{
	m_randAttrs.clear();
}

RandAttrValueDataEntryMgr::~RandAttrValueDataEntryMgr()
{

}

bool RandAttrValueDataEntryMgr::AddEntry(RandAttrValueDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<RandAttrValueDataEntry>::AddEntry(dataEntry)) return false;

	RandAttrValMap::iterator itr = m_randAttrs.find(dataEntry->type);
	if (itr == m_randAttrs.end())
	{
		std::vector<RandAttrValueDataEntry*> datas;
		datas.push_back(dataEntry);
		m_randAttrs.insert(std::make_pair(dataEntry->type, datas));
	}
	else
	{
		std::vector<RandAttrValueDataEntry*>& rDatas = itr->second;
		rDatas.push_back(dataEntry);
	}
	
	return true;
}

void RandAttrValueDataEntryMgr::GetDatas(UInt32 type, std::vector<RandAttrValueDataEntry*>& datas)
{
	RandAttrValMap::iterator itr = m_randAttrs.find(type);
	datas = itr->second;
}

bool RandAttrDataEntryMgr::AddEntry(RandAttrDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<RandAttrDataEntry>::AddEntry(dataEntry)) return false;
	m_types.push_back(dataEntry->type);
	m_wts.push_back(dataEntry->weight);
	return true;
}

RandAttrDataEntryMgr::RandAttrDataEntryMgr()
{
	m_types.clear();
	m_wts.clear();
}

RandAttrDataEntryMgr::~RandAttrDataEntryMgr()
{

}

RandBaseDataEntry::RandBaseDataEntry()
{

}

RandBaseDataEntry::~RandBaseDataEntry()
{

}

bool RandBaseDataEntry::Read(Avalon::DataRow& row)
{
	baseRatio = row.ReadUInt8();

	std::string strline = row.ReadString();
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(strline, strVec, "|");
	for (std::vector<std::string>::iterator iter = strVec.begin();
	iter != strVec.end(); ++iter)
	{
		QLPs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	strline = row.ReadString();
	strVec.clear();
	Avalon::StringUtil::Split(strline, strVec, "|");
	for (std::vector<std::string>::iterator iter = strVec.begin();
	iter != strVec.end(); ++iter)
	{
		QLOMin.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

	strline = row.ReadString();
	strVec.clear();
	Avalon::StringUtil::Split(strline, strVec, "|");
	for (std::vector<std::string>::iterator iter = strVec.begin();
	iter != strVec.end(); ++iter)
	{
		QLOMax.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}

// 	if (QLPs.size() >= EQL_MAX)
// 		return false;

	if (QLPs.size() == 0 || QLOMin.size() == 0 || QLOMax.size() == 0 ||
		(QLPs.size() != QLOMin.size() || QLOMin.size() != QLOMax.size()))
		return false;

	return true;
}

RandBaseDataEntryMgr::RandBaseDataEntryMgr()
{
	m_randBaseData = NULL;
}

RandBaseDataEntryMgr::~RandBaseDataEntryMgr()
{

}

bool RandBaseDataEntryMgr::AddEntry(RandBaseDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<RandBaseDataEntry>::AddEntry(dataEntry)) return false;
	m_randBaseData = dataEntry;
	return true;
}

DeComposeDataEntry::DeComposeDataEntry()
{
	quality = 0;
	minLv = 0;
	maxLv = 0;
	colorMats.clear();
	colorLessMats.clear();
	dogEyeNums.clear();
	colorMatId = 0;
	colorLessMatId = 0;
	dogEyeId = 0;
	pinkItemNum.clear();
	pinkItemId = 0;
	redItemNum.clear();
	redItemId = 0;
	resistMagicTotalRate = 0;
	pinkItemTotalRate = 0;
	redItemTotalRate = 0;
}

DeComposeDataEntry::~DeComposeDataEntry()
{

}

bool DeComposeDataEntry::Read(Avalon::DataRow& row)
{
	quality = row.ReadUInt8();
	color2 = row.ReadInt8();

	std::string lvStr = row.ReadString();
	if (lvStr == "")
		return false;

	std::vector<std::string> lvStrs;
	Avalon::StringUtil::Split(lvStr, lvStrs, "|");
	if (lvStrs.size() > 2 || lvStrs.size() == 0)
		return false;

	if (lvStrs.size() == 2)
	{
		minLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[0].c_str());
		maxLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[1].c_str());
	}
	else if (lvStrs.size() == 1)
	{
		maxLv = minLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[0].c_str());
	}

	//有色材料
	ReadDecMat(row, 1);
	colorMatId = row.ReadUInt32();
	//无色材料
	ReadDecMat(row, 2);
	colorLessMatId = row.ReadUInt32();
	// 狗眼数量
	ReadDecMat(row, 3);
	dogEyeId = row.ReadUInt32();
	// 异界数量
	ReadRateMat(row, 1);
	resistMagicMatId = row.ReadUInt32();
	//特殊
	ReadRateMat(row, 2);
	pinkItemId = row.ReadUInt32();
	//气息
	ReadRateMat(row, 3);
	redItemId = row.ReadUInt32();
	return true;
}

void DeComposeDataEntry::ReadDecMat(Avalon::DataRow& row, UInt8 matType)
{
	std::string matNum = row.ReadString();

	if (matNum == "")
		return;

	std::vector<std::string> matNums;
	Avalon::StringUtil::Split(matNum, matNums, "|");

	for (std::vector<std::string>::iterator iter = matNums.begin();
	iter != matNums.end(); ++iter)
	{
		if (1 == matType)
		{
			colorMats.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
		}
		else if (2 == matType)
		{
			colorLessMats.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
		}
		else if (3 == matType)
		{
			dogEyeNums.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
		}
	}
}

void DeComposeDataEntry::ReadRateMat(Avalon::DataRow& row, UInt8 matType)      // matType:1.异界 2.特殊 3.气息
{
	std::string str = row.ReadString();
	if (str == "")
		return;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");

	if (strs.size() == 0)
		return;

	decltype(resistMagicTotalRate) *totalRate = nullptr;
	decltype(resistMagicMatNums) *vec = nullptr;
	if (1 == matType)
	{
		totalRate = &resistMagicTotalRate;
		vec = &resistMagicMatNums;
	}
	else if (2 == matType)
	{
		totalRate = &pinkItemTotalRate;
		vec = &pinkItemNum;
	}
	else if (3 == matType)
	{
		totalRate = &redItemTotalRate;
		vec = &redItemNum;
	}

	if (totalRate == nullptr || vec == nullptr)
	{
		return;
	}

	*totalRate = 0;

	for (size_t j = 0; j < strs.size(); ++j)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(strs[j], pram, "_");

		if (pram.size() != 2)
			return;

		ItemRateNumPair pair;
		pair.first = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
		pair.second = Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str());

		vec->push_back(pair);
		*totalRate += pair.first;
	}
}


DeComposeDataEntryMgr::DeComposeDataEntryMgr()
{
	m_decomposeDatas.clear();
}

DeComposeDataEntryMgr::~DeComposeDataEntryMgr()
{

}

bool DeComposeDataEntryMgr::AddEntry(DeComposeDataEntry* dataEntry)
{
	//if (!Avalon::DataEntryMgrBase<DeComposeDataEntry>::AddEntry(dataEntry)) return false;
	QualityKey key(dataEntry->GetKey(), dataEntry->GetColor2());
	DeComposeDataMap::iterator itr = m_decomposeDatas.find(key);
	if (itr == m_decomposeDatas.end())
	{
		std::vector<DeComposeDataEntry*> datas;
		datas.push_back(dataEntry);
		m_decomposeDatas.insert(std::make_pair(key, datas));
	}
	else
	{
		std::vector<DeComposeDataEntry*>& rDatas = itr->second;
		rDatas.push_back(dataEntry);
	}

	return true;
}

void DeComposeDataEntryMgr::GetDatas(UInt8 quality, UInt8 color2, std::vector<DeComposeDataEntry*>& datas)
{
	QualityKey key(quality, color2);
	DeComposeDataMap::iterator itr = m_decomposeDatas.find(key);
	if (itr != m_decomposeDatas.end())
	{
		datas = itr->second;
	}
}

EquipStrRateData::EquipStrRateData()
{
	strengthen = 0;
	sucRate = 0;
	failPunishment = 0;
	fix1 = 0;
	fix1Max = 0;
	fix2 = 0;
	fix2Max = 0;
	fix3 = 0;
}

EquipStrRateData::~EquipStrRateData()
{

}

bool EquipStrRateData::Read(Avalon::DataRow& row)
{
	type = row.ReadUInt8();
	strengthen = row.ReadUInt8();
	sucRate = row.ReadUInt32();
	failPunishment = row.ReadUInt8();
	fix1 = row.ReadUInt32();
	fix1Max = row.ReadUInt32();
	fix2 = row.ReadUInt32();
	fix2Max = row.ReadUInt32();
	if (!ReadCondition(row))
		return false;
	fix3 = row.ReadUInt32();
	tickAddNum = row.ReadUInt32();

	return true;
}

bool EquipStrRateData::ReadCondition(Avalon::DataRow& row)
{
	std::string str = row.ReadString();
	if (str == "")
		return false;

	std::vector<std::string> splits;
	Avalon::StringUtil::Split(str, splits, "|");
	if (splits.size() != ITEM_QUALITY_MAX-2)
	{
		return false;
	}

	for (size_t i = 0; i < splits.size(); ++i)
	{
		fix3Condition[i] = Avalon::StringUtil::ConvertToValue<UInt32>(splits[i].c_str());
	}

	return true;
}

bool EquipStrRateDataMgr::AddEntry(EquipStrRateData* dataEntry)
{
	if (dataEntry->type == 1)
	{
		if (!Avalon::DataEntryMgrBase<EquipStrRateData>::AddEntry(dataEntry)) return false;
		m_equipStrRateMap[dataEntry->strengthen] = dataEntry;
	}
	else if (dataEntry->type == 2)
	{
		m_equipEnhanceRateMap[dataEntry->strengthen] = dataEntry;
	}
	
	return true;
}

EquipStrRateData* EquipStrRateDataMgr::GetDatas(UInt8 strengthen)
{
	return m_equipStrRateMap[strengthen];
}

EquipStrRateData* EquipStrRateDataMgr::GetEnhanceDatas(UInt8 enhance)
{
	return m_equipEnhanceRateMap[enhance];
}

EquipStrRateDataMgr::EquipStrRateDataMgr()
{
	m_equipStrRateMap.clear();
	m_equipEnhanceRateMap.clear();
}

EquipStrRateDataMgr::~EquipStrRateDataMgr()
{

}

EquipStrengthenData::EquipStrengthenData()
{
	strengthen = 0;
	minLv = 0;
	maxLv = 0;
}

EquipStrengthenData::~EquipStrengthenData()
{

}

bool EquipStrengthenData::Read(Avalon::DataRow& row)
{
	strengthen = row.ReadUInt8();

	std::string lvStr = row.ReadString();
	if (lvStr == "")
		return false;

	std::vector<std::string> lvStrs;
	Avalon::StringUtil::Split(lvStr, lvStrs, "|");
	if (2 != lvStrs.size() && 1 != lvStrs.size())
		return false;

	if (lvStrs.size() == 2)
	{
		minLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[0].c_str());
		maxLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[1].c_str());
	}
	else if (lvStrs.size() == 1)
	{
		maxLv = minLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStrs[0].c_str());
	}


	for (int i = ITEM_QUALITY_WHITE; i < (int)ITEM_QUALITY_MAX; ++i)
	{
		cost[i].goldCost = row.ReadUInt32();
		ret[i].goldRet = row.ReadUInt32();
		if (!ReadMatNum(row, cost[i].colorCost, cost[i].colorLessCost))	return false;
		if (!ReadMatNum(row, ret[i].colorRet, ret[i].colorLessRet))	return false;
		ret[i].strToTenDropRate = row.ReadUInt32();
		ret[i].strToTenNum = row.ReadUInt32();

		std::string str = row.ReadString();
		if (str == "")
			return false;

		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");

		if (strs.size() == 0)
			return false;

		ret[i].totalRate = 0;

		for (size_t j = 0; j < strs.size(); ++j)
		{
			std::vector<std::string> pram;
			Avalon::StringUtil::Split(strs[j], pram, "_");

			if (pram.size() != 2)
				return false;

			StrenthRet::ItemNumPair pair;
			pair.first = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
			pair.second = Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str());

			ret[i].ItemNumVec.push_back(pair);
			ret[i].totalRate += pair.first;
		}
	}

	return true;
}

bool EquipStrengthenData::ReadMatNum(Avalon::DataRow& row, UInt32& colorMat, UInt32& colorLessMat)
{
	std::string matNum = row.ReadString();

	if (matNum == "")
		return false;

	std::vector<std::string> matNums;
	Avalon::StringUtil::Split(matNum, matNums, "|");

	if (matNums.size() == 1)
	{
		colorLessMat = Avalon::StringUtil::ConvertToValue<UInt32>(matNums[0]);
		colorMat = 0;
		return true;
	}
	else if (matNums.size() == 2)
	{
		colorLessMat = Avalon::StringUtil::ConvertToValue<UInt32>(matNums[0]);
		colorMat = Avalon::StringUtil::ConvertToValue<UInt32>(matNums[1]);
		return true;
	}
	
	return false;
}

EquipStrengthenDataMgr::EquipStrengthenDataMgr()
{
	m_equipStrMap.clear();
}

EquipStrengthenDataMgr::~EquipStrengthenDataMgr()
{

}

bool EquipStrengthenDataMgr::AddEntry(EquipStrengthenData* dataEntry)
{
	//if (!Avalon::DataEntryMgrBase<EquipStrengthenData>::AddEntry(dataEntry)) return false;

	EquipStrMap::iterator itr = m_equipStrMap.find(dataEntry->strengthen);
	if (itr == m_equipStrMap.end())
	{
		std::vector<EquipStrengthenData*> datas;
		datas.push_back(dataEntry);
		m_equipStrMap.insert(std::make_pair(dataEntry->strengthen, datas));
	}
	else
	{
		std::vector<EquipStrengthenData*>& rDatas = itr->second;
		rDatas.push_back(dataEntry);
	}

	return true;
}

void EquipStrengthenDataMgr::GetDatas(UInt8 strengthen, std::vector<EquipStrengthenData*>& datas)
{
	EquipStrMap::iterator itr = m_equipStrMap.find(strengthen);
	if (itr != m_equipStrMap.end())
	{
		datas = itr->second;
	}
}



bool EquipEnhanceCostDataEntry::Read(Avalon::DataRow &row)
{ 
	key.quality = row.ReadUInt8();
	key.enhanceLevel = row.ReadUInt8();
	key.level = row.ReadUInt8();
	needGold = row.ReadUInt32();
	returnGold = row.ReadUInt32();
	itemID = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	returnItemID = row.ReadUInt32();
	returnItemRule = row.ReadString();
	if (returnItemRule == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(returnItemRule, strs, "|");

	if (strs.size() == 0)
		return false;

	returnTotalRate = 0;
	EnhanceReturn info;
	for (size_t i = 0; i < strs.size(); ++i)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(strs[i], pram, "_");

		if (pram.size() != 2)
			return false;

		info.m_Rate = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
		info.m_Num = Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str());

		retrunItem.push_back(info);
		returnTotalRate += info.m_Rate;
	}

	returnItemID2 = row.ReadUInt32();
	returnItemRule2 = row.ReadString();
	if (returnItemRule2 == "")
		return false;

	strs.clear();
	Avalon::StringUtil::Split(returnItemRule2, strs, "|");

	if (strs.size() == 0)
		return false;

	returnTotalRate2 = 0;
	for (size_t i = 0; i < strs.size(); ++i)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(strs[i], pram, "_");

		if (pram.size() != 2)
			return false;

		info.m_Rate = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
		info.m_Num = Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str());

		retrunItem2.push_back(info);
		returnTotalRate2 += info.m_Rate;
	}

	return true;
}

bool EquipEnhanceCostDataEntryMgr::AddEntry(EquipEnhanceCostDataEntry* entry)
{
	return m_EquipEnhanceCostMap.insert(std::make_pair(entry->key, entry)).second;
}

EquipEnhanceCostDataEntry* EquipEnhanceCostDataEntryMgr::FindEntry(UInt8 level, UInt8 enhanceLevel, UInt8 quality)
{
	auto iter = m_EquipEnhanceCostMap.find(EquipEnhanceDataKey{ quality, enhanceLevel, level });
	if (iter != m_EquipEnhanceCostMap.end())
	{
		return iter->second;
	}
	return nullptr;
}

void EquipEnhanceCostDataEntryMgr::ClearDataEntries()
{
	m_EquipEnhanceCostMap.clear();
}

bool EquipEnhanceAttributeDataEntry::Read(Avalon::DataRow &row)
{
	key.quality = row.ReadUInt8();
	key.enhanceLevel = row.ReadUInt8();
	key.level = row.ReadUInt8();
	enhanceAttribute = row.ReadUInt32();
	enhanceAttributePVP = row.ReadUInt32();
	eqScore = row.ReadUInt32();
	return true;
}

bool EquipEnhanceAttributeDataEntryMgr::AddEntry(EquipEnhanceAttributeDataEntry* entry)
{
	return m_EquipEnhanceAttributeMap.insert(std::make_pair(entry->key, entry)).second;
}

EquipEnhanceAttributeDataEntry* EquipEnhanceAttributeDataEntryMgr::FindEntry(UInt8 level, UInt8 enhanceLevel, UInt8 quality)
{
	auto iter = m_EquipEnhanceAttributeMap.find(EquipEnhanceDataKey{ quality, enhanceLevel, level });
	if (iter != m_EquipEnhanceAttributeMap.end())
	{
		return iter->second;
	}
	return nullptr;
}

void EquipEnhanceAttributeDataEntryMgr::ClearDataEntries()
{
	m_EquipEnhanceAttributeMap.clear();
}


MaterialsSynthesisDataEntry::MaterialsSynthesisDataEntry()
{
	composites = "";
	Materials.clear();
}

MaterialsSynthesisDataEntry::~MaterialsSynthesisDataEntry()
{

}

UInt32 MaterialsSynthesisDataEntry::GetKey() const
{
	return id;
}

bool MaterialsSynthesisDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	composites = row.ReadString();

	if (composites == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(composites, strs, "|");

	if (strs.size() == 0)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(strs[i], pram, "_");

		if (pram.size() != 2)
			return false;
		Materials.emplace_back(Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str()), Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str()));
	}

	return true;
}


bool MaterialsSynthesisDataEntryMgr::AddEntry(MaterialsSynthesisDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MaterialsSynthesisDataEntry>::AddEntry(entry))
	{
		return false;
	}
	return true;
}



MagicCardData::MagicCardData()
{
	id = 0;
}

bool MagicCardData::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	color = row.ReadUInt8();
	quality = row.ReadUInt8();

	std::string tmpParts = row.ReadString();
	if (tmpParts == "")
		return false;

	std::vector<std::string> strParts;
	Avalon::StringUtil::Split(tmpParts, strParts, "|");
	//部位不能超过最大值
	if (strParts.size() > 10)
		return false;

	for (size_t i = 0; i < strParts.size(); ++i)
	{
		UInt8 part = Avalon::StringUtil::ConvertToValue<UInt8>(strParts[i].c_str());
		parts.push_back(part);
	}

	std::string propTypes = row.ReadString();
	
	std::vector<std::string> strPropTypes;
	Avalon::StringUtil::Split(propTypes, strPropTypes, "|");

	std::string propValues = row.ReadString();
	
	std::vector<std::string> strPropValues;
	Avalon::StringUtil::Split(propValues, strPropValues, "|");

	if (strPropValues.size() != strPropTypes.size())
		return false;

	for (size_t i = 0; i < strPropTypes.size(); ++i)
	{
		MagicCardProp prop;
		prop.propType = Avalon::StringUtil::ConvertToValue<UInt8>(strPropTypes[i].c_str());
		prop.propValue = Avalon::StringUtil::ConvertToValue<UInt32>(strPropValues[i].c_str());
		props.push_back(prop);	
	}

	std::string tmpbuffs = row.ReadString();
	
	std::vector<std::string> strBuffs;
	Avalon::StringUtil::Split(tmpbuffs, strBuffs, "|");

	for (size_t i = 0; i < strBuffs.size(); ++i)
	{
		UInt32 buffId = Avalon::StringUtil::ConvertToValue<UInt32>(strBuffs[i].c_str());
		buffs.push_back(buffId);
	}

	if (buffs.size() + props.size() == 0)
		return false;

	maxLevel = row.ReadInt8();
	upValue = row.ReadInt32();
	weight = row.ReadUInt32();
	rate = row.ReadUInt32();
	costId = row.ReadUInt32();
	costNum = row.ReadUInt32();

	/*std::string tmpss = row.ReadString();
	std::vector<std::string> strUpgradeMaterials;
	Avalon::StringUtil::Split(tmpss, strUpgradeMaterials, "|");
	for (size_t i = 0; i < strUpgradeMaterials.size(); ++i)
	{
		std::string s = strUpgradeMaterials[i];
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(s, ss, "_");
		if (ss.size() != 2)
		{
			ErrorLogStream << "id(" << id << "), filed UpgradeMaterials is err!" << LogStream::eos;
			return false;
		}
		MagicUpgradeMaterial mm;
		mm.color = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
		mm.num = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
		upgradeMaterials.push_back(mm);
	}

	std::string baseRatesstr = row.ReadString();
	std::vector<std::string> baseRatesstrv;
	Avalon::StringUtil::Split(baseRatesstr, baseRatesstrv, "|");
	for (size_t i = 0; i < baseRatesstrv.size(); ++i)
	{
		std::string s = baseRatesstrv[i];
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(s, ss, "_");
		if (ss.size() != 2)
		{
			ErrorLogStream << "id(" << id << "), filed baseRates is err!" << LogStream::eos;
			return false;
		}

		UInt8 strengthen = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
		baseRates[strengthen] = rate;
	}*/

	//std::vector<std::string> upmaterials;
	//std::vector<std::string> uprates;
	//upmaterials.push_back(row.ReadString());
	//uprates.push_back(row.ReadString());
	//upmaterials.push_back(row.ReadString());
	//uprates.push_back(row.ReadString());
	//upmaterials.push_back(row.ReadString());
	//uprates.push_back(row.ReadString());

	//for (size_t i = 0; i < upmaterials.size(); ++i)
	//{
	//	UInt8 quality_ = i + 1;
	//	std::string tempUpMaterialStr = upmaterials[i];
	//	std::vector<std::string> strUpgradeMaterials;
	//	Avalon::StringUtil::Split(tempUpMaterialStr, strUpgradeMaterials, "|");
	//	for (size_t j = 0; j < strUpgradeMaterials.size(); ++j)
	//	{
	//		std::string s = strUpgradeMaterials[j];
	//		std::vector<std::string> ss;
	//		Avalon::StringUtil::Split(s, ss, "_");
	//		if (ss.size() != 3)
	//		{
	//			ErrorLogStream << "id(" << id << "), filed UpgradeMaterials is err! quality : " << quality_ << LogStream::eos;
	//			return false;
	//		}
	//		MagicUpgradeMaterial mm;
	//		mm.color = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
	//		mm.quality = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
	//		mm.num = Avalon::StringUtil::ConvertToValue<UInt32>(ss[2].c_str());
	//		upgradeMaterials[quality_].push_back(mm);
	//	}

	//	std::string uprateStr = uprates[i];
	//	std::vector<std::string> baseRatesstrv;
	//	Avalon::StringUtil::Split(uprateStr, baseRatesstrv, "|");
	//	for (size_t z = 0; z < baseRatesstrv.size(); ++z)
	//	{
	//		std::string s = baseRatesstrv[z];
	//		std::vector<std::string> ss;
	//		Avalon::StringUtil::Split(s, ss, "_");
	//		if (ss.size() != 2)
	//		{
	//			ErrorLogStream << "id(" << id << "), filed baseRates is err!" << LogStream::eos;
	//			return false;
	//		}

	//		UInt8 strengthen = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
	//		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
	//		upgradeMaterialsRates[quality_][strengthen] = rate;
	//	}
	//}

	//std::string tmpLevelAddRateStr = row.ReadString();
	//std::vector<std::string> levelAddRateStrs;
	//Avalon::StringUtil::Split(tmpLevelAddRateStr, levelAddRateStrs, "|");
	//for (size_t i = 0; i < levelAddRateStrs.size(); ++i)
	//{
	//	std::string s = levelAddRateStrs[i];
	//	std::vector<std::string> ss;
	//	Avalon::StringUtil::Split(s, ss, "_");
	//	if (ss.size() != 2)
	//	{
	//		ErrorLogStream << "id(" << id << "), filed LevelAddRate is err!" << LogStream::eos;
	//		return false;
	//	}
	//	
	//	UInt8 lev = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
	//	UInt32 addRate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
	//	levelAddRate[lev] = addRate;
	//}

	//std::string tmpSameCardIDStr = row.ReadString();
	//std::vector<std::string> sameCardIDsv;
	//Avalon::StringUtil::Split(tmpSameCardIDStr, sameCardIDsv, "|");
	//for (size_t i = 0; i < sameCardIDsv.size(); ++i)
	//{
	//	std::string s = sameCardIDsv[i];
	//	std::vector<std::string> ss;
	//	Avalon::StringUtil::Split(s, ss, "_");
	//	if (ss.size() != 2)
	//	{
	//		ErrorLogStream << "id(" << id << "), filed SameCardID is err!" << LogStream::eos;
	//		return false;
	//	}
	//	UInt32 sameCardID = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
	//	UInt32 comuseNum = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
	//	SameCardIds t;
	//	t.itemId = sameCardID;
	//	t.comuseNum = comuseNum;
	//	sameCardIds.push_back(t);
	//}

	////sameCardAddRate = row.ReadUInt32();
	//std::string tmpSameCardRateStr = row.ReadString();
	//std::vector<std::string> sameCardRateStrv;
	//Avalon::StringUtil::Split(tmpSameCardRateStr, sameCardRateStrv, "|");
	//for (size_t i = 0; i < sameCardRateStrv.size(); ++i)
	//{
	//	std::string s = sameCardRateStrv[i];
	//	std::vector<std::string> ss;
	//	Avalon::StringUtil::Split(s, ss, "_");
	//	if (ss.size() != 2)
	//	{
	//		ErrorLogStream << "id(" << id << "), filed SameCardAddRate is err!" << LogStream::eos;
	//		return false;
	//	}

	//	UInt8 lev = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
	//	UInt32 addRate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
	//	sameCardRates[lev] = addRate;
	//}

	//std::istringstream stream2(row.ReadString());
	//upgradeConsume.DecodeString(stream2);

	score = row.ReadUInt32();
	upgradeAddScore = row.ReadUInt32();
	return true;
}

MagicCardDataMgr::MagicCardDataMgr()
{

}

MagicCardDataMgr::~MagicCardDataMgr()
{

}

bool MagicCardDataMgr::AddEntry(MagicCardData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<MagicCardData>::AddEntry(dataEntry)) return false;

	auto itr = m_colorToDatas.find(dataEntry->color);
	if (itr == m_colorToDatas.end())
	{
		std::vector<MagicCardData*> datas;
		datas.push_back(dataEntry);
		m_colorToDatas.insert(std::make_pair(dataEntry->color, datas));
	}
	else
	{
		std::vector<MagicCardData*>& rDatas = itr->second;
		rDatas.push_back(dataEntry);
	}

	return true;
}

bool MagicCardDataMgr::GetDatasByColor(UInt8 color, std::vector<MagicCardData*>& datas)
{
	auto itr = m_colorToDatas.find(color);
	if (itr == m_colorToDatas.end())
		return false;

	datas = itr->second;
	return true;
}

bool MagicCardDataMgr::GetDatasByColorAndBind(UInt8 color, std::vector<MagicCardData*>& datas, bool isBind)
{
	auto itr = m_colorToDatas.find(color);
	if (itr == m_colorToDatas.end())
		return false;

	std::vector<MagicCardData*>& cards = itr->second;
	for (size_t i = 0; i < cards.size(); ++i)
	{
		MagicCardData* data = cards[i];
		if (!data)
		{
			continue;
		}
		ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(data->id);
		if (!itemdata)
		{
			continue;
		}
		if (isBind && itemdata->owner != 1)
		{
			datas.push_back(data);
		}
		else if (!isBind && itemdata->owner == 1)
		{
			datas.push_back(data);
		}
		else
		{
			continue;
		}
	}
	return true;
}

MagicCardUpdateData::MagicCardUpdateData()
{
	id = 0;
}

bool MagicCardUpdateData::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}
	id = row.ReadUInt32();
	dataId = row.ReadUInt32();
	minLv = row.ReadUInt8();
	maxLv = row.ReadUInt8();

	std::vector<std::string> upmaterials;
	std::vector<std::string> uprates;
	upmaterials.push_back(row.ReadString());
	uprates.push_back(row.ReadString());
	upmaterials.push_back(row.ReadString());
	uprates.push_back(row.ReadString());
	upmaterials.push_back(row.ReadString());
	uprates.push_back(row.ReadString());

	for (size_t i = 0; i < upmaterials.size(); ++i)
	{
		UInt8 quality_ = i + 1;
		std::string tempUpMaterialStr = upmaterials[i];
		std::vector<std::string> strUpgradeMaterials;
		Avalon::StringUtil::Split(tempUpMaterialStr, strUpgradeMaterials, "|");
		for (size_t j = 0; j < strUpgradeMaterials.size(); ++j)
		{
			std::string s = strUpgradeMaterials[j];
			std::vector<std::string> ss;
			Avalon::StringUtil::Split(s, ss, "_");
			if (ss.size() != 3)
			{
				ErrorLogStream << "id(" << id << "), filed UpgradeMaterials is err! quality : " << quality_ << LogStream::eos;
				return false;
			}
			MagicUpgradeMaterial mm;
			mm.color = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
			mm.quality = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
			mm.num = Avalon::StringUtil::ConvertToValue<UInt32>(ss[2].c_str());
			upgradeMaterials[quality_].push_back(mm);
		}

		std::string uprateStr = uprates[i];
		std::vector<std::string> baseRatesstrv;
		Avalon::StringUtil::Split(uprateStr, baseRatesstrv, "|");
		for (size_t z = 0; z < baseRatesstrv.size(); ++z)
		{
			std::string s = baseRatesstrv[z];
			std::vector<std::string> ss;
			Avalon::StringUtil::Split(s, ss, "_");
			if (ss.size() != 2)
			{
				ErrorLogStream << "id(" << id << "), filed baseRates is err!" << LogStream::eos;
				return false;
			}

			UInt8 strengthen = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
			UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
			upgradeMaterialsRates[quality_][strengthen] = rate;
		}
	}

	std::string tmpLevelAddRateStr = row.ReadString();
	std::vector<std::string> levelAddRateStrs;
	Avalon::StringUtil::Split(tmpLevelAddRateStr, levelAddRateStrs, "|");
	for (size_t i = 0; i < levelAddRateStrs.size(); ++i)
	{
		std::string s = levelAddRateStrs[i];
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(s, ss, "_");
		if (ss.size() != 2)
		{
			ErrorLogStream << "id(" << id << "), filed LevelAddRate is err!" << LogStream::eos;
			return false;
		}

		UInt8 lev = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
		UInt32 addRate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
		levelAddRate[lev] = addRate;
	}

	std::string tmpSameCardIDStr = row.ReadString();
	std::vector<std::string> sameCardIDsv;
	Avalon::StringUtil::Split(tmpSameCardIDStr, sameCardIDsv, "|");
	for (size_t i = 0; i < sameCardIDsv.size(); ++i)
	{
		std::string s = sameCardIDsv[i];
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(s, ss, "_");
		if (ss.size() != 2)
		{
			ErrorLogStream << "id(" << id << "), filed SameCardID is err!" << LogStream::eos;
			return false;
		}
		UInt32 sameCardID = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
		UInt32 comuseNum = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
		SameCardIds t;
		t.itemId = sameCardID;
		t.comuseNum = comuseNum;
		sameCardIds.push_back(t);
	}

	//sameCardAddRate = row.ReadUInt32();
	std::string tmpSameCardRateStr = row.ReadString();
	std::vector<std::string> sameCardRateStrv;
	Avalon::StringUtil::Split(tmpSameCardRateStr, sameCardRateStrv, "|");
	for (size_t i = 0; i < sameCardRateStrv.size(); ++i)
	{
		std::string s = sameCardRateStrv[i];
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(s, ss, "_");
		if (ss.size() != 2)
		{
			ErrorLogStream << "id(" << id << "), filed SameCardAddRate is err!" << LogStream::eos;
			return false;
		}

		UInt8 lev = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0].c_str());
		UInt32 addRate = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1].c_str());
		sameCardRates[lev] = addRate;
	}

	std::istringstream stream2(row.ReadString());
	upgradeConsume.DecodeString(stream2);
	return true;
}

MagicCardUpdateDataMgr::MagicCardUpdateDataMgr()
{
}

MagicCardUpdateDataMgr::~MagicCardUpdateDataMgr()
{
}

MagicCardUpdateData* MagicCardUpdateDataMgr::GetDataEntry(UInt32 cardId, UInt8 cardLv)
{
	class Visitor : public MagicCardUpdateDataVisitor
	{
	public:
		Visitor(UInt32 cardId_, UInt8 cardLv_) 
			: m_cardId(cardId_), m_cardLv(cardLv_), m_dataEntry(NULL)
		{}
		bool operator()(MagicCardUpdateData* dataEntry)
		{
			if (!dataEntry)
			{
				return true;
			}
			if (dataEntry->dataId != m_cardId)
			{
				return true;
			}
			if (m_cardLv < dataEntry->minLv || m_cardLv > dataEntry->maxLv)
			{
				return true;
			}

			m_dataEntry = dataEntry;
			return false;
		}
	public:
		UInt32 m_cardId;
		UInt8 m_cardLv;
		MagicCardUpdateData* m_dataEntry;
	};

	Visitor visitor(cardId, cardLv);
	this->Visit(visitor);
	return visitor.m_dataEntry;
}

PreciousBeadDataEntry::PreciousBeadDataEntry()
{

}

PreciousBeadDataEntry::~PreciousBeadDataEntry()
{

}

bool PreciousBeadDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	itemId = row.ReadUInt32();
	//InfoLogStream << "PreciousBeadDataEntry::Read  itemId:" << itemId << LogStream::eos;
	color = row.ReadUInt8();

	std::string tmpParts = row.ReadString();
	std::vector<std::string> strParts;
	Avalon::StringUtil::Split(tmpParts, strParts, "|");

	//部位不能超过最大值
	if (strParts.size() > 10)
	{
		ErrorLogStream << "Parts(itemid: " << itemId << ") is over!" << LogStream::eos;
		return false;
	}

	for (size_t i = 0; i < strParts.size(); ++i)
	{
		UInt8 part = Avalon::StringUtil::ConvertToValue<UInt8>(strParts[i].c_str());
		parts.push_back(part);
	}

	std::string propTypes = row.ReadString();
	std::vector<std::string> strPropTypes;
	Avalon::StringUtil::Split(propTypes, strPropTypes, "|");

	std::string propValues = row.ReadString();
	std::vector<std::string> strPropValues;
	Avalon::StringUtil::Split(propValues, strPropValues, "|");

	if (strPropValues.size() != strPropTypes.size())
	{
		ErrorLogStream << "Property(itemid: " << itemId << ") type not fit value!" << LogStream::eos;
		return false;
	}

	for (size_t i = 0; i < strPropTypes.size(); ++i)
	{
		PreciousBeadProp prop;
		prop.propType = Avalon::StringUtil::ConvertToValue<UInt8>(strPropTypes[i].c_str());
		prop.propValue = Avalon::StringUtil::ConvertToValue<UInt32>(strPropValues[i].c_str());
		props.push_back(prop);
	}

	std::string tmpbuffs = row.ReadString();
	std::vector<std::string> strBuffs;
	Avalon::StringUtil::Split(tmpbuffs, strBuffs, "|");

	for (size_t i = 0; i < strBuffs.size(); ++i)
	{
		UInt32 buffId = Avalon::StringUtil::ConvertToValue<UInt32>(strBuffs[i].c_str());
		buffsPve.push_back(buffId);
	}

	std::string tmpbuffsPvp = row.ReadString();
	std::vector<std::string> strBuffsPvp;
	Avalon::StringUtil::Split(tmpbuffsPvp, strBuffsPvp, "|");
	for (size_t i = 0; i < strBuffsPvp.size(); ++i)
	{
		UInt32 buffId = Avalon::StringUtil::ConvertToValue<UInt32>(strBuffsPvp[i].c_str());
		buffsPvp.push_back(buffId);
	}

	if (buffsPve.size() + props.size() == 0)
	{
		ErrorLogStream << "Total size of buffs and props(itemid: " << itemId << ") is zero!" << LogStream::eos;
		return false;
	}

	level = row.ReadUInt8();

	nextLevPrecbeadID = row.ReadUInt32();

	std::istringstream stream2(row.ReadString());
	char split = 0;
	while (stream2)
	{
		ItemReward item;
		item.DecodeString(stream2);
		if (item.id == 0) break;
		consumeItem.push_back(item);
		stream2 >> split;
	}
	/*InfoLogStream << "PreciousBeadDataEntry::Read itemId = " << itemId << ", consumeItem size = " << (UInt32)consumeItem.size() << LogStream::eos;
	for (UInt32 i = 0; i < consumeItem.size(); ++i)
	{
		InfoLogStream << "PreciousBeadDataEntry i: " << i << " id = " << consumeItem[i].id << "num = " << consumeItem[i].num << LogStream::eos;
	}*/

	addtionBuffPro = row.ReadUInt32();
	buffRandomGroupId = row.ReadUInt32();

	std::string str_replace = row.ReadString();
	std::vector<std::string> strReplaces;
	Avalon::StringUtil::Split(str_replace, strReplaces, "|");
	for (size_t i = 0; i < strReplaces.size(); ++i)
	{
		UInt32 replace_id = Avalon::StringUtil::ConvertToValue<UInt32>(strReplaces[i].c_str());
		replaceVec.push_back(replace_id);
	}

	score = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strReplaces;
		Avalon::StringUtil::Split(str, strReplaces, "|");
		for (size_t i = 0; i < strReplaces.size(); ++i)
		{
			UInt32 sameID = Avalon::StringUtil::ConvertToValue<UInt32>(strReplaces[i].c_str());
			sameBeadIDVec.push_back(sameID);
		}
	}
	

	return true;
}

bool PreciousBeadDataEntry::IsCanReplace(UInt32 replaceId)
{
	for (UInt32 id : replaceVec)
	{
		if (id == replaceId)
			return true;
	}

	return false;
}

PreciousBeadDataEntryMgr::PreciousBeadDataEntryMgr()
{

}

PreciousBeadDataEntryMgr::~PreciousBeadDataEntryMgr()
{

}

EquipAttrDataEntry::EquipAttrDataEntry()
{
	memset(AtkPropEx, 0, sizeof(AtkPropEx));
	memset(StrPropEx, 0, sizeof(AtkPropEx));
	memset(DefPropEx, 0, sizeof(AtkPropEx));
	AbnormalResistsTotal = 0;
	memset(AbnormalResists, 0, sizeof(AtkPropEx));
	skillScore = 0;
}

EquipAttrDataEntry::~EquipAttrDataEntry()
{

}

bool EquipAttrDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	atk = row.ReadInt32();
	magicAtk = row.ReadInt32();
	independAtk = row.ReadUInt32();
	def = row.ReadInt32();
	magicDef = row.ReadInt32();
	strenth = row.ReadInt32();
	intellect = row.ReadInt32();
	spirit = row.ReadInt32();
	stamina = row.ReadInt32();
	phySkillMp = row.ReadInt8();
	phySkillCd = row.ReadInt32();
	magSkillMp = row.ReadInt8();
	magSkillCd = row.ReadInt32();
	HPMax = row.ReadInt32();
	MPMax = row.ReadInt32();
	HPRecover = row.ReadInt32();;
	MPRecover = row.ReadInt32();
	AtkSpeedRate = row.ReadInt32();
	FireSpeedRate = row.ReadInt32();
	BattleSpeedRate = row.ReadInt32();
	TownSpeedRate = row.ReadInt16();
	HitRate = row.ReadInt32();
	AvoidRate = row.ReadInt32();
	PhysicCrit = row.ReadInt32();
	MagicCrit = row.ReadInt32();
	Spasticity = row.ReadInt32();

	std::string tmpStr = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(tmpStr, strs, "|");
	if (strs.size() >= EEP_MAX)
	{
		return false;
	}

	memset(AtkPropEx, 0, sizeof(AtkPropEx));
	for (std::vector<std::string>::iterator iter = strs.begin();
		iter != strs.end(); ++iter)
	{
		UInt32 exPropType = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
		if (0 == exPropType || exPropType > EEP_MAX)
		{
			return false;
		}

		AtkPropEx[exPropType - 1] = 1;
	}

	for (int i = 0; i < EEP_MAX; ++i)
	{
		StrPropEx[i] = row.ReadInt32();
	}
	
	for (int i = 0; i < EEP_MAX; ++i)
	{
		DefPropEx[i] = row.ReadInt32();
	}

	AbnormalResistsTotal = row.ReadInt32();

	for (int i = 0; i < EAR_MAX; ++i)
	{
		AbnormalResists[i] = row.ReadInt32();
	}
	
	skillScore = row.ReadInt32();

	std::string buffInfoIdsStr = row.ReadString();
	if (buffInfoIdsStr.size() > 0)
	{
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(buffInfoIdsStr, ss, "|");
		for (std::vector<std::string>::iterator iter = ss.begin();
			iter != ss.end(); ++iter)
		{
			UInt32 buffInfoId = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
			attchBuffInfoIds.push_back(buffInfoId);
		}
	}

	DungeonEpicDropAddition = row.ReadInt32();
	return true;
}

EquipAttrDataMgr::EquipAttrDataMgr()
{

}

EquipAttrDataMgr::~EquipAttrDataMgr()
{

}

bool EquipAttrDataMgr::AddEntry(EquipAttrDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<EquipAttrDataEntry>::AddEntry(dataEntry)) return false;

	return true;
}

JarBonusDataEntry::JarBonusDataEntry()
{
	actifactJarRewardNum = 0;
	smellType = 0;
}

JarBonusDataEntry::~JarBonusDataEntry()
{

}

bool JarBonusDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	type = row.ReadUInt32();
	
	std::vector<std::string> first;
	std::vector<std::string> second;

	JarCountBonus info;
	info.count = row.ReadUInt32();
	info.jarId = row.ReadUInt32();
	countBonus.push_back(info);
	info.count = row.ReadUInt32();
	info.jarId = row.ReadUInt32();
	countBonus.push_back(info);

	std::string sGetItems = row.ReadString();
	Avalon::StringUtil::Split(sGetItems, first, "|");
	for (size_t i = 0; i < first.size(); ++i)
	{
		Avalon::StringUtil::Split(first[i], second, ",");
		if (second.size() != 3)
			return false;

		JarBonusInfo info;
		info.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(second[0].c_str());
		info.minNum = Avalon::StringUtil::ConvertToValue<UInt16>(second[1].c_str());
		info.maxNum = Avalon::StringUtil::ConvertToValue<UInt16>(second[2].c_str());
		info.wt = 0;
		getItems.push_back(info);
		
	}

	moneyType = row.ReadUInt32();
	moneyValue = row.ReadUInt32();					
	dayBuyLimite = row.ReadUInt32();		
	countDayLimite = row.ReadString();
	needExItem = row.ReadUInt8();					
	exItemId = row.ReadUInt32();		
	exItemNum = row.ReadUInt32();
	comboBuyNum = row.ReadUInt32();	
	singleBuyDiscount = row.ReadUInt32();
	comboBuyDiscount = row.ReadUInt32();			
	counterKey = row.ReadString();

	getPointId = row.ReadUInt32();
	getPointNum = row.ReadUInt32();

	first.clear();
	second.clear();
	std::string sGetPointCrit = row.ReadString();
	Avalon::StringUtil::Split(sGetPointCrit, first, "|");
	for (size_t i = 0; i < first.size(); ++i)
	{
		Avalon::StringUtil::Split(first[i], second, ",");
		if (second.size() != 2)
			return false;

		
		getPointCritTime.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(second[0].c_str()));
		getPointCritRate.push_back(Avalon::StringUtil::ConvertToValue<UInt16>(second[1].c_str()));
	}

	maxFreeNum = row.ReadUInt32();
	freeCd = row.ReadUInt32();
	freeNumCounterKey = row.ReadString();
	nextFreeTimeCounterKey = row.ReadString();
	needRecord = row.ReadUInt8();
	discountMaxNum = row.ReadUInt32();
	discountReset = row.ReadUInt8();
	actifactJarRewardNum = row.ReadUInt32();

	jarGiftMustOutNum = row.ReadUInt32();
	curCycleIsOutTreasKey = row.ReadString();
	jarGiftOutJarId = row.ReadUInt32();
	smellType = row.ReadUInt32();
	return true;
}

JarBonusDataMgr::JarBonusDataMgr()
{

}

JarBonusDataMgr::~JarBonusDataMgr()
{

}

bool JarBonusDataMgr::AddEntry(JarBonusDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<JarBonusDataEntry>::AddEntry(dataEntry)) return false;

	return true;
}

EquipStrModData::EquipStrModData()
{

}

EquipStrModData::~EquipStrModData()
{

}

bool EquipStrModData::Read(Avalon::DataRow& row)
{
	m_id = row.ReadUInt32();
	if (!ReadSplitStr(row, m_wpStrMods)) return false;
	if (!ReadSplitStr(row, m_wpColorQaMods)) return false;
	if (!ReadSplitStr(row, m_wpColorQbMods)) return false;
	if (!ReadSplitStr(row, m_armStrMods)) return false;
	if (!ReadSplitStr(row, m_armColorQaMods)) return false;
	if (!ReadSplitStr(row, m_armColorQbMods)) return false;
	if (!ReadSplitStr(row, m_jewStrMods)) return false;
	if (!ReadSplitStr(row, m_jewColorQaMods)) return false;
	if (!ReadSplitStr(row, m_jewColorQbMods)) return false;
	
	for (int i = 0; i < TT_WP_TYPE_MAX - 1; ++i)
	{
		if (!ReadEquipPtMods(row, m_equipPtMods)) return false;
	}
	
	return true;
}

bool EquipStrModData::ReadSplitStr(Avalon::DataRow& row, std::vector<UInt32>& vec)
{
	std::string str = row.ReadString();
	if (str == "")
		return false;

	if (str == "-")
	{
		return true;
	}

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() == 0)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		vec.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i].c_str()));
	}
	
	return true;
}

bool EquipStrModData::ReadEquipPtMods(Avalon::DataRow& row, std::vector<EquipPtMod>& mods)
{
	std::string str = row.ReadString();
	if (str == "")
		return false;

	if (str == "-")
	{
		return true;
	}

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() != 2)
		return false;

	EquipPtMod mod;
	mod.phyMod = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0].c_str());
	mod.magMod = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1].c_str());
	mods.push_back(mod);

	return true;
}

float EquipStrModData::GetWpPhyMod(UInt8 subType)
{
	if (subType < TT_ITEM_HUGESWORD || subType > TT_WP_TYPE_MAX)
	{
		//ErrorLogStream << "subType:" << subType << " is err" << LogStream::eos;
		return 0;
	}

	if (subType > m_equipPtMods.size())
	{
		//ErrorLogStream << "subType:" << subType << " is not match to equipPtModSize:" << (UInt32)m_equipPtMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_equipPtMods[subType - 1].phyMod / 100.0f);
}

float EquipStrModData::GetWpMagMod(UInt8 subType)
{
	if (subType < TT_ITEM_HUGESWORD || subType > TT_WP_TYPE_MAX)
	{
		//ErrorLogStream << "subType:" << subType << " is err" << LogStream::eos;
		return 0;
	}

	if (subType > m_equipPtMods.size())
	{
		//ErrorLogStream << "subType:" << subType << " is not match to equipPtModSize:" << (UInt32)m_equipPtMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_equipPtMods[subType - 1].magMod / 100.0f);
}

float EquipStrModData::GetWpClQaMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		//ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_wpColorQaMods.size())
	{
		//ErrorLogStream << "color:" << color << " is not match to WpColorQaModsSize:" << (UInt32)m_wpColorQaMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_wpColorQaMods[color - 1] / 100.0f);
}

float EquipStrModData::GetWpClQbMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		//ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_wpColorQbMods.size())
	{
		//ErrorLogStream << "color:" << color << " is not match to WpColorQbModsSize:" << (UInt32)m_wpColorQbMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_wpColorQbMods[color - 1] / 100.0f);
}

float EquipStrModData::GetWpStrMod(UInt8 strLv)
{
	if (strLv == 0 || strLv > m_wpStrMods.size())
	{
		ErrorLogStream << "strLv:" << strLv << " is err" << LogStream::eos;
		return 0;
	}

	return ((float)m_wpStrMods[strLv - 1] / 100.0f);
}

float EquipStrModData::GetArmStrMod(UInt8 strLv)
{
	if (strLv == 0 || strLv > m_armStrMods.size())
	{
		ErrorLogStream << "strLv:" << strLv << " is err" << LogStream::eos;
		return 0;
	}

	return ((float)m_armStrMods[strLv - 1] / 100.0f);
}

float EquipStrModData::GetArmClQbMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_armColorQbMods.size())
	{
		ErrorLogStream << "color:" << color << " is not match to ArmColorQbModsSize:" << (UInt32)m_armColorQbMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_armColorQbMods[color - 1] / 100.0f);
}

float EquipStrModData::GetJewStrMod(UInt8 strLv)
{
	if (strLv == 0 || strLv > m_jewStrMods.size())
	{
		ErrorLogStream << "strLv:" << strLv << " is err" << LogStream::eos;
		return 0;
	}

	return ((float)m_jewStrMods[strLv - 1] / 100.0f);
}

float EquipStrModData::GetJewClQbMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_jewColorQbMods.size())
	{
		ErrorLogStream << "color:" << color << " is not match to JewColorQbModsSize:" << (UInt32)m_jewColorQbMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_jewColorQbMods[color - 1] / 100.0f);
}

float EquipStrModData::GetArmClQaMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_armColorQaMods.size())
	{
		ErrorLogStream << "color:" << color << " is not match to ArmColorQaModsSize:" << (UInt32)m_armColorQaMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_armColorQaMods[color - 1] / 100.0f);
}

float EquipStrModData::GetJewClQaMod(UInt8 color)
{
	if (color == ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "color:" << color << " is err" << LogStream::eos;
		return 0;
	}

	if (color > m_jewColorQaMods.size())
	{
		ErrorLogStream << "color:" << color << " is not match to JewColorQaModsSize:" << (UInt32)m_jewColorQaMods.size() << "data id:" << m_id << LogStream::eos;
		return 0;
	}

	return ((float)m_jewColorQaMods[color - 1] / 100.0f);
}

EquipStrModDataMgr::EquipStrModDataMgr()
{

}

EquipStrModDataMgr::~EquipStrModDataMgr()
{

}

bool EquipStrModDataMgr::AddEntry(EquipStrModData* dataEntry)
{
	EquipStrModMap::iterator itr = m_equipStrModMap.find(dataEntry->m_id);
	if (itr == m_equipStrModMap.end())
	{
		m_equipStrModMap.insert(std::make_pair(dataEntry->m_id, dataEntry));
	}
	else
	{
		return false;
	}

	return true;
}

EquipStrModData* EquipStrModDataMgr::GetDatas(UInt32 id)
{
	EquipStrModMap::iterator itr = m_equipStrModMap.find(id);
	if (itr != m_equipStrModMap.end())
	{
		return itr->second;
	}

	return NULL;
}

EquipStrModData* EquipStrModDataMgr::GetPVPStrMod()
{
	return GetDatas(2);
}

EquipStrModData* EquipStrModDataMgr::GetPVEStrMod()
{
	return GetDatas(1);
}

EquipStrModData* EquipStrModDataMgr::GetPVPDefPercentMod()
{
	return GetDatas(4);
}

EquipStrModData* EquipStrModDataMgr::GetPVEDefPercentMod()
{
	return GetDatas(3);
}

JarItemPoolData::JarItemPoolData()
{

}

JarItemPoolData::~JarItemPoolData()
{

}

bool JarItemPoolData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	itemId = row.ReadUInt32();
	
	std::string strWtsStr = row.ReadString();
	if (strWtsStr != "")
	{
		std::vector<std::string> strWtInfos;
		Avalon::StringUtil::Split(strWtsStr, strWtInfos, "|");
		for (size_t i = 0; i < strWtInfos.size(); ++i)
		{
			std::vector<std::string> strWtProp;
			Avalon::StringUtil::Split(strWtInfos[i], strWtProp, "_");
			if (strWtProp.size() != 2)
			{
				return false;
			}

			JarItemStrWt wt;
			wt.strength = Avalon::StringUtil::ConvertToValue<UInt8>(strWtProp[0].c_str());
			wt.weight = Avalon::StringUtil::ConvertToValue<UInt32>(strWtProp[1].c_str());
			strWts.push_back(wt);
		}
	}
	
	strOccus = row.ReadString();
	if (strOccus == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(strOccus, strs, "|");

	for (size_t i = 0; i < strs.size(); ++i)
	{
		occus.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(strs[i].c_str()));
	}

	num = row.ReadUInt32();
	wt = row.ReadUInt32();
	fix1 = row.ReadUInt32();
	fix2 = row.ReadUInt32();
	fix2Num = row.ReadUInt32();
	fixMax = row.ReadUInt32();
	fixMin = row.ReadUInt32();
	openCond = row.ReadUInt32();
	chargeCond = row.ReadUInt32();
	getNumLim = row.ReadUInt32();
	needAnouce = row.ReadUInt32();
	isTreasItem = row.ReadUInt8();
	isJarGiftNeedAnount = row.ReadUInt32();
	checkLoad = row.ReadUInt32();
	return true;
}

bool JarItemPoolData::HasOccu(UInt8 occu)
{
	for (size_t i = 0; i < occus.size(); ++i)
	{
		if (occus[i] == occu)
		{
			return true;
		}
	}
	
	return false;
}

JarItemPoolMgr::JarItemPoolMgr()
{

}

JarItemPoolMgr::~JarItemPoolMgr()
{

}

bool JarItemPoolMgr::AddEntry(JarItemPoolData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<JarItemPoolData>::AddEntry(dataEntry)) return false;
		
	JarBonusTrpKey tpKey(dataEntry->type, dataEntry->strOccus, dataEntry->itemId);

	//建立职业&ietmid&开罐类型 组合key索引

	auto itr = poolDatas.find(tpKey);
	if (itr != poolDatas.end())
	{
		ErrorLogStream << "Jar item pool(" << dataEntry->id << ")'s key is same with another(" << itr->second->id << ")!" << LogStream::eos;
		if (dataEntry->checkLoad)
		{
			return false;
		}
	}

	poolDatas[tpKey] = dataEntry;

	//建立职业作为索引的map
	for (size_t i = 0; i < dataEntry->occus.size(); ++i)
	{
		BonusVec& rdatas = occuToDatas[dataEntry->occus[i]];
		rdatas.push_back(dataEntry);
	}
	
	datas.push_back(dataEntry);
	return true;
}

JarItemPoolData* JarItemPoolMgr::GetPoolData(JarBonusTrpKey key)
{
	PoolDataMap::iterator itr = poolDatas.find(key);
	if (itr == poolDatas.end())
	{
		return NULL;
	}
	else
	{
		return itr->second;
	}
}

bool JarItemPoolMgr::GetOccuToDatas(UInt8 key, BonusVec& datas)
{
	PoolOccuMap::iterator itr = occuToDatas.find(key);
	if (itr == occuToDatas.end())
	{
		return false;
	}
	else
	{
		datas = itr->second;
		return true;
	}
}

void JarItemPoolMgr::GetTypeAndOccuToDatas(UInt32 type, UInt8 occu, BonusVec& rdatas)
{
	for (size_t i = 0; i < datas.size(); ++i)
	{
		JarItemPoolData* data = datas[i];
		if (!data)
		{
			continue;
		}

		if (data->type == type && data->HasOccu(occu))
		{
			rdatas.push_back(data);
		}
	}
}

BuffDrugData::BuffDrugData()
{

}

BuffDrugData::~BuffDrugData()
{

}

bool BuffDrugData::Read(Avalon::DataRow& row)
{
	itemId = row.ReadUInt32();
	name = row.ReadString();
	freeLevel = row.ReadUInt32();

	return true;
}

BuffDrugDataMgr::BuffDrugDataMgr()
{

}

BuffDrugDataMgr::~BuffDrugDataMgr()
{

}

StrTicketData::StrTicketData()
{

}

StrTicketData::~StrTicketData()
{

}

bool StrTicketData::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column num is not match!" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	rate = row.ReadUInt32();
	effectLv = row.ReadUInt32();
	canFuse = row.ReadUInt32() > 0;
	fuseValue = row.ReadUInt32();

	if (canFuse)
	{
		ItemDataEntryVec itemDatas;
		if (!ItemDataEntryMgr::Instance()->GetItemDataByStrenTicketIndex(id, itemDatas))
		{
			ErrorLogStream << "Strengthen ticket index(" << id << ") not match item data size!" << LogStream::eos;
			return false;
		}

		if (itemDatas.size() != STREN_TICKET_INDEX_ITEM_DATA_NUM_LIMIT)
		{
			ErrorLogStream << "Strengthen ticket index(" << id << ") not match item data size(" << (UInt32)itemDatas.size() << ")!" << LogStream::eos;
			return false;
		}
		else
		{
			// 宏定义值改动后记得改这里
			if (itemDatas[0]->owner == itemDatas[1]->owner)
			{
				ErrorLogStream << "Item(" << itemDatas[0]->id << ")'s owner is same with item(" << itemDatas[1]->id << ")'s!" << LogStream::eos;
				return false;
			}
		}
	}

	return true;
}

bool StrTicketDataMgr::AddEntry(StrTicketData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<StrTicketData>::AddEntry(dataEntry)) return false;

	if (dataEntry->canFuse)
	{
		if (GetFuseStrengthenTicketData(dataEntry->effectLv, dataEntry->rate) != NULL)
		{
			ErrorLogStream << "Repeat strengthen ticket data(level=" << dataEntry->effectLv << ", rate=" << dataEntry->rate << LogStream::eos;
			return false;
		}

		StrenKey key;
		key.level = dataEntry->effectLv;
		key.rate = dataEntry->rate;

		m_FuseStrTicketMap[key] = dataEntry;
		m_FuseStrTicketVec.push_back(dataEntry);
	}

	return true;
}

StrTicketData* StrTicketDataMgr::GetFuseStrengthenTicketData(UInt32 level, UInt32 rate)
{
	StrenKey key;
	key.level = level;
	key.rate = rate;

	auto itr = m_FuseStrTicketMap.find(key);
	if (itr != m_FuseStrTicketMap.end())
	{
		return itr->second;
	}

	return NULL;
}

void StrTicketDataMgr::SortFuseStrengthenTicketsByFuseValue()
{
	struct CompareHelper
	{
		bool operator()(const StrTicketData* lhs, const StrTicketData* rhs) const
		{
			return lhs->fuseValue > rhs->fuseValue;
		}
	};

	std::sort(m_FuseStrTicketVec.begin(), m_FuseStrTicketVec.end(), CompareHelper());
}

void StrTicketDataMgr::VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F)
{
	for (auto elem : m_FuseStrTicketVec)
	{
		if (elem != NULL)
		{
			if (!F(elem))
			{
				break;
			}
		}
	}
}


bool EnhanceTicketDataMgr::AddEntry(StrTicketData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<StrTicketData>::AddEntry(dataEntry)) return false;

	if (dataEntry->canFuse)
	{
		if (GetFuseStrengthenTicketData(dataEntry->effectLv, dataEntry->rate) != NULL)
		{
			ErrorLogStream << "Repeat strengthen ticket data(level=" << dataEntry->effectLv << ", rate=" << dataEntry->rate << LogStream::eos;
			return false;
		}

		StrenKey key;
		key.level = dataEntry->effectLv;
		key.rate = dataEntry->rate;

		m_FuseStrTicketMap[key] = dataEntry;
		m_FuseStrTicketVec.push_back(dataEntry);
	}

	return true;
}

StrTicketData* EnhanceTicketDataMgr::GetFuseStrengthenTicketData(UInt32 level, UInt32 rate)
{
	StrenKey key;
	key.level = level;
	key.rate = rate;

	auto itr = m_FuseStrTicketMap.find(key);
	if (itr != m_FuseStrTicketMap.end())
	{
		return itr->second;
	}

	return NULL;
}

void EnhanceTicketDataMgr::SortFuseStrengthenTicketsByFuseValue()
{
	struct CompareHelper
	{
		bool operator()(const StrTicketData* lhs, const StrTicketData* rhs) const
		{
			return lhs->fuseValue > rhs->fuseValue;
		}
	};

	std::sort(m_FuseStrTicketVec.begin(), m_FuseStrTicketVec.end(), CompareHelper());
}

void EnhanceTicketDataMgr::VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F)
{
	for (auto elem : m_FuseStrTicketVec)
	{
		if (elem != NULL)
		{
			if (!F(elem))
			{
				break;
			}
		}
	}
}

bool FashionComposeData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	occu = row.ReadUInt8();
	color = row.ReadUInt8();
	part = row.ReadUInt8();
	composeColor = row.ReadUInt8();
	weight = row.ReadUInt32();

	return true;
}

FashionComposeData::FashionComposeData()
{

}

FashionComposeData::~FashionComposeData()
{

}

FashionComposeSkyData::FashionComposeSkyData()
{

}

FashionComposeSkyData::~FashionComposeSkyData()
{

}

bool FashionComposeSkyData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();	
	index = row.ReadUInt32();
	suitId = row.ReadUInt32();
	occu = row.ReadUInt8();						
	part = row.ReadUInt8();	
	type = row.ReadUInt8();
	baseWeight = row.ReadUInt32();			
	std::string randNum = row.ReadString();
	if (randNum == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(randNum, strs, "|");
	if (strs.size() != 2)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		fixRandRange[i] = Avalon::StringUtil::ConvertToValue<UInt8>(strs[i].c_str());
	}

	hitMin = row.ReadUInt32();						
	maxWeight = row.ReadUInt32();					
	hitWeight = row.ReadUInt32();			
	suitName = row.ReadString();

	return true;
}

FashionComposeMgr::FashionComposeMgr()
{

}

FashionComposeMgr::~FashionComposeMgr()
{

}

bool FashionComposeMgr::AddEntry(FashionComposeData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<FashionComposeData>::AddEntry(dataEntry)) return false;

	FashionComposeKey key(dataEntry->color, dataEntry->occu, dataEntry->part);

	FashionVec& datas = datasMap[key];
	datas.push_back(dataEntry);
	

	return true;
}

bool FashionComposeMgr::GetFahionDatas(FashionComposeKey key, FashionVec& datas)
{
	auto itr = datasMap.find(key);
	if (itr != datasMap.end())
	{
		datas = itr->second;
		return true;
	}

	return false;
}

void FashionComposeMgr::GetFashions(UInt8 part, UInt8 occu, std::vector<UInt32>& fashions)
{
	class Visitor : public Avalon::DataEntryVisitor<FashionComposeData>
	{
	public:
		Visitor(UInt8 part, UInt8 occu) {
			m_part = part;
			m_occu = occu;
		}

		~Visitor(){}

		bool operator()(FashionComposeData *data)
		{
			if (data->part == m_part &&
				data->occu == m_occu)
			{
				m_retIds.push_back(data->id);
			}
			return true;
		}

		UInt8 m_part;
		UInt8 m_occu;
		std::vector<UInt32> m_retIds;
	};

	Visitor visitor(part, occu);
	Visit(visitor);

	fashions = visitor.m_retIds;
}

FashionComposeSkyMgr::FashionComposeSkyMgr()
{

}

FashionComposeSkyMgr::~FashionComposeSkyMgr()
{

}

bool FashionComposeSkyMgr::AddEntry(FashionComposeSkyData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<FashionComposeSkyData>::AddEntry(dataEntry)) return false;
	if (dataEntry->type < SkyFashionType::SFT_COMMON || dataEntry->type > SkyFashionType::SFT_TRANSP)
	{
		return true;
	}

	if (dataEntry->type == SkyFashionType::SFT_COMMON || dataEntry->type == SkyFashionType::SFT_ADVANCE)
	{
		FashionComposeSkyKey key(dataEntry->occu, dataEntry->part, dataEntry->suitId);

		FashionSkyVec& datas = datasMap[key];
		datas.push_back(dataEntry);
	}

	if (dataEntry->type == SkyFashionType::SFT_TRANSP)
	{
		FashionComposeSkyKey key(dataEntry->occu, dataEntry->part, dataEntry->suitId);

		FashionSkyVec& datas = datasMap_Trans[key];
		datas.push_back(dataEntry);
	}

	FashionComposeSkyKeyII key2(dataEntry->occu, dataEntry->type, dataEntry->suitId);
	FashionSkyVec& datasII = datasMapII[key2];
	datasII.push_back(dataEntry);

	SkyFashionSuitKey sfsKey(dataEntry->suitId, dataEntry->type);
	if (suitId2Name.find(sfsKey) == suitId2Name.end())
	{
		suitId2Name.insert(std::make_pair(sfsKey, dataEntry->suitName));
	}

	return true;
}

bool FashionComposeSkyMgr::GetFahionSkyDatas(FashionComposeSkyKey key, FashionSkyVec& datas)
{
	auto itr = datasMap.find(key);
	if (itr != datasMap.end())
	{
		datas = itr->second;
		return true;
	}

	return false;
}

bool FashionComposeSkyMgr::GetTransFashionSkyDatas(FashionComposeSkyKey key, FashionSkyVec& datas)
{
	auto itr = datasMap_Trans.find(key);
	if (itr != datasMap_Trans.end())
	{
		datas = itr->second;
		return true;
	}

	return false;
}

void FashionComposeSkyMgr::GetFashionSkysWithFilter(UInt32 suitId, UInt8 part, std::vector<UInt32>& filter, std::vector<UInt32>& fashionSkys)
{
	class Visitor : public Avalon::DataEntryVisitor<FashionComposeSkyData>
	{
	public:
		Visitor(UInt32 suitId, UInt8 part, std::vector<UInt32>& filter) { 
			m_suitId = suitId;
			m_part = part;
			m_filter = filter;
		}

		~Visitor(){}

		bool operator()(FashionComposeSkyData *data)
		{
			if (data->suitId == m_suitId &&
				data->part == m_part)
			{
				bool needFilter = false;
				for (size_t i = 0; i < m_filter.size(); ++i)
				{
					if (m_filter[i] == data->index)
					{
						needFilter = true;
					}
				}

				if (!needFilter)
				{
					m_retIds.push_back(data->id);
				}
				
			}
			return true;
		}

		UInt32 m_suitId;
		UInt8 m_part;
		std::vector<UInt32> m_filter;
		std::vector<UInt32> m_retIds;
	};

	Visitor visitor(suitId, part, filter);
	Visit(visitor);

	fashionSkys = visitor.m_retIds;
}

void FashionComposeSkyMgr::GetFashionSkys(UInt32 suitId, UInt8 part, UInt8 occu, std::vector<FashionComposeSkyData*>& fashionSkys)
{
	class Visitor : public Avalon::DataEntryVisitor<FashionComposeSkyData>
	{
	public:
		Visitor(UInt32 suitId, UInt8 part, UInt8 occu) {
			m_suitId = suitId;
			m_part = part;
			m_occu = occu;
		}

		~Visitor(){}

		bool operator()(FashionComposeSkyData *data)
		{
			if (data->suitId == m_suitId &&
				data->part == m_part &&
				data->occu == m_occu)
			{
				m_retIds.push_back(data);
			}
			return true;
		}

		UInt32 m_suitId;
		UInt8 m_part;
		UInt8 m_occu;
		std::vector<FashionComposeSkyData*> m_retIds;
	};

	Visitor visitor(suitId, part, occu);
	Visit(visitor);

	fashionSkys = visitor.m_retIds;
}

void FashionComposeSkyMgr::GetFashionIdxBySuitId(UInt32 suitId, std::vector<UInt32>& fashionIdx)
{
	class Visitor : public Avalon::DataEntryVisitor<FashionComposeSkyData>
	{
	public:
		Visitor(UInt32 suitId) {
			m_suitId = suitId;
		}

		~Visitor(){}

		bool operator()(FashionComposeSkyData *data)
		{
			if (data->suitId == m_suitId)
			{
				m_retIdx.push_back(data->index);
			}
			return true;
		}

		UInt32 m_suitId;
		std::vector<UInt32> m_retIdx;
	};

	Visitor visitor(suitId);
	Visit(visitor);

	fashionIdx = visitor.m_retIdx;
}

UInt32 FashionComposeSkyMgr::GetWingId(UInt32 suitId, UInt8 occu, UInt32 type)
{
	class Visitor : public Avalon::DataEntryVisitor<FashionComposeSkyData>
	{
	public:
		Visitor(UInt32 suitId, UInt8 occu, UInt32 type) {
			m_suitId = suitId;
			m_occu = occu;
			m_type = type;
			m_windId = 0;
		}

		~Visitor(){}

		bool operator()(FashionComposeSkyData *data)
		{
			if (data->occu == m_occu &&
				data->suitId == m_suitId &&
				data->part == ST_FASHION_HALO &&
				data->type == m_type)
			{
				m_windId = data->id;
				return false;
			}
			return true;
		}

		UInt8 m_occu;
		UInt32 m_suitId;
		UInt32 m_type;
		UInt32 m_windId;

	};

	Visitor visitor(suitId, occu, type);
	Visit(visitor);

	return visitor.m_windId;
}

bool FashionComposeSkyMgr::GetFashionSkyDatasII(FashionComposeSkyKeyII key, FashionSkyVec& datas)
{
	auto itr = datasMapII.find(key);
	if (itr != datasMapII.end())
	{
		datas = itr->second;
		return true;
	}

	return false;
}

std::string FashionComposeSkyMgr::GetSuitName(UInt32 suitId, UInt8 type)
{
	SkyFashionSuitKey sfsKey(suitId, type);
	auto itr = suitId2Name.find(sfsKey);
	if (itr != suitId2Name.end())
	{
		return itr->second;
	}
	
	return "";
}

EquipMakeData::EquipMakeData()
{

}

EquipMakeData::~EquipMakeData()
{

}

bool EquipMakeData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();

	std::string matInfo = row.ReadString();
	if (matInfo == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(matInfo, strs, "|");

	if (strs.size() == 0)
		return false;

	MatInfo info;
	for (size_t i = 0; i < strs.size(); ++i)
	{
		std::vector<std::string> pram;
		Avalon::StringUtil::Split(strs[i], pram, "_");
		
		if (pram.size() != 2)
			return false;

		info.matId = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
		info.num = Avalon::StringUtil::ConvertToValue<UInt16>(pram[1].c_str());

		needMats.push_back(info);
	}

	std::string moneyInfo = row.ReadString();
	if (moneyInfo == "")
		return false;

	Avalon::StringUtil::Split(moneyInfo, strs, "_");
	if (strs.size() != 2)
		return false;

	needMoney = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0].c_str());
	needMoneyNum = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1].c_str());

	return true;
}

EquipMakeDataMgr::EquipMakeDataMgr()
{

}

EquipMakeDataMgr::~EquipMakeDataMgr()
{

}

bool EquipMakeDataMgr::AddEntry(EquipMakeData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<EquipMakeData>::AddEntry(dataEntry)) return false;
	return true;
}

EquipValueSceorData::EquipValueSceorData()
{

}

EquipValueSceorData::~EquipValueSceorData()
{

}

bool EquipValueSceorData::Read(Avalon::DataRow& row)
{
	type = row.ReadUInt32();
	value = row.ReadUInt32();
	return true;
}

EquipValueSceorDataMgr::EquipValueSceorDataMgr()
{

}

EquipValueSceorDataMgr::~EquipValueSceorDataMgr()
{

}

EquipQLRandData::EquipQLRandData()
{

}

EquipQLRandData::~EquipQLRandData()
{

}

bool EquipQLRandData::Read(Avalon::DataRow& row)
{
	key = row.ReadUInt32();
	color = row.ReadUInt8();			
	count = row.ReadUInt32();	

	std::string randRangeStr = row.ReadString();
	if (randRangeStr == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(randRangeStr, strs, "|");
	if (strs.size() != 2)
		return false;

	randRangeMin = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0].c_str());
	randRangeMax = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1].c_str());

	surpriseRate = row.ReadUInt32();

	randRangeStr = row.ReadString();
	if (randRangeStr == "")
		return false;

	strs.clear();
	Avalon::StringUtil::Split(randRangeStr, strs, "|");
	if (strs.size() != 2)
		return false;

	surpriseRangeMin = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0].c_str());
	surpriseRangeMax = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1].c_str());

	return true;
}

EquipQLRandDataMgr::EquipQLRandDataMgr()
{

}

EquipQLRandDataMgr::~EquipQLRandDataMgr()
{

}

bool RandDataSortFunc(EquipQLRandData* a, EquipQLRandData* b) { return (a->count < b->count); }
bool EquipQLRandDataMgr::LoadData(const std::string& filename)
{
	if (!Avalon::DataEntryMgrBase<EquipQLRandData>::LoadData(filename)) return false;

	//对数据进行排序
	auto itr = m_color2Datas.begin();
	for (; itr != m_color2Datas.end(); ++itr)
	{
		std::vector<EquipQLRandData*>& datas = itr->second;
		std::sort(datas.begin(), datas.end(), RandDataSortFunc);
	}
	return true;
}

bool EquipQLRandDataMgr::AddEntry(EquipQLRandData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<EquipQLRandData>::AddEntry(dataEntry)) return false;
	
	std::vector<EquipQLRandData*>& datas = m_color2Datas[dataEntry->color];
	datas.push_back(dataEntry);

	return true;
}

void EquipQLRandDataMgr::GetDatas(UInt8 color, std::vector<EquipQLRandData*>& datas)
{
	auto itr = m_color2Datas.find(color);
	if (itr != m_color2Datas.end())
	{
		datas = itr->second;
	}
}

EquipQLRandData* EquipQLRandDataMgr::GetData(UInt8 color, UInt32 randNum)
{
	std::vector<EquipQLRandData*> randDatas;
	EquipQLRandDataMgr::Instance()->GetDatas(color, randDatas);
	if (randDatas.size() == 0)
	{
		return NULL;
	}

	UInt32 lastIdx = 0;
	for (UInt32 i = 0; i < randDatas.size(); ++i)
	{
		if (randNum < randDatas[i]->count)
		{
			break;
		}

		lastIdx = i;
	}

	EquipQLRandData* randData = randDatas[lastIdx];
	if (!randData)
	{
		return NULL;
	}

	return randData;
}

UInt32 EquipQLRandDataMgr::GetAdaptNum(UInt8 color, UInt8 ql)
{
	std::vector<EquipQLRandData*> randDatas;
	EquipQLRandDataMgr::Instance()->GetDatas(color, randDatas);
	if (randDatas.size() == 0)
	{
		return 0;
	}

	if (ql < randDatas[0]->randRangeMin)
	{
		return randDatas[0]->count;
	}

	UInt32 i = 0;
	for (; i < randDatas.size(); ++i)
	{
		if (ql >= randDatas[i]->randRangeMin &&
			ql <= randDatas[i]->randRangeMax)
		{
			break;
		}

	}

	if (i >= randDatas.size())
	{
		i = randDatas.size() - 1;
	}

	EquipQLRandData* randData = randDatas[i];
	if (!randData)
	{
		return 0;
	}

	return randData->count;
}

EquipQLValueDataMgr::EquipQLValueDataMgr()
{

}

EquipQLValueDataMgr::~EquipQLValueDataMgr()
{

}

bool EquipQLValueDataMgr::AddEntry(EquipQLValueData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<EquipQLValueData>::AddEntry(dataEntry)) return false;
	return true;
}

EquipQLValueData::EquipQLValueData()
{

}

EquipQLValueData::~EquipQLValueData()
{

}

bool EquipQLValueData::Read(Avalon::DataRow& row)
{
	part = row.ReadUInt32();				
	atkDef = (float)row.ReadUInt32() / 1000;				
	fourDimensional = (float)row.ReadUInt32() / 1000;
	perfectAtkDef = (float)row.ReadUInt32() / 1000;
	perfectfourDimensional = (float)row.ReadUInt32() / 1000;
	strProp = (float)row.ReadUInt32() / 1000;
	perfectStrProp = (float)row.ReadUInt32() / 1000;
	defProp = (float)row.ReadUInt32() / 1000;
	perfectDefProp = (float)row.ReadUInt32() / 1000;
	adnormalResists = (float)row.ReadUInt32() / 1000;
	perfectAbnormalResists = (float)row.ReadUInt32() / 1000;
	independAtkProp = (float)row.ReadUInt32() / 1000;
	perfectIndpendAtkProp = (float)row.ReadUInt32() / 1000;
	return true;
}

FashionAttrData::FashionAttrData()
{

}

FashionAttrData::~FashionAttrData()
{

}

bool FashionAttrData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	defaultAttr1 = row.ReadUInt32();
	std::string attrsStr = row.ReadString();
	if (attrsStr == "")
	{
		ErrorLogStream << "attrsStr is empty" << LogStream::eos;
		return false;
	}
		

	std::vector<std::string> attrStr;
	Avalon::StringUtil::Split(attrsStr, attrStr, "|");
	if (attrStr.size() == 0)
	{
		ErrorLogStream << "attrStr size is 0" << LogStream::eos;
		return false;
	}
		

	for (size_t i = 0; i < attrStr.size(); ++i)
	{
		UInt32 attrId = Avalon::StringUtil::ConvertToValue<UInt32>(attrStr[i].c_str());
		if (attrId == 0)
		{
			ErrorLogStream << "attrId is 0, idx:" << (UInt32)i << LogStream::eos;
			return false;
		}
		attrs.push_back(attrId);
	}
	
	defaultAttr = row.ReadUInt32();
	
	bool find = false;
	for (size_t i = 0; i < attrs.size(); ++i)
	{
		if (defaultAttr == attrs[i])
		{
			find = true;
		}
	}

	if (!find)
	{
		ErrorLogStream << "not find defaultAttr in attrList!" << LogStream::eos;
		return false;
	}

	freeSelNum = row.ReadUInt32();

	return true;
}

FashionAttrDataMgr::FashionAttrDataMgr()
{

}

FashionAttrDataMgr::~FashionAttrDataMgr()
{

}

bool FashionAttrDataMgr::AddEntry(FashionAttrData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<FashionAttrData>::AddEntry(dataEntry))
	{
		ErrorLogStream << "AddEntry error! key:" << dataEntry->GetKey() << LogStream::eos;
		return false;
	}
		
	return true;
}

bool EquipStrColorData::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt32();

	for (UInt32 i = ITEM_QUALITY_WHITE; i < ITEM_QUALITY_MAX; ++i)
	{
		colorRatio[i] = row.ReadInt32();
	}

	return true;
}

EquipStrColorData::EquipStrColorData()
{

}

EquipStrColorData::~EquipStrColorData()
{

}

EquipStrColorDataMgr::EquipStrColorDataMgr()
{

}

EquipStrColorDataMgr::~EquipStrColorDataMgr()
{

}

GiftPackData::GiftPackData()
{

}

GiftPackData::~GiftPackData()
{

}

bool GiftPackData::Read(Avalon::DataRow& row)
{
	giftPackId = row.ReadUInt32();
	filterType = row.ReadUInt32();
	filterCount = row.ReadInt32();

	std::string itemsStr = row.ReadString();

	std::vector<std::string> itemIdStr;
	Avalon::StringUtil::Split(itemsStr, itemIdStr, "|");
	for (size_t i = 0; i < itemIdStr.size(); ++i)
	{
		UInt32 giftId = Avalon::StringUtil::ConvertToValue<UInt32>(itemIdStr[i].c_str());
		if (giftId == 0)
		{
			ErrorLogStream << "GiftPackData giftId is 0, idx:" << (UInt32)i << LogStream::eos;
			return false;
		}
		giftItems.push_back(giftId);
	}

	uiType = row.ReadUInt8();
	//description = row.ReadString();
	return true;
}

GiftItemData::GiftItemData()
{

}

GiftItemData::~GiftItemData()
{

}

bool GiftItemData::Read(Avalon::DataRow& row)
{
	giftId = row.ReadUInt32();
	itemId = row.ReadUInt32();
	giftPackId = row.ReadUInt32();
	num = row.ReadUInt32();

	std::string occuStr = row.ReadString();
	if (occuStr == "")
	{
		ErrorLogStream << "GiftItemData occuStr is empty" << LogStream::eos;
		return false;
	}


	std::vector<std::string> ocs;
	Avalon::StringUtil::Split(occuStr, ocs, "|");
	for (size_t i = 0; i < ocs.size(); ++i)
	{
		UInt8 occu = Avalon::StringUtil::ConvertToValue<UInt32>(ocs[i].c_str());
		if (occu == 0)
		{
			continue;
		}
		occus.push_back(occu);
	}
	
	weight = row.ReadUInt32();

	std::string levelsStr = row.ReadString();
	if (levelsStr == "")
	{
		ErrorLogStream << "GiftItemData levelsStr is empty" << LogStream::eos;
		return false;
	}
	std::vector<std::string> levs;
	Avalon::StringUtil::Split(levelsStr, levs, "|");
	
	for (size_t i = 0; i < levs.size(); ++i)
	{
		UInt8 lev = Avalon::StringUtil::ConvertToValue<UInt32>(levs[i].c_str());
		if (lev == 0)
		{
			continue;
		}
		levels.push_back(lev);
	}
	if (levels.size() != 2)
	{
		ErrorLogStream << "GiftItemData levelsStr size is not 2" << LogStream::eos;
		return false;
	}

	equipType = row.ReadUInt8();
	strengthen = row.ReadUInt32(); 

	return true;
}

GiftPackDataMgr::GiftPackDataMgr()
{

}

GiftPackDataMgr::~GiftPackDataMgr()
{

}

GiftItemDataMgr::GiftItemDataMgr()
{

}

GiftItemDataMgr::~GiftItemDataMgr()
{

}

void GiftItemDataMgr::MergeToGiftPack()
{
	class GiftDataVisitor : public Avalon::DataEntryVisitor<GiftItemData>
	{
	public:
		GiftDataVisitor() {}

		bool operator()(GiftItemData* dataEntry)
		{
			GiftPackData* giftPackData = GiftPackDataMgr::Instance()->FindEntry(dataEntry->giftPackId);
			if (giftPackData != NULL)
			{
				giftPackData->giftItems.push_back(dataEntry->giftId);
			}
			return true;
		}
	};

	GiftDataVisitor visitor;
	Visit(visitor);
}

FashionComposeRate::FashionComposeRate()
{

}

FashionComposeRate::~FashionComposeRate()
{

}

bool FashionComposeRate::Read(Avalon::DataRow& row)
{
	type = row.ReadUInt32();
	rate = row.ReadUInt32() / 10;

	return true;
}

FashionCompPartRate::FashionCompPartRate()
{

}

FashionCompPartRate::~FashionCompPartRate()
{

}

bool FashionCompPartRate::Read(Avalon::DataRow& row)
{
	composeNum = row.ReadUInt32();
	noneSkyRate = row.ReadUInt32() / 10;
	normalSkyRate = row.ReadUInt32() / 10;
	goldSkyRate = row.ReadUInt32() / 10;

	return true;
}

UInt32 FashionCompPartRateMgr::GetMaxNum()
{
	return m_maxNum;
}

FashionCompPartRateMgr::FashionCompPartRateMgr()
{
	m_maxNum = 0;
}

FashionCompPartRateMgr::~FashionCompPartRateMgr()
{

}

bool FashionCompPartRateMgr::AddEntry(FashionCompPartRate* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<FashionCompPartRate>::AddEntry(dataEntry)) return false;

	if (dataEntry->composeNum > m_maxNum)
	{
		m_maxNum = dataEntry->composeNum;
	}

	return true;
}

FashionComposeRateMgr::FashionComposeRateMgr()
{

}

FashionComposeRateMgr::~FashionComposeRateMgr()
{

}

bool PreciousBeadExtirpeDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	color = row.ReadUInt32();
	level = row.ReadUInt32();
	materialId = row.ReadUInt32();
	materialNum = row.ReadUInt32();
	rate = row.ReadUInt32();
	type = row.ReadUInt8();
	pickNum = row.ReadInt32();

	return true;
}

PreciousBeadExtirpeDataEntry* PreciousBeadExtirpeDataEntryMgr::GetDataEntry(UInt32 color, UInt32 level, UInt32 materialId, UInt8 type)
{
	class DataEntryVisitor : public PreciousBeadExtirpeDataEntryVisitor
	{
	public:
		DataEntryVisitor(UInt32 _color, UInt32 _level, UInt32 _materialId, UInt8 _type)
			: color(_color), level(_level), materialId(_materialId), type(_type), entry(NULL){}
		~DataEntryVisitor() {}
		virtual bool operator()(PreciousBeadExtirpeDataEntry* dataEntry)
		{
			if (dataEntry->color == color && dataEntry->level == level 
				&& dataEntry->materialId == materialId && dataEntry->type == type)
			{
				entry = dataEntry;
				return false;
			}
			return true;
		}
	public:
		UInt32 color;
		UInt32 level;
		UInt32 materialId;
		UInt8  type;
		PreciousBeadExtirpeDataEntry* entry;
	};

	DataEntryVisitor visitor_(color, level, materialId, type);
	Visit(visitor_);
	return visitor_.entry;
}

bool PreciousBeadReplaceDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	color = row.ReadUInt32();
	level = row.ReadUInt32();
	type = row.ReadUInt8();
	costItemId = row.ReadUInt32();
	costItemNum = row.ReadUInt32();
	replaceNum = row.ReadInt32();

	return true;
}

PreciousBeadReplaceDataEntry* PreciousBeadReplaceDataEntryMgr::GetDataEntry(UInt32 color, UInt32 level, UInt8 type)
{
	class DataEntryVisitor : public PreciousBeadReplaceDataEntryVisitor
	{
	public:
		DataEntryVisitor(UInt32 _color, UInt32 _level, UInt8 _type)
			: color(_color), level(_level), type(_type), entry(NULL){}
		~DataEntryVisitor() {}
		virtual bool operator()(PreciousBeadReplaceDataEntry* dataEntry)
		{
			if (dataEntry->color == color && dataEntry->level == level
				&& dataEntry->type == type)
			{
				entry = dataEntry;
				return false;
			}
			return true;
		}
	public:
		UInt32 color;
		UInt32 level;
		UInt8  type;
		PreciousBeadReplaceDataEntry* entry;
	};

	DataEntryVisitor visitor_(color, level, type);
	Visit(visitor_);
	return visitor_.entry;
}

bool PrecBeadAddBuffRandomDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	buffGroupId = row.ReadUInt32();
	weight = row.ReadUInt32();
	buffInfoId = row.ReadUInt32();
	return true;
}

bool PrecBeadAddBuffRandomDataEntryMgr::AddEntry(PrecBeadAddBuffRandomDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<PrecBeadAddBuffRandomDataEntry>::AddEntry(dataEntry)) return false;

	DataEntryVector& v = dataEntrysMap[dataEntry->buffGroupId];
	v.push_back(dataEntry);

	return true;
}

UInt32 PrecBeadAddBuffRandomDataEntryMgr::RandomBuffIdByBuffGroupId(UInt32 buffGroupId)
{
	DataEntryMap::iterator it = dataEntrysMap.find(buffGroupId);
	if (it == dataEntrysMap.end()) return 0;

	DataEntryVector& vec = it->second;
	UInt32 buffId = 0;
	if (vec.size() > 0)
	{
		UInt32 totalWeight = 0;
		for (UInt32 i = 0; i < vec.size(); ++i)
		{
			totalWeight += vec[i]->weight;
		}
		UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
		for (UInt32 i = 0; i < vec.size(); ++i)
		{
			if (vec[i]->weight >= randWeight)
			{
				buffId = vec[i]->buffInfoId;
				break;
			}
			else
			{
				randWeight -= vec[i]->weight;
			}
		}
	}
	return buffId;
}

bool AssistEqStrengFouerDimAddDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	strengthen = row.ReadUInt8();
	equipLv = row.ReadUInt16();
	equipColor1 = row.ReadUInt8();
	equipColor2 = row.ReadUInt8();
	addValue = row.ReadUInt32();
	addValuePvp = row.ReadUInt32();
	eqScore = row.ReadUInt32();
	return true;
}

bool AssistEqStrengFouerDimAddDataEntryMgr::AddEntry(AssistEqStrengFouerDimAddDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<AssistEqStrengFouerDimAddDataEntry>::AddEntry(dataEntry)) return false;

	AssistEqStrengKey key(dataEntry->strengthen, dataEntry->equipLv, dataEntry->equipColor1, dataEntry->equipColor2);

	auto iter = dataEntrysMap.find(key);
	if (iter != dataEntrysMap.end())
	{
		ErrorLogStream << "add entry err duplicate id : " << dataEntry->GetKey() << LogStream::eos;
		return false;
	}

	dataEntrysMap.insert(std::pair<AssistEqStrengKey, AssistEqStrengFouerDimAddDataEntry*>(key, dataEntry));
	return true;
}

UInt32	AssistEqStrengFouerDimAddDataEntryMgr::GetAddValue(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2)
{
	AssistEqStrengKey key(strengthen, equipLv,equipColor1, equipColor2);
	auto iter = dataEntrysMap.find(key);
	if (iter == dataEntrysMap.end() || !iter->second)
	{
		return 0;
	}
	return iter->second->addValue;
}

UInt32	AssistEqStrengFouerDimAddDataEntryMgr::GetPvpAddValue(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2)
{
	AssistEqStrengKey key(strengthen, equipLv, equipColor1, equipColor2);
	auto iter = dataEntrysMap.find(key);
	if (iter == dataEntrysMap.end() || !iter->second)
	{
		return 0;
	}
	return iter->second->addValuePvp;
}

UInt32 AssistEqStrengFouerDimAddDataEntryMgr::GetEqScore(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2)
{
	AssistEqStrengKey key(strengthen, equipLv, equipColor1, equipColor2);
	auto iter = dataEntrysMap.find(key);
	if (iter == dataEntrysMap.end() || !iter->second)
	{
		return 0;
	}
	return iter->second->eqScore;
}

bool EnhanceOverLoadDataEntry::Read(Avalon::DataRow & row)
{
	id = row.ReadUInt32();
	itemID = row.ReadUInt32();
	enhanceLv = row.ReadUInt32();
	raito = row.ReadUInt32();
	return true;
}

bool EnhanceOverLoadDataEntryMgr::AddEntry(EnhanceOverLoadDataEntry * dataEntry)
{
	if (dataEntry == nullptr)
	{
		return false;
	}
	if (Avalon::DataEntryMgrBase<EnhanceOverLoadDataEntry>::AddEntry(dataEntry))
	{

		dataMap[dataEntry->itemID].push_back(std::make_pair(dataEntry->enhanceLv, dataEntry->raito));
		return true;
	}
	return false;
}

UInt32 EnhanceOverLoadDataEntryMgr::GetEnhanceLevel(UInt32 itemID)
{
	auto data = dataMap.find(itemID);
	if (data != dataMap.end())
	{
		UInt32 totalRaito = 0;
		for (auto &i : data->second)
		{
			totalRaito += i.second;
		}

		Int32 result = Avalon::Random::RandBetween(1, totalRaito);
		for (auto &i : data->second)
		{
			result -= i.second;
			if (result <= 0)
			{
				return i.first;
			}
		}
	}
	return 0;
}

bool EquipIndpendatkStrengModDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	
	std::string strengRatesStr = row.ReadString();
	std::vector<std::string> strengRatess;
	Avalon::StringUtil::Split(strengRatesStr, strengRatess, "|");
	for (size_t i = 0; i < strengRatess.size(); ++i)
	{
		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(strengRatess[i].c_str());
		strengRates.push_back(rate);
	}
	
	std::string colorRateAStr = row.ReadString();
	std::vector<std::string> colorRateAss;
	Avalon::StringUtil::Split(colorRateAStr, colorRateAss, "|");
	for (size_t i = 0; i < colorRateAss.size(); ++i)
	{
		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(colorRateAss[i].c_str());
		colorRateA.push_back(rate);
	}

	std::string colorRateBStr = row.ReadString();
	std::vector<std::string> colorRateBss;
	Avalon::StringUtil::Split(colorRateBStr, colorRateBss, "|");
	for (size_t i = 0; i < colorRateBss.size(); ++i)
	{
		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(colorRateBss[i].c_str());
		colorRateB.push_back(rate);
	}

	partRateAtk = row.ReadUInt32();

	return true;
}

UInt32 EquipIndpendatkStrengModDataEntryMgr::GetStrengRate(UInt32 id, UInt32 strengLv)
{
	if (strengLv == 0)
	{
		return 0;
	}
	EquipIndpendatkStrengModDataEntry* entry = FindEntry(id);
	if (entry == NULL)
	{
		return 0;
	}

	if (strengLv - 1> entry->strengRates.size() - 1)
	{
		return 0;
	}

	return entry->strengRates[strengLv - 1];
}

UInt32 EquipIndpendatkStrengModDataEntryMgr::GetColorARate(UInt32 id, UInt32 colorA)
{
	if (colorA == 0)
	{
		return 0;
	}
	EquipIndpendatkStrengModDataEntry* entry = FindEntry(id);
	if (entry == NULL)
	{
		return 0;
	}

	if (colorA - 1 > entry->colorRateA.size() - 1)
	{
		return 0;
	}

	return entry->colorRateA[colorA - 1];
}

UInt32 EquipIndpendatkStrengModDataEntryMgr::GetColorBRate(UInt32 id, UInt32 colorB)
{
	if (colorB == 0)
	{
		return 0;
	}
	EquipIndpendatkStrengModDataEntry* entry = FindEntry(id);
	if (entry == NULL)
	{
		return 0;
	}

	if (colorB - 1 > entry->colorRateB.size() - 1)
	{
		return 0;
	}

	return entry->colorRateB[colorB - 1];
}

UInt32 EquipIndpendatkStrengModDataEntryMgr::GetPartRate(UInt32 id)
{
	EquipIndpendatkStrengModDataEntry* entry = FindEntry(id);
	if (entry == NULL)
	{
		return 0;
	}

	return entry->partRateAtk;
}

OnceStrTicketData::OnceStrTicketData()
{
}

OnceStrTicketData::~OnceStrTicketData()
{
}

bool OnceStrTicketData::Read(Avalon::DataRow & row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column num is not match!" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	lvLimitBegin = row.ReadUInt8();
	lvLimitEnd = row.ReadUInt8();
	rate = row.ReadUInt32();

	return true;
}

bool OnceStrTicketDataMgr::AddEntry(OnceStrTicketData * dataEntry)
{
	if (!Avalon::DataEntryMgrBase<OnceStrTicketData>::AddEntry(dataEntry))
	{
		return false;
	}
	return true;
}
