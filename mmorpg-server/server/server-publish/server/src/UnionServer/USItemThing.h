#ifndef _US_ITEM_THING_H_
#define _US_ITEM_THING_H_

#include <CLDefine.h>
#include <CLItemThingProtocol.h>

class USPlayer;
struct ItemThingCost;
/**
 *@brief 事务
 */
class USItemThing
{
public:
	USItemThing(USPlayer* player);
	virtual ~USItemThing();

	USPlayer* GetUSPlayer() { return m_Player; }
	void SetUSPlayer(USPlayer* player) { m_Player = player; }

	void SetFunType(UInt32 funType) { m_FunType = funType; }
	UInt32 GetFunType() { return m_FunType; }

	UInt64 GetSerialId() { return m_SerialId; }

	void SetReason(const std::string& reason) { m_Reason = reason; }
	const std::string& GetReason() { return m_Reason; }

	ItemThingCost& GetItemCost() { return m_ItemCost; }
	void SetItemCost(ItemThingCost& itemCost) { m_ItemCost = itemCost; }

	UInt32 GetTimeOut() { return m_TimeOutStamp; }

	void StartThing();
	void CommitThing(UInt64 thingId, bool isCommit);
	bool CheckTimeOut(const Avalon::Time& now);

	UInt64 GenItemThingSerialId();

	virtual void OnCheckSucced(USPlayer* player, UInt64 thingId) {}
	virtual void OnCheckFailed(USPlayer* player, UInt32 result) {}
	virtual void OnTimeOut(USPlayer* player);

private:

	USPlayer* m_Player;
	// 序号id
	UInt64 m_SerialId;
	// funType
	UInt32 m_FunType;
	// reason
	std::string m_Reason;
	// 超时时间
	UInt32 m_TimeOutStamp;
	// 事务的消耗
	ItemThingCost m_ItemCost;
};


#endif // !_US_ITEM_THING_H_
