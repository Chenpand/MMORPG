#include "CLHonor.h"

#include <CLHonorRewardDataEntry.h>
#include <CLMatchDefine.h>

UInt32 CLHonor::PvpHonorReward(UInt32 pvpType, UInt32 result, UInt32 rank, UInt32 kill)
{
	const HonorRewardDataEntry* rewardData = HonorRewardDataEntryMgr::Instance()->GetHonorRewardDataEntry(pvpType);
	if (NULL == rewardData)
	{
		//ErrorLogStream << "not find honor reward pvpType: " << pvpType << LogStream::eos;
		return 0;
	}

	UInt32 honor = 0;
	for (auto& it : rewardData->rewardTypeVec)
	{
		switch (it)
		{
		case HONOR_RT_RESULT:
		{
			if (result == PKRR_WIN)
				honor += rewardData->victoryReward;
			else if( result == PKRR_LOSE)
				honor += rewardData->lostReward;

			break;
		}
		case HONOR_RT_RANK:
		case HONOR_RT_WINNUM:
		{
			for (auto& iter : rewardData->rankRewardVec)
			{
				if (rank >= iter.minRank && rank <= iter.maxRank)
				{
					honor += iter.honor;
					break;
				}
			}

			break;
		}
		case HONOR_RT_KILL:
		{
			honor += rewardData->killReward * kill;
			break;
		}
		default:break;
		}
	}

	return honor;
}