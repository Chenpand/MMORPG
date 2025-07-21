#include "MallScript.h"
#include "MallMgr.h"
#include "GameParamMgr.h"
#include "WSActivityMgr.h"
#include <sstream>

MallScript::MallScript(Avalon::ScriptState *state)
	: Avalon::ScriptFile(state)
{
}

MallScript::~MallScript()
{
}

void MallScript::Init()
{
	Call<void>("Init", this);
}

void MallScript::AddNormalLimitItem(UInt32 uIndex, const char *szItem)
{
	if(uIndex > 4)
		return;

	uIndex--;
	if(m_vecLimitedInfo[uIndex].size() >= 30)
		return;

	MallItemInfo	item;
	memset(&item, 0, sizeof(MallItemInfo));

	DecodeItemInfo(item, szItem);

	m_vecLimitedInfo[uIndex].push_back(item);
}

void MallScript::AddGSLimitItem(const char *szItem)
{
	MallItemInfo	item;
	memset(&item, 0, sizeof(MallItemInfo));

	DecodeItemInfo(item, szItem);

	m_vecGSItems.push_back(item);
}

void MallScript::AddFestivalLimitItem(const char *szActivity, const char *szItem)
{
	MallItemInfo	item;
	memset(&item, 0, sizeof(MallItemInfo));

	DecodeItemInfo(item, szItem);

	m_mapFestivalItems[szActivity].push_back(item);
}

void MallScript::AddGoldStoneItem(const char *szItem)
{
	MallItemInfo	item;
	memset(&item, 0, sizeof(MallItemInfo));

	DecodeItemInfo(item, szItem);

	m_vecGoldStoneItems.push_back(item);
}

void MallScript::AddSilverStoneItem(const char *szItem)
{
	MallItemInfo	item;
	memset(&item, 0, sizeof(MallItemInfo));

	DecodeItemInfo(item, szItem);

	m_vecSilverStoneItems.push_back(item);
}

void MallScript::SetCurLimitedItems()
{
	MallMgr::Instance()->ClearLimitedItem();

	UInt32 uRecords[4];
	uRecords[0] = GameParamMgr::Instance()->GetValue("mallitem_0");
	uRecords[1] = GameParamMgr::Instance()->GetValue("mallitem_1");
	uRecords[2] = GameParamMgr::Instance()->GetValue("mallitem_2");
	uRecords[3] = GameParamMgr::Instance()->GetValue("mallitem_3");
	if(SetCurNormalItems(uRecords))
	{
		GameParamMgr::Instance()->SetValue("mallitem_0", uRecords[0]);
		GameParamMgr::Instance()->SetValue("mallitem_1", uRecords[1]);
		GameParamMgr::Instance()->SetValue("mallitem_2", uRecords[2]);
		GameParamMgr::Instance()->SetValue("mallitem_3", uRecords[3]);
	}

	SetCurFestivalItems();

	for(std::vector<MallItemInfo>::iterator iterItem = m_vecGoldStoneItems.begin();
		iterItem != m_vecGoldStoneItems.end(); iterItem++)
	{
		MallMgr::Instance()->AddGoldStoneItem(*iterItem);
	}

	for(std::vector<MallItemInfo>::iterator iterItem = m_vecSilverStoneItems.begin();
		iterItem != m_vecSilverStoneItems.end(); iterItem++)
	{
		MallMgr::Instance()->AddSilverStoneItem(*iterItem);
	}

	SetGSItems();
}

bool MallScript::SetCurNormalItems(UInt32 uRecords[4])
{
	UInt32	uNow = UInt32(CURRENT_TIME.Sec());

	UInt32	uGameStartTime = GameParamMgr::Instance()->GetGameStartTime();
	Avalon::Date	date(Avalon::Time(UInt64(uGameStartTime) * 1000));
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	uGameStartTime = UInt32(date.ToTime().Sec());

	UInt32	uDays = 1;
	if(uNow >= uGameStartTime) 
	{
		uDays = (uNow - uGameStartTime) / DAY_TO_SEC + 1;
	}
	if(uDays < 3)
		return false;

	for(Int32 i = 0; i < 4; i++)
	{
		if(m_vecLimitedInfo[i].empty())
			continue;

		std::vector<UInt32>	vecList;
		vecList.clear();
		for(UInt32 uIndex = 0; uIndex < m_vecLimitedInfo[i].size(); uIndex++)
		{
			if((uRecords[i] & (UInt32(1) << uIndex)) == 0)
				vecList.push_back(uIndex);
		}

		if(vecList.empty())
		{
			UInt32 uRes = Avalon::Random::RandBetween(0, m_vecLimitedInfo[i].size() - 1);
			
			MallMgr::Instance()->AddNormalLimitedItem(m_vecLimitedInfo[i][uRes]);
			uRecords[i] = UInt32(1) << uRes;
		}
		else
		{
			std::random_shuffle(vecList.begin(), vecList.end());
			UInt32	uRes = vecList.front();

			MallMgr::Instance()->AddNormalLimitedItem(m_vecLimitedInfo[i][uRes]);
			uRecords[i] = uRecords[i] | (UInt32(1) << uRes);
		}
	}

	return true;
}

