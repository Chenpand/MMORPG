/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_MALLFASHIONLIMITEDSUIT_DATA_ENTRY_H__
#define __CL_MALLFASHIONLIMITEDSUIT_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

const UInt8 MAX_SUIT_CNT = 2;
const UInt32 FULL_SUIT = 0b1111100;

struct MallFashionLimitedSuitKey
{
	UInt32 occu;
	UInt32 itemID;

	bool operator<(const MallFashionLimitedSuitKey &rsh) const
	{
		if (occu < rsh.occu)
		{
			return true;
		}
		else if (occu == rsh.occu)
		{
			if (itemID < rsh.itemID)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}
};
// __CUSTOM_STRUCT_END__

class MallFashionLimitedSuitDataEntry : public Avalon::DataEntry
{
public:
	MallFashionLimitedSuitDataEntry();
	virtual ~MallFashionLimitedSuitDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          occu;
	// 限定套装
	std::string                                     limitedSuit;


// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class MallFashionLimitedSuitDataEntryMgr : public Avalon::DataEntryMgrBase<MallFashionLimitedSuitDataEntry>
	, public Avalon::Singleton<MallFashionLimitedSuitDataEntryMgr>
{
public:
	virtual bool AddEntry(MallFashionLimitedSuitDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	UInt32 GetSuitID(UInt32 occu, UInt32 itemID);
	std::vector<UInt32> GetAllSuitItemID(UInt32 occu);
private:
	std::map<MallFashionLimitedSuitKey, UInt32> m_Data;
	std::map<UInt32, std::vector<UInt32>> m_OccuSuit;
// __CUSTOM_MGR_END__
};

#endif
