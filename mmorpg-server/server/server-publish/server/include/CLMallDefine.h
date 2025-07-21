#ifndef _CL_MALL_DEFINE_H_
#define _CL_MALL_DEFINE_H_

#include "CLItemDefine.h"
#include "TimeUtil.h"

enum TagType
{
	TT_HOT = 1,	// ����
	TT_WHOLE_BARGAIN = 2,	// ȫ�񿳼ۻ
};

// �̳������
enum MallSubType
{
	MST_INVALID = 0, // �����
	MST_HEAD = 1,
	MST_UPWEAR = 2,
	MST_CHEST = 3,
	MST_DOWNWEAR = 4,
	MST_BELT = 5,
	MST_FASHION_LIMIT = 8,    //�޶��̳�
};

// �̳����
enum MallType
{
	MT_INVALID,
	MT_FASHION = 4,				// ʱװ
	MT_GOLD = 5,				// ���
	MT_LIMIT_GIFT_PACK = 6,		// ��ʱ���
	MT_GIFT_PACK_ACTIVITY = 7,	// ��ʱ���(�)
	MT_FLYUP_GIFT = 20,			// �������
	MT_DEDUCTION_COUPON = 24,	// �ֿ�ȯ
};

/**
*@brief �̳ǹ�����������
*/
enum MallBuyGotType
{
	// ��
	MALL_BUY_GOT_TYPE_NONE = 0,
	// �ؾ������
	MALL_BUY_GOT_TYPE_GNOME_COIN = 1,

	MALL_BUY_GOT_TYPE_MAX,
};

/**
*@brief �̳ǹ���������Ϣ
*/
struct MallBuyGotInfo
{
	MallBuyGotInfo() :buyGotType(0), itemDataId(0), buyGotNum(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & buyGotType & itemDataId & buyGotNum;
	}

	// ������������(MallBuyGotType)
	UInt8 buyGotType;
	// ���������Ӧ�ĵ���id
	UInt32 itemDataId;
	// ������������
	UInt32 buyGotNum;
};

typedef std::vector<MallBuyGotInfo> MallBuyGotInfoVec;

/**
*@brief �̳ǵ�����Ϣ
*/
struct MallItemInfo
{
	MallItemInfo()
	{
		id = 0;
		type = 0;
		subType = 0;
		jobType = 0;
		itemid = 0;
		itemnum = 0;
		price = 0;
		discountprice = 0;
		moneytype = 0;
		bLimit = 0;
		limitnum = 0;
		goodsType = 0;
		vipScore = 0;
		timeCalcType = 0;
		startTime = 0;
		endTime = 0;
		limitTotalNum = 0;
		tagType = 0;
		sortIdx = 0;
		hotSortIdx = 0;
		goodsSubType = 0;
		isRecommend = 0;
		personalTailorActivateCond = 0;
		discountRate = 0;
		loginPushId = 0;
		playerLevelLimit = 0;
		mallPushCond = 0;
		accountRefreshType = TimeUtil::REFRESH_TYPE_NONE;
		accountLimitBuyNum = 0;
		accountRestBuyNum = 0;
		discountCouponId = 0;
		multiple = 0;
		multipleEndTime = 0;
		deductionCouponId = 0;
		creditPointDeduction = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & subType & jobType & itemid & itemnum & price & discountprice & \
			moneytype & bLimit & limitnum & goodsType & vipScore & icon & startTime & endTime & \
			limitTotalNum & rewards & giftName & tagType & sortIdx & hotSortIdx & goodsSubType & \
			isRecommend & personalTailorActivateCond & discountRate & loginPushId & fashionImagePath & \
			desc & buyGotInfos & params & accountRefreshType & accountLimitBuyNum & accountRestBuyNum & \
			discountCouponId & multiple & multipleEndTime & deductionCouponId & creditPointDeduction;
	}
	
