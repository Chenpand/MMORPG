#include "ItemMaker.h"

#include <AvalonRandom.h>
#include <CLRecordClient.h>
#include <CLItemDataEntry.h>
#include <set>

#include "ScriptMgr.h"
#include "FinalEquipScript.h"
#include "Player.h"

ItemMaker::ItemMaker()
{
}

ItemMaker::~ItemMaker()
{
}

Item* ItemMaker::MakeItem(Player* player, UInt32 itemid, UInt16 num, UInt8 qualityLv, UInt8 strenth, UInt32 auctionCoolTimeStamp)
{
	if(num == 0) return NULL;
	if (!player) return NULL;

	Item* item = Item::CreateItem(itemid);
	if (!item) return NULL;
	
	item->SetParam1(1);	// �������µ��߱�ʶ

	item->SetID(CLRecordClient::Instance()->GenGuid());

	//��ʼ��
	item->SetNum(num);
	
	item->SetStrengthenValue(strenth);
	
	item->SetAuctionCoolTimeStamp(auctionCoolTimeStamp);
	
	//���õ���ʱ��
	if (!item->SetDeadLine())
	{
		ErrorLogStream << PLAYERINFO(player) << " Item SetDeadLine failed!" << LogStream::eos;
		Item::DestroyItem(item);
		return NULL;
	}
	
	//ָ��Ʒ��
	if (qualityLv > 0)
	{
		if (qualityLv > 100)	qualityLv = 100;
		item->SetQualityLv(qualityLv);
	}
	else
	{
		//���Ʒ��
		if (item->GetType() == ITEM_EQUIP)
		{
			Int32 ql = Avalon::Random::RandBetween(1, 99);
			item->SetQualityLv((UInt8)ql);
			player->GetItemMgr().AdaptQLData(item);
		}
	}

	//����װ��,�ƺ�,ʱװ����
	if (!item->SetAttribute())
	{
		ErrorLogStream << PLAYERINFO(player) << "Item SetAttribute failed!" << LogStream::eos;
		Item::DestroyItem(item);
		return NULL;
	}

	item->ClearDirty();

	//���ɱ�����Ƕ��
	item->GenPreciousBeadHoles();

	return item;
}

Item* ItemMaker::MakeItemWithCallback(Player* player, UInt32 itemid, UInt16 num, const std::function<bool(Item&)>& initCallback, UInt8 qualityLv, UInt8 strenth, UInt32 auctionCoolTimeStamp)
{
	if (num == 0) return NULL;
	if (!player) return NULL;

	Item* item = Item::CreateItem(itemid);
	if (!item) return NULL;

	item->SetParam1(1);	// �������µ��߱�ʶ

	item->SetID(CLRecordClient::Instance()->GenGuid());

	//��ʼ��
	item->SetNum(num);

	item->SetAuctionCoolTimeStamp(auctionCoolTimeStamp);

	//���õ���ʱ��
	if (!item->SetDeadLine())
	{
		ErrorLogStream << PLAYERINFO(player) << " Item SetDeadLine failed!" << LogStream::eos;
		Item::DestroyItem(item);
		return NULL;
	}

	if (initCallback == nullptr || !initCallback(*item))
	{
		item->SetStrengthenValue(strenth);
		//ָ��Ʒ��
		if (qualityLv > 0)
		{
			if (qualityLv > 100)	qualityLv = 100;
			item->SetQualityLv(qualityLv);
		}
		else
		{
			//���Ʒ��
			if (item->GetType() == ITEM_EQUIP)
			{
				Int32 ql = Avalon::Random::RandBetween(1, 99);
				item->SetQualityLv((UInt8)ql);
				player->GetItemMgr().AdaptQLData(item);
			}
		}
	}

	//����װ��,�ƺ�,ʱװ����
	if (!item->SetAttribute())
	{
		ErrorLogStream << PLAYERINFO(player) << "Item SetAttribute failed!" << LogStream::eos;
		Item::DestroyItem(item);
		return NULL;
	}

	item->ClearDirty();

	//���ɱ�����Ƕ��
	item->GenPreciousBeadHoles();

	return item;
}

