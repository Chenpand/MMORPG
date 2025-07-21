#ifndef __NEW_AUCTION_MGR_H__
#define __NEW_AUCTION_MGR_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include "WSItem.h"
#include <CLAuctionDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLAuctionProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <deque>
#include <CLAuctionSysValueDataEntry.h>

class WSPlayer;
class AuctionTransRecord;
// ������������ٽ��
#define GOLD_BASE_NUM 1000000
// ƽ���۸���Ҫ������ɽ��۴���
#define RECENT_PRICE_NUM 10
// �������ǿ���ȼ��ĵ��߶����¼����ƽ���۸�
#define AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL 10
// �쳣���׻Ḵ�Ƶ��ߵ���ߵȼ�
#define AUCTION_ABNORMAL_COPY_ITEM_MAX_STRENGTHEN 11

// ����װ���������,С��11������ƽ�����׼���������
#define  AUCTION_ZENFU_AVERAGE_SPAN	4

// �������߼۸�һֱ����
#define  AUCTION_ZENFU_AVERAGE_STR_LEV 6

namespace zjy_auction {

struct AuctionNumCache
{
	AuctionNumCache()
	{
		expireTime = 0;
	}
	CLProtocol::AuctionQueryCondition cond;
	CLProtocol::WorldAuctionItemNumRes res;
	UInt32		expireTime;
};


struct AuctionRecommendPriceKey
{
	AuctionRecommendPriceKey(UInt32 id_, UInt8 strengthen_, UInt32 beadBuffId_, UInt8 enhanceType_)
	: typeId(id_), strengthen(strengthen_), beadBuffId(beadBuffId_), enhanceType(enhanceType_){}

	bool operator<(const AuctionRecommendPriceKey& other) const
	{
		if (other.typeId != typeId)
		{
			return typeId < other.typeId;
		}
		if (other.strengthen != strengthen)
		{
			return strengthen < other.strengthen;
		}
		if (other.beadBuffId != beadBuffId)
		{
			return beadBuffId < other.beadBuffId;
		}

		return enhanceType < other.enhanceType;
	}

	UInt32	typeId;
	UInt8	strengthen;
	UInt32	beadBuffId;
	UInt8	enhanceType; //����װ����������(EnhanceType)
};

// ������Ʒ������
enum AuctionMainItemType
{
    // ��Ч
    AMIT_INVALID,
    // ����
    AMIT_WEAPON,
    // ����
    AMIT_ARMOR,
    // ����
    AMIT_JEWELRY,
    // ����Ʒ
    AMIT_COST,
    // ����
    AMIT_MATERIAL,
    // ����
    AMIT_OTHER,
    // ����
    AMIT_NUM
};

struct AuctionRushBuyPlayer 
{
	AuctionRushBuyPlayer()
	:playerId(0), buyNum(0)
	{}
	AuctionRushBuyPlayer(ObjID_t playerId_, UInt32 buyNum_)
	: playerId(playerId_), buyNum(buyNum_)
	{
	}
	ObjID_t playerId;
	UInt32  buyNum;
};

struct AuctionInfo : public AuctionBaseInfo
{
    AVALON_DEFINE_OBJECTPOOL(AuctionInfo, Avalon::NullMutex)

    AuctionInfo()
    {
        type = 0;
		account = 0;
		//owner = 0;
        counterFee = 0;
        item = NULL;
		itemSrcGuid = 0;
		mainType = AMIT_INVALID;
		ownerVipLev = 0;
		auctionAgaining = false;
		isRecoveHandled = 0;
		isRecordInOnsaleNum = 0;
    }

    ~AuctionInfo()
    {
		InfoLogStream << "delete auction object, guid : " << guid << "item type id : " << itemTypeId
			<< ", address : " << (UInt64)this << LogStream::eos;
        if (item)
        {
            delete item;
			item = NULL;
        }
    }
    
	UInt8 GetEnhanceType() const;