	const std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "MallItemInfo(" << id << ", " << (UInt32)type << ", " << (UInt32)subType << ", " << (UInt32)jobType << ")";
		return oss.str();
	}

	// id
	UInt32	id;
	//���
	UInt8	type;
	//�����
	UInt8	subType;
	//ְҵ���
	UInt8	jobType;
	// ����id
	UInt32	itemid;
	// һ������
	UInt32	itemnum;
	// ԭ��
	UInt32	price;
	// �ۿۼ�
	UInt32	discountprice;
	// ��������
	UInt8	moneytype;
	// �޹�����
	UInt8	bLimit;
	// �޹�����
	UInt16	limitnum;
	//��Ʒ����
	UInt8	goodsType;
	// ������õ�vip����
	UInt16		vipScore;
	// icon
	std::string	icon;
	// ʱ���������(��Ӧö��TimeUtil::TimestampCalcType)
	UInt8		timeCalcType;
	// ��ʼʱ��
	UInt32		startTime;
	// ����ʱ��
	UInt32		endTime;
	// �������
	std::string	openInterval;
	// �رռ��
	std::string	closeInterval;
	// ���޹�����
	UInt16		limitTotalNum;
	// �������
	ItemRewardVec rewards;
	// �������
	std::string	giftName;
	// ��ǩ,1.����
	UInt8	tagType;
	// �������
	UInt32	sortIdx;
	// �����������
	UInt32	hotSortIdx;
	// ��Ʒ�����
	UInt16 goodsSubType;
	// �Ƿ��Ƽ�
	UInt8 isRecommend;
	// ˽�˶�������
	UInt8 personalTailorActivateCond;
	// ������
	UInt32 discountRate;
	// ��¼���ͱ�id
	UInt8 loginPushId;
	// ʱװ�Ƽ�ͼƬ·��
	std::string fashionImagePath;
	// ����
	std::string desc;
	// ����������Ϣ
	MallBuyGotInfoVec buyGotInfos;
	// ��չ����
	std::vector<UInt32>	params;
	// ��ҵȼ�����
	UInt16 playerLevelLimit;
	// �̳���������
	UInt8 mallPushCond;
	// �˺�ˢ������(TimeUtil::RefreshType)
	UInt8 accountRefreshType;
	// �˺�ˢ������
	TimeUtil::RefreshDate accountRefreshDate;
	// �˺��޹�����
	UInt32 accountLimitBuyNum;
	// �˺�ʣ�๺�����
	UInt32 accountRestBuyNum;
	// �ۿ�ȯid
	UInt32 discountCouponId;
	// �౶���ֱ���
	UInt8 multiple;
	// �౶����ʱ��
	UInt32 multipleEndTime;
	// �ֿ۾�id
	UInt32 deductionCouponId;
	// �Ƿ�ʹ�����õ�ȯ
	UInt32 creditPointDeduction;
};

typedef std::vector<MallItemInfo> MallItemInfosVec;


/**
*@brief �̳������ϸ
*/
struct MallGiftDetail
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & num;
	}

	UInt32 itemId;
	UInt16 num;
};

/**
*@brief �̳���������������
*/
struct MallBatchBuyCtx
{
	MallBatchBuyCtx()
	{
		m_itemUids.clear();
		m_totleNum = 0;
	}

	std::vector<UInt64>	m_itemUids;		// �Ѿ�������ĵ���uid
	UInt32 m_totleNum;					// ������������
};

/**
*@brief ������������
*/
enum ConditionActivateType
{
	// ������ʱ���
	CAT_MALL_LIMIT_TIME_GIFT_PACK = 0,
	// ����˽�˶���
	CAT_MALL_PERSONAL_TAILOR,
};

/**
*@brief �����������ݿ����״̬
*/
enum ConditionActivateDBLoadState
{
	// δ����
	CADBLS_NONE = 0,
	// �������
	CADBLS_DONE,
};

