#ifndef _CL_NETWORK_PERFORMACE_H_
#define _CL_NETWORK_PERFORMACE_H_

#include <AvalonDefine.h>
#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <AvalonPropertySet.h>
#include <AvalonMutex.h>

#include "CLDefine.h"

/**
 *@brief ������������ͳ�������Ϣ
 */
struct CLConnectionPerfInfo
{
	CLConnectionPerfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// �յ�����Ϣ����
	UInt32		recvPacketNum;
	// �յ�����Ϣ��С
	UInt32		recvPacketSize;
	// ���͵���Ϣ����
	UInt32		sendPacketNum;
	// ���͵���Ϣ��С
	UInt32		sendPacketSize;
};

/**
 *@brief ��Ϣ��ͳ�������Ϣ
 */
struct CLPacketPerfInfo
{
	CLPacketPerfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// �յ�����Ϣ������
	UInt32		recvPacketNum;
	// �յ�����Ϣ����С
	UInt32		recvPacketSize;
	// ���͵���Ϣ����
	UInt32		sendPacketNum;
	// ���͵���Ϣ��С
	UInt32		sendPacketSize;
	// ������Ϣ������
	UInt32		procNum;
	// ������Ϣ����ʱ��
	UInt64		totalProcUsedTime;
	// ������Ϣ�����ʱ��
	UInt64		maxProcUsedTime;
};

class CLNetworkPerfMgr : public Avalon::Singleton<CLNetworkPerfMgr>
{
public:
	CLNetworkPerfMgr();
	~CLNetworkPerfMgr();

	/**
	 *@brief ��ʼ��
	 */
	void Init(Avalon::PropertySet* propertys);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

public:
	// �¼�
	/**
	 *@brief ������Ϣ
	 */
	void OnSendPacket(UInt32 connId, UInt32 protocolid, UInt32 size);

	/**
	 *@brief �յ���Ϣ
	 */
	void OnRecvPacket(UInt32 connId, UInt32 protocolid, UInt32 size);

	/**
	 *@brief �յ���Ϣ
	 */
	void OnProcessPacket(UInt32 connId, UInt32 protocolid, UInt64 usedTime);

public:
	/**
	 *@brief ���������������ͳ��
	 */
	inline void OpenPerfConnection(bool open);
	inline bool IsOpenPerfConnection() const;

	/**
	 *@brief ������������ͳ�ƿ���
	 */
	inline void OpenPerfPacket(bool open);
	inline bool IsOpenPerfPacket() const;

	/**
	*@brief ��ȡ��ǰʱ��(us)
	*/
	UInt64 GetUSecsNow();

	/**
	*@brief ��������ͳ��ʱ����
	*/
	void SetPerfIntervalMs(UInt32 ms);

private:
	Avalon::Mutex								m_Mutex;
	// ����ͳ��ʱ����(ms)
	UInt32										m_PerfIntervalMs;
	// �ϴ�����ͳ��ʱ��(ms)
	UInt64										m_LastShowPerfTime;
	// �Ƿ��������������ͳ��
	bool										m_OpenConnectionPerf;
	// ����ͳ�������Ϣ(������Ϊ��λ)
	HashMap<UInt32, CLConnectionPerfInfo>		m_ConnectionPerfInfoes;
	// ��Ϣ���������ͳ��
	bool										m_OpenPacketPerf;
	// ����ͳ�������Ϣ(����Ϣ��Ϊ��λ)
	HashMap<UInt32, CLPacketPerfInfo>			m_PacketPerfInfoes;
	// �Ƿ��ӡ��Ϣ��
	bool										m_ShowProtocolName;
};

#endif