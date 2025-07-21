#ifndef _CL_AUCTION_DEFINE_H_
#define _CL_AUCTION_DEFINE_H_

#include <CLDefine.h>
#include<memory>

/**
*@brief ��������
*/
enum AUCTION_ITEM_TYPE
{
	AUCTION_ITEM	= 0,	// ����
	AUCTION_GOLD	= 1,	// ��
	AUCTION_SILVER	= 2,	// ����
};

/**
*@brief ��������
*/
enum AuctionType
{
    AT_ITEM,      // ����
    AT_GOLD,      // ���
    AT_NUM,
};

// �ȼ�������
const static UInt32 LEVLE_STEP_NUM = 8;

// �������ÿ������
const static UInt32 DAILD_GOLD_AUCTION_SELL_MAX_NUM = 3;

/**
*@brief �����еȼ�ɸѡ
*/
const static UInt32 AUCTION_FILTER_LEVEL[LEVLE_STEP_NUM + 1] = {0, 29, 39, 49, 59, 69, 79, 89, 99};

/**
*@brief �����з���
*/
enum AUCTION_TYPE
{
	AUCTION_TYPE_ALL		= 0,	// ȫ��
	AUCTION_TYPE_WEAPON		= 1,	// ����
	AUCTION_TYPE_ARMOR		= 2,	// ����
	AUCTION_TYPE_ORNAMENT	= 3,	// ��Ʒ
	AUCTION_TYPE_PET		= 4,	// ����
	AUCTION_TYPE_MOUNT		= 5,	// ����
	AUCTION_TYPE_AEROBAT	= 6,	// ������
	AUCTION_TYPE_GEM		= 7,	// ��ʯ
	AUCTION_TYPE_MONEY		= 8,	// ��&����
	AUCTION_TYPE_OTHER		= 9,	// ����
};

/**
*@brief �����з���������
*/
enum AUCTION_ARMOR_SUBTYPE
{
	AUCTION_ARMOR_ALL		= 0,	// ȫ��
	AUCTION_ARMOR_CLOTHESE	= 1,	// �·�
	AUCTION_ARMOR_HANDGUARD	= 2,	// ����
	AUCTION_ARMOR_BELT		= 3,	// ����
	AUCTION_ARMOR_SHOES		= 4,	// Ь��
	AUCTION_ARMOR_LEGGUARD	= 5,	// ����
};

/**
*@biref ��������Ʒ������
*/
enum AUCTION_ORNAMENT_SUBTYPE
{
	AUCTION_ORNAMENT_ALL	= 0,	// ȫ��
	AUCTION_ORNAMENT_AMULET	= 1,	// �����
	AUCTION_ORNAMENT_TORQUE	= 2,	// ����
	AUCTION_ORNAMENT_RING	= 3,	// ��ָ
	AUCTION_ORNAMENT_BANGLE	= 4,	// ����
};

/**
*@brief �����г���������
*/
enum AUCTION_PET_SUBTYPE
{
	AUCTION_PET_ALL			= 0,	// ȫ��
	AUCTION_PET_SKILL		= 1,	// ���＼�ܵ���
	AUCTION_PET_GROWTH		= 3,	// ����ɳ�ǿ��
	AUCTION_PET_EGG			= 4,	// ���ﵰ
	AUCTION_PET_OTHER		= 5,	// �����������
};

// ÿ����ҵ����������������
const static UInt32 MAX_AUCTION_NUM = 12;

// ��������
const static UInt32 AUCTION_COST = 100;

// ����ʱ��
const static UInt32	AUCTION_LAST_TIME = 1 * DAY_TO_SEC;

// �𱴵���ID
const static UInt32 GOLD_ITEM_ID = 600000001;

// �󶨵ㄻ����ID
const static UInt32 BIND_POINT_ITEM_ID = 600000008;

// ÿ�β�ѯ��෵�ص�����
const static UInt32 MAX_AUCTION_PER_PAGE = 20;

// ÿ�ʻ��ҵ����������
const static UInt32 MAX_SELL_MONEY_NUM = 1000000;

// ��߶���
const static UInt32 MAX_AUCTION_PRICE = 1000000000;

// ����������Ѻ��
const static UInt32 AUCTION_ITEM_DEPOSIT_COST = 500;

// ������������ٽ������
const static UInt32 AUCTION_GOLD_SELL_MIN_NUM = 100 * 10000;

// ������������������
const static UInt32 AUCTION_GOLD_SELL_MAX_NUM = 1000 * 10000;

struct AuctionBaseInfo
{
    AuctionBaseInfo()
    {
        memset(this, 0, sizeof(*this));
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & guid & price & pricetype & num & itemTypeId & strengthen 
			& itemScore & publicEndTime & isTreas & owner & attent & attentNum 
			& duetime & onSaleTime & beadBuffId & isAgainOnsale & equipType & enhanceType
			& itemDueTime & enhanceNum & itemTransNum;
    }

