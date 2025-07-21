#ifndef _CL_AUCTION_MAGIC_EXPECT_UP_DATAENTRY_H_
#define _CL_AUCTION_MAGIC_EXPECT_UP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum MagicUpConsType
{
	MUCT_NORMAL = 1,  //����
	MUCT_CONSELF = 2, //ֻ�����Լ�
};

/**
*@brief �����и�ħ��������������
*/
class AuctionMagicExpectUpDataEntry : public Avalon::DataEntry
{
public:
	AuctionMagicExpectUpDataEntry() {};
	~AuctionMagicExpectUpDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };

public:
	//ΨһID
	UInt32				iD;
	//������������
	UInt8				type;
	//Ʒ��
	UInt8				quality;
	//ǿ���ȼ�����
	UInt8				strengthenLv;
	//������������
	UInt32				expectUpTimes;
};

typedef Avalon::DataEntryVisitor<AuctionMagicExpectUpDataEntry> AuctionMagicExpectUpDataEntryVisitor;

class AuctionMagicExpectUpDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionMagicExpectUpDataEntry>
	, public Avalon::Singleton<AuctionMagicExpectUpDataEntryMgr>
{
public:
	UInt32 GetExpectUpTimes(UInt8 quality, UInt8 strengthen, UInt8 type);
};

#endif