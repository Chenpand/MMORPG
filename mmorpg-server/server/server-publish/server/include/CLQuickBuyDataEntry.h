#ifndef _CL_QUICK_BUY_DATAENTRY_H_
#define _CL_QUICK_BUY_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ���ٹ���������
*/
class QuickBuyDataEntry : public Avalon::DataEntry
{
public:
	QuickBuyDataEntry() {}
	~QuickBuyDataEntry() {}

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// ����id
	UInt32	id;
	// ����ID
	UInt32	costItemId;
	// ��������
	UInt32	costNum;
	// ���ֱ���
	UInt8 multiple;
};

typedef Avalon::DataEntryMgr<QuickBuyDataEntry> QuickBuyDataEntryMgr;

#endif
