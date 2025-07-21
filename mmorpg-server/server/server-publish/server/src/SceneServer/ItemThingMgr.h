#ifndef _ITEM_THING_MGR_H_
#define _ITEM_THING_MGR_H_

#include <memory>
#include <CLDefine.h>
#include <CLItemThing.h>

class Player;

/**
 *@brief �����������
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
 *@brief �����������
 */
class ItemThingMgr : public Avalon::Singleton<ItemThingMgr>
{
public:
	ItemThingMgr();
	~ItemThingMgr();

	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ��ʼ����
	 */
	UInt32 StartThing(Player* player, UInt64 serialId, ItemThingCost& itemCost, const std::string& reason, UInt64& thingId);

	/**
	 *@brief �ύ����
	 */
	UInt32 CommitThing(Player* player, UInt64 thingId);

	/**
	 *@brief �ع�����
	 */
	UInt32 RollBackThing(Player* player, UInt64 thingId);

	/**
	 *@brief ȡ������
	 */
	void CancelThing(Player* player, UInt64 serialId);

private:

	SceneItemThingPtr _FindThingByThingId(UInt64 thingId);
	SceneItemThingPtr _FindThingBySerialId(UInt64 serialId);


	// (guid--����)
	std::map<UInt64, SceneItemThingPtr> m_ItemThingMap;

	// (serialId--����)
	std::map<UInt64, SceneItemThingPtr> m_ItemThingSerialMap;
};

#endif // !_ITEM_THING_MGR_H_