    UInt8		type;
	// �����˺�
	UInt32		account;
	// ���ҽ�ɫ
    //ObjID_t		owner;
	// ����vip�ȼ�
	UInt32		ownerVipLev;
    // ������
    UInt32      counterFee;
    WSItem*     item;
	ObjID_t		itemSrcGuid;
	AuctionMainItemType	mainType;
	// ����IP
	std::string	ip;
	// �����豸��
	std::string deviceId;
	// �������
	std::vector<AuctionRushBuyPlayer> rushBuyPlayers;
	//  �Ƿ��������ϼ���
	UInt8  auctionAgaining;
	//  �Ƿ�����ɨ������
	UInt8  isRecoveHandled;
	//  �Ƿ��Ѿ�������������(��Ը�ħ��)
	UInt8  isRecordInOnsaleNum;
};

enum AuctionSortType
{
    AST_PRICE_ASC,  // ���۸�����
    AST_PRICE_DESC, // ���۸���
};

enum AuctionSellDuration
{
    ASD_24_HOUR,    // 24Сʱ
    ASD_48_HOUR,    // 48Сʱ
    ASD_NUM,
};

enum AuctionSendItemType
{
    ASIT_BUY,       // ����
    ASIT_CANCEL,    // �¼�
    ASIT_TIMEOUT,   // ��ʱ
};

struct AuctionRecentPrice
{
    AuctionRecentPrice()
    {
        memset(this, 0, sizeof(*this));
    }

    UInt32      itemTypeID;
    UInt32      averPrice;
    UInt32      num;
};

struct AuctionItemRecentPrice
{
	AuctionItemRecentPrice(UInt32 price_, UInt32 num_, UInt32 timestamp_, UInt32 qualitylv_, UInt32 beadAverPrice_, UInt32 magicCardAverPrice_)
	: price(price_), num(num_), timestamp(timestamp_), qualitylv(qualitylv_), beadAverPrice(beadAverPrice_), magicCardAverPrice(magicCardAverPrice_)
	{
	}

	UInt32	price;			//�ܼ�
	UInt32  num;			//����
	UInt32	timestamp;
	//UInt32	mountedBeadItemId;		//����ʱ��Ƕ�ı���itemid
	//UInt32	mountedBeadItemBuffId;	//����ʱ��Ƕ�ı���ĸ���buffid
	//UInt32  mountedMagicCardItemId; //����ʱ��ħ��itemid
	UInt32	qualitylv;					//����ʱ��װ��Ʒ��
	UInt32	beadAverPrice;				//����ʱ����Ƕ�����ƽ�����׼۸�
	UInt32  magicCardAverPrice;			//����ʱ��ħ����ƽ�����׼۸�
};

struct AuctionRecommendPrice
{
    AVALON_DEFINE_OBJECTPOOL(AuctionRecommendPrice, Avalon::NullMutex)
	AuctionRecommendPrice() : guid(0), itemTypeID(0), strengthen(0), bead_buff_id(0),
	price(0), price_update_time(0), visibility_price(0), vis_price_update_time(0){}

	std::string GetRecentPriceStr()
	{
		std::stringstream ss;
		for (auto recent : recentPrice)
		{
			ss << recent.price << "," << recent.num << "," << recent.timestamp << "," << recent.qualitylv << ","
				<< recent.beadAverPrice << "," << recent.magicCardAverPrice << "|";
		}

		return ss.str();
	}

	bool UpdateVisPrice()
	{
		Avalon::Date curr_date = CURRENT_DATE;
		Avalon::Date vis_price_date((UInt64)this->vis_price_update_time * 1000);
		curr_date.Hour(0);
		curr_date.Min(0);
		curr_date.Sec(0);
		curr_date.MSec(0);
		vis_price_date.Hour(0);
		vis_price_date.Min(0);
		vis_price_date.Sec(0);
		vis_price_date.MSec(0);
		
		if (vis_price_date.ToTime().Sec() != curr_date.ToTime().Sec())
		{
			this->visibility_price = this->price;
			this->vis_price_update_time = (UInt32)curr_date.ToTime().Sec();
			return true;
		}
		return false;
	}

	void ForceUpdateVisPrice(UInt32 price);

	void AddRecentPrice(UInt32 price, UInt32 num, UInt32 timestamp, UInt32 quality, UInt32 beadAverPrice, UInt32 magicCardAverPrice, UInt8 istrea = 0)
	{
		UInt32 max_times = 100;
		if (istrea)
		{
			max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_TREAN_CALC_AVERPRICE_MAXTIMES);
		}
		else
		{
			max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES);
		}
		if (max_times == 0) max_times = 100;
		 
