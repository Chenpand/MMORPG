/**
*@author	zjy
*@email
*@date		2016-08-09
*@brief		玩家消息缓存
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
	*@brief 账号
	*/
	UInt32 GetAccID() const { return m_accid; }
	void SetAccID(UInt32 id) { m_accid = id; }

	/**
	*@brief 保存消息
	*/
	bool StorePacket(Avalon::Packet* packet);
	
	/**
	*@brief 提取消息，从指定的序号开始将所有消息体都放到流中，返回提取到的最后一个消息（0代表没有后序的消息了）
	*/
	UInt32 FetchPacket(UInt32 startSequence, Avalon::NetOutputStream& stream);

	/**
	*@brief 是不是合法的sequence
	*/
	bool IsValidSequence(UInt32 sequence) const;

	UInt32 GetMinSequence() const { return m_minSequence; }
	UInt32 GetMaxSequence() const { return m_maxSequence; }

	/**
	*@brief 清空数据
	*/
	void Clear();

private:
#ifdef _CHECK_THREAD_
	/**
	*@brief 检测线程安全
	*/
	void CheckThread();
#endif

private:
	// 账号ID
	UInt32							m_accid;
	// 保存的消息包
	std::list<Avalon::Packet*>		m_packets;
	// 最小的消息序号
	UInt32							m_minSequence;
	// 最大消息序号
	UInt32							m_maxSequence;
	// 消息包大小
	UInt32							m_packetSize;
#ifdef _CHECK_THREAD_
	// 主线程ID
	UInt32							m_MainThreadId;
#endif
};

#endif