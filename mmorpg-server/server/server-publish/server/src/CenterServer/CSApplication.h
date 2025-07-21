#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>
#include <CLOperateDefine.h>

/**
 *@brief 中心服务器应用程序类
 */
class CSApplication : public CLApplication, public Avalon::Singleton<CSApplication>
{
public:
	CSApplication();
	~CSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief 信号处理
	*/
	virtual void OnSignal(int signal);

public:
	const std::string& GetPushUrl() const { return m_PushUrl; }
	const std::string & GetPushKey() const { return m_PushKey; }
	bool CanPush() { return m_CanPush; }
	const std::string& GetCustomServiceAgentAddr() const { return m_CustomServerAgentAddr; }
	const std::string& GetRoleListServerAddr() const { return m_RoleListServerAddr; }
	const std::string& GetCheatReportAddr() const { return m_CheatReportAddr; }
	const std::string& GetPlayerInfoQueryUrl() const { return m_PlayerVipInfoUrl; }
	const std::string& GetPlayerAntiAddictInfoUrl() const { return m_PlayerAntiAddictInfoUrl; }
	const std::string& GetActivityYearSortListUrl() const{ return m_ActivityYearSortListUrl; }
	const std::string& GetAccountConvertQueryUrl() const { return m_AccountConvertQueryUrl; }
	const std::string& GetCheatQueryUrl() const { return m_CheatQueryUrl; }
	const std::string& GetGmOpActivityLibUrl() const { return m_GmOpActivityLibUrl; }
	const std::string& GetGmOpActivityKey() const { return m_GmOpActivityKey; }
	const std::string& GetRedPacketRankUrl() const { return m_RedPacketRankUrl; }
	const std::string& GetCheckRechargePushUrl() const { return m_RechargePushCheckUrl; }
	const std::string& GetAccLoginTimeUrl() const { return m_AccLoginTimeUrl; }

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }
	inline const OperateInfo& GetOperateInfo() { return m_operateInfo; }

private:
	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

private:
	
	//tick时间
	UInt64	m_TickTime;
	//数据库连接
	Avalon::RetryDBConnection* m_DBConn;
	//运营配置信息
	OperateInfo m_operateInfo;
	//推送消息签名key
	std::string		m_PushKey;
	//推送消息目标url
	std::string		m_PushUrl;
	//重新加载配置
	bool			m_ReloadConfig;
	//是否可以推送
	bool			m_CanPush;
	//客服系统签名服务器
	std::string		m_CustomServerAgentAddr;
	//保存角色信息服务器
	std::string		m_RoleListServerAddr;
	//作弊记录收集服务器
	std::string		m_CheatReportAddr;
	//用户vip等级
	std::string		m_PlayerVipInfoUrl;
	//用户防沉迷信息
	std::string		m_PlayerAntiAddictInfoUrl;
	//活动排行榜URL
	std::string		m_ActivityYearSortListUrl;
	//账号转换查询服务器
	std::string		m_AccountConvertQueryUrl;
	//惩罚信息url
	std::string		m_CheatQueryUrl;
	//gm运营活动库地址
	std::string		m_GmOpActivityLibUrl;
	//gm运营活动key
	std::string		m_GmOpActivityKey;
	//红包排行榜url
	std::string		m_RedPacketRankUrl;
	//充值推送检测url
	std::string		m_RechargePushCheckUrl;
	//上报账号登录时间url
	std::string		m_AccLoginTimeUrl;
};

#endif