		recentPrice.push_back(AuctionItemRecentPrice(price, num, timestamp, quality, beadAverPrice, magicCardAverPrice));
		if (recentPrice.size() > max_times)
		{
			for (int i = recentPrice.size() - max_times; i > 0; --i)
			{
				recentPrice.pop_front();
			}
		}
	}

	void UpdatePrice(bool isTreas);

	/**
	*@brief ��ȡ������Ʒװ�����׼۸�Mϵ��
	*/
	static void GetCalcTransMPriceRate(UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate, UInt32& guarantee_rate, UInt32& grade_uplimint);

	/**
	*@brief ��ȡ������Ʒװ�����׼۸�Mϵ��
	*/
	static void GetCalcTransMPriceRateNew(AuctionInfo& auctionInfo, UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate,
		UInt32& guarantee_rate, UInt32& grade_uplimint,UInt32& cmp_bead_rate, UInt32& cmp_magic_rate);

	/**
	*@brief ������Ʒװ�����׼۸�M
	*/
	/*static UInt32 CalcTransMPrice(UInt32 price, UInt32 num, UInt32 qualitylv, UInt32 beadAverPrice, UInt32 magicCardAverPrice,
		UInt32 bead_averprice_rate, UInt32 magiccard_averprice_rate, UInt32 guarantee_rate, UInt32 grade_uplimint);*/

	/**
	*@brief ������Ʒװ��(��װ, û����Ƕ����,��ħ��)���׼۸�M
	*/
	static UInt32 CalcTransEqBareMPrice(UInt32 price, UInt32 qualitylv, UInt32 guarantee_rate, UInt32 grade_uplimint);

	/*
	*@brief ������Ʒװ������Ƕ�˱����ħ�������׼۸�M
	*/
	static UInt32 CalcTransEqMountedMPrice(AuctionInfo& auctionInfo, UInt32 beadAverPrice, UInt32 magicCardAverPrice,
		UInt32 bead_averprice_rate, UInt32 magiccard_averprice_rate, UInt32 guarantee_rate, UInt32 grade_uplimint,
		UInt32 cmp_bead_rate, UInt32 cmp_magic_rate);

    ObjID_t					guid;
    UInt32					itemTypeID;
	UInt8					strengthen;
	UInt32					bead_buff_id;
	UInt8					enhanceType; //����װ����������
    UInt32					price;
	UInt32					price_update_time;
	// ����ɽ��۸�
	std::deque<AuctionItemRecentPrice> recentPrice;
	UInt32					visibility_price;
	UInt32					vis_price_update_time;
};

struct AuctionPriceSt
{
	ObjID_t  guid;
	UInt32   unitPrice;
};

struct AuctionPricesKey
{
	AuctionPricesKey(UInt32 id_, UInt8 strengthen_, UInt32 beadBuffId_, UInt8 isRedEquip_, UInt8 goodState_)
	: itemTypeId(id_), strengthen(strengthen_), beadBuffId(beadBuffId_), isRedEquip(isRedEquip_), goodState(goodState_){}

	bool operator<(const AuctionPricesKey& other) const
	{
		if (other.itemTypeId != itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}
		if (other.strengthen != strengthen)
		{
			return strengthen < other.strengthen;
		}
		if (other.goodState != goodState)
		{
			return goodState < other.goodState;
		}
		if (other.isRedEquip != isRedEquip)
		{
			return isRedEquip < other.isRedEquip;
		}
		return goodState < other.goodState;
	}

	UInt32	itemTypeId;
	UInt32	strengthen;
	UInt32  beadBuffId;
	UInt8	isRedEquip;
	UInt8   goodState;
};

struct AuctionPricesSt
{
	AuctionPricesSt()
	{
		timeStmap = 0;
	}
	CLProtocol::WorldAuctionQueryItemPricesRes res;
	UInt32 timeStmap;
};

struct AuctionPricesStForSwtich
{
	AuctionPricesStForSwtich()
	{
		timeStmap = 0;
	}
	CLProtocol::WorldAuctionQueryItemPriceListRes res;
	UInt32 timeStmap;
};

struct MagicOnsaleKey
{
	MagicOnsaleKey(UInt32 id_, UInt8 strengthen_)
	: itemTypeId(id_), strengthen(strengthen_){}

	bool operator<(const MagicOnsaleKey& other) const
	{
		if (other.itemTypeId != itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}
		
		return strengthen < other.strengthen;
	}

	UInt32	itemTypeId;
	UInt8	strengthen;
};



class AuctionMgr : public Avalon::Singleton<AuctionMgr>
{
    typedef HashMap<ObjID_t, AuctionInfo*> MapAuction;
	typedef std::map<UInt32, UInt32> MapPriceNum;
	typedef std::map<AuctionPricesKey, AuctionPricesSt> MapAuctionPrices;
	typedef std::map<AuctionPricesKey, AuctionPricesStForSwtich> MapAuctionPricesForSwtich;
	
	typedef std::map<MagicOnsaleKey, UInt32>	MapMagicOnsaleNum;
	typedef std::map<UInt8, UInt32> MapMagicMinAverPrices;

	static const UInt32 max_rush_buy_time = 5;
public:
    AuctionMgr();
    ~AuctionMgr();

