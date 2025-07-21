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

	//兑换超时
	const static UInt32	EXCHANGE_CDKEY_TIMEOUT = 10;

public:
	CDKeyMgr();
	~CDKeyMgr();

public:
	/**
	 *@brief 初始化终止
	 */
	bool Init(Avalon::PropertySet* properties);
	void Final();

public:
	/**
	 *@brief 开始兑换                                                                     
	 */
	UInt32 BeginExchangeCDKey(const std::string& cdkey);

	/**
	 *@brief 结束兑换                                                                     
	 */
	void EndExchangeCDKey(const std::string& cdkey, const std::string& openid);

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

public://消息相关
	/**
	 *@brief 发送消息
	 */
	void SendProtocol(UInt32 connId, Avalon::Protocol& protocol);

private:
	/**
	 *@brief 根据key获取cdkey数据
	 */
	bool GetCDKeyDataByKey(const std::string& cdkey, CDKeyData& data);

	bool SetCDKeyDataByKey(const CDKeyData& data);

private:
	//memcached状态
	memcached_st* m_pMemc;
	//兑换中的会话
	SessionMap	m_Sessions;
};

#endif
