#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>
#include <CLApplication.h>

/**
 *@brief 挑战服务器应用程序类
 */
class GASApplication : public CLApplication, public Avalon::Singleton<GASApplication>
{
public:
	GASApplication();
	~GASApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	void OnSignal(int signal);

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }

	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 等待数据库加载完成
	*/
	bool WaitDataLoadFinished();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

	void IncInitDataLoadCount(){ ++m_InitDataLoadCount; }
	void DecInitDataLoadCount(){ if (m_InitDataLoadCount > 0) --m_InitDataLoadCount; }

public:
	/**
	*@brief 赌马功能开关
	*/
	bool IsOpenBetHorse() { return m_BetHorseSwitch; }

	/**
	*@brief 赌马奖励补发机制
	*/
	bool IsOpenBetHorseResend() { return m_OpenBetHorseResend; }

	const std::string& GetGmOpActivityLibUrl() const { return m_GmOpActivityLibUrl; }
	const std::string& GetGmOpActivityKey() const { return m_GmOpActivityKey; }

	UInt32 GetServerIdInGM() const { return m_ServerIdInGM; }

private:
	//初始数据加载数
	UInt32			m_InitDataLoadCount;

	//重新加载配置
	bool	m_ReloadConfig;

	//数据库连接
	Avalon::RetryDBConnection* m_DBConn;

	// 赌马功能开关
	bool		m_BetHorseSwitch;
	// 赌马功能开关
	bool		m_OpenBetHorseResend;
	//gm运营活动库地址
	std::string		m_GmOpActivityLibUrl;
	//gm运营活动key
	std::string		m_GmOpActivityKey;
	//gm配置服务器id
	UInt32			m_ServerIdInGM;
};

#endif
