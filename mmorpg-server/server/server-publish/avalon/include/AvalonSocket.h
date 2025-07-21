/**
 *@author	 
 *@email	 
 *@date		2011-03-06	
 *@brief	套接字定义
 */
#ifndef _AVALON_SOCKET_H_
#define _AVALON_SOCKET_H_

#include "AvalonNetworkAPI.h"
#include "AvalonNetStream.h"
#include <string>

namespace Avalon
{
	/**
	 *@brief 套接字地址
	 */
	class SockAddr
	{
	public:
		SockAddr():m_Port(0){}
		SockAddr(const std::string& ip,UInt16 port):m_IP(ip),m_Port(port){}
		~SockAddr(){}

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_IP & m_Port;
		}

		/**
		 *@brief 将字符串转化为地址
		 */
		bool FromString(const std::string& str);
		std::string ToString() const;

		/**
		 *@brief 设置地址
		 */
		void SetAddr(const sockaddr_t& addr);

		/**
		 *@brief 转化为sockaddr结构
		 */
		void GetAddr(sockaddr_t& addr) const;

		/**
		 *@brief 获取ip
		 */
		const std::string& GetIP() const{ return m_IP; }

		/**
		 *@brief 设置ip
		 */
		void SetIP(const std::string& ip){ m_IP = ip; }

		/**
		 *@brief 获取端口
		 */
		UInt16 GetPort() const{ return m_Port; }

		/**
		 *@brief 设置端口
		 */
		void SetPort(UInt16 port){ m_Port = port; }

	private:
		//ip
		std::string m_IP;
		//端口
		UInt16	m_Port;
	};

	/**
	 *@brief 套接字定义
	 */
	class Socket
	{
	public:
		Socket():m_Sock(AVALON_INVALID_SOCK){}
		explicit Socket(sock_t sock):m_Sock(sock){}
		~Socket(){}

		/**
		 *@brief 创建套接字
		 *@param type 类型，SOCK_STREAM或SOCK_DGRAM
		 */
		bool Create(int type);

		/**
		 *@brief 绑定到一个地址
		 */
		bool Bind(const SockAddr& addr);

		/**
		 *@brief 连接到一个地址
		 */
		bool Connect(const SockAddr& addr);

		/**
		 *@brief 设定接受句柄列表大小
		 */
		bool Listen(int backlog);

		/**
		 *@brief 接受socket句柄
		 */
		sock_t Accept();

		/**
		 *@brief 发送数据
		 */
		int Send(const void* buf, Int32 len, int flags = 0);

		/**
		 *@brief 接受数据
		 */
		int Recv(void* buf, Int32 len, int flags = 0);

		/**
		 *@brief udp发送数据
		 */
		int SendTo(const void* buf,size_t len,const SockAddr& addr,int flags = 0);

		/**
		 *@brief udp接收数据
		 */
		int RecvFrom(void* buf,size_t len,SockAddr& addr,int flags = 0);

		/**
		 *@brief 关闭套接字
		 */
		void Close();
		void Shutdown();

		/**
		 *@brief 句柄设为无效
		 */
//		void Invalidate();

		/**
		 *@brief 获取套接字句柄
		 */
		sock_t GetHandle() const;

		/**
		 *@brief 获取本段地址
		 */
		bool GetSockAddr(SockAddr& addr);

		/**
		 *@brief 获取对端地址
		 */
		bool GetPeerAddr(SockAddr& addr);

		/**
		 *@brief 设置获取套接字选项
		 */
		bool SetOpt(int level,int opt,const void* optval,socklen_t optlen);
		bool GetOpt(int level,int opt,void *optval,socklen_t* optlen);

		/**
		 *@brief 设置为非阻塞模式
		 */
		bool SetNonblock();

		/**
		 *@brief 设置为地址重用
		 */
		bool SetReuseAddr();

		/**
		 *@brief 获取错误
		 */
		static int GetError();
	private:
		//系统套接字
		sock_t m_Sock;
	};
}

#endif
