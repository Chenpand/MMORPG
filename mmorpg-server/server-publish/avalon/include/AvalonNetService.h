/**
 *@author	 
 *@email	 
 *@date		2011-03-27	
 *@brief	�������
 */
#ifndef _AVALON_NETSERVICE_H_
#define _AVALON_NETSERVICE_H_

#include "AvalonNetConnection.h"

namespace Avalon
{
	/**
	 *@brief ��������
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

		UInt32 num_of_iothread;				//����io���߳���
		UInt32 maxnum_of_connection;		//���������
		UInt32 size_of_socket_buffer;		//�׽��ֻ����С
		bool   send_wait;					//�Ƿ��͵ȴ�
		UInt32 verify_timeout;				//��֤��ʱ
		UInt32 heartbeat_timeout;			//������ʱ
		bool   checkBodySize;				//�������С
		UInt32 tcp_nodelay;					//�Ƿ����Nagle�㷨
	};

	class NetThread;
	class NetThreadGroup;

	/**
	 *@brief ���ӷ�����
	 */
	class NetConnectionVisitor
	{
	public:
		virtual ~NetConnectionVisitor(){}

		virtual bool operator()(NetConnection* conn) = 0;
	};

	/**
	 *@brief ���������   NetService����һ�������̺߳����ɸ�IO�̣߳������ã����ɵ�
	 */
	class NetService
	{	
		friend class Acceptor;
		friend class NetConnection;

		typedef HashMap<UInt32,NetConnection*> ConnectionMap;   //noted by aprilliu,���ڲ�ͬ�ķ���������ͬ�������������� key�ǲ�һ���ģ����磺Gate Server������netservice�� key�����û�accid�� 
																//���������� һ����node id (����100����scene server 1 ����)

	public:
		NetService();
		virtual ~NetService();

	public:
		/**
		 *@brief �����������
		 */
		virtual bool Startup(NetProperties& properties);

		/**
		 *@brief �ر��������
		 */
		virtual void Shutdown(UInt32 inputTimeout = 100);

		/**
		 *@brief �󶨵�һ����ַ
		 */
		bool Bind(const SockAddr& addr);

		/**
		 *@brief ���ӵ�һ����ַ
		 */
		NetConnectionPtr Connect(const SockAddr& addr);

		/**
		 *@brief ���ӵ�һ����ַ
		 */
		NetConnectionPtr Connect(UInt32 id,const SockAddr& addr);

		/**
		 *@brief ��ȡһ������
		 */
		NetConnectionPtr FindConnection(UInt32 id);

		/**
		 *@brief ������������
		 */
		void VisitConnections(NetConnectionVisitor& visitor);

		/**
		 *@brief ���һ����֤ͨ��������
		 */
		bool AddVerifiedConnection(UInt32 id,NetConnection* conn);

		/**
		 *@brief ��ȡ��ǰ������
		 */
		UInt32 GetConnectionNum() const { return m_Connections.size(); }

		/**
		 *@brief ��ȡ�������
		 */
		const NetProperties* GetNetProperties() const { return &m_Properties; }

		/**
		*@brief �Ƿ�������С
		*/
		bool IsCheckBodySize() const { return m_Properties.checkBodySize; }

	protected:

		/**
		 *@brief ����һ����������
		 */
		virtual NetConnection* CreateConnection() = 0;

		/**
		 *@brief һ�����ӽ���(����֤)
		 */
		virtual void OnAsynConnected(UInt32 id, NetConnectionType type){}

		/**
		 *@brief һ�����ӶϿ�
		 */
		virtual void OnAsynDisconnected(UInt32 id, NetConnectionType type){}

	private:
		/**
		 *@brief ���յ�����  ��Acceptor�Ĵ������б����ã���������һ������
		 */
		void OnConnected(Socket& addr);
		
		/**
		 *@brief �Ƴ�һ������
		 */
		void RemoveConnection(NetConnection* conn);

		/**
		 *@brief ��ʼ�����׽���
		 */
		bool InitSocket(Socket& socket);

		/**
		 *@brief ������������Ƿ����
		 */
		virtual void CheckProperties(NetProperties& properties);

	private:
		//��������ͽ������ӵ��߳�
		NetThread* m_pInitThread;
		//����ͨ�ŵ��߳���
		NetThreadGroup* m_pIOThreads;
		//�����������
		NetProperties m_Properties;
		//�Ƿ�������
		bool m_bRun;

		//������   noted by aprilliu  ���ڻ������m_Connections
		Mutex m_Mutex;
		//����map
		ConnectionMap m_Connections;
	};
}

#endif
