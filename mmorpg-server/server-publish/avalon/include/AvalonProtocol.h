/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	网络协议基类
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
	 *@brief 协议
	 */
	class Protocol : public INetSerializable
	{
	public:
		Protocol();
		virtual ~Protocol();

		/**
		 *@brief 获取协议id接口
		 */
		virtual UInt32 GetProtocolID() const = 0;

		/**
		 *@brief 获取接收到协议的连接id
		 */
		UInt32 GetConnID() const{ return m_ConnID; }

		/**
		*@brief Sequence
		*/
		UInt32 GetSequence() const{ return m_Sequence; }
		void SetSequence(UInt32 sequence) { m_Sequence = sequence; }

        //收到包的网络服务
        NetService* GetNetService() const { return m_pService; }

		/**
		 *@brief 编码  打包函数，该函数内实际调用了具体协议的Output函数， 具体协议里就是要实现Output函数
		 */
		bool Encode(Packet* packet);

		/**
		 *@brief 解码 解包函数，该函数内实际调用了具体协议的Input函数，具体协议里就是要实现Input函数
		 */
		bool Decode(Packet* packet);

		/**
		 *@brief 回发协议
		 */
		virtual bool SendBack(Protocol& protocol);
	private:
		//连接id
		Avalon::UInt32 m_ConnID;
		//Sequence
		Avalon::UInt32 m_Sequence;
		//收到包的网络服务
		NetService* m_pService;
	};
}

#endif
