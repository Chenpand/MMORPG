#ifndef __CL_EQUIP_SCHEME_DEFINE_H__
#define __CL_EQUIP_SCHEME_DEFINE_H__

#include <CLDefine.h>
#include <AvalonNetStream.h>

//装备方案类型
enum EquipSchemeType
{
	EQST_NONE	= 0,	
	EQST_EQUIP	= 1,  //装备，称号
	
};

//装备方案信息
struct EquipSchemeInfo
{
	EquipSchemeInfo()
	{
		guid = 0;
		type = 0;
		id = 0;
		weared = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & guid & type & id & weared & equips;
	}
	//唯一id
	UInt64		guid;
	//方案类型
	UInt8		type;
	//方案id
	UInt32		id;
	//是否穿戴
	UInt8		weared;
	//装备id
	std::vector<UInt64>	 equips;
};

struct RaceEquipScheme
{
	RaceEquipScheme()
	{
		type = 0;
		id = 0;
		isWear = 0;
		title = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & id & isWear & equips & title;
	}

	//方案类型
	UInt8		type;
	//方案id
	UInt32		id;
	//是否穿上
	UInt8		isWear;
	//装备
	std::vector<UInt64>	equips;
	//副武器
	//std::vector<UInt64>	assistEquip;
	//称号
	UInt64		title;
};

#endif