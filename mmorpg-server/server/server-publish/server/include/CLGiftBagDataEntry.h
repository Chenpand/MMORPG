#ifndef _CL_GIFTBAG_DATAENTRY_H_
#define _CL_GIFTBAG_DATAENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDefine.h"
#include "CLItemDefine.h"

/**
 *@brief 礼包表
 */
class GiftBagDataEntry : public Avalon::DataEntry
{
public:
	GiftBagDataEntry();
	~GiftBagDataEntry();

	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//礼包id
	UInt32	id;
	//礼包名称
	std::string name;
	//内容
	std::vector<ItemReward>	rewards;
	//参数1
	UInt32	param1;
	//参数2
	UInt32	param2;
	//参数3
	UInt32	param3;
};

class GiftBagDataEntryMgr : public Avalon::DataEntryMgrBase<GiftBagDataEntry>
	, public Avalon::Singleton<GiftBagDataEntryMgr>
{
	typedef std::map<UInt32, GiftBagDataEntry*>	GiftbagMap;
public:
	GiftBagDataEntryMgr();
	~GiftBagDataEntryMgr();

public:
	bool AddEntry(GiftBagDataEntry* dataEntry);

public:
	/**
	 *@brief 获取在线礼包
	 */
	const GiftBagDataEntry* GetOnlineGiftbag(UInt32 time) const;

	/**
	 *@brief 获取下一个在线礼包
	 */
	const GiftBagDataEntry* GetNextOnlineGiftbag(UInt32 bagTime, UInt32& nextBagTime, UInt32& leftTime) const;

	/**
	 *@brief 获取等级礼包
	 */
	const GiftBagDataEntry* GetLevelGiftBag(UInt16 level) const;

	/**
	 *@brief 获取下一个等级礼包
	 */
	const GiftBagDataEntry* GetNextLevelGiftbag(UInt16 level, UInt16& nextLevel) const;

private:
	//在线礼包
	GiftbagMap	m_OnlineGiftbags;
	//等级礼包
	GiftbagMap	m_LevelGiftbags;
};

#endif
