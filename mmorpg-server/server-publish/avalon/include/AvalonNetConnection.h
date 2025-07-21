/**
 *@author	 
 *@email	 
 *@date		2011-03-26	
 *@brief	��������
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
	 *@brief �������ͣ����������Ӻͱ�������
	 */
	enum NetConnectionType
	{
		NCT_ACTIVE,	//��������
		NCT_PASSIVE,	//��������
	};
	
	/**
	 *@brief ����״̬
	 */
	enum NetConnectionStatus
	{
		NCS_CLOSED,			//��ʼ״̬����û����
		NCS_VERIFY,			//��֤�׶�
		NCS_NORMAL			//����ͨ��״̬
	};

	/**
	 *@brief ����������
	 */
	class NetConnection : public INetEventHandler, public IReferenceable
	{
		friend class NetService;
	public:
		NetConnection();
		virtual ~NetConnection();

	public:
		/**
		 *@brief ��һ������
		 */
		bool Open(const Avalon::Socket& socket, NetConnectionType type, NetService* service);

		/**
		 *@brief �ر����ӣ��������ֻ������һ��״̬  �� Noted by aprilliu, �ȴ�HandleTick���������йر����ӵĲ���
		 *@param waittime �ȴ�ʱ�䣬��
		 */
		void Close(UInt32 waittime = 0);

		/**
		 *@brief �����ֽ���
		 */
		Int32 Send(const void* buf, Int32 len);
		Int32 SendChunk(const void* buf, Int32 len);

		/**
		 *@brief �����ֽ���
		 */
		Int32 Recv(void* buf, Int32 len);

		/**
		 *@brief ����һ�������
		 */
		bool SendPacket(Packet* packet);

		/**
		 *@brief ����һ����Ϣ��
		 */
		Packet* RecvPacket();

		/**
		 *@brief ��ȡ������
		 */
		Int32 GetError(){ return Avalon::Socket::GetError(); }

		/**
		 *@brief ������ͷ
		 */
		virtual void EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence);
		virtual void DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence);

		/**
		 *@brief ��ȡ����
		 */
		NetConnectionType GetType() const{ return m_Type; }

		/**
		*@brief ���Ӵ򿪵�ʱ��
		*/
		UInt32 GetOpenTime() const { return m_OpenTime; }

		/**
		 *@brief ����id����֤ͨ������Խ��д˲���
		 */
		void SetID(UInt32 id) { m_NodeID = id; }

		/**
		 *@brief ��ȡid
		 */
		UInt32 GetID() const { return m_NodeID; }

		/**
		 *@brief ����Ϊ�Ѿ���֤
		 */
		void SetVerified();

		/**
		 *@brief �ж��Ƿ�Ϊ����֤
		 */
		bool IsVerified() const{ return m_Status == NCS_NORMAL; }

		/**
		 *@brief ��ȡ״̬
		 */
		NetConnectionStatus GetStatus() const{ return m_Status; }

		/**
		 *@brief �Ӷ��л�ȡ��Ϣ���ṩһ������һ�����еķ�ʽ
		 */
		virtual Avalon::Packet* PopPacket(){ return NULL; }

		/**
		 *@brief ��ȡ���غͶԶ�ip
		 */
		const Avalon::SockAddr& GetSockAddr() const{ return m_SockAddr; }
		const Avalon::SockAddr& GetPeerAddr() const{ return m_PeerAddr; }

	private://�̳���INetEventHandler
		/**
		 *@brief ��ȡ�׽��־��
		 */
		sock_t GetHandle() const{ return m_Socket.GetHandle(); }

		/**
		 *@brief ���������¼�
		 */
		void HandleInput();

		/**
		 *@brief ��������¼�
		 */
		void HandleOutput();

		/**
		 *@brief �����쳣
		 */
		void HandleException();

		/**
		 *@brief �ӷ�Ӧ���Ƴ�ʱ�ص�
		 */
		void HandleClose();

		/**
		 *@brief ��ʱ����
		 */
		void HandleTick(const Time& now);

	protected://�����ӿ�
		/**
		 *@brief ��ȡ�������
		 */
		NetService* GetNetService() const{ return m_pNetService; }

		/**
		 *@brief ������ܵ�����������ڴ˿��Խ��ܡ���ѹ��
		 */
		virtual void ProcessRecvPacket(Packet*& packet){ }
		
		/**
		 *@brief ����Ҫ���͵���������ڴ˿��Լ��ܡ�ѹ��
		 */
		virtual void ProcessSendPacket(Packet*& packet){ }

		/**
		 *@brief Ԥ������Ϣ��
		 *@return �Ѿ�������true�����򷵻�false����Ϣ�������ص��ϲ�
		 */
		virtual bool PreHandlePacket(Packet* packet);

		/**
		 *@brief �����ݰ�����ڴ˿���ѭ������RecvPacket�������ݰ�
		 */
		virtual void OnDataReached() = 0;

		/**
		 *@brief tick�¼�
		 */
		virtual void OnTick(const Time& now){}

		/**
		 *@brief һ�����ӽ���(����֤)
		 */
		virtual void OnAsynConnected(){}

		/**
		 *@brief һ�����ӶϿ�
		 */
		virtual void OnAsynDisconnected(){}

	private:
		//�����������
		NetService* m_pNetService;
		//���Ӵ򿪵�ʱ��
		UInt32	m_OpenTime;
		//�ϴν��ܵ��������ĵ����ڵ�������
		UInt32  m_TickNum;

		//������
		Avalon::Mutex m_Mutex;

		//�׽���
		Socket m_Socket;
		//��������
		NetConnectionType m_Type;
		//����״̬
		volatile NetConnectionStatus m_Status;
		//�رյȴ�ʱ��
		UInt32	m_CloseWaitTime;
		//�Զ˽ڵ�id
		UInt32	m_NodeID;
		//���ص�ַ
		SockAddr m_SockAddr;
		//�Զ˵�ַ
		SockAddr m_PeerAddr;
		//��ǰ���ڽ��ܵ���Ϣ��  Ҳ���������������ʱ�������ڽ��յİ�
		Packet* m_pRecvPacket;
		//����ʱ��ͷbuf
		UInt8 m_HeaderBuf[Packet::HEAD_SIZE];
		//�������ݴ�д
		UInt16 m_RecvSize;
	};

	/**
	 *@brief ��������ָ��
	 */
	 typedef SmartPtr<NetConnection> NetConnectionPtr;
}

#endif