// bool ItemMaker::GenEquipAttrs(Item* item)
// {
// 	if(item == NULL || item->GetDataEntry()->type != ITEM_EQUIP) return false;
// 	if(!IsEquipCanRemake(item->GetSubType())) return false;
// 
// 	// �ռ�װ���������������
// 	if(ScriptMgr::Instance()->GetFinalEquipScript()->IsFinalEquip(item->GetDataID()))
// 		return true;
// 
// 	UInt32	uAttrNum = GetRandAttrNum(item->GetNeedLevel());
// 	if(0 == uAttrNum)
// 		return true;
// 
// 	//��ȡ����������ɸ���
// 	class GetRateVisitor : public RandAttrDataEntryVisitor
// 	{
// 	public:
// 		GetRateVisitor(std::map<UInt32, UInt32>& rates, UInt8 occu)
// 			:m_Rates(rates), m_uOccu(occu){}
// 		~GetRateVisitor(){}
// 
// 		bool operator()(RandAttrDataEntry *dataEntry)
// 		{
// 			if(dataEntry->occu != m_uOccu)
// 				return true;
// 			UInt32 rate = dataEntry->rate;
// 			m_Rates.insert(std::make_pair(dataEntry->no, rate));
// 			return true;
// 		}
// 
// 	private:
// 		std::map<UInt32, UInt32>&	m_Rates;
// 		UInt8						m_uOccu;
// 	};
// 
// 	std::map<UInt32, UInt32> rates;
// 	GetRateVisitor visitor(rates, item->GetNeedOccu());
// 	RandAttrDataEntryMgr::Instance()->Visit(visitor);
// 	//��ȡ�������
// 	UInt32 totalRate = 0;
// 	for(std::map<UInt32, UInt32>::iterator iter = rates.begin();
// 		iter != rates.end(); ++iter)
// 	{
// 		totalRate += iter->second;
// 	}
// 	if(totalRate == 0)
// 		return true;
// 
// 	//��ȡ��������Ǽ�����
// 	class GetAttrStarRateVisitor : public RandAttrStarDataEntryVisitor
// 	{
// 	public:
// 		GetAttrStarRateVisitor(std::map<UInt32, UInt32>& rates, UInt16 uLevelSection, UInt8 quality)
// 			: m_Rates(rates), m_uLevelSection(uLevelSection), m_uQuality(quality)
// 		{
// 			m_TotalStarRate = 0;
// 		}
// 		~GetAttrStarRateVisitor(){}
// 
// 		bool operator()(RandAttrStarDataEntry* dataEntry)
// 		{
// 			if(dataEntry->quality != m_uQuality)
// 				return true;
// 
// 			m_Rates.insert(std::make_pair(dataEntry->star, dataEntry->rate[m_uLevelSection]));
// 			m_TotalStarRate += dataEntry->rate[m_uLevelSection];
// 			return true;
// 		}
// 
// 		UInt32 GetTotalStarRate() const{ return m_TotalStarRate; }
// 	private:
// 		std::map<UInt32, UInt32>&	m_Rates;
// 		UInt32	m_TotalStarRate;
// 		UInt16	m_uLevelSection;
// 		UInt8	m_uQuality;
// 	};
// 	std::map<UInt32, UInt32>	starRates;
// 	GetAttrStarRateVisitor starVisitor(starRates, GetEquipLevelSection(item->GetNeedLevel()), item->GetQuality());
// 	RandAttrStarDataEntryMgr::Instance()->Visit(starVisitor);
// 
// 	UInt32 totalStarRate = starVisitor.GetTotalStarRate();
// 	if(totalStarRate == 0)
// 		return true;
// 	
// 	for(UInt32 i = 0; i < uAttrNum; ++i)
// 	{
// 		UInt32 rate = Avalon::Random::RandBetween(0, totalRate - 1);
// 		UInt32 attrNo = 0;
// 		UInt32 value = 0;
// 		for(std::map<UInt32, UInt32>::iterator iter = rates.begin(); iter != rates.end(); ++iter)
// 		{
// 			value += iter->second;
// 			if(rate < value)
// 			{
// 				attrNo = iter->first;
// 				break;
// 			}
// 		}
// 		if(attrNo == 0)
// 			return true;
// 
// 		RandAttrDataEntry	*pEntry = RandAttrDataEntryMgr::Instance()->FindEntry(attrNo);
// 		if(NULL == pEntry)
// 			return true;
// 
// 		//��ȡ�����Ǽ�
// 		UInt32 star = 0;
// 		UInt32 starrate = Avalon::Random::RandBetween(0, totalStarRate - 1);
// 		UInt32 starvalue = 0;
// 		for(std::map<UInt32, UInt32>::iterator iter = starRates.begin();
// 			iter != starRates.end(); ++iter)
// 		{
// 			starvalue += iter->second;
// 			if(starrate < starvalue)
// 			{
// 				star = iter->first;
// 				break;
// 			}
// 		}
// 		if(star == 0 || star > MAX_EQUIPATTR_STAR_NUM)
// 			return true;
// 
// 		//�����Ǽ���ȡ����
// 		item->AddRandAttr(PkValueID(pEntry->attrid), MOD_PKVALUE_BASE, pEntry->val[star - 1]);
// 
// 		//���¼������Ը���
// 		std::map<UInt32, UInt32>::iterator rateIter = rates.find(attrNo);
// 		if(rateIter != rates.end())
// 		{
// 			UInt32 oldValue = rateIter->second;
// 			rateIter->second = UInt32(oldValue * RANDATTR_RATE_FACTOR);
// 			totalRate = totalRate - (oldValue - rateIter->second);
// 		}
// 	}
// 
// 	return true;
// }

