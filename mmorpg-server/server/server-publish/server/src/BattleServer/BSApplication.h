#ifndef _BS_APPLICATION_H_
#define _BS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief 战场服务器应用程序类
 */
class BSApplication : public CLApplication, public Avalon::Singleton<BSApplication>
{
public:
	BSApplication();
	~BSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }

	UInt32 GetRobotFlag(void) { return m_hasRobot; }

	UInt32 GetNoWarCD() { return m_NoWarCD; }

	/**
	*@brief 战斗标记
	*/
	UInt32 GetBattleFlag() const { return m_BattleFlag; }

private:
	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	bool ReloadDataTables();

private:
	//重新加载配置
	bool	m_ReloadConfig;
	
	//tick时间
	UInt64	m_TickTime;
	//数据库连接
	Avalon::RetryDBConnection* m_DBConn;

	//允许机器人
	UInt32 m_hasRobot;

	// 免战CD
	UInt32 m_NoWarCD;

	// 战斗标记
	UInt32			m_BattleFlag;
};

#endif