    void Init();
    void OnTick(const Avalon::Time &now);

	/**
	*@ ����������Ʒ״̬
	*/
	void UpdateTreasState(const Avalon::Time &now);
	
	/**
	*@ ���µ���������Ʒ״̬
	*/
	bool UpdateOneTreasState(AuctionInfo* pAuctionInfo, const Avalon::Time &now, bool& earse, bool& recoved, bool canDelete = false);

	/**
	*@ �������ε���
	*/
	void AddShieldItem(UInt32 itemId);

	/**
	*@ ������ε���
	*/
	void ClearShieldItem();

public:
    /**
    *@brief ����
    * type : AuctionType
    */
	bool OnPlayerAddItemToAuction(WSPlayer* player, AuctionType type, ObjID_t id, UInt32 typeId,
		UInt32 num, UInt32 price, AuctionSellDuration duration, UInt8 strength, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief �����ϼ�
	*/
	bool OnPlayerAddItemToAuctionAgain(WSPlayer* player, AuctionType type, ObjID_t auctionGuid, 
		UInt32 num, UInt32 price, UInt8 strength, UInt32 buffId, UInt8 enhanceType);

    /**
    *@brief ��ѯ��������Ʒ
    * type : AuctionType
    */
    bool OnPlayerQueryAuctionList(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);

	/**
	*@brief ����������ȡ��������Ʒ������������ItemTypeID���飩
	* type : AuctionType
	*/
	bool OnPlayerQueryAuctionItemNum(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief ��ѯ������Ʒ����ϸ��Ϣ
    */
    bool OnPlayerQueryAuctionItem(WSPlayer* player, ObjID_t id);

    /**
    *@brief ����������Ʒ
    */
    bool OnPlayerBuyAuctionItem(WSPlayer* player, ObjID_t id, UInt32 num, bool rushBuy = false);

	/**
	*@brief ������Ʒ��Ʒ
	*/
	bool OnPlayerRusyBuyAuctionItem(WSPlayer* player, ObjID_t guid, UInt32 num);

    /**
    *@brief ��������
    */
    bool OnPlayerCancelAuction(WSPlayer *player, ObjID_t id);
	bool OnSceneAuctionCheckPackageRet(WSPlayer *player, ObjID_t id);

    /**
    *@brief �����Լ���������Ʒ�б�
    */
    bool OnPlayerLoadSelfAuctionList(WSPlayer *player);

    /**
    *@brief ��ѯ�Լ���������Ʒ�б�
    */
    bool OnPlayerQuerySelfAuctionList(WSPlayer *player, UInt8 type);

    /**
    *@brief ��ѯ�Լ���������Ʒ�б�
    */
    bool OnPlayerQueryRecommendPrice(WSPlayer *player, AuctionType type, UInt32 itemTypeId);

	/**
	*@brief ��ѯ���߽��ڽ��׼۸�
	*/
	bool OnPlayerQueryItemRecentPrices(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType);

	/**
	*@breid ��ѯ����������׼۸�
	*/
	bool OnPlayerQueryItemRecentPricesList(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
public:
    /**
    *@brief ���ݿⷵ������������Ϣ
    */
    bool OnSelectAuctionRet(CLRecordCallback *pCallback);

    /**
    *@brief ���ݿⷵ���Ƽ��۸���Ϣ
    */
    bool OnSelectAuctionRecommendPriceRet(CLRecordCallback *pCallback);

    /**
    *@brief ���ݿⷵ�س�ʱ����Ʒ
    */
    bool OnSelectTimeoutAuctionRet(CLRecordCallback *pCallback);

    /**
    *@brief ���ݿⷵ������������Ϣ
    */
    bool OnQueryPlayerAuctionListRet(ObjID_t roleId, std::vector<std::pair<UInt8, ObjID_t> >& auctionList);

    /**
    *@brief ���ݿ��������
    */
	void OnQueryAuctionRet(ObjID_t roleId, UInt8 type, UInt16 curPage, UInt16 maxPage, std::vector<ObjID_t>& auctionIds, UInt8 goodState, UInt8 attent);

    /**
    *@brief SceneServer���ش���������Ʒ��Ϣ
    */
    void OnAuctionCheckRet(Avalon::NetInputStream &stream);

    /**
    *@brief SceneServer���ؿ۳���������
    */
    bool OnBuyAuctionCheckRet(WSPlayer *pPlayer, ObjID_t id, UInt32 num);

	/**
	*@brief ���ݿⷵ��ĳ����Ʒ������ͼ۸��б�
	*/
	bool OnQueryAuctionSaleLisRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32   beadbuffid, UInt8 enhanceType);
	
	/**
	*@brief ���ݿⷵ��ĳ����Ʒ������ͼ۸��б�
	*/
	bool OnQueryAuctionSalePriceListRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
public:
	/**
	*@brief ���ݽ�ɫID��ȡ��������Ϣ
	*/
	//std::vector<AuctionInfo*> GetPlayerAuctionInfoes(ObjID_t roleid);
	
	
	
	/**
	*@brief ��ȡ������ͼ۸�
	*/
	UInt32 _GetMinPriceOnsale(AuctionInfo& auctionInfo, UInt32 selfPrice);

	/**
	*@brief ��ȡ������Ʒװ����ͼ۸�
	*/
	UInt32 _GetTreasEqMinPriceOnSale(AuctionInfo& auctionInfo, UInt64& minPriceGuid, UInt32& minPriceOnsalePrice, UInt32& minPriceQualityLv,
		UInt32& minPriceMountBeadId, UInt32& minPriceMountBeadAverPrice, UInt32& minPriceMountMagicId, UInt32& minPriceMountMagicAverPrice);

	/**
	*@brief ��ȡ������ƷID
	*/
	UInt32 _GetItemTypeIDByPriceType(MoneyType type);

	/**
	*@brief ��Ʒ��Ʒ�ϼ�
	*/
	void OnTreasOnSale(AuctionInfo& auctionInfo);

	/**
	*@brief ��Ʒװ���ϼ�
	*/
	void OnTreasEqOnSale(AuctionInfo& auctionInfo);

	/**
	*@brief ��¼���ۼ۸�
	*/
	void AddTreasOnsalePrice(AuctionInfo& auctionInfo);

	/**
	*@brief ��¼���ۼ۸�
	*/
	void AddTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price);

	/**
	*@brief ȥ�����ۼ۸�
	*/
	void RemoveTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price);

