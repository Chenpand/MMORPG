#ifndef _CL_AUCTION_MAGIC_STRENG_ADDIT_DATAENTRY_H_
#define _CL_AUCTION_MAGIC_STRENG_ADDIT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum  AuctionMagicStrengType
{
	AMSTP_NONE = 0,		//��Ч
	AMSTP_MAGIC = 1,		//��ħ��
	AMSTP_STRENGTH = 2,		//װ��ǿ��
	AMSTP_ZENFU = 3,		//װ������
};

/**
*@brief �����и�ħ��ǿ���ӳ�ϵ��
*/
class AuctionMagicStrengAdditDataEntry : public Avalon::DataEntry
{
public:
	AuctionMagicStrengAdditDataEntry() {};
	~AuctionMagicStrengAdditDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };

public:
	//ΨһID
	UInt32				iD;
	//����
	UInt8				type;
	//ǿ���ȼ�����
	UInt8				strengthenLv;
	//Ʒ��
	UInt8				color;
	//ǿ���ӳ�ϵ��
	UInt32				addition;
};

typedef Avalon::DataEntryVisitor<AuctionMagicStrengAdditDataEntry> AuctionMagicStrengAdditDataEntryVisitor;

class AuctionMagicStrengAdditDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionMagicStrengAdditDataEntry>
	, public Avalon::Singleton<AuctionMagicStrengAdditDataEntryMgr>
{
public:
	UInt32 GetAddition(UInt8 color, UInt8 strengthen, UInt8 type);
};

#endif