#ifndef _CDKEY_MGR_H_
#define _CDKEY_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonProtocol.h>
#include <map>

struct memcached_st;

#pragma pack(1)

struct CDKeyData
{
	CDKeyData()
	{
		itemId = 0;
	}

	std::string	cdkey;
	UInt32	itemId;
	std::string openId;
};

#pragma pack()

class CDKeyMgr : public Avalon::Singleton<CDKeyMgr>
{
	typedef std::map<std::string, UInt32>	SessionMap;

	//�һ���ʱ
	const static UInt32	EXCHANGE_CDKEY_TIMEOUT = 10;

public:
	CDKeyMgr();
	~CDKeyMgr();

public:
	/**
	 *@brief ��ʼ����ֹ
	 */
	bool Init(Avalon::PropertySet* properties);
	void Final();

public:
	/**
	 *@brief ��ʼ�һ�                                                                     
	 */
	UInt32 BeginExchangeCDKey(const std::string& cdkey);

	/**
	 *@brief �����һ�                                                                     
	 */
	void EndExchangeCDKey(const std::string& cdkey, const std::string& openid);

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

public://��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendProtocol(UInt32 connId, Avalon::Protocol& protocol);

private:
	/**
	 *@brief ����key��ȡcdkey����
	 */
	bool GetCDKeyDataByKey(const std::string& cdkey, CDKeyData& data);

	bool SetCDKeyDataByKey(const CDKeyData& data);

private:
	//memcached״̬
	memcached_st* m_pMemc;
	//�һ��еĻỰ
	SessionMap	m_Sessions;
};

#endif