bool ItemMaker::GenEquipAttrs(Item* item)
{
	if (item == NULL || item->GetDataEntry()->type != ITEM_EQUIP) return false;

	// �õ����������Ŀ
	UInt32	uAttrNum = GetRandAttrNum((ItemQuality)item->GetQuality());
	if (0 == uAttrNum)
		return true;

	// �õ�����������
	std::vector<UInt32> randAttrs;
	GetRandAttr(randAttrs, uAttrNum);

	// �õ����������ֵ
	std::vector<UInt32> randVals;
	GetRandValue(randAttrs, (ItemQuality)item->GetQuality(), item->GetNeedLevel(), randVals);

	if (randAttrs.size() != randVals.size())
	{
		ErrorLogStream << "GenEquipAttrs randAttrs not equal to randValues!" << LogStream::eos;
		return false;
	}

	for (UInt32 i = 0; i < randAttrs.size();++i)
	{
		item->AddRandAttr((ItemRandProp)randAttrs[i], randVals[i]);
	}
	

	return true;
}

/**
*@brief ����װ���ȼ���������������
*/
UInt32 ItemMaker::GetRandAttrNum(ItemQuality color)
{
	RandAttrNumDataEntry* randAttrNum = RandAttrNumDataEntryMgr::Instance()->FindEntry(color);
	if (randAttrNum == NULL) return 0;
	return randByWeight(randAttrNum->nums, randAttrNum->weights);
}

// �õ�Ȩ�������
UInt32 ItemMaker::randByWeight(std::vector<UInt32>& vals, std::vector<UInt32>& wts)
{
	if (vals.size() != wts.size())
	{
		ErrorLogStream << "randByWeight weights not equal to values!" << LogStream::eos;
		return 0;
	}
		
	Int32 totWt = 0;
	for (UInt32 i = 0; i < wts.size(); ++i)
	{
		totWt += wts[i];
	}

	Int32 randWt = Avalon::Random::RandBetween(1, totWt);
	UInt32 sumWt = 0;
	for (UInt32 i = 0; i < wts.size(); ++i)
	{
		sumWt += wts[i];
		if (randWt <= (Int32)sumWt)
		{
			return vals[i];
		}
	}

	return 0;
}

Int32 ItemMaker::randByWeight(std::vector<UInt32>& wts)
{
	if (0 == wts.size())
		return -1;

	Int32 totWt = 0;
	for (UInt32 i = 0; i < wts.size(); ++i)
	{
		totWt += wts[i];
	}

	Int32 randWt = Avalon::Random::RandBetween(1, totWt);
	UInt32 sumWt = 0;
	for (UInt32 i = 0; i < wts.size(); ++i)
	{
		sumWt += wts[i];
		if (randWt <= (Int32)sumWt)
		{
			return i;
		}
	}

	return -1;
}

