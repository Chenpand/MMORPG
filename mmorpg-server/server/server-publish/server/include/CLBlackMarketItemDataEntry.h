#ifndef _CL_BLACKMARKET_ITEM_DATAENTRY_H_
#define _CL_BLACKMARKET_ITEM_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ����۸�
*/
struct PriceItem 
{
	UInt32 price;
	UInt32 weight;
};

/**
*@brief ���������չ���Ʒ��
*/
class BlackMarketItemDataEntry : public Avalon::DataEntry
{
public:
	BlackMarketItemDataEntry(){};
	~BlackMarketItemDataEntry(){};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 11; };
public:
	//ΨһID
	UInt32				iD;
	//֧���չ�����
	std::vector<UInt8>	buyTypes;
	//������
	UInt8				subType;
	//��id
	UInt8				groupId;
	//��Ʒid
	UInt32				itemId;
	//�Ƽ��۸�
	UInt32				recomPrice;
	//���²����۸�
	std::vector<PriceItem>  lowerPrices;
	//���ϲ����۸�
	std::vector<PriceItem>  upperPrices;
	//�̶��չ���������
	UInt32				fixBuyLimit;
};

typedef Avalon::DataEntryVisitor<BlackMarketItemDataEntry> BlackMarketItemDataEntryVisitor;

class BlackMarketItemDataEntryMgr : public Avalon::DataEntryMgrBase<BlackMarketItemDataEntry>
	, public Avalon::Singleton<BlackMarketItemDataEntryMgr>
{
public:
	void GetDataEntrysByBuyType(UInt8 buyType, UInt8 subTye, std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>& entrys);
};

#endif