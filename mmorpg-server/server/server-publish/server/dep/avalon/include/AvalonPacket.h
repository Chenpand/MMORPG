/**
 *@author	 
 *@email	 
 *@date		2011-03-26	
 *@brief	�ֽڻ���
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
	 *@brief ���������
	 */
	class Packet
	{
	public:
		//���������󳤶�
		const static size_t MAX_BODY_SIZE = 64000;
		//�����ͷ���ȣ���ͷ����id����С����ţ�
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
		 *@brief �����ͷ���Ϣ��
		 */
		static Packet* Create();
		static Packet* Create(size_t size);
		static void Destroy(Packet*& pack);
		static UInt32 GetNodeCount();
		static UInt32 GetFreeCount();

		/**
		 *@brief ������Ϣ��
		 */
		void CopyTo(Packet* packet);

		/**
		 *@brief ���û�ȡЭ��id
		 */
		UInt32 GetID() const { return m_Id; }
		void SetID(UInt32 id) { m_Id = id; }

		/**
		 *@brief ���û�ȡ��Ϣ����С
		 */
		UInt16 GetSize() const { return m_Size; }
		void SetSize(UInt16 size) { m_Size = size; }

		/**
		*@brief ���û�ȡ��Ϣ�����
		*/
		UInt32 GetSequence() const { return m_Sequence; }
		void SetSequence(UInt32 sequence) { m_Sequence = sequence; }

		/**
		 *@brief ��ȡ��󻺳��С
		 */
		UInt16 GetMaxSize() const { return m_MaxSize; }
		/**
		 *@brief ��ȡ�����׵�ַ  noted by aprilliu, ����������ʼλ��
		 */
		UInt8* GetBuffer() const { return (UInt8*)this + sizeof(Packet) + HEAD_SIZE; }

		/**
		 *@brief ���û�ȡ���ӵ�ַ
		 */
		UInt32 GetConnID() const { return m_Connid; }
		void SetConnID(UInt32 id) { m_Connid = id; } 

		/**
		 *@brief ���û�ȡ�������
		 */
		NetService* GetNetService() const { return m_pService; }
		void SetNetService(NetService* service) { m_pService = service; }

		/**
		*@brief ��ȡħ������
		*/
		UInt32 GetMagicCode() const { return m_MagicCode; }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			//modified by aprilliu, 2016.2.29 �����˳���������Ϊ2Byte size + 4Byte protocol ID
			//modified by zjy, 2016.08.09 ������������Ϣ��ţ�����Ϊ 2Byte size + 4Byte protocol ID + 4Byte sequence
			stream & m_Size & m_Id & m_Sequence;
			//stream & m_Id & m_Size;
			stream.SeriaArray(GetBuffer(), GetSize());
		}

		//adde by aprilliu, ����ͷ����2+4+4���ֽڽ��б���룩
		bool	EncodeHeader();
		bool	DecodeHeader(UInt8* header, size_t size);

		//�õ������İ�buff(����ͷ�������buff)
		UInt8*	GetPacketBuff();
		//�õ������İ����ȣ�����ͷ������ + body���� m_Size��
		UInt16  GetPacketSize();

	public:
		// ħ������
		UInt32 m_MagicCode;
		//��id
		UInt32 m_Id;
		//��С noted by aprilliu ������(������Body�ĳ��ȣ�������Head�ĳ��ȣ�2+4+4��)
		UInt16 m_Size;
		//���
		UInt32 m_Sequence;
		//����С
		UInt16 m_MaxSize;
		//�յ���������id  ��NetConnection::PreHandlePacket�б�����
		UInt32 m_Connid;
		//�յ����ӵ����������  ��NetConnection::PreHandlePacket�б�����
		NetService* m_pService;
		// ħ�����ֵ�����
		static UInt32 s_MagicCodeSeed;
	};

	typedef Avalon::DoubleQueue<Packet*,256> PacketQueue;
}

#endif
