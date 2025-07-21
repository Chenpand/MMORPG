#include "CLWudaoRewardDataEntry.h"

bool WudaoRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	minWinNum = row.ReadUInt32();
	maxWinNum = row.ReadUInt32();

	{
		auto jarRewardStr = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(jarRewardStr, params, "|");
		if (params.size() != maxWinNum - minWinNum + 1)
		{
			return false;
		}

		for (auto param : params)
		{
			jarRewards.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
		}
	}

	return true;
}

UInt32 WudaoRewardDataEntryMgr::GetRewardJar(UInt32 winNum)
{
	class WudaoRewardVistor : public Avalon::DataEntryVisitor<WudaoRewardDataEntry>
	{
	public:
		WudaoRewardVistor(UInt32 winNum_) : winNum(winNum_), jarType(0) {}

		bool operator()(WudaoRewardDataEntry *dataEntry)
		{
			if (dataEntry && dataEntry->minWinNum <= winNum && dataEntry->maxWinNum >= winNum)
			{
				size_t index = winNum - dataEntry->minWinNum;
				if (winNum < dataEntry->minWinNum || index >= dataEntry->jarRewards.size())
				{
					jarType = 0;
				}
				else
				{
					jarType = dataEntry->jarRewards[index];
				}
				
				return false;
			}
			return true;
		}

		UInt32		winNum;
		UInt32		jarType;
	};

	WudaoRewardVistor vistor(winNum);
	this->Visit(vistor);

	return vistor.jarType;
	
}