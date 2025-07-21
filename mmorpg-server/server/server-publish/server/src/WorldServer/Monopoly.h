#ifndef __MONOPOLY_H__
#define __MONOPOLY_H__

#include <vector>

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>
#include <CLRecordClient.h>
#include <CLMonopolyDataEntry.h>

class WSPlayer;

class AccountInfo;

/**
*@brief 大富翁随机类型
*/
enum MonopolyRandomType
{
	MRT_ALL = 0,
	MRT_RANDOM_ONE = 1,
};

/**
*@brief 大富翁事件类型
*/
enum MonopolyGridEventType
{
	MGET_ADD_ITEM = 0, //增加道具事件
	MGET_ADD_COIN = 1, // 获得硬币
	MGET_BUFF = 2, //获得buff
	MGET_MISSION = 3, //进入关卡
	MGET_RANDOM_CARD = 4, //随机祝福卡
	MGET_ADD_ROLL = 5, //增加骰子次数
	MGET_NULL = 6, //空事件
};

class MonopolyGridEvent
{
public:
	MonopolyGridEvent() = default;
	virtual ~MonopolyGridEvent() = default;
	virtual void OnTriged(WSPlayer* player) = 0;
	UInt32 GetWeight() const { return m_Weight; }
	static MonopolyGridEvent* GenEvent(MonopolyGridEventDataEntry* data);
	virtual void SendResult(WSPlayer* player, UInt32 result = 0);
protected:
	UInt32 m_Id = 0;
	UInt32 m_Weight = 0;
	UInt32 m_Param = 0;
	UInt32 m_Num = 0;
};

//增加道具事件
class AddItemMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//获得硬币
class AddCoinMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//获得buff
class AddBuffMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//进入关卡
class JoinMissionMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//随机祝福卡
class RandomCardMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
	virtual void SendResult(WSPlayer* player, UInt32 result = 0);
};

//增加骰子次数
class AddRollMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//空事件
class NullMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player) {};
};


class MonopolyMapGrid
{
public:
	MonopolyMapGrid() = default;
	virtual ~MonopolyMapGrid();
	virtual void OnTriged(WSPlayer* player) = 0;
	/**
	*@brief 根据类型生成格子
	*/
	static MonopolyMapGrid* GenGrid(UInt32 gridType);
protected:
	std::vector<MonopolyGridEvent*> m_Event;
	UInt32 m_TotalWeight = 0;
};

//格子中的所有事件都要触发
class AllMonopolyMapGrid : public MonopolyMapGrid
{
	virtual void OnTriged(WSPlayer* player);
};

//格子按权重随机触发一个事件
class RandomMonopolyMapGrid : public MonopolyMapGrid
{
	virtual void OnTriged(WSPlayer* player);
};





using MonopolyMap = std::vector<MonopolyMapGrid*>;

#define MONOPOLY_INFO(info) "accid(" <<info->GetAccid() << ")"

class MonopolyInfo : public CLSyncObject
{
public:
	MonopolyInfo();
	~MonopolyInfo();


	void Reset();
	/**
	*@brief 根据地图配置初始化地图
	*/
	void InitMap();
	void OnTick(const Avalon::Time &now);
	void OnWeekChange();
	void OnDayChanged();
	void ResetMap();
	void LoadData();
	void SetData(CLRecordCallback* callback);
	void InsertToDB();
	void OnOnline();
	void OnOffline();
	inline void SetLoaded() { m_IsLoaded = true; }

	void SendStatus();
	void SendCardList();
	void SendCoin();

public:
	/**
	*@brief 往前走几步
	*/
	UInt32 ForwardToMap(UInt32 cnt);
	/**
	*@brief 玩家roll一次筛子
	*/
	UInt32 OnOwnerRollOnce(UInt32& rolledNum);

	/**
	*@brief 计算前进步数
	*/
	UInt32 GetForwardNum();

	/**
	*@brief 增加某张卡牌指定数量
	*/
	UInt32 AddCard(UInt32 id, UInt32 num);

	/**
	*@brief 减少某张卡牌指定数量
	*/
	UInt32 RemoveCard(UInt32 id, UInt32 num);

	/**
	*@brief 获取某张卡牌数量
	*/
	UInt32 GetCardNum(UInt32 id);

