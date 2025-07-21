/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	����Э�����
 */
#ifndef _AVALON_PROTOCOL_H_
#define _AVALON_PROTOCOL_H_

#include "AvalonNetStream.h"

namespace Avalon
{
	class Packet;

#define AVALON_DEFINE_PROTOCOLID(Id) \
	public:\
	const static Avalon::UInt32 __ID = Id; \
	Avalon::UInt32 GetProtocolID() const {\
		return __ID; \
	}\
	private:

	class NetService;

	/**
	 *@brief Э��
	 */
	class Protocol : public INetSerializable
	{
	public:
		Protocol();
		virtual ~Protocol();

		/**
		 *@brief ��ȡЭ��id�ӿ�
		 */
		virtual UInt32 GetProtocolID() const = 0;

		/**
		 *@brief ��ȡ���յ�Э�������id
		 */
		UInt32 GetConnID() const{ return m_ConnID; }

		/**
		*@brief Sequence
		*/
		UInt32 GetSequence() const{ return m_Sequence; }
		void SetSequence(UInt32 sequence) { m_Sequence = sequence; }

        //�յ������������
        NetService* GetNetService() const { return m_pService; }

		/**
		 *@brief ����  ����������ú�����ʵ�ʵ����˾���Э���Output������ ����Э�������Ҫʵ��Output����
		 */
		bool Encode(Packet* packet);

		/**
		 *@brief ���� ����������ú�����ʵ�ʵ����˾���Э���Input����������Э�������Ҫʵ��Input����
		 */
		bool Decode(Packet* packet);

		/**
		 *@brief �ط�Э��
		 */
		virtual bool SendBack(Protocol& protocol);
	private:
		//����id
		Avalon::UInt32 m_ConnID;
		//Sequence
		Avalon::UInt32 m_Sequence;
		//�յ������������
		NetService* m_pService;
	};
}

#endif
