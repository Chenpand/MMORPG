#ifndef _TRADE_H_
#define _TRADE_H_

#include <CLGameDefine.h>
#include <AvalonObjectPool.h>

class Player;

/**
 *@brief 单方交易信息
 */
struct TradeInfo
{
	TradeInfo():player(NULL),silver(0)
	{
		status = TRADE_STATUS_UNLOCK;
	}

	//玩家
	Player* player;
	//道具列表
	std::map<ObjID_t, UInt16> items;
	//银币
	UInt32	silver;
	//状态
	UInt8	status;
};

/**
 *@brief 交易
 */
class Trade
{
public:
	Trade(Player* player1, Player* player2);
	~Trade();

public:
	/**
	 *@brief 打开关闭交易
	 */
	void Open();
	void Cancel();

	/**
	 *@brief 尝试关闭交易
	 */
	void Close();

public:
	/**
	 *@brief 设置银币数
	 */
	void SetSilver(Player* player, UInt32 silver);

	/**
	 *@brief 添加道具
	 */
	void AddItem(Player* player, ObjID_t itemid, UInt16 num);
	void RemoveItem(Player* player, ObjID_t itemid);

	/**
	 *@brief 设置交易状态
	 */
	void SetStatus(Player* player, UInt8 status);

	TradeInfo* GetInfo(Player* player);
	TradeInfo* GetOtherInfo(Player* player);

private:
	//同步状态
	void SyncStatus();
	//同步银币
	void SyncSilver();

private:
	//交易信息
	TradeInfo	m_TradeInfo[2];
};

#endif