	/**
	*@brief 增加卡牌兑换次数
	*/
	UInt32 AddCardExchange(UInt32 id, UInt32 num);

	/**
	*@brief 减少卡牌兑换次数
	*/
	UInt32 RemoveCardExchange(UInt32 id, UInt32 num);

	/**
	*@brief 获取卡牌兑换次数
	*/
	UInt32 GetCardExchange(UInt32 id);

	/**
	*@brief 增加关卡权限
	*/
	void AddMissionLimit(UInt32 id);

	/**
	*@brief 能否进入关卡
	*/
	bool CanJoinMission(UInt32 id);


	std::map<UInt32, UInt32>& GetCards();
	std::map<UInt32, UInt32>& GetCardExchangeList();

	void SaveCards();
	void LoadCards();

	void SaveCardExchange();
	void LoadCardExchange();
	
private:

	/**
	*@brief 获取最大格子id
	*/
	UInt8 GetMaxGridID();

public:
	void SetOwner(AccountInfo* info);
	AccountInfo* GetOwner();
	WSPlayer* GetPlayer();
	inline UInt32 GetAccid() const{ return m_Accid; }
	inline void SetCurrnetStep(UInt32 num) { m_Step = num; }
	inline UInt32 GetCurrnetStep() const { return m_Step; }
	void SetRemainRoll(UInt32 times, std::string reason = "");
	inline UInt32 GetVsersion() const { return m_Vsersion; }
	inline void SetVsersion(UInt32 vsersion) { m_Vsersion = vsersion; }
	inline UInt32 GetRemainRoll() const { return m_RemainRoll; }
	inline void SetRolledTimes(UInt32 times) { m_RolledTimes = times; }
	inline UInt32 GetRolledTimes() const { return m_RolledTimes; }
	void SetCoin(UInt32 num, std::string reason = "");
	inline UInt32 GetCoin() const { return m_Coin; }
	void SetBuff(UInt32 num);
	inline UInt32 GetBuff() const { return m_Buff; }
	inline UInt32 GetTurn() const { return m_Turn; }
	UInt32 GetFakeTurn();
	inline void SetTurn(UInt32 num) { m_Turn = num; }
public:

	void SaveToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("vsersion", m_Vsersion)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("turn", m_Turn)
		CL_DBPROPERTY("step", m_Step)
		CL_DBPROPERTY("remain_roll", m_RemainRoll)
		CL_DBPROPERTY("roll_times", m_RolledTimes)
		CL_DBPROPERTY("coin", m_Coin)
		CL_DBPROPERTY("buff", m_Buff)
		CL_DBPROPERTY("cards", m_Cards)
		CL_DBPROPERTY("exchanged", m_Exchanged)
	CL_DBPROPERTY_END()

private:
	//账号ID
	ObjUInt32 m_Vsersion;
	//圈数
	ObjUInt32 m_Turn;
	//账号ID
	ObjUInt32 m_Accid;
	//当前步数
	ObjUInt32 m_Step;
	//剩余骰子次数
	ObjUInt32 m_RemainRoll;
	//已摇骰子次数
	ObjUInt32 m_RolledTimes;
	//硬币
	ObjUInt32 m_Coin;
	//BUFF
	ObjUInt32 m_Buff;
	//卡牌情况
	ObjString m_Cards;
	//是否已交换过随机卡牌
	ObjString m_Exchanged;
	//是否已提交卡牌组合
	ObjUInt8 m_IsCommitedCard;
private:
	AccountInfo* m_Owner;
	// 保存数据库定时器
	Avalon::SimpleTimer	m_SaveTimer;
	// 大富翁地图
	MonopolyMap m_Map;
	// 卡牌内存结构 卡牌id -> 卡牌数量
	std::map<UInt32, UInt32> m_CardsMap;
	// 卡牌兑换内存结构 兑换项id -> 兑换次数
	std::map<UInt32, UInt32> m_CardExchangeMap;

	//大富翁关卡id
	UInt32			m_MissionID = 0;
	//大富翁关卡定时器
	Avalon::SimpleTimer	m_MissionTimer;
	//加载标记
	bool			m_IsLoaded = false;
};





#endif