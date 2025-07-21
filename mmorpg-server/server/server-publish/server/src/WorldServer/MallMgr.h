#ifndef _WS_MALL_MGR_H_
#define _WS_MALL_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include "CLMallProtocol.h"

#include "MallScript.h"
#include "MallItemBuyRecord.h"

class WSPlayer;
struct LoginPushInfo;
class MallItem;
class CLRecordCallback;

// 商城礼包活动状态
enum MallGiftPackActivityState
{
	MGPAS_INVALID = 0,
	MGPAS_OPEN,			// 开放
	MGPAS_CLOSED,		// 关闭
};

// 商城推送条件
enum MallPushCond
{
	// 不推送
	MALL_PUSH_COND_NONO = 0,
	// 玩家升级推送
	MALL_PUSH_COND_PLAYER_LEVEL_UP,
};

// 商品待上架
#define MALL_GOODS_WAIT_ON_SALE	1


//#define ITEM_NUM_PER_PAGE	999

struct ItemKey
{
	ItemKey(UInt8 type, UInt8 subType, UInt8 job) : m_type(type), m_subType(subType), m_job(job){}

	UInt8 m_type;
	UInt8 m_subType;
	UInt8 m_job;

	bool operator < (const ItemKey& other) const
	{
		if (m_type < other.m_type)
		{
			return true;
		}
		if (other.m_type < m_type)
		{
			return false;
		}
		
		if (m_subType < other.m_subType)
		{
			return true;
		}
		if (other.m_subType < m_subType)
		{
			return false;
		}

		if (m_job < other.m_job)
		{
			return true;
		}
		if (other.m_job < m_job)
		{
			return false;
		}

		return false;
	}
};


typedef std::vector<MallItemInfo*>			VecItem;

/**
*@brief 商城管理器
*/
class MallMgr : public Avalon::Singleton<MallMgr>
{

public:

	const static UInt16	INTIMACY_INC_BY_SEND_GIFT = 5;
	const static UInt32 BASE_LIMITED_ITEM_ID = 10000;
	const static UInt32 NORMAL_LIMITED_ITEM_ID = 11000;
	const static UInt32 FESTIVAL_LIMITED_ITEM_ID = 20000;
	const static UInt32 GOLDSTONE_ITEM_ID = 30000;
	const static UInt32 SILVERSTONE_ITEM_ID = 40000;
	const static UInt32 GM_GEN_UID = 1000000000;

public:
	MallMgr();
	~MallMgr();

	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief 重载表数据
	*/
	void ReloadDataTable();

	/**
	*@brief 回收资源
	*/
	void Destroy();

	/**
	*@brief 查询商店商品
	*/
	void OnQueryItem(WSPlayer* player, UInt32 mallItemId);
	UInt32 OnQueryItems(WSPlayer* player, UInt8 tagType, UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, UInt8 isPersonalTailor);
	void GetQueryItems(WSPlayer* player, const CLProtocol::SWRefreshLimiteBuyNum& protocol, std::vector<MallItemInfo>& outGoodsInfos);
	void QueryAllItems(VecItem& items);

	std::string EncodRewardItems(std::vector<ItemReward>& items);
	bool DecodeRewardItems(std::vector<ItemReward>& items, std::string strItems);

	/**
	*@brief GM接口
	*/

	//转换gm商品uid到内部管理的uid,返回0是错误
	UInt32 TransUidGMToLocal(UInt32 gmUid);
	UInt32 TransUidLocalToGm(UInt32 uid);

	//GM设置商品,处理添加或更新
	void GmSetGoods(MallItemInfo* good);
	bool GmDelGoods(UInt32 goodsId);

	//更新或删除一个商品
	void UpdateItem(VecItem& tmpItems, MallItemInfo* good);
	void DeleteItem(VecItem& tmpItems, UInt32 goodsId);

	/**
	*@brief 购买
	*/
	UInt32 Buy(WSPlayer* player, UInt32 itemId, UInt16 num);

	/**
	*@brief 批量购买
	*/
	UInt32 BatchBuy(WSPlayer* player, std::vector<ItemReward>& items);

	/**
	*@brief 查询商品礼包详细
	*/
	UInt32 OnQueryItemDetail(UInt32 mallItemId, std::vector<MallGiftDetail>& detail);

	/**
	*@brief 获得指定道具Id的价格
	*/
	void OnGetItemPrice(UInt32 itemId, UInt32& outPrice, UInt8& outMoneyT);

