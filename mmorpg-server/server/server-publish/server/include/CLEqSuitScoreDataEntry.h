/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EQSUITSCORE_DATA_ENTRY_H__
#define __CL_EQSUITSCORE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class EqSuitScoreDataEntry : public Avalon::DataEntry
{
public:
	EqSuitScoreDataEntry();
	virtual ~EqSuitScoreDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 唯一id
	UInt32                                          id;
	// 套装类型
	UInt8                                           type;
	// 套装装备数量
	UInt32                                          suitEqNum;
	// 套装等级
	UInt32                                          suitLev;
	// 套装评分
	UInt32                                          suitScore;
// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

struct EqSuitScoreKey
{
	EqSuitScoreKey(UInt8 type_, UInt32 suitEqSum_, UInt32	suitEqLv_)
	: type(type_), suitEqSum(suitEqSum_), suitEqLv(suitEqLv_)
	{}
	bool operator<(const EqSuitScoreKey& other) const
	{
		if (other.type != type)
		{
			return type < other.type;
		}
		if (other.suitEqSum != suitEqSum)
		{
			return suitEqSum < other.suitEqSum;
		}
		return suitEqLv < other.suitEqLv;
	}
	//套装类型
	UInt8	type;
	//套装数量
	UInt32	suitEqSum;
	//等级
	UInt32	suitEqLv;
};

class EqSuitScoreDataEntryMgr : public Avalon::DataEntryMgrBase<EqSuitScoreDataEntry>
	, public Avalon::Singleton<EqSuitScoreDataEntryMgr>
{
	typedef std::map<EqSuitScoreKey, EqSuitScoreDataEntry*>  SuitScores;
public:
	virtual bool AddEntry(EqSuitScoreDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	EqSuitScoreDataEntry*	GetEntry(UInt8 type, UInt32 suitNum, UInt32 suitLv);
			
	SuitScores					suitScores;
// __CUSTOM_MGR_END__
};

#endif
