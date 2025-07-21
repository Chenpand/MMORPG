#include "PlayerScript.h"
#include "Player.h"

PlayerScript::PlayerScript(Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
}

PlayerScript::~PlayerScript()
{
	for(RegexVec::iterator iter = m_Regexes.begin();
		iter != m_Regexes.end(); ++iter)
	{
		delete *iter;
	}
	m_Regexes.clear();
}

void PlayerScript::SetDailyTaskNpc(const char* npcs)
{
	UInt32 npcId = 0;
	char split = 0;
	std::istringstream iss(npcs);
	while((iss >> npcId) && npcId != 0)
	{
		iss >> split;
		m_DailyTaskNpcs.push_back(npcId);
		npcId = 0;
	}
}

bool PlayerScript::IsDailyTaskNpc(UInt32 npcId) const
{
	for(std::vector<UInt32>::const_iterator iter = m_DailyTaskNpcs.begin();
		iter != m_DailyTaskNpcs.end(); ++iter)
	{
		if(*iter == npcId) return true;
	}
	return false;
}

void PlayerScript::Init()
{
	Call<void>("Init", this);
}

void PlayerScript::OnBirth(Player* player)
{
	Call<void>("OnBirth", player);
}

void PlayerScript::OnOnline(Player* player, UInt64 offlineTime)
{
	Call<void>("OnOnline", player, offlineTime);
}

UInt8 PlayerScript::GetDailyTaskStatus(Player* player, UInt32 npcId)
{
	return Call<UInt8>("GetDailyTaskStatus", player, npcId);
}

void PlayerScript::OnDayChanged(Player* player)
{
	Call<void>("OnDayChanged", player);
}

void PlayerScript::OnLevelup(Player* player)
{
	Call<void>("OnLevelup", player);
}

void PlayerScript::OnDailyTaskAccepted(Player* player, BaseTask* task)
{
	Call<void>("OnDailyTaskAccepted", player, task);
}

void PlayerScript::OnDailyTaskSubmitted(Player* player, BaseTask* task)
{
	Call<void>("OnDailyTaskSubmitted", player, task);
}

void PlayerScript::OnConsumeGold(Player* player, UInt32 itemId, UInt16 num, UInt32 cost)
{
	Call<void>("OnConsumeGold", player, itemId, num, cost);
}

void PlayerScript::OnConsumePoint(Player* player, UInt32 itemId, UInt16 num, UInt32 cost)
{
	Call<void>("OnConsumePoint", player, itemId, num, cost);
}

bool PlayerScript::SetForbidPattern(const char* pattern)
{
	if(pattern == NULL) return false;

	Avalon::Regex* regex = new Avalon::Regex();
	if(!regex->Compile(pattern))
	{
		ErrorLogStream << "compile regex(" << pattern << ") failed!" << LogStream::eos;
		CL_SAFE_DELETE(regex);
		return false;
	}

	m_Regexes.push_back(regex);
	return true;
}

bool PlayerScript::CheckWordForbid(const char* word, int wordlen)
{
	int offsets[30];
	for(RegexVec::iterator iter = m_Regexes.begin();
		iter != m_Regexes.end(); ++iter)
	{
		Avalon::Regex* regex = *iter;
		if(regex->Match(word, wordlen, offsets, 30) >= 0)
		{
			return true;
		}
	}
	return false;
}

void PlayerScript::SetExchangeItem(UInt32 pos, UInt32 itemId, UInt8 quality, const char* secitems)
{
	ItemExchangeInfo info;
	info.itemId = itemId;
	info.quality = quality;

	std::istringstream iss(secitems);
	UInt32 srcItemId = 0;
	UInt16 srcItemNum = 0;
	char split = 0;
	while((iss >> srcItemId) && srcItemId != 0)
	{
		iss >> split >> srcItemNum >> split;
		info.items.insert(std::make_pair(srcItemId, srcItemNum));
	}
	m_ExchangeItems[pos].push_back(info);
}

const PlayerScript::ExchangeItemVec& PlayerScript::GetExchangeItems(UInt32 pos) const
{
	static ExchangeItemVec itemVec;
	ExchangeItemMap::const_iterator iter = m_ExchangeItems.find(pos);
	if(iter != m_ExchangeItems.end()) return iter->second;
	return itemVec;
}

void PlayerScript::CheckLoginLimitItem(Player *pPlayer, Item *pItem)
{
	Call<void>("CheckLoginLimitItem", pPlayer, pItem);
}

void PlayerScript::OnPassedCopyScene(Player *pPlayer, UInt32 mapId)
{
	Call<void>("OnPassedCopyScene", pPlayer, mapId);
}

void PlayerScript::OnUseVipCard(Player *pPlayer, UInt32 uItemId)
{
	Call<void>("OnUseVipCard", pPlayer, uItemId);
}

void PlayerScript::OnChijiBirth(Player* player)
{
	Call<void>("OnChijiBirth", player);
}

bool PlayerScript::IsMallPanicBuyId( UInt32 mallId )
{
	MallPanicBuySet::iterator iter = m_MallPanicBuySet.find(mallId);
	if(iter == m_MallPanicBuySet.end()) return false;
	return true;
}

void PlayerScript::AddEquipShopPrice(UInt32 uLevel, UInt32 uQuality, UInt32 uPrice)
{
	m_mapEquipShopPrice[(uLevel << 16) + uQuality] = uPrice;
}

void PlayerScript::AddEquipShopRefreshTime(UInt32 uHour)
{
	m_vecRefreshTimes.push_back(uHour);
}

UInt32 PlayerScript::GetEquipShopPrice(UInt32 uLevel, UInt32 uQuality)
{
	std::map<UInt32, UInt32>::iterator	iter = m_mapEquipShopPrice.find((uLevel << 16) + uQuality);
	if(iter == m_mapEquipShopPrice.end())
		return 0;

	return iter->second;
}

bool PlayerScript::NeedRefreshEquipShop(const Avalon::Time &now, UInt32 uLastRefreshTime)
{
	UInt32	uNow = UInt32(now.Sec());
	Avalon::Date	today(now);
	today.Min(0);
	today.Sec(0);

	for(std::vector<UInt32>::iterator iter = m_vecRefreshTimes.begin();
		iter != m_vecRefreshTimes.end(); iter++)
	{
		today.Hour(*iter);

		UInt32	uRefreshTime = UInt32(today.ToTime().Sec());
		if(uLastRefreshTime < uRefreshTime && uNow >= uRefreshTime)
		{
			return true;
		}
	}

	return false;
}

UInt32 PlayerScript::GetNextRefreshEquipTime(UInt32 uLastRefreshTime)
{
	Avalon::Date	today(CURRENT_TIME);
	today.Min(0);
	today.Sec(0);

	for(std::vector<UInt32>::iterator iter = m_vecRefreshTimes.begin();
		iter != m_vecRefreshTimes.end(); iter++)
	{
		today.Hour(*iter);

		UInt32	uRefreshTime = UInt32(today.ToTime().Sec());
		if(uLastRefreshTime < uRefreshTime)
		{
			return uRefreshTime;
		}
	}

	today.Hour(m_vecRefreshTimes.front());
	UInt32	uTime = UInt32(today.ToTime().Sec()) + 24 * 60 * 60;
	return uTime;
}
