#ifndef _CL_NETWORK_PERFORMACE_H_
#define _CL_NETWORK_PERFORMACE_H_

#include <AvalonDefine.h>
#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <AvalonPropertySet.h>
#include <AvalonMutex.h>

#include "CLDefine.h"

/**
 *@brief 单个连接性能统计相关信息
 */
struct CLConnectionPerfInfo
{
	CLConnectionPerfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// 收到的消息数量
	UInt32		recvPacketNum;
	// 收到的消息大小
	UInt32		recvPacketSize;
	// 发送的消息数量
	UInt32		sendPacketNum;
	// 发送的消息大小
	UInt32		sendPacketSize;
};

/**
 *@brief 消息包统计相关信息
 */
struct CLPacketPerfInfo
{
	CLPacketPerfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// 收到的消息包数量
	UInt32		recvPacketNum;
	// 收到的消息包大小
	UInt32		recvPacketSize;
	// 发送的消息数量
	UInt32		sendPacketNum;
	// 发送的消息大小
	UInt32		sendPacketSize;
	// 处理消息包数量
	UInt32		procNum;
	// 处理消息包总时长
	UInt64		totalProcUsedTime;
	// 处理消息包最多时长
	UInt64		maxProcUsedTime;
};

class CLNetworkPerfMgr : public Avalon::Singleton<CLNetworkPerfMgr>
{
public:
	CLNetworkPerfMgr();
	~CLNetworkPerfMgr();

	/**
	 *@brief 初始化
	 */
	void Init(Avalon::PropertySet* propertys);

	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

public:
	// 事件
	/**
	 *@brief 发送消息
	 */
	void OnSendPacket(UInt32 connId, UInt32 protocolid, UInt32 size);

	/**
	 *@brief 收到消息
	 */
	void OnRecvPacket(UInt32 connId, UInt32 protocolid, UInt32 size);

	/**
	 *@brief 收到消息
	 */
	void OnProcessPacket(UInt32 connId, UInt32 protocolid, UInt64 usedTime);

public:
	/**
	 *@brief 设置连接相关性能统计
	 */
	inline void OpenPerfConnection(bool open);
	inline bool IsOpenPerfConnection() const;

	/**
	 *@brief 设置连接性能统计开关
	 */
	inline void OpenPerfPacket(bool open);
	inline bool IsOpenPerfPacket() const;

	/**
	*@brief 获取当前时间(us)
	*/
	UInt64 GetUSecsNow();

	/**
	*@brief 设置性能统计时间间隔
	*/
	void SetPerfIntervalMs(UInt32 ms);

private:
	Avalon::Mutex								m_Mutex;
	// 性能统计时间间隔(ms)
	UInt32										m_PerfIntervalMs;
	// 上次性能统计时间(ms)
	UInt64										m_LastShowPerfTime;
	// 是否开启连接相关性能统计
	bool										m_OpenConnectionPerf;
	// 性能统计相关信息(以连接为单位)
	HashMap<UInt32, CLConnectionPerfInfo>		m_ConnectionPerfInfoes;
	// 消息包相关性能统计
	bool										m_OpenPacketPerf;
	// 性能统计相关信息(以消息包为单位)
	HashMap<UInt32, CLPacketPerfInfo>			m_PacketPerfInfoes;
	// 是否打印消息名
	bool										m_ShowProtocolName;
};

#endif