#ifndef _CL_EXTERNAL_NETWORK_H_
#define _CL_EXTERNAL_NETWORK_H_

#include "CLNetwork.h"

class CLNetAddress;

/**
 *@brief ��������
 */
class CLExternalConnection : public CLNetConnection
{
public:
	//ÿ�����������
	static UInt32 MAX_BYTES_PER_MIN;
	//ÿ����������
	static UInt32 MAX_PACKETS_PER_MIN;
	//TGWͷ������
	const static UInt32	TGW_HEADER_LENGTH = 80;
	//TGWͷ��α��Ϣ��
	const static UInt32	TGW_PACKET_ID = UInt32(-1);
	//hash������Чʱ��
	const static UInt32	HASH_VALID_TIME = 5 * 60;
	//idѹ����ʶλ
	const static UInt32	PACKET_COMPRESS_FLAG = UInt32(1) << 31;
	//��Сѹ������С
	const static UInt32 MIN_COMPRESS_SIZE = 64;

public:
	explicit CLExternalConnection(CLNetwork* network);
	~CLExternalConnection();

public:
	void DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id);

	void OnDataReached();

	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief Ԥ������Ϣ�� ������Ϣ��֤
	 */
	bool PreHandlePacket(Avalon::Packet* packet);

	/**
	 *@brief �ڴ˴������
	 */
	void ProcessSendPacket(Avalon::Packet *&packet);

	/**
	 *@brief �ڴ˴������
	 */
	void ProcessRecvPacket(Avalon::Packet *&packet);

private:
	//����ʱ��
	Avalon::Time m_CountTime;
	//һ�������յ����ֽ���
	UInt32	m_BytesRecved;
	//һ�������յ��İ���
	UInt32	m_PacketsRecved;
};



/**
 *@brief ����
 */
class CLExternalNetwork : public CLNetwork
{
public:
	CLExternalNetwork();
	~CLExternalNetwork();

public:
	/**
	 *@brief ����
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses = NULL);

	/**
	 *@brief �ر�
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief ��ȡ����
	 */
	NetworkType GetType() const{ return NT_EXTERVAL; }

	/**
	 *@brief Ԥ������Ϣ��
	 */
	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

public:
	Avalon::NetConnection* CreateConnection();
};

#endif