/**
*@brief �õ���Ӧ����������������
*/
void ItemMaker::GetRandAttr(std::vector<UInt32>& attrs, Int32 num)
{
	std::vector<UInt32> randProps;
	std::vector<UInt32> randWts;
	RandAttrDataEntryMgr::Instance()->GetTypes(randProps);
	RandAttrDataEntryMgr::Instance()->GetWeights(randWts);

	randByWeightCplx(randProps, randWts, attrs, num);
}


void ItemMaker::randByWeightCplx(std::vector<UInt32>& vals, std::vector<UInt32>& wts, std::vector<UInt32>& out, Int32 num /*= 1*/)
{
	if (vals.size() != wts.size())
	{
		ErrorLogStream << "randByWeightCplx weights not equal to values!" << LogStream::eos;
		return;
	}

	for (Int32 n = 0; n < num; ++n)
	{
		Int32 totWt = 0;
		for (UInt32 i = 0; i < wts.size(); ++i)
			totWt += wts[i];

		Int32 randWt = Avalon::Random::RandBetween(1, totWt);
		UInt32 sumWt = 0;
		for (UInt32 i = 0; i < wts.size(); ++i)
		{
			sumWt += wts[i];
			if (randWt <= (Int32)sumWt)
			{
				out.push_back(vals[i]);
				wts.erase(wts.begin() + i);
				vals.erase(vals.begin() + i);
				break;	
			}
		}
	}
	
	return;
}

/**
*@brief �õ���ӦƷ�ʺ͵ȼ������������ֵ
*/
void ItemMaker::GetRandValue(std::vector<UInt32>& attrs, ItemQuality color, UInt16 lv, std::vector<UInt32>& vals)
{
	for (Int32 n = 0; n < (Int32)attrs.size(); ++n)
	{
		std::vector<RandAttrValueDataEntry*> datas;
		RandAttrValueDataEntryMgr::Instance()->GetDatas(attrs[n], datas);

		// �ҵ������������������
		std::vector<UInt32> valsTmp;
		std::vector<UInt32> wtsTmp;
		for (UInt32 i = 0; i < datas.size(); ++i)
		{
			if (datas[i]->color == color &&
				lv <= datas[i]->lvMax && lv >= datas[i]->lvMin)
			{
				valsTmp = datas[i]->values;
				wtsTmp = datas[i]->weights;
				vals.push_back(randByWeight(valsTmp, wtsTmp));
				break;
			}

		}
	}
	
}

bool ItemMaker::GetRandQLValue(Item* item)
{
// 	RandBaseDataEntry* randBasedata = RandBaseDataEntryMgr::Instance()->GetData();
// 	ItemDataEntry* itemData = item->GetDataEntry();
// 	if (!randBasedata || !itemData)
// 		return false;
// 
// 	// ����õ�װ��Ʒ��
// 	Int32 qualityLv = randByWeight(randBasedata->QLPs);
// 	if (qualityLv <= -1 || qualityLv >= (Int32)randBasedata->QLOMin.size() || qualityLv >= (Int32)randBasedata->QLOMax.size())
// 		return false;
// 	
// 	// ��Ʒ������������õ�װ��Ʒ��������
// 	Int32 rate = Avalon::Random::RandBetween(randBasedata->QLOMin[qualityLv], randBasedata->QLOMax[qualityLv]);
// 	item->SetQualityLv(rate);
// 	float baseRatio = (float)randBasedata->baseRatio / 100.00f;		// �����ٷֱ�ϵ��
// 	float qlRate = (float)rate / 100.00f;							// Ʒ������ϵ��
// 
// 	if (itemData->attrId != 0)
// 	{
// 		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(itemData->attrId);
// 		if (!attrData)
// 		{
// 			ErrorLogStream << "make item(" << itemData->id << ") failed." << "attr id(" << itemData->attrId << ") err!" << LogStream::eos;
// 			return false;
// 		}
// 
// 		// ����װ��ʵ�ʻ���ֵ ��ʽ: ����ֵ = round������ֵ*�ٷֱ�a + ����ֵ*��1-�ٷֱ�a��*Ʒ��������
// 		if (0 != attrData->atk)			item->SetPhyAtk((UInt32)((attrData->atk * baseRatio + attrData->atk * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->magicAtk)	item->SetMagAtk((UInt32)((attrData->magicAtk * baseRatio + attrData->magicAtk * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->def)			item->SetPhyDef((UInt32)((attrData->def * baseRatio + attrData->def * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->magicDef)	item->SetMagDef((UInt32)((attrData->magicDef * baseRatio + attrData->magicDef * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->strenth)		item->SetStrenth((UInt32)((attrData->strenth * baseRatio + attrData->strenth * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->stamina)		item->SetStamina((UInt32)((attrData->stamina * baseRatio + attrData->stamina * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->spirit)		item->SetSpirit((UInt32)((attrData->spirit * baseRatio + attrData->spirit * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->intellect)	item->SetIntellect((UInt32)((attrData->intellect * baseRatio + attrData->intellect * (1 - baseRatio) * qlRate) + 0.5));
// 	}
	

	return true;
}

