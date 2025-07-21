#ifndef _CL_SHOP_MAINFRAME_DATAENTRY_H_
#define _CL_SHOP_MAINFRAME_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief �̵��ܱ���������
*/
class ShopMainframeDataEntry : public Avalon::DataEntry
{
public:
	ShopMainframeDataEntry() {};
	~ShopMainframeDataEntry() {};
public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	//ΨһID
	UInt32	iD;

	//���̵�id
	std::vector<UInt32>	childrenShopIds;
};

typedef Avalon::DataEntryVisitor<ShopMainframeDataEntry> ShopMainframeDataVisitor;

class ShopMainframeDataEntryMgr : public Avalon::DataEntryMgrBase<ShopMainframeDataEntry>
	, public Avalon::Singleton<ShopMainframeDataEntryMgr>
{
public:
	//UInt32 getRatio(UInt32 times);
	UInt32 FindKeyByShopId(UInt32 shopId);
};

#endif