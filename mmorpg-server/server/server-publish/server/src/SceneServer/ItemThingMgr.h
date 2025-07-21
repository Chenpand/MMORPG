#ifndef _ITEM_THING_MGR_H_
#define _ITEM_THING_MGR_H_

#include <memory>
#include <CLDefine.h>
#include <CLItemThing.h>

class Player;

/**
 *@brief 道具事务管理
 */
struct SceneItemThing
{
	SceneItemThing() : thingId(0), roleId(0), serialId(0), timeOutStamp(0) {}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " thingId:" << thingId;
		ss << " roleId:" << roleId;
		ss << " serialId:" << serialId;
		ss << " timeOutStamp:" << timeOutStamp;
		ss << " reason:" << reason;
		ss << " itemCost:" << itemCost.LogStr();
		return ss.str();
	}

	UInt64 thingId;
	UInt64 roleId;
	UInt64 serialId;
	UInt32 timeOutStamp;
	std::string reason;
	ItemThingCost itemCost;
};

typedef std::shared_ptr<SceneItemThing> SceneItemThingPtr;

/**
 *@brief 道具事务管理
 */
class ItemThingMgr : public Avalon::Singleton<ItemThingMgr>
{
public:
	ItemThingMgr();
	~ItemThingMgr();

	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 开始事务
	 */
	UInt32 StartThing(Player* player, UInt64 serialId, ItemThingCost& itemCost, const std::string& reason, UInt64& thingId);

	/**
	 *@brief 提交事务
	 */
	UInt32 CommitThing(Player* player, UInt64 thingId);

	/**
	 *@brief 回滚事务
	 */
	UInt32 RollBackThing(Player* player, UInt64 thingId);

	/**
	 *@brief 取消事务
	 */
	void CancelThing(Player* player, UInt64 serialId);

private:

	SceneItemThingPtr _FindThingByThingId(UInt64 thingId);
	SceneItemThingPtr _FindThingBySerialId(UInt64 serialId);


	// (guid--事务)
	std::map<UInt64, SceneItemThingPtr> m_ItemThingMap;

	// (serialId--事务)
	std::map<UInt64, SceneItemThingPtr> m_ItemThingSerialMap;
};

#endif // !_ITEM_THING_MGR_H_

