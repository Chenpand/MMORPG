#ifndef _WS_PLAYER_MGR_H_
#define _WS_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include <map>
#include "WSPlayer.h"

class WSScene;

/**
 *@brief ��ҹ�����
 */
class WSPlayerMgr : public Avalon::Singleton<WSPlayerMgr>
{
	typedef std::map<UInt32, WSPlayer*>	AccidPlayerMap;

	//�浵��ʱʱ��
	const static UInt32	DESTROY_TIMEOUT = 60;
	//�ϱ�social�����
	const static UInt32	REPORT_SOCIAL_INTERVAL = 60;

public:
	WSPlayerMgr();
	~WSPlayerMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief ��ӡ�ɾ�����������
	 */
	bool AddPlayer(WSPlayer* player);
	void RemovePlayer(WSPlayer* player);

	//���߲�ѯ��һ���
	bool AddOfflinePlayerCache(WSPlayer* player);

	/**
	 *@brief �����˺�id��ӡ�ɾ�����������
	 */
	bool AddPlayerByAccId(WSPlayer* player);
	void RemovePlayerByAccId(WSPlayer* player);
	
	WSPlayer* FindPlayer(ObjID_t id);
	WSPlayer* FindPlayerByAccid(UInt32 accid);
	WSPlayer* FindNormalPlayer(ObjID_t id);
	WSPlayer* FindNormalPlayerByName(const std::string& name);

	/**
	 *@brief �����������
	 */
	void VisitPlayers(WSPlayerVisitor& visitor);
	void VisitNormalPlayers(WSPlayerVisitor& visitor);

	/**
	 *@brief ��ý�ɫ��ʼֵ����
	 */
	Avalon::PropertySet* GetPlayerAttr() { return &m_InitAttrs; }

	/**
	 *@brief ��ȡ��������
	 */
	UInt32	GetOnlineNum() const{ return m_OnlineNum; }

	/**
	*@brief ��ȡ������ڵ�SceneServer
	*/
	UInt32 GetPlayerSceneServerID(ObjID_t roleId);

	/**
	*@brief ϵͳ���ŵĿ���չ��ɫ����
	*/
	UInt16 GetSystemOpenedExtensibleRoleNum() const { return m_SystemOpenedExtensibleRoleNum; }

public://���������ϱ����
	//void SendUdpLog(const char* str1, const char* str2, const char* str3 , const char* str4, const char* str5, const char* str6, const char* strType, Int32 nCount = 1);

public:
	/**
	 *@brief ִ��һ�������Ϣ
	 */
	void ExecutePacket(ObjID_t id,Avalon::Packet* packet);

public://�¼�
	/**
	 *@brief �������ӶϿ�
	 */
	void OnSceneDisconnected(UInt32 nodeid);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);
	
	/**
	 *@brief ����
	 */
	void OnDayChanged();

	/**
	 *@brief ������
	 */
	void OnNameChanged(ObjID_t playerId, const std::string& newname);

	/**
	 *@brief ���Ա���                                                                     
	 */
	void OnSexChanged(ObjID_t playerId, UInt8 sex);

	/**
	*@brief �����Scene�ϼ������
	*/
	void OnPlayerLoadFinishOnSceneServer(ObjID_t roleId, UInt32 sceneNodeId);

	/**
	*@brief �����Scene�ϼ������
	*/
	void OnPlayerLeaveSceneServer(ObjID_t roleId);

	/**
	*@brief �˺�������ɾ��
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId, UInt8 type);

	/**
	*@brief ��ѯ���ʱ�����������ɾ���Ľ�ɫ
	*/
	void QueryForeverDeletedPlayer();

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

	/**
	*@brief �����е����
	*/
	void AddLoadingPlayer(ObjID_t roleid) { m_loadingPlayer.insert(roleid); }
	void DelLoadingPlayer(ObjID_t roleid) { m_loadingPlayer.erase(roleid); }
	UInt32 GetLoadingPlayerNum() const { return m_loadingPlayer.size() + m_loadingPlayerNum; }
	
	// ������������
	void ClearPfPlayerNum();
	void AddPfPlayerNum(std::string pf);
	UInt32 GetPfPlayerNum(std::string pf);

private:
	//ע��Э��
	void RegisterProtocols();

	bool _RemovePlayerInAccountMap(WSPlayer* player);


private:
	//id�������
	CLIDObjIndex<WSPlayer>	m_Players;
	//name�������
	CLNameObjectIndex<WSPlayer>	m_NamePlayers;
	//�˺�id�������
	AccidPlayerMap	m_AccidPlayers;
	//�����Ϣ��ִ����
	CLParamPacketExecutor<WSPlayer*>	m_Executor;
	//�������SceneServerIDӳ���
	HashMap<ObjID_t, UInt32>	m_player2SceneServer;

	Avalon::SimpleTimer			m_CheckDeletePlayerTimer;

	//��������
	UInt32				m_OnlineNum;

	//��ɫ��ʼ����
	Avalon::PropertySet	m_InitAttrs;

	//�ϴμ�¼������������
	UInt8				m_LogOnlineMin;
	//�ϴ��ϱ�������������
	UInt8				m_ReportOnlineMin;
	//�´��ϱ�����ʱ��
	UInt32				m_ReportSocialTime;
	//�����������
	UInt32				m_TodayMaxOnlineNum;
	//�����������ʱ��
	Avalon::Time		m_TodayMaxOnlineTime;
	//������������
	UInt32				m_TodayMinOnlineNum;
	//������������ʱ��
	Avalon::Time		m_TodayMinOnlineTime;

	// �Ŷ��������
	// �Ŷ�ͬ����ʱ��
	Avalon::SimpleTimer			m_waitQueueTimer;
	// �����Ŷӵ����
	std::list<UInt32>			m_waitPlayers;
	// �Ƿ���Ҫͬ���Ŷ���Ϣ
	bool						m_needSyncWaitQueue;
	// ���ڼ����е����
	std::set<ObjID_t>			m_loadingPlayer;
	// ���ڼ����е��������
	UInt32						m_loadingPlayerNum;
	// ������������
	std::map<std::string, UInt32>	m_Pf2PlayerNum;

	//ϵͳ���ŵĿ���չ��ɫ����
	UInt32		m_SystemOpenedExtensibleRoleNum;
};

#endif
