#ifndef _CL_TRANSFER_H_
#define _CL_TRANSFER_H_

#include <CLDefine.h>
#include <CLPacketExecutor.h>

/**
 *@brief 中转消息管理
 */
class CLTransfer
{
public:
	CLTransfer();
	virtual ~CLTransfer();

	/**
	*@brief 注册中转消息
	*/
	virtual void RegisterTransferProtocols() {}

	/**
	*@brief 注册中转消息
	*/
	void RegisterTransferProtocol(UInt32 id, CLParamPacketFunction<TransferHeader&>* func);

	/**
	*@brief 执行中转消息
	*/
	bool ExecuteTransferPacket(TransferHeader& tHeader, Avalon::Packet* packet);

private:

	// 中转服务器消息
	CLParamPacketExecutor<TransferHeader&>	m_TransferExecutor;
};

#endif // !_CL_TRANSFER_H_