    // Ψһid
    ObjID_t		guid;			//����������IDֱ������Ʒ��ID
    UInt32		price;
    UInt8		pricetype;
    UInt32		duetime;		//�����ĵ���ʱ��(����ʱ��)
	UInt32		onSaleTime;		//�ϼ�ʱ�䣨���Ա����˿�����ʱ�䣩(��ƷʱҲΪ��ʾ����ʱ��)

    UInt32      num;
    UInt32      itemTypeId;
    UInt32      strengthen;		// ǿ���ȼ�
	UInt32		itemScore;		// װ������

	UInt8		goodState;		// ��Ʒ״̬
	UInt8		buyState;		// ����״̬
	UInt32		onPublicTime;	// ��ʾ��ʼʱ���
	UInt32		publicEndTime;	// ��ʾ����ʱ��
	UInt8		isTreas;		// �Ƿ�����Ʒ
	UInt8		itemtype;		// ��Ʒ������
	UInt32		subType;		// ��Ʒ������
	UInt32		couponStrengthenTo;	//ǿ����+ǿ���ȼ�
	UInt32		beadBuffId;			//���鸽��buffid
	ObjID_t		owner;			// ���ҽ�ɫid
	UInt8		attent;			// ��ע[0]δ��ע [1]��ע
	UInt32		attentNum;		// ����ע����
	UInt32      attentTime;		// ��עʱ���
	UInt8		offsaleHanled;  // �Ƿ������¼ܴ���
	UInt8		isAgainOnsale;	// �Ƿ������ϼ�
	UInt8		equipType;		// װ������(EquipType):��ͨ0/����Ϣ1/����2
	UInt8		enhanceType;	// ����װ���������� ����1/����2/����3/����4
	UInt32		itemDueTime;    // ���߹���ʱ���
	UInt32		enhanceNum;		// ����װ����������ֵ
	UInt32		itemTransNum;	// �Ѿ����״���
	UInt32		auctionSubType;  //������������
	UInt8		quality;		//Ʒ��
	UInt16		itemLevel;		//��Ҫ�ȼ�
};

struct AuctionBaseCmpForItemIdSet
{
	bool operator()(const AuctionBaseInfo* left, const AuctionBaseInfo* right) const
	{
		if (!left || !right)
		{
			return false;
		}
		if (left->guid == right->guid)
		{
			return false;
		}

		if (left->num != 0 && right->num != 0 && (left->price / left->num) != (right->price / right->num))
		{
			return (left->price / left->num) < (right->price / right->num);
		}

		if (left->price != right->price)
		{
			return left->price < right->price;
		}

		return left->guid < right->guid;
	}
};

enum AuctionRefreshReason
{
	SRR_BUY,
	SRR_SELL,
	SRR_CANCEL,
	SRR_RUSY_BUY,
	SRR_SYS_RECOVERY,	//ɨ��
};

// �����е��߻�����Ϣ�����ͣ�������
struct AuctionItemBaseInfo
{
	AuctionItemBaseInfo()
	{
		itemTypeId = 0;
		num = 0;
		isTreas = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemTypeId & num & isTreas;
	}

	UInt32		itemTypeId;
	UInt32		num;
	UInt8		isTreas;
};

inline UInt32 GetAuctionStrengthAddition(UInt8 strengthen)
{
	UInt32 addition = 0;
	//UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 30, 60, 100, 150, 200, 300, 400, 500, 600, 700 };
	//UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 60, 100, 150, 200, 300, 400, 500, 600, 700 };
	UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 60, 150, 300, 400, 500, 600, 700, 800, 900 };
	UInt32 additionNum = sizeof(strengthAddition) / sizeof(strengthAddition[0]);
	if (strengthen >= sizeof(strengthAddition) / sizeof(strengthAddition[0]))
	{
		addition = strengthAddition[additionNum - 1];
	}
	else
	{
		addition = strengthAddition[strengthen];
	}

	return addition;
}

//��������Ʒ����״̬
enum AuctionGoodState
{
	AGS_INVALID, //��Ч
	AGS_PUBLIC,  //��ʾ
	AGS_ONSALE,	 //�ϼ�
	AGS_NUM,
};

//��������Ʒ����״̬
enum AuctionGoodBuyState
{
	AGBS_INVALID,			//���ܹ���
	AGBS_RUSH_BUY,			//����
	AGBS_RUSH_BUY_DELIVER,  //����������
	AGBS_NORMAL_BUY,		//��������
	AGBS_NUM,
};

enum AuctionTransactionType
{
	ATT_INVAIL = 0,
	ATT_SELL = 1, //����
	ATT_BUY = 2, //����
};

