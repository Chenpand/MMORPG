#ifndef _GATESESSION_H_
#define _GATESESSION_H_

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonTime.h>
#include <CLDefine.h>

/**
 *@brief ���ػỰ
 */
class GateSession
{
public:
	GateSession();
	~GateSession();

public:
	/**
	 *@brief ��ʼ��
	 */
	bool Init(UInt32 connid);
	/**
	 *@brief ��ȡ�ڵ�id
	 */
	UInt32 GetID() const { return m_Id; }

	/**
	 *@brief ���û�ȡ���������
	 */
	void SetPlayerNum(UInt32 num){ m_PlayerNum = num; }
	UInt32 GetPlayerNum() const{ return m_PlayerNum; } 

	/**
	 *@brief ���û�ȡ������������                                                                     
	 */
	void SetMaxPlayerNum(UInt32 num){ m_MaxPlayerNum = num; }
	UInt32 GetMaxPlayerNum() const{ return m_MaxPlayerNum; }

	UInt32 GetOnlinePercent() const;

public:
	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief ����Э��
	 */
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	//�ڵ�id
	UInt32	m_Id;
	//����
	Avalon::NetConnectionPtr m_Conn;

	//�����
	UInt32	m_PlayerNum;
	//��������
	UInt32	m_MaxPlayerNum;
};

#endif
