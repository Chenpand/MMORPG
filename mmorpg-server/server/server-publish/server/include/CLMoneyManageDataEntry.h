#ifndef _CL_MONEY_MANAGE_DATAENTRY_H_
#define _CL_MONEY_MANAGE_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

class MoneyManageDataEntry : public Avalon::DataEntry
{
public:

	MoneyManageDataEntry() { }
	~MoneyManageDataEntry() { }

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;

	//µÈ¼¶
	UInt16 level;

	//½±Àø
	std::vector<ItemReward> itemRewards;

};

class MoneyManageDataEntryMgr :public Avalon::DataEntryMgrBase<MoneyManageDataEntry>, public Avalon::Singleton<MoneyManageDataEntryMgr>
{
public:
	MoneyManageDataEntryMgr(): m_InitMoneyManage(NULL){ };
	~MoneyManageDataEntryMgr() { };

public:
	bool AddEntry(MoneyManageDataEntry* entry);

	MoneyManageDataEntry* GetInitMoneyManage() { return m_InitMoneyManage; }

private:
	MoneyManageDataEntry* m_InitMoneyManage;

};


#endif
