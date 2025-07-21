/**
 *@author	 
 *@email	 
 *@date		2011-03-26	
 *@brief	网络连接
 */
#ifndef _AVALON_NETCONNECTION_H_
#define _AVALON_NETCONNECTION_H_

#include "AvalonSocket.h"
#include "AvalonINetEventHandler.h"
#include "AvalonINetReactor.h"
#include "AvalonPacket.h"
#include "AvalonSmartPtr.h"

namespace Avalon
{
	/**
	 *@brief 连接类型，分主动连接和被动连接
	 */
	enum NetConnectionType
	{
		NCT_ACTIVE,	//主动连接
		NCT_PASSIVE,	//被动连接
	};
	
	/**
	 *@brief 连接状态
	 */
	enum NetConnectionStatus
	{
		NCS_CLOSED,			//初始状态，还没连接
		NCS_VERIFY,			//验证阶段
		NCS_NORMAL			//正常通信状态
	};

	/**
	 *@brief 网络连接类
	 */
	class NetConnection : public INetEventHandler, public IReferenceable
	{
		friend class NetService;
	public:
		NetConnection();
		virtual ~NetConnection();

	public:
		/**
		 *@brief 打开一个连接
		 */
		bool Open(const Avalon::Socket& socket, NetConnectionType type, NetService* service);

		/**
		 *@brief 关闭连接，这个操作只会设置一个状态  ； Noted by aprilliu, 等待HandleTick才真正进行关闭连接的操作
		 *@param waittime 等待时间，秒
		 */
		void Close(UInt32 waittime = 0);

		/**
		 *@brief 发送字节流
		 */
		Int32 Send(const void* buf, Int32 len);
		Int32 SendChunk(const void* buf, Int32 len);

		/**
		 *@brief 接收字节流
		 */
		Int32 Recv(void* buf, Int32 len);

		/**
		 *@brief 发送一个网络包
		 */
		bool SendPacket(Packet* packet);

		/**
		 *@brief 接收一个消息包
		 */
		Packet* RecvPacket();

		/**
		 *@brief 获取错误码
		 */
		Int32 GetError(){ return Avalon::Socket::GetError(); }

		/**
		 *@brief 编解码包头
		 */
		virtual void EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence);
		virtual void DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence);

		/**
		 *@brief 获取类型
		 */
		NetConnectionType GetType() const{ return m_Type; }

		/**
		*@brief 连接打开的时间
		*/
		UInt32 GetOpenTime() const { return m_OpenTime; }

		/**
		 *@brief 设置id，验证通过后可以进行此操作
		 */
		void SetID(UInt32 id) { m_NodeID = id; }

		/**
		 *@brief 获取id
		 */
		UInt32 GetID() const { return m_NodeID; }

		/**
		 *@brief 设置为已经验证
		 */
		void SetVerified();

		/**
		 *@brief 判断是否为已验证
		 */
		bool IsVerified() const{ return m_Status == NCS_NORMAL; }

		/**
		 *@brief 获取状态
		 */
		NetConnectionStatus GetStatus() const{ return m_Status; }

		/**
		 *@brief 从队列获取消息，提供一个连接一个队列的方式
		 */
		virtual Avalon::Packet* PopPacket(){ return NULL; }

		/**
		 *@brief 获取本地和对端ip
		 */
		const Avalon::SockAddr& GetSockAddr() const{ return m_SockAddr; }
		const Avalon::SockAddr& GetPeerAddr() const{ return m_PeerAddr; }

	private://继承自INetEventHandler
		/**
		 *@brief 获取套接字句柄
		 */
		sock_t GetHandle() const{ return m_Socket.GetHandle(); }

		/**
		 *@brief 处理输入事件
		 */
		void HandleInput();

		/**
		 *@brief 处理输出事件
		 */
		void HandleOutput();

		/**
		 *@brief 处理异常
		 */
		void HandleException();

		/**
		 *@brief 从反应堆移除时回调
		 */
		void HandleClose();

		/**
		 *@brief 定时调用
		 */
		void HandleTick(const Time& now);

	protected://新增接口
		/**
		 *@brief 获取网络服务
		 */
		NetService* GetNetService() const{ return m_pNetService; }

		/**
		 *@brief 处理接受到的网络包，在此可以解密、解压缩
		 */
		virtual void ProcessRecvPacket(Packet*& packet){ }
		
		/**
		 *@brief 处理要发送的网络包，在此可以加密、压缩
		 */
		virtual void ProcessSendPacket(Packet*& packet){ }

		/**
		 *@brief 预处理消息包
		 *@return 已经处理返回true，否则返回false，消息包将返回到上层
		 */
		virtual bool PreHandlePacket(Packet* packet);

		/**
		 *@brief 有数据包到达，在此可以循环调用RecvPacket接收数据包
		 */
		virtual void OnDataReached() = 0;

		/**
		 *@brief tick事件
		 */
		virtual void OnTick(const Time& now){}

		/**
		 *@brief 一个连接建立(已验证)
		 */
		virtual void OnAsynConnected(){}

		/**
		 *@brief 一个连接断开
		 */
		virtual void OnAsynDisconnected(){}

	private:
		//所属网络服务
		NetService* m_pNetService;
		//连接打开的时间
		UInt32	m_OpenTime;
		//上次接受到心跳包的到现在的心跳数
		UInt32  m_TickNum;

		//互斥锁
		Avalon::Mutex m_Mutex;

		//套接字
		Socket m_Socket;
		//连接类型
		NetConnectionType m_Type;
		//连接状态
		volatile NetConnectionStatus m_Status;
		//关闭等待时间
		UInt32	m_CloseWaitTime;
		//对端节点id
		UInt32	m_NodeID;
		//本地地址
		SockAddr m_SockAddr;
		//对端地址
		SockAddr m_PeerAddr;
		//当前正在接受的消息包  也就是组包过程中临时保存正在接收的包
		Packet* m_pRecvPacket;
		//接收时包头buf
		UInt8 m_HeaderBuf[Packet::HEAD_SIZE];
		//接受数据大写
		UInt16 m_RecvSize;
	};

	/**
	 *@brief 连接智能指针
	 */
	 typedef SmartPtr<NetConnection> NetConnectionPtr;
}

#endif
