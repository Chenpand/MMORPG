#ifndef _CL_EXTERNAL_NETWORK_H_
#define _CL_EXTERNAL_NETWORK_H_

#include "CLNetwork.h"

class CLNetAddress;

/**
 *@brief 外网连接
 */
class CLExternalConnection : public CLNetConnection
{
public:
	//每分钟最大流量
	static UInt32 MAX_BYTES_PER_MIN;
	//每分钟最大包数
	static UInt32 MAX_PACKETS_PER_MIN;
	//TGW头部长度
	const static UInt32	TGW_HEADER_LENGTH = 80;
	//TGW头部伪消息号
	const static UInt32	TGW_PACKET_ID = UInt32(-1);
	//hash口令有效时间
	const static UInt32	HASH_VALID_TIME = 5 * 60;
	//id压缩标识位
	const static UInt32	PACKET_COMPRESS_FLAG = UInt32(1) << 31;
	//最小压缩包大小
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
	 *@brief 预处理消息包 进行消息验证
	 */
	bool PreHandlePacket(Avalon::Packet* packet);

	/**
	 *@brief 在此处理加密
	 */
	void ProcessSendPacket(Avalon::Packet *&packet);

	/**
	 *@brief 在此处理解密
	 */
	void ProcessRecvPacket(Avalon::Packet *&packet);

private:
	//计数时间
	Avalon::Time m_CountTime;
	//一分钟内收到的字节数
	UInt32	m_BytesRecved;
	//一分钟内收到的包数
	UInt32	m_PacketsRecved;
};



/**
 *@brief 外网
 */
class CLExternalNetwork : public CLNetwork
{
public:
	CLExternalNetwork();
	~CLExternalNetwork();

public:
	/**
	 *@brief 启动
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses = NULL);

	/**
	 *@brief 关闭
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief 获取类型
	 */
	NetworkType GetType() const{ return NT_EXTERVAL; }

	/**
	 *@brief 预处理消息包
	 */
	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

public:
	Avalon::NetConnection* CreateConnection();
};

#endif
