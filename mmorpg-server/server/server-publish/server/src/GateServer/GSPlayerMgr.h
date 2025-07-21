#ifndef _GS_PLAYER_MGR_H_
#define _GS_PLAYER_MGR_H_

#include "GSPlayer.h"
#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>

struct PerfInfo
{
	UInt32 count;
	UInt32 times;
};

/**
 *@brief ��ҹ�����
 */
class GSPlayerMgr : public Avalon::Singleton<GSPlayerMgr>
{
	typedef HashMap<UInt32,GSPlayer*>	AccPlayerMap;

public:
	GSPlayerMgr();
	~GSPlayerMgr();

public:
	/**
	 *@brief ��ʼ��
	 */
	bool Init(Avalon::PropertySet* properties);

	/**
	 *@brief ��ֹ
	 */
	void Final();


public:
	/**
	 *@brief ��ȡ��ʼ��ɫ����
	 */
	Avalon::PropertySet* GetPlayerInitAttr(){ return &m_InitAttrs; }

	/**
	 *@brief ���һ�����
	 */
	bool AddPlayer(GSPlayer* player);

	/**
	 *@brief ���һ��ѡ�н�ɫ�����
	 */
	bool AddGuidPlayer(GSPlayer* player);

    /**
    *@brief ɾ����ɫMap
    */
    bool RemoveGuidPlayer(GSPlayer* player);

	/**
	 *@brief ɾ��һ�����
	 */
	void RemovePlayer(GSPlayer* player);

	/**
	 *@brief �����˺Ų���һ�����
	 */
	GSPlayer* FindPlayerByAccID(UInt32 id);

	/**
	 *@brief ���ݽ�ɫguid����һ�����
	 */
	GSPlayer* FindPlayerByID(ObjID_t id);

	/**
	*@brief ��ȡ�������
	*/
	UInt32 GetPlayerNum() const { return m_accPlayers.size(); };

	/**
	*@brief ��ȡ�����ϵ��������
	*/
	UInt32 GetConnPlayerNum() const { return m_ConnPlayerNum; }

	/**
	*@brief ϵͳ���ŵĿ���չ��ɫ����
	*/
	UInt32 GetSystemOpenedExtensibleRoleFields() const { return m_SystemOpenedExtensibleRoleFields; }

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief �������ӽ���   Noted by aprilliu , id��account id
	 */
	void OnGateConnected(UInt32 id);

	/**
	 *@brief �������ӶϿ�
	 */
	void OnGateDisconnected(UInt32 id);

public://����
	/**
	 *@brief ����
	 */
	void VisitPlayers(GSPlayerVisitor& visitor);

public://��Ϣ
	/**
	 *@brief �㲥
	 */
	void Broadcast(UInt8 bZone, UInt16 level, Avalon::Packet* packet);

public:
	// �Ŷ����
	/**
	*@brief �Ƿ���Ҫ�Ŷ�
	*/
	bool IsNeedWaitOhters();

	/**
	*@brief ������˳���Ϸ��������ԭ�򣬳������ŵ�һ������ҵ�¼
	*/
	bool TryLetFirstPlayerLogin();

	/**
	*@brief ��ӵ������У���������ǰ��������
	*/
	UInt32 AddPlayerToWaitList(UInt32 accid);

	/**
	*@brief �Ӷ�����ɾ��
	*/
	void DelFromWaitList(UInt32 accid);

	/**
	*@brief ��ȡ����ڶ����е�λ��
	*/
	int GetIndexInWaitList(UInt32 accid);

	/**
	*@brief ͬ���Ŷ��е���ҵ��Ŷ���Ϣ
	*/
	void SyncWaitLoginPlayerInfo();

	UInt32 GetLoadingPlayerNum() const { return m_loadingPlayerNum; }
	UInt32 GetNormalPlayerNum() const { return m_normalPlayerNum; }
	UInt32 GetRoleSelectPlayerNum() const { return m_roleSelectPlayerNum; }

public:
	// ����ͳ�����

	/**
	*@brief ��ȡ��ǰʱ��(us)
	*/
	UInt64 GetUSecsNow();

	/**
	*@brief ��¼����ʱ��(us)
	*/
	void RecordUsedTime(UInt32 id, UInt32 usec);

	/**
	*@brief չʾһ֡�ڸ�ģ��ʹ�õ�ʱ��
	*/
	void ShowUsedTime();

public:
	//�˺�-�������
	AccPlayerMap				m_accPlayers;
	//��ɫ-�������
	CLIDObjIndex<GSPlayer>		m_Players;

	//��ɫ��ʼ����
	Avalon::PropertySet			m_InitAttrs;

	//�����ϵ������
	UInt32						m_ConnPlayerNum;

	// �Ŷ�ͬ����ʱ��
	Avalon::SimpleTimer			m_waitQueueTimer;
	// �����Ŷӵ����
	std::list<UInt32>			m_waitPlayers;
	// �Ƿ���Ҫͬ���Ŷ���Ϣ
	bool						m_needSyncWaitQueue;

	UInt32						m_loadingPlayerNum;
	UInt32						m_normalPlayerNum;
	UInt32						m_roleSelectPlayerNum;

	// �ϱ��Ŷ���Ϣ��ʱ��
	Avalon::SimpleTimer			m_reportWaitQueueTimer;
	//ϵͳ���ŵĿ���չ��ɫ��λ
	UInt32		m_SystemOpenedExtensibleRoleFields;

	// ����ͳ��
	// ����ͳ�ƶ�ʱ��
	Avalon::SimpleTimer			m_PerfTimer;
	// һ��tick�ڸ���ģ�����ĵ�ʱ��
	HashMap<UInt32, PerfInfo>	m_UsedTimeInOneTick;
};

#endif
