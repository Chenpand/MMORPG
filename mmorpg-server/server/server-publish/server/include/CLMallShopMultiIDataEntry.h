/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_MALLSHOPMULTII_DATA_ENTRY_H__
#define __CL_MALLSHOPMULTII_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class MallShopMultiIDataEntry : public Avalon::DataEntry
{
public:
	MallShopMultiIDataEntry();
	virtual ~MallShopMultiIDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__
public:
	// ID
	UInt32                                          id;
	// 开始时间
	UInt32                                          startTime;
	// 结束时间
	UInt32                                          endTime;
	// 商城商品ID
	std::string                                     malls;
	// 倍数
	UInt8                                          multiple;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__:
	std::vector<UInt32> MallsVec;
// __CUSTOM_ENTRYFIELD_END__
};


class MallShopMultiIDataEntryMgr : public Avalon::DataEntryMgrBase<MallShopMultiIDataEntry>
	, public Avalon::Singleton<MallShopMultiIDataEntryMgr>
{
public:
	virtual bool AddEntry(MallShopMultiIDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	void GetMallItemMultiple(UInt32 id,UInt8 &multiple ,UInt32 &endTime);
private:
	HashMap<UInt32, std::vector<MallShopMultiIDataEntry*>> m_MallDataEntry;
// __CUSTOM_MGR_END__
};

#endif
