#ifndef _ACTION_COUNTER_MGR_H_
#define _ACTION_COUNTER_MGR_H_

#include <CLObjectProperty.h>

class Player;

/**
 *@brief 行为计数类型                                                                     
 */
enum ActionCounterType
{
	ACTION_COUNTER_INVALID = 0,		//无效类型
	ACTION_COUNTER_KILLNPC = 1,		//杀怪数
	ACTION_COUNTER_KILLPLAYER = 2,	//非战场杀人数
	ACTION_COUNTER_CHALLENGEWIN = 3,	//擂台赛赢
	ACTION_COUNTER_ROBTRANS = 4,	//截取运送木材
	ACTION_COUNTER_CAIDAN = 5,		//彩蛋关
	ACTION_COUNTER_KILLBOSS = 6,	//杀boss数
	ACTION_COUNTER_KILLSUPERBOSS = 7,	//杀超级boss数
	ACTION_COUNTER_MAKEFRIEND = 8,	//交好友数
	ACTION_COUNTER_TERRITORYWIN = 9, //宝地战胜利
	ACTION_COUNTER_USETREASUREMAP = 10,
	
	ACTION_COUNTER_MAX,
};

/**
 *@brief                                                                      
 */
class ActionCounterMgr : public CLComplexProperty
{
public:
	ActionCounterMgr();
	~ActionCounterMgr();

public:
	void SetOwner(Player* player){ m_pPlayer = player; }
	Player* GetOwner() const{ return m_pPlayer; } 

	UInt32 IncCounter(int type);
	UInt32 GetCounter(int type) const;

public: //编解码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(m_Count, ACTION_COUNTER_MAX);
	}

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

public://事件
	/**
	 *@brief  计数增加                                                                     
	 */
	void OnIncCount(int type, UInt32 count);

private:
	//玩家
	Player* m_pPlayer;

	//计数
	UInt32	m_Count[ACTION_COUNTER_MAX];
};

#endif
