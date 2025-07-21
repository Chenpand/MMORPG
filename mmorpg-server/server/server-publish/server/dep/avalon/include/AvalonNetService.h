/**
 *@author	 
 *@email	 
 *@date		2011-03-27	
 *@brief	网络服务
 */
#ifndef _AVALON_NETSERVICE_H_
#define _AVALON_NETSERVICE_H_

#include "AvalonNetConnection.h"

namespace Avalon
{
	/**
	 *@brief 网络属性
	 */
	struct NetProperties
	{
		NetProperties()
		{
			num_of_iothread = 1;
			maxnum_of_connection = 10000;
			size_of_socket_buffer = 65600;
			send_wait = false;
			verify_timeout = 30;
			heartbeat_timeout = 60;
			checkBodySize = false;
			tcp_nodelay = 1;
		}

		UInt32 num_of_iothread;				//负责io的线程数
		UInt32 maxnum_of_connection;		//最大连接数
		UInt32 size_of_socket_buffer;		//套接字缓冲大小
		bool   send_wait;					//是否发送等待
		UInt32 verify_timeout;				//验证超时
		UInt32 heartbeat_timeout;			//心跳超时
		bool   checkBodySize;				//检测包体大小
		UInt32 tcp_nodelay;					//是否禁用Nagle算法
	};

	class NetThread;
	class NetThreadGroup;

	/**
	 *@brief 连接访问器
	 */
	class NetConnectionVisitor
	{
	public:
		virtual ~NetConnectionVisitor(){}

		virtual bool operator()(NetConnection* conn) = 0;
	};

	/**
	 *@brief 网络服务类   NetService是由一个监听线程和若干个IO线程（可配置）构成的
	 */
	class NetService
	{	
		friend class Acceptor;
		friend class NetConnection;

		typedef HashMap<UInt32,NetConnection*> ConnectionMap;   //noted by aprilliu,对于不同的服务器，不同的外网，内网， key是不一样的，比如：Gate Server的外网netservice中 key就是用户accid， 
																//而对于内网 一般是node id (比如100服的scene server 1 进程)

	public:
		NetService();
		virtual ~NetService();

	public:
		/**
		 *@brief 启动网络服务
		 */
		virtual bool Startup(NetProperties& properties);

		/**
		 *@brief 关闭网络服务
		 */
		virtual void Shutdown(UInt32 inputTimeout = 100);

		/**
		 *@brief 绑定到一个地址
		 */
		bool Bind(const SockAddr& addr);

		/**
		 *@brief 连接到一个地址
		 */
		NetConnectionPtr Connect(const SockAddr& addr);

		/**
		 *@brief 连接到一个地址
		 */
		NetConnectionPtr Connect(UInt32 id,const SockAddr& addr);

		/**
		 *@brief 获取一个连接
		 */
		NetConnectionPtr FindConnection(UInt32 id);

		/**
		 *@brief 访问所有连接
		 */
		void VisitConnections(NetConnectionVisitor& visitor);

		/**
		 *@brief 添加一个验证通过的连接
		 */
		bool AddVerifiedConnection(UInt32 id,NetConnection* conn);

		/**
		 *@brief 获取当前连接数
		 */
		UInt32 GetConnectionNum() const { return m_Connections.size(); }

		/**
		 *@brief 获取网络参数
		 */
		const NetProperties* GetNetProperties() const { return &m_Properties; }

		/**
		*@brief 是否检测包体大小
		*/
		bool IsCheckBodySize() const { return m_Properties.checkBodySize; }

	protected:

		/**
		 *@brief 创建一个网络连接
		 */
		virtual NetConnection* CreateConnection() = 0;

		/**
		 *@brief 一个连接建立(已验证)
		 */
		virtual void OnAsynConnected(UInt32 id, NetConnectionType type){}

		/**
		 *@brief 一个连接断开
		 */
		virtual void OnAsynDisconnected(UInt32 id, NetConnectionType type){}

	private:
		/**
		 *@brief 接收到连接  在Acceptor的处理函数中被调用，用来生成一个连接
		 */
		void OnConnected(Socket& addr);
		
		/**
		 *@brief 移除一个连接
		 */
		void RemoveConnection(NetConnection* conn);

		/**
		 *@brief 初始设置套接字
		 */
		bool InitSocket(Socket& socket);

		/**
		 *@brief 检查网络属性是否合理
		 */
		virtual void CheckProperties(NetProperties& properties);

	private:
		//负责监听和建立连接的线程
		NetThread* m_pInitThread;
		//负责通信的线程组
		NetThreadGroup* m_pIOThreads;
		//网络服务属性
		NetProperties m_Properties;
		//是否在运行
		bool m_bRun;

		//互斥锁   noted by aprilliu  用于互斥访问m_Connections
		Mutex m_Mutex;
		//连接map
		ConnectionMap m_Connections;
	};
}

#endif
