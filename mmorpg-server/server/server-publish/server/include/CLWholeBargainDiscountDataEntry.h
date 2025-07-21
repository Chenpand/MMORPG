/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_WHOLEBARGAINDISCOUNT_DATA_ENTRY_H__
#define __CL_WHOLEBARGAINDISCOUNT_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class WholeBargainDiscountDataEntry : public Avalon::DataEntry
{
public:
	WholeBargainDiscountDataEntry();
	virtual ~WholeBargainDiscountDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 参数次数
	UInt32                                          joinNum;
	// 折扣(百分比)
	UInt32                                          discount;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class WholeBargainDiscountDataEntryMgr : public Avalon::DataEntryMgrBase<WholeBargainDiscountDataEntry>
	, public Avalon::Singleton<WholeBargainDiscountDataEntryMgr>
{
public:
	WholeBargainDiscountDataEntryMgr():m_MaxJoinNum(0) {}
	virtual bool AddEntry(WholeBargainDiscountDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取最大参与数
	UInt32 GetMaxJoinNum() { return m_MaxJoinNum; }

	// 获取最后折扣，根据参与次数
	UInt32 GetDiscount(UInt32 joinNum);

private:

	// 最大参与数
	UInt32 m_MaxJoinNum;

	// 折扣
	std::map<UInt32, UInt32> m_JoinDiscountMap;

// __CUSTOM_MGR_END__
};

#endif