//�����н��׼�¼
struct AuctionTransaction
{
	AuctionTransaction()
	{
		type = 0;
		item_id = 0;
		item_num = 0;
		item_score = 0;
		unit_price = 0;
		verify_end_time = 0;
		enhance_type = 0;
		strength = 0;
		qualitylv = 0;
		trans_time = 0;
		beadbuffId = 0;
		mountBeadId = 0;
		mountMagicCardId = 0;
		mountMagicCardLv = 0;
		equipType = 0;
		enhanceNum = 0;

	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & item_id & item_num & item_score & unit_price & verify_end_time & enhance_type & strength 
			& qualitylv & trans_time & beadbuffId & mountBeadId & mountBeadBuffId & mountMagicCardId & mountMagicCardLv
			& equipType & enhanceNum;
	}
	UInt8		type;		//[1]:����,[2]����
	UInt32		item_id;	//��Ʒid
	UInt32		item_num;	//��Ʒ����
	UInt32		item_score; //��Ʒ����
	UInt32		unit_price; //������Ʒ����
	UInt32		verify_end_time; //����ʱ��
	UInt8       enhance_type;	//enhance����
	UInt8       strength;		//ǿ���ȼ�
	UInt8		qualitylv;		//Ʒ��
	UInt32		trans_time;		//����ʱ��
	UInt32      beadbuffId;		//����buffid(����)
	UInt32      mountBeadId;	//��Ƕ����id
	UInt32      mountBeadBuffId;  //��Ƕ����ĸ���buffid
	UInt32		mountMagicCardId; //��Ƕ��ħ��id
	UInt8		mountMagicCardLv; //��Ƕ��ħ���ȼ�
	UInt8		equipType;		  //װ������(EquipType):��ͨ0/����Ϣ1/����2
	UInt32		enhanceNum;		  //����װ����������ֵ
};

//�����������
enum AuctionRushBuyFailType
{
	ARBFT_INVAIL = 0,       //��Ч
	ARBFT_OVERTIME = 1,     //��ʱ
	ARBFT_CHECK_FAIL = 2,   //scene���ʧ��
	ARBFT_DELIVER_FAIL = 3, //����ʧ��
	ARBFT_TRANSOBJ_DES = 4,	//��������
};

//�����й�ע����
enum AuctionAttentOpType
{
	ATOT_ATTENT = 1,		//��ע
	ATOT_CANCEL = 2,		//ȡ����ע
};

//�����й�ע����
enum AuctionAttentType
{
	ATT_NOTATTENT = 0,	//�ǹ�ע
	ATT_ATTENT	  = 1,	//��ע
};

//���������۸�ħ������
struct AuctionMagicOnsale
{
	AuctionMagicOnsale()
	:strength(0), num(0){}
	AuctionMagicOnsale(UInt8 strength_, UInt32 num_)
	:strength(strength_), num(num_){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & strength & num;
	}

	UInt8  strength;
	UInt32 num;
};

#define GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE  "auction_curday_counterfee"
#define GAME_PARAM_AUCTION_YESTERDAY_COUNTERFEE "auction_yesterday_counterfee"

#define PARAM_NAME_AUCTION_RECOVE_TREAS_NUM "auction_rec_treas_num"
#define PARAM_NAME_AUCTION_RECOVE_TREAS_TIMESTAMP "auction_rec_treas_time"

#define PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM "auction_rec_nottreas_num"
#define PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_TIMESTAMP "auction_rec_nottreas_time"

enum AuctionPriceListOpType
{
	APLOP_ON_SALE = 1,	//�ϼ�
	APLOP_SWITCH = 2,	//�л�����,��ʾ���ۼ۸��б�
};

template <typename T>
inline bool CompareVectorEqual(const std::vector<T>& v1, const std::vector<T>& v2)
{
	if (v1.size() == 0 
		&& v1.size() == v2.size())
	{
		return true;
	}
	if (v1.size() != v2.size())
	{
		return false;
	}
		
	for (UInt32 i = 0; i < v1.size(); ++i)
	{
		if (v1[i] != v2[i])
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline bool FindInVector(const T& e, const std::vector<T>& v)
{
	for (UInt32 i = 0; i < v.size(); ++i)
	{
		if (v[i] == e)
		{
			return true;
		}
	}
	return false;
}

struct QueryAuctionItemNumSt
{
	QueryAuctionItemNumSt()
	{
		itemTypeId = 0;
		isTreas = 0;
		quality = 0;
		itemLevel = 0;
		num = std::make_shared<UInt32>(0);
	}
	bool operator<(const QueryAuctionItemNumSt& other) const
	{
		if (other.quality != quality)
		{
			return quality > other.quality;
		}
		if (other.itemLevel != itemLevel)
		{
			return itemLevel < other.itemLevel;
		}
		return itemTypeId < other.itemTypeId;
	}
	UInt32					itemTypeId;
	std::shared_ptr<UInt32> num;
	UInt8					isTreas;
	UInt8					quality;
	UInt16					itemLevel;
};

#endif

