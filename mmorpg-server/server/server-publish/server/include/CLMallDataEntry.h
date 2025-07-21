#ifndef _CL_MALL_DATAENTRY_H_
#define _CL_MALL_DATAENTRY_H_

#include "CLDefine.h"
#include <CLMallDefine.h>

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief �̳ǵ���������
*/
struct MallItemDataEntry : public Avalon::DataEntry
{
public:
	MallItemDataEntry();
	~MallItemDataEntry();
	
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 48; };

public:
	// ��Ʒid
	UInt32					id;
	// ���
	UInt8					type;
	// �����
	UInt8					subType;
	// ְҵ���
	UInt8					jobType;
	// ����id
	UInt32					itemId;
	// һ������
	UInt32					groupNum;
	// ԭ��
	UInt32					price;
	// �ۿۼ�
	UInt32					disPrice;
	// ��������
	UInt8					moneyType;
	// �޹�����
	UInt8					limitType;
	// �޹�����
	UInt16					limitNum;
	// �������
	std::vector<ItemReward>	giftPackItems;
	// icon
	std::string				giftPackIcon;
	// ʱ���������
	UInt8					timeCalcType;
	// ��ʼʱ��
	UInt32					limitStartTime;
	// ����ʱ��
	UInt32					limitEndTime;
	// �·����Ϸ����·����ڼ��쿪ʼ
	std::string				startTimeFromService;
	// �·����Ϸ����·����ڼ������
	std::string				endTimeFromService;
	// �·����Ϸ������·����ڼ��쿪ʼ
	std::string				startTimeFromFirstService;
	// �·����Ϸ������·����ڼ������
	std::string				endTimeFromFirstService;
	// �������
	std::string				openInterval;
	// �رռ��
	std::string				closeInterval;
	// ���޹�����
	UInt16					limitTotalNum;
	// ������õ�vip����
	UInt16					vipScore;
	// �������
	std::string				giftPackName;
	// ��ǩ,1.����
	UInt8					tagType;
	// �������
	UInt32					sort;
	// �����������
	UInt32					hotSort;
	// ��Ʒ����
	UInt8					goodsType;
	// ��Ʒ�����
	UInt16					goodsSubType;
	// ��Ʒ״̬
	UInt8					goodsState;
	// �Ƿ��Ƽ�
	UInt8					isRecommend;
	// ˽���Ƽ�����
	UInt8					personalTailorActivateCond;
	// ������(��ǰΪ��Ӫ���ã����ֻ�����ô���1)
	UInt32					discountRate;
	// ��¼���ͱ�id
	UInt8					loginPushId;
	// ʱװ�Ƽ�ͼƬ·��
	std::string				fashionImagePath;
	// ����
	std::string				desc;
	// ����������Ϣ
	MallBuyGotInfoVec		buyGotInfos;
	// ��չ����
	std::vector<UInt32>		params;
	// ��ҵȼ�����
	UInt16					playerLevelLimit;
	// �̳���������
	UInt8					mallPushCond;
	// �˺�ˢ������
	TimeUtil::RefreshType	accountRefreshType;
	// �˺�ˢ������
	TimeUtil::RefreshDate	accountRefreshDate;
	// �˺��޹�����
	UInt32					accountLimitBuyNum;
	// �ۿ�ȯid
	UInt32					discountCouponId;
	// ���ֱ���
	UInt8                   multiple;
	// �ֿ�ȯid
	UInt32					deductionCouponId;
	// �Ƿ�������õ�ȯ
	UInt32					creditPointDeduction;
};

typedef std::vector<MallItemDataEntry*> MallItemsVec;

/**
*@brief �̳ǵ��߹�����
*/
class MallItemDataEntryMgr : public Avalon::DataEntryMgrBase<MallItemDataEntry>
	, public Avalon::Singleton<MallItemDataEntryMgr>
{
public:
	MallItemDataEntryMgr();
	~MallItemDataEntryMgr();

	bool AddEntry(MallItemDataEntry* entry);

	bool CheckGiftPack(MallItemDataEntry* entry);

	//��ȡ�̳ǵ���
	MallItemsVec& GetMallItems() { return m_MallItems; }

	//��ȡ�̳Ǽ۸���Ϣ(������
	UInt32 GetMallItemPrice(UInt32 uItemId);

private:
	// �̳ǵ��ߵļ۸���Ϣ(����)
	std::map<UInt32, UInt32>	m_mapMallItemPrice;

	//�̳ǵ���
	MallItemsVec m_MallItems;
};

#endif