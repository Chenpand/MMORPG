#ifndef _CL_TEAM_AWARD_DATAENTRY_H_
#define _CL_TEAM_AWARD_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum TeamRewardType
{
	// ����
	TEAM_AWARD_ITEM,
	// ���㾭��
	TEAM_AWARD_RACE_END_EXP,
};

/**
*@brief ��ӽ���
*/
class TeamRewardDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32			id;
	TeamRewardType	type;
	UInt32			rewardId;
	UInt32			num;
	UInt32			prob;
	UInt32			dailyLimit;
	
};

typedef Avalon::DataEntryMgr<TeamRewardDataEntry> TeamRewardDataEntryMgr;

#endif
