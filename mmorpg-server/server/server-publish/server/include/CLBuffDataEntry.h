#ifndef _CL_BUFF_DATAENTRY_H_
#define _CL_BUFF_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/*enum UnionEntryType
{
	// 无效
	UNION_ENTRY_INVALID,
	// 固定数值
	UNION_ENTRY_FIX,
	// 随等级自动计算（差值）
	UNION_ENTRY_GROW,
	// 各等级数值
	UNION_ENTRY_ARRAY,
};*/

/*struct UnionEntry
{
	UnionEntry()
	{
		type = UNION_ENTRY_INVALID;
		fixValue = 0;
		growValue.a = 0;
		growValue.k = 0;
	}

	UnionEntryType	type;
	UInt32		fixValue;
	struct {
		// 对应的值就是 a + k * (level - 1)
		UInt32	a;
		UInt32	k;
	}			growValue;
	std::vector<UInt32> arrayValue;

	UInt32 GetValue(UInt8 level = 1)
	{
		if (type == UNION_ENTRY_FIX)
		{
			return fixValue;
		}
		else if (type == UNION_ENTRY_GROW)
		{
			return growValue.a + (level - 1) * growValue.k;
		}
		else if (type == UNION_ENTRY_ARRAY)
		{
			if (level >= arrayValue.size())
			{
				return 0;
			}
			return arrayValue[level - 1];
		}

		return 0;
	}

	bool DecodeFromString(std::string str)
	{
		// 格式说明：
		// 固定模式：只有数字
		// 成长模式：a;k
		// 数组模式：a,b,c,d,e...

		if (str.find(';') != std::string::npos)
		{
			type = UNION_ENTRY_GROW;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ";");
			if (params.size() != 2)
			{
				return false;
			}
			growValue.a = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
			growValue.k = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
		}
		else if (str.find(',') != std::string::npos)
		{
			type = UNION_ENTRY_ARRAY;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ",");
			for (auto& param : params)
			{
				arrayValue.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
			}
		}
		else
		{
			type = UNION_ENTRY_FIX;
			fixValue = Avalon::StringUtil::ConvertToValue<UInt32>(str);
		}

		return true;
	}
};*/

/**
 *@brief buff效果参数个数
 */
#define BUFF_EFFECT_PARAM_NUM	5

/**
 *@brief buff数据项
 */
struct BuffDataEntry  : public Avalon::DataEntry
{
public:
	BuffDataEntry();
	~BuffDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//buff id
	UInt32	id;
	//buff类型
	Int16	buffType;
	//作用类型
	UInt8	workType;
	//叠加类型
	UInt8	overLay;
	//叠加上限
	UInt8	overLayLimit;
	//经验加成百分比
	UnionEntry	expAddRate;
	//金币加成百分比
	UnionEntry	goldAddRate;
	//任务掉落数量百分比
	UInt8	taskDropNumRate;
	//神器掉落百分比
	UInt8	pinkDropRate;
	//翻牌(宝箱)掉落百分比
	UInt8	chestDropRate;
	//持续时长类型
	UInt8	durationType;
	//持续时长
	UInt32	duration;
	//生效次数
	UInt32	effectTimes;
	//适用地下城
	std::vector<UInt32> applyDungeons;
	// 参数1
	Int32 param1;
	// 参数2
	Int32 param2;
};

typedef Avalon::DataEntryMgr<BuffDataEntry> BuffDataEntryMgr;

/**
*@brief buff信息数据项
*/
struct BuffInfoEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32	id;
	//buffer id
	UInt32	bufferId;
};

typedef Avalon::DataEntryMgr<BuffInfoEntry> BuffInfoEntryMgr;

#endif
