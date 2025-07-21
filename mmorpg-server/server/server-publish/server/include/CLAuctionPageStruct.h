#ifndef _CL_AUCTION_PAGESTRUCT_DATAENTRY_H_
#define _CL_AUCTION_PAGESTRUCT_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 拍卖行结构表
*/
class AuctionPageStructDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// 公示商品对应子页签是否显示
	UInt8  bPubDisplay;
};

class AuctionPageStructDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionPageStructDataEntry>, public Avalon::Singleton<AuctionPageStructDataEntryMgr>
{
public:
	AuctionPageStructDataEntryMgr() {};
	~AuctionPageStructDataEntryMgr() {};

public:
	bool AddEntry(AuctionPageStructDataEntry* entry);

	void OnAddEntry(AuctionPageStructDataEntry* entry);
	
	std::vector<UInt32>& GetPubDisplayIds() { return m_PubDisplayIds; }
private:
	std::vector<UInt32> m_PubDisplayIds;
};


#endif
