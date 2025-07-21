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

		ET_ON_LOGIC_DAYCHANGE,		//�߼�����
		ET_ON_ONLINE,				//����
		ET_CHARGE_RMB,				//��ֵ

		ET_EQUIP_REMAKE_QL,			// װ��Ʒ������
		ET_NEWGUIDE_NOTIFY,			// ��������
		ET_NOTIFY_NEWITEM,			// ����µ���
		ET_NOTIFY_UPDATEITEM,		// ���µ���
		ET_LEVEL_UP,				// ����

		ET_USE_ITEM,				// ʹ�õ���
		ET_BUY_MALL_GIFTPACK,		// �����̳����
		ET_CLEAR_DUNGEON,			// ͨ�ص��³�
		ET_PHONE_BIND,				// �ֻ���

		ET_DAYCHANGE,				// ��Ȼ�տ���
		ET_COMPLETE_PK,				// ���pk
		ET_ENTER_DUNGEON,			// ������³�
		ET_TASK_FINISHED,			// �������һ���������ɴ������������߼���

		ET_MAX,
	};

	enum SubEventType
	{
		SUB_ET_DEFAULT = 0,				//Ĭ�������ͣ�ÿ�μ�1
		SUB_ET_ADD_ONE_DAILY = 1,		//���������ͣ�ÿ���1
		SUB_ET_ADD_ONE_WEEKLY= 2,		//���������ͣ�ÿ�ܼ�1
		SUB_ET_TRIGGER_OTHER_TASK = 3,	//���������ͣ��������󴥷���������
		SUB_ET_SPECIAL_CALL_FUNC = 4,	//���������ͣ����ú�����������ս�߻�ȵȣ�
		SUB_ET_DRAW_PRIZE = 5,			//���������ͣ�ÿ���1����ɺ�齱1��
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

//��ֵ�¼�����
class ChargeEventParam : public IEventParam
{
public:
	ChargeEventParam() : _chargeNum(0){}

	UInt32 _chargeNum;
};

//�����¼�����
class OnlineEventParam : public IEventParam
{
public:
	OnlineEventParam() : _isDayChange(false){}

	bool _isDayChange;
};

//���������¼�
class NewGuideEventParam : public IEventParam
{
public:
	NewGuideEventParam() : _newGuideId(0){}

	UInt32 _newGuideId;
};

//����µ���
class GetNewItemParam : public IEventParam
{
public:
	GetNewItemParam() : itemId(0), openJar(false), _newItem(NULL){}

	ObjID_t itemId;
	bool	openJar;
	void* _newItem;
};

//���µ���
class UpdateItemParam : public IEventParam
{
public:
	UpdateItemParam() : _updateItem(NULL){}

	void* _updateItem;
};

//�����¼�
class LevelUpParam : public IEventParam
{
public:
	LevelUpParam() : _oldLevel(0), _newLevel(0){}
	UInt32 _oldLevel;
	UInt32 _newLevel;
};

//ʹ�õ����¼�
class UseItemParam : public IEventParam
{
public:
	UseItemParam() : _itemId(0){}
	UInt32 _itemId;
};

//��ɵ��³�
class CompleteDungeonParam : public IEventParam
{
public:
	CompleteDungeonParam(UInt32 id) : _dungeonId(id){}
	UInt32 _dungeonId;
	std::vector<ObjID_t> _teammateIds;
};

//�����̳����
class BuyMallGiftParam : public IEventParam
{
public:
	BuyMallGiftParam(UInt32 id) : _giftId(id){}
	UInt32 _giftId;
};

//���Pk
class CompletePkParam : public IEventParam
{
public:
	CompletePkParam(UInt32 pkType, UInt32 pkResult) : _pkType(pkType), _pkResult(pkResult) {}
	UInt32 _pkType;
	UInt32 _pkResult;
};

//������³�
class EnterDungeonParam : public IEventParam
{
public:
	EnterDungeonParam(UInt32 id) : _dungeonId(id) {}
	UInt32 _dungeonId;
};

//�������
class TaskFinishedParam : public IEventParam
{
public:
	TaskFinishedParam(UInt32 id) : _taskId(id) {}
	UInt32 _taskId;
};

#endif