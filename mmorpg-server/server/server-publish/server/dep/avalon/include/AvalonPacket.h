/**
 *@author	 
 *@email	 
 *@date		2011-03-26	
 *@brief	字节缓冲
 */
#ifndef _AVALON_PACKET_H_
#define _AVALON_PACKET_H_

#include <AvalonObjectPool.h>
#include <AvalonMutex.h>
#include <AvalonDoubleQueue.h>
#include <AvalonNetStream.h>

namespace Avalon
{
	class NetService;

	/**
	 *@brief 网络包定义
	 */
	class Packet
	{
	public:
		//网络包体最大长度
		const static size_t MAX_BODY_SIZE = 64000;
		//网络包头长度（包头包含id、大小、序号）
		const static size_t HEAD_SIZE = 4 + 2 + 4;

	private:
#ifdef _THREAD_SAFE_
		Packet() :m_Id(0), m_Size(0), m_Sequence(0), m_MaxSize(0), m_Connid(0), m_pService(NULL)
		{
#ifdef _WIN32
			m_MagicCode = ++s_MagicCodeSeed;
#else
			m_MagicCode = __sync_add_and_fetch(&s_MagicCodeSeed, 1);
#endif
		}
#else
		Packet() : m_MagicCode(0), m_Id(0), m_Size(0), m_Sequence(0), m_MaxSize(0), m_Connid(0), m_pService(NULL){}
#endif
		~Packet(){}

	public:
		/**
		 *@brief 分配释放消息包
		 */
		static Packet* Create();
		static Packet* Create(size_t size);
		static void Destroy(Packet*& pack);
		static UInt32 GetNodeCount();
		static UInt32 GetFreeCount();

		/**
		 *@brief 拷贝消息包
		 */
		void CopyTo(Packet* packet);

		/**
		 *@brief 设置获取协议id
		 */
		UInt32 GetID() const { return m_Id; }
		void SetID(UInt32 id) { m_Id = id; }

		/**
		 *@brief 设置获取消息包大小
		 */
		UInt16 GetSize() const { return m_Size; }
		void SetSize(UInt16 size) { m_Size = size; }

		/**
		*@brief 设置获取消息包序号
		*/
		UInt32 GetSequence() const { return m_Sequence; }
		void SetSequence(UInt32 sequence) { m_Sequence = sequence; }

		/**
		 *@brief 获取最大缓冲大小
		 */
		UInt16 GetMaxSize() const { return m_MaxSize; }
		/**
		 *@brief 获取缓冲首地址  noted by aprilliu, 数据区的起始位置
		 */
		UInt8* GetBuffer() const { return (UInt8*)this + sizeof(Packet) + HEAD_SIZE; }

		/**
		 *@brief 设置获取连接地址
		 */
		UInt32 GetConnID() const { return m_Connid; }
		void SetConnID(UInt32 id) { m_Connid = id; } 

		/**
		 *@brief 设置获取网络服务
		 */
		NetService* GetNetService() const { return m_pService; }
		void SetNetService(NetService* service) { m_pService = service; }

		/**
		*@brief 获取魔幻数字
		*/
		UInt32 GetMagicCode() const { return m_MagicCode; }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			//modified by aprilliu, 2016.2.29 编解码顺序调整：改为2Byte size + 4Byte protocol ID
			//modified by zjy, 2016.08.09 编解码中添加消息序号：现在为 2Byte size + 4Byte protocol ID + 4Byte sequence
			stream & m_Size & m_Id & m_Sequence;
			//stream & m_Id & m_Size;
			stream.SeriaArray(GetBuffer(), GetSize());
		}

		//adde by aprilliu, 仅对头部（2+4+4个字节进行编解码）
		bool	EncodeHeader();
		bool	DecodeHeader(UInt8* header, size_t size);

		//得到完整的包buff(包含头部编码的buff)
		UInt8*	GetPacketBuff();
		//得到完整的包长度（包含头部长度 + body长度 m_Size）
		UInt16  GetPacketSize();

	public:
		// 魔幻数字
		UInt32 m_MagicCode;
		//包id
		UInt32 m_Id;
		//大小 noted by aprilliu 包长度(仅包含Body的长度，不包括Head的长度（2+4+4）)
		UInt16 m_Size;
		//序号
		UInt32 m_Sequence;
		//最大大小
		UInt16 m_MaxSize;
		//收到包的连接id  在NetConnection::PreHandlePacket中被设置
		UInt32 m_Connid;
		//收到连接的网络服务器  在NetConnection::PreHandlePacket中被设置
		NetService* m_pService;
		// 魔幻数字的种子
		static UInt32 s_MagicCodeSeed;
	};

	typedef Avalon::DoubleQueue<Packet*,256> PacketQueue;
}

#endif