	/**
	*@brief ��Ʒ�¼�
	*/
	void OnTreasOffSale(AuctionInfo& auctionInfo);

	/**
	*@brief ��Ʒװ���¼�
	*/
	void OnTreasEqOffSale(AuctionInfo& auctionInfo);

	/*
	*@brief ��Ʒ��װ���¼�
	*/
	void OnTreasNoEqOffSale(AuctionInfo& auctionInfo);

	/**
	*@breif ��¼��Ʒ���׼۸�(����ж����׳ɹ��Ļ�Υ�浫����δȡ��)
	*/
	void RecordTreasDealPrice(const AuctionTransRecord& auctionTrans, bool fore = false);

	/**
	*@brief ��ȡƽ���۸�
	*/
	UInt32 _GetAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief ��ȡʵʱƽ���۸�
	*/
	UInt32 _GetRealAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief ��ȡ�Ƽ��۸�
	*/
	UInt32 _GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt8 enhanceType);

	/**
	*@brief ��ȡ��ħ���Ƽ��۸�
	*/
	UInt32 _GetMagicRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, ItemDataEntry* itemData);

	/**
	*@brief ��ȡĳƷ�ʸ�ħ�����ʵʱƽ�����׼۸�
	*/
	UInt32 _GetMagicMinAverPriceByQuality(UInt8 quality);

	/**
	*@brief ���㸽ħ�����ʵʱƽ�����׼۸�
	*/
	void _CalcMagicMinAverPriceByQuality();

	/**
	*@brief ����������Ϣ
	*/
	AuctionInfo* _FindAuction(ObjID_t id);

	/**
	*@brief ��ȡ������
	*/
	UInt32 GetCounterFeeNew(UInt8 vipLvl, AuctionType type, UInt32 price);
