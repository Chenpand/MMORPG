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
 *@brief ��������                                                                     
 */
struct FlipCardAward
{
	FlipCardAward():itemId(0), quality(0), itemNum(0), rate(0), index(0){}

	UInt32	itemId;		//����id
	UInt8	quality;	//����Ʒ��
	UInt16	itemNum;	//��������
	UInt16	rate;		//��ֱ�
	UInt8	index;
};

/**
 *@brief �����ۼƽ�������                                                                     
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
 *@brief ��ҽű�
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
	 *@brief �����ճ�����npc
	 */
	void SetDailyTaskNpc(const char* npcs);
	bool IsDailyTaskNpc(UInt32 npcId) const;

	/**
	 *@brief ����˵���Ľ�ֹģʽ
	 */
	bool SetForbidPattern(const char* pattern);

	/**
	 *@brief �������Ƿ��ֹ
	 */
	bool CheckWordForbid(const char* word, int wordlen);

	/**
	 *@brief ���õ��߶һ�                                                                     
	 */
	void SetExchangeItem(UInt32 pos, UInt32 itemId, UInt8 quality, const char* secitems);
	const ExchangeItemVec& GetExchangeItems(UInt32 pos) const;

	/**
	*@brief �ж��Ƿ񸻹�����̳ǵ���
	*@param mallId:�̳ǵ���ID
	*/
	bool IsMallPanicBuyId(UInt32 mallId);

	/**
	*@brief ���װ�������
	*/
	void AddEquipShopPrice(UInt32 uLevel, UInt32 uQuality, UInt32 uPrice);
	void AddEquipShopRefreshTime(UInt32 uHour);
	UInt32 GetEquipShopPrice(UInt32 uLevel, UInt32 uQuality);
	bool NeedRefreshEquipShop(const Avalon::Time &now, UInt32 uLastRefreshTime);
	UInt32 GetNextRefreshEquipTime(UInt32 uLastRefreshTime);

public:
	/**
	 *@brief ��ʼ��
	 */
	void Init();

	/**
	 *@brief ����
	 */
	void OnBirth(Player* player);

	/**
	 *@brief ����
	 */
	void OnOnline(Player* player, UInt64 offlineTime);

	/**
	 *@brief ��ȡ�ճ�����npc״̬
	 */
	UInt8 GetDailyTaskStatus(Player* player, UInt32 npcId);

	/**
	 *@brief 0��
	 */
	void OnDayChanged(Player* player);

	/**
	 *@brief ����
	 */
	void OnLevelup(Player* player);

	/** 
	 *@brief �ճ��������
	 */
	void OnDailyTaskAccepted(Player* player, BaseTask* task);

	/** 
	 *@brief �ճ��������
	 */
	void OnDailyTaskSubmitted(Player* player, BaseTask* task);

	/** 
	 *@brief �������
	 */
	void OnConsumeGold(Player* player, UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	 *@brief �������q��                                                                     
	 */
	void OnConsumePoint(Player* player, UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	*@brief ����¼�������Ƶĵ���
	*/
	void CheckLoginLimitItem(Player *pPlayer, Item *pItem);

	/**
	*@brief ͨ�ظ���
	*/
	void OnPassedCopyScene(Player *pPlayer, UInt32 mapId);

	/**
	*@brief ʹ��vip��
	*/
	void OnUseVipCard(Player *pPlayer, UInt32 uItemId);

private:
	//�ճ�����npc
	NpcIdVec			m_DailyTaskNpcs;
	//������ʽ
	RegexVec			m_Regexes;

	//���߶һ�map
	ExchangeItemMap		m_ExchangeItems;

	//��������
	FlipCardAwardVec		m_FlipCardAwards;
	FlipCardAddupAwardMap	m_FlipCardAddupAwards;

	WheelOpenRateMap    m_MapWheelOpenRate;

	UInt32 m_uDrawWheelCost;

	//������ˣ�һԪ������
	MallPanicBuySet m_MallPanicBuySet;

	BuyedDujieChallCountMap m_BuyedDujieChallCountInfo;

	// ���װ���̵�۸�����
	std::map<UInt32, UInt32>	m_mapEquipShopPrice;
	// ���װ���̵�ˢ��ʱ������
	std::vector<UInt32>			m_vecRefreshTimes;

	GrowthCopySceneMap m_mapGrowthCopySceneVec;

	DragonTowerMap m_mapDragonTower;
};

#endif