bool ItemMaker::GenDataAttrs(Item* item, EquipAttrDataEntry* attrData)
{
	if (!item || !attrData)
		return false;

	item->SetPhyAtk(attrData->atk);
	item->SetMagAtk(attrData->magicAtk);
	item->SetPhyDef(attrData->def);
	item->SetMagDef(attrData->magicDef);
	item->SetStrenth(attrData->strenth);
	item->SetStamina(attrData->stamina);
	item->SetSpirit(attrData->spirit);
	item->SetIntellect(attrData->intellect);
	

	return true;
}

bool ItemMaker::SetQLValue(Item* item, EquipQualityLv ql)
{
// 	if (!item)
// 	{
// 		return false;
// 	}
// 
// 	if (ql != EQL_HIGHEST)
// 	{
// 		return false;
// 	}
// 
// 	RandBaseDataEntry* randBasedata = RandBaseDataEntryMgr::Instance()->GetData();
// 	ItemDataEntry* itemData = item->GetDataEntry();
// 	if (!randBasedata || !itemData)
// 		return false;
// 
// 	if (ql <= EQL_INVALID || ql >= EQL_MAX)
// 		return false;
// 
// 	// ��Ʒ������������õ�װ��Ʒ��������
// 	Int32 rate = randBasedata->QLOMax[randBasedata->QLOMax.size() - 1];
// 	item->SetQualityLv(rate);
// 	float baseRatio = (float)randBasedata->baseRatio / 100.00f;		// �����ٷֱ�ϵ��
// 	float qlRate = (float)rate / 100.00f;							// Ʒ������ϵ��
// 
// 	if (itemData->attrId != 0)
// 	{
// 		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(itemData->attrId);
// 		if (!attrData)
// 		{
// 			ErrorLogStream << "make item(" << itemData->id << ") failed." << "attr id(" << itemData->attrId << ") err!" << LogStream::eos;
// 			return false;
// 		}
// 
// 		// ����װ��ʵ�ʻ���ֵ ��ʽ: ����ֵ = round������ֵ*�ٷֱ�a + ����ֵ*��1-�ٷֱ�a��*Ʒ��������
// 		if (0 != attrData->atk)			item->SetPhyAtk((UInt32)((attrData->atk * baseRatio + attrData->atk * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->magicAtk)	item->SetMagAtk((UInt32)((attrData->magicAtk * baseRatio + attrData->magicAtk * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->def)			item->SetPhyDef((UInt32)((attrData->def * baseRatio + attrData->def * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->magicDef)	item->SetMagDef((UInt32)((attrData->magicDef * baseRatio + attrData->magicDef * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->strenth)		item->SetStrenth((UInt32)((attrData->strenth * baseRatio + attrData->strenth * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->stamina)		item->SetStamina((UInt32)((attrData->stamina * baseRatio + attrData->stamina * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->spirit)		item->SetSpirit((UInt32)((attrData->spirit * baseRatio + attrData->spirit * (1 - baseRatio) * qlRate) + 0.5));
// 		if (0 != attrData->intellect)	item->SetIntellect((UInt32)((attrData->intellect * baseRatio + attrData->intellect * (1 - baseRatio) * qlRate) + 0.5));
// 	}


	return true;
}
