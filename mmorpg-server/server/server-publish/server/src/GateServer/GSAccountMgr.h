#ifndef _GS_ACCOUNT_MGR_H_
#define _GS_ACCOUNT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonMutex.h>
#include <AvalonTime.h>
#include <map>
#include <set>
#include <CLDefine.h>
#include <CLAntiAddictDefine.h>

/**
 *@brief 账号信息
 */
struct AccountInfo
{
	AccountInfo()
		:accid(0), createTime(0), gmauthority(0), srcZoneId(0), playerId(0), level(0), mapId(0), phoneBindRoleId(0)
	{
		memset(key, 0, sizeof(key));
	}

	//账号id
	UInt32	accid;
	//创建时间
	UInt32	createTime;
	//口令
	UInt8	key[20];
	//gm权限
	UInt8	gmauthority;
	//openid
	std::string	openid;
	//qq特权信息
	QQVipInfo	qqVipInfo;
	//来源1
	std::string	source1;
	//来源2
	std::string	source2;
	//平台
	std::string	pf;
	//设备id
	std::string did;
	//设备型号
	std::string model;
	//设备版本
	std::string deviceVersion;

	//防沉迷信息
	AntiAddictInfo antiAddictInfo;

	UInt32		srcZoneId;
	ObjID_t		playerId;
	std::string	name;
	UInt32		level;
	UInt32		mapId;
	//手机绑定的角色
	ObjID_t		phoneBindRoleId;
};

struct ReconnInfo
{
	ReconnInfo() : accid(0), sequence(0) {}

	UInt32 accid;
	UInt32 sequence;
};

/**
 *@brief 账号管理器
 */
class GSAccountMgr : public Avalon::Singleton<GSAccountMgr>
{
	//临时口令超时时间
	const static Avalon::UInt32 KEY_TIMEOUT = 20;

	typedef std::map<UInt32, AccountInfo> AccountMap;

public:
	GSAccountMgr();
	~GSAccountMgr();

	void Init();
	void Final();

public:
	/**
	 *@brief 添加一个账号
	 */
	void AddAccount(AccountInfo& info);

	/**
	 *@brief 验证账号
	 */
	bool Verify(UInt32 accid, const UInt8* key);

	/**
	 *@brief 获取账号
	 */
	bool GetAccount(UInt32 accid, AccountInfo& info);

	/**
	*@brief 删除账号
	*/
	void DelAccount(UInt32 accid);

public:
	/**
	*@brief 添加需要重连的玩家
	*/
	void AddReconnAccount(UInt32 accid, UInt32 sequence);

	/**
	*@brief 玩家是否需要重连
	*/
	ReconnInfo GetReconnInfo(UInt32 accid);

	/**
	*@brief 更新防沉迷信息
	*/
	void UpdateAntiAddictTime(UInt32 accid, UInt32 time);

public://事件

	void OnTick(const Avalon::Time& now);

private:
	//互斥锁  noted by aprilliu 用于互斥访问m_Accounts
	Avalon::Mutex m_Mutex;
	//账号链表
	AccountMap m_Accounts;
	//准备重连的账号
	std::map<UInt32, ReconnInfo> m_ReconnAccounts;
};

#endif