private:
    /**
    *@brief ��ȡÿ�����������Ʒ�������
    */
    UInt8 _GetAuctionMaxItemPerPlayer(AuctionType type, WSPlayer* player);
    
    /**
    *@brief ͨ���ʼ���������Ʒ����ң�isBuy�����ǹ������˻�
    */
	bool _SendAuctionToPlayer(AuctionInfo *auctionInfo, UInt32 num, ObjID_t roleId, AuctionSendItemType sendType, bool abnormalDeal = false);

    /**
    *@brief ������������
    */
    WSItem* _FindAuctionItem(ObjID_t id);

    /**
    *@brief ɾ��������Ϣ
    */
    void _DeleteAuctionInfo(AuctionInfo *auctionInfo);

	/**
	*@brief ��ȡ������
	*/
	UInt32 _GetCounterFeeByAuctionType(WSPlayer* player, AuctionType type, UInt32 price);

    /**
    *@brief ��ȡ����ʱ��
    */
    UInt32 _GetAuctionDuetime(ItemDataEntry* dataEntry);

	/*
	*@brief ��ȡ��Ʒƽ���۸�
	*/
	UInt32 _GetTreasAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/*
	*@brief ��ȡ�ɼ�ƽ���۸�
	*/
	bool _GetVisAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType, UInt32& visAverPrice);
    
	/**
    *@brief ��¼�ɽ��۸�
    */
	void _RecordDealPrice(AuctionInfo* auctionInfo, UInt32 beadBuffId, UInt32 price, UInt32 num);

    /**
    *@brief ������Ʒ�Ƽ��۸�
    */
    void _UpdateRecommendPrice();

	/**
	*@brief ������Ʒ���¼�
	*/
	void _OnSendAuctionToPlayer(WSPlayer* player, AuctionSendItemType sendType, AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId, ObjID_t itemId,
		AuctionMainItemType mainType, UInt32 itemDataId, UInt32 num, MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen,
		UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount,
		UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
		UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice,
		UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief �Ƿ�����Ч�ļ۸�
	*/
	bool _IsValidPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32 price);

	/**
	*@brief ��ȡһ���޶��۸�  by huchenhui 2018.12.18
	*/
	bool _GetLimitPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32& minPrice, UInt32& maxPrice);

public:
	/**
	*@breif ������Ʒ���һ��������
	*/
	bool AddOneRushBuyPlayer(AuctionInfo* pAuctionInfo, WSPlayer* player, UInt32 num);

	/**
	*@brief һ��������Ʒ��������һ��ѡ�����
	*/
	//void DeliverOneRushBuyPlayer(AuctionInfo* pAuctionInfo);

	/**
	*@brief ��������
	*/
	bool DeliverOneRushBuyPlayerNew(AuctionInfo* pAuctionInfo);

	/**
	*@brief ѡ��һ��������
	*/
	bool SelectOneRushBuyer(const AuctionInfo& auctionInfo, AuctionRushBuyPlayer& rush_buyer);

	/**
	*@brief ����һ�������������
	*/
	void OnDeliverOneRushBuyResult(ObjID_t playerId, ObjID_t id, UInt32 num, bool succ, UInt8 reason);

	/**
	*@brief ��Ʒ������������
	*/
	void HandleTreaseRushDeliverEnd(AuctionInfo* pAuctionInfo);

	/*
	*@brief ������������Ʒ������ȴʱ���
	*/
	void SetAuctionItemCoolTime(WSItem* item);

	/*
	*@brief ������Ʒ������ȴʱ��
	*/
	static UInt32 CalcAuctionItemCoolTime(const std::string& pf, UInt32 itemId, UInt32 beginTime);
	static UInt32 CalcAuctionItemCoolTime(const std::string& pf, ItemDataEntry* dataEntry, UInt32 beginTime);
	
	void AddOneAution(AuctionInfo* auctionInfo);

	void AddOneTreas(AuctionInfo* auctionInfo);
	void RemoveOneTreas(AuctionInfo* auctionInfo);

	void GetBeadAndMagicCardAverPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice);

	//��ȡ��Ʒװ����Ƕ���飬��ħ��ƽ�����׼۸����ڼ���M�۸�
	void GetBeadAndMagicAverPriceForCalcTransEqMPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice,
		UInt32& mountedBeadId, UInt32& mountedMagicId, UInt32& mountedBeadBuffId, UInt8& mountedMagicLv);

	bool IsMountedBeadOrMagic(const AuctionInfo& auctionInfo);

	AuctionRecommendPrice* GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	bool SetAuctionGoodState(AuctionInfo& auctionInfo, UInt8 st);

	bool SetAuctionBuyState(AuctionInfo& auctionInfo, UInt8 st);

	void PrintAuctionState(AuctionInfo& auctionInfo);

	/**
	*@brief ������Ʒװ�����׼۸�Mֵ
	*/
	UInt32 CalcTransEqMPrice(AuctionInfo& auctionInfo);

	/**
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief ��ȡ������Ʒ��Ϣ
	*/
	AuctionInfo* GetAuctionInfo(WSPlayer* player, UInt64 auctionId);

	/**
	*@brief ����������id
	*/
	void FilterAuctions(std::vector<UInt64>& srcIds, std::vector<UInt64>& desIds, UInt8 goodState);

	/**
	*@brief ��ȡ��Ʒ���������ʱ��
	*/
	UInt32 GetItemLatestDealTime(UInt32 itemId);
