/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GOLDCONSIGNMENTVALUE_DATA_ENTRY_H__
#define __CL_GOLDCONSIGNMENTVALUE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//自定义头文件	
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举	
// __CUSTOM_STRUCT_BEGIN__


enum GoldConsignmentValueType
{
	GC_VALUE_AVERAGE_PRICE = 1,			// 平均交易价
	GC_VALUE_CLOSE_START_TIME = 2,		// 休市开始时间
	GC_VALUE_CLOSE_END_TIME = 3,		// 休市结束时间
	GC_VALUE_ACC_ORDER_NUM = 4,			// 账号上架订单数
	GC_VALUE_GRADE_PAGE_NUM = 5,		// 寄售显示挡位数
	GC_VALUE_UNIT_PRICE_BASE = 6,		// 单价以N万金币为单位
	GC_VALUE_NEW_SERVER_DAY = 7,		// 新服开启天数
	GC_VALUE_HANG_UP_TIME = 8,		// 挂单时间（小时）
	GC_VALUE_MIN_GOLD_NUM = 9,		// 最小寄售金币数量
	GC_VALUE_MAX_GOLD_NUM = 10,		// 最大寄售金币数量
	GC_VALUE_COMMISSIOM = 11,		// 金币寄售手续费比例（千分比）
	GC_VALUE_TRANSFER_POINT_RATE = 12,		// 信用点券兑换点券比例
	GC_VALUE_TRANSFER_POINT_DATA = 13,		// 信用点券转化日期
	GC_VALUE_MIN_PROTECT_PRICE = 14,		// 金币交易单价最低保护价
	GC_VALUE_MAX_PROTECT_PRICE = 15,		// 金币交易单价最高保护价
	GC_VALUE_PRICE_WAVE_RATE = 16,		// 金币寄售金币单价涨跌幅比例（千分比）
	GC_VALUE_RECENT_AVERAGE_PRICE_TIME = 18,// 最近平均成交价每次对比时间（秒为单位）
	GC_VALUE_CREDIT_POINT_SHOW_DAY = 20,// 信用点券记录显示天数
	GC_VALUE_RECENT_PRICE_NODE = 21,// 最近平均成交价计算节点数
	GC_VALUE_TRADE_UNIT_BASE = 22,// 交易单价基数
	GC_VALUE_TRADE_RECORD_SHOW_TIME = 23,// 交易记录显示时间
};

// __CUSTOM_STRUCT_END__

class GoldConsignmentValueDataEntry : public Avalon::DataEntry
{
public:
	GoldConsignmentValueDataEntry();
	virtual ~GoldConsignmentValueDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口	
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	//ID
	UInt32                                        id;
	//名字
	std::string                                   name;
	//数值
	UInt32                                        value;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GoldConsignmentValueDataEntryMgr : public Avalon::DataEntryMgrBase<GoldConsignmentValueDataEntry>
	, public Avalon::Singleton<GoldConsignmentValueDataEntryMgr>
{
public:
	virtual bool AddEntry(GoldConsignmentValueDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	void OnReload();

	UInt32 GetValue(UInt32 type);

	bool IsCheckZeroValue(UInt32 id);

private:

	std::map<UInt32, UInt32> m_ValueMap;

// __CUSTOM_MGR_END__
};

#endif
