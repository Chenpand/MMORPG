#ifndef _PLAYER_SCRIPT_H_
#define _PLAYER_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <AvalonRegex.h>

#include <CLDefine.h>
#include <CLItemDefine.h>
#include <CLSceneDefine.h>
#include <set>

class Player;
class BaseTask;
class Item;

/**
 *@brief 翻牌配置                                                                     
 */
struct FlipCardAward
{
	FlipCardAward():itemId(0), quality(0), itemNum(0), rate(0), index(0){}

	UInt32	itemId;		//道具id
	UInt8	quality;	//道具品质
	UInt16	itemNum;	//道具数量
	UInt16	rate;		//万分比
	UInt8	index;
};

/**
 *@brief 翻牌累计奖励配置                                                                     
 */
struct FlipCardAddupAward
{
	FlipCardAddupAward():itemId(0), quality(0), itemNum(0), times(0){}

	UInt32	itemId;
	UInt8	quality;
	UInt32	itemNum;
	UInt32	times;
};

struct BuyedDujieChallCountInfo
{
	BuyedDujieChallCountInfo():buyedCost(0), addHp(0) {}

	UInt32 buyedCost;
	UInt32 addHp;
};
/**
 *@brief 玩家脚本
 */
class PlayerScript : public Avalon::ScriptFile
{
	typedef std::vector<UInt32>	GoodsVec;
	typedef std::vector<UInt32>	NpcIdVec;
	typedef std::vector<Avalon::Regex*>	RegexVec;
	typedef std::vector<ItemExchangeInfo>		ExchangeItemVec;
	typedef std::map<UInt32, ExchangeItemVec>	ExchangeItemMap;
	typedef std::vector<FlipCardAward>	FlipCardAwardVec;
	typedef std::map<UInt32, FlipCardAddupAward> FlipCardAddupAwardMap;
	typedef std::map<UInt32, UInt32>    WheelOpenRateMap;
	typedef std::set<UInt32> MallPanicBuySet;
	typedef std::map<UInt32, BuyedDujieChallCountInfo> BuyedDujieChallCountMap;
	typedef std::map<UInt16, std::vector<UInt32> > GrowthCopySceneMap;
	typedef std::map<UInt16, UInt32>  DragonTowerMap;

public:
	PlayerScript(Avalon::ScriptState* state);
	~PlayerScript();

public:
	/**
	 *@brief 设置日常任务npc
	 */
	void SetDailyTaskNpc(const char* npcs);
	bool IsDailyTaskNpc(UInt32 npcId) const;

	/**
	 *@brief 设置说话的禁止模式
	 */
	bool SetForbidPattern(const char* pattern);

	/**
	 *@brief 检查语句是否禁止
	 */
	bool CheckWordForbid(const char* word, int wordlen);

	/**
	 *@brief 设置道具兑换                                                                     
	 */
	void SetExchangeItem(UInt32 pos, UInt32 itemId, UInt8 quality, const char* secitems);
	const ExchangeItemVec& GetExchangeItems(UInt32 pos) const;

	/**
	*@brief 判断是否富贵达人商城道具
	*@param mallId:商城道具ID
	*/
	bool IsMallPanicBuyId(UInt32 mallId);

	/**
	*@brief 随机装备商相关
	*/
	void AddEquipShopPrice(UInt32 uLevel, UInt32 uQuality, UInt32 uPrice);
	void AddEquipShopRefreshTime(UInt32 uHour);
	UInt32 GetEquipShopPrice(UInt32 uLevel, UInt32 uQuality);
	bool NeedRefreshEquipShop(const Avalon::Time &now, UInt32 uLastRefreshTime);
	UInt32 GetNextRefreshEquipTime(UInt32 uLastRefreshTime);

public:
	/**
	 *@brief 初始化
	 */
	void Init();

	/**
	 *@brief 出生
	 */
	void OnBirth(Player* player);

	/**
	 *@brief 上线
	 */
	void OnOnline(Player* player, UInt64 offlineTime);

	/**
	 *@brief 获取日常任务npc状态
	 */
	UInt8 GetDailyTaskStatus(Player* player, UInt32 npcId);

	/**
	 *@brief 0点
	 */
	void OnDayChanged(Player* player);

	/**
	 *@brief 升级
	 */
	void OnLevelup(Player* player);

	/** 
	 *@brief 日常任务接受
	 */
	void OnDailyTaskAccepted(Player* player, BaseTask* task);

	/** 
	 *@brief 日常任务完成
	 */
	void OnDailyTaskSubmitted(Player* player, BaseTask* task);

	/** 
	 *@brief 玩家消费
	 */
	void OnConsumeGold(Player* player, UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	 *@brief 玩家消费q点                                                                     
	 */
	void OnConsumePoint(Player* player, UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	*@brief 检查登录天数限制的道具
	*/
	void CheckLoginLimitItem(Player *pPlayer, Item *pItem);

	/**
	*@brief 通关副本
	*/
	void OnPassedCopyScene(Player *pPlayer, UInt32 mapId);

	/**
	*@brief 使用vip卡
	*/
	void OnUseVipCard(Player *pPlayer, UInt32 uItemId);

private:
	//日常任务npc
	NpcIdVec			m_DailyTaskNpcs;
	//正则表达式
	RegexVec			m_Regexes;

	//道具兑换map
	ExchangeItemMap		m_ExchangeItems;

	//翻牌配置
	FlipCardAwardVec		m_FlipCardAwards;
	FlipCardAddupAwardMap	m_FlipCardAddupAwards;

	WheelOpenRateMap    m_MapWheelOpenRate;

	UInt32 m_uDrawWheelCost;

	//富贵达人（一元抢购）
	MallPanicBuySet m_MallPanicBuySet;

	BuyedDujieChallCountMap m_BuyedDujieChallCountInfo;

	// 随机装备商店价格配置
	std::map<UInt32, UInt32>	m_mapEquipShopPrice;
	// 随机装备商店刷新时间配置
	std::vector<UInt32>			m_vecRefreshTimes;

	GrowthCopySceneMap m_mapGrowthCopySceneVec;

	DragonTowerMap m_mapDragonTower;
};

#endif
