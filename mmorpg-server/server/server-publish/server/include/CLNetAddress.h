#ifndef _CL_NETADDRESS_H_
#define _CL_NETADDRESS_H_

#include <AvalonSocket.h>
#include <AvalonNetStream.h>
#include <map>
#include "CLDefine.h"

typedef std::map<UInt32,Avalon::SockAddr> SockAddrMap;
typedef std::vector<Avalon::SockAddr> SockAddrVec;

/**
 *@brief 网络地址
 */
class CLNetAddress
{
public:
	CLNetAddress();
	~CLNetAddress();

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_InternalAddrs & m_ExternalAddrs;
	}

	/**
	 *@brief 加载所有地址
	 */
	bool Load(const std::string& config);

	/**
	 *@brief 根据节点id获取内网地址
	 */
	bool FindInternalAddress(UInt32 nodeid,Avalon::SockAddr& addr);

	/**
	 *@brief 根据节点id获取外网地址
	 */
	bool FindExternalAddress(UInt32 nodeid,Avalon::SockAddr& addr);

	/**
	 *@brief 根据节点id获取控制台地址
	 */
	bool FindConsoleAddress(UInt32 nodeid, Avalon::SockAddr& addr);

	/**
	 *@brief 根据节点id获取支付地址
	 */
	bool FindBillingAddress(UInt32 nodeid, Avalon::SockAddr& addr);

	/**
	 *@brief 添加一个内网地址
	 */
	bool AddInternalAddress(UInt32 nodeid,const Avalon::SockAddr& addr);

	/**
	 *@brief 添加一个外网地址
	 */
	bool AddExternalAddress(UInt32 nodeid,const Avalon::SockAddr& addr);

	/**
	 *@brief 添加一个控制台地址
	 */
	bool AddConsoleAddress(UInt32 nodeid, const Avalon::SockAddr& addr);

	/**
	 *@brief 添加一个支付地址
	 */
	bool AddBillingAddress(UInt32 nodeid, const Avalon::SockAddr& addr);

	/**
	*@brief 添加一个目录服务器地址
	*/
	void AddDirAddress(const Avalon::SockAddr& addr);
	
	/**
	 *@brief 根据服务器类型获取地址
	 */
	bool GetInternalAddresses(UInt32 type, std::map<UInt32,Avalon::SockAddr>& addrs);

	/**
	*@brief 获取目录服务器地址
	*/
	const SockAddrVec& GetDirServerAddresses() const { return m_DirServerAddr; }

private:
	//网络地址列表
	SockAddrMap m_InternalAddrs;
	//外网地址列表
	SockAddrMap m_ExternalAddrs;
	//控制台地址列表
	SockAddrMap	m_ConsoleAddrs;
	//支付地址列表
	SockAddrMap	m_BillingAddrs;
	// 目录服务器列表
	SockAddrVec m_DirServerAddr;
};

#endif