	/**
	*@brief 检查购买条件的返回处理
	*/
	void OnCheckBuyRet(ObjID_t uPlayerId, UInt32 retCode, UInt32 uId, const std::vector<UInt64>& itemUids, UInt16 uNum, ObjID_t uReceiver, Int32 restLimitNum);

	/**
	 *@brief 购买道具
	 */
	void Buy(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver);

	/**
	 *@brief 金贝购买道具
	 */
	void Buy(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 price, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId);

	/**
	*@brief 同步限量道具
	*/
	void SyncLimitedItem(WSPlayer *pPlayer);

	/**
	*@brief 根据道具id获取商城道具请求
	*/
	void GetMallItemByItemId(WSPlayer *pPlayer, UInt32 itemId);

	/**
	*@brief 推送商城商品
	*/
	void PushMallItems(WSPlayer *player, const MallItemInfosVec& mallItems);

public:
	/**
	*@brief 清除限量商品
	*/
	void ClearLimitedItem();

	/**
	*@brief 添加一般限量商品
	*/
	void AddNormalLimitedItem(const MallItemInfo &item);

	/**
	*@brief 添加开服限量商品
	*/
	void AddGSLimitedItem(const MallItemInfo &item);

	/**
	*@brief 添加节日限量商品
	*/
	void AddFestivalLimitedItem(const MallItemInfo &item);
	
	/**
	*@brief 添加金石商城商品
	*/
	void AddGoldStoneItem(const MallItemInfo &item);

	/**
	*@brief 添加银石商城商品
	*/
	void AddSilverStoneItem(const MallItemInfo &item);

	/**
	*@brief 设置广播消息
	*/
	void SetNotifyInfo(const std::string &strNotify);

	/**
	*@brief 添加商品
	*/
	void AddMallItem(UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, const char *szItem);
	bool AddMallItem(MallItemInfo* info);
	bool AddMallWaitGoods(MallItemInfo* info);

public://商城玩家绑定礼包相关
	/**
	*@brief 设置玩家绑定限时礼包数量
	*/
	void SetMallGiftPackNum(WSPlayer* pPlayer, UInt32 giftPackId, Int32 restLimitNum);

	/**
	*@brief 获取mallItemInfo
	*/
	MallItemInfo* GetMallItemInfo(UInt32 uid);

	/**
	*@brief 获取索引商品
	*/
	void GetIdxItems(ItemKey& key, VecItem& items);

	void GetRechargePushItems(WSPlayer* pPlayer);

	void BuyRechargePushItem(WSPlayer* pPlayer, UInt32 pushId);

	void OnSceneBuyRechargePushItemRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId);

	void OnCheckRechargePushRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId);

public://商品上架下架相关
	//上架商品
	bool OnSaleMallWaitGoods(UInt32 goodsId);

	//下架商品
	bool OffSaleMallWaitGoods(UInt32 goodsId);

public://商城限时礼包活动相关
	/**
	*@brief 同步礼包活动状态
	*/
	void SyncMallGiftPackActivityState(WSPlayer *pPlayer);

	/**
	*@brief 获取时间戳在0点的时间
	*/
	UInt32 GetTimeOnZero(Avalon::Time& time);

public://商品打折相关
	/**
	*@brief 获取运营活动打折率
	*/
	//UInt32 GetOpActivityDiscountRate();

	/**
	*@brief 根据一些条件改变打折率
	*/
	UInt32 GenDiscountRate(MallItemInfo* goods, UInt32 curTime);

	/**
	*@brief 生成最终打折率
	*/
	float GenDiscountRate(UInt32 rate);

public://登录推送相关
	/**
	*@brief 设置与登录推送绑定的商品信息
	*/
	void SetLoginPushBindGoodsInfo(LoginPushInfo* pushInfo);

public://账号限购相关
	/*
	*@brief 从数据库加载账号购买记录
	*/
	void LoadMallItemAccountBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief 查找商品账号购买记录
	*/
	MallItemAccountBuyRecord* FindMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId);
	MallItemAccountBuyRecord* FindMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index);

	/*
	*@brief 添加商品账号购买记录
	*/
	void AddMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index, MallItemAccountBuyRecord* record, bool saveDB = true);
	void AddMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId, UInt32 buyNum);

	/*
	*@brief 获取账号下剩余可购买数量
	*/
	UInt32 GetAccountRestBuyNum(UInt32 accid, const MallItemInfo* mallItem);

	/**
	*@brief 是否账号下限购
	*/
	bool IsLimitBuyOnAccount(UInt32 accid, const MallItemInfo* mallItem, UInt32 buyNum);

