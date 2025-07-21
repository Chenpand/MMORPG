#ifndef _CL_PREMIUM_LEAGUE_DATAENTRY_H_
#define _CL_PREMIUM_LEAGUE_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLPremiumLeagueDefine.h>

/**
*@brief 拍卖行栏位表
*/
class PremiumLeagueDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// 状态
	PremiumLeagueStatus		status;
	// 周
	UInt8					week;
	// 小时
	UInt8					hour;
	// 分钟
	UInt8					minute;
	// 持续时间(分钟)
	UInt8					durningMin;
};

typedef Avalon::DataEntryMgr<PremiumLeagueDataEntry> PremiumLeagueDataEntryMgr;

#endif
