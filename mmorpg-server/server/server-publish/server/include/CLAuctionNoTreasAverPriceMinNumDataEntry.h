#ifndef _CL_AUCTION_NOTREAS_AVERPRICE_MINNUM_DATAENTRY_H_
#define _CL_AUCTION_NOTREAS_AVERPRICE_MINNUM_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief �����з���Ʒƽ�����׼۸�����������������Ŀ
*/
class AuctionNoTreasAverPriceMinNumDataEntry : public Avalon::DataEntry
{
public:
	AuctionNoTreasAverPriceMinNumDataEntry() {};
	~AuctionNoTreasAverPriceMinNumDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	//ΨһID
	UInt32				iD;
	//�������
	UInt32				minNum;
};


typedef Avalon::DataEntryVisitor<AuctionNoTreasAverPriceMinNumDataEntry> AuctionNoTreasAverPriceMinNumDataEntryVisitor;

class AuctionNoTreasAverPriceMinNumDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionNoTreasAverPriceMinNumDataEntry>
	, public Avalon::Singleton<AuctionNoTreasAverPriceMinNumDataEntryMgr>
{
};

#endif