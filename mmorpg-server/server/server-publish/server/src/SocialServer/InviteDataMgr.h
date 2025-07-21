#ifndef _INVITE_DATA_MGR_H_
#define _INVITE_DATA_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonProtocol.h>
#include <CLInviteGameProtocol.h>

struct memcached_st;

struct PlayerData;

/**
 *@brief �������ݹ�����
 */
class InviteDataMgr : public Avalon::Singleton<InviteDataMgr>
{
public:
	InviteDataMgr();
	~InviteDataMgr();

public:
	/**
	 *@brief ��ʼ����ֹ
	 */
	bool Init(Avalon::PropertySet* properties);
	void Final();

	/**
	 *@brief �����������б�
	 */
	bool GetInviteeList(UInt32 inviter, std::vector<CLProtocol::InviteeInfo>& list, UInt32& totalGold);

	/**
	 *@brief ����������
	 */
	bool CheckGiftbagCond(UInt32 accId, UInt8 type, UInt16 minLevel, UInt16 maxLevel, UInt32 playerNum);

	/**
	 *@brief �����ȡ�ٻؽ�������
	 */
	bool CheckRecallAwardCond(UInt32 accId, UInt32 inviteeId, std::vector<CLProtocol::InviteeInfo>& list);

	/**
	 *@brief �����ȡ���ѷ���
	 */
	bool CheckConsumeAward(UInt32 accId, UInt32 maxGold, UInt32& gold, UInt32& totalGold);

	/**
	 *@brief �󶨿�����˺�                                                                     
	 */
	bool BindKLPlayer(UInt32 accId, const std::string& openid, UInt16 level, const std::string& klopenid);

	/**
	 *@brief �������                                                                     
	 */
	void CheckKLActivity(UInt32 nodeId, UInt32 serverId, UInt32 accId, ObjID_t playerId);

public://��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendProtocol(UInt32 connId, Avalon::Protocol& protocol);

public://�¼�
	/**
	 *@brief �������
	 */
	void OnPlayerLevelup(UInt32 uid, UInt16 level);

	/**
	 *@brief ����
	 */
	void OnPlayerOnline(UInt32 uid);

	/**
	 *@brief ����
	 */
	 void OnPlayerOffline(UInt32 uid);

	 /**
	  *@brief �������
	  */
	 void OnPlayerConsume(UInt32 uid, UInt32 gold);

	 /**
	  *@brief �������߽�����Ϸ
	  */
	 void OnInviteeEnterGame(UInt32 inviteeId, const std::string& openid, UInt32 inviterId);

private:
	/**
	 *@brief �������id��ȡ����
	 */
	bool GetPlayerDataByUID(UInt32 uid, PlayerData& data);

	/**
	 *@brief �������id�б��ȡ����
	 */
	bool GetPlayerDataByUIDs(const UInt32* ids, UInt32 idNum, std::vector<PlayerData*>& datas);

	/**
	 *@brief �����������
	 */
	bool SetPlayerDataByUID(UInt32 uid, const PlayerData& pData);

	/**
	 *@brief ���һ���������
	 */
	bool AddPlayerDataByUID(UInt32 uid, const PlayerData& pData);

	/**
	 *@brief ��������
	 */
	void FreePlayerDatas(std::vector<PlayerData*>& datas);

	/**
	 *@brief ��ȡ����������Ϣ
	 */
	void GetInviteeData(const PlayerData& pData, UInt32 dayBeginTime, CLProtocol::InviteeInfo& info);

private:
	//memcached״̬
	memcached_st* m_pMemc;
};

#endif
