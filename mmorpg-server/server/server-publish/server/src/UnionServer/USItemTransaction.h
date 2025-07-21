#ifndef _US_ITEM_TRANSACTION_H_
#define _US_ITEM_TRANSACTION_H_

#include <CLItemDefine.h>
#include <CLItemTransaction.h>

class USPlayer;

/**
 *@brief 道具事务
 */
class USItemTransaction : public CLItemTransaction
{
	friend class USPlayer;
public:
	USItemTransaction(USPlayer* player);
	virtual ~USItemTransaction();

public:
	USPlayer* GetPlayer() const { return m_Player; }
	UInt32 GetTimeOut() { return m_TimeOut; }

	void SetServiceType(UInt32 type) { m_ServiceType = type; }
	UInt32 GetServiceType() { return m_ServiceType; }

	void SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem = 0);
	UInt32 GetItemNum(UInt32 id);

	void SetNoBindItem(UInt32 id, UInt32 num);
	UInt32 GetNoBindItemNum(UInt32 id);

	/**
	 *@brief 发出检查
	 */
	void RemoveItem(const std::string& reason);

	/**
	 *@brief 解锁
	 */
	void UnLock();

	/**
	 *@brief 超时
	 */
	bool CheckTimeOut(const Avalon::Time& now);

	/**
	 *@brief 检查结果
	 */
	virtual void OnCheckSucced(USPlayer* player) = 0;
	virtual void OnCheckFailed(USPlayer* player) = 0;
	virtual void OnTimeOut(USPlayer* player) = 0;

private:
	void CheckItem();

private:

	USPlayer* m_Player;
	// 超时时间
	UInt32 m_TimeOut;
	// 所属的服务
	UInt32 m_ServiceType;
	// 道具
	std::map<UInt32, CLProtocol::TransItemData>	m_Items;
	// 绑定道具
	std::map<UInt32, UInt32>	m_NoBindItems;
};

#endif
