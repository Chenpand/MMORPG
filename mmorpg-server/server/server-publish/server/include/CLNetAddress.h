#ifndef _CL_NETADDRESS_H_
#define _CL_NETADDRESS_H_

#include <AvalonSocket.h>
#include <AvalonNetStream.h>
#include <map>
#include "CLDefine.h"

typedef std::map<UInt32,Avalon::SockAddr> SockAddrMap;
typedef std::vector<Avalon::SockAddr> SockAddrVec;

/**
 *@brief �����ַ
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
	 *@brief �������е�ַ
	 */
	bool Load(const std::string& config);

	/**
	 *@brief ���ݽڵ�id��ȡ������ַ
	 */
	bool FindInternalAddress(UInt32 nodeid,Avalon::SockAddr& addr);

	/**
	 *@brief ���ݽڵ�id��ȡ������ַ
	 */
	bool FindExternalAddress(UInt32 nodeid,Avalon::SockAddr& addr);

	/**
	 *@brief ���ݽڵ�id��ȡ����̨��ַ
	 */
	bool FindConsoleAddress(UInt32 nodeid, Avalon::SockAddr& addr);

	/**
	 *@brief ���ݽڵ�id��ȡ֧����ַ
	 */
	bool FindBillingAddress(UInt32 nodeid, Avalon::SockAddr& addr);

	/**
	 *@brief ���һ��������ַ
	 */
	bool AddInternalAddress(UInt32 nodeid,const Avalon::SockAddr& addr);

	/**
	 *@brief ���һ��������ַ
	 */
	bool AddExternalAddress(UInt32 nodeid,const Avalon::SockAddr& addr);

	/**
	 *@brief ���һ������̨��ַ
	 */
	bool AddConsoleAddress(UInt32 nodeid, const Avalon::SockAddr& addr);

	/**
	 *@brief ���һ��֧����ַ
	 */
	bool AddBillingAddress(UInt32 nodeid, const Avalon::SockAddr& addr);

	/**
	*@brief ���һ��Ŀ¼��������ַ
	*/
	void AddDirAddress(const Avalon::SockAddr& addr);
	
	/**
	 *@brief ���ݷ��������ͻ�ȡ��ַ
	 */
	bool GetInternalAddresses(UInt32 type, std::map<UInt32,Avalon::SockAddr>& addrs);

	/**
	*@brief ��ȡĿ¼��������ַ
	*/
	const SockAddrVec& GetDirServerAddresses() const { return m_DirServerAddr; }

private:
	//�����ַ�б�
	SockAddrMap m_InternalAddrs;
	//������ַ�б�
	SockAddrMap m_ExternalAddrs;
	//����̨��ַ�б�
	SockAddrMap	m_ConsoleAddrs;
	//֧����ַ�б�
	SockAddrMap	m_BillingAddrs;
	// Ŀ¼�������б�
	SockAddrVec m_DirServerAddr;
};

#endif
