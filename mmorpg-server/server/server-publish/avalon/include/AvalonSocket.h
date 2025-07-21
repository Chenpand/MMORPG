/**
 *@author	 
 *@email	 
 *@date		2011-03-06	
 *@brief	�׽��ֶ���
 */
#ifndef _AVALON_SOCKET_H_
#define _AVALON_SOCKET_H_

#include "AvalonNetworkAPI.h"
#include "AvalonNetStream.h"
#include <string>

namespace Avalon
{
	/**
	 *@brief �׽��ֵ�ַ
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
		 *@brief ���ַ���ת��Ϊ��ַ
		 */
		bool FromString(const std::string& str);
		std::string ToString() const;

		/**
		 *@brief ���õ�ַ
		 */
		void SetAddr(const sockaddr_t& addr);

		/**
		 *@brief ת��Ϊsockaddr�ṹ
		 */
		void GetAddr(sockaddr_t& addr) const;

		/**
		 *@brief ��ȡip
		 */
		const std::string& GetIP() const{ return m_IP; }

		/**
		 *@brief ����ip
		 */
		void SetIP(const std::string& ip){ m_IP = ip; }

		/**
		 *@brief ��ȡ�˿�
		 */
		UInt16 GetPort() const{ return m_Port; }

		/**
		 *@brief ���ö˿�
		 */
		void SetPort(UInt16 port){ m_Port = port; }

	private:
		//ip
		std::string m_IP;
		//�˿�
		UInt16	m_Port;
	};

	/**
	 *@brief �׽��ֶ���
	 */
	class Socket
	{
	public:
		Socket():m_Sock(AVALON_INVALID_SOCK){}
		explicit Socket(sock_t sock):m_Sock(sock){}
		~Socket(){}

		/**
		 *@brief �����׽���
		 *@param type ���ͣ�SOCK_STREAM��SOCK_DGRAM
		 */
		bool Create(int type);

		/**
		 *@brief �󶨵�һ����ַ
		 */
		bool Bind(const SockAddr& addr);

		/**
		 *@brief ���ӵ�һ����ַ
		 */
		bool Connect(const SockAddr& addr);

		/**
		 *@brief �趨���ܾ���б��С
		 */
		bool Listen(int backlog);

		/**
		 *@brief ����socket���
		 */
		sock_t Accept();

		/**
		 *@brief ��������
		 */
		int Send(const void* buf, Int32 len, int flags = 0);

		/**
		 *@brief ��������
		 */
		int Recv(void* buf, Int32 len, int flags = 0);

		/**
		 *@brief udp��������
		 */
		int SendTo(const void* buf,size_t len,const SockAddr& addr,int flags = 0);

		/**
		 *@brief udp��������
		 */
		int RecvFrom(void* buf,size_t len,SockAddr& addr,int flags = 0);

		/**
		 *@brief �ر��׽���
		 */
		void Close();
		void Shutdown();

		/**
		 *@brief �����Ϊ��Ч
		 */
//		void Invalidate();

		/**
		 *@brief ��ȡ�׽��־��
		 */
		sock_t GetHandle() const;

		/**
		 *@brief ��ȡ���ε�ַ
		 */
		bool GetSockAddr(SockAddr& addr);

		/**
		 *@brief ��ȡ�Զ˵�ַ
		 */
		bool GetPeerAddr(SockAddr& addr);

		/**
		 *@brief ���û�ȡ�׽���ѡ��
		 */
		bool SetOpt(int level,int opt,const void* optval,socklen_t optlen);
		bool GetOpt(int level,int opt,void *optval,socklen_t* optlen);

		/**
		 *@brief ����Ϊ������ģʽ
		 */
		bool SetNonblock();

		/**
		 *@brief ����Ϊ��ַ����
		 */
		bool SetReuseAddr();

		/**
		 *@brief ��ȡ����
		 */
		static int GetError();
	private:
		//ϵͳ�׽���
		sock_t m_Sock;
	};
}

#endif
