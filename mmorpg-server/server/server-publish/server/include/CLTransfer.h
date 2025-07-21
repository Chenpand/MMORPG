#ifndef _CL_TRANSFER_H_
#define _CL_TRANSFER_H_

#include <CLDefine.h>
#include <CLPacketExecutor.h>

/**
 *@brief ��ת��Ϣ����
 */
class CLTransfer
{
public:
	CLTransfer();
	virtual ~CLTransfer();

	/**
	*@brief ע����ת��Ϣ
	*/
	virtual void RegisterTransferProtocols() {}

	/**
	*@brief ע����ת��Ϣ
	*/
	void RegisterTransferProtocol(UInt32 id, CLParamPacketFunction<TransferHeader&>* func);

	/**
	*@brief ִ����ת��Ϣ
	*/
	bool ExecuteTransferPacket(TransferHeader& tHeader, Avalon::Packet* packet);

private:

	// ��ת��������Ϣ
	CLParamPacketExecutor<TransferHeader&>	m_TransferExecutor;
};

#endif // !_CL_TRANSFER_H_