void MallScript::SetGSItems()
{
	UInt32	uNow = UInt32(CURRENT_TIME.Sec());

	UInt32	uGameStartTime = GameParamMgr::Instance()->GetGameStartTime();
	Avalon::Date	date(Avalon::Time(UInt64(uGameStartTime) * 1000));
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	uGameStartTime = UInt32(date.ToTime().Sec());

	UInt32	uDays = 1;
	if(uNow >= uGameStartTime) 
	{
		uDays = (uNow - uGameStartTime) / DAY_TO_SEC + 1;
	}
	if(uDays > 7)
		return;

	for(std::vector<MallItemInfo>::iterator iter = m_vecGSItems.begin();
		iter != m_vecGSItems.end(); iter++)
	{
		MallMgr::Instance()->AddGSLimitedItem(*iter);
	}
}

void MallScript::SetCurFestivalItems()
{
	for(std::map<std::string, std::vector<MallItemInfo> >::iterator iterItemSet = m_mapFestivalItems.begin();
		iterItemSet != m_mapFestivalItems.end(); iterItemSet++)
	{
		if(!WSActivityMgr::Instance()->IsInActivity(iterItemSet->first))
			continue;
		
		for(std::vector<MallItemInfo>::iterator iterItem = iterItemSet->second.begin();
			iterItem != iterItemSet->second.end(); iterItem++)
		{
			MallMgr::Instance()->AddFestivalLimitedItem(*iterItem);
		}
	}

}

void MallScript::DecodeItemInfo(MallItemInfo &item, const char *szItem)
{
	UInt32	uVal = 0;
	char	split;
	std::istringstream	is(szItem);

	is >> item.id >> split;
	is >> item.itemid >> split;
	is >> item.itemnum >> split;
	is >> uVal>> split;
	//item.quality = UInt8(uVal);
	is >> item.price >> split;
	is >> item.discountprice >> split;
	is >> uVal >> split;
	item.moneytype = UInt8(uVal);
	//is >> item.totalnum >> split;
	is >> item.limitnum >> split;
	is >> uVal >> split;
	//item.bBind = UInt8(uVal);
	is >> uVal >> split;
	item.bLimit = UInt8(uVal);
	uVal = 0;
	is >> uVal;
	//item.hide = UInt8(uVal);
}

void MallScript::GetGSItems(UInt32 uBuyRecord, std::list<UInt32> &listBuy)
{
	UInt32	uSize = m_vecGSItems.size();
	if(uSize < 3)
		return;

	std::vector<UInt32>	vecRest;
	for(UInt32 uIndex = 0; uIndex < uSize; uIndex++)
	{
		if((uBuyRecord & (UInt32(1) << uIndex)) == 0)
		{
			listBuy.push_back(uIndex);
			if(listBuy.size() >= 3)
				break;
		}
		else
		{
			vecRest.push_back(uIndex);
		}
	}

	if(listBuy.empty())
	{
		return;
	}
	else if(listBuy.size() < 3)
	{
		UInt32	uNum = 3 - listBuy.size();
		for(UInt32 i = 0; i < uNum; i++)
			listBuy.push_back(vecRest[i]);
	}

	for(std::list<UInt32>::iterator iter = listBuy.begin();
		iter != listBuy.end(); iter++)
	{
		*iter = m_vecGSItems[*iter].id;
	}
}

UInt32 MallScript::GetGSItemIndex(UInt32 uId)
{
	UInt32	uIndex = 0;
	for(std::vector<MallItemInfo>::iterator iter = m_vecGSItems.begin();
		iter != m_vecGSItems.end(); iter++)
	{
		if(iter->id == uId)
		{
			return uIndex;
		}

		uIndex++;
	}

	return 0;
}

void MallScript::AddMallItem(UInt8 type, UInt8 subType, UInt8 job, const char *szItem)
{
	//MallMgr::Instance()->AddMallItem(type, subType, job, szItem);
}

const char* MallScript::GetGiftDetail(UInt32 giftId)
{
	return Call<const char*>("GetGiftDetail", giftId);
}

void MallScript::OnSaleMallWaitGoods(UInt32 goodId)
{
	MallMgr::Instance()->OnSaleMallWaitGoods(goodId);
}

void MallScript::OffSaleMallWaitGoods(UInt32 goodId)
{
	MallMgr::Instance()->OffSaleMallWaitGoods(goodId);
}

