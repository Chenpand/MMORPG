#include <CLMoneyManageDataEntry.h>

bool MoneyManageDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	level = row.ReadUInt16();

	std::string split = "|";
	std::string split3 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split3);
		if (strTmp.size() < 2) continue;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		itemRewards.push_back(reward);
	}

	return true;
}

bool MoneyManageDataEntryMgr::AddEntry(MoneyManageDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MoneyManageDataEntry>::AddEntry(entry)) return false;

	if (entry->level == 0)
	{
		m_InitMoneyManage = entry;
	}

	return true;
}
