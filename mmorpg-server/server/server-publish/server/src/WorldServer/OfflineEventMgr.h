#ifndef _OFFLINE_EVENT_MGR_H_
#define _OFFLINE_EVENT_MGR_H_

#include <AvalonSingleton.h>
#include "OfflineEvent.h"
#include "CLChatDefine.h"

class WSPlayer;

/**
*@brief 全局离线事件缓存管理器
*/
class GlobalOfflineEventMgr : public Avalon::Singleton<GlobalOfflineEventMgr>
{
	typedef std::vector<OfflineEvent*>						OfflineEventVec;
	typedef std::map<UInt16, OfflineEventVec>				OfflineType2Event;
	typedef std::map<ObjID_t, OfflineType2Event>			OfflineEventMap;

public:
	GlobalOfflineEventMgr();
	~GlobalOfflineEventMgr();

	GlobalOfflineEventQueryType FindOfflineEvent(ObjID_t uid, UInt16 type, std::vector<OfflineEvent*>& vec);
	void SelectEvent(ObjID_t uid, CLRecordCallback* callback);
	void AddOfflineEvent(ObjID_t uid, OfflineEvent* event);
	void DelOfflineEvent(ObjID_t uid);

private:
	OfflineEventMap		m_OfflineEvents;
	OfflineEventVec		m_EventsPtr;
};

/**
 *@brief 离线事件管理器
 */
class OfflineEventMgr
{
	typedef std::vector<OfflineEvent*>	OfflineEventVec;

public:
	OfflineEventMgr();
	~OfflineEventMgr();

public:
	inline void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	inline WSPlayer* GetOwner()	{ return m_pOwner; }

	/**
	 *@brief 加载事件
	 */
	void LoadEvents();

	/**
	 *@brief 放一个事件
	 */
	void PutEvent(OfflineEvent* offlineevent);

public: //事件
	/**
	 *@brief 上线
	 */
	void OnOnline();

	/**
	 *@brief 加载完成
	 */
	void OnLoadDataRet(CLRecordCallback* callback);

private:
	//拥有者
	WSPlayer*	m_pOwner;
	//离线事件列表
	OfflineEventVec	m_OfflineEvents;
};

#endif
