#ifndef _CL_GIFTBAG_DATAENTRY_H_
#define _CL_GIFTBAG_DATAENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDefine.h"
#include "CLItemDefine.h"

/**
 *@brief �����
 */
class GiftBagDataEntry : public Avalon::DataEntry
{
public:
	GiftBagDataEntry();
	~GiftBagDataEntry();

	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//���id
	UInt32	id;
	//�������
	std::string name;
	//����
	std::vector<ItemReward>	rewards;
	//����1
	UInt32	param1;
	//����2
	UInt32	param2;
	//����3
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
	 *@brief ��ȡ�������
	 */
	const GiftBagDataEntry* GetOnlineGiftbag(UInt32 time) const;

	/**
	 *@brief ��ȡ��һ���������
	 */
	const GiftBagDataEntry* GetNextOnlineGiftbag(UInt32 bagTime, UInt32& nextBagTime, UInt32& leftTime) const;

	/**
	 *@brief ��ȡ�ȼ����
	 */
	const GiftBagDataEntry* GetLevelGiftBag(UInt16 level) const;

	/**
	 *@brief ��ȡ��һ���ȼ����
	 */
	const GiftBagDataEntry* GetNextLevelGiftbag(UInt16 level, UInt16& nextLevel) const;

private:
	//�������
	GiftbagMap	m_OnlineGiftbags;
	//�ȼ����
	GiftbagMap	m_LevelGiftbags;
};

#endif
