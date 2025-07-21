#ifndef _CL_PREMIUM_LEAGUE_DATAENTRY_H_
#define _CL_PREMIUM_LEAGUE_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLPremiumLeagueDefine.h>

/**
*@brief ��������λ��
*/
class PremiumLeagueDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// ״̬
	PremiumLeagueStatus		status;
	// ��
	UInt8					week;
	// Сʱ
	UInt8					hour;
	// ����
	UInt8					minute;
	// ����ʱ��(����)
	UInt8					durningMin;
};

typedef Avalon::DataEntryMgr<PremiumLeagueDataEntry> PremiumLeagueDataEntryMgr;

#endif