public:
    /**
    *@brief ���ݽ��������ȡ��Ӧ�Ľ����Ʒ
    */
    static UInt32 GetGoldItemByNum(UInt32 num);

	/**
	*@brief ��ȡѺ��
	*/
	static UInt32 GetDepositByAuctionPrice(UInt32 price);

	/**
	*@brief ������������sql���
	*/
	static bool GenSqlByCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

	/**
	*@brief �����������������е�������
	*/
	static bool GenAuctionItemNumSql(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief ����sql��ѯ������
    */
	static bool GenAuctionQuerySqlCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief ����sql��ѯ������
    */
    static MoneyType GetMoneyTypeByAuctionType(AuctionType type);

    /*
    *@brief ��ȡ��������Ʒ������
    */
	static AuctionMainItemType GetAuctionMainType(ItemType type, ItemSubType subType);

    /*
    *@brief ��ȡ��������Ʒ������
    */
    static UInt32 GetAuctionSubType(ItemDataEntry* entry);

	/**
	*@brief GM����ƽ�����׼۸�
	*/
	bool SetAverPriceGM(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType,
		bool bSetAverPrice, UInt32 averPrice, bool bSetAverVisPrice, UInt32 averVisPrice, UInt32 transNum);

	/**
	*@brief ���촦��
	*/
	void OnDayChanged();

	/**
	*@brief ��¼����������
	*/
	void RecordCounterFee(UInt32 counterFee);

	/**
	*brief ��ȡʣ��ϵͳɨ����������
	*/
	UInt32 GetSysRecoveSurplus(UInt8 isTreas);

	/**
	*@brief ����ɨ������
	*/
	void IncRecoveryNum(UInt8 IsTreas, UInt32 num);

	/*
	*@brief �Ƿ�Ӧ��ɨ��
	*/
	bool IsShouldRecovery(AuctionInfo& auctionInfo);

	/**
	*@brief ��Ӵ�ɨ��
	*/
	void AddOneShouldRcoAuction(ObjID_t auctionId);

	/**
	*@brief ɨ������(����Ʒ)
	*/
	void RecoveryAuctions();

	/**
	*@brief ɨ������
	*/
	bool HandleForRecovery(AuctionInfo& auctionInfo, bool canDelete, bool& handled);

	/**
	*@brief ����ɨ������Ʒ������Ʒ
	*/
	bool OnSelectCheckRecovedNoTreasAuctionRet(CLRecordCallback *pCallback);

	AuctionPricesSt* GetAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	void UpdateAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPricesRes& res, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	AuctionPricesStForSwtich* GetAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	void UpdateAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPriceListRes& res, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	/**
	*@brief ��Ӵ��ϼܸ�ħ��
	*/
	void AddWaiteOnsaleMagic(ObjID_t auctionId);

	/**
	*@brief ������ϼܸ�ħ��
	*/
	void HandleWaiteOnsaleMagics();

	/**
	*@brief ��ħ���ϼ�
	*/
	void OnMagicOnsale(AuctionInfo& auctionInfo);

	/**
	*@brief ��ħ��������������
	*/
	void IncMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num = 1);

	/**
	*@brief ��ħ��������������
	*/
	void DecMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num = 1);

	/*
	*@brief ��ȡ��ħ����������
	*/
	UInt32 GetMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen);

	/**
	*@brief ��¼��ħ����������
	*/
	void RecordMagicOnsaleNum(AuctionInfo& auctionInfo);

	/**
	*@brief ��ħ���¼�
	*/
	void OnMagicOffsale(AuctionInfo& auctionInfo);

	/**
	*@brief ��ħ������
	*/
	void OnMagicTimeOut(AuctionInfo& auctionInfo);

	/**
	*@brief ���ٸ�ħ����������(������)
	*/
	void OnMagicBuy(AuctionInfo& auctionInfo, UInt32 num);

	/**
	*@brief ��ȡָ����ħ�����ȼ���������
	*/
	bool QueryMagicOnsales(WSPlayer *player, UInt32 itemTypeId);

	/**
	*@brief �����ڳ���ʱ���Զ��¼ܻص�����
	*/
	bool OnSelectTimeoutOffsaleAuctionRet(CLRecordCallback *pCallback);

	/**
	*@brief ����Ʒtick����
	*/
	void OnTickNoTreasAuctions();

	/**
	*@brief ��ȡ��¼ƽ�����׼۸��ǿ���ȼ�
	*/
	UInt8 GetStrengthenForAverPrice(const AuctionInfo& auctionInfo);

	UInt8 GetStrengthenForAverPrice(ItemDataEntry& itemData, UInt32 strengthen, UInt8 enhanceType);
	
	//��ȡ����װ��ǿ���ȼ�����
	void GetStrengthenRangeForHzEquip(UInt8 strengthen, UInt8& min, UInt8& max);

	//����
	void UpdateAuctionPriceCacheForExplaceHzEquip(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType);

	void OnQueryAuctionSaleLisForExplaceHzEquipRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8	realEnhanceType);

	//ְҵ���
	static bool CheckAuctionOccus(const ItemDataEntry* itemData, std::vector<UInt8>& occus);

	//ʱ����Ʒ�ϼ�ʱ����
	static bool CheckTimeLimitAuctionOnsaleAgain(WSPlayer* player, const AuctionInfo& auctionInfo);

	//��ѯ��Ʒ�ѽ��׼�¼
	UInt32  GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans);

	//��ѯ��Ʒ��������
	AuctionNumCache* GetAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond);

	//������Ʒ��������
	void UpdateAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond, CLProtocol::WorldAuctionItemNumRes& res);

