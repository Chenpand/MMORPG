/**
*@author	zjy
*@email
*@date		2016-08-09
*@brief		�����Ϣ����
*/

#ifndef __PLAYER_PACKET_BUFFER_H__
#define __PLAYER_PACKET_BUFFER_H__

#include <CLDefine.h>
#include <AvalonPacket.h>

class PlayerPacketBuffer
{
	static const UInt32 MAX_STORE_PACKET_NUM = 5000;
public:
	PlayerPacketBuffer();
	~PlayerPacketBuffer();

public:

	/**
	*@brief �˺�
	*/
	UInt32 GetAccID() const { return m_accid; }
	void SetAccID(UInt32 id) { m_accid = id; }

	/**
	*@brief ������Ϣ
	*/
	bool StorePacket(Avalon::Packet* packet);
	
	/**
	*@brief ��ȡ��Ϣ����ָ������ſ�ʼ��������Ϣ�嶼�ŵ����У�������ȡ�������һ����Ϣ��0����û�к������Ϣ�ˣ�
	*/
	UInt32 FetchPacket(UInt32 startSequence, Avalon::NetOutputStream& stream);

	/**
	*@brief �ǲ��ǺϷ���sequence
	*/
	bool IsValidSequence(UInt32 sequence) const;

	UInt32 GetMinSequence() const { return m_minSequence; }
	UInt32 GetMaxSequence() const { return m_maxSequence; }

	/**
	*@brief �������
	*/
	void Clear();

private:
#ifdef _CHECK_THREAD_
	/**
	*@brief ����̰߳�ȫ
	*/
	void CheckThread();
#endif

private:
	// �˺�ID
	UInt32							m_accid;
	// �������Ϣ��
	std::list<Avalon::Packet*>		m_packets;
	// ��С����Ϣ���
	UInt32							m_minSequence;
	// �����Ϣ���
	UInt32							m_maxSequence;
	// ��Ϣ����С
	UInt32							m_packetSize;
#ifdef _CHECK_THREAD_
	// ���߳�ID
	UInt32							m_MainThreadId;
#endif
};

#endif