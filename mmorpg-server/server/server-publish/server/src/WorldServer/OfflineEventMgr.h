#ifndef _OFFLINE_EVENT_MGR_H_
#define _OFFLINE_EVENT_MGR_H_

#include <AvalonSingleton.h>
#include "OfflineEvent.h"
#include "CLChatDefine.h"

class WSPlayer;

/**
*@brief ȫ�������¼����������
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
 *@brief �����¼�������
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
	 *@brief �����¼�
	 */
	void LoadEvents();

	/**
	 *@brief ��һ���¼�
	 */
	void PutEvent(OfflineEvent* offlineevent);

public: //�¼�
	/**
	 *@brief ����
	 */
	void OnOnline();

	/**
	 *@brief �������
	 */
	void OnLoadDataRet(CLRecordCallback* callback);

private:
	//ӵ����
	WSPlayer*	m_pOwner;
	//�����¼��б�
	OfflineEventVec	m_OfflineEvents;
};

#endif