public:	//�ڴ��ѯ��Ʒ��������Ʒ�б�
	//
	void AddOneAuctionForMemQuery(AuctionInfo* auctionInfo);
	void RemoveAuctionForMemQuery(AuctionInfo* auctionInfo);
	void QueryAuctionItemNumFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);
	void QueryAuctionItemListFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);
	void QueryItemRecentPricesFormMem(WSPlayer* player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType);
	void UpdateAuctionPriceCacheForExplaceHzEquipFromMem(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType);
	void QueryItemRecentPricesFormMemForSwitch(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
private:
    MapAuction                                      m_auctiones;

	//��Ҫ����״̬����Ʒ
	MapAuction										m_needUpdateTreasAuctions;

    // ��������г�ʱ��Ʒ��ʱ��
    Avalon::SimpleTimer	                            m_checkTimer;

	// �����������Ʒ״̬��ʱ��
	Avalon::SimpleTimer	                            m_checkTreasTimer;

	// ���ɨ��,��ħ���ϼܶ�ʱ��
	Avalon::SimpleTimer	                            m_checkNoTreasTimer;

	// ����ĳƷ�ʸ�ħ�����ʵʱƽ�����׼۸�ʱ��
	Avalon::SimpleTimer	                            m_clacMagicMinAverPriceTimer;

    // �Ƽ��۸�
	std::map<AuctionRecommendPriceKey, AuctionRecommendPrice*>	m_recommendPrice;

	// ������Ʒ��Ʒ(��װ��)�۸�
	std::map<AuctionRecommendPriceKey, MapPriceNum> m_onSaleTreaPrices;

    // ����ۼ�
    HashMap<UInt32, AuctionRecentPrice>             m_recentPrice;

    // �����Ƽ��۸�Ķ�ʱ��
    Avalon::SimpleTimer                             m_priceTimer;

	// �ϼܵ���Ʒװ��
	std::map<AuctionRecommendPriceKey, MapAuction> m_onSaleTreaEqAuctions;

	// ��������з���Ʒɨ����ʱ��
	Avalon::SimpleTimer	                            m_checkRecoveNoTreasTimer;

	// ��������Ʒ�۸񻺴��ϼ�
	MapAuctionPrices								m_pircesCacheOfOnSale;
	// ��������Ʒ�۸񻺴��б��л�					
	MapAuctionPricesForSwtich						m_pircesCacheOfSwitch; 

	//��ħ����������(���ֵȼ�)
	MapMagicOnsaleNum								m_magicOnsaleNum;

	// ����¼������г�ʱ��Ʒ��ʱ��
	Avalon::SimpleTimer	                            m_checkExpireAuctionTimer;

	// ��ɨ������Ʒ
	std::set<ObjID_t>								m_shouldRecoveryAuctions;

	// ���ϼܸ�ħ��
	std::set<ObjID_t>								m_waiteOnsaleMagics;

	// ��ħ��ĳƷ�����ʵʱƽ�����׼۸�
	MapMagicMinAverPrices							m_magicMinAverPrice;

	// �������������(����)
	std::vector<UInt32>								m_ShieldItemVec;

	// ������Ʒ��������
	std::vector<AuctionNumCache>					m_auctionNumCaches;

	// ������Ʒ����key:item_main_type
	std::map<UInt32, MapAuction>				m_AuctionsMainType;
	
	// ������Ʒ����key:item_sub_type
	std::map<UInt32, MapAuction>				m_AuctionsSubType;

	// ������Ʒ����key:itemid
	std::map<UInt32, std::set<AuctionInfo*, AuctionBaseCmpForItemIdSet>>	m_auctionesItemId;
};

};

#endif