public:
	/*
	*@brief 从数据库加载自定义商品
	*/
	void LoadCustomerMallItemFromDB(WSPlayer* player, CLRecordCallback* callback);

	/*
	*@brief 创建自定义商品
	*/
	MallItem* CreateCustomerMallItem(WSPlayer* player, const MallItemInfo* mallItem);

	/*
	*@brief 查找自定义商品
	*/
	MallItem* FindCustomerMallItem(ObjID_t roleid, UInt32 mallItemId);

	/*
	*@brief 添加自定义商品
	*/
	void AddCustomerMallItem(MallItem* mallItem);

public:
	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief 跨天
	*/
	void OnDayChanged();

	/**
	*@brief 玩家等级改变
	*/
	void OnPlayerLevelChanged(WSPlayer* player, UInt16 oldLv);

private:
	/**
	*@brief 加载表数据
	*/
	bool LoadData(bool isReload = false);

	/**
	*@brief 读数据库配置
	*/
	void LoadDBConfig();

	/**
	*@brief DB操作
	*/
	void InsertDB(MallItemInfo* good);
	void DeleteDB(MallItemInfo* good);
	void UpdateDB(MallItemInfo* newGood, MallItemInfo* oldGood);

	/**
	*@brief 获取热门商品
	*/
	void GetHotGoods(UInt8 moneyType, std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief 获取私人订制商品
	*/
	void GetPersonalTailorGoods(std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief 获取普通商品
	*/
	UInt32 GetCommonGoods(WSPlayer* player, ItemKey& key, std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief 人民币礼包商品数据
	*/
	void UpdateGiftGood(MallItemInfo& good, WSPlayer* player);

	/**
	*@brief 保存限量道具出售信息
	*/
	void ResetLimitedItemSelledInfo();
	
	void DecodeItemInfo(MallItemInfo &item, const char *szItem);

	/**
	 *@brief 购买限量道具
	 */
	void BuyLimitedItem(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 price, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId);

	/**
	*@brief 同步限量道具数量
	*/
	void SyncLimitedItemNum(WSPlayer *pPlayer, MallItemInfo *pItem);

	/**
	*@brief 设置当前金券礼包活动
	*/
	void SetCurJQLBActivity();

	/**
	*@brief 是否上架
	*/
	bool IsOnSale(WSPlayer* player, MallItemInfo* mallItem, UInt32 limitTime);

private:
	// 索引商品
	std::map<ItemKey, VecItem>			m_idxItems;

	// 所有商品(商品id-->商品信息集)
	std::map<UInt32, MallItemInfo*>		m_allItems;
	std::map<UInt32, MallItemInfo*>		m_TempItems;

	// 待上架商品
	std::map<UInt32, MallItemInfo*>		m_waitGoods;

	// 玩家批量购买上下文
	std::map<UInt64, MallBatchBuyCtx>	m_PlayersBatchBuyCtx;

	// 限时礼包活动状态
	MallGiftPackActivityState			m_GiftActivityState;

	// 秒计时器
	Avalon::SimpleTimer					m_secTimer;

	// 自定义商品
	HashMap<UInt64, std::vector<MallItem*> > m_MallItemsOnPlayer;

	// 账号下商品购买记录
	std::map<MallItemAccountBuyIndex, MallItemAccountBuyRecord*> m_AccountMallItemBuyRecords;

	//-----------------（废弃）--------------------------------
	// 一般限量商品
	std::map<UInt32, MallItemInfo>	m_mapNormalItem;
	// 开服限量商品
	std::map<UInt32, MallItemInfo>	m_mapGSItem;
	// 节日限量商品
	std::map<UInt32, MallItemInfo>	m_mapFestivalItem;
	// 金石商城商品
	std::map<UInt32, MallItemInfo>	m_mapGoldStoneItem;
	// 银石商城商品
	std::map<UInt32, MallItemInfo>	m_mapSilverStoneItem;
	// 分钟计时器
	Avalon::SimpleTimer						m_minTimer;
	// 广播消息类容
	std::string							m_strNotify;
};

#endif