/**
*@brief �̳���ʱ�����������
*/
enum MallGiftPackActivateCond
{
	MGPAC_INVALID = 0,
	MGPAC_STRENGEN_TEN,					//ǿ����10
	MGPAC_STRENGEN_NO_RESOURCE,			//ǿ����Դ����
	MGPAC_NO_QUILTY_ADJUST_BOX,			//Ʒ�������䲻��
	MGPAC_NO_FATIGUE,					//ƣ�Ͳ��㣬���¿��ұ�������ƣ��ҩˮ
	MGPAC_NO_HELL_TICKET,				//ˢ��Ԩ��Ʊ����
	MGPAC_NO_ANCIENT_TICKET,			//ˢԶ����Ʊ����
	MGPAC_DIE,							//����
	MGPAC_STRENGEN_BROKE_TEN,			//ǿ��װ�����,����10��װ��
	MGPAC_STRENGEN_BROKE_FIFTEEN,		//ǿ��װ�����,����15��װ��
	MGPAC_STRENGEN_BROKE_TWENTY,		//ǿ��װ�����,����20��װ��
	MGPAC_STRENGEN_BROKE_TWENTY_FIVE,	//ǿ��װ�����,����25��װ��
	MGPAC_STRENGEN_BROKE_THIRTY,		//ǿ��װ�����,����30��װ��
	MGPAC_STRENGEN_BROKE_THIRTY_FIVE,	//ǿ��װ�����,����35��װ��
	MGPAC_STRENGEN_BROKE_FORTY,			//ǿ��װ�����,����40��װ��
	MGPAC_STRENGEN_BROKE_FORTY_FIVE,	//ǿ��װ�����,����45��װ��
	MGPAC_STRENGEN_BROKE_FIFTY,			//ǿ��װ�����,����50��װ��
	MGPAC_PLAYER_LEVEL_UP_GET_GIFT,		//�������������
};

/**
*@brief �̳��޹�����
*/
enum MallLimitBuyType
{
	MLBT_INVALID = 0,		//���޹�
	MLBT_DAILY_REFRESH,		//ÿ��ˢ���޹�
	MLBT_NO_REFRESH,		//��ˢ���޹�
	MLBT_WEEKLY_REFRESH,	//ÿ��ˢ���޹�
};

/**
*@brief �̳���ʱ���������Ŀ����
*/
enum MallGiftPackLimitNum
{
	MGPLN_INVALID,
	MGPLN_ONCE = 1,			//����һ��
	MGPLN_THREE_TIMES = 3,	//��������
};

/**
*@brief �̳���Ʒ����
*/
enum MallGoodsType
{
	MGT_INVALID = 0,					//��ͨ��Ʒ
	MGT_GIFT_DAILY_REFRESH,				//������������ÿ��ˢ��
	MGT_GIFT_ACTIVATE_ONCE,				//������˺ż�������һ��
	MGT_GIFT_ACTIVATE_THREE_TIMES,		//������˺ż�����������
	MGT_COMMON_CHOOSE_ONE,				//��ͨ��Ʒ���ɶ�ѡһ
	//MGT_GIFT_CHOOSE_ONE,				//������ɶ�ѡһ
};

/**
*@brief ��ҵȼ����
*/
#define PLAYER_LEVEL_INTERVAL 5

/**
*@brief ��ҵȼ�
*/
enum PlayerLevel
{
	PL_INVALID = 0,
	PL_TEN = 10,
	PL_FIFTEEN = 15,
	PL_TWENTY = 20,
	PL_TWENTY_FIVE = 25,
	PL_THIRTY = 30,
	PL_THIRTY_FIVE = 35,
	PL_FORTY = 40,
	PL_FORTY_FIVE = 45,
	PL_FIFTY = 50,
};

/**
*@brief ���۷�ĸ
*/
#define DISCOUNT_RATE_DENOMINATOR 100

//��ֵ���͹���
struct RechargePushItem
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & pushId & itemId & itemCount & buyTimes & maxTimes & price & discountPrice & validTimestamp & lastTimestamp;
	}

	UInt32 pushId;
	UInt32 itemId;
	UInt32 itemCount;
	UInt32 buyTimes;
	UInt32 maxTimes;
	UInt32 price;
	UInt32 discountPrice;
	UInt32 validTimestamp;
	UInt32 lastTimestamp;
};

typedef std::vector<RechargePushItem> RechargePushItemVec;

#endif