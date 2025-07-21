#ifndef __EVENT_MGR_H__
#define __EVENT_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>

#define REGIST_EVT_HANDLER(cls, et, obj, func)	EventHandler<cls>* evt = new EventHandler<cls>(obj, func); \
												CLEventMgr::Instance()->RegistEventHandler(CLEventMgr::et, evt);

#define UNREGIST_EVT_HANDLER(cls, et, obj, func)	EventHandler<cls> evt(obj, func); \
													CLEventMgr::Instance()->UnRegistEventHandler(CLEventMgr::et, &evt);


#define REGIST_PLAYER_EVT_HANDLER(uid, cls, et, obj, func)	EventHandler<cls>* et##evt = new EventHandler<cls>(obj, func); \
	CLEventMgr::Instance()->RegistPlayerEventHandler(uid, CLEventMgr::et, et##evt);

#define UNREGIST_PLAYER_EVT_HANDLER(uid, cls, et, obj, func)	EventHandler<cls> et##evt(obj, func); \
	CLEventMgr::Instance()->UnRegistPlayerEventHandler(uid, CLEventMgr::et, &et##evt);

class IEventParam;

class IEventHandler
{
public:
	virtual void HandleEvent(IEventParam* param) = 0;
	virtual bool operator==(IEventHandler* h) = 0;

	virtual ~IEventHandler() {}
};

template<typename T>
class EventHandler : public IEventHandler
{
public:
	typedef void(T::*HandlerFunc)(IEventParam* param);

	EventHandler(T* obj, HandlerFunc func) :m_obj(obj), m_func(func){}

	virtual bool operator==(IEventHandler* h)
	{
		if (!h)	return false;
		
		EventHandler* eh = (EventHandler*)h;
		if (m_obj == eh->m_obj && m_func == eh->m_func)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual void HandleEvent(IEventParam* param)
	{
		if (m_obj && m_func)
		{
			(m_obj->*m_func)(param);
		}
	}

	T* m_obj;
	HandlerFunc m_func;
};

class CLEventMgr : public Avalon::Singleton<CLEventMgr>
{
public:
	CLEventMgr();
	~CLEventMgr();

	enum EventType
	{ 
		ET_RELOAD_SHOP_DATA = 0,

		ET_ON_LOGIC_DAYCHANGE,		//逻辑跨日
		ET_ON_ONLINE,				//上线
		ET_CHARGE_RMB,				//充值

		ET_EQUIP_REMAKE_QL,			// 装备品级调整
		ET_NEWGUIDE_NOTIFY,			// 新手引导
		ET_NOTIFY_NEWITEM,			// 获得新道具
		ET_NOTIFY_UPDATEITEM,		// 更新道具
		ET_LEVEL_UP,				// 升级

		ET_USE_ITEM,				// 使用道具
		ET_BUY_MALL_GIFTPACK,		// 购买商城礼包
		ET_CLEAR_DUNGEON,			// 通关地下城
		ET_PHONE_BIND,				// 手机绑定

		ET_DAYCHANGE,				// 自然日跨日
		ET_COMPLETE_PK,				// 完成pk
		ET_ENTER_DUNGEON,			// 参与地下城
		ET_TASK_FINISHED,			// 完成任务（一个任务的完成触发其他任务逻辑）

		ET_MAX,
	};

	enum SubEventType
	{
		SUB_ET_DEFAULT = 0,				//默认子类型，每次加1
		SUB_ET_ADD_ONE_DAILY = 1,		//特殊子类型，每天加1
		SUB_ET_ADD_ONE_WEEKLY= 2,		//特殊子类型，每周加1
		SUB_ET_TRIGGER_OTHER_TASK = 3,	//特殊子类型，完成任务后触发其他任务
		SUB_ET_SPECIAL_CALL_FUNC = 4,	//特殊子类型，调用函数（比如挑战者活动等等）
		SUB_ET_DRAW_PRIZE = 5,			//特殊子类型，每天加1，完成后抽奖1次
	};
	
	typedef std::vector<IEventHandler*>	HandlerVec;
	typedef std::map<EventType, HandlerVec>  HandlerMap;
	typedef std::map<ObjID_t, HandlerMap>	PlayerHandlerMap;
	

	void RegistEventHandler(EventType t, IEventHandler* h);
	void UnRegistEventHandler(EventType t, IEventHandler* h);

	void RegistPlayerEventHandler(ObjID_t uid, EventType t, IEventHandler* h);
	void UnRegistPlayerEventHandler(ObjID_t uid, EventType t, IEventHandler* h);
	void RemovePlayerEventsHandler(ObjID_t uid);

	void SendEvent(EventType t, IEventParam* param);
	void SendPlayerEvent(ObjID_t uid, EventType t, IEventParam* param);

private:
	HandlerMap m_handlers;
	PlayerHandlerMap	m_playerHandlers;
};

class IEventParam
{
public:
	CLEventMgr::EventType _eventType;
};

//充值事件参数
class ChargeEventParam : public IEventParam
{
public:
	ChargeEventParam() : _chargeNum(0){}

	UInt32 _chargeNum;
};

//上线事件参数
class OnlineEventParam : public IEventParam
{
public:
	OnlineEventParam() : _isDayChange(false){}

	bool _isDayChange;
};

//新手引导事件
class NewGuideEventParam : public IEventParam
{
public:
	NewGuideEventParam() : _newGuideId(0){}

	UInt32 _newGuideId;
};

//获得新道具
class GetNewItemParam : public IEventParam
{
public:
	GetNewItemParam() : itemId(0), openJar(false), _newItem(NULL){}

	ObjID_t itemId;
	bool	openJar;
	void* _newItem;
};

//更新道具
class UpdateItemParam : public IEventParam
{
public:
	UpdateItemParam() : _updateItem(NULL){}

	void* _updateItem;
};

//升级事件
class LevelUpParam : public IEventParam
{
public:
	LevelUpParam() : _oldLevel(0), _newLevel(0){}
	UInt32 _oldLevel;
	UInt32 _newLevel;
};

//使用道具事件
class UseItemParam : public IEventParam
{
public:
	UseItemParam() : _itemId(0){}
	UInt32 _itemId;
};

//完成地下城
class CompleteDungeonParam : public IEventParam
{
public:
	CompleteDungeonParam(UInt32 id) : _dungeonId(id){}
	UInt32 _dungeonId;
	std::vector<ObjID_t> _teammateIds;
};

//购买商城礼包
class BuyMallGiftParam : public IEventParam
{
public:
	BuyMallGiftParam(UInt32 id) : _giftId(id){}
	UInt32 _giftId;
};

//完成Pk
class CompletePkParam : public IEventParam
{
public:
	CompletePkParam(UInt32 pkType, UInt32 pkResult) : _pkType(pkType), _pkResult(pkResult) {}
	UInt32 _pkType;
	UInt32 _pkResult;
};

//参与地下城
class EnterDungeonParam : public IEventParam
{
public:
	EnterDungeonParam(UInt32 id) : _dungeonId(id) {}
	UInt32 _dungeonId;
};

//完成任务
class TaskFinishedParam : public IEventParam
{
public:
	TaskFinishedParam(UInt32 id) : _taskId(id) {}
	UInt32 _taskId;
};

#endif