#ifndef _PLAYER_MGR_H_
#define _PLAYER_MGR_H_

#include <AvalonPropertySet.h>
#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include "Player.h"

struct PerfInfo 
{
	UInt32 count;
	UInt32 times;
};

/**
 *@brief ��ҹ�����
 */
class PlayerMgr : public Avalon::Singleton<PlayerMgr>
{
public:
	PlayerMgr();
	~PlayerMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief ���һ�����
	 */
	bool AddPlayer(Player* player);

	/**
	 *@brief ����һ�����
	 */
	Player* FindPlayer(ObjID_t id);
	Player* FindNormalPlayer(ObjID_t id);
	Player* FindPlayer(const std::string &strName);
	Player* FindNormalPlayer(const std::string &strName);
	Player* FindLoadingFinishPlayer(ObjID_t id);
	// ���߼�����һص������߼�
	void LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, PlayerLoadingCallback* cb);

	/**
	 *@brief �Ƴ�һ�����
	 */
	void RemovePlayer(Player* player, bool removeFromMap = false);

public:
	/**
	 *@brief ִ��һ����Ϣ��
	 */
	void ExecutePacket(ObjID_t id,Avalon::Packet* packet);

	/**
	*@brief ��¼ִ����һ����Ϣ
	*/
	void RecordExcutePacket(UInt32 msgid);

	/**
	*@brief չʾһ֡���յ��İ�
	*/
	void ShowPacketInOneTick();

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
	/**
	 *@brief �����������
	 */
	void VisitPlayers(PlayerVisitor& visitor);
	void VisitNormalPlayers(PlayerVisitor& visitor);

public:
	/**
	 *@brief ���ضϿ�����
	 */
	void OnGateDisconnected(UInt32 id);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �˺�������ɾ��
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief ��ý�ɫ��ʼֵ����
	*/
	Avalon::PropertySet* GetPlayerAttr() { return &m_InitAttrs; }

	/**
	*@brief ��ȡ��ҳ�ʼ����
	*/
	UInt32 GetInitSceneID() { return GetPlayerAttr()->GetProperty("PlayerInitInfo", "scene"); }

	/**
	*@brief ����
	*/
	void OnChangedMonth();

private:
	/**
	 *@brief ע�����Э��
	 */
	void RegisterProtocols();

	/**
	*@brief �жϻ��������������������һ��ֵ���ͳ���ɾ���������
	*/
	void TryRelaseCachePlayerData();

private:
	//id->player����
	CLIDObjIndex<Player>	m_Players;
	//�����Ϣ��ִ����
	CLParamPacketExecutor<Player*>	m_Executor;

	//�ֶ�ʱ��
	Avalon::SimpleTimer		m_MinTimer;

	//��ɫ��ʼ����
	Avalon::PropertySet		m_InitAttrs;

	// һ��tick�ڸ�����Ϣ������
	HashMap<UInt32, UInt32>	m_PacketNumInOneTick;

	// һ��tick�ڸ���ģ�����ĵ�ʱ��
	HashMap<UInt32, PerfInfo>	m_UsedTimeInOneTick;
};


#endif
