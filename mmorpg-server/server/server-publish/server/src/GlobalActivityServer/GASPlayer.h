#ifndef _GAS_PLAYER_H_
#define _GAS_PLAYER_H_

#include <AvalonObjectPool.h>
#include <AvalonProtocol.h>
#include <CLObject.h>
#include <CLGameDefine.h>


#define PLAYERINFO(T) "uid(" << T->GetUId() << "), serverName(" << T->GetServerName() << ")"

class GASItemTransaction;

class GASPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(GASPlayer, Avalon::NullMutex)

	//��������ʱʱ��
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;

public:
	GASPlayer();
	~GASPlayer();

public:
	/**
	*@brief ��ȡΨһid
	*/
	const std::string& GetUId() const { return GetName(); }

	/**
	*@brief ���û�ȡ�˺�id
	*/
	void SetAccId(UInt32 id) { m_AccId = id; }
	UInt32 GetAccId() const { return m_AccId; }

	/**
	*@brief ���û�ȡ�������
	*/
	void SetPlayerName(const std::string& name) { m_PlayerName = name; }
	const std::string GetPlayerName() const { return m_PlayerName; }

	/**
	*@brief ���û�ȡƽ̨��
	*/
	void SetPlatform(const std::string& pf) { m_Platform = pf; }
	const std::string GetPlatform() const { return m_Platform; }

	/**
	*@brief ���û�ȡ���ڷ���id
	*/
	void SetServerZoneId(UInt32 id) { m_ServerZoneId = id; }
	UInt32 GetServerZoneId() const { return m_ServerZoneId; }

	/**
	*@brief ���û�ȡ���ڷ�����
	*/
	void SetServerName(const std::string& server) { m_ServerName = server; }
	const std::string GetServerName() const { return m_ServerName; }

	/**
	*@brief ���û�ȡ��ҹ����ڵ�
	*/
	void SetSourceNode(ServerType serverType, UInt32 node);
	UInt32 GetSourceNode(ServerType serverType) const;

	/**
	*@brief ����ɾ��ʱ��
	*/
	void SetRemoveTime(UInt64 time) { m_RemoveTime = time; }
	UInt64 GetRemoveTime() const { return m_RemoveTime; }

public:
	void OnTick(const Avalon::Time& now);

public://�������
	/**
	*@brief ׼����������
	*/
	bool BeginItemTransaction(GASItemTransaction* transaction);

	/**
	*@brief ��ȡ��ǰ�ĵ�������
	*/
	GASItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	*@brief �����Ʋ�������
	*/
	void UnLockAsserts();

	/**
	*@brief �Ƴ�ָ�����ߣ��ڵ���ʹ����������
	*/
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);

public:
	/**
	*@brief ���͵��ͻ���
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

	/**
	*@brief ���͵�center
	*/
	void SendToCenter(Avalon::Protocol& protocol);
	void SendToCenter(Avalon::Packet* packet);

	/**
	*@brief ���͵�admin
	*/
	void SendToAdmin(Avalon::Protocol& protocol);
	void SendToAdmin(Avalon::Packet* packet);

	/**
	*@brief ���͵�world
	*/
	void SendToWorld(Avalon::Protocol& protocol);
	void SendToWorld(Avalon::Packet* packet);

	/**
	*@brief ���͵�gate
	*/
	void SendToGate(Avalon::Protocol& protocol);
	void SendToGate(Avalon::Packet* packet);

	/**
	*@brief ���͵�scene
	*/
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

private:
	UInt32 m_AccId;
	std::string m_PlayerName;
	std::string m_Platform;
	UInt32 m_ServerZoneId;
	std::string m_ServerName;
	UInt64 m_RemoveTime;

	//�߼���ʱ�¼�
	UInt64		m_TickTime[LOGICTICK_MAX];

	// ��������
	GASItemTransaction* m_pItemTrans;
	//��������ʱ��ʱ
	UInt8			m_ItemTransTimeout;

	// �����ڵ�
	UInt32 m_SourceNode[SERVER_TYPE_MAX];
};

typedef CLVisitor<GASPlayer> GASPlayerVisitor;

#endif